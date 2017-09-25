#ifndef _CONTROLLER_STATUS_MONITOR_THREAD
#define _CONTROLLER_STATUS_MONITOR_THREAD

#include "../../MMDevice/MMDevice.h"
#include "../../MMDevice/DeviceBase.h"
#include "../../MMDevice/DeviceThreads.h"
#include <vector>
#include <assert.h>

//Forward Declaration of AbstractControllerInterface
class AbstractControllerInterface;

/*  MonitoringThread For Any Given Hub Instance That Uses A Controller-Motor Interface Similar to OrientalMotorHub
*     Asynchronously checks (via controller protocol) to see if it's busy and updates the given register or boolean
*     For This Reason IsBusy() needs to be implemented in the controller to signal changes of busy state for other methods
*/

class ControllerStatusMonitorThread: public MMDeviceThreadBase
{
   public:
	   ControllerStatusMonitorThread( MM::Device& device, MM::Core& core, long minTickIntervalMS = 1 ): 
			stop_(false),
			core_(core),
			device_(device),
			minTickIntervalMS_(minTickIntervalMS),
			listSize_(0)
			{ } 
	  ~ControllerStatusMonitorThread() {  }; 
      
	  /* Conditional Loop For Timed Checking An AbstractController's IsMotorBusy() and PermitMotorStep()
	  *    Note:  May be Modified Later to Include Other Checks, but current Timing should compensate for Driver Issues
	  *    Returns - 0 if successful or errCode otherwise
	  */
	  int svc( void );

	  /* Add a controller to the monitor base
	  *   Note: controller uses GetStepPeriodUs to store the Increment Step For Controller Specific Waiting
	  *   Returns - 0 if successful or errCode otherwise
	  */
	  int addController( AbstractControllerInterface* controller );
	  /*  Added to Specifically remove a Controller When It is Being deleted
	  *	    @param controller - reference to controller in question
	  *     Returns - 0 if removed, otherwise non-zero is nothing was deleted
	  */
	  int removeController( AbstractControllerInterface* controller );

	  //Threadsafe Return of monitorList_ size
	  int getCurrentListSize() { MMThreadGuard guard( mutex_ ); return monitorList_.size(); }
      
	  int open (void*) { return 0;}
      int close(unsigned long) {return 0;}

	  /* Used to Start the Thread Loop
	  */
	  void Start() { //MMThreadGuard guard(stopLock_);
		  stop_ = false; 
		  //guard.~MMThreadGuard(); 
		  activate(); };

	  /* Used to Stop the Thread Loop
	  */
      void Stop() { //MMThreadGuard guard(stopLock_); 
		  stop_ = true;}

	  bool GetStopCondition( ) { MMThreadGuard guard(stopLock_); return stop_; }

   private:
	  //Used to Lock The Thread From Changing it's Value in svc
	  MMThreadLock mutex_;
	  MMThreadLock stopLock_;
      MM::Core& core_;
	  MM::Device& device_;
      bool stop_;
	  int listSize_;
      const long minTickIntervalMS_;
	  //Monitor List Consists of The Interface and the Step Speed at Which it was Registered
	  std::vector< std::pair< AbstractControllerInterface*, long> > monitorList_;

      ControllerStatusMonitorThread& operator=(ControllerStatusMonitorThread& ) {assert(false); return *this;}
};

#endif