#include <xge/xge.h>
#include <xge/filesystem.h>

#include <sys/stat.h>
#include <zlib.h>

#include <JUCE/AppConfig.h>
#include <JUCE/modules/juce_core/juce_core.h>



////////////////////////////////////////////////////////////////////
unsigned char* FileSystem::ReadFile(std::string filename,unsigned long& filesize,bool bZeroTerminated)
{
	static const int megabyte=1024*1024;

	unsigned long buffsize=0;
	unsigned char* buff=0;

	//i can read always using gz
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



