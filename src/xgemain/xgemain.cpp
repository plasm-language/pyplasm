#include <xge/xge.h>




/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
int main(int nargs,char** argv)
{	

	//remove me
	/*if (false)
	{
		int pointdim=3;
		int npoints=12;
		const float points[]=
		{
			0.5,0.5,0, 0.5,0.5,1,

			0,0,0,  1,0,0,  2,1,0,  1,2,0,  0,1,0,
			0,0,1,  1,0,1,  2,1,1,  1,2,1,  0,1,1
		};
		Graph::qhull(pointdim,npoints,points,1e-6f,true);
		return 0;
	}*/

	//Batch::openObj("sphere12x18.obj");
	//	if (false)
	//	{
	//		std::vector<SmartPointer<Batch> > batches=Plasm::getBatches(Plasm::open(":models/pisa.hpc.gz"));
	//		SmartPointer<Octree> octree(new Octree(batches));
	//		
	//		for (int i=0;i<(int)batches.size();i++)
	//		{
	//			batches[i]->ambient=Batch::DefaultAmbientColor;
	//			batches[i]->diffuse=Batch::DefaultDiffuseColor;
	//			batches[i]->specular=Batch::DefaultSpecularColor;
	//		}
	//
	//		Viewer viewer(octree);
	//		viewer.Run();
	//		viewer.Wait();
	//		return 0;
	//	}
	//
	//	if (false)
	//	{
	//		#if 0
	//		{
	//			chdir(FileSystem::FullPath(":/models/theorematica").c_str());
	//
	//			Plasm::convertOldXml("sotterraneo_piano.mesh","sotterraneo_piano.mesh.gz",":models/theorematica/");
	//			Plasm::convertOldXml("piano_terra.mesh","piano_terra.mesh.gz",":models/theorematica/");
	//			Plasm::convertOldXml("primo_piano.mesh","primo_piano.mesh.gz",":models/theorematica/");
	//			Plasm::convertOldXml("secondo_piano.mesh","secondo_piano.mesh.gz",":models/theorematica/");
	//			Plasm::convertOldXml("terzo_piano.mesh","terzo_piano.mesh.gz",":models/theorematica/");
	//			Plasm::convertOldXml("quarto_piano.mesh","quarto_piano.mesh.gz",":models/theorematica/");
	//			Plasm::convertOldXml("copertura_piano.mesh","copertura_piano.mesh.gz",":models/theorematica/");
	//
	//			std::vector<SmartPointer<Hpc> > args;
	//			args.push_back(Plasm::open(":models/theorematica/sotterraneo_piano.mesh.gz"));
	//			args.push_back(Plasm::open(":models/theorematica/piano_terra.mesh.gz"));
	//			args.push_back(Plasm::open(":models/theorematica/primo_piano.mesh.gz"));
	//			args.push_back(Plasm::open(":models/theorematica/secondo_piano.mesh.gz"));
	//			args.push_back(Plasm::open(":models/theorematica/terzo_piano.mesh.gz"));
	//			args.push_back(Plasm::open(":models/theorematica/quarto_piano.mesh.gz"));
	//			args.push_back(Plasm::open(":models/theorematica/copertura_piano.mesh.gz"));
	//
	//			SmartPointer<Hpc> out=Plasm::Struct(args);
	//
	//			std::vector<SmartPointer<Batch> > batches=Plasm::getBatches(out);
	//
	//			for (int i=0;i<(int)batches.size();i++)
	//			{
	//				batches[i]->matrix=Mat4f::scale(1,1,1) * Mat4f::rotatex(+M_PI/2) * batches[i]->matrix;
	//				batches[i]->invalidateBox();
	//			}
	//
	//			Batch::Save(":models/theorematica/theorematica.mesh.gz",batches);
	//		}
	//
	//		#else
	//		std::vector<SmartPointer<Batch> > batches=Batch::Open(":models/theorematica/theorematica.mesh.gz");
	//		#endif
	//		
	//		SmartPointer<Octree> octree(new Octree(batches));
	//		Viewer v(octree);
	//		v.Run();
	//		v.Wait();
	//		return 0;
	//	}
	//
	//	if (false)
	//	{
	//		std::string filename=":models/palatino/palatino.mesh.gz";
	//
	//		char* mesh_filenames[]=
	//		{
	//			":models/palatino/palatinoalaoccidentalis.mesh.gz",
	//			":models/palatino/palatinoatrium.mesh.gz",
	//			":models/palatino/palatinoaulae.mesh.gz",
	//			":models/palatino/palatinoaularegia.mesh.gz",
	//			":models/palatino/palatinobasilica.mesh.gz",
	//			":models/palatino/palatinobibliotheca.mesh.gz",
	//			":models/palatino/palatinoculinae.mesh.gz",
	//			":models/palatino/palatinodiaetaporticata.mesh.gz",
	//			":models/palatino/palatinodomusdwfloor.mesh.gz",
	//			":models/palatino/palatinodomusupfloor.mesh.gz",
	//			":models/palatino/palatinoinferiorperistylium.mesh.gz",
	//			":models/palatino/palatinolararium.mesh.gz",
	//			":models/palatino/palatinonymphaeum1.mesh.gz",
	//			":models/palatino/palatinonymphaeum2.mesh.gz",
	//			":models/palatino/palatinoperistylium.mesh.gz",
	//			":models/palatino/palatinoperistyliumdomus.mesh.gz",
	//			":models/palatino/palatinoporticus.mesh.gz",
	//			":models/palatino/palatinostadium.mesh.gz",
	//			":models/palatino/palatinotriclinium.mesh.gz",
	//			0
	//		};
	//
	//		#if 0
	//		std::vector<SmartPointer<Batch> > batches;
	//		for (int i=0;mesh_filenames[i];i++)
	//		{
	//			SmartPointer<Hpc> hpc=Plasm::open(mesh_filenames[i]);
	//			std::vector< SmartPointer<Batch> >temp=Plasm::getBatches(hpc); 
	//			for (int j=0;j<(int)temp.size();j++) batches.push_back(temp[j]);
	//		}
	//		//to have in meters
	//		for (int i=0;i<(int)batches.size();i++)
	//		{
	//			batches[i]->matrix=/* Mat4f::scale(0.2,0.2,0.2) * */batches[i]->matrix;
	//			batches[i]->invalidateBox();
	//		}
	//		Batch::Save(filename,batches);
	//		#else	
	//		std::vector<SmartPointer<Batch> > batches=Batch::Open(filename);
	//		#endif
	//
	//		SmartPointer<Octree> octree(new Octree(batches));
	//		Viewer viewer(octree);
	//		viewer.Run();
	//		viewer.Wait();
	//		return 0;
	//	}
	//


	//{


	//	{
	//		int pointdim=3;
	//		int nplanes=18;

	//		float planes[]=
	//		{
	//			+1,+1,      0,      0,  //x>=-1
	//			+1,-1,      0,      0,  //x<=+1
	//			+1, 0,     +1,      0,  //y>=-1
	//			+1, 0,     -1,      0,  //y<=+1
	//			+1, 0,      0,     +1,  //z>=-1
	//			+1, 0,      0,     -1,  //z<=+1


	//			-0.5,-1,      0,      0,  //x<=-0.5
	//			-0.5, 1,      0,      0,  //x>=+0.5
	//			-0.5, 0,     -1,      0,  //y<=-0.5
	//			-0.5, 0,      1,      0,  //y>=+0.5
	//			-0.5, 0,      0,     -1,  //z<=-0.5
	//			-0.5, 0,      0,      1,   //z>=+0.5

	//			-1,-1,      0,      0,  //x<=-1
	//			-1,+1,      0,      0,  //x>=+1
	//			-1, 0,     -1,      0,  //y<=-1
	//			-1, 0,     +1,      0,  //y>=+1
	//			-1, 0,      0,     -1,  //z<=-1
	//			-1, 0,      0,     +1,  //z>=+1

	//		};

	//		float interior_point[]={0,0,0};
	//		Matf Vmat,Hmat;
	//		float tolerance=1e-6f;
	//		SmartPointer<Graph> g=Graph::mkpolf(Vmat,Hmat,pointdim,nplanes,planes,interior_point,tolerance);

	//		g->Print();
	//	}

	//	return 0;
	//}

	//self testing
	Engine::PrintStatistics();

	if (true)
	{
		Utils::SelfTest();
		MemPool::SelfTest();
		Encoder::SelfTest();
		Decoder::SelfTest();
		Archive::SelfTest();
		Vec3f::SelfTest();
		Vec4f::SelfTest();
		Vecf::SelfTest();
		Plane4f::SelfTest();
		Planef::SelfTest();
		
		Ray3f::SelfTest();
		Rayf::SelfTest();
		Mat4f::SelfTest();
		Matf::SelfTest();
		Box3f::SelfTest();
		Boxf::SelfTest();
		Ball3f::SelfTest();
		Ballf::SelfTest();
		Quaternion::SelfTest();
		Vector::SelfTest();
		Graph::SelfTest();
		Plasm::SelfTest();
		Plasm::boolop_selftest();
		Texture::SelfTest(); 
		
		#if ENGINE_ENABLE_FBO
		EngineFbo::SelfTest();
		#endif

		Manipulator::SelfTest();
		Viewer::SelfTest();
		
		//{Viewer v(Batch::openPly(":models/dinosaur.ply",true));v.Run();v.Wait()};
		//{Viewer v(Batch::openObj(":models/lucy.obj"         ));v.Run();v.Wait()};
		//{Viewer v(Batch::openPtx(":models/pointcloud.ptx"   ));v.Run();v.Wait()};

		#if ENGINE_ENABLE_SHADERS
		EngineShader::SelfTest();
		#endif
		
		#if ENGINE_ENABLE_SHADOW_SHADER
		EngineShadowShader::SelfTest();
		#endif

		#if 0
		Bake::SelfTest();
		#endif

		//test two background viewer sharing VBO
		if (true)
		{
			std::vector<SmartPointer<Batch> >  batches=Plasm::getBatches(Plasm::open(":models/pisa.hpc.gz"));
			SmartPointer<Octree> octree(new Octree(batches));
			Viewer v1(octree);
			Viewer v2(octree);
			v1.Run();
			v2.Run();
			v1.Wait();
			v2.Wait();
		}
	}
	Engine::PrintStatistics();

	#if defined(_WINDOWS) && defined(_DEBUG)
	printf("\n\nPress a char to exit\n");
	getchar();
	#endif

	return 0;
}


