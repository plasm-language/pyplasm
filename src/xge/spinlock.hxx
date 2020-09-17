
////////////////////////////////////////////////
inline SpinLock::SpinLock(int max_delta_time)
{
	static int _id=1;
	this->id=(_id++);
	this->value=0;
	this->max_delta_time=max_delta_time;
}

////////////////////////////////////////////////
inline void SpinLock::Lock()
{
	#if PYPLASM_WINDOWS
	{
		while(InterlockedExchange(&value,1)==1) 
			Thread::Sleep(Utils::IntRand(1,max_delta_time));
	}
	#elif PYPLASM_APPLE
	{
		while (!OSSpinLockTry(&value))
			Thread::Sleep(Utils::IntRand(1,max_delta_time));
	}
	#elif PYPLASM_LINUX
	{
		while(__sync_lock_test_and_set(&value,1)==1) 
			Thread::Sleep(Utils::IntRand(1,max_delta_time));
	}
	#else
	{
		bogus
	}
	#endif
}

////////////////////////////////////////////////
inline void SpinLock::Unlock()
{
	#if PYPLASM_WINDOWS
	{
		InterlockedExchange(&value,0);
	}
	#elif PYPLASM_APPLE
	{
		OSSpinLockUnlock(&value);
	}
	#elif PYPLASM_LINUX
	{
		__sync_lock_test_and_set(&value,0);
	}
	#else
	{
		bogus
	}
	#endif

}
