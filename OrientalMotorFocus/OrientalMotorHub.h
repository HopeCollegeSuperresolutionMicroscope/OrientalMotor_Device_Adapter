#ifndef _ORIENTAL_MOTOR_HUB_H_
#define _ORIENTAL_MOTOR_HUB_H_

#include "../../MMDevice/MMDevice.h"
#include "../../MMDevice/DeviceBase.h"
#include "OrientalControllerTemplate.h"
#include "ControllerStatusMonitorThread.h"
#include <string>
#include <map>
#include "ftd2xx.h"


//Manufacturer Defaults
#define FTDI_DEFAULT_VID 0x0403
#define FTDI_DEFAULT_PID 0x6001 //or 0x6010 or 0x6006

//Custom Constants
#define DEVICE_OCCUPIED -1


class OrientalFTDIHub : public HubBase<OrientalFTDIHub>
{
public:
	OrientalFTDIHub( int maxConnRetries );
   ~OrientalFTDIHub();
   
   // Device API
   // ---------
   int Initialize();
   int Shutdown();
   void GetName(char* pName) const;
   bool Busy() { return false; }

   // HUB api

   //Use FTDI Library to Retrieve and determine the Hub
   int FindAvailableHubs( /* params */ );


   MM::DeviceDetectionStatus DetectDevice() { return MM::CanCommunicate; }
   int VerifyListedDevice(unsigned int index, void* &handle, int retries = 3);
   int VerifiedClose(void* &ptr, int retries = 3);
   int DetectInstalledDevices(void);

   //Using FTDI Library...  

   int FTDISerialCommunicate( unsigned char txMsgBuffer[], int txMsgLen,  AbstractControllerInterface* controller, bool broadcast = false );

   //Property Events
   int OnVID(MM::PropertyBase* pProp, MM::ActionType pAct);
   int OnPID(MM::PropertyBase* pProp, MM::ActionType pAct);
   int OnHubSelect(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnPeripheralNumber(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnControllerSelect(MM::PropertyBase* pProp, MM::ActionType eAct, long peripheralNumber);
   int onPort( MM::PropertyBase* pProp, MM::ActionType pAct );

   //Monitor Thread
   ControllerStatusMonitorThread* GetStatusMonitorThread( void ) { return statusMonitorThread_; }

private:
		
   void GetPeripheralInventory();
   int SetHubAndRelatedValues( MM::PropertyBase* hubProp );
   int ChangeOpenHubBySerialNumber( char serialNum[16] );

   FT_DEVICE_LIST_INFO_NODE* GetFTDIDeviceFromComValue( std::string value );

   //Related to to selected FTDI (See Definition)
   static const char * hubRelatedPropertyNames_[];

   	//ThreadLock For Serial Function
	MMThreadLock serialLineMutex_;
	ControllerStatusMonitorThread* statusMonitorThread_;

   //Current Opened Device Handle
   FT_HANDLE openHub_;
   int maxConnRetries_;

   std::vector<std::string> peripherals_;
   //static MMThreadLock lock_;
   int numPeripherals_;
   
   std::vector<std::string> availControllers_;
   std::map< std::string , FT_DEVICE_LIST_INFO_NODE > curAvailDevMap_;

   bool shutterState_;
   bool initialized_;
   bool busy_;
   long vid_;
   long pid_;

   //Semantics for Serial Write From Arduino
   std::string port_;
   bool portAvailable_;

};

#endif