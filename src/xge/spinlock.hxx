
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
	 while(InterlockedExchange(&value,1)==1) 
		 Thread::Sleep(Utils::IntRand(1,max_delta_time));
	#endif

	#ifdef Darwin
	while (!OSSpinLockTry(&value))
		 Thread::Sleep(Utils::IntRand(1,max_delta_time));
	#endif
	
	#ifdef Linux
	Utils::Error(HERE,"todo");
	#endif
}

////////////////////////////////////////////////
inline void SpinLock::Unlock()
{
	#ifdef _WINDOWS
	InterlockedExchange(&value,0);
	#endif

	#ifdef Darwin
	OSSpinLockUnlock(&value);
	#endif

	#ifdef Linux
	Utils::Error(HERE,"todo");
	#endif

}