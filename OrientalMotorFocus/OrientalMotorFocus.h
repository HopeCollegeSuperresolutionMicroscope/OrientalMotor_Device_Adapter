#ifndef _ORIENTALMOTORFOCUS_
#define _ORIENTALMOTORFOCUS_


#include "../../MMDevice/DeviceBase.h"
#include "../../MMDevice/DeviceThreads.h"
#include "OrientalFocusKnobs.h"
#include "OrientalControllerTemplate.h"
#include "OrientalMotorHub.h"
/*
class ErrorLogger : public CGenericBase< ErrorLogger >
{

	public:

		ErrorLogger(){};
		~ErrorLogger(){};
		
		int Initialize(){ initialized_ = true;  return DEVICE_OK; };
		int Shutdown(){};

		static bool LogMessage( char * entityName, char * Message){ 

	private:

		bool initialized_;


}*/

class OrientalMotorFocus : public CStageBase<OrientalMotorFocus>
{
public:
	OrientalMotorFocus( std::string name );
	~OrientalMotorFocus(void);

   bool Busy() {return busy_; }
   void GetName(char* pszName) const;

   int Initialize();
   int Shutdown();
     
   // Stage API
   int SetPositionUm(double pos);
   int GetPositionUm(double& pos) {pos = pos_um_; LogMessage("Reporting position", true); return DEVICE_OK;}
   double GetStepSize() {return stepSize_um_;}
   int SetPositionSteps(long steps) 
   {
      pos_um_ = steps * stepSize_um_; 
      return  OnStagePositionChanged(pos_um_);
   }
   int GetPositionSteps(long& steps)
   {
      steps = (long)(pos_um_ / stepSize_um_);
      return DEVICE_OK;
   }
   int SetOrigin() { return DEVICE_OK; }
   int GetLimits(double& lower, double& upper)
   {
      lower = lowerLimit_;
      upper = upperLimit_;
      return DEVICE_OK;
   }
   int Move(double /*v*/) {return DEVICE_OK;}

   int IsStageSequenceable(bool& isSequenceable) const;
   bool IsContinuousFocusDrive() const {return false;}
   int GetStageSequenceMaxLength(long& nrEvents) const;
   int StartStageSequence();
   int StopStageSequence();
   int ClearStageSequence();
   int AddToStageSequence(double /* position */);
   int SendStageSequence();

   // action interface
   // ----------------
   int OnBaseAngleSelect( MM::PropertyBase* pProp, MM::ActionType eAct );
   int OnBaseAnglePartitionSelect( MM::PropertyBase* pProp, MM::ActionType eAct );
   int OnRestingEnergyStateSelect(MM::PropertyBase* pProp, MM::ActionType eAct);

   int OnPosition(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnAdjusterSelect(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnControllerSelect(MM::PropertyBase* pProp, MM::ActionType eAct);
   int OnAddressSelect(MM::PropertyBase* pProp, MM::ActionType eAct);

   int OnSequence(MM::PropertyBase* pProp, MM::ActionType eAct);

   // Sequence functions

private:

   //Constructor Property Logic
   int SetAdjuster( std::string key );
   int SetController( std::string key );

   std::string name_;

   //Adjuster knob used just for reference
   const AdjusterKnob * adjuster_;
   //AbstractControllerInterface copied from Resolver for each instance
   AbstractControllerInterface * controller_;
   //Boolean For Indicating a Change in the Base Angle List
   bool baseAngleChangeSignal_;

   double stepSize_um_;
   double pos_um_;
   bool initialized_;
   double lowerLimit_;
   double upperLimit_;

   //Extras for compile
   bool sequenceable_;
   bool busy_;

   OrientalFTDIHub* hub_;

};


#endif  // _ORIENTALMOTORFOCUS_