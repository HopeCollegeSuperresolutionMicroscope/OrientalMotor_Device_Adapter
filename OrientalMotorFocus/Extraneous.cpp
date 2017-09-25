/*
Functions that were undermined by undetailed interplay of properties

int OrientalFTDIHub::OnControllerSelect(MM::PropertyBase* pProp, MM::ActionType eAct, long peripheralNumber)
{

	if( eAct == MM::BeforeGet )
	{
		//If the peripheral Number if greater than the numPeripherals_ make it readOnly
		if( peripheralNumber >= numPeripherals_ )
		{
			MM::Property * fullProp = dynamic_cast<MM::Property *>(pProp);
			//Not sure if the API uses this value but I would assume so
			//Make The Property Unusable to the User
			fullProp->SetReadOnly();
		}

	}
	else if( eAct == MM::AfterSet )
	{
		std::string contName;
		pProp->Get( contName );

		//Store peripheral controller StringName for later reference
		if(peripheralNumber < numPeripherals_)
		{
			peripherals_[peripheralNumber] = contName;
		}
		else
		{
			std::ostringstream os;
			os << "Peripheral Number " << peripheralNumber << " Outside of numPeripherals_ " << numPeripherals_;
			LogMessage( os.str(), true );
		}

	}

	return DEVICE_OK;
}








*/

/*

#include <iostream>
#include <cassert>
using namespace std;

//Base Class for Shared Method Calling
struct AbstractRegister
{
    
    virtual void Write( long ) = 0;
    virtual void Read( long& ) = 0;
    virtual long getRegisterSize( void ) = 0;
    //Given the Buffer Passed From Hub Functions, it is the burden of the programmer to pass the numBytesToEnd
    virtual void ReadToBytes( unsigned char buffer[], int numBytesToEnd, bool isBigEndianBuf = true )
    {
        long size = getRegisterSize();
        assert( numBytesToEnd >= size );
        long temp;
        Read( temp );
        for( int i = 0; i< size; ++i )
        {
            if( isBigEndianBuf )
            {
            
                buffer[ i ] = ( temp  >> (8 * size-i-1) );
            }
            else
            {
                buffer[i] = (temp >> ( 8 * i ) );   
            }
        }
        
    }
    virtual void WriteFromBytes( const unsigned char buffer[], int numBytesToEnd, bool isBigEndianBuf = true )
    {
        long size = getRegisterSize();
        assert( numBytesToEnd >= size );
        long temp = 0;
        for( int i = 0; i < size; ++i )
        {
            
            if( isBigEndianBuf )
            {
                temp = ( (temp | buffer[ i ]) << (8 * size-i-1 ) );
            }
            else
            {
                temp = (temp | (static_cast<long>( buffer[i] ) << (8 * i) ) );
            }
            
        }
        Write( temp );
    }

    
};

//ReadWritePolicy Structs
template< typename inputType >
struct LinearRangeWritePolicy
{
    
    static inputType ProduceWriteValue( long value ) 
    {
        inputType temp = static_cast< inputType >( value );
        //Logic For Checking
        return temp;
    }
    
    static long ProduceReadValue( inputType value )
    {
        
        long temp = static_cast< long >( value );
        //Logic for Checking
        return temp;
    }
    
    
};

template< typename value >
struct TypeFault
{
  
  
    
    
    
    
};

template< typename inputType, template< class > class ReadWritePolicy >
struct genericRegister : AbstractRegister
{
    
    inputType val_;
    
    void Write( long value )
    {
        val_ = ReadWritePolicy<inputType>::ProduceWriteValue( value );
    }
    
    void Read( long& value )
    {
        value = ReadWritePolicy<inputType>::ProduceReadValue( val_ );   
    }
    
    long getRegisterSize( void )
    {
        return sizeof( inputType );
    }
    
};

enum testEnum : short {
    
    a = 12,
    b,
    c,
    d,
    e,
    f,
    g
};

typedef genericRegister< testEnum, LinearRangeWritePolicy > _16bitLinearReg;

int main() {
	
	_16bitLinearReg* foo = new _16bitLinearReg();
	
	foo->Write( 32767+ 10000 );
	
	long temp;
	
	foo->Read( temp );
	
	cout << "The Register is " << temp << endl;
	
	return 0;
}

*/