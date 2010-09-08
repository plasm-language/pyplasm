#ifndef _THREAD_H__
#define _THREAD_H__

#include <xge/xge.h>



#ifndef WIN32
#include <pthread.h>
#endif

/*
Example of use

class ThreadExample:public Thread
{
public:

	ThreadExample()
		:Thread(5)
	{
		//first mode, wait for completition
		printf("Starting...\n");
		run();
		wait();
		printf("All done\n");

		//second mode, do something while waiting
		printf("Starting...\n");
		run();
		while (!done()) 
		{
			Thread::sleep(100);
		}
		printf("All done\n");


	}

protected:

	virtual void doJob(int nworker) 
	{
		printf("Job %d\n",nworker);

		while (!bAbortThread)
		{
			... your code here.....
		}
	}

};
*/

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
class XGE_API Thread
{
protected:


	typedef struct
	{
	  //pointer to this class (the controller)
	  Thread*       driver;
	  
	  //the id for the current workder
	  int           nworker;
	  
	  //if the thread is running or not
	  bool          running;

	  //handle of the thread running
#ifdef WIN32
	  void*         winhandle;
#else
	  pthread_t     thread;
#endif

	}
	Worker;

	//total number of workers
	int     nworkers;

	//total process actually running
	int     nrunning;

	//pointer to thread structures
	Worker* workers;
	
	//special mode for DEBUG
	bool bDisableThread;

	//abort
	bool bAbortThread;

	#ifdef WIN32
	static unsigned long __stdcall startfun(void far * data);
	#else
	static void* startfun(void* data);
	#endif

	//to override
	virtual void doJob(int nworker)=0;

public:

	//constructor (NOTE IIF (bDisableThread==true && nworker=1) it won't create threads)
	Thread(int nworkers,bool bDisableThread=false);

	//destructor
	~Thread();

	// test if al jobs are done
	virtual bool Done();

	//abort all the process
	virtual void Abort();

	//run all the threads
	virtual void Run();

	//wait for all process to end
	virtual void Wait();

	// to sleep a certain amount of millisec
	static void Sleep(unsigned int msec);

	//access number of workers
	int getNumberOfWorkers() const;


};// end class


#endif //_THREAD_H__

