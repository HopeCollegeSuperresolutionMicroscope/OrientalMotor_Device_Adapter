#pragma once
#ifndef _ORIENTALAbstractControllerInterface_
#define _ORIENTALAbstractControllerInterface_

#include <string>
#include <map>
#include <vector>
#include <stdint.h>
#include "../../MMDevice/MMDeviceConstants.h"
#include "../../MMDevice/DeviceBase.h"
#include "../../MMDevice/DeviceThreads.h"
#include "ReadWritePolicies.h"
#include "smartRegisters.h"
#include "ResetDependency.h"

//forward Declaration of OrientalFTDIHub for Use in Member Function Pointers
class OrientalFTDIHub;
class ControllerStatusMonitorThread;

//forward Declaration of AbstractControllerInterface Classes
//class OrientalCRK525MAKD;
class AbstractControllerInterface;
class AbstractControllerInterfaceFactory;

template<class T>
AbstractControllerInterface* make( void );


//Abstract Base With Methods to Be Accessed From Resolver
class AbstractControllerInterface
{

	public:
		typedef int (OrientalFTDIHub::*SerialCommFuncPtr)( unsigned char [], int ,  AbstractControllerInterface*, bool );

		/* Only Constructor
		*   @param name - Controller Specific name used for user differentiation and selection of Controller Types
		*   Throws an Exception if serialCommPtr Has not Been Set
		*/
		AbstractControllerInterface( std::string name, OrientalFTDIHub* hub, SerialCommFuncPtr serialCommPtr ) : 
			name_(name),
			serialCommHub_(hub),
			serialCommPtr_(serialCommPtr),
			statusMonitorThreadPtr_(nullptr),
			stepPeriodUS_(10),
			currentTimeInc_(0),
			monitorTimeInc_(0),
			currentBaseAnglePartition_(400),
			currentBaseAngle_(400)
			{ 
				SetPosWritePermission( true );
			};

		~AbstractControllerInterface();

		/* Controller Specific Initialization Function to initialize Physical Controller Via Serial Commands
		*   Called By Serial Hub Before any other Serial Communications may be implemented
		*    Note:  NO Serial Communication should be done inside of controller constructor, due to user ability to correct errors later
		*    Returns - 0 or errCode otherwise
		*/
		virtual int InitializePhysicalController() = 0;

		/* Virtual Function To Send a testConnection Packet Request to verify working Order
		*	Returns - 0 if successful or errCode otherwise
		*/
		virtual int testConnection( void ) = 0;

		/* Templated Function that takes Any-Type Position Value and passes it to a Given Serial Communication Function
		*   Note: All Position Values are parsed into BigEndian unsigned arrays
		*		  This Function Implements the virtual function WritePosBuffer using the BigEndian Array
		*  @param posValue - Typed Value to be parsed into BigEndian unsigned Array
		*  @param *SerialCommFuncPtr - Function Pointer to Chosen Serial Communication Function
		*  Returns - Error Codes Returned By WritePosBuffer()
		*/
		template< typename T >
		int WritePos( T posValue, int (OrientalFTDIHub::*SerialCommFuncPtr)( unsigned char txMsgBuffer[], int txMsgLen,  AbstractControllerInterface* controller, bool broadcast ) )
		{
			unsigned char valueArray[ sizeof(T) ];
			for( int i = 0; i< sizeof(T); ++i )
			{
				std::ostringstream os2;
				os2 << "valueArray[ " << sizeof(T) - (i + 1) << "] = " << (int) ( (unsigned char) posValue >> (8*i) ) << "\n";
				valueArray[ sizeof(T) - (i + 1) ] = ( posValue >> ( 8*i ) );
			}

			std::ostringstream os;
			os << "The value from WritePos before is " << posValue << " and the size is " << (int) sizeof(T) << "\n";
			os << "The Array is " << (int) valueArray[0] << ", " << (int) valueArray[1] << ", " << (int) valueArray[2] << ", " << (int) valueArray[3];
			AbstractControllerInterfaceFactory::LogMessage( os.str() );

			return WritePosBuffer( valueArray, sizeof(T), true, (*SerialCommFuncPtr) );
		}

		/*  @OVERLOAD Single Parameter - Passes Current serialCommPtr_ For Serial Communication
		*  Templated Function that takes Any-Type Position Value and passes it to a Given Serial Communication Function
		*   Note: All Position Values are parsed into BigEndian unsigned arrays
		*		  This Function Implements the virtual function WritePosBuffer using the BigEndian Array
		*  @param posValue - Typed Value to be parsed into BigEndian unsigned Array
		*  Returns - 12 if not Able to write currently, other Error Codes Returned By WritePosBuffer() or Thread Error Code
		*/
		template< typename T >
		int WritePos( T posValue )
		{
			int errCode;

			if( GetPosWritePermission() == false )
			{
				//Non-Error, but Subvert Process to Allow Main Thread to Continue
				return 12;
			}

			unsigned char valueArray[ sizeof(T) ];
						
			for( int i = 0; i< sizeof(T); ++i )
			{
				AbstractControllerInterfaceFactory::LogMessage( "Repeating for I" );
				valueArray[ sizeof(T) - (i + 1) ] = ( posValue >> ( 8*i ) );
			}


			AbstractControllerInterfaceFactory::LogMessage( "Here is the Outside" );
			//Implement Virtual Function From Child
			errCode = WritePosBuffer( valueArray, sizeof(T), true );

			if( errCode != 0 );
			{
				return errCode;
			}

			//Disable other Write Commands
			SetPosWritePermission( false );

			//Start Monitor To Enable Write Commands Once IsMotorBusy == 0
			if( (errCode = getStatusMonitorThread()->addController( this ) ) != 0 )
			{
				return errCode;
			}

			return 0;
		}

		/* Templated Function that takes a reference to Any-Type Position Value and fills it with the Position Read From Serial Communication
		*   Note:  This Function Implements the virtual function ReadPosBuffer using the BigEndian Array
		*  @param posValue - Typed Value to be filled with Byte Response
		*  Returns - Number of Bytes Returned or Error Codes ( < 0 )
		*/
		template< typename T >
		int ReadPos( T& posValue )
		{
			int errCode = 0;
			unsigned char posValueArray[ sizeof(T) ];
			errCode = ReadPosBuffer( posValueArray, sizeof(T) );

			//Since errCode is < 0, only numBytes returned will make this work
			//Returned BigEndian From Virtual Function
			for( int i = 0; i < errCode; ++i )
			{
				posValue = posValueArray[ i ] << ( 8 * ( errCode - (i + 1) ) );
			}

			return errCode;
		}

		/* Templated Function that implements setAddressBuffer Function From Templated Child
		*   Note - Passes a BigEndian unsigned char Array of the Typed Value 
		*   @param addr - address of an intended Type for the anticipated Controller
		*   Returns - DEVICE_OK or Error Codes 
		*/
		template< typename T >
		int setAddress( T addr )
		{
			unsigned char addrValueArray[ sizeof( T ) ];
			
			ReadWrite< T, true>::read( addrValueArray, sizeof(T), addr);

			return setAddressBuffer( addrValueArray, sizeof( T ) );

		}

		/* Templated Function that takes a reference to Any-Type Address Value and fills it with the Address
		*   Note:  This Function Implements the virtual function ReadAddressBuffer using the BigEndian Array
		*   @param addrValue - Typed Value to be filled with address
		*   Returns - Number of Bytes for address Value or Error Codes ( < 0 )
		*/
		template< typename T >
		int GetAddress( T& addrValue )
		{
			int errCode = 0;
			unsigned char addrValueArray[ sizeof(T) ];
			errCode = getAddressBuffer( addrValueArray, sizeof(T) );

			//Since errCode is < 0, only numBytes returned will make this work
			//Returned BigEndian From Virtual Function
			for( int i = 0; i < errCode; ++i )
			{
				addrValue = addrValueArray[ i ] << ( 8 * ( errCode - (i + 1) ) );
			}

			return errCode;
		}
		
		/* Templated Function that implements setStepSpeedBuffer Function From Child
		*   Note - Passes a BigEndian unsigned char Array of the Typed Value 
		*   @param addr - address of an intended Type for the anticipated Controller
		*   Returns - DEVICE_OK or Error Codes 
		*/
		template< typename T >
		int WriteStepSpeed( T speedValue )
		{

			if( speedValue > 1000000 || speedValue < 0 )
			{
				//Error: The Speed Value Surpassed microsecond Resolution
				return 1;
			}

			int errCode;
			unsigned char speedValueArray[ sizeof(T) ];
			ReadWrite< T, true >::read( speadValueArray, sizeof(T), speedValue );

			errCode = WriteStepSpeedBuffer( speedValueArray, sizeof(T) );

			if( errCode != 0 )
			{
				return errCode;
			}

			SetStepPeriodUS( 1000000 / speedValue );

			return 0;

		}

		virtual int WriteStepSpeedBuffer( unsigned char serializedSpeedValue[], unsigned int speedValueLen ) = 0;


		/* Read Address into a Big Endian Buffer Representation of an Address
		*	Note - Only Returns Run-Time Exceptions
		*   @param addrBuffer[] - Address Buffer to have the address read into BigEndian
		*   @param bufferSize - Size of the Buffer
		*   Returns - number of Bytes that composes address if successful, or -1 if the Type Length does not match otherwise
		*/
		virtual int getAddressBuffer( unsigned char addressBuffer[], int bufSize ) = 0;

		/* Process For Creating and Writing A Serial Buffer Representative of a Position Change To a Serial Port Through Serial Comm Function
		*   @param serializedValue[] - buffer of unsigned characters representative of Pos Value
		*   @param serializedValueLen - Length of the defined buffer Values
		*   @param isBigEndian - Refers to Whether the Buffer is Big or Little Endian
		*   @param *SerialCommFuncPtr - Function Pointer to Chosen Serial Communication Function ( To Be Used in Function )
		*   Returns - Error Codes or 0 if Write occurred
		*/
		virtual int WritePosBuffer( unsigned char serializedValue[], int serializedValueLen, bool isBigEndian, SerialCommFuncPtr serialCommPtr ) = 0;
		
		/* @OVERLOAD
		*	3 Argument overload of WritePosBuffer, passes serialCommPtr_ (Globally Selected Process)
		*   @param serializedValue[] - buffer of unsigned characters representative of Pos Value
		*   @param serializedValueLen - Length of the defined buffer Values
		*   @param isBigEndian - Refers to Whether the Buffer is Big or Little Endian
		*	Returns - Error Codes or 0 if Write occurred
		*/
		virtual int WritePosBuffer( unsigned char serializedValue[], int serializedValueLen, bool isBigEndian ) 
		{
			return WritePosBuffer( serializedValue, serializedValueLen, isBigEndian, serialCommPtr_ );
		};

		/* Process for Reading Position From Serial Request and returning it in a BigEndian Byte Array
		*   @param readBuffer[] - Buffer to Return BigEndian Byte Ordered Position Value
		*   @param bufferSize - Size of the Buffer passed
		*   Returns - Number of Bytes Defined in Buffer or Error Code ( < 0 )
		*/
		virtual int ReadPosBuffer( unsigned char readBuffer[], int bufferSize ) = 0;

		/* Controller Specific Logic to take the transmit message last sent and look up anticipate header length in bytes
		*	Used specifically by Hub Serial communications to Determine Recieved Data Header Length Read
		*	@param txBuffer[] - Byte Message that was sent, used to be evaluated
		*   @param bufLen - length of the txBuffer (to avoid misIndexing)
		*   Returns - Expected Byte length of the Header in the Response
		*/
		virtual int headerLengthLookup( const unsigned char txBuffer[], const int bufLen) = 0;

		/* Controller Specific check of stored controller address against the slave address in response header
		*   Used specifically by Hub Serial communications to Determine if the anticipated Controller and return are different
		*   @param rxHeader[] - Byte Response Header that was retrieved, in which the slave address is found
		*   @param rxHeaderLen - length of the defined rxHeader
		*   Returns - True if Addresses Match
		*/
		virtual bool sameAddress( const unsigned char rxHeader[], const int rxHeaderLen ) = 0;

		/* Controller Specific Logic to determine the data length to read after the header
		*   Used specifically by Hub Serial communications to Determine Recieved Data Length Read
		*   @param rxHeader[] - Byte Response Header that was retrieved
		*   @param rxHeaderLen - length of the defined rxHeader
		*   Returns - True if Addresses Match
		*/
		virtual int dataLengthLookup( const unsigned char rxHeader[], const int rxHeaderLen ) = 0;

		/* Controller Specific Logic to parse Whole Response Message in conjunction with the transmitMessage Sent and update Software Registers
		*   Called Specifically after Verification in Hub Serial Communications
		*   @param txMsgBuffer[] = byte message that was transmitted 
		*   @param txMsgLen = byte length of defined Trasmit Message, used to detect undefined Index
		*   @param rxMsgBufer[] = byte message that was recived (Header + Data)
		*   @rxMsgLen = byte length of recieved message
		*   Return - 0 if okay, or err_code otherwise
		*/
		virtual int parseData( const unsigned char txMsgBuffer[], const int txMsgBufLen, const unsigned char rxBuffer[], const int rxBufLen ) = 0;

		/* Get Controller Specific name used in user controller selection
		*	Return - the controller specific string defined permanently in the Implementing Controller Constructor
		*/
		std::string getName() const { return name_; };

		/*  Sets Serial Communication Function Pointer from OrientalHub for all controllers
		*    @param serialCommPtr - Pointer from OrientalHub Function that is chosen to be used (May change based on hardward from USB)
		*    Return - 0 if ok.  -1 if ptr is somehow nullptr
		*/
		int setSerialCommFuncPtr( SerialCommFuncPtr serialCommPtr ) {

			if( serialCommPtr == nullptr )
				return -1;

			serialCommPtr_ = serialCommPtr;
			return 0;						
		}

		/*  Sets OrientalHub Object for Serial Communication Function Pointer from OrientalHub for all controllers
		*    @param serialCommPtr - Pointer from OrientalHub Function that is chosen to be used (May change based on hardward from USB)
		*    Return - 0 if ok.  -1 if ptr is somehow nullptr
		*/
		int setSerialCommHubPtr( OrientalFTDIHub* serialCommHubPtr );

		/* Gets ControllerStatusMonitorThread* to Object that was stored in focus initialization
		*    Note:  Fails if setSerialCommHubPtr() has not been called
		*    Return - ControllerStatusMonitorThread* to the hub controllerStatusMonitorThread
		*/
		ControllerStatusMonitorThread* getStatusMonitorThread( void );

		/*  Store a Base Register Pointer of a unique Address Value to a map for quick access
		*    Note:  This is intended to be passed to GenericRegister Constructor for auto-registration on creation
		*	 @param addr - address of the given register
		*    @param reg - Decomposed Register Pointer
		*    Returns - 0 is complete or errCode Otherwise
		*/
		int RegisterNewRegisterAddress( AbstractRegisterBase* reg ) {

			uint32_t addr = reg->getAddress();

			std::map< uint32_t, AbstractRegisterBase* >::iterator it = regAddressMap_.find( addr );

			if( it != regAddressMap_.end() )
			{
				//Error Already Registered
				return 1;
			}

			regAddressMap_[ addr ] = reg;

			return 0;
		}

		int UnregisterRegisterAddress( AbstractRegisterBase* reg ) {

			uint32_t addr = reg->getAddress();

			std::map< uint32_t, AbstractRegisterBase* >::iterator it = regAddressMap_.find( addr );

			if( it == regAddressMap_.end() )
			{
				//Error Register Does not Exist
				return 1;
			}

			regAddressMap_.erase( it );

			return 0;
		}

		/*  Retrieve a Stored Base Register Pointer with the corresponding addressType
		*	@param deviceAddress - address of the device that was stored
		*   Returns - Pointer to Register Object or nullptr if failed to find
		*/
		AbstractRegisterBase* GetRegisterByAddress(  uint32_t addr )
		{
						
			std::map< uint32_t, AbstractRegisterBase* >::iterator it = regAddressMap_.find( addr );

			if( it == regAddressMap_.end() )
			{
				//Error No Registered Addres
				return nullptr;
			}

			return regAddressMap_[ addr ];

		}

		/**************************************************************
		*
		*		Virtual Functions For Motor Angle Information
		*		Used in determining Steps-to-ZTranslation
		*
		**************************************************************/

		/* Virtual Function Set the Base Turn Angle (In Degrees) For Control
		*   Note:  Assumes at least one register has been added using RegisterBaseAngleRegToNumber()
		*  param angle - double integer that corresponds to registered keys in baseAngleRegisterOptions_
		*  Returns - 0 if successful, otherwise Error Codes
		*/
		virtual int SetBaseAngleOption( double angleOption ) {
			
			int errCode;
			//May need to check for empty() set

			std::map< double, AbstractRegisterBase* >::iterator it = baseAngleRegisterOptions_.find( angleOption );

			if( it != baseAngleRegisterOptions_.end() )
			{
				if( currentBaseAngle_ != angleOption && it != baseAngleRegisterOptions_.end() )
				{
					//Since Each Register Holds the same address just different Binary Values Unregister the Previous
					assert( UnregisterRegisterAddress( it->second ) == 0 );
					//Register this current Register Pointer
					assert( RegisterNewRegisterAddress( it->second ) == 0 );
					currentBaseAngle_ = angleOption;
					//Read From the Hardware, the current Base Angle Partition and store its UI (double) value
					double anglePart = ReadCurrentBaseAnglePartition();
					if( anglePart < 0 || anglePart > 360 )
					{
						//Error:  BaseAnglePartition Had Problems Being Read
						return 2;
					}
					currentBaseAnglePartition_ = anglePart;
				}
			}
			else
			{
				//Error, Requesting unavailable controller
				return 1;
			}

			return 0;

		}

		/*  Register a Register Responsible for BaseAngle Data To baseAngleRegisterOptions_ and the first Reg to regAddressMap_
		*    Note:  Used in Constructor in place of RegisterNewRegisterAddress for any BaseAngleRegister (allows multiple types)
		*    @param number - the GUI number the user will see displayed to pick that current register
		*    @param reg - the actual register corresponding to the number for instance (.36, _36BaseAngleRegister)
		*    Returns - 0 if successful or errorcode
		*/
		protected: int RegisterBaseAngleRegToNumber( double angleOption , AbstractRegisterBase* reg ) {

			//For Consistency, 0 < angleOption <= 360
			if( angleOption < 0 || angleOption > 360 )
			{
				//Base angle is greater than 1 rotation
				return 2;
			}

			if( baseAngleRegisterOptions_.empty() ) //Apply first number as current base Angle
			{
				currentBaseAngle_ = angleOption ;
			}

			std::map< double, AbstractRegisterBase* >::iterator it = baseAngleRegisterOptions_.find( angleOption );

			if( it != baseAngleRegisterOptions_.end() )
			{
				//Register Already Registered
				return 1;
			}
			
			//If the register is not a repeat address, register it to the controller RegisterMap
			for( it = baseAngleRegisterOptions_.begin(); it != baseAngleRegisterOptions_.end(); it++ ) 
			{
				if( it->second->getAddress() == reg->getAddress() )
				{
					break;
				}
			}
			if( it == baseAngleRegisterOptions_.end() )
			{
				RegisterNewRegisterAddress( reg );
			}
	
			baseAngleRegisterOptions_[ angleOption  ] = reg;

			return 0;
		}

		/* Returns a standard Vector of all angleOptions registered in baseAngleRegisterOptions_
		*	Note:  Used to Pass Back Angle Options to the GUI
		*   Returns - vector of doubles or an empty vector if there are no options
		*/
		public: virtual std::vector<double> GetBaseAngleOptions( void ){

			std::map< double, AbstractRegisterBase* >::iterator it = baseAngleRegisterOptions_.begin();
			std::vector< double > options;

			for( ; it != baseAngleRegisterOptions_.end(); it++ )
			{
				options.push_back(it->first);
			}

			return options;

		}
		
		/* Return The Corresponding BaseAngle Register
		*   @param baseAngleNumber - Corresponding Base Angle Number to look up Register added through RegisterBaseAngleRegToNumber()
		*	Returns - AbstractRegisterBase to register or nullptr if not found
		*
		*/
		AbstractRegisterBase* GetBaseAngleRegister( double angleOption  )
		{

			std::map< double, AbstractRegisterBase* >::iterator it = baseAngleRegisterOptions_.find( angleOption );

			if( it == baseAngleRegisterOptions_.end() )
			{
				return nullptr;
			}

			return it->second;
		}

		/* Return The Current BaseAngle Register that corresponds to the currentBaseAngle_
		*	Returns - AbstractRegisterBase* to register or nullptr if no currentBaseAngle_ is set
		*
		*/
		AbstractRegisterBase* GetCurrentBaseAngleRegister( void )
		{
			return GetBaseAngleRegister( currentBaseAngle_ );
		}



		/* Return the current selected Base Angle
		*   if currentBaseAngle > 400, it has never been set with RegisterBaseAngleRegToNumber()
		*/
		double GetCurrentBaseAngle( void )
		{
			return currentBaseAngle_;
		}

		/* Set the current BaseAngle Partition from the one passed
		*    Note: child-level behavior is implemented through SetBaseAnglePartitionImpl()
		*    @param baseAnglePartition - number that corresponds to UI value and enumerated Value in register
		*    Returns - 0 if successful or errCode otherwise
		*/
		int SetBaseAnglePartition( double baseAnglePartition ) {

			int errCode;

			//Alow for Any Child Modifications
			if( ( errCode = SetBaseAnglePartitionImpl( baseAnglePartition ) ) != 0 )
			{
				return errCode;
			}

			currentBaseAnglePartition_ = baseAnglePartition;

			return 0;

		}

		/* Virtual - Sets the current BaseAnglePartition from the one passed
		*	Note:  This will be hard-coded for every new controller
		*   Will Store a Value For Access By GetCurrentBaseAnglePartition - Particularly By Writing to the software Register
		*   @param baseAnglePartition - number that corresponds to an enumerated Value in the register
		*   Returns - 0 if successful or errCode otherwise
		*/
		virtual int SetBaseAnglePartitionImpl( double baseAnglePartition ) = 0;

		/* Virtual - Returns a Standard Vector of double values indicative of all Base Angle Partitions for a given BaseAngle
		*   Note: This will be hard-coded for every new controller
		*   Note:  Used to display all available step Increments for a selected Base Angle
		*   @param baseAngleOption - the current BaseAngle Being Queried
		*   Returns - vector of doubles or an empty vector if there is a problem
		*/

		virtual std::vector<double> GetBaseAnglePartitionOptions( double baseAngleOption ) = 0; 

		/* get the currentBaseAnglePartition_ as set by SetBaseAnglePartition
		*   LightWeight Function that assumes any implementations invoked the set command
		*   Note:  Returned Value only is meaningful if SetBaseAnglePartion() has been called
		*   Returns - last saved value to currentBaseAnglePartition_ or >360 if never set
		*/
		double GetCurrentBaseAnglePartition( void ){ 
			return currentBaseAnglePartition_;
		}

		/* Virtual - Returns the current BaseAngle Partition From the Controller
		*	Note: This Function Reads The CurrentSelectedBaseAngle Register Value
		*         It is the programmer's burden to make sure the baseAngleRegister is not subject to external manipulation
		*	Returns - Current Base Angle Partition correllated to GetBaseAnglePartitionOptions() or number > 360 if error
		*/
		virtual double ReadCurrentBaseAnglePartition( void ) = 0;

		/*  Virtual - Returns whether or not the Motor Being Controlled is currently operating
		*	 Note:  This function is necessary for enabling a write process after the motor has already spun
		*    Note:  Is intended to be Used in ControllerStatusMonitorThread, but may find other use
		*/
		virtual int IsMotorBusy( void ) = 0;

		/*  Determine Whether there is power to the motor being controlled
		*	  @param energyOn - whether or not the motor should be energized (true = On) while resting
		*	  @param useChildProcesses - Determines whether or not to use the Child Implementation function
		*		Note: SetRestingHardwareEnergizedImpl performs child-level functions, (specifically hardware communication)
		*             before storing the base level isHardwareEnergized_.  This can be time-consuming, and in certain situations
		*             like positioning, the hardware state may be known through software tabs, allowing faster return with 
		*             useChildProcesses = false
		*     Returns - 0 if successful or errCode otherwise
		*/
		int SetRestingHardwareEnergized( bool energyOn, bool useChildProcesses = true ) 
		{

			if( useChildProcesses )
			{
			   try {
				   SetRestingHardwareEnergizedImpl( energyOn );
			   }
			   catch( std::runtime_error& ex ) {
				   throw ex;
			   }
			}

			isHardwareEnergized_ = energyOn;

			return 0;
		}

		/* Virtual - Sets the current energized state in the inheriting controller logic and hardware communication
		*	Note:  This will be hard-coded for every new type of controller
		*	  @param energyOn - whether or not the motor should be energized (true = On)
		*   Returns - 0 if successful or errCode otherwise  (errCode is used to determine if the intention should be stored in isHardwareEnergized_)
		*   Throws - Exceptions for Communication Errors
		*/
		virtual void SetRestingHardwareEnergizedImpl( bool energyOn ) = 0;

		/*  Get Whether there is power to the motor being controlled
		*	  @param useChildProcesses - Determines whether or not to use the Child Implementation function
		*		Note: GetRestingHardwareEnergizedImpl performs child-level functions, (specifically hardware reading)
		*             before storing the base level isHardwareEnergized_.  This can be time-consuming, and in certain situations
		*             like positioning, the hardware state may be known through software monitoring which allows faster return 
		*             when useChildProcesses = false
		*     Returns - whether or not the motor should be energized (true = On) while resting 
	    *                 return is either software tracked, or a direct result of a hardware query (in the case of volatile status)
		*     Throws - Exceptions for communication Errors
		*/
		bool GetRestingHardwareEnergized( bool useChildProcesses = true ) 
		{

			int errCode;

			bool tempBool;

			if( useChildProcesses )
			{
				try {
					tempBool = GetRestingHardwareEnergizedImpl( );
				}
				catch( std::runtime_error& ex ) {
					//Log Message
					throw ex;
				}

			}

			isHardwareEnergized_ = tempBool;

			return 0;
		}

		/* Virtual Implementation - Gets the current energized state as a result of the inheriting controller logic and hardware communication
		*	Note:  This will be hard-coded for every new type of controller
		*	Returns - boolean value if successful  
		*   throws - SerialHardwareCommunication Failure - used to determine if the value should be stored in isHardwareEnergized_
		*/
		virtual bool GetRestingHardwareEnergizedImpl( ) = 0;

		bool isHardwareEnergized_;

		/* Permit the Controller To Do Another Step Process
		*	Note:  This function implements A ThreadSafe Redirection of WritePos() by toggling posWritePermitted_;
		*   Note:  This Function Calls the PermitPosWriteImpl Write Permission to the actual controller
		*   Returns - 0 is successful or errCode otherwise
		*/
		int PermitPosWrite( void ) {

				int errCode;

				if( ( errCode = PermitPosWriteImpl() ) != 0 )
				{
					return errCode;
				}

				SetPosWritePermission( true );

				return 0;
			
			};

		/* Permit Busy Dependent Controller Processes
		*	Note:  This function is a ThreadSafe Redirection of WritePos() by toggling posWritePermitted_;
		*   Virtual Functions That Are Called For Permission:
		*       PermitPosWriteImpl() - specifically performs child-level and controller-level permissions
		*       PermitBusyDependentProcessesImpl() - performs child-level and controller-level permissions for any other busy-dependent
		*                                   processes
		*   Returns - 0 is successful or errCode otherwise
		*/
		int PermitBusyDependentProcesses( void ) {

				int errCode;

				if( ( errCode = PermitPosWrite() ) != 0 )
				{
					return errCode;
				}

				//Any other Busy Dependent Processes (System, writing certain registers) should be enabled too
				if( ( errCode = PermitBusyDependentProcessesImpl() ) != 0 )
				{
					return errCode;
				}

				return 0;
			
			};

		/* Virtual - Perform Necessary Child-level and Serial Commands to Permit the Controller To Do Another Step Process
		*    Note:  This function is necessary for enabling a write process after the motor has already spun
		*    Note2:  Is intended to be Used in ControllerStatusMonitorThread, but may find other use
		*    Note3: Should Implement A ThreadGuard Due to the Accessing of Any Member Variables In This Function
		*	        The protected member From AbstractControllerInterface busyLock_ may be used for this purpose
		*	        It is the burden of the Programmer to ThreadLock Any Other Functions Pertaining to Values Used in This Implementation
		*	 Returns - 0 on success or errCode otherwise
		*/
		virtual int PermitPosWriteImpl( void ) = 0;

		/* Virtual - Perform Necessary Child-level and Serial Commands to Permit the Controller To Do Any Other Busy-Dependent Processes
		*    Note:  Is intended to be Used in ControllerStatusMonitorThread, but may find other use
		*    Note2: Should Implement A ThreadGuard Due to the Accessing of Any Member Variables In This Function
		* 	        The protected member From AbstractControllerInterface busyLock_ may be used for this purpose
		*	        It is the burden of the Programmer to ThreadLock Any Other Functions Pertaining to Values Used in This Implementation
		*	 Returns - 0 on success or errCode otherwise
		*/
		virtual int PermitBusyDependentProcessesImpl( void ) = 0;

		/***********************************************************************
		*
		*	Reset Lock and Waiting Functions
		*	   For Use When A Certain Type of Controller Reset is Necessary 
		*	   and is toggled either manually or automatically later
		*
		************************************************************************/


		/* Register a Controller's ResetDependency
		*   ResetDependency's must be registered to be used for ResetAll()
		*   @param resetObj - the ResetDependency that has been instantiated and is now being registered
		*	Returns - 0 if successful or error code otherwise
		*/
		int RegisterResetDependency( ResetDependency* resetObj ) {

			std::map< std::string, ResetDependency* >::iterator it = resetDepMap_.find( resetObj->getTag() );

			if( it != resetDepMap_.end() )
			{
				//Error:  There was already a tag registered
				return 1;
			}

			resetDepMap_[ resetObj->getTag() ] = resetObj;

			return 0;

		}

		/* For all registered ResetDependency's, trigger their reset() functions to indicate all resets were performed
		*	
		*/
		void ResetAllDependencies( void ) 
		{
			std::map< std::string, ResetDependency* >::iterator it = resetDepMap_.begin();

			for( ; it != resetDepMap_.end(); ++it )
			{
				it->second->reset();
			}
		}

		/* For a given tag, determine if the ResetDependency is registered and then trigger its reset() to indicate a reset was performed
		*   @param tag - tag that was registered upon construction of the ResetDependency
		*   Returns - 0 if successful or errorCode Otherwise
		*/
		int ResetSingleDependency( std::string tag ) {

			std::map< std::string, ResetDependency* >::iterator it = resetDepMap_.find( tag );

			if( it == resetDepMap_.end() )
			{
				//Error:  There was no such registered reset
				return 1;
			}

			it->second->reset();

			return 0;

		}

	private: 

		std::map< std::string, ResetDependency* > resetDepMap_;

	protected:
		
		//Used to Pass Up the serialCommPtr to Child Classes Without allowing them to change the serialCommPtr
		SerialCommFuncPtr retrieveSerialCommFuncPtr( void )
		{
			//Due to the Nature of API, hub can be passed as nullptr, this must throw an exception and should be caught
			assert( serialCommPtr_ != nullptr );
			return serialCommPtr_;
		}

		//Used to Pass Up the hub being referenced by the SerialCommFuncPtr to Child Classes Without allowing them to change
		OrientalFTDIHub* retrieveSerialCommHubPtr( void )
		{
			return serialCommHub_;
		}

		//Thread Lock Available For IsMotorBusy() Implementations
		MMThreadLock busyLock_;

	private:

		/* Set Address from a Big Endian Buffer Representation of an Address
		*	Note - Only Returns Run-Time Exceptions
		*   @param addrBuffer[] - Big Endian Buffer of the Address
		*   @param bufferSize - Size of the Buffer
		*   Returns - 0 if successful, or -1 if the Type Length does not match otherwise
		*/
		virtual int setAddressBuffer( const unsigned char serializedAddrBuffer[], int bufferSize ) = 0;

		//Default Serial Communication Function Pointer (Passed From OrientalHub in Creation)
		SerialCommFuncPtr serialCommPtr_;
		//Hub Object Used for Calling serialCommPtr;		
		OrientalFTDIHub* serialCommHub_; 

		std::string name_;

		//Map of Address keyed Registers
		std::map< uint32_t, AbstractRegisterBase* > regAddressMap_;

		//Multiple Motor Hardware means multiple baseAngle Values Stored in map and tracked by currentBaseAngle_
		std::map< double, AbstractRegisterBase* > baseAngleRegisterOptions_;
		double currentBaseAngle_;
		double currentBaseAnglePartition_;


		/*
		*  Status Monitor Scheduling Functions and Values
		*/
		friend class ControllerStatusMonitorThread;

		long GetStepPeriodUS( void ) {
			MMThreadGuard guard( stepSpeedLock_ );
			return stepPeriodUS_;
		}

		void SetStepPeriodUS( long stepPeriodUS )
		{
			MMThreadGuard guard( stepSpeedLock_ );
			stepPeriodUS_ = stepPeriodUS;
		}

		void IncCurrentMonitorTime( long numStepPeriodsPassed ) { 
			MMThreadGuard guard(timeLock_);
			currentTimeInc_ += numStepPeriodsPassed; 
		}

		bool IsPastMonitorTimeInc( void ){ 
			MMThreadGuard guard(timeLock_);
			if( currentTimeInc_ > monitorTimeInc_) 
				return true;

			return false;
		}
		void restartTimeMonitor( void ) { 
			MMThreadGuard guard(timeLock_);
			currentTimeInc_ = 0; 
		}
		void defineMonitorTime( unsigned int newMonitorTimeInc )
		{
			MMThreadGuard guard(timeLock_);
			monitorTimeInc_ = newMonitorTimeInc;
		}

		void SetPosWritePermission( bool permit )
		{
			MMThreadGuard guard( posPermissionLock_ );
			posWritePermitted_ = permit;
		}

		bool GetPosWritePermission( void )
		{
			MMThreadGuard guard( posPermissionLock_ );
			return posWritePermitted_;
		}
		
		//For Thread Scheduling
		ControllerStatusMonitorThread* statusMonitorThreadPtr_;

		MMThreadLock timeLock_;
		MMThreadLock stepSpeedLock_;
		MMThreadLock posPermissionLock_;

		bool posWritePermitted_;
		unsigned int stepPeriodUS_;
		unsigned int currentTimeInc_;
		unsigned int monitorTimeInc_;
};


//Development of Templated Storage Base For Addresses
template< typename addressType, bool isBigEndian = true >
class ControllerInterface : public AbstractControllerInterface {

	public:

		ControllerInterface( std::string name, OrientalFTDIHub* hub, AbstractControllerInterface::SerialCommFuncPtr serialCommPtr ) : AbstractControllerInterface( name, hub, serialCommPtr ), address_(0) {}
		~ControllerInterface(){}

		/* Set Address from a Big Endian Buffer Representation of an Address
		*	Note - Only Returns Run-Time Exceptions
		*   @param addrBuffer[] - Big Endian Buffer of the Address
		*   @param bufferSize - Size of the Buffer
		*   Returns - 0 if successful, or -1 if the Type Length does not match otherwise
		*/
		int setAddressBuffer( const unsigned char serializedAddrBuffer[], int bufferSize ) {

			int addrTypeValueIndex = 0;

			if( bufferSize != sizeof( addressType ) ) 
			{
				addrTypeValueIndex = bufferSize - sizeof( addressType);
				if( addrTypeValueIndex < 0 ) //Undersized Value Not Accepted
				{
					return -1;
				}

				//One Exception: if buffer is passed with padded 0's from something like a long
				for( int i = 0; i < addrTypeValueIndex; i++ )
				{
					if( serializedAddrBuffer[i] != 0 )
					{
						AbstractControllerInterfaceFactory::LogMessage("There is a type mismatch!");
						return -1;
					}
				}
			}
			
			addressType tempVal;
			if( ReadWrite< addressType, true >::write( &serializedAddrBuffer[addrTypeValueIndex], sizeof(addressType), tempVal ) != 0 )
			{
				AbstractControllerInterfaceFactory::LogMessage("ReadWriteProblem");
				return -2;
			}

			std::ostringstream os;
			os << "Buffer Size is " << bufferSize << "\n The Value to be stored is " << (int) tempVal << "\n" ;

			for( int i = 0; i < bufferSize; i++ )
			{
				os << "Buffer [" << i << "] = " << (int) serializedAddrBuffer[i] << "\n";
			}

			AbstractControllerInterfaceFactory::LogMessage( os.str() );

			address_ = tempVal;
			return DEVICE_OK;
		};

public:
		
		/* Read Address into a Big Endian Buffer Representation of an Address
		*	Note - Only Returns Run-Time Exceptions
		*   @param addrBuffer[] - Address Buffer to have the address read into BigEndian
		*   @param bufferSize - Size of the Buffer
		*   Returns - number of Bytes that composes address if successful, or -1 if the Type Length does not match otherwise
		*/
		int getAddressBuffer( unsigned char addressBuffer[], int bufSize ) {

			if( bufSize < sizeof(addressType) )
			{
				return -1;
			}

			return ReadWrite<addressType, isBigEndian>::read( addressBuffer, sizeof(addressType), address_ );
		};


	protected:

		typedef addressType ControllerAddressType;

	private:

		addressType address_;

};


typedef AbstractControllerInterface* (*ControllerMaker)( OrientalFTDIHub* hub, AbstractControllerInterface::SerialCommFuncPtr serialCommPtr );

class AbstractControllerInterfaceFactory
{
	private:
		AbstractControllerInterfaceFactory(){};
		~AbstractControllerInterfaceFactory(){};

		static bool loggerRegistered_;

	public:

		//read all keys
		static std::vector<std::string> ReadAllOptionNames( void );

		//Return single ControllerOption
		static AbstractControllerInterface* GetNewControllerOption( std::string name, OrientalFTDIHub* hub,  AbstractControllerInterface::SerialCommFuncPtr serialCommPtr );

		static void RegisterLogger( MM::Device* caller ,MM::Core * LogCore ) { loggerRegistered_ = true; Caller_ = caller; LogCore_ = LogCore; LogCore_->LogMessage( Caller_, "IN Registration of AbstractControllerInterface", true);  return;};
		static void LogMessage( std::string msg /*More Arguments */ );
		static void LogMessage( char * msg);

	private:

		static int Initialize();
		static bool initialized_;
		//Change availableControllers if new Controller is coded
		static ControllerMaker const availableControllers_[];

		//Stores Name and corresponding index for availableControllers_
		static std::map< std::string, ControllerMaker > controllerOptions_;
		static std::map< std::string, ControllerMaker >::iterator iterator_;

		static MM::Device * Caller_;
		static MM::Core* LogCore_;


};


#endif  //_ORIENTALCONTROLLERTEMPLATE_