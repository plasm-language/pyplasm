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

	#ifdef _WINDOWS
	int length = _vscprintf(format, argList);
	tmp = (char*)malloc(length+1);
	vsprintf(tmp, format, argList);
	tmp[length] = 0;
	#else //_WINDOWS
	vasprintf(&tmp,format,argList);
	#endif //_WINDOWS

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
#ifdef _WINDOWS
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

	//#ifdef _WINDOWS
	//MessageBoxA(NULL, (LPCSTR)formatted_msg.c_str(), (LPCSTR)"Error",MB_ICONERROR | MB_OK);
	//#endif

	//throw string!
	#ifdef _DEBUG
	assert(false);
	#endif

	throw formatted_msg.c_str();
}


///////////////////////////////////////////////
int Utils::SelfTest()
{
	Log::printf("Testing Utils...\n");
	
	XgeReleaseAssert(Utils::IsPower2(16) && !Utils::IsPower2(15));
	XgeReleaseAssert(Utils::FuzzyEqual(Utils::Degree2Rad(90),(float)M_PI/2));
	XgeReleaseAssert(Utils::FuzzyEqual(Utils::Rad2Degree((float)M_PI/2),90));
	XgeReleaseAssert(Utils::LTrim(" \thello ")=="hello ");
	XgeReleaseAssert(Utils::RTrim(" hello \t")==" hello");
	XgeReleaseAssert(Utils::Trim(" \thello \t")=="hello");
	XgeReleaseAssert(Utils::ToLower("Hello")=="hello");
	//XgeReleaseAssert(Utils::ToUpper("HellO")=="HELLO");
	XgeReleaseAssert(Utils::StartsWith("hello all","HELLO"));
	XgeReleaseAssert(!Utils::StartsWith("hello all","HELLO",true));
	XgeReleaseAssert(Utils::Replace("hello all","hello","Hy")=="Hy all");

	{
		std::vector<int> v;
		v.push_back(1);
		v.push_back(2);
		v.push_back(3);
		XgeReleaseAssert(Utils::IndexOf(v,2)==1 && Utils::IndexOf(v,10)==-1);

		XgeReleaseAssert(Utils::Contains(v,2) && !Utils::Contains(v,10));


		v=Utils::RemoveAll(v,3);
		XgeReleaseAssert(v.size()==2);
		v=Utils::RemoveAll(v,10);
		XgeReleaseAssert(v.size()==2);
		v=Utils::RemoveLast(v);
		XgeReleaseAssert(v.size()==1 && v[0]==1);

		v=Utils::Insert(v,0,-10);
		v=Utils::Insert(v,2,+10);
		XgeReleaseAssert(v.size()==3 && v[0]==-10 && v[1]==1 && v[2]==10);
	}

	return 0;
}


