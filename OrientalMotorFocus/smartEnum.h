#ifndef _SMART_ENUM_
#define _SMART_ENUM_

#include <stdlib.h>
#include <vector>
#include <map>
using namespace std;

/*****************************
 * 
 * Smart Enum Class Compostion For C++03
 * 
 * Description:  The Following Classes are intended to be used to derive classes that wrap a stdEnum enumerated Value
 * Two Standard Top Level Bases Are Exposed :
 *								LinearEnumWrapper - Enumerated Values Bounded By MinBound and MaxBound as Limits
 *								BitMaskEnumWrapper - Enumerated Values that describe some bits of a composite BitMask that Limits Values
 *			
 *     Derived Smart Enum Structure
 *
*		//In .h
 *		EnumClassName : TopBase< EnumName , EnumeratedBaseType >
 *		{
 *			public:
 *				//Necessary For Binary Operator Instantiation
 *				typedef EnumeratedBaseType EnumBaseType;
 *
 *				enum stdEnum : EnumBaseType { ... EnumValues ... };
 *
 *				//For BitMask Inheritors
 *				const static stdEnum bitSelectorArray_[];
 *
 *		}
 *		//For BitMaskInheritors
 *		const typename EnumClassName::stdEnum EnumClassName::bitSelectorArray_[] = { ... EnumClassName::EnumValue, etc. ... }
 *      HEADER_DECLARE_ENUM_CLASS_BINARY_OPERATORS( EnumClassName );
 *		
 *		//In .cpp File
 *      DEFINE_ENUM_CLASS_BINARY_OPERATORS( EnumClassName );
 *
 * *****************************/

//MACRO Functions To Instantiate all Necessary Binary Operators to Facillitate Combining and Passing Enumerated Values
#define HEADER_DECLARE_ENUM_CLASS_BINARY_OPERATORS( CompleteClass ) \
		CompleteClass::stdEnum operator|(const CompleteClass::stdEnum& lhs, const CompleteClass::stdEnum& rhs )

#define DEFINE_ENUM_CLASS_BINARY_OPERATORS( CompleteClass ) \
            CompleteClass::stdEnum operator|(  \
            const CompleteClass::stdEnum& lhs, const CompleteClass::stdEnum& rhs ) { \
                return static_cast< CompleteClass::stdEnum > ( \
                static_cast< CompleteClass::EnumBaseType >( lhs ) | \
                static_cast< CompleteClass::EnumBaseType >( rhs ) ); \
            }

//Abstract Base Class
//Used to allow for MetaProgramming Inheritance Checks and Logic
class AbstractEnumWrapper {
    
   protected:

      virtual ~AbstractEnumWrapper() = 0;
    
};

//CRTPEnumWrapperBase
//Uses CRTP To Ensure Compile Time Errors For Missing Static Functions in Derived Classes
//Template Parameter CRTPClass MUST be the Derived Class
template< typename CRTPClass >
class CRTPEnumWrapperBase : public AbstractEnumWrapper 
{
    public:
    //Generic Function to Enforce static polymorphism
	//Calls the Implementing Class Specialized Function
	//Returns true if value is within accepted Range for Enums
    template< typename T >
    static bool isWithinAcceptedRange( T value )
    {
        return CRTPClass::isWithinAcceptedRange( value );
    }

	//Generic Function to Enforce static polymorphism
	//Calls the Implementing Class Specialized Function
	//Returns 0 if ok or number of missing values that were not typed
	template< typename T >
	static int mapTypedValuesToEnum( std::vector< T > typedValues )
	{
		CRTPClass::mapTypedValuesToEnum( typedValues);
		return 0;
	}

};

//LinearEnumWrapper
//Class Implements Behavior to Define A Derived Wrapper of Enums with a Linear Permissible Value Range
//Inheriting Enums Must Implement MaxBound and MinBound Synonymous for Max and Min Values Allowed
//Template Parameter InheritingClass MUST be the Derived Class
//Template Parameter BaseEnumType MUST be the explicit base of the enumerated Type
template< class InheritingClass, typename BaseEnumType > 
class LinearEnumWrapper : public CRTPEnumWrapperBase< LinearEnumWrapper<InheritingClass, BaseEnumType > >
{
  
  public:
  //Reference to Inheriting Class
  typedef InheritingClass TopEnumSpace;
  
  //isWithinAcceptedRange( value )
  //Used to Evaluate if value is within Derived Enum Range
  //Returns true is Value within Range
  static bool isWithinAcceptedRange( BaseEnumType value )
  {
     bool ret = false;
     if( value >= TopEnumSpace::MinBound && value <= TopEnumSpace::MaxBound )
     {
         ret = true;
     }
     return ret;
  }

  template< typename T >
  static std::map< T, BaseEnumType > mapTypedValuesToEnum( std::vector< T > typedValues )
  {
	  std::map< T, BaseEnumType > typeValueToEnumMap;
	  
	  if( typedValues.size() != TopEnumSpace::MaxBound - TopEnumSpace::MinBound + 1 )
	  {
		  return typeValueToEnumMap;
	  }

	  for( std::vector<T>::size_type i = 0; i < typedValues.size(); ++i )
	  {
		  typeValueToEnumMap[ typedValues[i] ] = static_cast< BaseEnumType >(TopEnumSpace::MinBound + i);
	  }

	  return typeValueToEnumMap;
  }


    
};

//BitMaskEnumWrapper
//Class Implements Behavior to Define A Derived Wrapper of Enums that define a BitMask Value
//Inheriting Class Must Implement bitSelectorArray_[] 
//Inheriting Class Must define bitSelectorArray_[] and SETUP_CLASS_ENUM_BINARY_OPERATORS( InheritingClass ) outside of Class
//Template Parameter InheritingClass MUST be the Derived Class
//Template Parameter BaseEnumType MUST be the explicit base of the enumerated Type
template< class InheritingClass, typename BaseEnumType>
class BitMaskEnumWrapper : public  CRTPEnumWrapperBase< BitMaskEnumWrapper<InheritingClass, BaseEnumType > >
{
    public:
	//Reference to Inheriting Class
	typedef InheritingClass TopEnumSpace;
  
	//isWithinAcceptedRange( value )
	//Used to Evaluate if value only contains 1s within BitMask
	//Returns true is Value is within BitMask
    static bool isWithinAcceptedRange( BaseEnumType val )
    {
        bool ret = false;
        BaseEnumType maskValue = 0;
        //Construct Mask
        for( int i = 0; i < sizeof( TopEnumSpace::bitSelectorArray_ )/sizeof( TopEnumSpace::bitSelectorArray_[0] ); ++i )
        {
            maskValue = maskValue | TopEnumSpace::bitSelectorArray_[i];
        }
        
        //Test to See if any Bits Are Outside of Mask
        if( ((maskValue | val) - maskValue) == 0 )
        {
            ret = true;
        }
        
        return ret;
    
    }

	template< typename T >
	static std::map< T, BaseEnumType > mapTypedValuesToEnum( std::vector< T > typedValues )
	{
		std::map< T, BaseEnumType > typeValueToEnumMap;

		if( typedValues.size() != sizeof( TopEnumSpace::bitSelectorArray_ )/sizeof( TopEnumSpace::bitSelectorArray_[0] ) )
		{
			return typeValueToEnumMap;
		}

		for( int i = 0; i < sizeof( TopEnumSpace::bitSelectorArray_ )/sizeof( TopEnumSpace::bitSelectorArray_[0] ); ++i )
        {
            typeValueToEnumMap[ typedValues[i] ] = TopEnumSpace::bitSelectorArray_[i];
        }

		return &typeValueToEnumMap;
	}

    
};

//Helper Class For Static Conversion Inheritance
//Credit: Modern C++ Design
template <class T, class U>
class Conversion
{
	typedef char Small;
	class Big { char dummy[2]; };
	static Small Test(U&); 
	static Big Test(...);
	static T& MakeT();

	public:
		enum { 
			exists = sizeof( Test(MakeT()) ) == sizeof(Small),
			sameType = false
		};
}; 

template< class T >
class Conversion< T, T >
{
	public:
		enum { exists = true, sameType = true };
};

template< typename ValueType >
struct DetermineIfAbstractWrappedType
{
    private:
    //MetaProgramming Function for Selection AbstractEnumWrapper Types
    template < bool flag, typename T >
    struct SelectWrappedEnumType
    {
        typedef T Result;   
    };
    template< typename T >
    struct SelectWrappedEnumType< true, T >
    {
        typedef typename T::stdEnum Result;  
    };
    
    public:
    typedef typename SelectWrappedEnumType< Conversion< ValueType, AbstractEnumWrapper >::exists, ValueType >::Result Result;
};

#endif