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
	XgeDebugAssert(tohandle_first>tohandle_last);

	while (pos<buffersize)
	{
		int c=buffer[pos++];

		//the end!
		if (c == '=')
		{
			XgeDebugAssert(char_count==2 || char_count==3);

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

		XgeDebugAssert(c>=0 && c<256 && inalphabet[c]);
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

	XgeReleaseAssert(false);
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
	XgeDebugAssert((pos+howmuch)<=buffersize);
}



/////////////////////////////////////////////////////////////////
const char* Encoder::c_str() 
{
	//last four bytes+padding
	if (char_count)
	{
		XgeDebugAssert(char_count==1 || char_count==2);
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


