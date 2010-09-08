
#include <xge/xge.h>
#include <xge/mempool.h>


extern "C" XGE_API unsigned long xge_total_memory=0;


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



/////////////////////////////////////////// 
int MemPool::SelfTest()
{
	Log::printf("Testing MemPool...\n");

	void* allocated[2][MEMPOOL_TABLE_SIZE];
	
	for (int size=1;size<MEMPOOL_TABLE_SIZE;size++)
	{
		allocated[0][size]=getSingleton()->malloc(size);
		allocated[1][size]=getSingleton()->malloc(size);
	}

	for (int size=1;size<MEMPOOL_TABLE_SIZE;size++)
	{
		getSingleton()->free(size,allocated[1][size]);
		getSingleton()->free(size,allocated[0][size]);
	}

	for (int size=1;size<MEMPOOL_TABLE_SIZE;size++)
	{
		void* temp1=getSingleton()->malloc(size);
		void* temp2=getSingleton()->malloc(size);

		ReleaseAssert(temp1==allocated[0][size]);
		ReleaseAssert(temp2==allocated[1][size]);
	}

	return 0;

}