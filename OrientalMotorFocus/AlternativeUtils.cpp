#include "AlternativeUtils.h"
#include <sstream>
#include <stdio.h>

#ifdef WIN32
   #define WIN32_LEAN_AND_MEAN
   #include <windows.h>
   #define snprintf _snprintf 
#else
   #include <unistd.h>
#endif

//TODO:  Need to produce some sort of check for overflowing of snprintf buffer with large numeric doubles


char CAlternativeUtils::m_pszBuffer[MM::MaxStrLength]={""};
//Default m_floatTag
std::string CAlternativeUtils::m_floatTag = "%.2f";

/**
 * Convert double value to string while taking the current number of decimalPlaces
 * Note:  Use SetFloatDecimalTag and then single argument ConvertToString for fast multiple conversions
 *
 * This function is not thread-safe, and the return value is only valid until
 * the next call to ConvertToString().
 */
const char* CAlternativeUtils::ConvertToString(double dVal, unsigned int numPlaces )
{
   	std::ostringstream os;
	os << "%." << numPlaces << "f";
   snprintf(m_pszBuffer, MM::MaxStrLength-1, os.str().c_str(), dVal); 
   return m_pszBuffer;
}

/**
 * Convert double value to string Using Last Established Number of DecimalPlaces To Produce String
 *
 * This function is not thread-safe, and the return value is only valid until
 * the next call to ConvertToString().
 */
const char* CAlternativeUtils::ConvertToString( double dVal)
{
	snprintf(m_pszBuffer, MM::MaxStrLength-1, m_floatTag.c_str(), dVal); 
	return m_pszBuffer;
}

/**
 * SetNumber of DecimalPlaces to be converted in double to String Conversion
 *
 * This function is not thread-safe, and the return value is only valid until
 * the next call to ConvertToString().
 */
void CAlternativeUtils::SetFloatDecimalTag( unsigned int numPlaces ) {

	std::ostringstream os;
	os << "%." << numPlaces << "f";
	m_floatTag = os.str();

}