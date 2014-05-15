#include <xge/xge.h>


////////////////////////////////////////////////////////////
int ArchiveSelfTest()
{
	Log::printf("Testing Archive...\n");

	std::vector<std::string> filenames;
	filenames.push_back(":temp.xml");
	filenames.push_back(":temp.xml.gz");
	filenames.push_back(":temp.bin");
	filenames.push_back(":temp.bin.gz");

	for (int i=0;i<(int)filenames.size();i++)
	{
		{
			Archive ar;
			XgeReleaseAssert(ar.Open(filenames[i],true));

			ar.WriteString("string","hello");
			ar.WriteInt("int"  ,20);
			ar.WriteFloat("float",30);

			ar.Push("char_p_4");
			char p[4]={1,2,3,4};
			ar.WriteRaw("data",p,sizeof(p));
			ar.Pop("char_p_4");

			ar.Push("vec3f");
			Vec3f(1,2,3).Write(ar);
			ar.Pop("vec3f");

			std::vector<int> std_vector_int;
			std_vector_int.push_back(1);
			std_vector_int.push_back(2);
			std_vector_int.push_back(3);
			ar.WriteVectorInt("std_vector_int",std_vector_int);

			std::vector<float> std_vector_float;
			std_vector_float.push_back(10);
			std_vector_float.push_back(20);
			std_vector_float.push_back(30);
			ar.WriteVectorFloat("std_vector_float",std_vector_float);

			std::vector<Vec3f> std_vector_vec3f;
			std_vector_vec3f.push_back(Vec3f(1,2,3));
			std_vector_vec3f.push_back(Vec3f(4,5,6));

			ar.Push("stl_vector_vec3f");
			{
				ar.WriteInt("num",std_vector_vec3f.size());

				for (int i=0;i<(int)std_vector_vec3f.size();i++)
				{
					ar.Push("Vec3f");
					std_vector_vec3f[i].Write(ar);
					ar.Pop("Vec3f");
				}
			}
			ar.Pop("stl_vector_vec3f");


			SmartPointer<Vec3f> sp_vec3f(new Vec3f(1,2,3));
			ar.Push("sp_vec3f");
			ar.WriteSmartPointer(sp_vec3f);
			ar.Pop("sp_vec3f");

			SmartPointer<Vec3f> sp_vec3f_null;
			ar.Push("sp_vec3f_null");
			ar.WriteSmartPointer(sp_vec3f_null);
			ar.Pop("sp_vec3f_null");
			ar.Push("sp_vec3f_bis");
			ar.WriteSmartPointer(sp_vec3f);
			ar.Pop("sp_vec3f_bis");

			std::vector<SmartPointer<Vec3f> > vector_sp_vec3f;
			vector_sp_vec3f.push_back(SmartPointer<Vec3f>(new Vec3f(1,2,3)));
			vector_sp_vec3f.push_back(SmartPointer<Vec3f>());
			vector_sp_vec3f.push_back(vector_sp_vec3f[0]);

			ar.Push("vector_sp_vec3f");
			{
				ar.WriteInt("num",(int)vector_sp_vec3f.size());

				for (int i=0;i<(int)vector_sp_vec3f.size();i++)
				{
					ar.Push("item");
					ar.WriteSmartPointer(vector_sp_vec3f[i]);
					ar.Pop("item");
				}
			}
			ar.Pop("vector_sp_vec3f");

			uint64 data_64bit=0xffffffff;
			data_64bit<<=32;
			data_64bit|=0xffffffff;
			--data_64bit;
			ar.WriteUint64("data_64bit",data_64bit);
			ar.Close();
		}

		{
			Archive ar;
			XgeReleaseAssert(ar.Open(filenames[i],false));

			std::string svalue=ar.ReadString("string");
			XgeReleaseAssert(svalue=="hello");

			int ivalue=ar.ReadInt("int");
			XgeReleaseAssert(ivalue==20);

			float fvalue=ar.ReadFloat("float");
			XgeReleaseAssert(fvalue==30);

			char p[4];
			ar.Push("char_p_4");
			ar.ReadRaw("data",p,sizeof(p));
			XgeReleaseAssert(p[0]==1 && p[1]==2 && p[2]==3 && p[3]==4);
			ar.Pop("char_p_4");

			Vec3f vec3f;
			ar.Push("vec3f");
			vec3f.Read(ar);
			ar.Pop("vec3f");
			XgeReleaseAssert(vec3f.x==1 && vec3f.y==2 && vec3f.z==3);

			std::vector<int> std_vector_int=ar.ReadVectorInt("stl_vector_int");
			XgeReleaseAssert(std_vector_int.size()==3 && std_vector_int[0]==1 && std_vector_int[1]==2 && std_vector_int[2]==3);

			std::vector<float> std_vector_float=ar.ReadVectorFloat("std_vector_float");
			XgeReleaseAssert(std_vector_float.size()==3 && std_vector_float[0]==10 && std_vector_float[1]==20 && std_vector_float[2]==30);

			std::vector<Vec3f> std_vector_vec3f;
			ar.Push("stl_vector_vec3f");
			{
				int num=ar.ReadInt("num");
				std_vector_vec3f=std::vector<Vec3f>(num);

				for (int i=0;i<num;i++)
				{
					ar.Push("item");
					std_vector_vec3f[i].Read(ar);
					ar.Pop("item");
				}
			}
			ar.Pop("stl_vector_vec3f");
			XgeReleaseAssert(std_vector_vec3f.size()==2 && std_vector_vec3f[0]==Vec3f(1,2,3) && std_vector_vec3f[1]==Vec3f(4,5,6));

			{
				SmartPointer<Vec3f> sp_vec3f;
				ar.Push("sp_vec3f");
				sp_vec3f=ar.ReadSmartPointer<Vec3f>();
				ar.Pop("sp_vec3f");
				XgeReleaseAssert((*sp_vec3f)==Vec3f(1,2,3));

				SmartPointer<Vec3f> sp_vec3f_null;
				ar.Push("sp_vec3f_null");
				sp_vec3f_null=ar.ReadSmartPointer<Vec3f>();
				ar.Pop("sp_vec3f_null");
				XgeReleaseAssert(!sp_vec3f_null);

				SmartPointer<Vec3f> sp_vec3f_bis;
				ar.Push("sp_vec3f_bis");
				sp_vec3f_bis=ar.ReadSmartPointer<Vec3f>();
				ar.Pop("sp_vec3f_bis");
				XgeReleaseAssert((*sp_vec3f_bis)==Vec3f(1,2,3) && sp_vec3f_bis.get()==sp_vec3f.get() && sp_vec3f_bis.use_count()==3); //2 counter from this, one from archive
			}
				
			{
				std::vector<SmartPointer<Vec3f> > vector_sp_vec3f;

				ar.Push("vector_sp_vec3f");
				{
					int num=ar.ReadInt("num");
					vector_sp_vec3f=std::vector<SmartPointer<Vec3f> >(num);

					for (int i=0;i<num;i++)
					{
						ar.Push("item");
						vector_sp_vec3f[i]=ar.ReadSmartPointer<Vec3f>();
						ar.Pop("item");
					}
				}
				ar.Pop("vector_sp_vec3f");

				
				XgeReleaseAssert(vector_sp_vec3f.size()==3 && *vector_sp_vec3f[0]==Vec3f(1,2,3) && !vector_sp_vec3f[1] && *vector_sp_vec3f[2]==Vec3f(1,2,3) );
				XgeReleaseAssert(vector_sp_vec3f[0].get()==vector_sp_vec3f[2].get());
			}

			uint64 data_64bit=ar.ReadUint64("data_64bit");

			uint64 data_64bit_check=0xffffffff;
			data_64bit_check<<=32;
			data_64bit_check|=0xffffffff;
			--data_64bit_check;

			XgeReleaseAssert(data_64bit==data_64bit_check);

			ar.Close();
		}
	}

	return 0;
}


//////////////////////////////////////////////////////////////
int ArraySelfTest()
{
	Log::printf("Testing Array...\n");

	Array floats(3);
	floats.set(0,0);
	floats.set(1,1);
	floats.set(2,2);
	XgeReleaseAssert(floats.size()==3 && floats.memsize()==(sizeof(float)*3) && floats[0]==0 && floats[1]==1 && floats[2]==2);

	//test copu constructor
	Array floats_bis(floats);
	XgeReleaseAssert(floats_bis.size()==3 && floats_bis[0]==0 && floats_bis[1]==1 && floats_bis[2]==2);

	//test constructor from stl vector
	{
		std::vector<float> temp;
		temp.push_back(10);
		temp.push_back(20);
		floats=Array(temp);
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

		Array floats=Array(indices,temp);
		XgeReleaseAssert(floats.size()==4 && floats[0]==10 && floats[1]==10 && floats[2]==20 && floats[3]==20);

		floats=Array(indices,&temp[0]);
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



/////////////////////////////////////////////////////////
int BakeSelfTest()
{
	Log::printf("Testing Bake::..\n");

	Bake bake;
	bake.DebugMode=false;
	bake.PointOcclusion=false;
	std::vector< SmartPointer<Batch> > batches=bake.Unwrap
	(
		Plasm::getBatches(Plasm::open(":resources/models/temple.hpc.xml")) ,
		5.0f,
		":resources/models/temple.%02d.png",
		1024
	);
	Batch::Save(":resources/models/temple.ao.mesh.gz",batches);
	bake.Add(batches);
	bake.Export();
	bake.run();
	bake.PostProcess();
	SmartPointer<Octree> octree(new Octree(batches));
	GLCanvas glcanvas;
  glcanvas.setOctree(octree);
	glcanvas.runLoop();
	return 0;
}



//////////////////////////////////////////////////////////////////////
int Ball3fSelfTest()
{
	Log::printf("Testing Ball3f...\n");

	//add a point
	std::vector<float> points;
	

	points.push_back(-1);points.push_back(-1);points.push_back(-1);
	points.push_back(+1);points.push_back(-1);points.push_back(-1);
	points.push_back(+1);points.push_back(+1);points.push_back(-1);
	points.push_back(-1);points.push_back(+1);points.push_back(-1);
	points.push_back(-1);points.push_back(-1);points.push_back(+1);
	points.push_back(+1);points.push_back(-1);points.push_back(+1);
	points.push_back(+1);points.push_back(+1);points.push_back(+1);
	points.push_back(-1);points.push_back(+1);points.push_back(+1);

	Ball3f b=Ball3f::bestFittingBall(points);

	float radius=Vec3f(1,1,1).module();
	XgeReleaseAssert(b.center.fuzzyEqual(Vec3f(0,0,0)) && Utils::FuzzyEqual(b.radius,radius));

	//test volume and surface
	XgeReleaseAssert(Utils::FuzzyEqual(b.surface(),(4*(float)M_PI*radius*radius),0.0001f) && Utils::FuzzyEqual(b.volume(),4*(float)M_PI*radius*radius*radius/3,0.0001f));

	return 0;
}


//////////////////////////////////////////////////////////////////////
int BallfSelfTest()
{
	Log::printf("Testing Ballf...\n");


	//add a point
	std::vector<float> P;

	P.push_back(1.0f);P.push_back(-1.0f);P.push_back(-1.0f);P.push_back(-1.0f);
	P.push_back(1.0f);P.push_back(+1.0f);P.push_back(-1.0f);P.push_back(-1.0f);
	P.push_back(1.0f);P.push_back(+1.0f);P.push_back(+1.0f);P.push_back(-1.0f);
	P.push_back(1.0f);P.push_back(-1.0f);P.push_back(+1.0f);P.push_back(-1.0f);
	P.push_back(1.0f);P.push_back(-1.0f);P.push_back(-1.0f);P.push_back(+1.0f);
	P.push_back(1.0f);P.push_back(+1.0f);P.push_back(-1.0f);P.push_back(+1.0f);
	P.push_back(1.0f);P.push_back(+1.0f);P.push_back(+1.0f);P.push_back(+1.0f);
	P.push_back(1.0f);P.push_back(-1.0f);P.push_back(+1.0f);P.push_back(+1.0f);

	Ballf b=Ballf::bestFittingBall(3,P);
	
	XgeReleaseAssert(b.dim()==3);
	XgeReleaseAssert(b.center.fuzzyEqual(Vecf(1.0f, 0.0f,0.0f,0.0f)));
	XgeReleaseAssert(Utils::FuzzyEqual(b.radius,Vecf(0.0f, 1.0f,1.0f,1.0f).module(),1e-6f));

	return 0;
}


//////////////////////////////////////////////////////////////////////
int Box3fSelfTest()
{
	Log::printf("Testing Box3f...\n");

	Box3f b;
	XgeReleaseAssert(!b.isValid());//at the beginning shoud not be valud

	//add a point
	Vec3f p1(1,2,3);
	b.add(p1);
	XgeReleaseAssert(b.p1==p1 && b.p2==p1);

	//add a second point
	Vec3f p2(-1,+3,+4);
	b.add(p2);
	XgeReleaseAssert(
		   b.p1==Vec3f(min2(p1.x,p2.x),min2(p1.y,p2.y),min2(p1.z,p2.z)) 
		&& b.p2==Vec3f(max2(p1.x,p2.x),max2(p1.y,p2.y),max2(p1.z,p2.z))); 

	//test volume and surface
	float s=b.surface();
	float v=b.volume();
	XgeReleaseAssert(s==10.0f && v==2.0);

	//add a second bounding box
	Box3f b2;
	b2.add(Vec3f(10,11,12));
	b2.add(Vec3f(20,21,22));
	b.add(b2);
	XgeReleaseAssert(b.p1==Vec3f(-1,2,3) && b.p2==Vec3f(20,21,22));

	return 0;
}


//////////////////////////////////////////////////////////////////////
int BoxfSelfTest()
{
	Log::printf("Testing Boxf...\n");

	Boxf b(5);
	XgeReleaseAssert(!b.isValid());//at the beginning shoud not be valid

	//add a point
	float _p1[]={1, 1,2,3,4,5};
	Vecf p1(5,_p1);

	b.add(p1);
	XgeReleaseAssert(b.p1==p1 && b.p2==p1 && b.isValid());

	//add a second point
	float _p2[]={1, -1,3,4,5,6};
	Vecf p2(5,_p2);

	b.add(p2);

	XgeReleaseAssert(
		b.p1[0]==1 && b.p1[1]==-1 && b.p1[2]==2 && b.p1[3]==3 && b.p1[4]==4 && b.p1[5]==5 &&
		b.p2[0]==1 && b.p2[1]==+1 && b.p2[2]==3 && b.p2[3]==4 && b.p2[4]==5 && b.p2[5]==6 
		); 

	//test volume and surface
	float v=b.volume();
	XgeReleaseAssert(b.isValid());


	XgeReleaseAssert(v==2);

	//add a second bounding box
	Boxf b2(5);

	float _p3[]={1, 10,11,12,13,14};Vecf p3(5,_p3);
	float _p4[]={1, 20,21,22,23,24};Vecf p4(5,_p4);

	b2.add(p3);
	b2.add(p4);
	b.add(b2);
	XgeReleaseAssert(b.p1[0]==1 && b.p1[1]==-1 && b.p1[2]==2 && b.p1[3]==3 && b.p1[4]==4 && b.p1[5]==5 && b.p2==p4);

	return 0;
}


//////////////////////////////////////////////////////////////
int EncoderSelfTest()
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
		XgeReleaseAssert(decoder.ReadChar     ()==1);
		XgeReleaseAssert(decoder.ReadUchar    ()==2);
		XgeReleaseAssert(decoder.ReadShort    ()==3);
		XgeReleaseAssert(decoder.ReadUshort   ()==4);
		XgeReleaseAssert(decoder.ReadInt      ()==5);
		XgeReleaseAssert(decoder.ReadUint     ()==6);
		XgeReleaseAssert(decoder.ReadInt64    ()==myint64);
		XgeReleaseAssert(decoder.ReadUint64   ()==myuint64);
		XgeReleaseAssert(decoder.ReadFloat    ()==9.1f);
		XgeReleaseAssert(decoder.ReadDouble   ()==10.2);

		decoder.ReadRaw(sizeof(buff2),(unsigned char*)buff2);
		XgeReleaseAssert(!memcmp(buff1,buff2,sizeof(buff1)));
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
				XgeReleaseAssert(j==buffsize);

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
				XgeReleaseAssert(j==buffsize);

				//test the 2 buffers are now the same
				XgeReleaseAssert(!memcmp(temp1,temp2,buffsize));

			}

			delete [] temp1;
			delete [] temp2;
		}
	}

	return 0;
}


//////////////////////////////////////////////////////////////
int DecoderSelfTest()
{
	return 0;
}


/////////////////////////////////////////////
int GraphKMemSelfTest()
{
	Log::printf("Testing GraphKMem...\n");

	GraphKMem m(sizeof(float));
	XgeReleaseAssert(m.itemsize()==sizeof(float));

	//block/release operation
	unsigned int Ki=m.alloc();
	*((float*)m[Ki])=10.0f;
	unsigned int Kj=m.alloc();
	*((float*)m[Kj])=30.0f;
	XgeReleaseAssert(m.getNBlocked()==2);
	m.free(Ki);
	XgeReleaseAssert(m.getNBlocked()==1);
	m.free(Kj);
	XgeReleaseAssert(m.getNBlocked()==0);

	//resize operation
	struct A {int a,b;};
	struct B {int a;};
	GraphKMem n(sizeof(struct A));
	unsigned int Kk=n.alloc();
	(*((struct A*)n[Kk])).a=11;
	(*((struct A*)n[Kk])).b=22;
	n.resize(sizeof(struct B));
	XgeReleaseAssert(((*((struct B*)n[Kk])).a)==11);

	//clone
	GraphKMem l(n);
	n.flush();
	XgeReleaseAssert(n.getNBlocked()==0);
	XgeReleaseAssert(((*((struct B*)l[Kk])).a)==11);

	return 0;

}
 

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
int GraphSelfTest()
{
	Log::printf("Testing Graph...\n");

	//http://en.wikipedia.org/wiki/Hypercube
	static int cube_num_cells[11][11]=
	{
		/*0 */{    1,   0,    0,    0,    0,   0,   0,  0,  0,  0,  0},
		/*1 */{    2,   1,    0,    0,    0,   0,   0,  0,  0,  0,  0},
		/*2 */{    4,   4,    1,    0,    0,   0,   0,  0,  0,  0,  0},
		/*3 */{    8,  12,    6,    1,    0,   0,   0,  0,  0,  0,  0},
		/*4 */{   16,  32,   24,    8,    1,   0,   0,  0,  0,  0,  0},
		/*5 */{   32,  80,   80,   40,   10,   1,   0,  0,  0,  0,  0},
		/*6 */{   64, 192,  240,  160,   60,  12,   1,  0,  0,  0,  0},
		/*7 */{  128, 448,  672,  560,  280,  84,  14,  1,  0,  0,  0},
		/*8 */{  256,1024, 1792, 1792, 1120, 448, 112, 16,  1,  0,  0},
		/*9 */{  512,2304, 4608, 5376, 4032,2016, 672,144, 18,  1,  0},
		/*10*/{ 1024,5120,11520,15360,13440,8064,3360,960,180, 20,  1}
	};

	//http://en.wikipedia.org/wiki/Simplex
	int simplex_num_cells[11][11]=
	{
		{1,0,0,0,0,0,0,0,0,0,0},
		{2,1,0,0,0,0,0,0,0,0,0},
		{3,3,1,0,0,0,0,0,0,0,0},
		{4,6,4,1,0,0,0,0,0,0,0},
		{5,10,10,5,1,0,0,0,0,0,0},
		{6,15,20,15,6,1,0,0,0,0,0},
		{7,21,35,35,21,7,1,0,0,0,0},
		{8,28,56,70,56,28,8,1,0,0,0},
		{9,36,84,126,126,84,36,9,1,0,0},
		{10,45,120,210,252,210,120,45,10,1,0},
		{11,55,165,330,462,462,330,165,55,11,1}
	};

	GraphKMemSelfTest();

	//simple test for basic connectivity
	{
		Graph g(1);

		unsigned int N0=g.addNode(0);XgeReleaseAssert(g.Level(N0)==0);g.NodeData(N0)=0;g.Sign(N0)=0;g.NodeTmp(N0)=0;
		unsigned int N1=g.addNode(0);XgeReleaseAssert(g.Level(N1)==0);g.NodeData(N1)=1;g.Sign(N1)=1;g.NodeTmp(N1)=1;
		unsigned int N2=g.addNode(0);XgeReleaseAssert(g.Level(N2)==0);g.NodeData(N2)=2;g.Sign(N2)=2;g.NodeTmp(N2)=2;

		unsigned int E01=g.addNode(1);g.addArch(N0,E01);g.addArch(N1,E01);XgeReleaseAssert(g.Level(E01)==1);g.NodeData(E01)=3;g.Sign(E01)=3;g.NodeTmp(E01)=3;
		unsigned int E12=g.addNode(1);g.addArch(N1,E12);g.addArch(N2,E12);XgeReleaseAssert(g.Level(E12)==1);g.NodeData(E12)=4;g.Sign(E12)=4;g.NodeTmp(E12)=4;
		unsigned int E20=g.addNode(1);g.addArch(N2,E20);g.addArch(N0,E20);XgeReleaseAssert(g.Level(E20)==1);g.NodeData(E20)=5;g.Sign(E20)=5;g.NodeTmp(E20)=5;

		unsigned int F=g.addNode(2);XgeReleaseAssert(g.Level(F)==2);g.Sign(F)=6;g.NodeTmp(F)=6;
		g.NodeData(F)=6;
		g.addArch(E01,F);
		g.addArch(E12,F);
		g.addArch(E20,F);

		XgeReleaseAssert(g.getNCells(0)==3);
		XgeReleaseAssert(g.getNCells(1)==3);
		XgeReleaseAssert(g.getNCells(2)==1);

		g.ArchData(g.getFirstDwArch(F))=100;
		g.ArchData(g.getLastDwArch (F))=200;


		//force memory move
		unsigned int temp[4096];
		for (int i=0;i<4096;i++) temp[i]=g.addNode(0);
		for (int i=0;i<4096;i++) g.remNode(temp[i]);

		//copy constructor
		Graph g2(1);
		g2=g;
		g.clear();

		XgeReleaseAssert(g2.getNumNode()==7 && g2.getNumArch()==9);

		XgeReleaseAssert(g2.NodeData( N0)==0);
		XgeReleaseAssert(g2.NodeData( N1)==1);
		XgeReleaseAssert(g2.NodeData( N2)==2);
		XgeReleaseAssert(g2.NodeData(E01)==3);
		XgeReleaseAssert(g2.NodeData(E12)==4);
		XgeReleaseAssert(g2.NodeData(E20)==5);
		XgeReleaseAssert(g2.NodeData( F )==6);

		XgeReleaseAssert(g2.Sign( N0)==0);
		XgeReleaseAssert(g2.Sign( N1)==1);
		XgeReleaseAssert(g2.Sign( N2)==2);
		XgeReleaseAssert(g2.Sign(E01)==3);
		XgeReleaseAssert(g2.Sign(E12)==4);
		XgeReleaseAssert(g2.Sign(E20)==5);
		XgeReleaseAssert(g2.Sign( F )==6);

		XgeReleaseAssert(g2.NodeTmp( N0)==0);
		XgeReleaseAssert(g2.NodeTmp( N1)==1);
		XgeReleaseAssert(g2.NodeTmp( N2)==2);
		XgeReleaseAssert(g2.NodeTmp(E01)==3);
		XgeReleaseAssert(g2.NodeTmp(E12)==4);
		XgeReleaseAssert(g2.NodeTmp(E20)==5);
		XgeReleaseAssert(g2.NodeTmp( F )==6);

		XgeReleaseAssert(g2.ArchData(g2.getFirstDwArch(F))==100);
		XgeReleaseAssert(g2.ArchData(g2.getLastDwArch (F))==200);

		XgeReleaseAssert(g2.getNDw(F  )==3);
		XgeReleaseAssert(g2.getNDw(E01)==2);
		XgeReleaseAssert(g2.getNDw(E12)==2);
		XgeReleaseAssert(g2.getNDw(E20)==2);
		XgeReleaseAssert(g2.getNDw(N0 )==0);
		XgeReleaseAssert(g2.getNDw(N1 )==0);
		XgeReleaseAssert(g2.getNDw(N2 )==0);

		XgeReleaseAssert(g2.getNUp(F )==0);
		XgeReleaseAssert(g2.getNUp(E01)==1);
		XgeReleaseAssert(g2.getNUp(E12)==1);
		XgeReleaseAssert(g2.getNUp(E20)==1);
		XgeReleaseAssert(g2.getNUp(N0 )==2);
		XgeReleaseAssert(g2.getNUp(N1 )==2);
		XgeReleaseAssert(g2.getNUp(N2 )==2);

		XgeReleaseAssert(g2.getFirstDwArch(F)==g2.getFirstUpArch(E01));
		XgeReleaseAssert(g2.getLastDwArch (F)==g2.getFirstUpArch(E20));
		XgeReleaseAssert(g2.getNextDwArch(g2.getFirstDwArch(F))==g2.getFirstUpArch(E12));
		XgeReleaseAssert(g2.getPrevDwArch(g2.getLastDwArch(F))==g2.getFirstUpArch(E12));

		XgeReleaseAssert(g2.getFirstDwNode(F)==E01);
		XgeReleaseAssert(g2.getLastDwNode (F)==E20);

		XgeReleaseAssert(g2.getFirstUpNode(N0)==E01);
		XgeReleaseAssert(g2.getLastUpNode (N0)==E20);


		GraphIterator it=g2.goDw(F);
		XgeReleaseAssert(it.getArch()==g2.getFirstDwArch(F) && it.getNode()==g2.getFirstDwNode(F));
		it++;
		XgeReleaseAssert(it.getArch()==g2.getNextDwArch(g2.getFirstDwArch(F)) && it.getNode()==E12);
		it++;
		XgeReleaseAssert(it.getArch()==g2.getLastDwArch(F) && it.getNode()==E20);
		it++;
		XgeReleaseAssert(it.end());

		std::map<unsigned int,bool> map;

		GraphListIterator jt=g2.each(0);
		XgeReleaseAssert((*jt==N0 || *jt==N1 || *jt==N2) && map.find(*jt)==map.end());map[*jt]=true;
		jt++;
		XgeReleaseAssert((*jt==N0 || *jt==N1 || *jt==N2) && map.find(*jt)==map.end());map[*jt]=true;
		jt++;
		XgeReleaseAssert((*jt==N0 || *jt==N1 || *jt==N2) && map.find(*jt)==map.end());map[*jt]=true;
		jt++;
		XgeReleaseAssert(jt.end());

		jt=g2.each(1);
		XgeReleaseAssert((*jt==E01 || *jt==E12 || *jt==E20) && map.find(*jt)==map.end());map[*jt]=true;
		jt++;
		XgeReleaseAssert((*jt==E01 || *jt==E12 || *jt==E20) && map.find(*jt)==map.end());map[*jt]=true;
		jt++;
		XgeReleaseAssert((*jt==E01 || *jt==E12 || *jt==E20) && map.find(*jt)==map.end());map[*jt]=true;
		jt++;
		XgeReleaseAssert(jt.end());

		jt=g2.each(2);
		XgeReleaseAssert((*jt==F) && map.find(*jt)==map.end());map[*jt]=true;
		jt++;
		XgeReleaseAssert(jt.end());

		unsigned int A=g2.getFirstDwArch(F);
		XgeReleaseAssert(g2.getN0(A)==E01 && g2.getN1(A)==F);

		//XgeReleaseAssert(g2.findCommonNode(E01,E12,DIRECTION_UP)==F);
		//XgeReleaseAssert(g2.findCommonNode(E01,E12,DIRECTION_DOWN)==N1);
		//unsigned int Node;
		//XgeReleaseAssert(g2.findCommonNodes(E01,E12,&Node,DIRECTION_UP)==1 && Node==F);
		//XgeReleaseAssert(g2.findCommonNodes(E01,E12,&Node,DIRECTION_DOWN)==1 && Node==N1);

		g2.swapDwOrder(g2.getFirstDwArch(F),g2.getLastDwArch(F));
		XgeReleaseAssert(g2.getFirstDwNode(F)==E20 && g2.getLastDwNode(F)==E01);
		g2.swapDwOrder(g2.getFirstDwArch(F),g2.getLastDwArch(F));
		XgeReleaseAssert(g2.getFirstDwNode(F)==E01 && g2.getLastDwNode(F)==E20);

		g2.remArch(N0,E01);
		XgeReleaseAssert(g2.getNDw(E01)==1 && g2.getNUp(N0)==1);
		g2.addArch(N0,E01);
		XgeReleaseAssert(g2.getNDw(E01)==2 && g2.getNUp(N0)==2);

		XgeReleaseAssert(g2.findArch(N0,E01) && !g2.findArch(N0,F));

		GraphNavigator navigator;
		int num=g2.findCells(0,F,navigator);
		XgeReleaseAssert(num==3 
			&& (navigator.nav[0][0]==N0 || navigator.nav[0][0]==N1 || navigator.nav[0][0]==N2)
			&& (navigator.nav[0][1]==N0 || navigator.nav[0][1]==N1 || navigator.nav[0][1]==N2)
			&& (navigator.nav[0][2]==N0 || navigator.nav[0][2]==N1 || navigator.nav[0][2]==N2));


		num=g2.findCells(2,N0,navigator);
		XgeReleaseAssert(num==1 && navigator.nav[2][0]==F );

		g2.check();
	}


	//cuboid
	{
		for (int pointdim=0;pointdim<=10;pointdim++)
		{
			SmartPointer<Graph> cube=Graph::cuboid(pointdim);

			for (int i=0;i<=pointdim;i++)
				XgeReleaseAssert(cube->getNCells(i)==cube_num_cells[pointdim][i]);

			//check double connectivity
			if (pointdim>=1)
			{
				XgeReleaseAssert(cube->getNUp(*cube->each(pointdim))==cube_num_cells[pointdim][0]);

				for (GraphListIterator it=cube->each(0);!it.end();it++)
					XgeReleaseAssert(cube->getNDw(*it)==1 && cube->getNUp(*it)==pointdim);
			}

			cube->check();
			cube.reset();
		}
	}

	//simplex
	{
		for (int pointdim=0;pointdim<=10;pointdim++)
		{
			SmartPointer<Graph> s=Graph::simplex(pointdim);
			
			for (int i=0;i<=pointdim;i++)
				XgeReleaseAssert(s->getNCells(i)==simplex_num_cells[pointdim][i]);


			//check double connectivity
			if (pointdim>=1)
			{
				XgeReleaseAssert(s->getNUp(*s->each(pointdim))==simplex_num_cells[pointdim][0]);

				for (GraphListIterator it=s->each(0);!it.end();it++)
					XgeReleaseAssert(s->getNDw(*it)==1 && s->getNUp(*it)==pointdim);
			}

			s->check();
			s.reset();
		}
	}

	//test remNode
	{
		int pointdim=5;
		SmartPointer<Graph> cube=Graph::cuboid(pointdim);
		cube->remNode(*cube->each(pointdim),true);

		cube->check();

		for (int i=0;i<=pointdim;i++)
			XgeReleaseAssert(cube->getNCells(i)==0);

		cube.reset();
	}
	
	//fixBoundaryFacesOrientation
	{
		{
			SmartPointer<Graph> cube=Graph::cuboid(3);
			cube->fixBoundaryFaceOrientation(0);

			//check that the bounding box for the faces are flat on the fitting plane
			for (GraphListIterator it=cube->each(2);!it.end();it++)
			{
				unsigned int F=*it;
				Planef h=cube->getFittingPlane(F);

				XgeReleaseAssert(
					   h.fuzzyEqual(Planef(-1.0, 1.0, 0.0, 0.0)) 
					|| h.fuzzyEqual(Planef(-1.0, 0.0, 1.0, 0.0))
					|| h.fuzzyEqual(Planef(-1.0, 0.0, 0.0 ,1.0))
					|| h.fuzzyEqual(Planef( 0.0,-1.0, 0.0, 0.0))
					|| h.fuzzyEqual(Planef( 0.0, 0.0,-1.0, 0.0))
					|| h.fuzzyEqual(Planef( 0.0, 0.0, 0.0,-1.0))
					);
			}

			cube.reset();
		}

		//translate
		{
			SmartPointer<Graph> cube=Graph::cuboid(3);
			Vecf vt(0.0, 1,1,1);
			cube->translate(vt);
			cube->fixBoundaryFaceOrientation(0);

			//check that the bounding box for the faces are flat on the fitting plane
			for (GraphListIterator it=cube->each(2);!it.end();it++)
			{
				unsigned int F=*it;
				Planef h=cube->getFittingPlane(F);


				XgeReleaseAssert(
					   h.fuzzyEqual(Planef(-2.0, 1.0, 0.0, 0.0)) 
					|| h.fuzzyEqual(Planef(-2.0, 0.0, 1.0, 0.0))
					|| h.fuzzyEqual(Planef(-2.0, 0.0, 0.0 ,1.0))
					|| h.fuzzyEqual(Planef( 1.0,-1.0, 0.0, 0.0))
					|| h.fuzzyEqual(Planef( 1.0, 0.0,-1.0, 0.0))
					|| h.fuzzyEqual(Planef( 1.0, 0.0, 0.0,-1.0))
					);
			}

			cube->check();
		}

		//scale
		{
			SmartPointer<Graph> cube=Graph::cuboid(3);
			Vecf vs(0.0, -1,-1,-1);
			cube->scale(vs);
			cube->fixBoundaryFaceOrientation(0);

			//check that the bounding box for the faces are flat on the fitting plane
			for (GraphListIterator it=cube->each(2);!it.end();it++)
			{
				unsigned int F=*it;
				Planef h=cube->getFittingPlane(F);


				XgeReleaseAssert(
					   h.fuzzyEqual(Planef( 0.0, 1.0, 0.0, 0.0)) 
					|| h.fuzzyEqual(Planef( 0.0, 0.0, 1.0, 0.0))
					|| h.fuzzyEqual(Planef( 0.0, 0.0, 0.0 ,1.0))
					|| h.fuzzyEqual(Planef(-1.0,-1.0, 0.0, 0.0))
					|| h.fuzzyEqual(Planef(-1.0, 0.0,-1.0, 0.0))
					|| h.fuzzyEqual(Planef(-1.0, 0.0, 0.0,-1.0))
					);
			}

			cube->check();
		}

		//rotate
		{
			SmartPointer<Graph> cube=Graph::cuboid(3);

			//rotate along Z
			cube->rotate(1,2,(float)M_PI/2.0);
			cube->fixBoundaryFaceOrientation(0);

			//check that the bounding box for the faces are flat on the fitting plane
			for (GraphListIterator it=cube->each(2);!it.end();it++)
			{
				unsigned int F=*it;
				Planef h=cube->getFittingPlane(F);

				XgeReleaseAssert(
					   h.fuzzyEqual(Planef( 0.0, 1.0, 0.0, 0.0)) 
					|| h.fuzzyEqual(Planef(-1.0, 0.0, 1.0, 0.0))
					|| h.fuzzyEqual(Planef(-1.0, 0.0, 0.0 ,1.0))
					|| h.fuzzyEqual(Planef(-1.0,-1.0, 0.0, 0.0))
					|| h.fuzzyEqual(Planef( 0.0, 0.0,-1.0, 0.0))
					|| h.fuzzyEqual(Planef( 0.0, 0.0, 0.0,-1.0))
					);
			}

			cube->check();
		}
	}
	

	//bounding box,bounding ball
	{
		for (int pointdim=2;pointdim<5;pointdim++)
		{
			SmartPointer<Graph> cube=Graph::cuboid(pointdim);

			for (GraphListIterator it=cube->each(0);!it.end();it++)
			{
				unsigned int V=*it;
				Vecf pos(pointdim,cube->getGeometry(V));
				Boxf bbox1=cube->getBoundingBox(V);
				Boxf bbox2(pointdim);bbox2.add(pos);
				XgeReleaseAssert(bbox1.fuzzyEqual(bbox2));

				Ballf bball1=cube->getBoundingBall(V);

				std::vector<float> _list; 

				for (int U=0;U<=pos.dim;U++)
					_list.push_back(pos[U]);
				
				Ballf bball2=Ballf::bestFittingBall(pos.dim,_list);
				XgeReleaseAssert(bball1.fuzzyEqual(bball2));
			}

			//check that the bounding box for the faces are flat on the fitting plane
			for (GraphListIterator it=cube->each(pointdim-1);!it.end();it++)
			{
				unsigned int F=*it;
				Planef h=cube->getFittingPlane(F);
				Boxf bbox=cube->getBoundingBox(F);
				XgeReleaseAssert(Utils::FuzzyEqual(h.getDistance(bbox.p1),0)  && Utils::FuzzyEqual(h.getDistance(bbox.p2),0));
			}

			unsigned int C=*cube->each(pointdim);
			Boxf bbox1=cube->getBoundingBox(C);

			Vecf points[2];
			points[0]=Vecf(pointdim);points[0].set(0.0f);points[0].mem[0]=1;
			points[1]=Vecf(pointdim);points[1].set(1.0f);points[1].mem[0]=1;

			Boxf bbox2=Boxf(points[0],points[1]);
			XgeReleaseAssert(bbox1.fuzzyEqual(bbox2));

			Ballf bball1=cube->getBoundingBall(C);

			std::vector<float> _list;
			
			for (int U=0;U<=pointdim;U++)
				_list.push_back(points[0][U]);

			for (int U=0;U<=pointdim;U++)
				_list.push_back(points[1][U]);

			Ballf bball2=Ballf::bestFittingBall(pointdim,_list);
			XgeReleaseAssert(bball1.fuzzyEqual(bball2));

			cube->check();
		}
	}



	//scale to unit box
	{
		SmartPointer<Graph> cube=Graph::cuboid(3);

		Matf vmat(3),hmat(3);
		cube->toUnitBox(vmat,hmat);

		for (GraphListIterator it=cube->each(2);!it.end();it++)
		{
			unsigned int F=*it;
			Planef h=cube->getFittingPlane(F);
	
			XgeReleaseAssert(
				   h.fuzzyEqual(Planef(-1.0, 1.0, 0.0, 0.0)) 
				|| h.fuzzyEqual(Planef(-1.0, 0.0, 1.0, 0.0))
				|| h.fuzzyEqual(Planef(-1.0, 0.0, 0.0 ,1.0))
				|| h.fuzzyEqual(Planef(-1.0,-1.0, 0.0, 0.0))
				|| h.fuzzyEqual(Planef(-1.0, 0.0,-1.0, 0.0))
				|| h.fuzzyEqual(Planef(-1.0, 0.0, 0.0,-1.0)));
		}
		cube->check();
	}

	
	//permutate
	{
		SmartPointer<Graph> cube=Graph::cuboid(3);
		Vecf vt(0.0, 1.0,2.0,3.0);cube->translate(vt);	
		
		


		SmartPointer<Graph> cubep=cube->clone();

		//homo=homo  x'=z y'=x z'=y
		std::vector<int> perm;
		perm.push_back(0);
		perm.push_back(3);
		perm.push_back(1);
		perm.push_back(2);

		cubep->permutate(perm);
		cubep->check();

		for (GraphListIterator it=cubep->each(0);!it.end();it++)
		{
			unsigned int V=*it;

			Vecf v1(3,cubep->getGeometry(V));

			Vecf v2=Vecf(3,cube->getGeometry(V));
			v2=v2.permutate(3,&perm[0]);
			XgeReleaseAssert(v2==v1);
			
			XgeReleaseAssert(
				   v1==Vecf(1.0, 3.0, 1.0, 2.0)
				|| v1==Vecf(1.0, 4.0, 1.0, 2.0)
				|| v1==Vecf(1.0, 3.0, 2.0 ,2.0)
				|| v1==Vecf(1.0, 4.0, 2.0, 2.0)
				|| v1==Vecf(1.0, 3.0, 1.0, 3.0)
				|| v1==Vecf(1.0, 4.0, 1.0, 3.0)
				|| v1==Vecf(1.0, 3.0, 2.0, 3.0)
				|| v1==Vecf(1.0, 4.0, 2.0, 3.0));
		}
	}

	//embed(pointdim)
	{
		SmartPointer<Graph> cube=Graph::cuboid(2);
		cube->embed(3); //embed in 3dim space

		XgeReleaseAssert(cube->getNCells(2)==1 && cube->getNCells(3)==0);
		unsigned int F=*(cube->each(2));

		//now the previous full cell is a boundary cell with plane information
		XgeReleaseAssert(cube->NodeData(F) && cube->getGeometry(F)[3]==1.0f);
		
		for (GraphListIterator it=cube->each(0);!it.end();it++)
		{
			Vecf v1(3,cube->getGeometry(*it));
			XgeReleaseAssert(
				   v1==Vecf(1.0, 0.0, 0.0, 0.0)
				|| v1==Vecf(1.0, 1.0, 0.0, 0.0)
				|| v1==Vecf(1.0, 0.0, 1.0 ,0.0)
				|| v1==Vecf(1.0, 1.0, 1.0, 0.0)
				);
		}

		cube->check();
	}

	//triangulation
	{
		{
			SmartPointer<Graph> cube=Graph::cuboid(2);
			XgeReleaseAssert(cube->getNCells(0)==4 && cube->getNCells(1)==4 && cube->getNCells(2)==1);
			cube->triangulate(0);
			XgeReleaseAssert(cube->getNCells(0)==4 && cube->getNCells(1)==5 && cube->getNCells(2)==2);
			cube->check();
		}

		{
			SmartPointer<Graph> cube=Graph::cuboid(3);
			XgeReleaseAssert(cube->getNCells(0)==8 && cube->getNCells(1)==12 && cube->getNCells(2)==6  && cube->getNCells(3)==1);
			cube->triangulate(0);
			XgeReleaseAssert(cube->getNCells(0)==9 && cube->getNCells(1)==26 && cube->getNCells(2)==30 && cube->getNCells(3)==12);
			cube->check();
		}
	}

	// order face 2d
	{
		Graph g(2);

		unsigned int N0=g.addNode(0);
		unsigned int N1=g.addNode(0);
		unsigned int N2=g.addNode(0);
		unsigned int N3=g.addNode(0);

		unsigned int E01=g.addNode(1);g.addArch(N0,E01);g.addArch(N1,E01);
		unsigned int E12=g.addNode(1);g.addArch(N1,E12);g.addArch(N2,E12);
		unsigned int E23=g.addNode(1);g.addArch(N2,E23);g.addArch(N3,E23);
		unsigned int E30=g.addNode(1);g.addArch(N3,E30);g.addArch(N0,E30);

		unsigned int F=g.addNode(2);
		g.addArch(E23,F);
		g.addArch(E01,F);
		g.addArch(E12,F);
		g.addArch(E30,F);

		bool ok=g.orderFace2d(F);
		XgeReleaseAssert(ok);

		unsigned int M0=g.getFirstDwNode(g.getLastDwNode(F));
		unsigned int M1=g.getLastDwNode(g.getLastDwNode(F) );

		for (GraphIterator it=g.goDw(F);!it.end();it++)
		{
			unsigned int E=*it;
			unsigned int N0=g.getFirstDwNode(E);
			unsigned int N1=g.getLastDwNode(E);
			XgeReleaseAssert((M0==N0 || M0==N1) || (M1==N0 || M1==N1));
			M0=N0;
			M1=N1;
		}

		//cause failure
		{
			unsigned int N4=g.addNode(0);
			unsigned int E34=g.addNode(1);g.addArch(N3,E34);g.addArch(N4,E34);
			g.addArch(E34,F);
			bool ok=g.orderFace2d(F);
			XgeReleaseAssert(!ok);
		}
	}

	
	//mkpol filtering points
	{
		const int pointdim=2;
		const int npoints=6;
		const int hull_npoints=4;
		float points[pointdim*npoints]={0.5f,0.5f,0.9f,0.9f,0,0,1,0,1,1,0,1};
		float hull[hull_npoints*pointdim]={0,0,1,0,1,1,0,1};

		Matf vmat(pointdim),hmat(pointdim);
		SmartPointer<Graph> g=Graph::mkpol(vmat,hmat,pointdim,npoints,points,1e-6f);

		std::set<int> found_in_hull;

		for (GraphListIterator it=g->each(0);!it.end();it++)
		{
			Vecf v(g->getPointDim(),g->getGeometry(*it));

			//find only the first
			for (int I=0;I<hull_npoints;I++)
			{
				Vecf compare(pointdim,1.0f,hull+I*pointdim);
				
				if (v.fuzzyEqual(compare,0.0001f))
				{
					XgeReleaseAssert(found_in_hull.find(I)==found_in_hull.end());
					found_in_hull.insert(I);
					break;
				}
			}
		}
	}

	//example of mkpol in lower dimension
	{
		const int pointdim=3;
		const int npoints=6;
		const int hull_npoints=4;
		float PlaneZ=10.0f;
		float points[pointdim*npoints]={0.1f,0.1f,PlaneZ, 0.9f,0.9f,PlaneZ, 0,0,10, 1,0,PlaneZ, 1,1,10, 0,1,PlaneZ}; //note: all in the same plane Z
		float hull[hull_npoints*pointdim]={0,0,10, 1,0,10, 1,1,10, 0,1,10};

		Matf g_vmat(pointdim),g_hmat(pointdim);
		SmartPointer<Graph> g=Graph::mkpol(g_vmat,g_hmat,pointdim,npoints,points,1e-6f);
		XgeReleaseAssert(g->getNCells(0)==hull_npoints);
		XgeReleaseAssert(g_vmat.dim==3 && g->getPointDim()==2);
		std::set<int> found_in_hull;

		for (GraphListIterator it=g->each(0);!it.end();it++)
		{
			Vecf v(g_vmat.dim);
			memcpy(&v.mem[0],g->getGeometry(*it),sizeof(float)*(g->getPointDim()+1));
			v=g_vmat * v;

			//find only the first
			for (int I=0;I<hull_npoints;I++)
			{
				Vecf compare(pointdim,1.0f,hull+I*pointdim);
				
				if (v.fuzzyEqual(compare,0.0001f))
				{
					XgeReleaseAssert(found_in_hull.find(I)==found_in_hull.end());
					found_in_hull.insert(I);
					break;
				}
			}
		}
	}


	//mkpol in lower dimension -1
	{ 
		for (int pointdim=2;pointdim<8;pointdim++)
		{
			//build a plane in pointdim where all points will be!
			int npoints=pointdim*2;

			Planef h=Planef::getRandomPlane(pointdim);

			float* points=new float[npoints*pointdim];
			for (int I=0;I<npoints;I++)
			{
				Vecf p=h.getRandomPoint();
				memcpy(points+I*pointdim,p.mem+1,sizeof(float)*pointdim);
			}

			//find the mkpol
			Matf g_vmat(pointdim),g_hmat(pointdim);
			SmartPointer<Graph> g=Graph::mkpol(g_vmat,g_hmat,pointdim,npoints,points,1e-4f);

			//check that g has space dim > point dim
			XgeReleaseAssert(g_vmat.dim==pointdim && g->getPointDim()==(g_vmat.dim-1));

			//...and all points transformed belongs to the plane
			for (GraphListIterator it=g->each(0);!it.end();it++)
			{
				Vecf v(g_vmat.dim);
				memcpy(&v.mem[0],g->getGeometry(*it),sizeof(float)*(g->getPointDim()+1));
				v=g_vmat * v;
				float Distance=fabs(h.getDistance(v));
				XgeReleaseAssert(Distance<0.001f);
			}

			delete [] points;
		}
	}



	//(symbolic) mkpol
	{
		for (int pointdim=0;pointdim<8;pointdim++)
		{
			Log::printf("   mkpol %d...\n",pointdim);

			int npoints =1;
			for (int i=0;i<pointdim;i++) npoints*=2;

			float* points=(float*)MemPool::getSingleton()->calloc(npoints,pointdim*sizeof(float));
			float* p=points;

			for (int i=0;i<npoints;i++)
			{
				for (int bin=i,ref=0;bin;bin>>=1,ref++)
					if (bin & 1) p[ref]=1.0f;
				
				p+=pointdim;
			}

			Matf g_vmat(pointdim),g_hmat(pointdim);
			SmartPointer<Graph> cube=Graph::mkpol(g_vmat,g_hmat,pointdim,npoints,points,1e-6f);

			//check number of cells
			for (int i=0;i<=pointdim;i++)
				XgeReleaseAssert(cube->getNCells(i)==cube_num_cells[pointdim][i]);

			cube->check();

			//check double connectivity
			if (pointdim>=1)
			{
				XgeReleaseAssert(cube->getNUp(*cube->each(pointdim))==cube_num_cells[pointdim][0]);

				for (GraphListIterator it=cube->each(0);!it.end();it++)
					XgeReleaseAssert(cube->getNDw(*it)==1 && cube->getNUp(*it)==pointdim);
			}

			MemPool::getSingleton()->free(npoints*pointdim*sizeof(float),points);
		}
	}


	//split algorithm (only very basic check, to test in real cases)
	{
		GraphNavigator nav;
		SmartPointer<Graph> cube3=Graph::cuboid(3);
		unsigned int C=*cube3->each(3);
		float start_tolerance=1e-4f;
		unsigned int Cb,Ca,Ce; //below,above,equa
		Planef h(-0.5f,1.0f,0.0f,0.0f);
		unsigned int Id=0xffff;
		int max_try=3;
		int retcode=cube3->split(nav,C,h,start_tolerance,max_try,Cb,Ca,Ce,Id);
		XgeReleaseAssert(retcode==Graph::SPLIT_OK);
		XgeReleaseAssert(cube3->getNCells(0)==12 && cube3->getNCells(1)==20 && cube3->getNCells(2)==11 && cube3->getNCells(3)==2);
		XgeReleaseAssert(cube3->Level(Ce)==2 && cube3->getNode(Ce).Id==Id);
	}


	//cuboid by symbolic power
	{
		for (int L1=0;L1<=5;L1++)
		for (int L2=0;L2<=5;L2++)
		{
			Log::printf("   symbolic power of cube(%d) * cube(%d)...\n",L1,L2);
			int pointdim=L1+L2;
			XgeReleaseAssert(pointdim<=10);

			SmartPointer<Graph> g1=Graph::cuboid(L1);
			SmartPointer<Graph> g2=Graph::cuboid(L2);

			Matf V(L1+L2);
			Matf H(L1+L2);
			SmartPointer<Graph> cube=Graph::power(V,H,g1,SmartPointer<Matf>(),SmartPointer<Matf>(),g2,SmartPointer<Matf>(),SmartPointer<Matf>());
			XgeReleaseAssert(V.almostIdentity(0.0f) && H.almostIdentity(0.0f));
			XgeReleaseAssert(cube->getPointDim()==pointdim);

			for (int i=0;i<=pointdim;i++)
				XgeReleaseAssert(cube->getNCells(i)==cube_num_cells[pointdim][i]);

			//check double connectivity
			if (pointdim>=1)
			{
				XgeReleaseAssert(cube->getNUp(*cube->each(pointdim))==cube_num_cells[pointdim][0]);

				for (GraphListIterator it=cube->each(0);!it.end();it++)
					XgeReleaseAssert(cube->getNDw(*it)==1 && cube->getNUp(*it)==pointdim);
			}
		}
	}


	//power algorithm, another way to build a cube

	{
		for (int Dim1=0;Dim1<=3;Dim1++)
		for (int Dim2=0;Dim2<=3;Dim2++)
		{
			int pointdim=Dim1+Dim2;

			SmartPointer<Graph> g1=Graph::cuboid(Dim1,+7.0,+11.0);
			SmartPointer<Graph> g2=Graph::cuboid(Dim2,-7.0,-11.0);

			SmartPointer<Matf> VmatT(new Matf(pointdim));
			SmartPointer<Matf> HmatT(new Matf(pointdim));
			SmartPointer<Graph> cube=Graph::power(*VmatT.get(),*HmatT.get(),g1,SmartPointer<Matf>(),SmartPointer<Matf>(),g2,SmartPointer<Matf>(),SmartPointer<Matf>());
			XgeReleaseAssert(cube->getPointDim()==pointdim && VmatT->dim==pointdim && HmatT->dim==pointdim);

			//check bounding box
			Boxf bbox=cube->getBoundingBox(0,VmatT,HmatT);
			Vecf p0(pointdim),p1(pointdim);
			p0.mem[0]=p1.mem[0]=1.0f; //is a point
			
			for (int I=1;I<=pointdim;I++)
			{
				if (I<=Dim1)
				{
					p0.mem[I]= +7.0f;
					p1.mem[I]=+11.0f;
				}
				else
				{
					p0.mem[I]= -7.0f;
					p1.mem[I]=-11.0f;
				}
			}
			
			//cube->print();
			XgeReleaseAssert(bbox.fuzzyEqual(Boxf(p0,p1)));
		}
	}

	//power algorithm, test the embedding matrices (so the order of coordinates is mantained)
	{
		SmartPointer<Graph> g1=Graph::cuboid(2);Vecf t1(0.0f,+1.0f,+3.0f,+7.0f);SmartPointer<Matf> g1vmat(new Matf(Matf::translateV(t1)));SmartPointer<Matf> g1hmat(new Matf(Matf::translateH(t1)));
		SmartPointer<Graph> g2=Graph::cuboid(2);Vecf t2(0.0f,-1.0f,-3.0f,-7.0f);SmartPointer<Matf> g2vmat(new Matf(Matf::translateV(t2)));SmartPointer<Matf> g2hmat(new Matf(Matf::translateH(t2)));

		SmartPointer<Matf> VmatT(new Matf(0));
		SmartPointer<Matf> HmatT(new Matf(0));
		SmartPointer<Graph> power=Graph::power(*VmatT.get(),*HmatT.get(),g1,g1vmat,g1hmat,g2,g2vmat,g2hmat);
		XgeReleaseAssert(power->getPointDim()==(g1->getPointDim()+g2->getPointDim()) && VmatT->dim==HmatT->dim && VmatT->dim==(g1vmat->dim+g2vmat->dim));
		XgeReleaseAssert(power->getNCells(power->getPointDim())==1); //should produce only one cell!
		Boxf bbox=power->getBoundingBox(0,VmatT,HmatT);
		XgeReleaseAssert(bbox.fuzzyEqual(Boxf(Vecf(1.0f, 1.0f,3.0f,7.0f, -1.0f,-3.0f,-7.0f ),Vecf(1.0f, 2.0f,4.0f,7.0f, 0.0f,-2.0f,-7.0f))));
	}


	


	

	//mkpol which cause deadlock without the rescaling of points (done in Graph::mkpol)
	{
		float points[20*3]={
			145.19400f,167.10451f,50.291454f,
			153.26077f,167.10451f,50.291454f,
			153.26077f,167.10451f,33.768009f,
			145.19400f,167.10451f,33.768009f,
			145.19400f,160.87621f,33.416248f,
			153.28918f,160.87621f,33.416248f,
			153.27327f,164.36740f,33.416248f,
			153.27184f,164.67984f,33.416248f,
			153.27904f,163.10172f,33.416248f,
			145.19400f,164.36740f,33.416248f,
			145.19400f,164.67984f,33.416248f,
			145.19400f,163.10172f,33.416248f,
			145.19400f,154.64792f,33.416248f,
			153.31760f,154.64792f,33.416248f,
			153.30145f,158.18697f,33.416248f,
			153.29318f,160.00089f,33.416248f,
			145.19400f,158.18697f,33.416248f,
			145.19400f,160.00089f,33.416248f,
			145.19400f,154.64792f,50.291454f,
			153.31760f,154.64792f,50.291454f
		};

		Matf Vmat(3),Hmat(3);
		SmartPointer<Graph> g=Graph::mkpol(Vmat,Hmat,3,20,points,1e-6f);
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////
int PlasmBoolopSelftest()
{
	Log::printf("Testing PlasmBoolOperation...\n");
	{
		SmartPointer<Hpc> c1=Plasm::cube(0);
		SmartPointer<Hpc> c2=Plasm::cube(0);

		std::vector<SmartPointer<Hpc> > pols;
		pols.push_back(c1);
		pols.push_back(c2);

		std::vector<SmartPointer<Hpc> >args;

 		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_OR  ,pols),2,2,0.0f));
		args.push_back(Plasm::translate(Plasm::cube(1,-0.5,+0.5),2,2,0.5));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_AND ,pols),2,2,1.0f));
		args.push_back(Plasm::translate(Plasm::cube(1,-0.5,+0.5),2,2,1.5));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_DIFF,pols),2,2,2.0f));
		args.push_back(Plasm::translate(Plasm::cube(1,-0.5,+0.5),2,2,2.5));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_XOR ,pols),2,2,3.0f));
		args.push_back(Plasm::translate(Plasm::cube(1,-0.5,+0.5),2,2,3.5));

		SmartPointer<Hpc> Out=Plasm::Struct(args);
		Plasm::view(Out);
	}

	XgeReleaseAssert(!xge_total_hpc);


	//test in 1-dim

	{
		SmartPointer<Hpc> c1=Plasm::cube(1,0,3);
		SmartPointer<Hpc> c2=Plasm::cube(1,1,4);
		SmartPointer<Hpc> c3=Plasm::cube(1,2,5);

		std::vector<SmartPointer<Hpc> > pols;
		pols.push_back(c1);
		pols.push_back(c2);
		pols.push_back(c3);

		std::vector<SmartPointer<Hpc> >args;

		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_OR   ,pols),2,2,0.0f));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_AND  ,pols),2,2,1.0f));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_DIFF ,pols),2,2,2.0f));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_XOR  ,pols),2,2,3.0f));

		SmartPointer<Hpc> Out=Plasm::Struct(args);
		Plasm::view(Out);
	}

	XgeReleaseAssert(!xge_total_hpc);

	
	{
		SmartPointer<Hpc> base_cube=Plasm::translate(Plasm::translate(Plasm::translate(Plasm::cube(3),3,1,-0.5),3,2,-0.5),3,3,-0.5);

		std::vector<SmartPointer<Hpc> > pols;

		const int npols=4;
		for (int i=0;i<npols;i++)
			pols.push_back(Plasm::rotate(base_cube,3,1,2,i*(float)M_PI/(npols*2)));

		std::vector<SmartPointer<Hpc> > args;
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_OR  ,pols),3,1,0.0f));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_AND ,pols),3,1,1.5f));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_DIFF,pols),3,1,3.0f));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_XOR ,pols),3,1,4.5f));

		SmartPointer<Hpc> Out=Plasm::Struct(args);
		Plasm::view(Out);	
	}
	
	XgeReleaseAssert(!xge_total_hpc);

	return 0;
}




///////////////////////////////////////////////////////////////////////
#include <xge/glcanvas.h>

class DisplayManipulator: public GLCanvas
{
public:

  Manipulator manipulator;
  Box3f box;
  Mat4f T;

  DisplayManipulator():GLCanvas()
  {
    this->box=Box3f(Vec3f(0,0,0),Vec3f(1,1,1));
    this->T=Mat4f(); // identity matrix;
    this->manipulator.setObject(this->box,&this->T);
    this->frustum->guessBestPosition(this->box);
    this->redisplay();
  }

  //onKeyboard
  virtual bool onKeyboard(int key,int x,int y)
  {
    if (key=='t' || key=='T')
    {
      manipulator.setOperation(Manipulator::TRANSLATE);
      this->manipulator.setObject(this->box,&this->T);
      this->redisplay();
      return true;
    }
    else if (key=='s' || key=='S')
    {
      manipulator.setOperation(Manipulator::SCALE);
      this->manipulator.setObject(this->box,&this->T);
      this->redisplay();
      return true;
    }
    else if (key=='r' || key=='R')
    {
      manipulator.setOperation(Manipulator::ROTATE);
      this->manipulator.setObject(this->box,&this->T);
      this->redisplay();
      return true;
    }

    return GLCanvas::onKeyboard(key,x,y);
  }

  //onMouseDown
  virtual void onMouseDown(int button,int x,int y)
    {manipulator.onMouseDown(button,x,y,frustum->unproject(x,y)); redisplay();}

  //onMouseMove
  virtual void onMouseMove(int button,int x,int y)
    {manipulator.onMouseMove(button,x,y,frustum->unproject(x,y));redisplay();}

  //onMouseUp
  virtual void onMouseUp(int button,int x,int y)
    {manipulator.onMouseUp(button,x,y,frustum->unproject(x,y));redisplay();}

  //renderScene
  virtual  void renderScene()
  {
    clearScreen();
    setViewport(frustum->x,frustum->y,frustum->width,frustum->height);
    setProjectionMatrix(frustum->projection_matrix);
    setModelviewMatrix(frustum->getModelviewMatrix());
    setDefaultLight(this->frustum->pos,this->frustum->dir);

    SmartPointer<Batch> cube=Batch::Cube(this->box);
    cube->matrix=T * cube->matrix;
    renderBatch(cube);
    manipulator.render(this);
    swapBuffers();
  }
}; //end class


void ManipulatorSelfTest()
{
	DisplayManipulator v;
	v.runLoop();
}




/////////////////////////////////////////////////////////////////
int Mat4fSelfTest()
{
	Log::printf("Testing Mat4f...\n");
	Mat4f identity;

	float _m2[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	Mat4f m2(_m2);

	float _m3[16]={
		Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1),
		Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1),
		Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1),
		Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1)
	};
	Mat4f m3(_m3);

	XgeReleaseAssert(identity.a11()==1.0f && identity.a22()==1.0f && identity.a33()==1.0f && identity.a44()==1.0f);

	//equality and subtract
	XgeReleaseAssert((identity-identity)==Mat4f::zero());

	//transpose
	XgeReleaseAssert(identity.transpose()==identity);
	float _m2t[16]={1,5,9,13,2,6,10,14,3,7,11,15,4,8,12,16};
	XgeReleaseAssert(m2.transpose()==Mat4f(_m2t));

	//inverse
	XgeReleaseAssert(identity.invert()==identity);
	XgeReleaseAssert((m3.invert()*m3).almostIdentity(0.001f));

	//addition/subtraction
	XgeReleaseAssert((((m3+identity)-identity)-m3).almostZero(0.001f));

	//access
	float _m4[16]={
		m3.a11(),m3.a12(),m3.a13(),m3.a14(),
		m3.a21(),m3.a22(),m3.a23(),m3.a24(),
		m3.a31(),m3.a32(),m3.a33(),m3.a34(),
		m3.a41(),m3.a42(),m3.a43(),m3.a44()
	};

	Mat4f m4(_m4);

	float _m5[16]={
		m3(0,0),m3(0,1),m3(0,2),m3(0,3),
		m3(1,0),m3(1,1),m3(1,2),m3(1,3),
		m3(2,0),m3(2,1),m3(2,2),m3(2,3),
		m3(3,0),m3(3,1),m3(3,2),m3(3,3)
	};

	Mat4f m5(_m5);
	XgeReleaseAssert(m3==m4 && m3==m5);

	//multiplication
	XgeReleaseAssert(m3*identity==m3);
	XgeReleaseAssert((((m3*3.0f)*(1.0f/3.0f))-m3).almostZero(0.001f));


	//multiplication for a vector
	XgeReleaseAssert(
		   m3*Vec4f(1,0,0,0)==m3.col(0) 
		&& m3*Vec4f(0,1,0,0)==m3.col(1)
		&& m3*Vec4f(0,0,1,0)==m3.col(2)
		&& m3*Vec4f(0,0,0,1)==m3.col(3));

	XgeReleaseAssert(
		   Vec4f(1,0,0,0)*m3==m3.row(0) 
		&& Vec4f(0,1,0,0)*m3==m3.row(1)
		&& Vec4f(0,0,1,0)*m3==m3.row(2)
		&& Vec4f(0,0,0,1)*m3==m3.row(3));

	//scale and invert
	Mat4f m6=Mat4f::scale(1,2,3);
	float _m6_bis[16]={1/1.0f,0,0,0, 0,1/2.0f,0,0, 0,0,1/3.0f,0, 0,0,0,1};
	XgeReleaseAssert(((m6 * Mat4f(_m6_bis))).almostIdentity(0.001f));

	//translate and invert
	XgeReleaseAssert(((Mat4f::translate(-1,-2,-3) * Mat4f::translate(1,2,3))).almostIdentity(0.001f));

	//rotation and multiplication
	Mat4f m7=identity
		* Mat4f::rotatex(-(float)M_PI/4.0f)
		* Mat4f::rotatey(-(float)M_PI/4.0f)
		* Mat4f::rotatez(-(float)M_PI/4.0f)
		* Mat4f::rotatez(+(float)M_PI/4.0f)
		* Mat4f::rotatey(+(float)M_PI/4.0f)
		* Mat4f::rotatex(+(float)M_PI/4.0f);

	XgeReleaseAssert(m7.almostIdentity(0.001f));

	//decompose
	Vec3f angles((float)M_PI/4.0f,(float)M_PI/5.0f,(float)M_PI/6.0f);

	Mat4f m8=
		identity
		*Mat4f::translate(1,2,3)
		 //YXZ
		*Mat4f::rotatey(angles.y)
		*Mat4f::rotatex(angles.x)
		*Mat4f::rotatez(angles.z)
		*Mat4f::scale(1,2,3);

	Vec3f trans, rot, scale;
	m8.decompose(trans,rot,scale);
	XgeReleaseAssert(
		(trans - Vec3f(1,2,3)).module()<=0.001f &&
		(rot   - angles              ).module()<=0.001f &&
		(scale - Vec3f(1,2,3)).module()<=0.001f);

	//rotate generic axis

	Mat4f _ma=Mat4f::rotate(Vec3f(1,0,0),+angles.x);
	Mat4f _mb=Mat4f::rotatex(+angles.x);

	XgeReleaseAssert((Mat4f::rotate(Vec3f(1,0,0),+angles.x) - Mat4f::rotatex(+angles.x)).almostZero(0.001f));
	XgeReleaseAssert((Mat4f::rotate(Vec3f(0,1,0),+angles.y) - Mat4f::rotatey(+angles.y)).almostZero(0.001f));
	XgeReleaseAssert((Mat4f::rotate(Vec3f(0,0,1),+angles.z) - Mat4f::rotatez(+angles.z)).almostZero(0.001f));
	XgeReleaseAssert((Mat4f::rotate(Vec3f(1,0,0),-angles.x) - Mat4f::rotatex(-angles.x)).almostZero(0.001f));
	XgeReleaseAssert((Mat4f::rotate(Vec3f(0,1,0),-angles.y) - Mat4f::rotatey(-angles.y)).almostZero(0.001f));
	XgeReleaseAssert((Mat4f::rotate(Vec3f(0,0,1),-angles.z) - Mat4f::rotatez(-angles.z)).almostZero(0.001f));

	return 0;

}




/////////////////////////////////////////////////////////////////
int MatfSelfTest()
{
	Log::printf("Testing Matf...\n");


	Matf M1(0);M1.mem[0]=0.0f;
	XgeReleaseAssert(M1.dim==0 && M1(0,0)==0.0f);

	Matf M2(0,1,2,3);
	XgeReleaseAssert(M2.dim==1 && M2(0,0)==0.0f && M2(0,1)==1.0f && M2(1,0)==2.0f && M2(1,1)==3.0f);

	Matf M3(0,1,2,3,4,5,6,7,8);
	XgeReleaseAssert(M3.dim==2 
		&& M3(0,0)==0.0f && M3(0,1)==1.0f && M3(0,2)==2.0f 
		&& M3(1,0)==3.0f && M3(1,1)==4.0f && M3(1,2)==5.0f 
		&& M3(2,0)==6.0f && M3(2,1)==7.0f && M3(2,2)==8.0f);

	float _M4[16]={0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15};
	Matf M4(3,_M4);
	XgeReleaseAssert(M4.dim==3
		&& M4(0,0)== 0.0f && M4(0,1)== 1.0f && M4(0,2)== 2.0f && M4(0,3)== 3.0f 
		&& M4(1,0)== 4.0f && M4(1,1)== 5.0f && M4(1,2)== 6.0f && M4(1,3)== 7.0f 
		&& M4(2,0)== 8.0f && M4(2,1)== 9.0f && M4(2,2)==10.0f && M4(2,3)==11.0f 
		&& M4(3,0)==12.0f && M4(3,1)==13.0f && M4(3,2)==14.0f && M4(3,3)==15.0f );


	Matf m2(1);
	XgeReleaseAssert(m2.dim==1 && m2(0,0)==1 && m2(0,1)==0 && m2(1,0)==0 && m2(1,1)==1);

	Matf m3(2);
	XgeReleaseAssert(m3.dim==2 && m3(0,0)==1 && m3(0,1)==0 && m3(0,2)==0 && m3(1,0)==0 && m3(1,1)==1 && m3(1,2)==0 && m3(2,0)==0 && m3(2,1)==0 && m3(2,2)==1);
 
	XgeReleaseAssert(m3.transpose()==m3);

	//transpose
	{
		Matf m(1,2,3,4);
		m=m.transpose();
		XgeReleaseAssert(m(0,0)==1 && m(0,1)==3 && m(1,0)==2 && m(1,1)==4);
	}

	//assignment
	{
		Matf m(1,2,3,4);
		Matf n(2);
		n=m;
		XgeReleaseAssert(n(0,0)==1 && n(0,1)==2 && n(1,0)==3 && n(1,1)==4);
	}
 
	//zero matrix
	XgeReleaseAssert(Matf::zero(3).almostZero(0));

	//extract
	{
		Matf m(1,2,3,4);

		//down size (1->0)
		XgeReleaseAssert(m.extract(0).dim==0 && m.extract(0)==Matf(0));
		
		//super size (1->2)
		XgeReleaseAssert(m.extract(2).dim==2 && m.extract(2)==Matf(1,2,0, 3,4,0, 0,0,1));
	}

	//swap rows
	{
		Matf m(1.0,2.0,3.0,4.0);
		XgeReleaseAssert(m.dim==1);

		XgeReleaseAssert(m.swapRows(0,1)==Matf(3.0,4.0,1.0,2.0));
		
		std::vector<int> perm;
		perm.push_back(1);
		perm.push_back(0);
		XgeReleaseAssert(m.swapRows(perm)==Matf(3.0,4.0,1.0,2.0));
	}

	//swap cols
	{
		Matf m(1,2,3,4);
		XgeReleaseAssert(m.dim==1);

		XgeReleaseAssert(m.swapCols(0,1)==Matf(2,1,4,3));
		
		std::vector<int> perm;
		perm.push_back(1);
		perm.push_back(0);
		XgeReleaseAssert(m.swapCols(perm)==Matf(2,1,4,3));
	}

	//plus/minus
	{
		XgeReleaseAssert(Matf(1,2,3,4)+Matf(10,20,30,40) == Matf(11,22,33,44));
		XgeReleaseAssert(Matf(1,2,3,4)-Matf(10,20,30,40) == Matf(-9,-18,-27,-36));
	}

	//product
	{
		XgeReleaseAssert(Matf(1,2,3,4)*Matf(5,6,7,8)==Matf(19,22,43,50));
	}

	//product
	{
		XgeReleaseAssert((Matf(1,2,3,4)*0.1f).fuzzyEqual(Matf(0.1f,0.2f,0.3f,0.4f)));
	}

	//vector product
	{
		Matf m(1,2,3, 4,5,6, 7,8,9);
		Vecf c0=m.col(0);
		XgeReleaseAssert(m*Vecf(1.0f,0.0f,0.0f)==c0 && m*Vecf(0.0f,1.0f,0.0f)==m.col(1) && m*Vecf(0.0f,0.0f,1.0f)==m.col(2));
	}

	//plane product
	{
		Matf m(1,2,3, 4,5,6, 7,8,9);
		XgeReleaseAssert(Vecf(1.0f,0.0f,0.0f)*m==m.row(0) && Vecf(0.0f,1.0f,0.0f)*m==m.row(1) && Vecf(0.0f,0.0f,1.0f)*m==m.row(2));
	}

	//invert
	{
		for (int dim=1;dim<8;dim++)
		{
			for (int ntry=0;ntry<10;ntry++)
			{
				Matf m(dim);
				for (int r=0;r<=dim;r++)
				for (int c=0;c<=dim;c++) 
					m.set(r,c,Utils::FloatRand(0,1));
				XgeReleaseAssert((m.invert() * m).almostIdentity());
			}
		}
	}

	//scale
	{
		for (int i=0;i<8;i++)
		{
			int dim=3;
			Vecf v(1,Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));
			Vecf s(0,Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));
			Matf mv=Matf::scaleV(s);
			Matf mh=Matf::scaleH(s);
			XgeReleaseAssert(mv.dim==dim && mh.dim==dim);
			XgeReleaseAssert(mv.invert().fuzzyEqual(mh));
			Vecf t=Vecf(v[0],v[1]*s[1],v[2]*s[2],v[3]*s[3]);
			XgeReleaseAssert((mv*v).fuzzyEqual(t));
		}
	}
	
	//translate
	{
		for (int i=0;i<8;i++)
		{
			int dim=3;
			Vecf v(1,Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));
			Vecf s(0,Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));
			Matf mv=Matf::translateV(s);
			Matf mh=Matf::translateH(s);
			XgeReleaseAssert(mv.invert().fuzzyEqual(mh));
			Vecf t=Vecf(v[0]+s[0],v[1]+s[1],v[2]+s[2],v[3]+s[3]);
			XgeReleaseAssert((mv*v).fuzzyEqual(t));
		}
	}

	//rotate
	{
		for (int i=0;i<8;i++)
		{
			int dim=3;
			Vecf v(1,Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));
			Vecf a(0,Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));

			Matf mv=Matf::rotateV(dim,1,2,a[1])*Matf::rotateV(dim,1,3,a[2])*Matf::rotateV(dim,2,3,a[3]);
			Matf mh=Matf::rotateH(dim,2,3,a[3])*Matf::rotateH(dim,1,3,a[2])*Matf::rotateH(dim,1,2,a[1]);
			XgeReleaseAssert(mv.invert().fuzzyEqual(mh));
		}
	}

	//test congruency with mat4f
	{
		//x rotation
		XgeReleaseAssert(Matf::rotateV(3 ,2,3,(float)M_PI/2).toMat4f().fuzzyEqual(Mat4f::rotatex((float)M_PI/2)));

		//y rotation
		XgeReleaseAssert(Matf::rotateV(3 ,3,1,(float)M_PI/2).toMat4f().fuzzyEqual(Mat4f::rotatey((float)M_PI/2)));

		//z rotation
		XgeReleaseAssert(Matf::rotateV(3 ,1,2,(float)M_PI/2).toMat4f().fuzzyEqual(Mat4f::rotatez((float)M_PI/2)));
	}
	
	return 0;
}


/////////////////////////////////////////// 
int MemPoolSelfTest()
{
	Log::printf("Testing MemPool...\n");

	void* allocated[2][MEMPOOL_TABLE_SIZE];

	for (int size=1;size<MEMPOOL_TABLE_SIZE;size++)
	{
		allocated[0][size]=MemPool::getSingleton()->malloc(size);
		allocated[1][size]=MemPool::getSingleton()->malloc(size);
	}

	for (int size=1;size<MEMPOOL_TABLE_SIZE;size++)
	{
		MemPool::getSingleton()->free(size,allocated[1][size]);
		MemPool::getSingleton()->free(size,allocated[0][size]);
	}

	for (int size=1;size<MEMPOOL_TABLE_SIZE;size++)
	{
		void* temp1=MemPool::getSingleton()->malloc(size);
		void* temp2=MemPool::getSingleton()->malloc(size);

		XgeReleaseAssert(temp1==allocated[0][size]);
		XgeReleaseAssert(temp2==allocated[1][size]);
	}

	return 0;
}






/////////////////////////////////////////////
int Plane4fSelfTest()
{
	Log::printf("Testing Plane4f...\n");


	//from equation
	Plane4f h(Vec3f(1,1,1),10.0f);
	XgeReleaseAssert(fabs(h.getNormal().module()-1)<0.0001f && fabs(h.getDistance(Vec3f(0,0,0))-(-10.0f))<0.0001f);

	//three points
	Vec3f O(2,3,-1);
	h=Plane4f(O+Vec3f(1,0,0),O+Vec3f(0,1,0),O);
	XgeReleaseAssert((h.getNormal()-Vec3f(0,0,1)).module()<0.0001f && fabs(h.getDistance(Vec3f(0,0,0))-(1.0f))<0.0001f);

	//fitting plane
	h=Plane4f(Vec3f(1,1,1),1);
	int npoints=20;

	std::vector<float> points;
	for (int i=0;i<npoints;i++)
	{
		Vec3f O(0,0,0);
		Vec3f D(Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1));
		Ray3f r(O,D);
		Vec3f P=r.intersection(h);

		Vec3f Q=P+Vec3f(Utils::FloatRand(0,0.001f),Utils::FloatRand(0,0.001f),Utils::FloatRand(0,0.001f));
		points.push_back(Q.x);
		points.push_back(Q.y);
		points.push_back(Q.z);
	}

	Plane4f hbis=Plane4f::bestFittingPlane(points);
	XgeReleaseAssert(hbis.fuzzyEqual(h) || hbis.fuzzyEqual(-1.0f*h));

	//check the possibility to find a plane even with very little points
	{
		std::vector<float> points;

		points.push_back(  3.0f);points.push_back(-7.0f);points.push_back(11.0f);
		points.push_back(-15.0f);points.push_back(21.0f);points.push_back(33.0f);

		Plane4f h=Plane4f::bestFittingPlane(points);

		XgeReleaseAssert(fabs(h.getDistance(Vec3f(&points[0])))<0.001f && fabs(h.getDistance(Vec3f(&points[3])))<0.001f);
	}

	//the ability to maintain the orientation of points in 3d
	{
		float points_ccw[]={0,0,0,1,0,0,1,1,0,2,1,0,2,0,0,3,0,0,3,2,0,0,2,0};
		Plane4f h_ccw=Plane4f::bestFittingPlane(8,points_ccw);
		XgeReleaseAssert(h_ccw[2]==1);

		float points_cw[]={0,2,0,3,2,0,3,0,0,2,0,0,2,1,0,1,1,0,1,0,0,0,0,0};
		Plane4f h_cw=Plane4f::bestFittingPlane(8,points_cw);
		XgeReleaseAssert(h_cw[2]==-1);
	}
	
	//all ok
	return 0;

}



/////////////////////////////////////////////
int PlanefSelfTest()
{
	Log::printf("Testing Planef...\n");

	Planef h(Vecf(0.0f,0.0f,0.0f,1.0f),3);//z>=3

	XgeReleaseAssert(h.getDistance(Vecf(1.0f, 0.0f,0.0f,3.0f))== 0);
	XgeReleaseAssert(h.getDistance(Vecf(1.0f, 0.0f,0.0f,0.0f))==-3);

	XgeReleaseAssert(h.forceAbove(Vecf(1.0f, 0.0f,0.0f,10.0f))==   h);
	XgeReleaseAssert(h.forceBelow(Vecf(1.0f, 0.0f,0.0f,10.0f))==-1*h);

	XgeReleaseAssert(fabs(Planef(Vecf(0.0f,1.0f,1.0f,1.0f),0).getNormal().module()-1)<0.0001f);

	//set best fitting plane with Vec3f(1.0,x,y,z)

	for (int dim=1;dim<16;dim++)
	{
		//plane x+y+...>=1
		Planef h(dim);
		h.mem[0]=-1;
		for (int _i=1;_i<=dim;_i++) h.mem[_i]=1.0f;
		h.normalize();

		const int npoints=20;
		std::vector<float> points;

		for (int i=0;i<npoints;i++)
		{
			//it's a point in the origin
			Vecf O(dim);
			O.mem[0]=1.0f; 

			//it's a random direction (first component is 0)
			Vecf D(dim);
			for (int j=1;j<=dim;j++) 
				D.mem[j]=Utils::FloatRand(0,1);

			Rayf r(O,D);
			Vecf inters=r.intersection(h);
			XgeReleaseAssert(inters[0]==1 && fabs(h.getDistance(inters))<0.001f);

			//NOTE: I'm removing the homo coordinae
			for (int U=1;U<=dim;U++)
				points.push_back(inters[U]);

			XgeReleaseAssert(fabs(h.getDistance(inters))<0.001f);
		}

		Planef hbis=Planef::bestFittingPlane(dim,npoints,&points[0]);
		XgeReleaseAssert(hbis.fuzzyEqual(h) || hbis.fuzzyEqual(-1.0f*h));
	}


	//set best fitting plane with a vector of float x,y,z,x,y,z
	for (int dim=1;dim<16;dim++)
	{
		//plane x+y+...>=1
		Planef h(dim);
		h.mem[0]=-1;
		for (int _i=1;_i<=dim;_i++) h.mem[_i]=1.0f;
		h.normalize();

		const int npoints=20;

		float* points=new float[npoints*dim];

		for (int i=0;i<npoints;i++)
		{
			//it's a point in the origin
			Vecf O(dim);
			O.mem[0]=1.0f; 

			//it's a random direction (first component is 0)
			Vecf D(dim);
			for (int j=1;j<=dim;j++) D.mem[j]=Utils::FloatRand(0,1);

			Rayf r(O,D);
			Vecf inters=r.intersection(h);
			XgeReleaseAssert(inters[0]==1 && fabs(h.getDistance(inters))<0.001f);

			memcpy(points+i*dim,&inters.mem[1],sizeof(float)*dim);
		}

		Planef hbis=Planef::bestFittingPlane(dim,npoints,points);
		XgeReleaseAssert(hbis.fuzzyEqual(h) || hbis.fuzzyEqual(-1.0f*h));
		delete [] points;
	}

	//random points on random planes
	{
		for (int dim=2;dim<10;dim++)
		{
			Planef h=Planef::getRandomPlane(dim);

			for (int I=0;I<dim*4;I++)
			{
				Vecf p=h.getRandomPoint();
				XgeReleaseAssert (h.getDistance(p)<0.0001f);
			}
		}
	}

	//check the possibility to find a plane even with very little points

	{
		int Dim=5;
		int npoints=2;
		std::vector<float> points;
		points.push_back(  3.0f);points.push_back( -7.0f);points.push_back(11.0f);points.push_back(+0.5f);points.push_back(+100);
		points.push_back(-15.0f);points.push_back( 21.0f);points.push_back(33.0f);points.push_back(-0.7f);points.push_back(-46);

		Planef h=Planef::bestFittingPlane(Dim,points);
		XgeReleaseAssert(h.dim==Dim);

		//check if points are on the plane
		for (int J=0;J<npoints;J++)
		{
			Vecf T(Dim,1,&points[J*Dim]);
			float distance=fabs(h.getDistance(T));
			XgeReleaseAssert(distance<0.001f);
		}
		
	}



	return 0;
}




///////////////////////////////////////////////////
static inline SmartPointer<Hpc> mkpol_ukpol(SmartPointer<Hpc> src)
{
	std::vector<float> points;
	std::vector<std::vector<int> > hulls;
	int pointdim=Plasm::ukpol(src,points,hulls);
	SmartPointer<Hpc> ret=Plasm::mkpol(pointdim,points,hulls);
	return ret;
}

template<typename T>
static std::vector<T> make_vector(int n,T *v)
{
	std::vector<T> ret;
	for (int i=0;i<n;i++) ret.push_back(v[i]);
	return ret;
}



int PlasmSelfTest()
{
	Log::printf("Testing Plasm::..\n");

	//simplex bulders
	{
		int N=0;
		std::vector<SmartPointer<Hpc> >__simplices;
		__simplices.push_back(Plasm::translate(Plasm::simplex(0),3,1,0.0f));
		__simplices.push_back(Plasm::translate(Plasm::simplex(1),3,1,1.0f));
		__simplices.push_back(Plasm::translate(Plasm::simplex(2),3,1,2.0f));
		__simplices.push_back(Plasm::translate(Plasm::simplex(3),3,1,3.0f));
		SmartPointer<Hpc> Struct=Plasm::Struct(__simplices);
		Plasm::view(Struct);
	}
	XgeReleaseAssert(!xge_total_hpc);

	//cube bulders (and check limits)
	{
		int N=0;
		std::vector<SmartPointer<Hpc> > __cubes;
		
		__cubes.push_back(Plasm::translate(Plasm::cube(0),3,1,0.0f));
		__cubes.push_back(Plasm::translate(Plasm::cube(1),3,1,1.0f));
		__cubes.push_back(Plasm::translate(Plasm::cube(2),3,1,2.0f));
		__cubes.push_back(Plasm::translate(Plasm::cube(3),3,1,3.0f));

		SmartPointer<Hpc> cubes=Plasm::Struct(__cubes);
		Boxf box=Plasm::limits(cubes);
		XgeReleaseAssert(box.fuzzyEqual(Boxf(Vecf(1.0f,0.0f,0.0f,0.0f),Vecf(1.0f,4.0f,1.0f,1.0f))));

		Plasm::view(cubes);
	}
	XgeReleaseAssert(!xge_total_hpc);

	//join
	{
		int N=0;
		std::vector<SmartPointer<Hpc> > __cubes;
		__cubes.push_back(Plasm::translate(Plasm::cube(0),3,1,0.0f));
		__cubes.push_back(Plasm::translate(Plasm::cube(1),3,1,1.0f));
		__cubes.push_back(Plasm::translate(Plasm::cube(2),3,1,2.0f));
		__cubes.push_back(Plasm::translate(Plasm::cube(3),3,1,3.0f));

		SmartPointer<Hpc> join=Plasm::join(__cubes);
		Plasm::view(join);
	}
	XgeReleaseAssert(!xge_total_hpc);


	//mkpol
	{
		float dx=0;
		float _dim0_points[]={dx+0,0,0};std::vector<float> dim0_points=make_vector(sizeof(_dim0_points)/sizeof(float),_dim0_points);
		int  _dim0_hull0[]={0};std::vector<int> dim0_hull=make_vector(sizeof(_dim0_hull0)/sizeof(int),_dim0_hull0);
		std::vector<std::vector<int> > dim0_hulls;dim0_hulls.push_back(dim0_hull);
		SmartPointer<Hpc> dim0_mkpol=Plasm::mkpol(3,dim0_points,dim0_hulls);
		XgeReleaseAssert(Plasm::getSpaceDim(dim0_mkpol)==3);
		XgeReleaseAssert(Plasm::getPointDim(dim0_mkpol)==0);

		dx+=1;
		float _dim1_points[]={dx+0,0,0,dx+1,0,0};std::vector<float> dim1_points=make_vector(sizeof(_dim1_points)/sizeof(float),_dim1_points);
		int  _dim1_hull[]={0,1};std::vector<int> dim1_hull=make_vector(sizeof(_dim1_hull)/sizeof(int),_dim1_hull);
		std::vector<std::vector<int> > dim1_hulls;dim1_hulls.push_back(dim1_hull);
		SmartPointer<Hpc> dim1_mkpol=Plasm::mkpol(3,dim1_points,dim1_hulls);
		XgeReleaseAssert(Plasm::getSpaceDim(dim1_mkpol)==3);
		XgeReleaseAssert(Plasm::getPointDim(dim1_mkpol)==1);

		dx+=1;
		float _dim2_points[]={dx+0,0,0,dx+1,0,0,dx+1,1,0,dx+0,1,0};std::vector<float> dim2_points=make_vector(sizeof(_dim2_points)/sizeof(float),_dim2_points);
		int  _dim2_hull[]={0,1,2,3};std::vector<int> dim2_hull=make_vector(sizeof(_dim2_hull)/sizeof(int),_dim2_hull);
		std::vector<std::vector<int> > dim2_hulls;dim2_hulls.push_back(dim2_hull);
		SmartPointer<Hpc> dim2_mkpol=Plasm::mkpol(3,dim2_points,dim2_hulls);
		XgeReleaseAssert(Plasm::getSpaceDim(dim2_mkpol)==3);
		XgeReleaseAssert(Plasm::getPointDim(dim2_mkpol)==2);

		dx+=1;
		float _dim3_points[]={dx+0,0,0,dx+1,0,0,dx+1,1,0,dx+0,1,0, dx+0,0,1,dx+1,0,1,dx+1,1,1,dx+0,1,1};std::vector<float> dim3_points=make_vector(sizeof(_dim3_points)/sizeof(float),_dim3_points);
		int  _dim3_hull[]={0,1,2,3,4,5,6,7};std::vector<int> dim3_hull=make_vector(sizeof(_dim3_hull)/sizeof(int),_dim3_hull);
		std::vector<std::vector<int> > dim3_hulls;dim3_hulls.push_back(dim3_hull);
		SmartPointer<Hpc> dim3_mkpol=Plasm::mkpol(3,dim3_points,dim3_hulls);
		XgeReleaseAssert(Plasm::getSpaceDim(dim3_mkpol)==3);
		XgeReleaseAssert(Plasm::getPointDim(dim3_mkpol)==3);


		std::vector<SmartPointer<Hpc> > __args;
		__args.push_back(dim0_mkpol);
		__args.push_back(dim1_mkpol);
		__args.push_back(dim2_mkpol);
		__args.push_back(dim3_mkpol);

		SmartPointer<Hpc> cubes=Plasm::Struct(__args);
		Plasm::view(cubes);
	}
	XgeReleaseAssert(!xge_total_hpc);



	
	//rotate
	{
		int N=0;
		std::vector<SmartPointer<Hpc> > __cubes;
		__cubes.push_back(Plasm::rotate(Plasm::cube(2),3,1,2,0*(float)M_PI/8));
		__cubes.push_back(Plasm::rotate(Plasm::cube(2),3,1,2,1*(float)M_PI/8));
		__cubes.push_back(Plasm::rotate(Plasm::cube(2),3,1,2,2*(float)M_PI/8));
		__cubes.push_back(Plasm::rotate(Plasm::cube(2),3,1,2,3*(float)M_PI/8));
		SmartPointer<Hpc> cubes=Plasm::Struct(__cubes);
		Plasm::view(cubes);
	}
	XgeReleaseAssert(!xge_total_hpc);

	//scale
	{
		int N=0;
		std::vector<SmartPointer<Hpc> > __cubes;

		__cubes.push_back(Plasm::scale(Plasm::translate(Plasm::cube(2),2,1,0.0f), 3,2,1.0));
		__cubes.push_back(Plasm::scale(Plasm::translate(Plasm::cube(2),2,1,1.0f),3,2,2.0));
		__cubes.push_back(Plasm::scale(Plasm::translate(Plasm::cube(2),2,1,2.0f),3,2,3.0));
		__cubes.push_back(Plasm::scale(Plasm::translate(Plasm::cube(2),2,1,3.0f),3,2,4.0));
	
		SmartPointer<Hpc> cubes=Plasm::Struct(__cubes);
		Plasm::view(cubes);
	}
	XgeReleaseAssert(!xge_total_hpc);




	//skeleton
	{
		int N=0;
		std::vector<SmartPointer<Hpc> > __cubes;
		__cubes.push_back(Plasm::translate(Plasm::cube(0),3,1,0.0f));
		__cubes.push_back(Plasm::translate(Plasm::cube(1),3,1,1.0f));
		__cubes.push_back(Plasm::translate(Plasm::cube(2),3,1,2.0f));
		__cubes.push_back(Plasm::translate(Plasm::cube(3),3,1,3.0f));

		SmartPointer<Hpc> cubes=Plasm::Struct(__cubes);

		SmartPointer<Hpc> s0=Plasm::skeleton(cubes,0);
		SmartPointer<Hpc> s1=Plasm::skeleton(cubes,1);
		SmartPointer<Hpc> s2=Plasm::skeleton(cubes,2);

		std::vector<SmartPointer<Hpc> > __args;
		__args.push_back(s0);
		__args.push_back(s1);
		__args.push_back(s2);
		SmartPointer<Hpc> s=Plasm::Struct(__args);

		Plasm::view(s);
	}
	XgeReleaseAssert(!xge_total_hpc);



	//ukpol
	{
		
		float dx=0;
		float dz=-2;
		float _dim0_points[]={dx+0,0,dz+0};std::vector<float> dim0_points=make_vector(sizeof(_dim0_points)/sizeof(float),_dim0_points);
		int  _dim0_hull[]={0};std::vector<int> dim0_hull=make_vector(sizeof(_dim0_hull)/sizeof(int),_dim0_hull);
		std::vector<std::vector<int> > dim0_hulls;dim0_hulls.push_back(dim0_hull);
		SmartPointer<Hpc> dim0_mkpol=Plasm::mkpol(3,dim0_points,dim0_hulls);
		SmartPointer<Hpc> dim0_ukpol=mkpol_ukpol(dim0_mkpol);

		dx+=1;dz+=1;
		float _dim1_points[]={dx+0,0,dz+0, dx+1,0,dz+0};std::vector<float> dim1_points=make_vector(sizeof(_dim1_points)/sizeof(float),_dim1_points);
		int  _dim1_hull[]={0,1};std::vector<int> dim1_hull=make_vector(sizeof(_dim1_hull)/sizeof(int),_dim1_hull);
		std::vector<std::vector<int> > dim1_hulls;dim1_hulls.push_back(dim1_hull);
		SmartPointer<Hpc> dim1_mkpol=Plasm::mkpol(3,dim1_points,dim1_hulls);
		SmartPointer<Hpc> dim1_ukpol=mkpol_ukpol(dim1_mkpol);


		dx+=1;dz+=1;
		float _dim2_points[]={dx+0,0,dz+0, dx+1,0,dz+0, dx+1,1,dz+0, dx+0,1,dz+0};std::vector<float> dim2_points=make_vector(sizeof(_dim2_points)/sizeof(float),_dim2_points);
		int  _dim2_hull[]={0,1,2,3};std::vector<int> dim2_hull=make_vector(sizeof(_dim2_hull)/sizeof(int),_dim2_hull);
		std::vector<std::vector<int> > dim2_hulls;dim2_hulls.push_back(dim2_hull);
		SmartPointer<Hpc> dim2_mkpol=Plasm::mkpol(3,dim2_points,dim2_hulls);
		SmartPointer<Hpc> dim2_ukpol=mkpol_ukpol(dim2_mkpol);


		dx+=1;dz+=1;
		float _dim3_points[]={dx+0,0,dz+0, dx+1,0,dz+0, dx+1,1,dz+0, dx+0,1,dz+0, dx+0,0,dz+1, dx+1,0,dz+1, dx+1,1,dz+1, dx+0,1,dz+1};std::vector<float> dim3_points=make_vector(sizeof(_dim3_points)/sizeof(float),_dim3_points);
		int  _dim3_hull[]={0,1,2,3,4,5,6,7};std::vector<int> dim3_hull=make_vector(sizeof(_dim3_hull)/sizeof(int),_dim3_hull);
		std::vector<std::vector<int> > dim3_hulls;dim3_hulls.push_back(dim3_hull);
		SmartPointer<Hpc> dim3_mkpol=Plasm::mkpol(3,dim3_points,dim3_hulls);
		SmartPointer<Hpc> dim3_ukpol=mkpol_ukpol(dim3_mkpol);

		std::vector<SmartPointer<Hpc> > __args;
		__args.push_back(dim0_ukpol);
		__args.push_back(dim1_ukpol);
		__args.push_back(dim2_ukpol);
		__args.push_back(dim3_ukpol);

		SmartPointer<Hpc> cubes=Plasm::Struct(__args);
		Plasm::view(cubes);
	}
	XgeReleaseAssert(!xge_total_hpc);


		return 0;

	//embed
	{
		SmartPointer<Hpc> embed=Plasm::embed(Plasm::cube(1),3);
		XgeReleaseAssert(Plasm::getSpaceDim(embed)==3 && Plasm::getPointDim(embed)==1);
		Plasm::view(embed);
	}
	XgeReleaseAssert(!xge_total_hpc);

	//transform
	{
		SmartPointer<Hpc> cube3=Plasm::cube(3);


		float __vmat[16]=
		{
			1,0,0,0,
			0,1,0,1,
			0,0,1,1,
			0,0,0,1
		};

		SmartPointer<Matf> vmat(new Matf(3,__vmat));
		SmartPointer<Matf> hmat(new Matf(vmat->invert()));
		SmartPointer<Hpc> transform=Plasm::transform(cube3,vmat,hmat);

		Plasm::view(transform);
	}
	XgeReleaseAssert(!xge_total_hpc);


	//boolean operation 2d
	{ 
		int N=0;
		SmartPointer<Hpc> base_cube=Plasm::translate(Plasm::translate(Plasm::cube(2),2,1,-0.5),2,2,-0.5);

		std::vector<SmartPointer<Hpc> > __cubes;
		__cubes.push_back(Plasm::rotate(base_cube,2,1,2,0*(float)M_PI/8));
		__cubes.push_back(Plasm::rotate(base_cube,2,1,2,1*(float)M_PI/8));
		__cubes.push_back(Plasm::rotate(base_cube,2,1,2,2*(float)M_PI/8));
		__cubes.push_back(Plasm::rotate(base_cube,2,1,2,3*(float)M_PI/8));

	
		SmartPointer<Hpc> bool_or  =Plasm::translate(Plasm::boolop(BOOL_CODE_OR  ,__cubes),2,1,0.0f);
		SmartPointer<Hpc> bool_and =Plasm::translate(Plasm::boolop(BOOL_CODE_AND ,__cubes),2,1,2.0f);
		SmartPointer<Hpc> bool_dif =Plasm::translate(Plasm::boolop(BOOL_CODE_DIFF,__cubes),2,1,4.0f);
		SmartPointer<Hpc> bool_xor =Plasm::translate(Plasm::boolop(BOOL_CODE_XOR ,__cubes),2,1,6.0f);

		std::vector<SmartPointer<Hpc> > __args;
		__args.push_back(bool_or);
		__args.push_back(bool_and);
		__args.push_back(bool_dif);
		__args.push_back(bool_xor);
		SmartPointer<Hpc> boolop=Plasm::Struct(__args);

		Plasm::view(boolop);
	}
	XgeReleaseAssert(!xge_total_hpc);


	//power
	{
		int N=0;
		std::vector<SmartPointer<Hpc> > __cubes2d;
		__cubes2d.push_back(Plasm::translate(Plasm::cube(2),2,1,0.0f));
		__cubes2d.push_back(Plasm::translate(Plasm::cube(2),2,1,1.5f));

		SmartPointer<Hpc> s1=Plasm::Struct(__cubes2d);

		std::vector<SmartPointer<Hpc> > __cubes1d;
		__cubes1d.push_back(Plasm::translate(Plasm::cube(1),1,1,0.0f));
		__cubes1d.push_back(Plasm::translate(Plasm::cube(1),1,1,1.5f));

		SmartPointer<Hpc> s2=Plasm::Struct(__cubes1d);

		SmartPointer<Hpc> power=Plasm::power(s1,s2/*,1e-6f,10,0*/);
		Plasm::view(power);
	}
	XgeReleaseAssert(!xge_total_hpc);


	//boolean operation 3d
	{ 
		int N=0;
		SmartPointer<Hpc> base_cube=Plasm::translate(Plasm::translate(Plasm::translate(Plasm::cube(3),3,1,-0.5),3,2,-0.5),3,3,-0.5);

		std::vector< SmartPointer<Hpc> > __cubes;
		__cubes.push_back(Plasm::rotate(base_cube,3,1,2,0*(float)M_PI/8));
		__cubes.push_back(Plasm::rotate(base_cube,3,1,2,1*(float)M_PI/8));
		__cubes.push_back(Plasm::rotate(base_cube,3,1,2,2*(float)M_PI/8));
		__cubes.push_back(Plasm::rotate(base_cube,3,1,2,3*(float)M_PI/8));

	
		SmartPointer<Hpc> bool_or  =Plasm::translate(Plasm::boolop(BOOL_CODE_OR  ,__cubes),2,1,0.0f);
		SmartPointer<Hpc> bool_and =Plasm::translate(Plasm::boolop(BOOL_CODE_AND ,__cubes),2,1,2.0f);
		SmartPointer<Hpc> bool_dif =Plasm::translate(Plasm::boolop(BOOL_CODE_DIFF,__cubes),2,1,4.0f);
		SmartPointer<Hpc> bool_xor =Plasm::translate(Plasm::boolop(BOOL_CODE_XOR ,__cubes),2,1,6.0f);

		std::vector<SmartPointer<Hpc> > __args;
		__args.push_back(bool_or);
		__args.push_back(bool_and);
		__args.push_back(bool_dif);
		__args.push_back(bool_xor);
		SmartPointer<Hpc> boolop=Plasm::Struct(__args);

		Plasm::view(boolop);
	}
	XgeReleaseAssert(!xge_total_hpc);

	//properties
	{
		SmartPointer<Hpc> base_cube=Plasm::translate(
				Plasm::translate(
					Plasm::translate(
						Plasm::cube(3)
					,3,1,-0.5)
				,3,2,-0.5)
			,3,3,-0.5);

		char* Red   = "1.0 0.0 0.0 1.0";
		char* Blue  = "0.0 0.0 1.0 1.0";
		char* Green = "0.0 1.0 0.0 1.0";
		char* Mtl   = "0.1 0.1 0.1 1.0   0.8 0.2 0.8 1.0   1.0 1.0 1.0 1.0   0.0 0.0 0.0 1.0   100.0";
		
		std::vector<SmartPointer<Hpc> > __cubes;
		__cubes.push_back(Plasm::addProperty(Plasm::translate(base_cube,3,1,0.0f),HPC_PROP_RGB_COLOR    ,Red   ));
		__cubes.push_back(Plasm::addProperty(Plasm::translate(base_cube,3,1,1.5f),HPC_PROP_VRML_MATERIAL,Mtl   ));
		__cubes.push_back(Plasm::addProperty(Plasm::translate(base_cube,3,1,3.0f),HPC_PROP_RGB_COLOR    ,Blue  ));
		__cubes.push_back(Plasm::addProperty(Plasm::translate(base_cube,3,1,4.5f),HPC_PROP_RGB_COLOR    ,Green ));

		SmartPointer<Hpc> Struct=Plasm::Struct(__cubes);
		Plasm::view(Struct);
	}

	//transparency
	if (true)
	{
		SmartPointer<Hpc> cuboid_central=Plasm::cube(3,-0.3f,0.3f);
	
		SmartPointer<Hpc> cuboid_px=Plasm::addProperty(Plasm::translate(cuboid_central,3,1,+1),HPC_PROP_VRML_MATERIAL,"0.1 0.1 0.1 1.0   0.8 0.1 0.1 0.5  0 0 0 1.0   0.0 0.0 0.0 1.0   100.0");
		SmartPointer<Hpc> cuboid_py=Plasm::addProperty(Plasm::translate(cuboid_central,3,2,+1),HPC_PROP_VRML_MATERIAL,"0.1 0.1 0.1 1.0   0.1 0.8 0.1 0.5  0 0 0 1.0   0.0 0.0 0.0 1.0   100.0");
		SmartPointer<Hpc> cuboid_pz=Plasm::addProperty(Plasm::translate(cuboid_central,3,3,+1),HPC_PROP_VRML_MATERIAL,"0.1 0.1 0.1 1.0   0.1 0.1 0.8 0.5  0 0 0 1.0   0.0 0.0 0.0 1.0   100.0");

		SmartPointer<Hpc> cuboid_nx=Plasm::addProperty(Plasm::translate(cuboid_central,3,1,-1),HPC_PROP_VRML_MATERIAL,"0.1 0.1 0.1 1.0   0.8 0.1 0.1 0.5  0 0 0 1.0   0.0 0.0 0.0 1.0   100.0");
		SmartPointer<Hpc> cuboid_ny=Plasm::addProperty(Plasm::translate(cuboid_central,3,2,-1),HPC_PROP_VRML_MATERIAL,"0.1 0.1 0.1 1.0   0.1 0.8 0.1 0.5  0 0 0 1.0   0.0 0.0 0.0 1.0   100.0");
		SmartPointer<Hpc> cuboid_nz=Plasm::addProperty(Plasm::translate(cuboid_central,3,3,-1),HPC_PROP_VRML_MATERIAL,"0.1 0.1 0.1 1.0   0.1 0.1 0.8 0.5  0 0 0 1.0   0.0 0.0 0.0 1.0   100.0");


		std::vector<SmartPointer<Hpc> > args;
		args.push_back(cuboid_central);
		args.push_back(cuboid_px);
		args.push_back(cuboid_py);
		args.push_back(cuboid_pz);
		args.push_back(cuboid_nx);
		args.push_back(cuboid_ny);
		args.push_back(cuboid_nz);
		SmartPointer<Hpc> Out=Plasm::Struct(args);
		Plasm::view(Out);
	}

	XgeReleaseAssert(!xge_total_hpc);
	return 0;
}




////////////////////////////////////////////////////////////////////////////
int QuaternionSelfTest()
{
	Log::printf("Testing Quaternion...\n");

	Quaternion q;
	XgeReleaseAssert(q.getMatrix().almostIdentity(0.001f) && Utils::FuzzyEqual(q.getAngle(),0));

	q=Quaternion(Vec3f(1,1,1),(float)M_PI/4);
	XgeReleaseAssert(Utils::FuzzyEqual((q.getAxis()-Vec3f(1,1,1).normalize()).module(),0,0.0001f));
	XgeReleaseAssert(Utils::FuzzyEqual(q.getAngle(),(float)M_PI/4,0.0001f));

	//inverse
	XgeReleaseAssert((q*q.Inverse()).getMatrix().almostIdentity(0.001f));

	//multiply a vector
	Vec3f V=Vec3f(Utils::FloatRand(0,1),Utils::FloatRand(0,1),Utils::FloatRand(0,1)).normalize();
	XgeReleaseAssert(((q.getMatrix()* V)-(q * V)).module()<0.001f);

	Vec3f rotations[]=
	{
		Vec3f(-1,-1,-1),
		Vec3f(+1,-1,-1),
		Vec3f(-1,+1,-1),
		Vec3f(+1,+1,-1),
		Vec3f(-1,-1,+1),
		Vec3f(+1,-1,+1),
		Vec3f(-1,+1,+1),
		Vec3f(+1,+1,+1)
	};

	for (int i=0;i<8;i++)
	{
		float angle=(float)M_PI/4.0f;

		rotations[i]=rotations[i].normalize();

		Mat4f _rotmat=Mat4f::rotate(rotations[i],angle);
		Quaternion q(_rotmat);
		XgeReleaseAssert((q.getAxis()-rotations[i]).module()<0.001f && fabs(q.getAngle()-angle)<0.001f);

		Mat4f m1=Mat4f::rotate(rotations[i],angle);
		Mat4f m2=q.getMatrix();
		XgeReleaseAssert((m2 - m1).almostZero(0.001f));

		{
			q=Quaternion(rotations[i],angle);
			XgeReleaseAssert((q.getAxis()-rotations[i]).module()<0.001f && fabs(q.getAngle()-angle)<0.001f);
		}

		XgeReleaseAssert(((q.getMatrix()* V)-(q * V)).module()<0.001f);
	}

	q=Quaternion(Vec3f(1,0,0),Vec3f(0,1,0),Vec3f(0,0,1));
	Vec3f X,Y,Z;
	q.getAxis(X,Y,Z);
	XgeReleaseAssert( (X-Vec3f(1,0,0)).module()<0.001);
	XgeReleaseAssert( (Y-Vec3f(0,1,0)).module()<0.001);
	XgeReleaseAssert( (Z-Vec3f(0,0,1)).module()<0.001);
	XgeReleaseAssert(q.getMatrix().almostIdentity(0.001f));

	//test product
	Quaternion qx(Vec3f(1,0,0),(float)M_PI/4.0);
	Quaternion qy(Vec3f(0,1,0),(float)-M_PI/3.0);
	Quaternion qz(Vec3f(0,0,1),(float)M_PI/8.0);

	Mat4f M1=(qx*qy*qz).getMatrix();
	Mat4f M2=(Mat4f::rotatex((float)M_PI/4.0) * Mat4f::rotatey(-(float)M_PI/3.0) * Mat4f::rotatez((float)M_PI/8.0));
	XgeReleaseAssert((M1 - M2 ).almostZero(0.0001f));
	XgeReleaseAssert(((qx*qy*qz)*V - M2*V).module()<0.001f );


	return 0;
}


////////////////////////////////////////////////////////////////////////////
int Ray3fSelfTest()
{
	Log::printf("Testing Ray3f...\n");


	Ray3f r(Vec3f(0,0,0),Vec3f(1,1,1));
	XgeReleaseAssert((r.dir.module()-1)<0.0001f);

	Ray3f r2(r);
	r2.setOrigin(r.origin+Vec3f(0.0001f,0,0));
	XgeReleaseAssert(r==r && r!=r2);

	XgeReleaseAssert(r.getPoint(0).fuzzyEqual(Vec3f(0,0,0)) && r.getPoint(1).fuzzyEqual(Vec3f(1,1,1).normalize())) ;

	//intersection
	Vec3f i=Ray3f(Vec3f(0,0,0),Vec3f(0,0,1)).intersection(Plane4f(Vec3f(0,0,1),+2));
	XgeReleaseAssert(i.fuzzyEqual(Vec3f(0,0,2)));


	//intersect line
	{
		Ray3f ray(Vec3f(0,0,0),Vec3f(1,0,0));
		float p0[]={1,-1.01f};
		float p1[]={1,+1.01f};
		float dist=ray.intersectLine(p0,p1);
		XgeReleaseAssert(fabs(dist-1.0f)<0.001f);

		ray=Ray3f(Vec3f(0,0,0),Vec3f(1,1,0));
		dist=ray.intersectLine(p0,p1);
		XgeReleaseAssert(fabs(dist-sqrt(2.0f))<0.001f);

		ray=Ray3f(Vec3f(0,0,0),Vec3f(0,1,0));
		dist=ray.intersectLine(p0,p1);
		XgeReleaseAssert(dist==-1);
	}

	//intertect triangle
	{

		Ray3f ray(Vec3f(0,0,0),Vec3f(0,0,1));
		Vec3f p0(0,0,1);
		Vec3f p1(1.01f,0,1);
		Vec3f p2(1.01f,1.01f,1);

		 float dist=ray.intersectTriangle(p0,p1,p2);
		 XgeReleaseAssert(fabs(dist-1.0)<0.001f);

		ray=Ray3f(Vec3f(0,0,0),Vec3f(1,1,1));
		dist=ray.intersectTriangle(p0,p1,p2);
		XgeReleaseAssert(fabs(dist-sqrt(3.0f))<0.001f);

		ray=Ray3f(Vec3f(0,0,0),Vec3f(1,0,0));
		dist=ray.intersectTriangle(p0,p1,p2);
		XgeReleaseAssert(dist==-1);
	}

	//intersect box
	{
		Box3f box;
		float tmin,tmax;
		
		box=Box3f(Vec3f(-1,-1,-1),Vec3f(+1,+1,+1));
		XgeReleaseAssert(Ray3f(Vec3f(0,0,0),Vec3f(0,0,1)).intersectBox(tmin,tmax,box) && Utils::FuzzyEqual(tmin,-1) && Utils::FuzzyEqual(tmax,+1));
		XgeReleaseAssert(Ray3f(Vec3f(0,0,0),Vec3f(0,1,0)).intersectBox(tmin,tmax,box) && Utils::FuzzyEqual(tmin,-1) && Utils::FuzzyEqual(tmax,+1));
		XgeReleaseAssert(Ray3f(Vec3f(0,0,0),Vec3f(1,0,0)).intersectBox(tmin,tmax,box) && Utils::FuzzyEqual(tmin,-1) && Utils::FuzzyEqual(tmax,+1));

		box=Box3f(Vec3f(0,0,0),Vec3f(1,1,1));

		//test limit cases
		XgeReleaseAssert(Ray3f(Vec3f(-1,+1e-4f,+1e-4f),Vec3f(1,0,0)).intersectBox(tmin,tmax,box) && Utils::FuzzyEqual(tmin,1) && Utils::FuzzyEqual(tmax,+2));
		XgeReleaseAssert(Ray3f(Vec3f(+1e-4f,-1,+1e-4f),Vec3f(0,1,0)).intersectBox(tmin,tmax,box) && Utils::FuzzyEqual(tmin,1) && Utils::FuzzyEqual(tmax,+2));
		XgeReleaseAssert(Ray3f(Vec3f(+1e-4f,+1e-4f,-1),Vec3f(0,0,1)).intersectBox(tmin,tmax,box) && Utils::FuzzyEqual(tmin,1) && Utils::FuzzyEqual(tmax,+2));

		XgeReleaseAssert(!Ray3f(Vec3f(-1,-1e-4f,-1e-4f),Vec3f(1,0,0)).intersectBox(tmin,tmax,box));
		XgeReleaseAssert(!Ray3f(Vec3f(-1e-4f,-1,-1e-4f),Vec3f(0,1,0)).intersectBox(tmin,tmax,box));
		XgeReleaseAssert(!Ray3f(Vec3f(-1e-4f,-1e-4f,-1),Vec3f(0,0,1)).intersectBox(tmin,tmax,box));

		XgeReleaseAssert(!Ray3f(Vec3f(-1,0,0),Vec3f(0,1,0)).intersectBox(tmin,tmax,box));
		XgeReleaseAssert(!Ray3f(Vec3f(-1,0,0),Vec3f(0,0,1)).intersectBox(tmin,tmax,box));

		XgeReleaseAssert(!Ray3f(Vec3f(0,-1,0),Vec3f(1,0,0)).intersectBox(tmin,tmax,box));
		XgeReleaseAssert(!Ray3f(Vec3f(0,-1,0),Vec3f(0,0,1)).intersectBox(tmin,tmax,box));

		XgeReleaseAssert(!Ray3f(Vec3f(0,0,-1),Vec3f(1,0,0)).intersectBox(tmin,tmax,box));
		XgeReleaseAssert(!Ray3f(Vec3f(0,0,-1),Vec3f(0,1,0)).intersectBox(tmin,tmax,box));
	}

	return 0;
}




////////////////////////////////////////////////////////////////////////////
int RayfSelfTest()
{
	Log::printf("Testing Rayf...\n");

	//test intersection
	for (int dim=1;dim<16;dim++)
	{
		//the plane
		Planef plane(dim);

		//x+y+z+...>=1
		plane.mem[0]=-1;
		for (int i=1;i<=dim;i++) plane.mem[i]=1;

		//a point in the origin
		Vecf Origin(dim);
		Origin.mem[0]=1.0f; //is a point

		const int ntests=100;
		for (int i=0;i<ntests;i++)
		{
			Vecf Dir(dim);
			Dir.mem[0]=0; //is a vector
			for (int i=1;i<=dim;i++) Dir.mem[i]=Utils::FloatRand(0,1);

			Rayf ray(Origin,Dir);
			Vecf point=ray.intersection(plane);
			XgeReleaseAssert(fabs(plane.getDistance(point))<0.0001f);
		}
	}
	

	return 0;
}



/////////////////////////////////////////////////////////////////
class TextureViewer:public GLCanvas
{
	SmartPointer<Texture> texture0;
	SmartPointer<Texture> texture1;

public:

	//constructor
	TextureViewer(SmartPointer<Texture> texture0,SmartPointer<Texture> texture1)
	{
		this->texture0=texture0;
		this->texture1=texture1;
	}

	//renderScene
	virtual void renderScene()
	{
		int W=this->frustum->width;
		int H=this->frustum->height;
		SmartPointer<Batch> batch(new Batch);
		batch->primitive=Batch::QUADS;
		batch->setColor(Color4f(1,1,1));
		float _vertices[]       = {0,0,0,  W,0,0,  W,H,0, 0,H,0};batch->vertices.reset(new Array(12,_vertices));
		batch->texture0=texture0;
		batch->texture1=texture1;
		float _texture0coords[] ={0,0, 1,0, 1,1, 0,1};batch->texture0coords.reset(new Array(8,_texture0coords));
		float _texture1coords[] ={0,0, 1,0, 1,1, 0,1};batch->texture1coords.reset(new Array(8,_texture1coords));
		clearScreen();
		setViewport(0,0,W,H);
		setProjectionMatrix(Mat4f::ortho(0,W,0,H,-1,+1));
		setModelviewMatrix(Mat4f());
		renderBatch(batch);
		swapBuffers();
	}
};

int TextureSelfTest()
{
	Log::printf("Testing Texture...\n");
	
	SmartPointer<Texture> gioconda=Texture::open(":resources/img/gioconda.tga",false,false);

	//two textures
	if (true)
	{
		SmartPointer<Texture> back    =Texture::open(":resources/img/gioconda.tga",false,false);
		SmartPointer<Texture> texture1=Texture::open(":resources/img/gioconda.texture1.tga",false,false);
    TextureViewer v1(back,texture1);v1.runLoop();
	}

	//TGA open/save
	if (true)
	{
		bool ret=gioconda->save(":temp_gioconda.copy.tga");
		XgeReleaseAssert(gioconda->filename==":temp_gioconda.copy.tga");

		XgeReleaseAssert(ret);
		SmartPointer<Texture> back=Texture::open(":temp_gioconda.copy.tga",false,false);
		SmartPointer<Texture> texture1=Texture::open(":resources/img/gioconda.texture1.tga",false,false);
		XgeReleaseAssert(back && texture1);
		TextureViewer v(back,texture1);v.runLoop();
	}

	//test PNG load/save
	if (true)
	{
		bool ret=gioconda->save(":temp_gioconda.copy.png");
		XgeReleaseAssert(gioconda->filename==":temp_gioconda.copy.png");

		XgeReleaseAssert(ret);
		SmartPointer<Texture> back=Texture::open(":temp_gioconda.copy.png",false,false);
		SmartPointer<Texture> texture1=Texture::open(":resources/img/gioconda.texture1.png",false,false);
		XgeReleaseAssert(back && texture1);
		TextureViewer v(back,texture1);v.runLoop();
	}

	//test jpeg load/save
	if (true)
	{
		bool ret=gioconda->save(":temp_gioconda.copy.jpg");
		XgeReleaseAssert(gioconda->filename==":temp_gioconda.copy.jpg");

		XgeReleaseAssert(ret);
		SmartPointer<Texture> back=Texture::open(":temp_gioconda.copy.jpg",false,false);
		SmartPointer<Texture> texture1=Texture::open(":resources/img/gioconda.texture1.jpg",false,false);
		XgeReleaseAssert(back && texture1);
		TextureViewer v(back,texture1);v.runLoop();
	}

	Texture::flushCache();

	return 0;
}



///////////////////////////////////////////////
int UtilsSelfTest()
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






/////////////////////////////////////////////////////////////
int Vec3fSelfTest()
{
	Log::printf("Testing Vec3f...\n");

	Vec3f v;

	XgeReleaseAssert(v==Vec3f(0,0,0));

	float _vf[]={10,11,12};
	v=Vec3f(_vf);
	XgeReleaseAssert(v==Vec3f(10,11,12));

	double _vd[]={100,110,120};
	v=Vec3f(_vd);
	XgeReleaseAssert(v==Vec3f(100,110,120));


	XgeReleaseAssert(v*0.1f==Vec3f(10,11,12) && 0.1f*v==Vec3f(10,11,12));
	XgeReleaseAssert(v/0.1f==Vec3f(1000,1100,1200) );


	XgeReleaseAssert(Vec3f(1,2,3).Min(Vec3f(0,5,6))==Vec3f(0,2,3));
	XgeReleaseAssert(Vec3f(1,2,3).Max(Vec3f(0,5,6))==Vec3f(1,5,6));

	float epsilon=0.0001f;
	XgeReleaseAssert(Vec3f(1+epsilon,2-epsilon,3+epsilon).fuzzyEqual(Vec3f(1,2,3)));

	XgeReleaseAssert(Vec3f(1,2,3)+Vec3f(1,2,3)==Vec3f(2,4,6));
	XgeReleaseAssert(Vec3f(1,2,3)-Vec3f(1,2,3)==Vec3f(0,0,0));

	XgeReleaseAssert(Vec3f(1,2,3).module2()==14);

	XgeReleaseAssert(Vec3f(1,1,1).normalize().fuzzyEqual(Vec3f(0.5773f,0.5773f,0.5773f)));

	XgeReleaseAssert(Vec3f(1,0,0).cross(Vec3f(0,1,0))==Vec3f(0,0,1));
	
	return 0;
}



/////////////////////////////////////////////////////////////
int Vec4fSelfTest()
{
	Log::printf("Testing Vec4f...\n");

	Vec4f v;
	

	XgeReleaseAssert(v==Vec4f(0,0,0,0));

	v=Vec4f(1,2,3,4);
	XgeReleaseAssert(v==Vec4f(1,2,3,4));

	float _vf[]={10,11,12,13};
	v=Vec4f(_vf);
	XgeReleaseAssert(v==Vec4f(10,11,12,13));

	double _vd[]={100,110,120,130};
	v=Vec4f(_vd);
	XgeReleaseAssert(v==Vec4f(100,110,120,130));

	XgeReleaseAssert(Vec3f(v.x,v.y,v.z)==Vec3f(100,110,120));

	XgeReleaseAssert(v*0.1f==Vec4f(10,11,12,13) && 0.1f*v==Vec4f(10,11,12,13));
	XgeReleaseAssert(v/0.1f==Vec4f(1000,1100,1200,1300) );


	XgeReleaseAssert(Vec4f(1,2,3,4).Min(Vec4f(0,5,6,7))==Vec4f(0,2,3,4));
	XgeReleaseAssert(Vec4f(1,2,3,4).Max(Vec4f(0,5,6,7))==Vec4f(1,5,6,7));

	float epsilon=0.0001f;
	XgeReleaseAssert(Vec4f(1+epsilon,2-epsilon,3+epsilon,4-epsilon).fuzzyEqual(Vec4f(1,2,3,4)));

	XgeReleaseAssert(Vec4f(1,2,3,4)+Vec4f(1,2,3,4)==Vec4f(2,4,6,8));
	XgeReleaseAssert(Vec4f(1,2,3,4)-Vec4f(1,2,3,4)==Vec4f(0,0,0,0));

	XgeReleaseAssert(Vec4f(1,1,1,1).normalize().fuzzyEqual(Vec4f(0.5f,0.5f,0.5f,0.5f)));

	return 0;
}



/////////////////////////////////////////////////////////////
int VecfSelfTest()
{
	Log::printf("Testing Vecf...\n");

	Vecf v1(0);v1.mem[0]=0.0f;
	XgeReleaseAssert(v1.dim==0 && v1[0]==0 && v1==v1);

	Vecf v2(1);v2.mem[0]=0.0f;v2.mem[1]=1.0f;
	XgeReleaseAssert(v2.dim==1 && v2[0]==0.0f && v2[1]==1.0f && v2==v2);

	Vecf v3(0.0f,1.0f,2.0f);
	XgeReleaseAssert(v3.dim==2 && v3[0]==0.0f && v3[1]==1.0f && v3[2]==2.0f && v3==v3);

	Vecf v4(0.0f,1.0f,2.0f,3.0f);
	XgeReleaseAssert(v4.dim==3 && v4[0]==0.0f && v4[1]==1.0f && v4[2]==2.0f && v4[3]==3.0f && v4==v4);



	Vecf v=Vecf(1.0,2.0,3.0);
	{
		v*=0.1f;
		XgeReleaseAssert(v.fuzzyEqual(Vecf(0.1f,0.2f,0.3f)));
	}

	{
		v=Vecf(1.0,2.0,3.0);
		v/=0.1f;
		XgeReleaseAssert(v.fuzzyEqual(Vecf(10.0,20.0,30.0)));
	}

	//test + -
	{
		Vecf v1(1.0f,2.0f,3.0f);
		Vecf v2(0.1f,0.2f,0.3f);

		XgeReleaseAssert((v1+v2).fuzzyEqual(Vecf(1.1f,2.2f,3.3f)));
		XgeReleaseAssert((v1-v2).fuzzyEqual(Vecf(0.9f,1.8f,2.7f)));

		XgeReleaseAssert(fabs(v1*v2-(v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2]))<=0.001f);
	}

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
void GLCanvasSelfTest(int nx=100,int ny=100,int nz=1)
{
  Log::printf("Testing GLCanvas...\n");
  Box3f world_box(Vec3f(0,0,0),Vec3f(nx,ny,nz));

  SmartPointer<Batch> cube=Graph::cuboid(3)->getBatch();
  std::vector<SmartPointer<Batch> > batches;

  for (int x=0;x<nx;x++)
  for (int y=0;y<ny;y++)
  for (int z=0;z<nz;z++)
  {
    SmartPointer<Batch> batch(new Batch(*cube));
    batch->matrix=Mat4f::translate(x,y,z) * Mat4f::scale(0.8f,0.8f,0.8f);
    batches.push_back(batch);
  }
  SmartPointer<Octree> octree(new Octree(batches));
  GLCanvas v;
  v.setOctree(octree);
  v.runLoop();
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
int main(int nargs,char** argv)
{	
  XgeModule::init();
  {
	  UtilsSelfTest();
	  MemPoolSelfTest();
	  EncoderSelfTest();
	  DecoderSelfTest();
	  ArchiveSelfTest();
	  Vec3fSelfTest();
	  Vec4fSelfTest();
	  VecfSelfTest();
	  Plane4fSelfTest();
	  PlanefSelfTest();
	  Ray3fSelfTest();
	  RayfSelfTest();
	  Mat4fSelfTest();
	  MatfSelfTest();
	  Box3fSelfTest();
	  BoxfSelfTest();
	  Ball3fSelfTest();
	  BallfSelfTest();
	  QuaternionSelfTest();
	  ArraySelfTest();
	  GraphSelfTest();
    GLCanvasSelfTest();
    ManipulatorSelfTest();
    TextureSelfTest(); 
	  PlasmSelfTest();
	  PlasmBoolopSelftest();
  }
	XgeModule::shutdown();

	#if defined(PYPLASM_WINDOWS) && defined(_DEBUG)
	printf("\n\nPress a char to exit\n");
	getchar();
	#endif

	return 0;
}


