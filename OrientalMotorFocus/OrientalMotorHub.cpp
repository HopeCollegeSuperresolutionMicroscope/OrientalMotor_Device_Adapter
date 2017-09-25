#include "OrientalMotorHub.h"
#include "../../MMDevice/ModuleInterface.h"
#include "OrientalDeviceConstants.h"
#include <limits>
#include <sstream>

//include possible Hub classes with options

/*const char* g_OrientalHubName = "OrientalStepperAdjuster";
const char* g_GenericUsbHubPropName = "Oriental Motor USB Controller Hub";
const char* g_GenericHubFoundPrefix = "COM ";  //Prefix for GenericUsbHubName
const char* g_ControllerDevicePrefix = "Controller #";
const double g_maxPeripherals = 12;

const char * g_DeviceNameOrientalMotorFocus = "Oriental Motor ";*/

//Array of all AbstractControllerInterface Pointers that can be used

//To allow for uniformity As to what is displayed When a new Hub is selected
//Correlates to FTDI_DEVICE_LIST_INFO_NODE Members
//[0] = .Type (DWORD), [1] = .ID (DWORD*), [2] = .LocId(DWORD), [3] = .SerialNumber (char*), 
//[4] = .Description (char*), [5] = .ftHandle
//Used For Display UI purposed only
const char* OrientalFTDIHub::hubRelatedPropertyNames_[] = {"", "Device Id", "", "Serial Number", "Description", "" };


OrientalFTDIHub::OrientalFTDIHub( int maxConnRetries = 3 ) :
		initialized_(false),
		maxConnRetries_( maxConnRetries ),
		numPeripherals_(0),
		statusMonitorThread_(nullptr)
{
	InitializeDefaultErrorMessages();

	//Pre-Initialize Data
	
    //Allow User to Configure Adjuster attached to Motor
    CPropertyAction* pAct = new CPropertyAction(this, &OrientalFTDIHub::OnHubSelect);
	CreateProperty( g_GenericUsbHubPropName, "Unavailable", MM::String, false, pAct, true);
	
	std::ostringstream os;
	//Create Other List Properties for User to See Change from hubRelatedPropertyNames_
	for(std::size_t i = 0; i < (sizeof(hubRelatedPropertyNames_) / sizeof(hubRelatedPropertyNames_[0])); i++)
	{
		//Query Fixed Array of HubRelated Properties and instantiate named ones
		if( strlen( hubRelatedPropertyNames_[i]) > 0 )
		{
			os << "Property created is " << hubRelatedPropertyNames_[i];
			LogMessage( os.str(), true);
			CreateProperty(hubRelatedPropertyNames_[i], "Unavailable", MM::String, false, nullptr, true);
		}
	}

	AddAllowedValue( g_GenericUsbHubPropName, "Unavailable" );
	int numHubs = FindAvailableHubs( );

	//Check so that type conversions for DeviceUtils are not producing overflowed indices
	if( numHubs > INT_MAX )
	{

		os << "Number of Detected Devices Exceeds Computer Integer Max:" << INT_MAX << "\n";
		//Handle in a more appropriate Manner Later
		LogMessage( os.str() , false );
	}

	//Allow User to Configure Adjuster attached to Motor
    pAct = new CPropertyAction(this, &OrientalFTDIHub::OnPeripheralNumber);
	CreateProperty( "Number of Peripherals Attached", "0", MM::String, false, pAct, true);

	//Note: Property Limits do not use :AfterSet unless explicitly made an allowed value
	for( int i = 0; i < g_maxPeripherals; i++ )
	{
		AddAllowedValue( "Number of Peripherals Attached", CDeviceUtils::ConvertToString(i) );
	}

	/*  Semantic Use of Serial Port to Test Same Amounts of Stuff */
/*	CPropertyAction* pAct = new CPropertyAction(this, &OrientalFTDIHub::OnPort);
	CreateProperty(MM::g_Keyword_Port, "Undefined", MM::String, false, pAct, true);
	*/

}

OrientalFTDIHub::~OrientalFTDIHub() {
	Shutdown();
}

int OrientalFTDIHub::Shutdown() { 

		LogMessage( "In Hub ShutDown" );
		if(openHub_) {
			FT_Close(openHub_);
		}    
		//Values Being Initialized
		LogMessage("Closed COM");
		if(	initialized_ == true )
		{
			initialized_ = false;  
			
			if( statusMonitorThread_ != nullptr )
			{
				//Dangling Pointers to ControllerTemplates Currently'
				LogMessage("Stopping Thread");
				statusMonitorThread_->Stop();
				LogMessage("Waiting For Thread");
				statusMonitorThread_->wait();
				LogMessage("Deleting");
				delete statusMonitorThread_;
				LogMessage("Saving Nullptr");
				statusMonitorThread_ = nullptr;
			}

		}

	return DEVICE_OK; 
}


int OrientalFTDIHub::Initialize()
{
	if( initialized_ == true )
		return DEVICE_OK;

		AbstractControllerInterfaceFactory::RegisterLogger(this, GetCoreCallback());
		//Setup Status Monitor Thread For Hub
		statusMonitorThread_ = new ControllerStatusMonitorThread( *this, *GetCoreCallback() );
		statusMonitorThread_->Start();

		LogMessage("In This Part of Initialize First");

	    // Name  ( Used By MM )
		//Should want to change identifier By User Since This Hub Can Be for Multiple Uses
		char value[MM::MaxStrLength];
		GetProperty( g_GenericUsbHubPropName, value);
		std::string fullName(g_GenericUsbHubPropName);
		fullName += value;

		//should probably check for "Unavailabe" value to be sure
		int ret = CreateProperty(MM::g_Keyword_Name, fullName.c_str() , MM::String, true);
		if (DEVICE_OK != ret)
			return ret;

		initialized_ = true;

		return DEVICE_OK;
}

int OrientalFTDIHub::DetectInstalledDevices(void)
{
	LogMessage("Should Be Calling From This");
   //Currently, since the User is responsible for inputting data, the device detection is not done
   //May Change in future/with transfer to use of MM::COM port
   if( MM::CanCommunicate == DetectDevice()) 
   {
	  //Every Peripheral is an Oriental Motor
	  //So We're making numPeripherals_ installed devices with a new name each
      for (int i = 0; i < numPeripherals_; i++) 
      {
		 std::string deviceName( g_OrientalStageDeviceName );
		 deviceName += CDeviceUtils::ConvertToString(i);
		 LogMessage(" Calling Create " );
		 MM::Device* pDev = ::CreateDevice( const_cast<const char*>(deviceName.c_str()) );  //No reason to call this...
         if (pDev != nullptr) 
         {
			 LogMessage(" Adding " );
            AddInstalledDevice(pDev);
         }
	  }
   }
   
   return DEVICE_OK;
}

void OrientalFTDIHub::GetName(char* name) const
{
   CDeviceUtils::CopyLimitedString(name, g_OrientalUSBHubDeviceName);
}


//[0] = .Type (DWORD), [1] = .ID (DWORD*), [2] = .LocId(DWORD), [3] = .SerialNumber (char*), 
//[4] = .Description (char*), [5] = .ftHandle
//Used to call SetProperty/SetAllowedProptery
//hubProp allows for the setting of a property without recursion
//hardwareFail is an override property that indicates the hardware value failed
int OrientalFTDIHub::SetHubAndRelatedValues( MM::PropertyBase* hubProp )
{
	FT_DEVICE_LIST_INFO_NODE * availDevObj = nullptr;
	int ret = DEVICE_OK;
	std::string hubName;


	//Some Way to check that HubProp is not another property would make this more safe
	hubProp->Get(hubName);

	availDevObj = GetFTDIDeviceFromComValue( hubName );

	if( availDevObj == nullptr )
	{
		if( hubName == "Unavailable" )
		{
			//No Hub is Selected
			ret = DEVICE_INVALID_PROPERTY_VALUE;
		}
		else
		{
			//hubProp is assumed to be a property other than the HubProp
			LogMessage("Warning: SetHubandRelatedValues passed Property object that may not be Hub Property");
			return DEVICE_INVALID_PROPERTY_VALUE;
		}
	}
	else
	{
		if( ChangeOpenHubBySerialNumber( availDevObj->SerialNumber ) == DEVICE_INVALID_PROPERTY_VALUE )
		{
			availDevObj = nullptr;
		}
	}

	//A Set that Disregards the SetValue Rules from higher access levels
	hubProp->Set( hubName.c_str() );	

	for(std::size_t i = 0; i < (sizeof(hubRelatedPropertyNames_) / sizeof(hubRelatedPropertyNames_[0])); i++)
	{
		//Only For Properties with non-"" names
		if( strlen( hubRelatedPropertyNames_[i]) > 0 )
		{
			if( availDevObj != nullptr )
			{
				//Switch case to correlate hubRelatedPropteryNames_ to hubInfo Types
				switch( i )
				{
				  case 0:
					ret = SetProperty( hubRelatedPropertyNames_[i], CDeviceUtils::ConvertToString( static_cast<long>(availDevObj->Type) ) );
					break;
				  case 1:
					ret = SetProperty( hubRelatedPropertyNames_[i], CDeviceUtils::ConvertToString( static_cast<long>(availDevObj->ID) ) );
					break;
				  case 2:
					ret = SetProperty( hubRelatedPropertyNames_[i], CDeviceUtils::ConvertToString( static_cast<long>(availDevObj->LocId) ) );
					break;
				  case 3:
					ret = SetProperty( hubRelatedPropertyNames_[i], availDevObj->SerialNumber );
					break;
				  case 4:
					ret = SetProperty( hubRelatedPropertyNames_[i], availDevObj->Description );
					break;
				  case 5:
					SetProperty( hubRelatedPropertyNames_[i], CDeviceUtils::ConvertToString( availDevObj->ftHandle) );
					break;
				  default:
					  //Should be an error
					SetProperty( hubRelatedPropertyNames_[i], "Unavailable" );
					break;
				}
			}
			else
			{
				SetProperty( hubRelatedPropertyNames_[i], "Unavailable" );
			}
		}

	}

	return DEVICE_OK;
}




//Bundled Separately Due to the constant need to update the List 
//Fills Map of <IDS, FTDI_DEVICE_LIST_INFO_NODE> for current Setup
//Completely clears any previous data
//Should only be used to determine and store Handle Data
//Returns Vector Length
 int OrientalFTDIHub::FindAvailableHubs( )
 {
	//Ensure Map List Is Completely Blank
	curAvailDevMap_.clear();

	DWORD numHubs;

	FT_CreateDeviceInfoList( &numHubs );

	//if( numHubs == 0 ) Prompt for plugins

	FT_DEVICE_LIST_INFO_NODE devList[ MM::MaxStrLength/sizeof(FT_DEVICE_LIST_INFO_NODE) ];
	long portNum;
	std::string hubName(g_GenericHubFoundPrefix);
	std::string hubOption;
	bool first = true;
	FT_HANDLE handle;

	FT_GetDeviceInfoList( devList, &numHubs );

	//Cycle through Device List Info
	for( int i = 0; i < numHubs; ++i )
	{
		//Check if Device is not opened in other threads and store as an option
		if( (devList[i].Flags & 1) == 0 )
		{
			//If Device Won't open, softly discard it from available lists
			if( FT_OpenEx( devList[i].SerialNumber, FT_OPEN_BY_SERIAL_NUMBER, &handle ) == FT_OK )
			{
				FT_GetComPortNumber( handle, &portNum );
				FT_Close( handle );

				hubOption = hubName + CDeviceUtils::ConvertToString( portNum );
				curAvailDevMap_[ hubOption ] = devList[i];
				AddAllowedValue( g_GenericUsbHubPropName, hubOption.c_str() );
				if( first )
				{
					SetProperty(g_GenericUsbHubPropName, hubOption.c_str() );
					first = false;
				}
			}
		}
	}

	return curAvailDevMap_.size();

 }

 /******************************************

 Action Handlers

 *******************************************/

 int OrientalFTDIHub::FTDISerialCommunicate( unsigned char txMsgBuffer[], int txMsgLen,  AbstractControllerInterface* controller, bool broadcast )
{

	DWORD bytesWritten;
	std::ostringstream os;

	MMThreadGuard guard(serialLineMutex_);

	/*
	*  This Set Rate will change to a dynamic value in the another function
	*/
	if( FT_SetBaudRate( openHub_, 9600 ) != FT_OK )
	{
		LogMessage( "Could not set Baud Rate" );
	}

	if( FT_SetDataCharacteristics(openHub_, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_EVEN) != FT_OK )
	{
		LogMessage("Could not set characteristics");
	}
	FT_SetTimeouts( openHub_, 5000, 0 );
	/*
	*
	*/
	/*
	os << "The address being sent is " << (int) txMsgBuffer[0] << " and Data is ";
	for( int i = 1; i< txMsgLen; i++ )
	{
		os << "byte " << i << " Is: " << (int) txMsgBuffer[i] << "\n";
	}
	LogMessage( os.str() );*/
	
	FT_Write( openHub_, txMsgBuffer, txMsgLen, &bytesWritten );

	if( bytesWritten != txMsgLen )
	{
		//error handling internal Write Error
		return 1;
	}

	if( broadcast == true)
		return DEVICE_OK;
	LogMessage("Header Length Look Up");
	int headerLen = controller->headerLengthLookup( txMsgBuffer, txMsgLen );
	//Handle Internal Error
	if( headerLen <= 0 )
	{
		return DEVICE_ERR;
	}

	unsigned char rxBuffer[ MM::MaxStrLength ];
	DWORD bytesRead;
	LogMessage("Retrieve Header");
	if( FT_Read( openHub_, rxBuffer, headerLen, &bytesRead ) != FT_OK )
	{
		LogMessage( "Error Read" );
		return 1;
	}
	else if( bytesRead != headerLen )
	{
		LogMessage( "Failed To Read all of this" );
		//Error Report:  TimeOut Without response
		return DEVICE_ERR;
	}

	LogMessage("Check Address");
	//Generic Address Check
	if( controller->sameAddress( rxBuffer, bytesRead ) == false )
	{
		//Error Mismatched Slave Address
		return DEVICE_ERR;
	}
	LogMessage("dataLengthLookup");
	int dataLen = controller->dataLengthLookup( rxBuffer, bytesRead );

	if( dataLen <= 0 )
	{
		//Data Length Lookup Unexpected Header Information
		return DEVICE_ERR;
	}

	LogMessage("Read Data");
	if( FT_Read( openHub_, &rxBuffer[headerLen], dataLen, &bytesRead ) != FT_OK )
	{
		LogMessage("Read Issue");
		return 1;
	}
	else if( bytesRead != dataLen )
	{
		LogMessage("bytes not equal to dataLen");
		//Error Report:  TimeOut Without response
		return DEVICE_ERR;
	}

	LogMessage("Parse Data");
	//Generic controller 
	int errCode = controller->parseData( txMsgBuffer, txMsgLen, rxBuffer, bytesRead + headerLen );
	if( errCode != DEVICE_OK )
	{
		//Error Report: data returns an error
		LogMessage( CDeviceUtils::ConvertToString( errCode ) );
		return errCode;
	}

	LogMessage( "It's all good." );

	return DEVICE_OK;
}

 /******************************************

 Action Handlers

 *******************************************/

 int OrientalFTDIHub::OnHubSelect(MM::PropertyBase* pProp, MM::ActionType eAct)
 {
	 int ret;
	
	 if( eAct == MM::BeforeGet )
	 {
		 ret = SetHubAndRelatedValues( pProp );
	 }
	 else if ( eAct == MM::AfterSet )
	 {
		 //This avoids the problem of changing Hub Communications Mid-operation
		 //Attempt to Set the Hub and its related values
		 ret = SetHubAndRelatedValues( pProp );
	 }

	 return DEVICE_OK;
 }

 //Used to Allow for Addition of other controllers
int OrientalFTDIHub::OnPeripheralNumber(MM::PropertyBase* pProp, MM::ActionType eAct)
{
	int ret;
	std::ostringstream os;
	long answer;
	pProp->Get( answer );


	 if( eAct == MM::BeforeGet )
	 {
		//Could use separate initialization of AbstractControllerInterfaceFactory, but that's unnecessary given 1 time initialization in first fcn calls
	 }
	 else if ( eAct == MM::AfterSet )
	 {
		numPeripherals_ = answer;
	 }
	 return DEVICE_OK;

}

 //Attempt to Open Hub A Number of Times, in case it is still enumerating
 int OrientalFTDIHub::ChangeOpenHubBySerialNumber( char serialNum[16] )
 {
	 int ret;
	 //Close Previous Connection
	 if( openHub_ )
	 {
		FT_Close( openHub_ );
	 }
	
	 for( int i = 0; i < maxConnRetries_; i++ )
	 {
		ret = FT_OpenEx( serialNum, FT_OPEN_BY_SERIAL_NUMBER, &openHub_ );
		if( ret == FT_OK )
		{
			return DEVICE_OK;
		}
	 }

	 openHub_ = 0;

	 return DEVICE_INVALID_PROPERTY_VALUE;
 }

 //Use the User Interface Value to correlate to 
FT_DEVICE_LIST_INFO_NODE* OrientalFTDIHub::GetFTDIDeviceFromComValue( std::string value )
{
	std::map< std::string, FT_DEVICE_LIST_INFO_NODE >::iterator it = curAvailDevMap_.find( value );
	if( it != curAvailDevMap_.end() )
	{
		//Use the stored index for 
		return &(it->second);
	}
	else
	{
		return nullptr;
	}

}


//Semantics for Serial Port Testing
int OrientalFTDIHub::onPort( MM::PropertyBase* pProp, MM::ActionType pAct )
{

   if (pAct == MM::BeforeGet)
   {
      pProp->Set(port_.c_str());
   }
   else if (pAct == MM::AfterSet)
   {
      pProp->Get(port_);
      portAvailable_ = true;
   }
   return DEVICE_OK;

}


