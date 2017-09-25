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

//forward Declaration of OrientalFTDIHub for Use in Member Function Pointers
class OrientalFTDIHub;

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
			serialCommPtr_(serialCommPtr) {  };

		~AbstractControllerInterface(){};

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
		int WritePos( T posValue, int (*SerialCommFuncPtr)( unsigned char txMsgBuffer[], int txMsgLen,  AbstractControllerInterface* controller, bool broadcast ) )
		{
			unsigned char valueArray[ sizeof(T) ];
			for( int i = 0; i< sizeof(T); ++i )
			{
				valueArray[ sizeof(T) - (i + 1) ] = ( posValue >> ( 8*i ) );
			}

			return WritePosBuffer( valueArray, sizeof(T), true, (*SerialCommFuncPtr) );
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
			int errCode = 0;
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
		int setSerialCommHubPtr( OrientalFTDIHub* serialCommHubPtr ) {

			assert( serialCommHubPtr != nullptr );

			serialCommHub_ = serialCommHubPtr;
			return 0;						
		}

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

	public:

		//read all keys
		static std::vector<std::string> ReadAllOptionNames( void );

		//Return single ControllerOption
		static AbstractControllerInterface* GetNewControllerOption( std::string name, OrientalFTDIHub* hub,  AbstractControllerInterface::SerialCommFuncPtr serialCommPtr );

		static void RegisterLogger( MM::Device* caller ,MM::Core * LogCore ) { Caller_ = caller; LogCore_ = LogCore; LogCore_->LogMessage( Caller_, "IN Registration of AbstractControllerInterface", true);  return;};
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