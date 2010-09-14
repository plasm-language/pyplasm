#ifndef _ENCODER_H__
#define _ENCODER_H__

#include <xge/xge.h>

//=====================================================================
/*!
Classi responsabile della serializzazione/deserializzazione di
dati binari in una stringa.

Esempio tipico di uso:

	Encoder encoder;
	encoder.write_int32(100);
	encoder.write_string("hello");
	std::string encoded_data=encoder.str();

	Decoder decoder(encoded_data);
	int32 myint32=decoder.read_int32();
	ReleaseAssert(myint32==100);
	std::string mystring=decoder.read_string();
	ReleaseAssert(mystring=="hello");
*/
//=====================================================================

class XGE_API Encoder
{
public:

	//! constructor for encoding binary data
	Encoder();

	//! destructor
	~Encoder();

	//! general write for binary data 
	void            WriteRaw      (int size,unsigned char* buffer);

	//some utility to speed up the write
	inline void     WriteChar     (char             value) {WriteRaw(sizeof(char            ),(unsigned char*)&value);}
	inline void     WriteUchar    (unsigned char    value) {WriteRaw(sizeof(unsigned char   ),(unsigned char*)&value);}
	inline void     WriteShort    (short            value) {WriteRaw(sizeof(short           ),(unsigned char*)&value);}
	inline void     WriteUshort   (unsigned short   value) {WriteRaw(sizeof(unsigned short  ),(unsigned char*)&value);}
	inline void     WriteInt      (int              value) {WriteRaw(sizeof(int             ),(unsigned char*)&value);}
	inline void     WriteUint     (unsigned int     value) {WriteRaw(sizeof(unsigned int    ),(unsigned char*)&value);}
	inline void     WriteInt64    (int64            value) {WriteRaw(sizeof(int64           ),(unsigned char*)&value);}
	inline void     WriteUint64   (uint64           value) {WriteRaw(sizeof(uint64          ),(unsigned char*)&value);}
	inline void     WriteFloat    (float            value) {WriteRaw(sizeof(float           ),(unsigned char*)&value);}
	inline void     WriteDouble   (double           value) {WriteRaw(sizeof(double          ),(unsigned char*)&value);}
	
	//specific for string
	inline void WriteString(std::string value)
	{
		WriteInt((int)value.length());
		//include the final 0
		WriteRaw((int)value.length()+1,(unsigned char*)value.c_str());
	}
	
	//get raw text
	const char* c_str();

	//! get internal encoded text as STL string
	std::string str();

	//SelfTest
	static int SelfTest();

protected:

	//buffer
	int            buffersize;
	unsigned char* buffer;
	int            pos;
	int            char_count;
	int            bits;

	//dynamic realloc array to store bytes
	void NeedBytes(int howmuch);
};



//=====================================================================
//=====================================================================
class XGE_API Decoder
{
public:

	//! constructor for reading data from string
	Decoder(const char* src);

	//! constructor from STL string
	Decoder(const std::string& src);

	//! destructor
	~ Decoder();

	//general read 
	void             ReadRaw       (int size,unsigned char* dest);

	//some utility to speed up the read
	inline char            ReadChar      ()               {char           value;ReadRaw(sizeof(char             ),(unsigned char*)&value);return value;}
	inline unsigned char   ReadUchar     ()               {unsigned char  value;ReadRaw(sizeof(unsigned char    ),(unsigned char*)&value);return value;}
	inline short           ReadShort     ()               {short          value;ReadRaw(sizeof(short            ),(unsigned char*)&value);return value;}
	inline unsigned short  ReadUshort    ()               {unsigned short value;ReadRaw(sizeof(unsigned short   ),(unsigned char*)&value);return value;}
	inline int             ReadInt       ()               {int            value;ReadRaw(sizeof(int              ),(unsigned char*)&value);return value;}
	inline unsigned int    ReadUint      ()               {unsigned int   value;ReadRaw(sizeof(unsigned int     ),(unsigned char*)&value);return value;}
	inline int64           ReadInt64     ()               {int64          value;ReadRaw(sizeof(int64            ),(unsigned char*)&value);return value;}
	inline uint64          ReadUint64    ()               {uint64         value;ReadRaw(sizeof(uint64           ),(unsigned char*)&value);return value;}
	inline float           ReadFloat     ()               {float          value;ReadRaw(sizeof(float            ),(unsigned char*)&value);return value;}
	inline double          ReadDouble    ()               {double         value;ReadRaw(sizeof(double           ),(unsigned char*)&value);return value;}
	
	//! specific for string
	inline std::string  ReadString() 
	{
		int len=ReadInt();
		//include the final 0
		unsigned char* temp=(unsigned char*)MemPool::getSingleton()->malloc(len+1); 
		ReadRaw(len+1,temp);
		std::string ret((char*)temp);
		MemPool::getSingleton()->free(len+1,temp);
		return ret;
	}

	//! SelfTest
	static int SelfTest();

private:

	//internal function to decode bytes
	void Decode();

	//! init
	void Init(const char* src);

	//! internal variables
	int            buffersize;
	unsigned char* buffer;
	int            pos;
	int            char_count;
	int            bits;
	bool           __eof;

	char           tohandle[3];
	int            tohandle_first;
	int            tohandle_last;

}; //end Decoder classs


#endif//_ENCODER_H__

