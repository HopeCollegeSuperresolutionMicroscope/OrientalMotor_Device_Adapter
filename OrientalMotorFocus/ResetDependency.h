/*
*
*  Helper Classes For Implementing Thread-safe access to conditional variables 
*  that control whether or not to perform certain behaviors without being cued to a reset
*
*  This class should be registered in AbstractControllerInterface::RegisterResetDependencyObj().
*  Additionally, any implementations that might perform a reset on their own, 
*  should then call AbstractControllerInterface::FullReset for all resets to be reset, or ResetSingleDependency()
*  
*
*/

#ifndef RESET_DEPENDENCY
#define RESET_DEPENDENCY

#include <stdlib.h>
#include <vector>
#include "../../MMDevice/DeviceThreads.h"

//Forward Declaration of ResetDependency
class ResetDependency;

/* Class to be Implemented in a given method that is ResetDependent
*   It should have static duration inside of the method or duration for as long as 
*   the parent object that contains the Reset Dependency is within scope.
*	
*	Note:  This Class wraps a boolean value in thread-safe accessors
*/
class ResetLock 
{
	public:

		ResetLock() : lock_(false), //All locks are initialized false
			numHoldingDependencies_(0)
		{}
		~ResetLock(){}

		//Determine whether or not the lock is still waiting on a lock
		//This is false only when no Dependencies hold this ResetLock
		bool isLocked() { MMThreadGuard guard(statusLock_); return lock_; }

		//Get the tags of any remaining holding dependencies
		std::vector< std::string > getHoldingResets() 
		{ 
			MMThreadGuard guard( tagLock_ );
			return currentHoldingDependencyTags_; 
		}


	private:
		friend ResetDependency; //Only let ResetDependency modify the ResetLock
		//Checks From ResetDependency Guarantee that setLock is not set more than once per Dependency
		void setLock( bool lock, std::string tag ) { 
			if( lock )
			{
				++numHoldingDependencies_;
				addTag( tag );
			}
			else
			{
				--numHoldingDependencies_;
				removeTag( tag );
			}
			//If there are no-independent Resets Holding This, reset it
			if( numHoldingDependencies_ < 1 )
			{
				MMThreadGuard guard(statusLock_); 
				lock_ = lock;
			}
		}
 
		//Tag Manipulation functions
		//Remove a given tag from the list of holding tags
		void removeTag( std::string tag ) 
		{
			MMThreadGuard guard( tagLock_ );
			std::vector< std::string >::iterator it = currentHoldingDependencyTags_.begin();

			for( ; it != currentHoldingDependencyTags_.end(); ++it )
			{
				if( (*it).compare(tag) == 0 )
				{
					currentHoldingDependencyTags_.erase( it );
					return;
				}
			}

		}

		//Add a given tag to the list of holding tags
		//TODO:  Report on Duplicates
		void addTag( std::string tag )
		{
			MMThreadGuard guard( tagLock_ );
			currentHoldingDependencyTags_.push_back(tag);			
		}


		int numHoldingDependencies_;
		std::vector< std::string > currentHoldingDependencyTags_;
		MMThreadLock tagLock_;  //lock for modifying and reading Tags
		MMThreadLock statusLock_; //lock for modifying and reading ResetLock status (lock_)
		bool lock_;
};

/*  Class For Implementing Different Types of Resets (i.e. fullReset, factory, software, hardware )
*	  This Class adds ResetLocks to it's waiting list and until reset() is invoked, will not release them
*     To accomodate for Resets that can encompass others (i.e. fullReset encompasses software reset)
*       this class takes instances of other ResetDependencies in order to override any pending locks with them when it is triggered.
*/
class ResetDependency 
{

	public:

		//Possible TODO:  Add Error Logging, etc
		/* Constructor - 1 argument
		*   @param tag - name for GUI and error printing (should be meaningful and different than others)
		*   Note:  creates an empty vector of dependencyOverrides, so there is no relation to other ResetDependency's
		*/
		ResetDependency( std::string tag ) : tag_(tag), 
			dendencyOverrideList_( std::vector<ResetDependency*>() )
		{}

		/* Constructor Override - 2 argument
		*  @param tag - name for GUI and error printing (should be meaningful and different than others)
		*  @param dependencyOverrides - vector of other ResetDependency objects that are overridden if the user performs this Reset
		*/
		ResetDependency( std::string tag, std::vector< ResetDependency *> dependencyOverrides ) : tag_(tag), 
			dendencyOverrideList_(dependencyOverrides)
		{}

		//Destructor - resets any remaining ResetLocks
		~ResetDependency() { reset(); }

		/* Adds a ResetLock& to the lockList that of locks that are waiting for a reset 
		*   @param lock - reference to a ResetLock that needs to wait for the given ResetDependency
		*   Note:  This function only ever stores 1 instance of a ResetLock object and sets it's isLocked() to true;
		*   Note:  ResetLocks are stored in other objects or methods and should have lifetimes >= the object holding the ResetDependency
		*/
		int add( ResetLock& lock ) 
		{
			MMThreadGuard guard( listLock_ );

			lock.setLock( true, tag_ ); 
			if( isNewLock( lock) )
			{
				lockList_.push_back( &lock ); 
			}
			return 0; 
		}

		/* Resets all locks that have currently been waiting for this particular Reset Dependency
		*   Note: This reset will remove any overriding Reset Dependencies
		*	Note 2: If multiple independent Reset Dependencies have all added a given lock, this does not guarantee isLocked() == false
		*           Instead, the programmer must be careful, if using shared locks, to reset all Dependencies before expecting isLocked() == true
		*   
		*/
		int reset( ) 
		{ 
			MMThreadGuard guard( listLock_ );

			std::vector< ResetLock * >::iterator it = lockList_.begin();
			for( ; it != lockList_.end(); ++it ) 
			{
				resetSingleLock( (*it) );
				//selectively check for any erasures from overRiding ResetDependency's

			}
			lockList_.erase( lockList_.begin(), lockList_.end() );
		}

		//Returns the unique String Value Pertaining to the TAG of the Reset Object
		//This is used for both GUI and error Reporting
		std::string getTag() const 
		{
			return tag_;
		}

	private:

		/* Resets a single ResetLock and any overrides
		*   @param lock - a pointer to the given ResetLock is being reset
		*   Note:  This is a node for simplifying recursion
		*/
		void resetSingleLock( ResetLock* lock ) 
		{
			lock->setLock( false, tag_ );
			//Call Overrides as well
			std::vector< ResetDependency* >::iterator it = dendencyOverrideList_.begin();

			for( ; it != dendencyOverrideList_.end(); ++it )
			{
				(*it)->overrideReset( lock );
			}
		}

		/* Called From another Reset Dependency to release any pending locks as if reset.
		*   Note:This cleans up the ability for an out of order reset on another Dependency reset() call
		*   @param lock - a pointer to the given ResetLock that the Overrider is resetting
		*/
		void overrideReset( ResetLock* lock ) 
		{
			std::vector< ResetLock* >::iterator it = lockList_.begin();

			for( ; it != lockList_.end(); ++it )
			{
				//If this lock is found, override it with a reset as well
				if( *it == lock ) 
				{
					//We call this so that we can count down the number of resets on the lock
					lockList_.erase( it ); 
					resetSingleLock( lock );
					return;
				}
			}
		}

		//Checks to see if the given lock is not already a part of the vector list
		bool isNewLock( ResetLock& lock )
		{
			std::vector< ResetLock* >::iterator it = lockList_.begin();

			for( ; it != lockList_.end(); ++it )
			{
				if( *it == &lock )
				{
					return false;
				}
			}
			return true;
		}

		MMThreadLock listLock_;
		const std::string tag_;
		std::vector< ResetDependency* > dendencyOverrideList_;
		std::vector< ResetLock* > lockList_;
};


#endif