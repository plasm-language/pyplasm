#include <xge/xge.h>
#include <xge/log.h>


#ifdef _WINDOWS
HANDLE Log::__redirect=0;
#endif

////////////////////////////////////////////////////////////////////
bool Log::redirect(uint64 handle)
{
	#ifdef _WINDOWS
	__redirect=(HANDLE)handle;
	return true;
	#else
	//TODO
	return false;
	#endif
}

 
////////////////////////////////////////////////////////////////////
void Log::printf(const char * format, ...)
{
	int ret;
	va_list args;
	va_start(args,format);

	#ifdef _WINDOWS
	if (!__redirect)
	{
		ret = vfprintf(stdout, format, args);	
		fflush(stdout);
	}
	else
	{
		//need to write to a Windows File (is a PIPE!)
		OVERLAPPED o; 
		memset(&o, 0, sizeof(o)); 
		o.Offset =  0xffffffff; 
		o.OffsetHigh = -1;
		int length = _vscprintf(format, args);
		char* tmp = (char*)malloc(length+1);
		vsprintf(tmp, format, args);
		tmp[length] = 0;
		DWORD written;
		ret=WriteFile(__redirect,tmp,length,&written,&o);
		free(tmp);
	}
	#else
	{
		ret = vfprintf(stdout, format, args);	
		fflush(stdout);
	}
	#endif

	va_end(args);
}


