#include <xge/xge.h>
#include <xge/encoder.h>
#include <xge/mempool.h>

static unsigned char alphabet[65]= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static bool base64dtable_init_needed=true;
static char inalphabet[256];
static char decoder[256];


/////////////////////////////////////////////////////////////////
Encoder::Encoder()
{
	this->char_count=0;
	this->bits=0;
	this->buffersize=1024; //start with this default size
	this->buffer=(unsigned char*)MemPool::getSingleton()->malloc(this->buffersize);
	this->pos   =0;
}

/////////////////////////////////////////////////////////////////
Decoder::Decoder(const char* src)
{
	Init(src);
}

/////////////////////////////////////////////////////////////////
Decoder::Decoder(const std::string& src)
{
	Init(src.c_str());
}


/////////////////////////////////////////////////////////////////
void Decoder::Init(const char* src)
{
	//one time table build for decoder
	if (base64dtable_init_needed==true)
	{
		base64dtable_init_needed=false;
		for (int i = sizeof(alphabet) - 1; i >= 0 ; i--) 
		{
			inalphabet[alphabet[i]] = 1;
			decoder   [alphabet[i]] = i;
		}
	}

	this->char_count     = 0;
	this->bits           = 0;
	this->tohandle_first = 1;
	this->tohandle_last  = 0;
	this->pos            = 0;
	this->__eof          = false;
	this->buffersize     = strlen(src);
	this->buffer         = (unsigned char*)MemPool::getSingleton()->malloc(this->buffersize);
	memcpy(this->buffer,src,this->buffersize);
}

/////////////////////////////////////////////////////////////////
Encoder::~Encoder()
{
	//deallocate temporary buffer
	MemPool::getSingleton()->free(this->buffersize,this->buffer); 
}

/////////////////////////////////////////////////////////////////
Decoder::~Decoder()
{
	//deallocate temporary buffer
	MemPool::getSingleton()->free(this->buffersize,this->buffer); 
}


/////////////////////////////////////////////////////////////////
void Decoder::Decode()
{
	//safety check
	DebugAssert(tohandle_first>tohandle_last);

	while (pos<buffersize)
	{
		int c=buffer[pos++];

		//the end!
		if (c == '=')
		{
			DebugAssert(char_count==2 || char_count==3);

			if (char_count==2)
			{
				tohandle[0]=((bits >> 10));
				tohandle_first=0;
				tohandle_last =0;
			}
			else
			{
				tohandle[0]=((bits >> 16));
				tohandle[1]=(((bits >> 8) & 0xff));
				tohandle_first=0;
				tohandle_last =1;
			}

			__eof=true;
			return;
		}

		DebugAssert(c>=0 && c<256 && inalphabet[c]);
		bits += decoder[c];

		char_count++;
		if (char_count == 4) 
		{
			tohandle[0]=((bits >> 16)       );
			tohandle[1]=((bits >> 8 ) & 0xff);
			tohandle[2]=((bits      ) & 0xff);
			bits = 0;
			char_count = 0;
			tohandle_first=0;
			tohandle_last =2;
			return; //important
		} 
		else 
		{
			bits <<= 6;
		}
	}

	ReleaseAssert(false);
}


/////////////////////////////////////////////////////////////////
inline void Encoder::NeedBytes(int howmuch)
{
	//reallocate only there is not enough space
	if ((pos+howmuch)>buffersize)
	{
		int new_buffersize=max2(buffersize*2,pos+howmuch);
		this->buffer=(unsigned char*)MemPool::getSingleton()->realloc(buffersize,buffer,new_buffersize);
		this->buffersize=new_buffersize;
	}
	DebugAssert((pos+howmuch)<=buffersize);
}



/////////////////////////////////////////////////////////////////
const char* Encoder::c_str() 
{
	//last four bytes+padding
	if (char_count)
	{
		DebugAssert(char_count==1 || char_count==2);
		bits <<= 16 - (8 * char_count);
		NeedBytes(4);
		unsigned char* p=this->buffer+pos;
		*p++=alphabet[(bits >> 18)];
		*p++=alphabet[(bits >> 12) & 0x3f];
		*p++=(char_count == 1)?'=':alphabet[(bits >> 6) & 0x3f];
		*p++='=';
		pos+=4;
		char_count=0;
		bits = 0;
	}

	//append a zero at the end
	if (this->buffer[this->pos-1]!=0)
	{
		NeedBytes(1);
		this->buffer[this->pos++]=0;
	}

	return (char*)this->buffer;
}

/////////////////////////////////////////////////////////////////
std::string Encoder::str()
{
	return this->c_str();
}

/////////////////////////////////////////////////////////////////
void Encoder::WriteRaw(int size,unsigned char* buffer)
{
	for (int i=0;i<size;i++)
	{
		int c=buffer[i];
		bits += c;
		char_count++;

		//can flush the content in bits
		if (char_count == 3) 
		{
			NeedBytes(4);
			unsigned char* p=this->buffer+pos;
			*p++=alphabet[(bits >> 18)       ];
			*p++=alphabet[(bits >> 12) & 0x3f];
			*p++=alphabet[(bits >> 6 ) & 0x3f];
			*p++=alphabet[(bits      ) & 0x3f];
			pos+=4;
			bits = 0;
			char_count = 0;
		} 
		else 
		{
			bits <<= 8;
		}
	}
}

/////////////////////////////////////////////////////////////////
void Decoder::ReadRaw(int size,unsigned char* dest)
{
	for (int i=0;i<size;i++)
	{
		if (tohandle_first>tohandle_last) 
			Decode(); //no data available, decode64 4 bytes->3 bytes
		(*dest++)=tohandle[tohandle_first++];
	}
}



//////////////////////////////////////////////////////////////
int Encoder::SelfTest()
{
	Log::printf("Testing Encoder/Decoder...\n");

	int64 myint64=0;
	myint64&=0x0fffffff;
	myint64<<=32;
	myint64 |=0xffffffff;

	uint64 myuint64=0;
	myuint64=0x0fffffff;
	myuint64<<=32;
	myuint64 |=0xffffffff;

	//basic test
	{
		char buff1[4]={10,-11,12,-13};
		char buff2[4]={ 0, 0,  0, 0};
		Encoder encoder;
		encoder.WriteChar     (1);
		encoder.WriteUchar    (2);
		encoder.WriteShort    (3);
		encoder.WriteUshort   (4);
		encoder.WriteInt      (5);
		encoder.WriteUint     (6);
		encoder.WriteInt64    (myint64);
		encoder.WriteUint64   (myuint64);
		encoder.WriteFloat    (9.1f);
		encoder.WriteDouble   (10.2);

		encoder.WriteRaw(sizeof(buff1),(unsigned char*)buff1);

		std::string encoded_text=encoder.str();

		Decoder decoder(encoded_text);
		ReleaseAssert(decoder.ReadChar     ()==1);
		ReleaseAssert(decoder.ReadUchar    ()==2);
		ReleaseAssert(decoder.ReadShort    ()==3);
		ReleaseAssert(decoder.ReadUshort   ()==4);
		ReleaseAssert(decoder.ReadInt      ()==5);
		ReleaseAssert(decoder.ReadUint     ()==6);
		ReleaseAssert(decoder.ReadInt64    ()==myint64);
		ReleaseAssert(decoder.ReadUint64   ()==myuint64);
		ReleaseAssert(decoder.ReadFloat    ()==9.1f);
		ReleaseAssert(decoder.ReadDouble   ()==10.2);

		decoder.ReadRaw(sizeof(buff2),(unsigned char*)buff2);
		ReleaseAssert(!memcmp(buff1,buff2,sizeof(buff1)));
	}	

	//more complete test (test alignment!)
	{
		for (int buffsize=1;buffsize<128;buffsize++)
		{
			char* temp1=new char[buffsize];
			char* temp2=new char[buffsize];

			//test read and write in chunks
			for (int chunk_to_write=1;chunk_to_write<=buffsize;chunk_to_write++)
			for (int chunk_to_read =1;chunk_to_read <=buffsize;chunk_to_read ++ )
			{
				int j;

				//fill with random numbers
				for (j=0;j<buffsize;j++) 
				{
					temp1[j]=rand() % 256;
					temp2[j]=rand() % 256;
				}

				//write
				Encoder encoder; 

				int towrite=0;
				for (j=0;j<buffsize;j+=towrite)
				{
					towrite=chunk_to_write;
					if ((j+towrite)>buffsize) towrite=buffsize-j;
					encoder.WriteRaw(towrite,(unsigned char*)(temp1+j));
				}
				ReleaseAssert(j==buffsize);

				//deserialize
				std::string encoded_text=encoder.str();
				Decoder decoder(encoded_text);

				//read in chunks to test decode64
				int toread=0;
				for (j=0;j<buffsize;j+=toread) 
				{
					toread=chunk_to_read;
					if ((j+toread)>buffsize) toread=buffsize-j;
					decoder.ReadRaw(toread,(unsigned char*)(temp2+j));
				}
				ReleaseAssert(j==buffsize);

				//test the 2 buffers are now the same
				ReleaseAssert(!memcmp(temp1,temp2,buffsize));

			}

			delete [] temp1;
			delete [] temp2;
		}
	}

	return 0;
}


//////////////////////////////////////////////////////////////
int Decoder::SelfTest()
{
	return 0;
}