#include "ControllerStatusMonitorThread.h"
#include "OrientalControllerTemplate.h"

int ControllerStatusMonitorThread::svc() {

	assert( minTickIntervalMS_ > 0 );

	int errCode;

	AbstractControllerInterface* cont;
	long stepPeriodUS;
	int counter = 0;
	while( GetStopCondition() == false )
	{
		//Note, Sleep Resolution is 1 ms min (Windows), so that will be predominant most of the time
		CDeviceUtils::SleepMs( minTickIntervalMS_ );
		for( int i = 0; i < getCurrentListSize(); i++ )
		{

			std::ostringstream es;

			//Destructor ends at each iteration: prioritizes controller Addition
			MMThreadGuard guard( mutex_ );
			cont = monitorList_[i].first;
			stepPeriodUS = monitorList_[i].second;
			//Check to Make Sure Controller Monitor Time Is Past Basic Monitor Time
			cont->IncCurrentMonitorTime( (minTickIntervalMS_*1000)/stepPeriodUS );
			if( cont->IsPastMonitorTimeInc() )
			{
				core_.LogMessage( &device_, "The PastMonitorTime Triggered" , false );
				if( ( errCode = cont->IsMotorBusy() ) == 0 )
				{
					errCode = cont->PermitBusyDependentProcesses();
					es << "PermitPosWrite Errcode =  " << errCode << "\n";
					cont->restartTimeMonitor();
					monitorList_.erase( monitorList_.begin() + i );
					core_.LogMessage( &device_, es.str().c_str(), false );
				}
				else 
				{
					es << "This is The errCode from Motor Busy() " << errCode;
					core_.LogMessage( &device_, es.str().c_str(), false );
				}
			}
		}
		
	}

	return 0;

};

/* Add a controller to the monitor base
*   Note: controller uses GetStepPeriodUs to store the Increment Step For Controller Specific Waiting
*   @param controller - Current Controller That is Waiting For a status Update
*   Returns - 0 if successful or errCode otherwise
*/
int ControllerStatusMonitorThread::addController( AbstractControllerInterface* controller ) {

	//Check Lock To Make Sure Process is not Operating On Anything in monitorList_
	MMThreadGuard guard( mutex_ );
	//No multiple instances of same controller
	for( int i = 0; i < monitorList_.size(); i++ )
	{
		if( monitorList_[i].first == controller )
		{
			//Controller Already Registered
			return 1;
		}
	}

	controller->restartTimeMonitor();
	monitorList_.push_back( std::pair< AbstractControllerInterface*, long>( controller, controller->GetStepPeriodUS() ) );
	return 0;

}

/*  Added to Specifically remove a Controller When It is Being deleted
*	    @param controller - reference to controller in question
*     Returns - 0 if removed, otherwise non-zero is nothing was deleted
*/
int ControllerStatusMonitorThread::removeController( AbstractControllerInterface* controller ) {

	MMThreadGuard guard( mutex_ );
	
	for( int i = 0; i < monitorList_.size(); i++ )
	{
		if( monitorList_[i].first == controller )
		{
			monitorList_.erase( monitorList_.begin() + i );
			core_.LogMessage( &device_, "We Removed a Controller", false);
			return 0;
		}
	}

	return 1;

}