#include <xge/xge.h>
#include <xge/mempool.h>
#include <xge/archive.h>
#include <xge/vec.h>
#include <xge/encoder.h>

 
#include <zlib.h>
#include <tinyxml/tinyxml.h>

#define CURNODE ((TiXmlNode*)xml_objects.top().first)


////////////////////////////////////////////////////////////
Archive::Archive()
{
	this->bWriteMode= false;
	this->bTextMode = false;
	this->file=0;
	this->gzfile=0;
}


////////////////////////////////////////////////////////////
Archive::~Archive()
{
	if (Saving() || Loading())
		this->Close();
}


////////////////////////////////////////////////////////////
bool Archive::Open(std::string filename,bool bWriteMode)
{
	if (Loading() || Saving())
		Close();

	this->bWriteMode  = bWriteMode;
	this->bTextMode   = Utils::ToLower(filename).find(".xml")!=std::string::npos;
	bool bZip         = Utils::ToLower(filename).find( ".gz")!=std::string::npos;

	std::string smode=std::string(bWriteMode?"w":"r") + std::string((bTextMode && !bZip)?"t":"b") ;

	this->file=0;
	this->gzfile=0;

	if (!bZip)
	{
		this->file=fopen(filename.c_str(),smode.c_str());
		if (!this->file) return false;
	}
	else
	{
		this->gzfile=gzopen(filename.c_str(),smode.c_str());
		if (!this->gzfile) return false;
	}

	//if xml output, prepare the XML document
	if (bTextMode)
	{
		TiXmlDocument* xml_doc  =new TiXmlDocument;

		if (Saving())
		{	
			xml_doc->LinkEndChild( new TiXmlDeclaration( "1.0", "", "" ) );
			xml_doc->LinkEndChild( new TiXmlElement("root") );
		}
		
		if (Loading())
		{
			unsigned long filesize;
			unsigned char* content=FileSystem::ReadFile(filename.c_str(),filesize,true);

			if (!xml_doc->Parse((const char*)content))
			{
				Log::printf("Failed of open_xml for file %s ErrorRow(%d) ErrorCol(%d) ErrorDesc(%s)",filename.c_str(),(int)xml_doc->ErrorRow(),(int)xml_doc->ErrorCol(),xml_doc->ErrorDesc());
				Close();
				return false;
			}
		}

		StackItem stack_doc ={"xml_doc",xml_doc                           ,0};xml_objects.push(stack_doc );
		StackItem stack_root={"root"   ,xml_doc->FirstChildElement("root"),0};xml_objects.push(stack_root);
	}

	return true;
}



////////////////////////////////////////////////////////////
void Archive::Close()
{
	//destroy the xml root (if exists)
	if (xml_objects.size())
	{
		//take the document to deallocate it
		while (xml_objects.size()!=1) xml_objects.pop();
		TiXmlDocument* xml_doc=CURNODE->ToDocument();

		//close the file (==save it)
		if (Saving())
		{
			TiXmlPrinter printer;
			printer.SetIndent("\t");
			xml_doc->Accept(&printer);
			const char* content= printer.CStr();
			int len=(int)strlen(content)+1;

			bTextMode=false; //want the output to the real file
			InnerWrite((void*)content,len);
		}
		delete xml_doc;
	}
	xml_objects=std::stack<StackItem >();

	//destry the map for SmartPointer Objects
	if (this->Loading())
	{
		for (std::map<uint64,uint64>::iterator it=smart_pointers.begin();it!=smart_pointers.end();it++)
		{
				//see: http://www.boost.org/doc/libs/1_40_0/libs/smart_ptr/sp_techniques.html#preventing_delete
				//Interesting part: Using shared_ptr<void> to hold an arbitrary object
				//Checked and it seems to work (probably shared_ptr probably stores inside the correct deleter!)
				SmartPointer<void>* temp=(SmartPointer<void>*)(it->second);
				temp->reset();
				delete temp;
			
		}
	}
	smart_pointers.clear();

	//close the gzfile
	if (this->gzfile) gzclose((gzFile)this->gzfile);
	this->gzfile=0;

	//close the regular file
	if (this->file) fclose(this->file);
	this->file=0;
}



////////////////////////////////////////////////////////////

void Archive::InnerWrite(void* buffer,int memsize)
{
	if (bTextMode)
	{
		Encoder encoder;
		encoder.WriteRaw(memsize,(unsigned char*)buffer);
		const char* encoded_data=(const char*)encoder.c_str();

		TiXmlElement * xml_binary_element = new TiXmlElement("data");
		TiXmlText * xml_base64_content = new TiXmlText(encoded_data);
		xml_binary_element->LinkEndChild( xml_base64_content);
		CURNODE->LinkEndChild(xml_binary_element);
	}
	else
	{
		for (int ndone=0,nwrite=0;ndone!=memsize;ndone+=nwrite)
		{
			nwrite=(int)(gzfile?gzwrite((gzFile)gzfile,(char*)buffer+ndone,memsize-ndone):fwrite((char*)buffer+ndone,1,memsize-ndone,file));
			XgeReleaseAssert(nwrite);
		}
	}
}



////////////////////////////////////////////////////////////
void Archive::InnerRead(void* buffer,int memsize)
{
	if (bTextMode)
	{
		TiXmlNode* xml_child=CURNODE->IterateChildren("data",(TiXmlNode*)xml_objects.top().second);
		xml_objects.top().second=xml_child;
		const char* encoded_data=xml_child->ToElement()->GetText();
		Decoder decoder(encoded_data);
		decoder.ReadRaw(memsize,(unsigned char*)buffer);
	}
	else
	{
		for (int ndone=0,nread=0;ndone!=memsize;ndone+=nread)
		{
			nread=(int)(gzfile?gzread((gzFile)gzfile,(char*)buffer+ndone,memsize-ndone):fread((char*)buffer+ndone,1,memsize-ndone,file));
			XgeReleaseAssert(nread);
		}
	}
}



////////////////////////////////////////////////////////////
void Archive::Push(std::string name)
{
	if (bTextMode && Saving())
	{
		TiXmlElement * xml_element = new TiXmlElement(name.c_str());
		CURNODE->LinkEndChild(xml_element);

		StackItem stack_item={name,xml_element,0};
		xml_objects.push(stack_item);
		return;
	}

	if (bTextMode && Loading())
	{
		StackItem& current=xml_objects.top();
		TiXmlNode* xml_child=((TiXmlNode*)current.first)->IterateChildren((TiXmlNode*)current.second);
		current.second=xml_child;

		StackItem stack_item={name,xml_child,0};
		xml_objects.push(stack_item);
		return;
	}
}

////////////////////////////////////////////////////////////
void Archive::Pop(std::string name)
{
	if (bTextMode)
	{
		XgeReleaseAssert(xml_objects.top().name==name);
		xml_objects.pop();
	}
}

bool Archive::ExistsNode(std::string name)
{
	if (!bTextMode)
	{
		return false;
	}
	if (CURNODE->NoChildren())
	{
		return false;
	}
	TiXmlNode* child = CURNODE->FirstChild(name.c_str());
	return child != NULL;
}

////////////////////////////////////////////////////////////
void Archive::WriteInt(std::string name,int value)
{
	if (bTextMode)
		CURNODE->ToElement()->SetAttribute(name.c_str(),value);
	else
		InnerWrite((char*)&value,sizeof(int));
}

int Archive::ReadInt(std::string name)
{
	int value;

	if (bTextMode)
	{
		TiXmlElement* xml_element=CURNODE->ToElement();
		const char* bOk=xml_element->Attribute(name.c_str(),&value);
		XgeReleaseAssert(bOk);
	}
	else
		InnerRead((char*)&value,sizeof(int));

	return value;
}



////////////////////////////////////////////////////////////
void Archive::WriteUint(std::string name,unsigned int value)
{

	if (bTextMode)
		CURNODE->ToElement()->SetAttribute(name.c_str(),(int)value);
	else
		InnerWrite((char*)&value,sizeof(unsigned int));
}

unsigned int Archive::ReadUint(std::string name)
{
	unsigned int value;

	if (bTextMode)
	{
		int ivalue;

		TiXmlElement* element=CURNODE->ToElement();
		const char* bOk=element->Attribute(name.c_str(),&ivalue);
		XgeReleaseAssert(bOk);
		value=(unsigned int)ivalue;
	}
	else
		InnerRead((char*)&value,sizeof(unsigned int));
	
	return value;
}


////////////////////////////////////////////////////////////
void Archive::WriteUint64(std::string name,uint64 value)
{
	if (bTextMode)
	{
		unsigned int low=(unsigned int)((value    ) & (0xffffffff));
		unsigned int high=(unsigned int)((value>>32) & (0xffffffff));
		std::string svalue=(high)?Utils::Format("%u %u",high,low):Utils::Format("%u",low);
		WriteString(name,svalue);
	}
	else
	{
		InnerWrite((char*)&value,sizeof(uint64));
	}
}

uint64 Archive::ReadUint64(std::string name)
{
	uint64 value;

	if (bTextMode)
	{	
		unsigned int low=0;
		unsigned int high=0;
		std::string svalue=ReadString(name);

		if (svalue.find(" ")!=std::string::npos)
			sscanf(svalue.c_str(),"%u %u",&high,&low);
		else
			sscanf(svalue.c_str(),"%u",&low);

		value=((uint64)(low)) | (((uint64)high)<<32);
	}
	else
	{
		InnerRead((char*)&value,sizeof(uint64));
	}

	return value;
}



////////////////////////////////////////////////////////////
void Archive::WriteFloat(std::string name,float value)
{
	if (bTextMode)
		CURNODE->ToElement()->SetDoubleAttribute(name.c_str(),(double)value);
	else
		InnerWrite((char*)&value,sizeof(float));
}

float Archive::ReadFloat(std::string name)
{
	float value;

	if (bTextMode)
	{
		double dvalue;
		const char* bOk=CURNODE->ToElement()->Attribute(name.c_str(),&dvalue);
		XgeReleaseAssert(bOk);
		value=(float)dvalue;
	}
	else
		InnerRead((char*)& value,sizeof(float));
	
	return value;
}


////////////////////////////////////////////////////////////
void Archive::WriteString(std::string name,std::string value)
{

	if (bTextMode)
	{
		CURNODE->ToElement()->SetAttribute(name.c_str(),value.c_str());
	}
	else
	{
		int len=(int)value.length();
		InnerWrite(&len,sizeof(int));
		InnerWrite((void*)value.c_str(),len+1);
	}

}

std::string Archive::ReadString(std::string name)
{
	std::string value;

	if (bTextMode)
	{
		const char* svalue=CURNODE->ToElement()->Attribute(name.c_str());
		XgeReleaseAssert(svalue);
		value=svalue;
	}
	else
	{
		int len;
		InnerRead(&len,sizeof(int));
		char* temp=(char*)MemPool::getSingleton()->malloc(len+1);
		InnerRead(temp,len+1);
		XgeDebugAssert(temp[len]==0);
		value=temp;
		MemPool::getSingleton()->free(len+1,temp);
	}
	
	return value;
}

////////////////////////////////////////////////////////////
void Archive::WriteRaw(std::string name,char* p,int memsize)
{

	if (bTextMode)
	{
		Encoder encoder;
		encoder.WriteRaw(memsize,(unsigned char*)p);
		const char* encoded_data=(const char*)encoder.c_str();
		TiXmlElement * xml_binary_element = new TiXmlElement(name.c_str());
		TiXmlText * xml_base64_content = new TiXmlText(encoded_data);
		xml_binary_element->LinkEndChild( xml_base64_content);
		CURNODE->LinkEndChild(xml_binary_element);
	}
	else
	{
		InnerWrite(p,memsize);
	}
}

void Archive::ReadRaw(std::string name,char* p,int memsize)
{
	if (bTextMode)
	{
		TiXmlNode* xml_child=CURNODE->IterateChildren(name.c_str(),(TiXmlNode*)xml_objects.top().second);
		xml_objects.top().second=xml_child;
		const char* encoded_data=xml_child->ToElement()->GetText();
		Decoder decoder(encoded_data);
		decoder.ReadRaw(memsize,(unsigned char*)p);
	}
	else
	{
		InnerRead(p,memsize);
	}
}


////////////////////////////////////////////////////////////
void Archive::WriteVectorInt(std::string name,std::vector<int> v)
{
	Push(name);

	int num=(int)v.size();
	WriteInt("num",num);

	if (num)
	{
		if (TextMode())
		{
			std::string s;
			for (int i=0;i<(int)v.size();i++) s+=Utils::Format("%d ",v[i]);
			TiXmlText * xml_content = new TiXmlText(s.c_str());
			CURNODE->LinkEndChild(xml_content);
		}
		else
		{
			InnerWrite(&v[0],sizeof(int)*num);
		}
	}
	Pop(name);
}

std::vector<int> Archive::ReadVectorInt(std::string name)
{
	std::vector<int> v;

	Push(name);
	{
		int num=ReadInt("num");
		
		if (num)
		{
			v=std::vector<int>(num);

			if (TextMode())
			{
				Utils::ParseInts(num,&v[0],CURNODE->ToElement()->GetText(),"%d");
			}
			else
			{
				InnerRead(&v[0],sizeof(int)*num);
			}
		}
	}
	Pop(name);

	return v;
}


////////////////////////////////////////////////////////////
void Archive::WriteVectorFloat(std::string name,std::vector<float> v)
{
	Push(name);
	{
		int num=(int)v.size();
		WriteInt("num",num);

		if (num)
		{
			if (TextMode())
			{
				std::string s;
				for (int i=0;i<(int)v.size();i++) s+=Utils::Format("%f ",v[i]);
				TiXmlText * xml_content = new TiXmlText(s.c_str());
				CURNODE->LinkEndChild(xml_content);
			}
			else
			{
				InnerWrite(&v[0],sizeof(float)*num);
			}
		}
	}
	Pop(name);
}
std::vector<float> Archive::ReadVectorFloat(std::string name)
{
	std::vector<float> v;

	Push(name);
	{
		int num=ReadInt("num");

		if (num)
		{
			v=std::vector<float>(num);

			if (TextMode())
			{
				Utils::ParseFloats(num,&v[0],CURNODE->ToElement()->GetText(),"%f");
			}
			else
			{
				InnerRead(&v[0],sizeof(float)*num);
			}
		}
	}
	Pop(name);

	return v;
}



