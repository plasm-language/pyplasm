#include <xge/xge.h>
#include <xge/utils.h>



///////////////////////////////////////////////////////////
void Utils::ParseInts(int num,int* dest,const char* string_source,char* string_format)
{
	char format[256];
	::sprintf(format,"%s%%num",string_format);

	const char* ptr=string_source;
	for (int i=0,howmuch;i<num;i++)
	{
		int aux;
		sscanf(ptr,format,&aux,&howmuch);
		dest[i]=aux;
		ptr+=howmuch;
	}
}

///////////////////////////////////////////////////////////
void Utils::ParseFloats(int num,float* dest,const char* string_source,char* string_format)
{
	char format[256];
	sprintf(format,"%s%%num",string_format);

	const char* ptr=string_source;
	for (int i=0,howmuch;i<num;i++)
	{
		float aux;
		sscanf(ptr,format,&aux,&howmuch);
		dest[i]=aux;
		ptr+=howmuch;
	}
}

////////////////////////////////////////////////////////////////////
std::string Utils::Format(const char * format, ...)
{
	va_list argList;
	va_start(argList,format);
	char* tmp=0;

	#if PYPLASM_WINDOWS
	int length = _vscprintf(format, argList);
	tmp = (char*)malloc(length+1);
	vsprintf(tmp, format, argList);
	tmp[length] = 0;
	#else 
	vasprintf(&tmp,format,argList);
	#endif 

	va_end( argList );
	std::string ret=std::string(tmp);
	free(tmp);
	return ret;
}

////////////////////////////////////////////////////////////////////
void Utils::Error(std::string location,const char * format, ...)
{
	va_list argList;
	va_start(argList,format);
	char* tmp=0;
#if PYPLASM_WINDOWS
	int length = _vscprintf(format, argList);
	tmp = (char*)malloc(length+1);
	vsprintf(tmp, format, argList);
	tmp[length] = 0;
#else
	vasprintf(&tmp,format,argList);
#endif

	va_end( argList );
	std::string formatted_msg="ERROR at " + location + " msg:" + tmp + "\r\n";
	free(tmp);
	Log::printf("%s",formatted_msg.c_str());

	//throw string!
	#ifdef _DEBUG
	assert(false);
	#endif

	throw formatted_msg.c_str();
}

