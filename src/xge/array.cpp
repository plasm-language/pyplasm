
#include <xge/xge.h>
#include <xge/array.h>

 
////////////////////////////////////////////////////
Array::~Array()
{
	gpu.reset();
	if (pointer) MemPool::getSingleton()->free(sizeof(float)*num,pointer);
}

  
////////////////////////////////////////////////////
void Array::parse(int num,float* dest,const char* source,char* _format)
{
	char format[256];
	sprintf(format,"%s%%num",_format);
	const char* ptr=source;
	for (int i=0,howmuch;i<num;i++)
	{
		float aux;
		sscanf(ptr,format,&aux,&howmuch);
		dest[i]=aux;
		ptr+=howmuch;
	}
}


