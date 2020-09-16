
#include <xge/xge.h>
#include <xge/mempool.h>


extern "C" unsigned long xge_total_memory=0;


/////////////////////////////////////////// 
MemPool::MemPool()
{
	//xge_allocated_memory+=sizeof(SinglePool)*MEMPOOL_TABLE_SIZE;

	for (int i=0;i<MEMPOOL_TABLE_SIZE;i++) 
	{
		
		mem_pools[i]=new SinglePool(i);
	}
}

/////////////////////////////////////////// 
MemPool::~MemPool()
{
	for (int i=0;i<MEMPOOL_TABLE_SIZE;i++) 
		delete mem_pools[i];

	//xge_allocated_memory-=sizeof(SinglePool)*MEMPOOL_TABLE_SIZE;
}


