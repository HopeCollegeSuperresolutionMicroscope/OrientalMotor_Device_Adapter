#include "OrientalMotorFocus.h"
#include "../../MMDevice/ModuleInterface.h"
#include "OrientalDeviceConstants.h"
#include "AlternativeUtils.h"
#include <cmath>
#include "OrientalMotorExceptions.h"


#ifdef WIN32
   #define WIN32_LEAN_AND_MEAN
   #include <windows.h>
   #define snprintf _snprintf 
#endif

#if _MSC_VER >= 1300    // for VC 7.0
  // from ATL 7.0 sources
  #ifndef _delayimp_h
  extern "C" IMAGE_DOS_HEADER __ImageBase;
  #endif
#endif

//include possible Hub classes with options

//Hub slave classes are written declared according to template

//All AdjusterKnobs that Might be Attached
const AdjusterKnob g_DM10( 25, 275,	true, true,	false,	"ThorLabs DM10" );


char * globalString;
//Controller Motor singleton class

///////////////////////////////////////////////////////////////////////////////
// Exported MMDevice API
///////////////////////////////////////////////////////////////////////////////
MODULE_API void InitializeModuleData()
{

   //Set Default number of floating decimal Places for AlternativeUtil Conversion
   CAlternativeUtils::SetFloatDecimalTag( 6 );

   RegisterDevice(g_OrientalUSBHubDeviceName, MM::HubDevice, g_OrientalUSBHubDeviceDescrip);
   
   for( int i = 0; i < g_maxPeripherals; i++ )
   {
	 std::string deviceLabel( g_OrientalUSBHubDeviceName );
	 deviceLabel += CDeviceUtils::ConvertToString(i);
	 RegisterDevice( deviceLabel.c_str(), MM::StageDevice, "Oriental Stepper Motor and Controller Attached to an Adjuster Knob");
   }
	//RegisterDevice( g_OrientalErrorLogger, MM::GenericDevice, "Error Logger for internal Reporting");
}

MODULE_API MM::Device* CreateDevice(const char* deviceName)
{

   //Add Options to Knob Resolver
   KnobOptionsResolver::RegisterKnobOption( &g_DM10 );


   //Add Options to Controller Motor Resolver
   

   if (deviceName == 0)
      return nullptr;

   if (strcmp(deviceName, g_OrientalUSBHubDeviceName) == 0)
   {
      // create nosepiece
      return new OrientalFTDIHub( 3 );
   }
   if( strstr( deviceName, g_OrientalStageDeviceName) != nullptr && strcmp(deviceName, g_OrientalStageDeviceName) > 0 )
   {
	   std::ostringstream os;
	   os << "The Focus Name Is " << deviceName;
	   AbstractControllerInterfaceFactory::LogMessage( os.str() );
	   return new OrientalMotorFocus( (char*) deviceName );
   }

   AbstractControllerInterfaceFactory::LogMessage( "Null ptr Suckage " );

   return nullptr;
}

MODULE_API void DeleteDevice(MM::Device* pDevice)
{
   delete pDevice;
}

/*******************************************************

Focus Implementation
********************************************************/

OrientalMotorFocus::OrientalMotorFocus(std::string name) :
   stepSize_um_(0.025),
   pos_um_(0.0),
   initialized_(false),
   lowerLimit_(0.0),
   upperLimit_(20000.0),
   name_(name),
   controller_(nullptr),
   adjuster_(nullptr),
   hub_(nullptr),
   busy_( false ),
   baseAngleChangeSignal_(false)
{
	AbstractControllerInterfaceFactory::LogMessage("Knob Value");
	InitializeDefaultErrorMessages();

	//Pre-Initialize Data

	std::vector< std::string > stringOptions;
	AbstractControllerInterfaceFactory::LogMessage("Knob Value");
    //Allow User to Configure Adjuster attached to Motor
    CPropertyAction* pAct = new CPropertyAction(this, &OrientalMotorFocus::OnAdjusterSelect);
	CreateProperty(g_OrientalAdapterPropertyName, "Default Knob Value", MM::String, false, pAct, true);
	stringOptions = KnobOptionsResolver::ReadAllOptionNames();
	SetAllowedValues( g_OrientalAdapterPropertyName, stringOptions );
	//Set Default to first available property
	SetProperty( g_OrientalAdapterPropertyName, stringOptions[0].c_str() );
	//AddAllowedValue( "Adjuster Knob Attached", "Custom" );
    //Allow User to Specify Controller Protocol
	AbstractControllerInterfaceFactory::LogMessage("Controller Value");
	pAct = new CPropertyAction(this, &OrientalMotorFocus::OnControllerSelect);
    CreateProperty(g_OrientalControllerPropertyName, "Default Controller Value", MM::String, false, pAct, true);
	stringOptions = AbstractControllerInterfaceFactory::ReadAllOptionNames();
	AbstractControllerInterfaceFactory::LogMessage( stringOptions[0] );
	SetAllowedValues( g_OrientalControllerPropertyName, stringOptions );
	//Set Default to first available property
	SetProperty( g_OrientalControllerPropertyName, stringOptions[0].c_str() );
		AbstractControllerInterfaceFactory::LogMessage("Address Value");
	//Allow User to Specify Address
    pAct = new CPropertyAction(this, &OrientalMotorFocus::OnAddressSelect);
    CreateProperty("RS-485 Device Address", "0", MM::Integer, false, pAct, true);

	// Description
   int ret = CreateProperty(MM::g_Keyword_Description, "This is a Description", MM::String, true);
   assert(DEVICE_OK == ret);

   // Name
   ret = CreateProperty(MM::g_Keyword_Name, "This is the Name Being Pulled", MM::String, true);
   assert(DEVICE_OK == ret);
 

   // parent ID display
   CreateHubIDProperty();

}


OrientalMotorFocus::~OrientalMotorFocus(void)
{
	Shutdown();
   
}

void OrientalMotorFocus::GetName(char* Name) const
{
   CDeviceUtils::CopyLimitedString(Name, name_.c_str());
}

int OrientalMotorFocus::Initialize()
{

   if (initialized_)
      return DEVICE_OK;

   LogMessage("In Focus Initialize");
   
   //MUST BE FIRST, for sake of Controller Object, set hub_ value
   hub_ = static_cast<OrientalFTDIHub*>(GetParentHub());
   char hubLabel[MM::MaxStrLength];
   hub_->GetLabel(hubLabel);
   assert( hub_ != nullptr);
   SetParentID(hubLabel); 
   //set controller_ hub property to desired property
   //This Also updates The MonitorThread Pointer
   //(Sad Reality of non-uniform Constructor-to-Initialize Calls)
   controller_->setSerialCommHubPtr( hub_ );

   // set property list
   // -----------------

   // Position
   // --------
   CPropertyAction* pAct = new CPropertyAction (this, &OrientalMotorFocus::OnPosition);
   int ret = CreateFloatProperty(MM::g_Keyword_Position, 0, false, pAct);
   if (ret != DEVICE_OK)
      return ret;

   // Sequenceability
   // --------
   //pAct = new CPropertyAction (this, &OrientalMotorFocus::OnSequence);
   pAct = nullptr;
   ret = CreateStringProperty("UseSequences", "No", false, pAct);
   AddAllowedValue("UseSequences", "No");
   AddAllowedValue("UseSequences", "Yes");

   if (ret != DEVICE_OK)
      return ret;

   //BaseAngle Partitions Options (Must Be Before BaseAngle Options) - Linked
   //--------
    pAct = new CPropertyAction(this, &OrientalMotorFocus::OnBaseAnglePartitionSelect);
	ret = CreateProperty(g_OrientalBaseAnglePartitionOptionsName, "No Partitions of Base Angle", MM::Float, false, pAct);
	if (ret != DEVICE_OK)
      return ret;

    //BaseAngle Options
    //-------
    pAct = new CPropertyAction(this, &OrientalMotorFocus::OnBaseAngleSelect);
	ret = CreateProperty(g_OrientalBaseAngleOptionsName, "0.0", MM::Float, false, pAct);
	if (ret != DEVICE_OK)
	  return ret;
	vector<double> angleOptions = controller_->GetBaseAngleOptions();
	vector< std::string > stringOptions;
	for( int i = 0; i < angleOptions.size(); i++ )
	{
		stringOptions.push_back( CDeviceUtils::ConvertToString( angleOptions[i] ) );
	}
	SetAllowedValues( g_OrientalBaseAngleOptionsName, stringOptions );
	SetProperty( g_OrientalBaseAngleOptionsName, stringOptions[0].c_str() ); //This should Be Doing someStuff AftwerWard Thru callback

   //Resting Energized State
   pAct = new CPropertyAction(this, &OrientalMotorFocus::OnRestingEnergyStateSelect);
   ret = CreateProperty(g_OrientalRestingEnergyStateName, "Enable", MM::String, false, pAct);
   stringOptions.clear();
   stringOptions.push_back( std::string( "Enable" ) );
   stringOptions.push_back( std::string( "Disable" ) );
   SetAllowedValues( g_OrientalRestingEnergyStateName, stringOptions );


   ret = UpdateStatus();
   if (ret != DEVICE_OK)
      return ret;

   //Test To Make Sure Connection Is Established
   //If Not, Throw an Error
   LogMessage("This thing is starting");
   if( controller_->InitializePhysicalController( ) != 0 )
   {
	   LogMessage( "Initialization Failed" );
   }

   LogMessage( "After Initialize" );
   
   initialized_ = true;

   return DEVICE_OK;
}

int OrientalMotorFocus::Shutdown()
{
	LogMessage("Here in Shutdown For Focus");
   if (initialized_)
   {
      initialized_ = false;
   }

   //Needed in Shutdown so that Pointers in Controller to Hub Are not available
   if( controller_ != nullptr )
   {
		delete controller_;
		controller_ = nullptr;
   }

   return DEVICE_OK;
}

int OrientalMotorFocus::SetPositionUm(double pos) 
{
   //Work Around For Higher Resolution floating Values
   int ret = SetProperty(MM::g_Keyword_Position, CAlternativeUtils::ConvertToString(pos) );
   return OnStagePositionChanged(pos_um_);
}

int OrientalMotorFocus::IsStageSequenceable(bool& isSequenceable) const
{
   isSequenceable = sequenceable_;
   return DEVICE_OK;
}

int OrientalMotorFocus::GetStageSequenceMaxLength(long& nrEvents) const
{
   if (!sequenceable_) {
      return DEVICE_UNSUPPORTED_COMMAND;
   }

   nrEvents = 2000;
   return DEVICE_OK;
}

int OrientalMotorFocus::StartStageSequence()
{
   if (!sequenceable_) {
      return DEVICE_UNSUPPORTED_COMMAND;
   }

   return DEVICE_OK;
}

int OrientalMotorFocus::StopStageSequence()
{
   if (!sequenceable_) {
      return DEVICE_UNSUPPORTED_COMMAND;
   }

   return DEVICE_OK;
}

int OrientalMotorFocus::ClearStageSequence()
{
   if (!sequenceable_) {
      return DEVICE_UNSUPPORTED_COMMAND;
   }

   return DEVICE_OK;
}

int OrientalMotorFocus::AddToStageSequence(double /* position */)
{
   if (!sequenceable_) {
      return DEVICE_UNSUPPORTED_COMMAND;
   }

   return DEVICE_OK;
}

int OrientalMotorFocus::SendStageSequence()
{
   if (!sequenceable_) {
      return DEVICE_UNSUPPORTED_COMMAND;
   }

   return DEVICE_OK;
}


//When This is Set, it assumes the Adjuster is at the pos_um that is currently there...
int OrientalMotorFocus::SetAdjuster( std::string key )
{
	const AdjusterKnob * adjuster;

	adjuster = KnobOptionsResolver::GetKnobOption( key );

	LogMessage("Adjuster KNobSetting");

	if( adjuster == nullptr )
	{
		return DEVICE_INVALID_PROPERTY_VALUE;
	}

	adjuster_ = adjuster;
	upperLimit_ = adjuster->abs_total_travel_um_ - adjuster->single_rot_travel_um_;
	lowerLimit_ = 0;

	return DEVICE_OK;
}

int OrientalMotorFocus::SetController( std::string key )
{

	AbstractControllerInterface * controller;

	//Set Hub for OrientalMotorFocus if it is not already set
	//If this is called from the constructor, hub_ will remain a nullptr, otherwise it will be initialized
	if( hub_ == nullptr )  
	{
		hub_ = static_cast<OrientalFTDIHub*>(GetParentHub());
	}

	controller = AbstractControllerInterfaceFactory::GetNewControllerOption( key, hub_, &OrientalFTDIHub::FTDISerialCommunicate );

	if( controller == nullptr )
	{
		return DEVICE_INVALID_PROPERTY_VALUE;
	}
	//delete previous controller_ instance
	//if the instance is of a different type
	if( controller_ != nullptr )
	{
		if( controller_->getName() != key )
		{
			delete controller_;
		}
		else
		{
			//We just requeried for the same controller
			//Delete heap object
			delete controller;
			return DEVICE_OK;
		}
	}

	//Conclude controller is a different contribution
	controller_ = controller;

	return DEVICE_OK;
}



///////////////////////////////////////////////////////////////////////////////
// Action handlers
///////////////////////////////////////////////////////////////////////////////

int OrientalMotorFocus::OnBaseAnglePartitionSelect( MM::PropertyBase* pProp, MM::ActionType eAct )
{
	std::ostringstream os;
	int ret;

	LogMessage( "In Base Angle Partition Callback" );

	if( eAct == MM::BeforeGet )
	{
		//Initialize slider
		//Note:  This is a bit of a hack to allow for a configuration that feels more usable
		if( initialized_ == false && baseAngleChangeSignal_ == true )
		{
			std::vector< double > baseOptions = controller_->GetBaseAngleOptions();
			std::vector<double> partitionOptions;
			double min = 360;  //Max itfeasible Angle
			double max = 0;
			for( int a = 0; a < baseOptions.size(); a++ )
			{
				LogMessage( "In BeforeGet of Partition! ");

				partitionOptions = controller_->GetBaseAnglePartitionOptions( baseOptions[a] );			
				if( a == 0 )
				{
					min = partitionOptions[0];
					max = partitionOptions[0];
				}

				for( int i = 0; i < partitionOptions.size(); ++i )
				{
					min = min( min, partitionOptions[i] );
					max = max( max, partitionOptions[i] );
				}

			}
			//Workaround For Poor Config Update Callbacks, Make a Slider with Maximum anticipated Values
			SetPropertyLimits( g_OrientalBaseAnglePartitionOptionsName, min, max );
			//Operate On Current Parition List
			partitionOptions = controller_->GetBaseAnglePartitionOptions( controller_->GetCurrentBaseAngle() );
			//Set Error Codes Here For a Serial Process
			controller_->SetBaseAnglePartition( partitionOptions[0] );
			pProp->Set( partitionOptions[0] );
			baseAngleChangeSignal_ = false;
		}
		//AnyTime This is Signaled By a Change in Base Angle (i.e. a Motor has changed, etc.)
		if( baseAngleChangeSignal_ == true )
		{
			std::vector< double > baseOptions = controller_->GetBaseAngleOptions();
			std::vector<double> partitionOptions;
			partitionOptions = controller_->GetBaseAnglePartitionOptions( controller_->GetCurrentBaseAngle() );			
			double min = partitionOptions[0];
			double max = partitionOptions[0];

			for( int i = 0; i < partitionOptions.size(); ++i )
			{
				min = min( min, partitionOptions[i] );
				max = max( max, partitionOptions[i] );
			}
		
			//Workaround For Poor Config Update Callbacks, Make a Slider with Maximum anticipated Values
			SetPropertyLimits( g_OrientalBaseAnglePartitionOptionsName, min, max );
			controller_->SetBaseAnglePartition( partitionOptions[0] );
			pProp->Set( partitionOptions[0] );
			baseAngleChangeSignal_ = false;
		}

	}
	else
	{
		double answer;
		pProp->Get( answer );

		double currentVal = controller_->GetCurrentBaseAnglePartition();
		double diff = answer - currentVal;
		double negater;
		// Determine Whether we are looking below over above previous value
		if( diff < 0 )
		{
			negater = -1;
		}
		else if ( diff > 0 )
		{
			negater = 1;
		}
		else {  return DEVICE_OK; }  //No Change

		double mindiff;
		unsigned int minDiffIndex;
		bool firstOption = true;
		std::vector<double> options = controller_->GetBaseAnglePartitionOptions( controller_->GetCurrentBaseAngle() );

		for( int i = 0; i < options.size(); i++ )
		{
			std::ostringstream es;
			//If Looking Below this condition is still met
			if( options[i] * negater > currentVal * negater )
			{
				es << " The Current Option That Is Being Passed is: " << options[i];
				es << "\n The Negated Options Are: " << options[i] * negater << "\n The negatedCurrentValue is: " << currentVal * negater;
				LogMessage( es.str() );
				if( firstOption == true )
				{
					mindiff = abs( options[i] - answer );
					minDiffIndex = i;
					firstOption = false;
				}
				else if( abs(options[i] - answer) < mindiff )
				{
					mindiff = abs(options[i] - answer);
					minDiffIndex = i;
				}
			}
		}
		
		//Check to See if We were looking below the bound
		if( firstOption == true )
		{
			answer = controller_->GetCurrentBaseAnglePartition();
		}		
		else
		{
			answer = options[minDiffIndex];
		}

		//Set Slider to the appropriate Value
		//Effectively Makes a Plus Minus Increment
		pProp->Set( answer );

		/*os << "pulled the answer as " << answer << "\n";
		LogMessage( os.str() );*/

		if( ( ret = controller_->SetBaseAnglePartition( answer ) ) != 0 )
		{
			LogMessage("Failed To Set Base Angle Partition" );
			pProp->Set( controller_->GetCurrentBaseAnglePartition() );
			return DEVICE_INVALID_PROPERTY_VALUE;
		}

	}

	return DEVICE_OK;
}


int OrientalMotorFocus::OnBaseAngleSelect( MM::PropertyBase* pProp, MM::ActionType eAct )
{

	std::ostringstream os;

	if( eAct == MM::BeforeGet )
	{
		if( initialized_ == false )
		{
			os << "BeforeGet: The Current BaseAngle " << controller_->GetCurrentBaseAngle();
			LogMessage( os.str() );
			//Somehow the Controller Has Not Set The Base Angle Appropriately
			if( controller_->GetCurrentBaseAngle() > 360 )
			{
				return DEVICE_LOCALLY_DEFINED_ERROR;
			}
			pProp->Set( controller_->GetCurrentBaseAngle() );
			//Update The BaseAngle Partitions to Reflect the New BaseAngle Options
			baseAngleChangeSignal_ = true;
			UpdateProperty( g_OrientalBaseAnglePartitionOptionsName );
		}
	}
	else if( eAct == MM::AfterSet )
	{

		double answer;
		pProp->Get( answer );

		os << "AfterGet: The Current BaseAngle " << controller_->GetCurrentBaseAngle();
		LogMessage( os.str() );

		if( answer != controller_->GetCurrentBaseAngle() )
		{
			if( controller_->SetBaseAngleOption( answer ) != 0 )
			{
				return DEVICE_INVALID_PROPERTY_VALUE;
			}
			pProp->Set( controller_->GetCurrentBaseAngle() );
		}

		//Update The BaseAngle Partitions to Reflect the New BaseAngle Options
		baseAngleChangeSignal_ = true;
		UpdateProperty( g_OrientalBaseAnglePartitionOptionsName );
		
	}

	return DEVICE_OK;
}

//Action Handler only set during Pre-Initialization Phase
int OrientalMotorFocus::OnAdjusterSelect(MM::PropertyBase* pProp, MM::ActionType eAct)
{
   const AdjusterKnob * adjuster;
   int ret = DEVICE_OK;

   if (eAct == MM::BeforeGet)
   {
	   //Should only be Hardware Related
   }
   else if (eAct == MM::AfterSet)
   {
      std::string answer;
      pProp->Get(answer);

	  //Custom properties can only be put in before initialization
	  if( answer == "Custom" )
	  {
		//Read the other aspects that might be used
		//Unlock other characteristics

	  }
	  else
	  {
		ret = SetAdjuster( answer );	
	  }
   }

   return ret;
}


//Action Handler only set during Pre-Initialization Phase
int OrientalMotorFocus::OnControllerSelect(MM::PropertyBase* pProp, MM::ActionType eAct)
{
   AbstractControllerInterface * controller;
   int ret = DEVICE_OK;

   if (eAct == MM::BeforeGet)
   {
		
   }
   else if (eAct == MM::AfterSet)
   {
      std::string answer;
      pProp->Get(answer);
	
	  ret = SetController( answer );

	  //Try and recover previous controller_
	  if( ret != DEVICE_OK )
	  {
		  if( controller_ != nullptr )
		  {

			  ret = pProp->Set( controller_->getName().c_str() );
		  }
	  }

   }

   return ret;
}

//Action Handler only set during Pre-Initialization Phase
int OrientalMotorFocus::OnAddressSelect(MM::PropertyBase* pProp, MM::ActionType eAct)
{
   if (eAct == MM::BeforeGet)
   {
	   
   }
   else if (eAct == MM::AfterSet)
   {
      long answer;
      pProp->Get(answer);
	  controller_->setAddress( answer );

	  //Error Checking On getAddress
	  controller_->GetAddress( answer );
	  
   }

   return DEVICE_OK;
}

int OrientalMotorFocus::OnPosition(MM::PropertyBase* pProp, MM::ActionType eAct)
{
   int errCode;
   LogMessage("In OnPosition");

   if (eAct == MM::BeforeGet)
   {
      std::stringstream s;
      s << pos_um_;
      pProp->Set(s.str().c_str());
   }
   else if (eAct == MM::AfterSet)
   {
      double pos;
      pProp->Get(pos);
      if (pos > upperLimit_ || lowerLimit_ > pos)
      {
		  LogMessage("Out of Limit");
         pProp->Set(pos_um_); // revert
         return DEVICE_UNKNOWN_POSITION;
      }
	  
	  double degrees = (pos - pos_um_) * 360 / adjuster_->single_rot_travel_um_;
	  int steps = degrees / controller_->GetCurrentBaseAnglePartition();
	  std::ostringstream os;
	  //os << "Gonna Write: Degrees is " << degrees << "\nSteps is " << steps;
	  errCode = controller_->WritePos( steps * -1 );
	  //os << "\n The ErrCode is :" << errCode;
	  //LogMessage(os.str());
	  if( errCode == 0 )
	  {
		  //Match pos_um_ to SetPropertyValue With No Revert
		 pos_um_ = pos;
	  }
	  else
	  {
		  pProp->Set( pos_um_ );
		  if( errCode != 12 ) //Only if not Redirected Due to Motor Busyness
		  {
			  return DEVICE_SERIAL_COMMAND_FAILED;
		  }
	  }

   }

   return DEVICE_OK;
}

int OrientalMotorFocus::OnSequence(MM::PropertyBase* pProp, MM::ActionType eAct)
{
   if (eAct == MM::BeforeGet)
   {
      std::string answer = "No";
      if (sequenceable_)
         answer = "Yes";
      pProp->Set(answer.c_str());
   }
   else if (eAct == MM::AfterSet)
   {
      std::string answer;
      pProp->Get(answer);
      if (answer == "Yes")
         sequenceable_ = true;
      else
         sequenceable_ = false;
   }
   return DEVICE_OK;
}

int OrientalMotorFocus::OnRestingEnergyStateSelect(MM::PropertyBase* pProp, MM::ActionType eAct) 
{

	if( eAct == MM::BeforeGet )
	{
		//Do Nothing, Because This is a Bit that Should Be Set Of Its Own Accord 
	}
	else if ( eAct == MM::AfterSet ) 
	{

		std::string answer;
		pProp->Get(answer);
		bool energyOn;
		if( answer == "Enable" ) {
			energyOn = true;
		}
		else if( answer == "Disable" ) {
			energyOn = false;
		}
		//Catch And Propagate any Errors as MMErrCodes
		try {
			controller_->SetRestingHardwareEnergized( energyOn, true );  //Use HardWare Processes
		}  
		catch ( MMErrorCodeException& ex ) {
			return ex.getErrCode();
		}

	}

	return DEVICE_OK;


}