#include <xge/xge.h>
#include <xge/plasm.h>

#include <tinyxml/tinyxml.h>




//______________________________________________________________________________
static TiXmlNode* getChild(TiXmlNode* xnode,const char* name)
{
	for (TiXmlNode* xchild = xnode->FirstChild(); xchild != 0; xchild = xchild->NextSibling()) 
	{
		if (!strcmpi(xchild->Value(),name))
			return xchild;
	}
	return 0;
}

//______________________________________________________________________________
static TiXmlAttribute* getAttribute(TiXmlNode* xnode,const char* name)
{
	for (TiXmlAttribute* att=((TiXmlElement*)xnode)->FirstAttribute();att;att=att->Next())
	{
		if (!strcmp(name,att->Name()))
			return att;
	}
	return 0;
}

static std::string prefix="";

//______________________________________________________________________________
static SmartPointer<Hpc> openXmlNode(TiXmlNode* xnode,std::map<int, SmartPointer<Vector>  >& arrays)
{
	ReleaseAssert(!strcmpi(xnode->Value(),"node"));
	SmartPointer<Hpc> node(new Hpc);

	SmartPointer<Batch> batch(new Batch());

	//parse attributees
	for (TiXmlAttribute* att=((TiXmlElement*)xnode)->FirstAttribute();att;att=att->Next())
	{
		const char* att_name=att->Name();
		if (!strcmp("name",att_name)) 
		{
			if (!node->prop) node->prop.reset(new PropertySet);
			(*node->prop)[HPC_PROP_NAME]=std::string(att->Value());
			continue;
		}
		if (!strcmp("pointdim",att_name)) 
		{
			node->pointdim=atoi(att->Value());
			continue;
		}
		if (!strcmp("spacedim",att_name)) 
		{
			node->spacedim=atoi(att->Value());
			continue;
		}
		if (!strcmp("vmat",att_name)) 
		{
			node->vmat.reset(new Matf(node->spacedim));
			Vector::parse((node->spacedim+1)*(node->spacedim+1),(float*)node->vmat->mem,att->Value(),(char*)"%e");
			node->hmat.reset(new Matf(node->vmat->invert()));
			continue;
		}

		if (!strcmp("gltype",att_name))
		{
			batch->primitive=atoi(att->Value());
			continue;
		}

		if (!strcmp("VRMLmaterial" ,att_name)) 
		{
			if (!node->prop) node->prop.reset(new PropertySet);
			(*node->prop)[HPC_PROP_VRML_MATERIAL]=att->Value();
			continue;
		}

		if (!strcmp("VRMLtexture" ,att_name)) 
			continue;

		if (!strcmp("geometry" ,att_name)) 
			continue;

		if (!strcmp("vertices" ,att_name)) 
		{
			batch->vertices=arrays[atoi(att->Value())];
			continue;
		}

		if (!strcmp("normals" ,att_name)) 
		{
			batch->normals=arrays[atoi(att->Value())];
			continue;
		}

		if (!strcmp("skin",att_name)) 
		{
			batch->texture0=Texture::open(att->Value());
			if (batch->texture0->filename[0]!=':') batch->texture0->filename=prefix + batch->texture0->filename;
			continue;
		}

		if (!strcmp("skincoords" ,att_name)) 
		{
			batch->texture0coords=arrays[atoi(att->Value())];
			continue;
		}

		if (!strcmp("light" ,att_name)) 
		{
			batch->texture1=Texture::open(att->Value());
			if (batch->texture1->filename[0]!=':') batch->texture1->filename=prefix + batch->texture1->filename;
			continue;
		}

		if (!strcmp("lightcoords" ,att_name)) 
		{
			batch->texture1coords=arrays[atoi(att->Value())];
			continue;
		}

		//unknown node type
		ReleaseAssert(false);
	}

	if (batch->primitive>=0)
		node->batches.push_back(batch);

	//sub nodes
	for (TiXmlNode* xchild = xnode->FirstChild(); xchild != 0; xchild = xchild->NextSibling()) 
	{
		//other old format!
		if (!strcmpi("compiled" ,xchild->Value())) 
		{
			SmartPointer<Batch> batch(new Batch);

			for (TiXmlAttribute* att=((TiXmlElement*)xchild)->FirstAttribute();att;att=att->Next())
			{
				if (!strcmp("primitive",att->Name()))
				{
					batch->primitive=atoi(att->Value());
					continue;
				}

				if (!strcmp("vertices" ,att->Name())) 
				{
					batch->vertices=arrays[atoi(att->Value())];
					continue;
				}

				if (!strcmp("normals" ,att->Name())) 
				{
					batch->normals=arrays[atoi(att->Value())];
					continue;
				}

				if (!strcmp("texture0" ,att->Name())) 
				{
					batch->texture0=Texture::open(att->Value());
					if (batch->texture0->filename[0]!=':') batch->texture0->filename=prefix + batch->texture0->filename;
					continue;
				}

				if (!strcmp("texture0coords" ,att->Name())) 
				{
					batch->texture0coords=arrays[atoi(att->Value())];
					continue;
				}

				if (!strcmp("texture1" ,att->Name())) 
				{
					batch->texture1=Texture::open(att->Value());
					if (batch->texture1->filename[0]!=':') batch->texture1->filename=prefix + batch->texture1->filename;
					continue;
				}

				if (!strcmp("texture1coords" ,att->Name())) 
				{
					batch->texture1coords=arrays[atoi(att->Value())];
					continue;
				}
			}

			node->batches.push_back(batch);
		}
		else
		{
			node->childs.push_back(openXmlNode(xchild,arrays));
		}
	}

	return node;
}



///////////////////////////////////////////////////////////////////////////
void Plasm::convertOldXml(char* infilename,char* outfilename,char* prefix)
{
	::prefix=std::string(prefix);

	TiXmlDocument doc;

	unsigned long filesize;
	unsigned char* buff=FileSystem::ReadFile(infilename,filesize,true); 

	if (!doc.Parse((const char*)buff))
	{
		MemPool::getSingleton()->free(filesize,buff);
		Utils::Error(HERE,"Failed to open XML fle %s [%d] %s",infilename,doc.ErrorRow(),doc.ErrorDesc());
	}
	
	MemPool::getSingleton()->free(filesize,buff);

	TiXmlNode* xnode=doc.FirstChild();
	const char* xname=xnode->Value();
	ReleaseAssert(!strcmpi(xname,"mesh"));
	
	//geometry
	std::map<int, SmartPointer<Vector >  > arrays;
	TiXmlNode* xgeometry=getChild(xnode,"arrays");
	for (TiXmlNode* xchild= xgeometry->FirstChild(); xchild != 0; xchild = xchild->NextSibling())
	{
		ReleaseAssert(!strcmpi(xchild->Value(),"array"));

		int id    =atoi(getAttribute(xchild,"id"  )->Value());
		int size  =atoi(getAttribute(xchild,"size")->Value());

		SmartPointer<Vector> arr(new Vector(size));
		Vector::parse(size,arr->mem(),xchild->FirstChild()->Value(),(char*)"%e");
		arrays[id]=arr;
	}

	SmartPointer<Hpc> hpc=openXmlNode(getChild(xnode,"node"),arrays);
	Log::printf("Opened XML file %s\n",infilename);

	Plasm::save(hpc,outfilename);
}

	

