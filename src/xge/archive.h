#ifndef _ARCHIVE_H__
#define _ARCHIVE_H__

#include <xge/xge.h>
#include <xge/mempool.h>



/////////////////////////////////////////////////
//main class to archive objects
/////////////////////////////////////////////////

class Archive
{
public:

	//! constructor
	Archive();

	//! destructor
	~Archive();

	//! open the file
	bool Open(std::string filename,bool bWriteMode);

	//! close the file
	void Close();

	//! test if saving
	inline bool Saving    () {return (this->gzfile || this->file) &&  this->bWriteMode;}
	inline bool Loading   () {return (this->gzfile || this->file) && !this->bWriteMode;}
	inline bool TextMode  () {return (this->gzfile || this->file) &&  this->bTextMode ;}
	inline bool BinaryMode() {return (this->gzfile || this->file) && !this->bTextMode ;}

	//! utils for building the structure
	void Push   (std::string name);
	void Pop    (std::string name);

	//! read and write
	
	void                                WriteString      (std::string name,std::string value       );
	void                                WriteInt         (std::string name,int value               );
	void                                WriteUint        (std::string name,unsigned int value      );
	void                                WriteUint64      (std::string name,uint64 value            );
	void                                WriteFloat       (std::string name,float value             );
	void                                WriteVectorInt   (std::string name,std::vector<int> value);
	void                                WriteVectorFloat (std::string name,std::vector<float> value);

	std::string                         ReadString       (std::string name);
	int                                 ReadInt          (std::string name);
	unsigned int                        ReadUint         (std::string name);
	uint64                              ReadUint64       (std::string name);
	float                               ReadFloat        (std::string name);
	std::vector<int>                    ReadVectorInt    (std::string name);
	std::vector<float>                  ReadVectorFloat  (std::string name);

	//! raw write
	void                                WriteRaw         (std::string name,char* p,int memsize);
	void                                ReadRaw          (std::string name,char* p,int memsize);

	bool								ExistsNode		 (std::string name);

	//specific for C++
	template <class T>
	void  WriteSmartPointer(SmartPointer<T> ptr)
	{
		uint64 UID=(uint64)ptr.get();
		WriteUint64("UID",UID);

		//first time?
		if (UID && smart_pointers.find(UID)==smart_pointers.end())
		{
			smart_pointers[UID]=UID;
			ptr->Write(*this);
		}
	}

	//specific for C++
	template <class T>
	SmartPointer<T> ReadSmartPointer()
	{
		SmartPointer<T> ptr;
		
		uint64 UID=ReadUint64("UID");

		if (UID)
		{
			//first time?
			if (smart_pointers.find(UID)==smart_pointers.end())
			{
				T* obj=new T();
				obj->Read(*this);
				smart_pointers[UID]=(uint64)(new SmartPointer<T>(obj));
			}
			ptr=*((SmartPointer<T>*)smart_pointers[UID]);
		}

		return ptr;
	}

protected:

	//! for gzipped file (extension is gz)
	void* gzfile;

	//! for regular file (extension not gz)
	FILE* file;

	//! loading or saving
	bool bWriteMode;

	//! text mode (==use xml output, otherwise use binary output)
	bool bTextMode;

	//special for smart pointers
	std::map<uint64,uint64> smart_pointers;
	
	void   InnerWrite (void* buffer,int memsize);
	void   InnerRead  (void* buffer,int memsize);

	//! for bTextMode (xml output)
	typedef struct 
	{
		std::string name;
		void* first;
		void* second;
	}
	StackItem;

	std::stack<StackItem> xml_objects;

}; //end class

#endif //_ARCHIVE_H__






