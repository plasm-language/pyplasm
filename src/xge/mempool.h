#ifndef _MEMPOOL_H__
#define _MEMPOOL_H__

#include <xge/xge.h>

//total memory allocated by pools
extern "C" XGE_API unsigned long xge_total_memory;


//=====================================
//! internal class for very fast alloc/dealloc for a fixed size
//=====================================
class XGE_API SinglePool
{
public:

	//!constructor (note: size is rouded at least to be the size of a generic pointer)
	SinglePool(int size);

	//~destructor
	~SinglePool();

	//malloc
	void* SinglePool::malloc();

	//free
	void SinglePool::free(void* p);

protected:

	//not thread safe
	SpinLock _lock;

	int   size;
	void* pool;
	int   npool;
};


//=====================================
//! internal class for very fast alloc/dealloc
/*!
	Internally it handles memory and it is thread safe (thanks to SinglePool SpinLock)
*/
//=====================================
class XGE_API MemPool
{
public:

	//! access the unique mempool
	static MemPool* getSingleton();

	//! equivalent to C malloc
	void* MemPool::malloc(int size);
	 
	//! equivalent to C free (but you must know the size)
	void MemPool::free(int size,void* p);

	//! equivalent to C calloc
	void* MemPool::calloc(int num,int size);

	//! equivalent to C realloc (but you must know the previous size)
	void* MemPool::realloc(int old_size,void* p,int new_size);

	//SelfTest
	static int SelfTest();

protected:

	//default constructor hidden
	MemPool();

	//default destructor hidden
	~MemPool();

	SinglePool* mem_pools[MEMPOOL_TABLE_SIZE];

	#ifdef XGE_TRACK_MEMORY
	std::map<uint64,int> _memory_in_use;
	#endif

};

//inlines here!
#include <xge/mempool.hxx>

#endif //_MEMPOOL_H__



