#include <xge/xge.h>
#include <xge/filesystem.h>

#include <sys/stat.h>
#include <zlib.h>


//must be set from outside
std::string FileSystem::ResourcesDir="";


/////////////////////////////////////////////////
/////////////////////////////////////////////////
static std::string normalize_path(std::string filename)
{
	std::string ret=filename;
	for (int i=0;i<(int)ret.length();i++)
	{
		//#ifdef _WINDOWS
		//ret[i]=tolower(ret[i]);
		//#endif
		if (ret[i]=='\\') ret[i]='/';
	}
	return ret;
}



/////////////////////////////////////////////////
/////////////////////////////////////////////////
std::string FileSystem::FullPath(std::string Filename)
{
	std::string ret=normalize_path(Filename);

	//if starts with :
	if (ret.length() && ret[0]==':')
	{
		if (ResourcesDir.length()==0)
		{
			ResourcesDir=getenv("TRS_RESOURCES")?getenv("TRS_RESOURCES"):".";
			ResourcesDir=normalize_path(ResourcesDir);	
		}

		ret=ret.substr(1);
		char last_char=ResourcesDir[ResourcesDir.length()-1];
		std::string sep=(last_char!='/' && last_char!='\\' && ret!="")?"/":"";
		ret=ResourcesDir + sep + ret;
	}
	
	return ret;
}



/////////////////////////////////////////////////
/////////////////////////////////////////////////
std::string FileSystem::ShortPath(std::string Filename)
{
	std::string ret=normalize_path(Filename);

	//the ResourceDir is not set so leave the path unchanged
	if (ResourcesDir.length()==0)
		return ret;
		
	//if the string is something like c:/free/trs_resources/file.txt -> :file.txt
	if (Utils::StartsWith(ret,ResourcesDir))
	{
		ret=std::string(ret.c_str()+ResourcesDir.length());
		char first_char=ret[0];
		if (first_char=='/') ret=std::string(ret.c_str()+1);
		ret=":" + ret;
	}	
	return ret;
}



////////////////////////////////////////////////////////////////////
std::string FileSystem::Extension(std::string filename)
{
	int i=(int)(filename.length()-1);

	while  (i>=0 && filename[i]!='.') 
		--i;

	return (i>=0)?filename.substr(i):"";
}



////////////////////////////////////////////////////////////////////
unsigned char* FileSystem::ReadFile(std::string Filename,unsigned long& filesize,bool bZeroTerminated)
{
	static const int megabyte=1024*1024;

	unsigned long buffsize=0;
	unsigned char* buff=0;

	//i can read always using gz
	std::string filename=FileSystem::FullPath(Filename);
	gzFile gzfile=gzopen(filename.c_str(),"rb");

	if (!gzfile)
	{
		filesize=0;
		return 0;
	}

	unsigned long ndone=0;
	while (true)
	{
		//i'm going to read beyond the buffer?
		if ((ndone+megabyte)>buffsize)
		{
			buff=(unsigned char*)MemPool::getSingleton()->realloc(buffsize,buff,buffsize+megabyte);
			buffsize+=megabyte; //increase it
		}

		unsigned long nread=gzread(gzfile,buff+ndone,megabyte);
		ndone+=nread;

		//finished reading
		if (!nread) break;
	}
	gzclose(gzfile);

	//fix the size
	filesize=ndone;
	buff=(unsigned char*)MemPool::getSingleton()->realloc(buffsize,buff,filesize);

	//force zero termination
	if (bZeroTerminated && filesize && buff[filesize-1]!=0)
	{
		buff=(unsigned char*)MemPool::getSingleton()->realloc(filesize,buff,filesize+1);
		buff[filesize++]=0;
	}
	
	return buff;
}


/////////////////////////////////////////////////
/////////////////////////////////////////////////
uint64 FileSystem::FileSize(std::string filename)
{
	filename=FullPath(filename);
	struct stat __buf;
	stat(filename.c_str(), &__buf);
	return  __buf.st_size;
}


/////////////////////////////////////////////////
/////////////////////////////////////////////////
bool FileSystem::FileExists(std::string filename)
{
	filename=FullPath(filename);
	FILE* file= fopen( filename.c_str(), "rb" );
	bool ret=(file != NULL );
	if (file) fclose(file);
	return ret;
}

