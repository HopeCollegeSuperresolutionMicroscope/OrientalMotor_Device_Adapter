#ifndef _SMART_REGISTERS_
#define _SMART_REGISTERS_

#include <stdlib.h>
#include <stdint.h>
#include <map>
#include "ReadWritePolicies.h"
#include <boost/static_assert.hpp>
#include "smartEnum.h"

class AbstractControllerInterfaceFactory;


/******************
 * 
 * Register Stuff
 * 
 ******************/

//Abstract Base For Register Objects
//Allows Writing and Reading From/To arrays of unsigned Bytes
class AbstractRegisterBase
{
	public:
		//Given An Array of Bytes, Write Them To the Register Object
		//0 Return Indicates A Correct Return, Otherwise Error Codes Are Reported
		virtual int write( const unsigned char buffer[], int bufSize ) = 0;

		//Read the Current Value in the Register Object To a Buffer
		virtual int read( unsigned char buffer[] , int bufSize ) = 0;

		virtual uint32_t getAddress( void ) = 0;

		//Read the Constant Address for the Register To a Buffer
		virtual int readAddress( unsigned char buffer[], int bufSize ) = 0;

		//Returns the Byte Size of the Object Held in The Register
		virtual int getRegisterByteSize( void ) = 0;

		//Test a Packet to Be Sent For Conformance to Range of Accepted Values
		//Return 1 if Packet Conforms, 0 is Packet does not, or Otherwise Error Codes
		virtual int testSerialDataConformance( unsigned char buffer[], int bufSize ) = 0;

		//External Reference to Endianess of the Value, Used for Passing Values
		virtual bool isBigEndianCheck( void ) const = 0;
    
};

//Generic Register Implementation
//Template Parameters:
//	AddressType - Should Match Byte Length and Signed Nature of Actual Register Emulated
//	ValueType - May Be A Basic Type Matching Emulated Register Length, or an EnumWrapped Class
//			ValueType Note: Basic Types Are Converted To LinearEnumWrapper Classes
//  ValueIsBigEndian - Describes The Endianness of Written and Read Byte Arrays to characterize ValueType
//  Min - Used to Create LinearEnumWrapper Max For BasicTypes
//  Max - Used to Create LinearEnumWrapper Min for BasicTypes
//  ReadWritePolicy - Policy Structure Considering ValueType and Endianess, Communicates via Byte Arrays For all Data Queries (address or value)
template< typename AddressType, typename ValueType, bool IsBigEndian = true, int Min = 0, int Max = 0, template< class, bool > class ReadWritePolicy = ReadWrite >
class GenericRegister : public AbstractRegisterBase
{
	
	private:

		//Type Checking To Be Used to Ensure That Only AbstractEnumWrapper or signed/unsigned Char Derivate Types are Used
		//Further Checking at http://stackoverflow.com/questions/16132123/how-to-determine-whether-the-template-type-is-a-basic-type-or-a-class
		/*class NullType {};

		template< typename H, typename T >
		class TypeList {

			public:
			typedef H Head;
			typedef T Tail;

		};

		#define CONSTRUCT_NULL_TERMINATED_TYPELIST_1( Type1 ) TypeList<Type1, NullType>
		#define CONSTRUCT_NULL_TERMINATED_TYPELIST_2( Type2, Type1 ) TypeList<Type2, CONSTRUCT_NULL_TERMINATED_TYPELIST_1(Type1) >
		#define CONSTRUCT_NULL_TERMINATED_TYPELIST_3( Type3, Type2, Type1 ) TypeList<Type3, CONSTRUCT_NULL_TERMINATED_TYPELIST_2(Type2, Type1) >
		#define CONSTRUCT_NULL_TERMINATED_TYPELIST_4( Type4, Type3, Type2, Type1 ) TypeList<Type4, CONSTRUCT_NULL_TERMINATED_TYPELIST_3( Type3, Type2, Type1) >
		#define CONSTRUCT_NULL_TERMINATED_TYPELIST_5( Type5, Type4, Type3, Type2, Type1 ) TypeList<Type5, CONSTRUCT_NULL_TERMINATED_TYPELIST_4( Type4, Type3, Type2, Type1) >



		template< class TList > class IsAllowedTypeDerivative;

		template< >
		class IsAllowedTypeDerivative<NullType> {

			public:

				enum { Result = false };
		};

		template< typename Head, typename Tail >
		class IsAllowedTypeDerivative< TypeList< Head, Tail > > {

			public:

				enum { Result = Conversion< ValueType, Head>::isSame || Conversion< ValueType, Head>::exists || IsAllowedTypeDerivative< Tail > };

		};

		

		//STATIC_ASSERT TO ENSURE DATA IS BASIC TYPE OR AbstractEnumWrapper Derivative
		BOOST_STATIC_ASSERT( );*/

	    //MetaProgramming Conversion of Basic Types to LinearEnumClass
		template< bool isBasicType, typename T>
	    struct DummyEnumSelect
		{
			public:
	        struct DummyEnum : LinearEnumWrapper<DummyEnum, T >
		    {
			    enum stdEnum : T {
				    MinBound = Min,
	    			MaxBound = Max
		    	};
			};
			typedef DummyEnum Result;
		};
	    template< typename T >
		struct DummyEnumSelect< false, T>
	    {
		    typedef T Result;
	    };

    public:
		//TypeDefs For Resulting Enumerated Types From Meta Functions
		typedef typename DetermineIfAbstractWrappedType<ValueType>::Result BaseValueType; 
		typedef typename DummyEnumSelect< Conversion<ValueType, BaseValueType >::sameType, ValueType >::Result EnumSpace;

		//Constructor Needs Address and Initial Value
		GenericRegister( AddressType address, BaseValueType value ) : 
				addr_(address), 
				val_( value ) { };

		//Virtual Implementation of Abstract Register Base write()
		int write( const unsigned char buffer[], int size )
		{
			BaseValueType tempVal;
			//Buffer and BaseValueType Size Mismatch
			if( ReadWritePolicy<BaseValueType, IsBigEndian>::write( buffer, size, tempVal ) == -1 )
			{
				
				return -1;
			}
			if( isAcceptedValue( tempVal ) )
			{
				val_ = tempVal; 
			}
			else
			{
				return false;
			}

			return true;
		}

		//Virtual Implementation of AbstractRegisterBase write()
		int read( unsigned char buffer[], int size ) {
			return ReadWritePolicy<BaseValueType, IsBigEndian>::read( buffer, size, val_ );
		};

		//Virtual Implementation of AbstractRegisterBase readAddress()
		//Utilizes Same ReadWrite Policy to Return Address
		int readAddress( unsigned char buffer[], int size )
		{
			return ReadWritePolicy< AddressType, IsBigEndian >::read( buffer, size, addr_ );
		};

		int getRegisterByteSize( void )
		{
			return sizeof( BaseValueType );
		}

		//Virtual Implementation of AbstractRegisterBase testSerialDataConformance()
		//Allows access to isAcceptedValue In Base
		//provides error checking on inadequate byte size and on actualy value range
		int testSerialDataConformance( unsigned char buffer[], int bufSize ) {

			if( bufSize != sizeof( BaseValueType ) )
			{
				//Log Error (Serialized Number is Smaller than target Register)
				return -1;
			}

			BaseValueType tempVal;
			ReadWritePolicy<BaseValueType, IsBigEndian>::write( buffer, bufSize, tempVal );
			return ( isAcceptedValue( tempVal ) == true ) ? 1:0 ;
			
		};


		//isAcceptedValue( value )
		//returns the result of EnumWrapper isWithinAcceptedRange( value )
		bool isAcceptedValue( BaseValueType value ) {

			return EnumSpace::isWithinAcceptedRange( value );

		};

		//External Reference to Endianess of the Value, Used for Passing Values
		bool isBigEndianCheck( void ) const {  return isBigEndian_; }
		
		//Function that returns val_
		//Note: May Be Used to Deduce the type of the value for ReadWrites
		BaseValueType getVal()
		{
			return val_;
		}

		uint32_t getAddress( void )
		{
			return static_cast< uint32_t >( addr_ );
		}



		//Since Every Class Is built on BigEndianess as a Defining feature, make it a static const Variable
		static const bool isBigEndian_ = IsBigEndian;

    private:
    
		const AddressType addr_;
		BaseValueType val_;

};

/*
//Translation Unit Unique Register Resolver Class
//Intended for Implementation in Controller Interface Extensions
namespace {

	class RegisterResolver
	{

		public:

			static int RegisterNewRegister( uint32_t deviceAddress, uint32_t registerAddress, AbstractRegisterBase* basePointer )
			{
				std::map< uint32_t, std::map< uint32_t, AbstractRegisterBase* > >::iterator deviceIter = uniqueDeviceRegistersMap.find( deviceAddress );

				//If Device address is not already registered, create new entry And Assign BasePointer Immediately
				//(Note, need to handle if different controller has same address)
				if( deviceIter == uniqueDeviceRegistersMap.end() )
				{
					uniqueDeviceRegistersMap[ deviceAddress ][ registerAddress ] = basePointer;
				}

				std::ostringstream os;
				os << "The Register Address Being Registered is " << registerAddress;
				AbstractControllerInterfaceFactory::LogMessage( os.str() );

				std::map< uint32_t, AbstractRegisterBase* >::iterator it = uniqueDeviceRegistersMap[deviceAddress].find( registerAddress );

				if( it != uniqueDeviceRegistersMap[deviceAddress].end() )
				{
					//Error Handling
					return 1;
				}

				AbstractControllerInterfaceFactory::LogMessage( "Registered" );

				uniqueDeviceRegistersMap[deviceAddress][ registerAddress ] = basePointer;

				return 0;
			}

			static int UnRegisterRegister( uint32_t deviceAddress, uint32_t registerAddress )
			{

				std::map< uint32_t, std::map< uint32_t, AbstractRegisterBase* > >::iterator deviceIter = uniqueDeviceRegistersMap.find( deviceAddress );

				//If Device address is non-existent, return error Code
				if( deviceIter == uniqueDeviceRegistersMap.end() )
				{
					return 1;
				}

				std::map< uint32_t, AbstractRegisterBase* >::iterator it = uniqueDeviceRegistersMap[ deviceAddress ].find(registerAddress);

				//Register is not found in the first place
				if( it != uniqueDeviceRegistersMap[ deviceAddress ].end() )
				{
					//Error Handling
					return 1;
				}

				//This needs to be smart_ptr'd for further implementations that could call extra instances of the pointer
				//Leave the Empty Map, as a way of saving the space for any other controllers
				uniqueDeviceRegistersMap[ deviceAddress ].erase( it );

				return 0;

			}

			static AbstractRegisterBase* GetRegisterByAddress(  uint32_t deviceAddress, uint32_t registerAddress )
			{

				std::map< uint32_t, std::map< uint32_t, AbstractRegisterBase* > >::iterator deviceIter = uniqueDeviceRegistersMap.find( deviceAddress );

				//If Device address is non-existent, return nullptr
				if( deviceIter == uniqueDeviceRegistersMap.end() )
				{
					return nullptr;
				}

				std::map< uint32_t, AbstractRegisterBase* >::iterator it = uniqueDeviceRegistersMap[ deviceAddress ].find( registerAddress );

				if( it != uniqueDeviceRegistersMap[ deviceAddress ].end() )
				{
					//Error Handling
					return nullptr;
				}

				return uniqueDeviceRegistersMap[ deviceAddress ][ registerAddress ];

			}

		private: 

			//Map of Unique Addressed Devices
			static std::map< uint32_t, std::map< uint32_t, AbstractRegisterBase* > > uniqueDeviceRegistersMap; 

	};

	std::map< uint32_t, std::map< uint32_t, AbstractRegisterBase* > > uniqueDeviceMap;
}
*/

#endif