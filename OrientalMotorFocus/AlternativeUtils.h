/**************************************************************
*
*  Static Class Used For Modified Utilities From DeviceUtils.h
*
*    Current Implementations: Adjustable Conversion of String to Float Decimal Places
*
**************************************************************/

#ifndef _ALTERNATIVE_UTILS_H_
#define _ALTERNATIVE_UTILS_H_

#include "../../MMDevice/MMDeviceConstants.h"
#include <vector>
#include <string>

class CAlternativeUtils
{

public:
   static const char* ConvertToString(double dVal, unsigned int numPlaces);
   static const char* ConvertToString(double dVal);
   static void SetFloatDecimalTag( unsigned int numPlaces );
private:

   static char m_pszBuffer[MM::MaxStrLength];
   static std::string m_floatTag;
};

#endif //_ALTERNATIVE_UTILS_H_