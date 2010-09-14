


////////////////////////////////////////////////
inline SpinLock::SpinLock(int max_delta_time)
{
	static int _id=1;
	this->id=(_id++);

	#ifdef _WINDOWS
		this->value=0;
	#else
		pthread_spin_init (&__lock,0);
	#endif

	this->max_delta_time=max_delta_time;
}

////////////////////////////////////////////////
inline SpinLock::~SpinLock()
{
	#ifdef _WINDOWS
		;
	#else
		pthread_spin_destroy (&__lock);
	#endif
}

////////////////////////////////////////////////
inline void SpinLock::Lock()
{
	
	#ifdef _WINDOWS
		 while(InterlockedExchange(&value,1)==1) 
			 Thread::Sleep(Utils::IntRand(1,max_delta_time));
	#else
		while (!pthread_spin_trylock (&__lock))
			 Thread::Sleep(Utils::IntRand(1,max_delta_time));
	#endif
}

////////////////////////////////////////////////
inline void SpinLock::Unlock()
{
	#ifdef _WINDOWS
		InterlockedExchange(&value,0);
	#else
		pthread_spin_unlock (&__lock)
	#endif
}