


////////////////////////////////////////////////////////////////////////////
inline SinglePool::SinglePool(int size)
{
	this->npool=0;
	this->size=max2(size,sizeof(void*));
	this->pool=0;
}

////////////////////////////////////////////////////////////////////////////
inline SinglePool::~SinglePool()
{
	while (pool)
	{
		void* temp=pool;
		this->pool=(*((void**)pool));
		::free(temp);
		xge_total_memory-=this->size;
		--npool;
	}
	ReleaseAssert(!npool);
}


////////////////////////////////////////////////////////////////////////////
inline void* SinglePool::malloc()
{
	_lock.Lock();

	void* ret;

	//in cache
	if (pool)
	{
		--npool;
		ret=pool;
		this->pool=(*((void**)pool));
	}
	//not in cache
	else
	{
		ret=::malloc(this->size);
		ReleaseAssert(ret!=0);
		xge_total_memory+=this->size;
	}

	

	_lock.Unlock();
	return ret;
}

////////////////////////////////////////////////////////////////////////////
inline void SinglePool::free(void* p)
{
	//put in cache
	_lock.Lock();

	{
		++npool;
		(*((void**)p))=pool;
		this->pool=p;
	}

	#if XGE_TRACK_MEMORY
	//check the correctness of the first item in the pool
	void* _temp=(*((void**)pool));
	printf("",_temp);
	#endif

	_lock.Unlock();
}



////////////////////////////////////////////////////////////////////////////
inline MemPool* MemPool::getSingleton()
{
	static MemPool* __singleton__=new MemPool();
	return __singleton__;
}

////////////////////////////////////////////////////////////////////////////
inline void* MemPool::malloc(int size)
{	
	//allocation of 0 bytes
	if (!size)
		return 0;

	void* ret;

	if (size<MEMPOOL_TABLE_SIZE)
	{
		ret=mem_pools[size]->malloc();
	}
	else
	{
		//use general malloc
		ret=::malloc(size);
		xge_total_memory+=size;
		ReleaseAssert(ret);
	}
	
	#if XGE_TRACK_MEMORY
	ReleaseAssert(_memory_in_use.size()==0 || _memory_in_use.find((uint64)ret)==_memory_in_use.end());
	_memory_in_use[(uint64)ret]=size;
	#endif

	return ret;
}
 

////////////////////////////////////////////////////////////////////////////
inline void MemPool::free(int size,void* p)
{
	//not size
	if (!size)
		return;

	#if XGE_TRACK_MEMORY
	ReleaseAssert(_memory_in_use.size()>0 && _memory_in_use.find((uint64)p)!=_memory_in_use.end() && _memory_in_use[(uint64)p]==size);
	_memory_in_use.erase((uint64)p);
	#endif

	//in cache
	if (size<MEMPOOL_TABLE_SIZE)
	{
		mem_pools[size]->free(p);
		return;
	}
	
	//generic free
	::free(p);
	xge_total_memory-=size;
}

////////////////////////////////////////////////////////////////////////////
inline void* MemPool::calloc(int num,int size)
{
	int totsize=num*size;
	void* ret=this->malloc(totsize);
	memset(ret,0,totsize);
	return ret;
}

////////////////////////////////////////////////////////////////////////////
inline void* MemPool::realloc(int old_size,void* p,int new_size)
{
	//realloc from scratch
	if (!old_size)
	{
		DebugAssert(p==0);
		void* ret= this->malloc(new_size);
		return ret;
	}

	//deallocation
	if (!new_size)
	{
		this->free(old_size,p);
		return 0;
	}

	#if XGE_TRACK_MEMORY
	ReleaseAssert(_memory_in_use.size()>0 && _memory_in_use.find((uint64)p)!=_memory_in_use.end());
	ReleaseAssert(_memory_in_use[(uint64)p]==old_size);
	#endif

	//useless call
	if (old_size==new_size)
		return p;

	//use clib standard realloc
	if (new_size>=MEMPOOL_TABLE_SIZE && old_size>=MEMPOOL_TABLE_SIZE)
	{
		void* ret=::realloc(p,new_size);
		ReleaseAssert(ret);
		xge_total_memory+=(new_size-old_size);

		#if XGE_TRACK_MEMORY
		_memory_in_use.erase((uint64)p);
		_memory_in_use[(uint64)ret]=new_size;
		#endif

		return ret;
	}
	
	//use my calls
	void* ret=this->malloc(new_size);
	memcpy(ret,p,min2(old_size,new_size));
	this->free(old_size,p);
	return ret;
}
