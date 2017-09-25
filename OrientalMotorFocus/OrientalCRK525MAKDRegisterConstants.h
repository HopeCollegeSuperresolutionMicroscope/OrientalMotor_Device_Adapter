#ifndef _ORIENTALCRK525MAKD_REGISTER_CONSTANTS_
#define _ORIENTALCRK525MAKD_REGISTER_CONSTANTS_
#include "smartEnum.h"
#include <cstdint>


/*
*
*  Oriental CRK Series Constants for register Logic
*
*/
const char g_exceptionBase = 0x80;



//Function Codes
namespace functionCodes {

	enum functionCodes : unsigned char {

	registerRead =  0x03,
	registerWrite = 0x06,
	diagnose = 0x08,
	multipleRegisterWrite = 0x10

	};
}

/*
 *
 *  SmartEnum Wrapper Objects To Standardize Values for Generic Registers
 *
 */

/*****************************************************
			Linear Ranged Enum Values
*****************************************************/
template< typename EnumeratedBaseType >
class switchActuationEnum : public LinearEnumWrapper< switchActuationEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public: 

		typedef EnumeratedBaseType EnumBaseType;
		
		//No name prevents accidental Mismatching Context
		enum stdEnum : EnumBaseType {
			MinBound = 0,
			Make = MinBound,
			Brake = 1,
			MaxBound = Brake
		};

};
typedef switchActuationEnum< uint16_t > switchActuationEnum16Bit;

//Enclosing Class that provides scope for enum
template< typename EnumeratedBaseType >
class genericEnableEnum : public LinearEnumWrapper< genericEnableEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public: 

		typedef EnumeratedBaseType EnumBaseType;

		//No name prevents accidental Mismatching Context
		enum stdEnum : EnumBaseType {
			MinBound= 0,
			Disable = MinBound,
			Enable = 1,
			MaxBound = Enable
		};
};
typedef genericEnableEnum< uint16_t > genericEnableEnum16Bit;

template< typename EnumeratedBaseType >
class inputTypeEnum : public LinearEnumWrapper< inputTypeEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;
	//No name prevents accidental Mismatching Context
		enum stdEnum : EnumBaseType {
			MinBound = 0,
			RS485 = MinBound,
			IO = 1,
			MaxBound = IO
		};

};
typedef inputTypeEnum< uint16_t > inputTypeEnum16Bit;

template< typename EnumeratedBaseType >
class inversionTypeEnum : public LinearEnumWrapper< inversionTypeEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public: 

		typedef EnumeratedBaseType EnumBaseType;
	//This name should never be referenced and is unnecessary except for enum Base purposes
		enum stdEnum : EnumBaseType {
			MinBound= 0,
			Noninverted = MinBound,
			Inverted = 1,
			MaxBound = Inverted
		};

};
typedef inversionTypeEnum< uint16_t > inversionTypeEnum16Bit;

template< typename EnumeratedBaseType >
class stopActionExpandedTypesEnum : public LinearEnumWrapper< stopActionExpandedTypesEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound= 0,
			Immediate = MinBound,
			Decelerate,
			ImmediateAndCurrentOff,
			DecelerateAndCurrentOff,
			MaxBound = DecelerateAndCurrentOff

		};

};
typedef stopActionExpandedTypesEnum< uint16_t > stopActionExpandedTypesEnum16Bit;

template< typename EnumeratedBaseType >
class signalModesEnum : public LinearEnumWrapper< signalModesEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound= 5,
			Area = MinBound,
			Tim = 6,
			Ready = 7,
			WNG = 8,
			HomeP = 9,
			ZSG = 10,
			ROut1 = 11,
			ROut2 = 12,
			StepOut=13,
			OH = 14,
			ROut3 = 15,
			ROut4 = 16,
			MaxBound = ROut4
		};

};
typedef signalModesEnum< uint16_t > signalModesEnum16Bit;

template< typename EnumeratedBaseType >
class homeOptionsEnum : public LinearEnumWrapper< homeOptionsEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound = 0,
			Home = MinBound,
			PPreset = 1,
			MaxBound = PPreset
		};

};
typedef homeOptionsEnum< uint16_t > homeOptionsEnum16Bit;

template< typename EnumeratedBaseType >
class stopActionTypeEnum : public LinearEnumWrapper< stopActionTypeEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound = 0,
			Immediate = MinBound,
			Decelerate,
			MaxBound = Decelerate
		};

};
typedef stopActionTypeEnum< uint16_t > stopActionTypeEnum16Bit;

template< typename EnumeratedBaseType >
class stepOutActionTypesEnum : public LinearEnumWrapper< stepOutActionTypesEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound = 0,
			NoOperation = MinBound,
			Warning,
			Alarm,
			MaxBound = Alarm
		};

};
typedef stepOutActionTypesEnum< uint16_t > stepOutActionTypesEnum16Bit;

template< typename EnumeratedBaseType >
class accelRateTypeEnum : public LinearEnumWrapper< accelRateTypeEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound = 0,
			Common = MinBound, 
			Separate,
			MaxBound = Separate
		};

};
typedef accelRateTypeEnum< uint16_t > accelRateTypeEnum16Bit;

template< typename EnumeratedBaseType >
class numSensorsEnum : public LinearEnumWrapper< numSensorsEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound = 0,
			Sensors2 = MinBound, 
			Sensors3,
			MaxBound = Sensors3
		};

};
typedef numSensorsEnum< uint16_t > numSensorsEnum16Bit;

template< typename EnumeratedBaseType >
class posNegTypeEnum : public LinearEnumWrapper< posNegTypeEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound = 0,
			Negative = MinBound, 
			Positive,
			MaxBound = Positive
		};

};
typedef posNegTypeEnum< uint16_t > posNegTypeEnum16Bit;

template< typename EnumeratedBaseType >
class timDetectHomeSeekingTypeEnum : public LinearEnumWrapper< timDetectHomeSeekingTypeEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound = 0,
			Disable = MinBound,
			EnableTIM,
			EnableZSG,
			MaxBound = EnableZSG
		};

};
typedef timDetectHomeSeekingTypeEnum< uint16_t > timDetectHomeSeekingTypeEnum16Bit;

template< typename EnumeratedBaseType >
class bitSignTypeEnum : public LinearEnumWrapper< bitSignTypeEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound= 0,
			Signed = MinBound,
			Absolute,
			MaxBound = Absolute
		};

};
typedef bitSignTypeEnum< uint16_t > bitSignTypeEnum16Bit;

template< typename EnumeratedBaseType >
class base_36DegStepAnglesEnum : public LinearEnumWrapper< base_36DegStepAnglesEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound = 0,
			_36deg = MinBound,
			_18deg,
			_144deg,
			_09deg,
			_072deg,
			_045deg,
			_036deg,
			_018deg,
			_0144deg,
			_009deg,
			_0072deg,
			_0045deg,
			_0036deg,
			_0028deg,
			_0018deg,
			_00144deg,
			MaxBound = _00144deg
		};

};
typedef base_36DegStepAnglesEnum< uint16_t > base_36DegStepAnglesEnum16Bit;

template< typename EnumeratedBaseType >
class base_72DegStepAnglesEnum : public LinearEnumWrapper< base_72DegStepAnglesEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound = 0,
			_72deg = MinBound,
			_36deg,
			_288deg,
			_18deg,
			_144deg,
			_09deg,
			_072deg,
			_036deg,
			_0288deg,
			_018deg,
			_0144deg,
			_009deg,
			_0072deg,
			_00576deg,
			_0036deg,
			_00288deg,
			MaxBound = _00288deg
		};

};
typedef base_72DegStepAnglesEnum< uint16_t > base_72DegStepAnglesEnum16Bit;

template< typename EnumeratedBaseType >
class spinDirectionsEnum : public LinearEnumWrapper< spinDirectionsEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound = 0,
			CounterClockWise = MinBound,
			ClockWise = 1,
			MaxBound = ClockWise
		};

};
typedef spinDirectionsEnum< uint16_t > spinDirectionsEnum16Bit;

template< typename EnumeratedBaseType >
class parityTypeEnum : public LinearEnumWrapper< parityTypeEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound= 0,
			None = MinBound,
			Even = 1,
			Odd = 2,
			MaxBound = Odd
		};

};
typedef parityTypeEnum< uint16_t > parityTypeEnum16Bit;

template< typename EnumeratedBaseType >
class stopBitsEnum : public LinearEnumWrapper< stopBitsEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound= 0,
			Bits1 = MinBound,
			Bits2 = 1,
			MaxBound = Bits2
		};

};
typedef stopBitsEnum< uint16_t > stopBitsEnum16Bit;

template< typename EnumeratedBaseType >
class executeEnum : public LinearEnumWrapper< executeEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound = 0,
			DoNotExecute = MinBound,
			Execute,
			MaxBound = Execute
		};
};
typedef executeEnum< uint16_t > executeEnum16Bit;

template< typename EnumeratedBaseType >
class positioningModeEnum : public LinearEnumWrapper< positioningModeEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound = 0,
			Incremental = MinBound,
			Absolute,
			MaxBound = Absolute
		};
};
typedef positioningModeEnum< uint16_t > positioningModeEnum16Bit;

template< typename EnumeratedBaseType >
class operatingModeEnum : public LinearEnumWrapper< operatingModeEnum<EnumeratedBaseType>, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			MinBound = 0,
			SingleMotion = MinBound,
			LinkedMotion1,
			LinkedMotion2,
			MaxBound = LinkedMotion2
		};
};
typedef operatingModeEnum< uint16_t > operatingModeEnum16Bit;

/****************************************************************
			BitMask Range Enum Values
****************************************************************/

/*********************************************
		Operation Area Enumerated BitMasks
*********************************************/
//Cmd1 Register Enumerated Bits
template< typename EnumeratedBaseType >
class cmd1BitsEnum : public BitMaskEnumWrapper< cmd1BitsEnum< EnumeratedBaseType >, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			//Lower Byte
			//Operation Data Numbers Individual Bit Names
			M0 = 0x01,
			M1 = 0x02,
			M2 = 0x04,
			M3 = 0x08,
			M4 = 0x10,
			M5 = 0x20,
			//Operation Data Numbers Mask Value
			OpDataNumberMask = M0 | M1 | M2 | M3 | M4 | M5,
			//Upper Byte
			Start = 0x0100,  //Enable Positioning Operation
			Fwd = 0x0200,    //Enable Continuous operation in forward
			Rvs = 0x0400,    //Enable Continuous opperation in reverse
			Home = 0x0800,   //Enable return to Home Operation
			Stop = 0x1000,   //Stop the Motor
			COn = 0x2000     //Enable motor Excitation

		};

	const static stdEnum bitSelectorArray_[];

};
typedef cmd1BitsEnum< uint16_t > cmd1BitsEnum16Bit;
const cmd1BitsEnum16Bit::stdEnum cmd1BitsEnum16Bit::bitSelectorArray_[] = { cmd1BitsEnum16Bit::M0,
						cmd1BitsEnum16Bit::M1, cmd1BitsEnum16Bit::M2, cmd1BitsEnum16Bit::M3, cmd1BitsEnum16Bit:: M4, 
						cmd1BitsEnum16Bit::M5, cmd1BitsEnum16Bit::OpDataNumberMask, cmd1BitsEnum16Bit::Start, 
						cmd1BitsEnum16Bit::Fwd, cmd1BitsEnum16Bit::Rvs,	cmd1BitsEnum16Bit::Home, 
						cmd1BitsEnum16Bit::Stop, cmd1BitsEnum16Bit::COn };
HEADER_DECLARE_ENUM_CLASS_BINARY_OPERATORS( cmd1BitsEnum16Bit );

//Cmd2 Register Enumerated Bits
template< typename EnumeratedBaseType >
class cmd2BitsEnum : public BitMaskEnumWrapper< cmd2BitsEnum< EnumeratedBaseType >, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			//Lower Byte
			//Enable Rout Outputs
			ROut1 = 0x01,
			ROut2 = 0x02,
			ROut3 = 0x04,
			ROut4 = 0x08

		};

		const static stdEnum bitSelectorArray_[];

};
typedef cmd2BitsEnum< uint16_t > cmd2BitsEnum16Bit;
const cmd2BitsEnum16Bit::stdEnum cmd2BitsEnum16Bit::bitSelectorArray_[] = { cmd2BitsEnum16Bit::ROut1,
	cmd2BitsEnum16Bit::ROut2, cmd2BitsEnum16Bit::ROut3, cmd2BitsEnum16Bit::ROut4 };
HEADER_DECLARE_ENUM_CLASS_BINARY_OPERATORS( cmd2BitsEnum16Bit );

//Status 1 Register Enumerated Bits
template< typename EnumeratedBaseType >
class status1BitsEnum : public BitMaskEnumWrapper< status1BitsEnum< EnumeratedBaseType >, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			//Lower Byte
			M0_R = 0x01,
			M1_R = 0x02,
			M2_R = 0x04,
			M3_R = 0x08,
			M4_R = 0x10,
			M5_R = 0x20,
			//Operation Data Numbers Mask Value
			OpDataNumberMask_R = M0_R | M1_R | M2_R | M3_R | M4_R | M5_R,
			Wng = 0x40,        //Warning is Present
			Alm = 0x80,        //An Alarm is Present
			//Upper Byte
			Start_R = 0x0100,  //Start Bit Is On
			StepOut = 0x0200,  //Abnormal Step Deviation
			Move = 0x0400,     //Motor is Moving
			HomeP = 0x0800,    //Motor is at Home Position
			Ready = 0x2000,    //Driver is Ready
			Area = 0x8000      //Motor is inside specified range
		};

		const static stdEnum bitSelectorArray_[];

};
typedef status1BitsEnum< uint16_t > status1BitsEnum16Bit;
const status1BitsEnum16Bit::stdEnum status1BitsEnum16Bit::bitSelectorArray_[] = { status1BitsEnum16Bit::M0_R,
	status1BitsEnum16Bit::M1_R, status1BitsEnum16Bit::M2_R, status1BitsEnum16Bit::M3_R, status1BitsEnum16Bit::M4_R,
	status1BitsEnum16Bit::M5_R, status1BitsEnum16Bit::OpDataNumberMask_R, status1BitsEnum16Bit::Wng, status1BitsEnum16Bit::Alm, 
	status1BitsEnum16Bit::Start_R, status1BitsEnum16Bit::StepOut, status1BitsEnum16Bit::Move, status1BitsEnum16Bit::HomeP,
	status1BitsEnum16Bit::Ready, status1BitsEnum16Bit::Area };
HEADER_DECLARE_ENUM_CLASS_BINARY_OPERATORS( status1BitsEnum16Bit );

//Status 2 Register Enumerated Bits
template< typename EnumeratedBaseType >
class status2BitsEnum : public BitMaskEnumWrapper< status2BitsEnum< EnumeratedBaseType >, EnumeratedBaseType >
{
	public:

		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {
			//Lower Byte
			SBsy = 0x01,  //Internal Processing in Progress
			Enable = 0x02,  //Motor is Excited
			OH = 0x04,   //OverHeat Warning Present
			Tim = 0x08,  //Excitation Home Detected
			Zsg = 0x10   //Z Phase Condition Detected
			
		};

		const static stdEnum bitSelectorArray_[];

};
typedef status2BitsEnum< uint16_t > status2BitsEnum16Bit;
const status2BitsEnum16Bit::stdEnum status2BitsEnum16Bit::bitSelectorArray_[] = { status2BitsEnum16Bit::SBsy,
	status2BitsEnum16Bit::Enable, status2BitsEnum16Bit::OH, status2BitsEnum16Bit::Tim, status2BitsEnum16Bit::Zsg };
HEADER_DECLARE_ENUM_CLASS_BINARY_OPERATORS( status2BitsEnum16Bit );


/*********************************************
		Monitor Area Enumerated BitMasks
*********************************************/
//DriverStatus Register Enumerated Bits
template< typename EnumeratedBaseType >
class driverStatusBitsEnum : public BitMaskEnumWrapper< driverStatusBitsEnum< EnumeratedBaseType >, EnumeratedBaseType >
{

	public:
		
		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {

			//Least Significant Bit
			Move = 0x01,
			HomeP = 0x04,
			Ready = 0x08,
			SBusy = 0x10,
			Area = 0x20,
			Alm = 0x40,
			Enable = 0x80,
			//Second Least Significant Bit
			Wng = 0x0100,
			StepOut = 0x0200,
			LsPos = 0x0400,
			LsNeg = 0x0800,
			Slit = 0x1000,
			Homes = 0x2000,
			OH = 0x4000,
			Start_R = 0x8000,
			//Third Least Significant Bit
			M0_R = 0x010000,
			M1_R = 0x020000,
			M2_R = 0x040000,
			M3_R = 0x080000,
			M4_R = 0x100000,
			M5_R = 0x200000,
			//Operation Data Numbers Mask Value
			OpDataNumberMask_R = M0_R | M1_R | M2_R | M3_R | M4_R | M5_R,
			Tim = 0x400000,
			ZSG = 0x800000,
			//Fourth Least Signficant Bit (Alarm Codes)
			AlarmCode  =0xFF000000

		};

		const static stdEnum bitSelectorArray_[];

};
typedef driverStatusBitsEnum< uint32_t > driverStatusBitsEnum32Bit;
const driverStatusBitsEnum32Bit::stdEnum driverStatusBitsEnum32Bit::bitSelectorArray_[] = { driverStatusBitsEnum32Bit::Move,
	driverStatusBitsEnum32Bit::HomeP, driverStatusBitsEnum32Bit::Ready, driverStatusBitsEnum32Bit::SBusy, driverStatusBitsEnum32Bit::Area, 
	driverStatusBitsEnum32Bit::Alm, driverStatusBitsEnum32Bit::Enable, driverStatusBitsEnum32Bit::Wng, driverStatusBitsEnum32Bit::StepOut,
	driverStatusBitsEnum32Bit::LsPos, driverStatusBitsEnum32Bit::LsNeg, driverStatusBitsEnum32Bit::Slit, driverStatusBitsEnum32Bit::Homes,
	driverStatusBitsEnum32Bit::OH, driverStatusBitsEnum32Bit::Start_R, 	driverStatusBitsEnum32Bit::M0_R, driverStatusBitsEnum32Bit::M1_R,
	driverStatusBitsEnum32Bit::M2_R, driverStatusBitsEnum32Bit::M3_R, driverStatusBitsEnum32Bit::M4_R, driverStatusBitsEnum32Bit::M5_R,
	driverStatusBitsEnum32Bit::OpDataNumberMask_R, driverStatusBitsEnum32Bit::Tim, driverStatusBitsEnum32Bit::ZSG, driverStatusBitsEnum32Bit::AlarmCode };
HEADER_DECLARE_ENUM_CLASS_BINARY_OPERATORS( driverStatusBitsEnum32Bit );

//IO Status Register Enumerated Bits
template< typename EnumeratedBaseType >
class IOStatusBitsEnum : public BitMaskEnumWrapper< IOStatusBitsEnum< EnumeratedBaseType >, EnumeratedBaseType >
{

	public:
		
		typedef EnumeratedBaseType EnumBaseType;

		enum stdEnum : EnumBaseType {

			//Least Significant Bit
			M0 = 0x01,
			M1 = 0x02,
			M2 = 0x04,
			M3 = 0x08,
			M4 = 0x10,
			M5 = 0x20,
			//Operation Data Numbers Mask Value
			OpDataNumberMask = M0 | M1 | M2 | M3 | M4 | M5,
			//Second Least Significant Bit
			Start = 0x0100,
			AlmRst = 0x0200,
			AWO = 0x0400,
			Stop = 0x0800,
			HomePPreset = 0x1000,
			FWD = 0x2000,
			RVS = 0x4000,
			//Third Least Significant Bit
			LSPos = 0x010000,
			LSNeg = 0x020000,
			Homes = 0x040000,
			Slit = 0x080000,
			//Fourth Least Signficant Bit (Alarm Codes)
			Move = 0x01000000,
			Alm = 0x02000000,
			Out1 = 0x04000000,
			Out2 = 0x08000000,
			Out3 = 0x10000000,
			Out4 = 0x20000000

		};

		const static stdEnum bitSelectorArray_[];

};
typedef IOStatusBitsEnum< uint32_t > IOStatusBitsEnum32Bit;
const IOStatusBitsEnum32Bit::stdEnum IOStatusBitsEnum32Bit::bitSelectorArray_[] = { IOStatusBitsEnum32Bit::M0,
	IOStatusBitsEnum32Bit::M1, IOStatusBitsEnum32Bit::M2, IOStatusBitsEnum32Bit::M3, IOStatusBitsEnum32Bit::M4, 
	IOStatusBitsEnum32Bit::M5, IOStatusBitsEnum32Bit::OpDataNumberMask, IOStatusBitsEnum32Bit::Start, 
	IOStatusBitsEnum32Bit::AlmRst, IOStatusBitsEnum32Bit::AWO, IOStatusBitsEnum32Bit::Stop, 
	IOStatusBitsEnum32Bit::HomePPreset, IOStatusBitsEnum32Bit::FWD, IOStatusBitsEnum32Bit::RVS,	IOStatusBitsEnum32Bit::LSPos,
	IOStatusBitsEnum32Bit::LSNeg, IOStatusBitsEnum32Bit::Homes, IOStatusBitsEnum32Bit::Slit, IOStatusBitsEnum32Bit::Move, 
	IOStatusBitsEnum32Bit::Alm, IOStatusBitsEnum32Bit::Out1, IOStatusBitsEnum32Bit::Out2, IOStatusBitsEnum32Bit::Out3, 
	IOStatusBitsEnum32Bit::Out4 };
HEADER_DECLARE_ENUM_CLASS_BINARY_OPERATORS( IOStatusBitsEnum32Bit );


#endif