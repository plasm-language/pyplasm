#ifndef _SPIN_LOCK_H__

#include <xge/xge.h>

#ifdef PYPLASM_APPLE
#include <libkern/OSAtomic.h>
#endif


#include <xge/thread.h>

/*!
Class for very fast lock with active sleep
*/

class SpinLock
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

	#if PYPLASM_WINDOWS
		long value;
		
	#elif PYPLASM_APPLE
		OSSpinLock value;
		
	#elif PYPLASM_LINUX
		long value;
	#endif

	int  max_delta_time;
};

//inlines here!
#include <xge/spinlock.hxx>

#endif //_SPIN_LOCK_H__
