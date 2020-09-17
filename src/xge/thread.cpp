#include <xge/xge.h>
#include <xge/thread.h>

//------------------------------------------------------------------------
//------------------------------------------------------------------------
Thread::Thread(int nworkers,bool bDisableThread)
{
	this->bAbortThread=false;
	this->nrunning=0;
	this->nworkers=nworkers;
	this->bDisableThread=bDisableThread && nworkers==1;

	workers=new Worker[nworkers];

	for (int i=0;i<nworkers;i++)
	{
		workers[i].driver=this;
		workers[i].nworker=i;
		workers[i].running=false;
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
Thread::~Thread()
{
	for (int i=0;i<nworkers;i++)
	{
		//if a thread is running then close it
		if (workers[i].running)
		{
			#if PYPLASM_WINDOWS
			CloseHandle(workers[i].winhandle);
			#else
			pthread_cancel(workers[i].thread);
			#endif
		}
	}

	delete [] workers;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
int Thread::getNumberOfWorkers() const
{
	return nworkers;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
#if PYPLASM_WINDOWS
unsigned long __stdcall Thread::startfun(void far * data)
#else
void* Thread::startfun(void * data)
#endif
{
	Thread::Worker* worker=(Worker*)data;

	//wait for the handle to be assigned for deallocation
	while (!worker->running) 
		Thread::Sleep(10);

	//run the thread (==do the job)
	worker->driver->doJob(worker->nworker);
	worker->running=false;

	// is a lock necessary here?
	--(worker->driver->nrunning);

	//finished with this one
	#if PYPLASM_WINDOWS
	CloseHandle(worker->winhandle);
	#else
	pthread_exit(NULL);
	#endif
	
	return 0;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Thread::run()
{
	bAbortThread=false;
	nrunning=nworkers;
	for (int i=0;i<nworkers;i++)
	{
		if (bDisableThread)
		{
			workers[i].running=true;
			doJob(i);
			workers[i].running=false;
		}
		else
		{
			#if PYPLASM_WINDOWS
			workers[i].winhandle=CreateThread( NULL, 0, startfun, &workers[i] , 0, NULL); 
			#else
			pthread_create( &workers[i].thread, NULL, startfun, (void*) &workers[i]);
			#endif
			workers[i].running=true;
		}
	}
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Thread::Abort()
{
	this->bAbortThread=true;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool Thread::Done()
{
	return (nrunning==0);
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Thread::Wait(int nworker)
{
	if (workers[nworker].running)
	{
		#if PYPLASM_WINDOWS
		WaitForSingleObject(workers[nworker].winhandle, 0xFFFFFFFF);
		#else	
		void* status;
		pthread_join(workers[nworker].thread,&status);
		#endif
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Thread::Wait()
{
	//wait all threads
	for (int i=0;i<nworkers;i++)
	{
		Wait(i);
	}
}	

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Thread::Sleep(unsigned int msec)
{
	#if PYPLASM_WINDOWS
	::Sleep(msec);
	#else
	usleep(((unsigned long)msec)*1000);
	#endif
}
