
#ifndef _ORIENTALDEVICECONSTANTS_
#define _ORIENTALDEVICECONSTANTS_

const char* const g_OrientalHubName = "OrientalStepperAdjuster";
const char* const g_GenericUsbHubPropName = "Oriental Motor USB Controller Hub";
const char* const g_GenericHubFoundPrefix = "COM ";  //Prefix for GenericUsbHubName
const char* const g_ControllerDevicePrefix = "Controller #";
const double g_maxPeripherals = 12;

const char * const g_DeviceNameOrientalMotorFocus = "Oriental Motor ";

const char* const g_OrientalStageDeviceName = "Oriental Stepper Motor Z Adjust";

const char* const g_OrientalStageDeviceDescrip = "Oriental Stepper Motor Z Adjust";
const char* const g_OrientalUSBHubDeviceName = "Communication Hub for Oriental Motors";
const char* const g_OrientalUSBHubDeviceDescrip = "Hub Used to Communicate via Rs-285 with other Oriental Motors as Focus Knobs";

const char* const g_OrientalErrorLogger = "Oriental Error Logger";

const char* const g_OrientalAdapterPropertyName = "Adjuster Knob Attached";
const char* const g_OrientalControllerPropertyName = "Stepper Motor Controller";

const char* const g_OrientalBaseAnglePartitionOptionsName = "Partitions of Selected Motor Base Step Angle";
const char* const g_OrientalBaseAngleOptionsName = "Motor Base Step Angle";
const char* const g_OrientalRestingEnergyStateName = "Resting Energized State";

#endif