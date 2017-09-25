#include "OrientalControllerTemplate.h"
#include "OrientalCRK525PMAKD.h"
#include "OrientalMotorHub.h"

bool AbstractControllerInterfaceFactory::initialized_ = false;
std::map< std::string, ControllerMaker > AbstractControllerInterfaceFactory::controllerOptions_;
std::map< std::string, ControllerMaker >::iterator AbstractControllerInterfaceFactory::iterator_;
MM::Core* AbstractControllerInterfaceFactory::LogCore_ = nullptr;
MM::Device * AbstractControllerInterfaceFactory::Caller_ = nullptr;
bool AbstractControllerInterfaceFactory::loggerRegistered_ = false;


AbstractControllerInterface::~AbstractControllerInterface(){ 
	//Due to Loading and UnLoading In program, check if it's already a nullptr
	if( statusMonitorThreadPtr_ != nullptr)
	{
		statusMonitorThreadPtr_->removeController( this ); 
	}
}

/*  Sets OrientalHub Object for Serial Communication Function Pointer from OrientalHub for all controllers
*    @param serialCommPtr - Pointer from OrientalHub Function that is chosen to be used (May change based on hardward from USB)
*    Return - 0 if ok.  -1 if ptr is somehow nullptr
*/
int AbstractControllerInterface::setSerialCommHubPtr( OrientalFTDIHub* serialCommHubPtr ) {

	assert( serialCommHubPtr != nullptr );

	serialCommHub_ = serialCommHubPtr;

	statusMonitorThreadPtr_ = serialCommHubPtr->GetStatusMonitorThread();

	assert( statusMonitorThreadPtr_ != nullptr );

	return 0;						
}

/* Gets ControllerStatusMonitorThread* to Object that was stored in focus initialization
*    Note:  Fails if setStatusMonitorThread() has not been called
*    Return - ControllerStatusMonitorThread* to the hub controllerStatusMonitorThread
*/
ControllerStatusMonitorThread* AbstractControllerInterface::getStatusMonitorThread( void ) {

	assert( statusMonitorThreadPtr_ != nullptr );

	return statusMonitorThreadPtr_;
}


//Templated function for producing AbstractControllerInterface Pointers to dynamically allocated Objects
//  Note:  AbstractControllerInterface Implements nullptrs to simply produce a readable value from getName() for easier code injection later
//  Use of nullptrs is the burden of the programmer to avoid
template< class T >
AbstractControllerInterface* make( OrientalFTDIHub* hub, AbstractControllerInterface::SerialCommFuncPtr serialCommPtr )
{ 
	return new T( hub, serialCommPtr );
}

ControllerMaker const AbstractControllerInterfaceFactory::availableControllers_[] = { make<OrientalCRK525MAKD> };

//Take availableControllers_ and place in a name-correlated map
//Assumes no name is the same in each class
int AbstractControllerInterfaceFactory::Initialize( )
	{

		AbstractControllerInterface * cont;
		int i = 0;
		
		LogMessage( "In the AbstractControllerInterface INitialize" );
		for( ; i< sizeof(availableControllers_)/sizeof(availableControllers_[0]); i++ )
		{
			LogMessage( "This is an available Controller Creation" );
			//Instantiate a controller to get it's name only for display purposes
			cont = availableControllers_[i]( nullptr, nullptr );
			controllerOptions_[ cont->getName() ] = availableControllers_[i];
			delete cont;
		}

		initialized_ = true;

		LogMessage("Leaving Initialize");

		//return size
		return i;

	};


//read all keys
std::vector<std::string> AbstractControllerInterfaceFactory::ReadAllOptionNames( void )
	{
		LogMessage( "In the AbstractControllerInterface ReadAllOptions");
		//Initialize the AbstractControllerInterfaceFactory if there is no controllerOptions_
		if( controllerOptions_.empty() && initialized_ == false )
		{
			Initialize();
		}

		std::vector< std::string > v;

		for( iterator_ = controllerOptions_.begin(); iterator_ != controllerOptions_.end(); ++iterator_ )
		{
			v.push_back( iterator_->first );
		}

		std::string testStr("Vector Length is ");
		testStr += CDeviceUtils::ConvertToString( (int) v.size() );

		LogMessage( testStr.c_str() );

		LogMessage( v[0].c_str() );

		return v;

	};

//Return single ControllerOption
//Soft Returns a null AbstractControllerInterface Object
//This should be handled in calling class for Error handling
//Throws Exception If There's a Problem in the 
AbstractControllerInterface* AbstractControllerInterfaceFactory::GetNewControllerOption( std::string name, OrientalFTDIHub* hub,  AbstractControllerInterface::SerialCommFuncPtr serialCommPtr )
	{
		LogMessage( "In the AbstractControllerInterface NewController" );
		//Initialize the AbstractControllerInterfaceFactory if there is no controllerOptions_
		if( controllerOptions_.empty() && initialized_ == false )
		{
			Initialize();
		}


		iterator_ = controllerOptions_.find( name );
		if( iterator_ == controllerOptions_.end() )
		{
			//Log Error
			return nullptr;
		}

		return (iterator_->second)( hub, serialCommPtr );

	};

void AbstractControllerInterfaceFactory::LogMessage( std::string msg /*More Arguments */ )
{
	if( loggerRegistered_ )
	{
		LogCore_->LogMessage( Caller_, msg.c_str(), true);
	}
};

void AbstractControllerInterfaceFactory::LogMessage( char* msg /*More Arguments */ )
{
	if( loggerRegistered_ )
	{
		LogCore_->LogMessage( Caller_, msg, true);
	}
}
