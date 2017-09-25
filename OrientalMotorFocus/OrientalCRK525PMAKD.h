#ifndef _ORIENTALCRK525MAKD_
#define _ORIENTALCRK525MAKD_

#include "OrientalControllerTemplate.h"
#include "../../MMDevice/MMDeviceConstants.h"
#include <string>
#include "smartRegisters.h"
#include "OrientalCRK525MAKDRegisterConstants.h"
#include "ReadWritePolicies.h"

class OrientalCRK525MAKD : public ControllerInterface< uint8_t >
{

	//Private Register List For Controller
	private:

		//Number of Bytes in a Single Register Address
		//This is Used To query if a Register Object takes up More than One Object
		typedef uint16_t baseRegisterType;
		static const int baseRegisterByteSize_ = sizeof(baseRegisterType); 
		//Number of Bytes in A Controller's Serial Address
		typedef uint16_t baseAddressType;
		static const int baseAddressByteSize_ = sizeof( baseAddressType );

		//Maximum Packet Sizes For Write Communication
		static const int maxWritePacketbytes_ = 29;

		/***************************
		  Operation Area Registers
		***************************/
		GenericRegister< baseAddressType, baseRegisterType> dwellTimeReg;
		GenericRegister< baseAddressType, genericEnableEnum16Bit > seqPosReg;
		GenericRegister< baseAddressType, operatingModeEnum16Bit > opModeReg;
		GenericRegister< baseAddressType, positioningModeEnum16Bit> posModeReg;
		GenericRegister< baseAddressType, uint32_t, true, 0, 1000000 > decelRateReg;
		GenericRegister< baseAddressType, uint32_t, true, 0, 1000000 > accelRateReg;
		GenericRegister< baseAddressType, uint32_t, true, 1, 500000 > operatingSpeedReg;
		GenericRegister< baseAddressType, int32_t, true, -8388608, 8388607 > posReg;
		GenericRegister< baseAddressType, cmd1BitsEnum16Bit > cmd1Reg;
		GenericRegister< baseAddressType, cmd2BitsEnum16Bit > cmd2Reg;
		GenericRegister< baseAddressType, status1BitsEnum16Bit > status1Reg;
		GenericRegister< baseAddressType, status2BitsEnum16Bit > status2Reg;

		/******************************
		    Maintentance Area Registers
		******************************/
		//All Addresses are boolean ( 1 = execute)
		GenericRegister< baseAddressType, executeEnum16Bit> resetAlarmReg;  		//Reset the alarms that are present
		GenericRegister< baseAddressType, executeEnum16Bit> clearAlarmRecReg;		//Clear Alarm Records
		GenericRegister< baseAddressType, executeEnum16Bit> clearWarningRecReg;	//Clear Warning Records
		GenericRegister< baseAddressType, executeEnum16Bit> batchNVReadReg;		//Read from Non-Volatile Memory into RAM
		GenericRegister< baseAddressType, executeEnum16Bit> batchNVWriteReg;		//Write from RAM into Non-Volatile Memory
		GenericRegister< baseAddressType, executeEnum16Bit> allDataInitReg;		//Resets all registers (including NV) to their defaults
		GenericRegister< baseAddressType, executeEnum16Bit> presetPosReg;			//Presets command position to "preset position" parameter
		GenericRegister< baseAddressType, executeEnum16Bit> clearComErrRecReg;		//Clear Communication Error Records
		GenericRegister< baseAddressType, executeEnum16Bit> presetEncoderReg;		//Preset encoder counter to "encoder counter preset value" parameter
		GenericRegister< baseAddressType, executeEnum16Bit> clearCounterReg;		//Clear Command Position, Encoder Counter, and deviation errors
		GenericRegister< baseAddressType, executeEnum16Bit> opDataInitReg;			//Resets all operataion data to defaults
		GenericRegister< baseAddressType, executeEnum16Bit> appParamInitReg;		//Resets all application parameters to defaults
		GenericRegister< baseAddressType, executeEnum16Bit> sysParamInitReg;		//Resets all system parameters to defaults

		/******************************
		    Monitor Area Registers
		******************************/
		static const baseAddressType g_MonitorAreaByteBase = 0x0100;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > presentAlarmReg;				//Monitors Most Recent Alarm
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > alarmRecord1Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > alarmRecord2Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > alarmRecord3Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > alarmRecord4Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > alarmRecord5Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > alarmRecord6Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > alarmRecord7Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > alarmRecord8Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > alarmRecord9Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > alarmRecord10Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > presentWarningReg;				//Monitors Most Recent Warning
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > WarningRecord1Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > WarningRecord2Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > WarningRecord3Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > WarningRecord4Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > WarningRecord5Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > WarningRecord6Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > WarningRecord7Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > WarningRecord8Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > WarningRecord9Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > WarningRecord10Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 63 > PresentSelectOpDataNumberReg;		//Check Current Operation Data Number Selected
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 63 > PresentOpDataNumberReg;			//Check Current Operation Data Number in linked-motion sequence
		GenericRegister< baseAddressType, int32_t, true, -2147483648, 2147483647 > CommandPosReg;
		GenericRegister< baseAddressType, int32_t, true, -500000, 500000 > CommandSpeedReg;
		GenericRegister< baseAddressType, int32_t, true, -2147483648, 2147483647 > EncodeCounterReg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 50000 > RemDwellTimeReg;
		GenericRegister< baseAddressType, IOStatusBitsEnum32Bit > IOStatusReg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > CommErrCodeReg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > CommErrCodeRecord1Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > CommErrCodeRecord2Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > CommErrCodeRecord3Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > CommErrCodeRecord4Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > CommErrCodeRecord5Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > CommErrCodeRecord6Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > CommErrCodeRecord7Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > CommErrCodeRecord8Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > CommErrCodeRecord9Reg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 65535 > CommErrCodeRecord10Reg;
		GenericRegister< baseAddressType, driverStatusBitsEnum32Bit > DriverStatusReg;
		
		/******************************
		     Parameter Area Registers
		******************************/
		static const baseAddressType g_ParameterAreaByteBase = 0x0200;
		GenericRegister< baseAddressType, inputTypeEnum16Bit> startInputModeReg;				//Allowed Communication For Motor Starting
		GenericRegister< baseAddressType, genericEnableEnum16Bit> IOStopInputReg;			//Enable IO Signal Stopping
		GenericRegister< baseAddressType, stopActionExpandedTypesEnum16Bit> stopActionReg;	//Stop Motion Profile
		GenericRegister< baseAddressType, switchActuationEnum16Bit> stopContactConfigReg;	
		GenericRegister< baseAddressType, inversionTypeEnum16Bit> conLogCongifReg;			//Logic Definitions For (Un)/Excited States
		//Signal Modes for Rout Outputs
		GenericRegister< baseAddressType, signalModesEnum16Bit> out1SignalModeReg;			
		GenericRegister< baseAddressType, signalModesEnum16Bit> out2SignalModeReg;
		GenericRegister< baseAddressType, signalModesEnum16Bit> out3SignalModeReg;
		GenericRegister< baseAddressType, signalModesEnum16Bit> out4SignalModeReg;
		GenericRegister< baseAddressType, homeOptionsEnum16Bit> homePPresetSwitchingReg;		
		GenericRegister< baseAddressType, inputTypeEnum16Bit> motorExciteInputModeReg;		//Allowed Communication For Motor Excitation
		GenericRegister< baseAddressType, inputTypeEnum16Bit> homeFwdRvsInputModeReg;		//Allowed Communication For Motor Seeking Movement
		GenericRegister< baseAddressType, inputTypeEnum16Bit> dataNumInputModeReg;			///Allowed Communication For Data Number Input
		GenericRegister< baseAddressType, switchActuationEnum16Bit> awoContactConfigReg;
		GenericRegister< baseAddressType, genericEnableEnum16Bit> hardwareOvertravelDetectReg;
		GenericRegister< baseAddressType, switchActuationEnum16Bit> lsContactConfigReg;
		GenericRegister< baseAddressType, switchActuationEnum16Bit> homesContactConfigReg;
		GenericRegister< baseAddressType, switchActuationEnum16Bit> slitContactConfigReg;
		GenericRegister< baseAddressType, stopActionTypeEnum16Bit> overTravelActionReg;
		GenericRegister< baseAddressType,  int32_t, true, -8388608, 8388607 > presetPosParamReg;
		GenericRegister< baseAddressType,  int32_t, true, -8388608, 8388607 > area1Reg;
		GenericRegister< baseAddressType,  int32_t, true, -8388608, 8388607 > area2Reg;
		GenericRegister< baseAddressType,  int32_t, true, -8388608, 8388607 > encoderCountPresetReg;
		//5%-100% assuming 1% is unit
		GenericRegister< baseAddressType, baseRegisterType, true, 5, 100 > operatingCurrentReg;
		//5%-50% Assuming 1% is unit
		GenericRegister< baseAddressType, baseRegisterType, true, 5, 50 > standstillCurrentReg;
		//Decimal Shifted .001 - 1000.000
		GenericRegister< baseAddressType, uint32_t, true, 1, 1000000 > commonAccelRateReg;
		//Decimal Shifted .001 - 1000.000
		GenericRegister< baseAddressType, uint32_t, true, 1, 1000000 > commonDecelRateReg;
		GenericRegister< baseAddressType, uint32_t, true, 1, 500000 > startSpeedReg;
		GenericRegister< baseAddressType, uint32_t, true, 1, 500000 > jogOperatingSpeedReg;
		//Decimal Shifted .001 - 1000.000
		GenericRegister< baseAddressType, uint32_t, true, 1, 1000000 > jogAccelRateReg;
		GenericRegister< baseAddressType, uint32_t, true, 1, 500000 > jogStartSpeedReg;
		GenericRegister< baseAddressType, accelRateTypeEnum16Bit> accelRateTypeReg;
		GenericRegister< baseAddressType, numSensorsEnum16Bit> homeSeekModeReg;
		GenericRegister< baseAddressType, uint32_t, true, 1, 500000 > homeSeekOperatingSpeedReg;
		//Assumes Decimal Place is Shifted (.001 to 1000.000)
		GenericRegister< baseAddressType, uint32_t, true, 1, 1000000 > homeSeekAccelRateReg;
		GenericRegister< baseAddressType, uint32_t, true, 1, 500000 > homeSeekStartSpeedReg;
		//This assumes 0 in register is the lowest value again
		GenericRegister< baseAddressType,  int32_t, true, -8388608, 8388607 > homeSeekPosOffsetReg;
		GenericRegister< baseAddressType, posNegTypeEnum16Bit> homeSeekStartDirReg;
		GenericRegister< baseAddressType, genericEnableEnum16Bit> homeSeekSlitDetectionReg;
		GenericRegister< baseAddressType, timDetectHomeSeekingTypeEnum16Bit> homeSeekTIMDectectionReg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 32767 > _2SensorModeBackStepsReg;
		//Assumes 1 to 3600 actual represents (.1 to 360 degrees)
		GenericRegister< baseAddressType, baseRegisterType, true, 1, 3600 > stepOutDetectionBandReg;
		//Assumes 250 is actual value represents( 25 to 35 Volts )
		GenericRegister< baseAddressType, baseRegisterType, true, 250, 350 > overVoltageWarningReg;
		//Assumes 40 to 85 are actual values (degrees C)
		GenericRegister< baseAddressType, baseRegisterType, true, 40, 85 > overHeatWarningReg;
		GenericRegister< baseAddressType, genericEnableEnum16Bit> softwareOverTravelReg;
		//Theoretically should be the upper value, if it linearly progresses
		GenericRegister< baseAddressType, int32_t, true, -8388608, 8388607 > positiveSotwareLimitReg;
		//According to other Specifications 00 is most negative
		GenericRegister< baseAddressType, int32_t, true, -8388608, 8388607 > negativeSotwareLimitReg;
		GenericRegister< baseAddressType, bitSignTypeEnum16Bit> dataSetterDisplayModeReg;
		GenericRegister< baseAddressType, genericEnableEnum16Bit> dataSetterEditModeReg;
		GenericRegister< baseAddressType, stopActionTypeEnum16Bit> communicationTimeOutActionReg;
		GenericRegister< baseAddressType, stepOutActionTypesEnum16Bit> stepOutDetectionActionReg;
		
		/******************************************
		     System Parameters Area Registers
		*******************************************/
		static const baseAddressType g_SystemParameterByteBase = 0x0300; 
		GenericRegister< baseAddressType, spinDirectionsEnum16Bit> motorRotationDirReg;
		GenericRegister< baseAddressType, genericEnableEnum16Bit> stepOutDetectionReg;
		//Multiple Motors Mean Different Enumerated Values
		GenericRegister< baseAddressType, base_36DegStepAnglesEnum16Bit> motorStepAngleReg_36;
		GenericRegister< baseAddressType, base_72DegStepAnglesEnum16Bit> motorStepAngleReg_72;
		GenericRegister< baseAddressType,  uint32_t, true, 1, 250000 > encoderGearAReg;
		GenericRegister< baseAddressType,  uint32_t, true, 1, 250000 > encoderGearBReg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 31 > communicationAxisNumReg;
		GenericRegister< baseAddressType, parityTypeEnum16Bit> communicationParityReg;
		GenericRegister< baseAddressType, stopBitsEnum16Bit> communicationStopBitReg;
		//Assumes that all decimal Values are treated as 0 point
		GenericRegister< baseAddressType, baseRegisterType, true, 1, 10000 > transmissionWaitTimeReg;
		GenericRegister< baseAddressType, baseRegisterType, true, 0, 30000 > communicationTimeOutReg;
		GenericRegister< baseAddressType, baseRegisterType, true, 1, 10 > communicationErrorAlarmReg;

	public:
	
		//Implementation function for Controllers In Use with Different Names
		OrientalCRK525MAKD( OrientalFTDIHub* hub, AbstractControllerInterface::SerialCommFuncPtr serialCommPtr ) : ControllerInterface("OrientalCRK525MAKD", hub, serialCommPtr),
			//Operations Area Registers
			dwellTimeReg( 0x0012, 0x0000 ),
			seqPosReg( 0x0013, genericEnableEnum16Bit::Disable ),
			opModeReg( 0x0014, operatingModeEnum16Bit::SingleMotion ),
			posModeReg( 0x0015, positioningModeEnum16Bit::Incremental ),
			decelRateReg( 0x0016, 0x00000000 ),
			accelRateReg( 0x0018, 0x00000000 ),
			operatingSpeedReg( 0x001A, 0x00000000 ),
			posReg( 0x001C, 0x00000000 ),
			cmd1Reg( 0x001E, static_cast< cmd1BitsEnum16Bit::stdEnum >( 0x0000) ),
			cmd2Reg( 0x001F, static_cast< cmd2BitsEnum16Bit::stdEnum >( 0x0000 ) ),
			status1Reg( 0x0020, static_cast< status1BitsEnum16Bit::stdEnum >( 0x0000 ) ),
			status2Reg( 0x0021, static_cast< status2BitsEnum16Bit::stdEnum >( 0x0000 ) ),
			//Maintenance Area Registers
			resetAlarmReg( 0x0040, executeEnum16Bit::DoNotExecute ),
			clearAlarmRecReg( 0x0041, executeEnum16Bit::DoNotExecute ),
			clearWarningRecReg( 0x0042, executeEnum16Bit::DoNotExecute ),
			batchNVReadReg( 0x0044, executeEnum16Bit::DoNotExecute ),
			batchNVWriteReg( 0x0045, executeEnum16Bit::DoNotExecute ),
			allDataInitReg( 0x0046, executeEnum16Bit::DoNotExecute ),
			presetPosReg( 0x0048, executeEnum16Bit::DoNotExecute ),
			clearComErrRecReg( 0x0049, executeEnum16Bit::DoNotExecute ),
			presetEncoderReg( 0x004A, executeEnum16Bit::DoNotExecute ),
			clearCounterReg( 0x004B, executeEnum16Bit::DoNotExecute ),
			opDataInitReg( 0x004C, executeEnum16Bit::DoNotExecute ),
			appParamInitReg( 0x004D, executeEnum16Bit::DoNotExecute ),
			sysParamInitReg( 0x004E, executeEnum16Bit::DoNotExecute ),
			//Monitor Area Registers
			presentAlarmReg( g_MonitorAreaByteBase | 0x00, 0 ),
			alarmRecord1Reg( g_MonitorAreaByteBase | 0x01, 0 ),
			alarmRecord2Reg( g_MonitorAreaByteBase | 0x02, 0 ),
			alarmRecord3Reg( g_MonitorAreaByteBase | 0x03, 0 ),
			alarmRecord4Reg( g_MonitorAreaByteBase | 0x04, 0 ),
			alarmRecord5Reg( g_MonitorAreaByteBase | 0x05, 0 ),
			alarmRecord6Reg( g_MonitorAreaByteBase | 0x06, 0 ),
			alarmRecord7Reg( g_MonitorAreaByteBase | 0x07, 0 ),
			alarmRecord8Reg( g_MonitorAreaByteBase | 0x08, 0 ),
			alarmRecord9Reg( g_MonitorAreaByteBase | 0x09, 0 ),
			alarmRecord10Reg( g_MonitorAreaByteBase | 0x0A, 0 ),
			presentWarningReg( g_MonitorAreaByteBase | 0x0B, 0 ),
			WarningRecord1Reg( g_MonitorAreaByteBase | 0x0C, 0 ),
			WarningRecord2Reg( g_MonitorAreaByteBase | 0x0D, 0 ),
			WarningRecord3Reg( g_MonitorAreaByteBase | 0x0E, 0 ),
			WarningRecord4Reg( g_MonitorAreaByteBase | 0x0F, 0 ),
			WarningRecord5Reg( g_MonitorAreaByteBase | 0x10, 0 ),
			WarningRecord6Reg( g_MonitorAreaByteBase | 0x11, 0 ),
			WarningRecord7Reg( g_MonitorAreaByteBase | 0x12, 0 ),
			WarningRecord8Reg( g_MonitorAreaByteBase | 0x13, 0 ),
			WarningRecord9Reg( g_MonitorAreaByteBase | 0x14, 0 ),
			WarningRecord10Reg( g_MonitorAreaByteBase | 0x15, 0 ),
			PresentSelectOpDataNumberReg( g_MonitorAreaByteBase | 0x16, 0 ),
			PresentOpDataNumberReg( g_MonitorAreaByteBase | 0x17, 0 ),
			CommandPosReg( g_MonitorAreaByteBase | 0x18, 0 ),
			CommandSpeedReg( g_MonitorAreaByteBase | 0x1C, 0 ),
			EncodeCounterReg( g_MonitorAreaByteBase | 0x1E, 0 ),
			RemDwellTimeReg( g_MonitorAreaByteBase | 0x24, 0 ),
			IOStatusReg( g_MonitorAreaByteBase | 0x26, static_cast< IOStatusBitsEnum32Bit::stdEnum >(0) ),
			CommErrCodeReg( g_MonitorAreaByteBase | 0x28, 0 ),
			CommErrCodeRecord1Reg( g_MonitorAreaByteBase | 0x29, 0 ),
			CommErrCodeRecord2Reg( g_MonitorAreaByteBase | 0x2A, 0 ),
			CommErrCodeRecord3Reg( g_MonitorAreaByteBase | 0x2B, 0 ),
			CommErrCodeRecord4Reg( g_MonitorAreaByteBase | 0x2C, 0 ),
			CommErrCodeRecord5Reg( g_MonitorAreaByteBase | 0x2D, 0 ),
			CommErrCodeRecord6Reg( g_MonitorAreaByteBase | 0x2E, 0 ),
			CommErrCodeRecord7Reg( g_MonitorAreaByteBase | 0x2F, 0 ),
			CommErrCodeRecord8Reg( g_MonitorAreaByteBase | 0x30, 0 ),
			CommErrCodeRecord9Reg( g_MonitorAreaByteBase | 0x31, 0 ),
			CommErrCodeRecord10Reg( g_MonitorAreaByteBase | 0x32, 0 ),
			DriverStatusReg( g_MonitorAreaByteBase | 0x33, static_cast< driverStatusBitsEnum32Bit::stdEnum >(0) ),
			//Parameter Area Registers
			startInputModeReg( g_ParameterAreaByteBase | 0x00, inputTypeEnum16Bit::IO ),
			IOStopInputReg( g_ParameterAreaByteBase | 0x01, genericEnableEnum16Bit::Enable ),
			stopActionReg( g_ParameterAreaByteBase | 0x02, stopActionExpandedTypesEnum16Bit::Decelerate ),
			stopContactConfigReg( g_ParameterAreaByteBase | 0x03, switchActuationEnum16Bit::Brake ),
			conLogCongifReg( g_ParameterAreaByteBase | 0x04, inversionTypeEnum16Bit::Noninverted ),
			out1SignalModeReg( g_ParameterAreaByteBase | 0x06, signalModesEnum16Bit::Area ),
			out2SignalModeReg( g_ParameterAreaByteBase | 0x07, signalModesEnum16Bit::Ready ),
			out3SignalModeReg( g_ParameterAreaByteBase | 0x08, signalModesEnum16Bit::WNG ),
			out4SignalModeReg( g_ParameterAreaByteBase | 0x09, signalModesEnum16Bit::HomeP ),
			homePPresetSwitchingReg( g_ParameterAreaByteBase | 0x0A, homeOptionsEnum16Bit::Home ),
			motorExciteInputModeReg( g_ParameterAreaByteBase | 0x0B, inputTypeEnum16Bit::IO ),
			homeFwdRvsInputModeReg( g_ParameterAreaByteBase | 0x0C, inputTypeEnum16Bit::IO ),
			dataNumInputModeReg( g_ParameterAreaByteBase | 0x0D, inputTypeEnum16Bit::IO ),
			awoContactConfigReg( g_ParameterAreaByteBase | 0x0E, switchActuationEnum16Bit::Make ),
			hardwareOvertravelDetectReg( g_ParameterAreaByteBase | 0x0F, genericEnableEnum16Bit::Enable ),
			lsContactConfigReg( g_ParameterAreaByteBase | 0x10, switchActuationEnum16Bit::Make ),
			homesContactConfigReg( g_ParameterAreaByteBase | 0x11, switchActuationEnum16Bit::Make ),
			slitContactConfigReg( g_ParameterAreaByteBase | 0x12, switchActuationEnum16Bit::Make ),
			overTravelActionReg( g_ParameterAreaByteBase | 0x13, stopActionTypeEnum16Bit::Decelerate ),
			presetPosParamReg( g_ParameterAreaByteBase | 0x14, 0x0080 ),
			area1Reg( g_ParameterAreaByteBase | 0x16, 0x0080 ),
			area2Reg( g_ParameterAreaByteBase | 0x18, 0x0080 ),
			encoderCountPresetReg( g_ParameterAreaByteBase | 0x1C, 0x0080 ),
			operatingCurrentReg( g_ParameterAreaByteBase | 0x1E, 0x0064 ),
			standstillCurrentReg( g_ParameterAreaByteBase | 0x1F, 0x0032 ),
			commonAccelRateReg( g_ParameterAreaByteBase | 0x24, 0x0000 ),
			commonDecelRateReg( g_ParameterAreaByteBase | 0x26, 0x0000 ),
			startSpeedReg( g_ParameterAreaByteBase | 0x28, 0x0000 ),
			jogOperatingSpeedReg( g_ParameterAreaByteBase | 0x30, 0x0000 ),
			jogAccelRateReg( g_ParameterAreaByteBase | 0x32, 0x0000 ),
			jogStartSpeedReg( g_ParameterAreaByteBase | 0x34, 0x0000 ),
			accelRateTypeReg( g_ParameterAreaByteBase | 0x36, accelRateTypeEnum16Bit::Common ),
			homeSeekModeReg( g_ParameterAreaByteBase | 0x37, numSensorsEnum16Bit::Sensors3 ),
			homeSeekOperatingSpeedReg( g_ParameterAreaByteBase | 0x3A, 0x0000 ),
			homeSeekAccelRateReg( g_ParameterAreaByteBase | 0x3C, 0x0000 ),
			homeSeekStartSpeedReg( g_ParameterAreaByteBase | 0x3E, 0x0000 ),
			homeSeekPosOffsetReg( g_ParameterAreaByteBase | 0x40, 0x0080 ),
			homeSeekStartDirReg( g_ParameterAreaByteBase | 0x42, posNegTypeEnum16Bit::Positive ),
			homeSeekSlitDetectionReg( g_ParameterAreaByteBase | 0x43, genericEnableEnum16Bit::Disable ),
			homeSeekTIMDectectionReg( g_ParameterAreaByteBase | 0x44, timDetectHomeSeekingTypeEnum16Bit::Disable ),
			_2SensorModeBackStepsReg( g_ParameterAreaByteBase | 0x45, 0x00C8 ),
			stepOutDetectionBandReg( g_ParameterAreaByteBase | 0x46, 0x0048 ),
			overVoltageWarningReg( g_ParameterAreaByteBase | 0x4A, 0x0136 ),
			overHeatWarningReg( g_ParameterAreaByteBase | 0x4C, 85 ),
			softwareOverTravelReg( g_ParameterAreaByteBase | 0x52, genericEnableEnum16Bit::Enable ),
			positiveSotwareLimitReg( g_ParameterAreaByteBase | 0x54, 0x0100 ),
			negativeSotwareLimitReg( g_ParameterAreaByteBase | 0x56, 0x0000 ),
			dataSetterDisplayModeReg( g_ParameterAreaByteBase | 0x58, bitSignTypeEnum16Bit::Signed ),
			dataSetterEditModeReg( g_ParameterAreaByteBase | 0x59, genericEnableEnum16Bit::Enable ),
			communicationTimeOutActionReg( g_ParameterAreaByteBase | 0x5B, stopActionTypeEnum16Bit::Decelerate ),
			stepOutDetectionActionReg( g_ParameterAreaByteBase | 0x5C, stepOutActionTypesEnum16Bit::NoOperation ),
			//System Parameters Registers
			motorRotationDirReg( g_SystemParameterByteBase | 0x0E, spinDirectionsEnum16Bit::ClockWise ),
			stepOutDetectionReg( g_SystemParameterByteBase | 0x10, genericEnableEnum16Bit::Disable ),
			//Multiple Possible StepAngle Bases For a Single Controller
			motorStepAngleReg_36( g_SystemParameterByteBase | 0x11, base_36DegStepAnglesEnum16Bit::_36deg ),
			motorStepAngleReg_72( g_SystemParameterByteBase | 0x11, base_72DegStepAnglesEnum16Bit::_72deg ),
			encoderGearAReg( g_SystemParameterByteBase | 0x12, 0 ),
			encoderGearBReg( g_SystemParameterByteBase | 0x14, 0 ),
			communicationAxisNumReg( g_SystemParameterByteBase | 0x16, 15 ),
			communicationParityReg( g_SystemParameterByteBase | 0x18, parityTypeEnum16Bit::Even ),
			communicationStopBitReg( g_SystemParameterByteBase | 0x19, stopBitsEnum16Bit::Bits1 ),
			transmissionWaitTimeReg( g_SystemParameterByteBase | 0x1A, 10 ),
			communicationTimeOutReg( g_SystemParameterByteBase | 0x1B, 0 ),
			communicationErrorAlarmReg( g_SystemParameterByteBase | 0x1C, 3 ),
			isInternalProc_(false)
			{
				/********************************************************
				*	Register Base Angle Registers to Base Angle Register Map 
				*	For Selection of Multiple BaseAngles
				********************************************************/
				//Produce Ordered Double Lists That Equate to Enumerated Types in Registers
				//Store These in Maps On this Level
				static const double base_72DegOrderedNumbers[] = {.72, .36, .288, .18, .144, .09, .072, .036, .0288, .018, .0144, .009, .0072, .00576, .0036, .00288 };
				static const double base_36DegOrderedNumbers[] = { .36, .18, .144, .09, .072, .045, .036, .018, .0144, .009, .0072, .0045, .0036, .0028, .0018, .00144 };

				degOptions_36_.assign( base_36DegOrderedNumbers, base_36DegOrderedNumbers + sizeof(base_36DegOrderedNumbers)/sizeof(base_36DegOrderedNumbers[0]) );
				degOptions_72_.assign( base_72DegOrderedNumbers, base_72DegOrderedNumbers + sizeof(base_72DegOrderedNumbers)/sizeof(base_72DegOrderedNumbers[0]) );

				_36DegBaseStepAnglesTypeMap_ = base_36DegStepAnglesEnum16Bit::mapTypedValuesToEnum<double>( degOptions_36_ );
				_72DegBaseStepAnglesTypeMap_ = base_72DegStepAnglesEnum16Bit::mapTypedValuesToEnum<double>( degOptions_72_ );

				RegisterBaseAngleRegToNumber( .36, &motorStepAngleReg_36 );
				RegisterBaseAngleRegToNumber( .72, &motorStepAngleReg_72 );
				
				/*********************************************************
				*	Register Other Defined Registers For Address Queries
				*   Use GetRegisterByAddress( addr ) Afterwards
				**********************************************************/
				RegisterNewRegisterAddress( &dwellTimeReg );
				RegisterNewRegisterAddress( &seqPosReg );
				RegisterNewRegisterAddress( &opModeReg );
				RegisterNewRegisterAddress( &posModeReg );
				RegisterNewRegisterAddress( &decelRateReg );
				RegisterNewRegisterAddress( &accelRateReg );
				RegisterNewRegisterAddress( &operatingSpeedReg );
				RegisterNewRegisterAddress( &posReg );
				RegisterNewRegisterAddress( &cmd1Reg );
				RegisterNewRegisterAddress( &cmd2Reg );
				RegisterNewRegisterAddress( &status1Reg );
				RegisterNewRegisterAddress( &status2Reg );
				//Maintenance Area Registers
				RegisterNewRegisterAddress( &resetAlarmReg );
				RegisterNewRegisterAddress( &clearAlarmRecReg );
				RegisterNewRegisterAddress( &clearWarningRecReg );
				RegisterNewRegisterAddress( &batchNVReadReg );
				RegisterNewRegisterAddress( &batchNVWriteReg );
				RegisterNewRegisterAddress( &allDataInitReg );
				RegisterNewRegisterAddress( &presetPosReg );
				RegisterNewRegisterAddress( &clearComErrRecReg );
				RegisterNewRegisterAddress( &presetEncoderReg );
				RegisterNewRegisterAddress( &clearCounterReg );
				RegisterNewRegisterAddress( &opDataInitReg );
				RegisterNewRegisterAddress( &appParamInitReg );
				RegisterNewRegisterAddress( &sysParamInitReg );
				//Monitor Area Registers
				RegisterNewRegisterAddress( &presentAlarmReg );
				RegisterNewRegisterAddress( &alarmRecord1Reg );
				RegisterNewRegisterAddress( &alarmRecord2Reg );
				RegisterNewRegisterAddress( &alarmRecord3Reg );
				RegisterNewRegisterAddress( &alarmRecord4Reg );
				RegisterNewRegisterAddress( &alarmRecord5Reg );
				RegisterNewRegisterAddress( &alarmRecord6Reg );
				RegisterNewRegisterAddress( &alarmRecord7Reg );
				RegisterNewRegisterAddress( &alarmRecord8Reg );
				RegisterNewRegisterAddress( &alarmRecord9Reg );
				RegisterNewRegisterAddress( &alarmRecord10Reg );
				RegisterNewRegisterAddress( &presentWarningReg );
				RegisterNewRegisterAddress( &WarningRecord1Reg );
				RegisterNewRegisterAddress( &WarningRecord2Reg );
				RegisterNewRegisterAddress( &WarningRecord3Reg );
				RegisterNewRegisterAddress( &WarningRecord4Reg );
				RegisterNewRegisterAddress( &WarningRecord5Reg );
				RegisterNewRegisterAddress( &WarningRecord6Reg );
				RegisterNewRegisterAddress( &WarningRecord7Reg );
				RegisterNewRegisterAddress( &WarningRecord8Reg );
				RegisterNewRegisterAddress( &WarningRecord9Reg );
				RegisterNewRegisterAddress( &WarningRecord10Reg );
				RegisterNewRegisterAddress( &PresentSelectOpDataNumberReg );
				RegisterNewRegisterAddress( &PresentOpDataNumberReg );
				RegisterNewRegisterAddress( &CommandPosReg );
				RegisterNewRegisterAddress( &CommandSpeedReg );
				RegisterNewRegisterAddress( &EncodeCounterReg );
				RegisterNewRegisterAddress( &RemDwellTimeReg );
				RegisterNewRegisterAddress( &IOStatusReg );
				RegisterNewRegisterAddress( &CommErrCodeReg );
				RegisterNewRegisterAddress( &CommErrCodeRecord1Reg );
				RegisterNewRegisterAddress( &CommErrCodeRecord2Reg );
				RegisterNewRegisterAddress( &CommErrCodeRecord3Reg );
				RegisterNewRegisterAddress( &CommErrCodeRecord4Reg );
				RegisterNewRegisterAddress( &CommErrCodeRecord5Reg );
				RegisterNewRegisterAddress( &CommErrCodeRecord6Reg );
				RegisterNewRegisterAddress( &CommErrCodeRecord7Reg );
				RegisterNewRegisterAddress( &CommErrCodeRecord8Reg );
				RegisterNewRegisterAddress( &CommErrCodeRecord9Reg );
				RegisterNewRegisterAddress( &CommErrCodeRecord10Reg );
				RegisterNewRegisterAddress( &DriverStatusReg );
				//Parameter Area Registers
				RegisterNewRegisterAddress( &startInputModeReg );
				RegisterNewRegisterAddress( &IOStopInputReg );
				RegisterNewRegisterAddress( &stopActionReg );
				RegisterNewRegisterAddress( &stopContactConfigReg );
				RegisterNewRegisterAddress( &conLogCongifReg );
				RegisterNewRegisterAddress( &out1SignalModeReg );
				RegisterNewRegisterAddress( &out2SignalModeReg );
				RegisterNewRegisterAddress( &out3SignalModeReg );
				RegisterNewRegisterAddress( &out4SignalModeReg );
				RegisterNewRegisterAddress( &homePPresetSwitchingReg );
				RegisterNewRegisterAddress( &motorExciteInputModeReg );
				RegisterNewRegisterAddress( &homeFwdRvsInputModeReg );
				RegisterNewRegisterAddress( &dataNumInputModeReg );
				RegisterNewRegisterAddress( &awoContactConfigReg );
				RegisterNewRegisterAddress( &hardwareOvertravelDetectReg );
				RegisterNewRegisterAddress( &lsContactConfigReg );
				RegisterNewRegisterAddress( &homesContactConfigReg );
				RegisterNewRegisterAddress( &slitContactConfigReg );
				RegisterNewRegisterAddress( &overTravelActionReg );
				RegisterNewRegisterAddress( &presetPosParamReg );
				RegisterNewRegisterAddress( &area1Reg );
				RegisterNewRegisterAddress( &area2Reg );
				RegisterNewRegisterAddress( &encoderCountPresetReg );
				RegisterNewRegisterAddress( &operatingCurrentReg );
				RegisterNewRegisterAddress( &standstillCurrentReg );
				RegisterNewRegisterAddress( &commonAccelRateReg );
				RegisterNewRegisterAddress( &commonDecelRateReg );
				RegisterNewRegisterAddress( &startSpeedReg );
				RegisterNewRegisterAddress( &jogOperatingSpeedReg );
				RegisterNewRegisterAddress( &jogAccelRateReg );
				RegisterNewRegisterAddress( &jogStartSpeedReg );
				RegisterNewRegisterAddress( &accelRateTypeReg );
				RegisterNewRegisterAddress( &homeSeekModeReg );
				RegisterNewRegisterAddress( &homeSeekOperatingSpeedReg );
				RegisterNewRegisterAddress( &homeSeekAccelRateReg );
				RegisterNewRegisterAddress( &homeSeekStartSpeedReg );
				RegisterNewRegisterAddress( &homeSeekPosOffsetReg );
				RegisterNewRegisterAddress( &homeSeekStartDirReg );
				RegisterNewRegisterAddress( &homeSeekSlitDetectionReg );
				RegisterNewRegisterAddress( &homeSeekTIMDectectionReg );
				RegisterNewRegisterAddress( &_2SensorModeBackStepsReg );
				RegisterNewRegisterAddress( &stepOutDetectionBandReg );
				RegisterNewRegisterAddress( &overVoltageWarningReg );
				RegisterNewRegisterAddress( &overHeatWarningReg );
				RegisterNewRegisterAddress( &softwareOverTravelReg );
				RegisterNewRegisterAddress( &positiveSotwareLimitReg );
				RegisterNewRegisterAddress( &negativeSotwareLimitReg );
				RegisterNewRegisterAddress( &dataSetterDisplayModeReg );
				RegisterNewRegisterAddress( &dataSetterEditModeReg );
				RegisterNewRegisterAddress( &communicationTimeOutActionReg );
				RegisterNewRegisterAddress( &stepOutDetectionActionReg );
				//System Parameters Registers
				RegisterNewRegisterAddress( &motorRotationDirReg );
				RegisterNewRegisterAddress( &stepOutDetectionReg );
				//Base Step Registers Are Registered Up Above
				RegisterNewRegisterAddress( &encoderGearAReg );
				RegisterNewRegisterAddress( &encoderGearBReg );
				RegisterNewRegisterAddress( &communicationAxisNumReg );
				RegisterNewRegisterAddress( &communicationParityReg );
				RegisterNewRegisterAddress( &communicationStopBitReg );
				RegisterNewRegisterAddress( &transmissionWaitTimeReg );
				RegisterNewRegisterAddress( &communicationTimeOutReg );
				RegisterNewRegisterAddress( &communicationErrorAlarmReg );
		
			};

		~OrientalCRK525MAKD(){};

		/* Virtual Implementation - Sets the current BaseAnglePartition from the one passed
		*	Note:  This will be hard-coded for every new controller
		*   @param baseAnglePartition - number that corresponds to an enumerated Value in the register
		*   Returns - 0 if successful or errCode otherwise
		*/
		int SetBaseAnglePartitionImpl( double baseAnglePartition );

		/* Virtual Implementation - Returns the current BaseAngle Partition From the CurrentBaseAngleRegister
		*	Note: This Function Reads The Current Selected BaseAngle Register Value
		*         It is the programmer's burden to make sure the baseAngleRegister is not subject to external manipulation
		*	Returns - Current Base Angle Partition correllated to GetBaseAnglePartitionOptions() or number > 360 if error
		*/
		double ReadCurrentBaseAnglePartition( void );

		/* Templated Function For Looking up BaseAnglePartitions Based on the Register Binary Value
		*   @param registerValue - value pulled from the BaseAnglePartition Register of its ::EnumBaseType
		*   @param std::map< double, EnumBaseType> - BaseAnglePartition Maps Defined in Constructor After Registering BaseAngleRegisters
		*/
		template< typename EnumBaseType >
		double matchBaseAnglePartitionRegisterValueToNumber( EnumBaseType registerValue, std::map< double, EnumBaseType >& partitionMap )
		{

			std::map< double, EnumBaseType >::iterator it;
			
			for( it = partitionMap.begin(); it != partitionMap.end(); ++it )
			{
				if( it->second == registerValue )
				{
					return it->first;
				}
			}

			return 400;

		}

		/* Virtual - Returns a Standard Vector of double values indicative of all Base Angle Partitions for a given BaseAngle
		*   Note: This will be hard-coded for every new controller
		*   Note:  Used to display all available step Increments for a selected Base Angle
		*   @param baseAngleOption - the current BaseAngle Being Queried
		*   Returns - vector of doubles or an empty vector if there is a problem
		*/
		std::vector<double> GetBaseAnglePartitionOptions( double baseAngleOption ); 

		/* Virtual Implementation - Initialize Physical Controller Via Serial Commands
		*    Note:  NO Serial Communication should be done inside of controller constructor, due to user ability to correct errors later
		*    Returns - 0 or errCode otherwise
		*/
		int InitializePhysicalController();

		/* Virtual Function To Send a testConnection Packet Request to verify working Order
		*	Implements Child-level overload of testConnection( uint16_t )
		*   Returns - 0 if successful or errCode otherwise
		*/
		int testConnection( void ) { return testConnection( 0x1234 ); }

		/*  @Overload to virtual function testConnection
		*   Sends a testConnection Packet Request to the Controller to verify Working Order
		*		@param testValue - May be any 2 Byte number to test Byte integrity
		*		Returns - 0 if successful, or errCode if not 
		*/
		int testConnection( uint16_t testValue );
		
		/* Writes a Serialized Value Through the passed SerialCommFunction
		*   @param serializedValue[] = Byte Array Passed From MMDevice Object for desired value
		*   @param serializedValueLen =  Number of Defined Bytes in the array
		*   @param valueIsBigEndian = Identifies The Way the Value was packaged into Bytes before being passed
		*   @param *SerialCommFuncPtr = Pointer to the Desired Serial Communication Function Being Used
		*/
		int WritePosBuffer( unsigned char serializedValue[], int serializedValueLen, bool valueIsBigEndian, AbstractControllerInterface::SerialCommFuncPtr serialCommFuncPtr );
		
		/* Reads Position From Serial Request 
		*   @param readBuffer[] - Buffer to Return BigEndian Byte Ordered Position Value
		*   @param bufferSize - Size of the Buffer Passed
		*   Returns - Number of Bytes Defined in Buffer or -1 if buffer size if too small
		*/
		int ReadPosBuffer( unsigned char readBuffer[], int bufferSize ){ return 0; };

		/*
		* Logic To Determine the Expected Header Length of the Response Message
		*  @param txMsgBuffer[] = byte message that was transmitted
		*  @param txMsgLen = length of defined Transmit Message, used to detect undefined Index
		*  Return = Number of Bytes in the Header or -1
		*/
		int headerLengthLookup( const unsigned char txMsgBuffer[], const int txMsgLen);

		/*
		* SameAddres - Check to ensure that the address in the header is the same as the controller recieving it
		*  @param rxHeader[] - byte array of the header recieved from RX
		*  @param headerLen - length of the header recieved
		*  Return - true (sameAddress) or false ( not the same address/header passed is corrupted )
		*
		*Note:  headerLen is expected to match the length of the header not the whole message.  
		*It is typically used after using a headerLengthLookup to sample the only the expected header
		*Other uses would require the user to pass only the determined header Length is the whole Msg was sent
		*/
		bool sameAddress( const unsigned char rxHeader[], const int headerLen );

		/*
		* DataLengthLookup - Takes The Sampled Header and Parses it to Determine the length of the data packet expected
		*  @param rxHeader[] - byte array of the header recieved from RX
		*  @param headerLen - length of the header recieved
		*  Returns - number of remaining bytes after header to read or -1 if there was an error in the dataspecifier
		*/
		int dataLengthLookup( const unsigned char rxHeader[], const int headerLen );

		/*
		* ParseData - Verifies Each Recieved Packet and Directs it to a Parsing Action
		*  @param txMsgBuffer[] = byte message that was transmitted 
		*  @param txMsgLen = byte length of defined Trasmit Message, used to detect undefined Index
		*  @param rxMsgBufer[] = byte message that was recived (Header + Data)
		*  @rxMsgLen = byte length of recieved message
		*/
		int parseData( const unsigned char txMsgBuffer[], const int txMsgBufLen, const unsigned char rxBuffer[], const int rxBufLen );

		/*Virtual Implementation - Returns a code corresponding to Whether or not the Motor is Currently Running
		*  Note:  Should Implement A ThreadGuard Due to the Accessing of Any Information In This Function
		*         The protected member From AbstractControllerInterface busyLock_ may be used for this purpose
		*         It is the burden of the Programmer to ThreadLock AnyOther Functions Pertaining to Values Used in This Implemtation
		*  Returns - Code for Non-Busy, Busy, or errCode Otherwise
		*/
		int IsMotorBusy( void );
		
		/* Virtual Implementation - Permit the Controller To Do Another Step Process
		*    Note:  This function is necessary for enabling a write process after the motor has already spun
		*    Note:  Is intended to be Used in ControllerStatusMonitorThread, but may find other use
		*	 Returns - 0 on success or errCode otherwise
		*/
		int PermitPosWriteImpl( void );

		/* Virtual Implementation - Gets the current energized state as a result of the inheriting controller logic and hardware communication
		*	Note:  This will be hard-coded for every new type of controller
		*	Returns - boolean value if successful  
		*   throws - SerialHardwareCommunication Failure - used to determine if the value should be stored in isHardwareEnergized_
		*/
		bool GetRestingHardwareEnergizedImpl( );

	    /* Virtual - Sets the current energized state in the inheriting controller logic and hardware communication
		*	Note:  This will be hard-coded for every new type of controller
		*	  @param energyOn - whether or not the motor should be energized (true = On)
		*   Returns - 0 if successful or errCode otherwise  (errCode is used to determine if the intention should be stored in isHardwareEnergized_)
		*   Throws - Exceptions for Communication Errors
		*/
		void SetRestingHardwareEnergizedImpl( bool energyOn );

		/* Virtual - Perform Necessary Child-level and Serial Commands to Permit the Controller To Do Any Other Busy-Dependent Processes
		*    Note:  Is intended to be Used in ControllerStatusMonitorThread, but may find other use
		*	 Note2: Should Implement A ThreadGuard Due to the Accessing of Any Member Variables In This Function
		* 	   The protected member From AbstractControllerInterface busyLock_ may be used for this purpose
		*	   It is the burden of the Programmer to ThreadLock AnyOther Functions Pertaining to Members Used in This Implementation
		*	 
		*    Returns - 0 on success or errCode otherwise
		*/
		int PermitBusyDependentProcessesImpl( void );

		/* Vitrual Implementation -Writes a Serialized Value For Step Speed to Motor Step Speed Register
		*   Note:  Assumes SerializedSpeedValue is BigEndian and expects WriteStepSpeed() to be public Implementation
		*   @param serializedSpeedValue[] = Byte Array Passed From MMDevice Object for desired value
		*   @param serializedValueLen =  Number of Defined Bytes in the array
		*/
		int WriteStepSpeedBuffer( unsigned char serializedSpeedValue[], unsigned int speedValueLen ); 

	private:

		/* Generates and Appends the CRC Check Value to a byteBuffer to Be Transmitted
		*   @param byteBuffer[] - Buffer of Defined Bytes To Be Transmitted
		*   @param bufferSize - Size of the Buffer Containing Bytes
		*   @param numDefinedBytes - Number of Defined Bytes that are in the byteBuffer
		*   Returns - -1 if bufferSize Mismatch or Size (in Bytes) of CRC Value
		*/
		int appendCRCCheckValue( unsigned char byteBuffer[], int bufferSize, int numDefinedBytes );

		/* Compute CRC Value For Given Packet of Number of Bytes
		*   @param bytes[] - Array of Bytes to be Used in the CRC Check
		*   @param numBytes - Number of Defined Bytes in the Array
		*   Returns - CRC Value (No Error Checks)
		*/
		uint32_t crcCompute( const unsigned char bytes[] , const int numBytes );

		//Functions for Various RS-485 Function Modes

		/* Write A Given Value Via Serial Wire to the Register on the controller
		*	Note: If a Register's size is greater than baseRegisterByteSize_, will throw an exception
		*	@param &reg - Reference to the register to be written
		*	@param value - Value of Corresponding Register Type to be written 
		*	Return - 0 if okay, error codes otherwise
		*/
		template< typename regValType >
		int serialWriteSingleRegister( AbstractRegisterBase& reg, regValType value, AbstractControllerInterface::SerialCommFuncPtr serialCommFuncPtr = nullptr, bool debug = false )
			{

				static const int packetByteSize = 8;
				unsigned char packet[packetByteSize];
				int currentByte = 0;
				int valueIndex;
				int numBytesWritten;
				int errCode = 0;

				//Use nullptr as an indicator to retrieve stored serialCommPtr
				if( serialCommFuncPtr == nullptr )
				{
					serialCommFuncPtr = retrieveSerialCommFuncPtr();
				}

				//Verify Register Is Single Register
				if( reg.getRegisterByteSize() != baseRegisterByteSize_ )
				{
					//Register Cannot Be MultiRegister - Programmers Burden and exception
					return 1;
				}

				AbstractControllerInterfaceFactory::LogMessage(  "Get Address" );
				//Controller Slave Address
				numBytesWritten = getAddressBuffer( packet, 1 );
				if( numBytesWritten == -1 )  
				{
					//The Buffer must have been too small
					return -1;
				}
				currentByte +=numBytesWritten;

				//Function Code
				packet[currentByte] =  functionCodes::registerWrite;
				currentByte += sizeof( functionCodes::registerWrite );

				AbstractControllerInterfaceFactory::LogMessage(  "Get Register Address" );
				//Register Address
				numBytesWritten = reg.readAddress( &packet[currentByte],  packetByteSize - currentByte );
				if( numBytesWritten == -1 ) 
				{
					//The Buffer Must have Been Too small
					return -1;
				}
				currentByte += numBytesWritten;

				AbstractControllerInterfaceFactory::LogMessage(  "Serialize Value" );
				//Serialize Value
				//Complete Type Work Around For BigEndianness
				if( reg.isBigEndianCheck() )
				{
					numBytesWritten = ReadWrite< regValType, true >::read( &packet[currentByte], packetByteSize - currentByte, value );
				}
				else
				{
					numBytesWritten = ReadWrite< regValType, false >::read( &packet[currentByte], packetByteSize - currentByte, value );
				}
				if( numBytesWritten == -1 )
				{
					//The Buffer Must have Been Too small
					return -1;
				}
				valueIndex = currentByte;  // Save for Value Referencing later
				//Print the Value
				if( debug )
				{
					ostringstream es;
					es << "The Value Packet is " << (int) packet[ currentByte ] << ", " << (int) (packet[currentByte+ 1]);
					AbstractControllerInterfaceFactory::LogMessage( es.str() );
				}

				AbstractControllerInterfaceFactory::LogMessage(  "Acceptable Range Test" );
				//Check to see if value is inside of Acceptable Range
				if( ( errCode = reg.testSerialDataConformance( &packet[valueIndex], sizeof( regValType ) ) ) != 1 )
				{
					if( errCode == -1 )
					{
						//The buffer was too small
						AbstractControllerInterfaceFactory::LogMessage(  "Small Buffer" );
						return -1;
					}
					if( errCode == 0 )
					{
						//Value outside of acceptable Range
						AbstractControllerInterfaceFactory::LogMessage(  "Outside Range:  " );
						return -2;
					}
				}
				currentByte += numBytesWritten;

				AbstractControllerInterfaceFactory::LogMessage(  "Append CRC" );
				//Add CRCCheckValue
				numBytesWritten = appendCRCCheckValue( packet, sizeof(packet), currentByte );
				if( numBytesWritten == -1 )
				{
					//The Buffer Must have been too small
					return -1;
				}
				currentByte += numBytesWritten;

				//Communicate With Controller
				if( ( errCode = ( retrieveSerialCommHubPtr()->*serialCommFuncPtr )( packet, currentByte, this, false ) ) != DEVICE_OK )
				{
					//Register Was not Written
					AbstractControllerInterfaceFactory::LogMessage(  CDeviceUtils::ConvertToString( errCode ) );
					return errCode;
				}

				return DEVICE_OK;

		}

		/* Write Multiple Consecutive Registers From A PreBuilt Value Array
		*   @param &startReg - Reference to the register with the first address
		*   @param numRegs - number of registers to write in total
		*   @param valueArray[] - Byte array of already parsed Values (most likely using ReadWrite< decltype( register), isBigEndian::read()
		*							Note:  Each Array Value is assumed to have been placed in the correct endian order
		*   @param valueArraySize - Size of the Defined Bytes in the Array to be passed along
		*   Returns - 0 on completion or errorCodes otherwise
		*/
		int serialWriteMultiRegister( AbstractRegisterBase &startReg, unsigned int numRegs, unsigned char valueArray[], int valueArraySize );

		/* Reads A Number of Consecutive Registers into the controller Register Members
		*   @param startReg - Pointer to the register with the first address
		*   @param numRegs - number of registers to read in total
		*   @param serialCommFuncPtr - Function Pointer to a Given Serial Communication Function
		*		Note: if serialCommFuncPtr is nullptr, uses default SerialCommFunction as returned from retrieveSerialCommFuncPtr()
		*   Returns - 0 on completion or errorCodes otherwise
		*/
		int ReadRegisters( AbstractRegisterBase* startReg, unsigned int numRegs, AbstractControllerInterface::SerialCommFuncPtr serialCommFuncPtr = nullptr )
		{

				static const int packetByteSize = 8;
				unsigned char packet[packetByteSize];
				int currentByte = 0;
				int numBytesWritten;
				int errCode = 0;
				baseAddressType addr;

				//Use nullptr as an indicator to retrieve stored serialCommPtr
				if( serialCommFuncPtr == nullptr )
				{
					serialCommFuncPtr = retrieveSerialCommFuncPtr();
				}

				AbstractControllerInterfaceFactory::LogMessage(  "Get Address" );
				//Controller Slave Address
				numBytesWritten = getAddressBuffer( packet, 1 );
				if( numBytesWritten == -1 )  
				{
					//The Buffer must have been too small
					return -1;
				}
				currentByte +=numBytesWritten;

				//Function Code
				packet[currentByte] =  functionCodes::registerRead;
				currentByte += sizeof( functionCodes::registerRead );

				AbstractControllerInterfaceFactory::LogMessage( "Get Register Address" );
				//Register Address
				numBytesWritten = startReg->readAddress( &packet[currentByte],  packetByteSize - currentByte );
				if( numBytesWritten == -1 ) 
				{
					//The Buffer Must have Been Too small
					return -1;
				}
				//Incomplete Type Workaround
				if( startReg->isBigEndianCheck() )
				{
					ReadWrite< baseAddressType, true >::write( &packet[currentByte], numBytesWritten, addr );
				}
				else
				{
					ReadWrite< baseAddressType, false >::write( &packet[currentByte], numBytesWritten, addr );
				}
				currentByte += numBytesWritten;

				AbstractControllerInterfaceFactory::LogMessage( "Counting Num Registers This is Changed" );

				//Check to Make Sure We Are not writing a partial Register
				AbstractRegisterBase * reg = startReg;
				int i;
				for( i = 0; i < numRegs; )
				{
					i += reg->getRegisterByteSize()/baseRegisterByteSize_;
					reg = GetRegisterByAddress( addr + 1 );
					AbstractControllerInterfaceFactory::LogMessage( "In for loop " );
					if( reg == nullptr )
					{
						if( i != numRegs )
						{
							//We have Tried to Read From an Unavailable Register
							AbstractControllerInterfaceFactory::LogMessage( "Unavailable Register To Read" );
							return 1;
						}
					}

				}

				std::ostringstream os;
				os << "i is: " << i << "\nnumRegs is: " << numRegs;
				AbstractControllerInterfaceFactory::LogMessage( os.str() );

				if( i != numRegs )
				{
					//Error:  We Are Reading a Fraction of a Register
					AbstractControllerInterfaceFactory::LogMessage( "Reading Portion of Register Only" );
					return 1;
				}

				//Add Number of Registers to packet
				numBytesWritten = ReadWrite< uint16_t, true >::read( &packet[currentByte], currentByte, numRegs );
				if( numBytesWritten == -1 )
				{
					return -1;
				}
				currentByte += numBytesWritten;

				AbstractControllerInterfaceFactory::LogMessage(  "Append CRC" );
				//Add CRCCheckValue
				numBytesWritten = appendCRCCheckValue( packet, sizeof(packet), currentByte );
				if( numBytesWritten == -1 )
				{
					//The Buffer Must have been too small
					return -1;
				}
				currentByte += numBytesWritten;

				//Communicate With Controller
				if( ( errCode = ( retrieveSerialCommHubPtr()->*serialCommFuncPtr )( packet, currentByte, this, false ) ) != DEVICE_OK )
				{
					//Register Was not Written
					AbstractControllerInterfaceFactory::LogMessage(  CDeviceUtils::ConvertToString( errCode ) );
					return errCode;
				}

				return DEVICE_OK;

		}

		//Functions for Various Function Command Response Parses
		int onMultipleRegisterWriteResponse( const unsigned char txMsgBuffer[], const unsigned int txMsgBufLen, const unsigned char rxBuffer[], const unsigned int rxBufLen );
		int onSingleRegisterWriteResponse( const unsigned char txMsgBuffer[], const unsigned int txMsgBufLen, const unsigned char rxBuffer[], const unsigned int rxBufLen );
		int onRegisterRead( const unsigned char txMsgBuffer[], const unsigned int txMsgBufLen, const unsigned char rxBuffer[], const unsigned int rxBufLen );

		//Numerical Values For Different Base Angles
		std::vector<double> degOptions_72_;
		std::vector<double> degOptions_36_;

		std::map< double, base_36DegStepAnglesEnum16Bit::EnumBaseType > _36DegBaseStepAnglesTypeMap_;
		std::map< double, base_72DegStepAnglesEnum16Bit::EnumBaseType > _72DegBaseStepAnglesTypeMap_;

		//Boolean For Controller Processing State
		bool isInternalProc_;
};



#endif  //_ORIENTALCRK525MAKD_