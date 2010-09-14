#ifndef _SPIN_LOCK_H__

#include <xge/xge.h>
#include <xge/thread.h>


/*!
Class for very fast lock with active sleep
*/

class XGE_API SpinLock
{
public:

	//! default constructor
	inline SpinLock(int max_delta_time=SPIN_LOCK_DEFAULT_MAX_DELTA_TIME);

	inline ~SpinLock();

	//! lock a resource
	inline void Lock();

	//unlock a Resource
	void Unlock();

private:

	long id;

#ifdef _WINDOWS
	long value;
#else
	pthread_spinlock_t __lock;
#endif

	int  max_delta_time;
};

//inlines here!
#include <xge/spinlock.hxx>

#endif //_SPIN_LOCK_H__
