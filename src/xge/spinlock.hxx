
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
	#ifdef _WINDOWS
	{
		while(InterlockedExchange(&value,1)==1) 
			Thread::Sleep(Utils::IntRand(1,max_delta_time));
	}
	#elif defined(PLATFORM_Darwin)
	{
		while (!OSSpinLockTry(&value))
			Thread::Sleep(Utils::IntRand(1,max_delta_time));
	}
	#elif defined(PLATFORM_Linux)
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
	#ifdef _WINDOWS
	{
		InterlockedExchange(&value,0);
	}
	#elif defined(PLATFORM_Darwin)
	{
		OSSpinLockUnlock(&value);
	}
	#elif defined(PLATFORM_Linux)
	{
		__sync_lock_test_and_set(&value,0);
	}
	#else
	{
		bogus
	}
	#endif

}
