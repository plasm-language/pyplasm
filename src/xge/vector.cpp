
#include <xge/xge.h>
#include <xge/vector.h>



  
 
////////////////////////////////////////////////////
Vector::~Vector()
{
	gpu.reset();
	if (pointer) MemPool::getSingleton()->free(sizeof(float)*num,pointer);
}

  
////////////////////////////////////////////////////
void Vector::parse(int num,float* dest,const char* source,char* _format)
{
	char format[256];
	sprintf(format,"%s%%num",_format);
	const char* ptr=source;
	for (int i=0,howmuch;i<num;i++)
	{
		float aux;
		sscanf(ptr,format,&aux,&howmuch);
		dest[i]=aux;
		ptr+=howmuch;
	}
}


//////////////////////////////////////////////////////////////
int Vector::SelfTest()
{
	Log::printf("Testing Vector...\n");

	Vector floats(3);
	floats.set(0,0);
	floats.set(1,1);
	floats.set(2,2);
	XgeReleaseAssert(floats.size()==3 && floats.memsize()==(sizeof(float)*3) && floats[0]==0 && floats[1]==1 && floats[2]==2);

	//test copu constructor
	Vector floats_bis(floats);
	XgeReleaseAssert(floats_bis.size()==3 && floats_bis[0]==0 && floats_bis[1]==1 && floats_bis[2]==2);

	//test constructor from stl vector
	{
		std::vector<float> temp;
		temp.push_back(10);
		temp.push_back(20);
		floats=Vector(temp);
		XgeReleaseAssert(floats.size()==2 && floats.memsize()==(sizeof(float)*2) && floats[0]==10 && floats[1]==20);
	}

	//test constructor from indices
	{
		std::vector<int  > indices;
		std::vector<float> temp;
		indices.push_back(0);
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(1);
		temp.push_back(10);
		temp.push_back(20);

		Vector floats=Vector(indices,temp);
		XgeReleaseAssert(floats.size()==4 && floats[0]==10 && floats[1]==10 && floats[2]==20 && floats[3]==20);

		floats=Vector(indices,&temp[0]);
		XgeReleaseAssert(floats.size()==4 && floats[0]==10 && floats[1]==10 && floats[2]==20 && floats[3]==20);
	

		indices.clear();
		indices.push_back(0);
		indices.push_back(3);
		floats=floats.extract(indices,false);
		XgeReleaseAssert(floats.size()==2 && floats[0]==10 && floats[1]==20);

		floats.zero();
		XgeReleaseAssert(floats.size()==2 && floats[0]==0 && floats[1]==0);
	}

	return 0;
}


