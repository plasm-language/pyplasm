#ifndef _SPIN_LOCK_H__

#include <xge/xge.h>

#ifdef PLATFORM_Darwin
#include <libkern/OSAtomic.h>
#endif


#include <xge/thread.h>

/*!
Class for very fast lock with active sleep
*/

class XGE_API SpinLock
{
public:

	//! default constructor
	inline SpinLock(int max_delta_time=SPIN_LOCK_DEFAULT_MAX_DELTA_TIME);

	//! lock a resource
	inline void Lock();

	//unlock a Resource
	void Unlock();

private:

	long id;

	#if defined(_WINDOWS)
		long value;
		
	#elif defined(PLATFORM_Darwin)
		OSSpinLock value;
		
	#elif defined(PLATFORM_Linux)
		long value;
		
	#else
		bogus
		
	#endif

	int  max_delta_time;
};

//inlines here!
#include <xge/spinlock.hxx>

#endif //_SPIN_LOCK_H__
