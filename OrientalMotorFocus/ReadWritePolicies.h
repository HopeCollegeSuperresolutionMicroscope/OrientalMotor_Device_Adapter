#pragma once

#ifndef _READ_WRITE_POLICIES_
#define _READ_WRITE_POLICIES_

#include <stdlib.h>

/************************************

	READ/WRITE POLICIES

	Structures containing specialized Policies for use with Multi-Byte Register Implementations
	New Policies May be Created And Added Via Template Initialization In User Code
	Note: Due to the Use of Type Determination inside of Register Classes,
		Register Policy Functions Must Be Static instead of protected/public inherited

*************************************/

//Read Write Policy
//Dependent on Endianess and the Underlying Register Value Type
template< typename ValueType, bool isBigEndian > class ReadWrite;

//LittleEndian ReadWrite Policies
template<typename ValueType>
class ReadWrite< ValueType, false>
{
   
   //Note All static_casts are not pretty, but reduce operator overloading required by Implementer
   
   public: 

	   //Write( Buffer, size, value& )
	   //Buffer - Assumed to Be Little Endian in Byte Order
	   //Size - Number of Bytes to Read From Buffer
	   //value& - Reference Register Value To Be Stored
	   //Returns:
	   //    0 if value was written
	   //    Error Code otherwise
	   static int write( const unsigned char buffer[], int size, ValueType& value )
	   {

		   if( size != sizeof(ValueType) )
		   {
				//Error
				return 1;
		   }

		   ValueType temp = static_cast<ValueType>(0);

		   for( int i = 0; i < size && i < sizeof(ValueType); ++i )
		   {
			   temp = static_cast<ValueType>( ( static_cast<ValueType>( buffer[i] ) << static_cast<ValueType>(8*i) ) | temp);
		   }

		   value = temp;

		   return 0;

	   }

	   //Read( Buffer, size, value )
	   //Buffer - Assuemd to Be Little Endian in Byte Order
	   //Size - Number of Bytes to Read From Buffer
	   //value - Register Value to be parsed into Buffer
	   //Returns:
	   //    Number of Bytes if successful
	   //	  -1 if Buffer Size is too small
	   static int read( unsigned char buffer[], int size, const ValueType value )
	   {
		   if( size < sizeof(ValueType) )
		   {
			   return -1;
		   }

		   int numBytes = 0; //More Easily Expanded With Later Operations

		   for( int i = 0; i < sizeof(ValueType); ++i)
		   {
			   buffer[i] = value >> (8 * i);
			   numBytes++;
		   }

		   return numBytes;

	   }

};

//BigEndian Read Write Policies
template< typename ValueType >
class ReadWrite< ValueType, true >
{
    public:
		//Write( Buffer, size, value& )
		//Buffer - Assuemd to Be Big Endian in Byte Order
		//Size - Number of Bytes to Read From Buffer
		//value& - Reference Register Value To Be Stored
		//Returns:
		//    0 if value was written
		//    Error Code otherwise
		static int write( const unsigned char buffer[], int size, ValueType& value )
		{
			if( size != sizeof(ValueType) )
			{
				//Error
				return 1;
			}

			ValueType temp = static_cast<ValueType>(0);

			for( int i = 0; i < sizeof(ValueType); ++i )
			{
				if( i != sizeof(ValueType) - 1 )
				{
					temp = static_cast<ValueType>( (buffer[i] | temp) << static_cast<ValueType>(8) );    
				}
				else
				{
					temp = static_cast<ValueType>( (buffer[i] | temp) );
				}
			}

			value = temp;

			return 0;

		}

		//Read( Buffer, size, value )
		//Buffer - Assumed to Be Big Endian in Byte Order
		//Size - Number of Bytes to Read From Buffer
		//value - Register Value to be parsed into Buffer
		//Returns:
		//    Number of Bytes if successful
		//	  -1 if Buffer Size is too small

		static int read( unsigned char buffer[], int size, const ValueType value )
		{
			if( size < sizeof(ValueType) )
			{
				return -1;
			}

			int numBytes = 0; //More Easily Expanded With Later Operations

			for( int i = 0; i < sizeof(ValueType); ++i)
			{
				buffer[i] = value >> ( 8 * ( sizeof(ValueType) - i - 1 ) );
				numBytes++;
			}

			return numBytes;

		}
    
};

#endif
