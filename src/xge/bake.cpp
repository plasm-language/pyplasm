
#include <xge/xge.h>
#include <xge/bake.h>

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
static bool ExecuteShellCmd(std::string cmd,bool bWait) 
{ 
#ifdef _WINDOWS
    PROCESS_INFORMATION piProcessInfo; 
    memset(&piProcessInfo, 0, sizeof(piProcessInfo)); 

    // CreateProcess API initialization
    STARTUPINFOW siStartupInfo; 
	memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	siStartupInfo.cb = sizeof(siStartupInfo); 

    if (!CreateProcessA(0, (LPSTR)cmd.c_str(), 0, 0, false, CREATE_DEFAULT_ERROR_MODE, 0, 0, (LPSTARTUPINFOA)&siStartupInfo, &piProcessInfo) != false) 
		return false; 

	if (bWait)
		WaitForSingleObject(piProcessInfo.hProcess,INFINITE); 

    CloseHandle(piProcessInfo.hProcess); 
    CloseHandle(piProcessInfo.hThread); 

    return true; 
#else
	return false;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
Bake::Bake()
{
	this->DebugMode  =false;
	this->PointOcclusion=false;	
}


/////////////////////////////////////////////////////////////////////////
std::vector< SmartPointer<Batch> > Bake::Unwrap(std::vector< SmartPointer<Batch> > batches,float factor,std::string texture_template,int texturedim)
{
	std::vector<SmartPointer<Batch> > ret;

	Unwrapper unwrapper;
	unwrapper.texture_template=texture_template;
	unwrapper.factor=factor;
	unwrapper.texturedim=texturedim;

	for (int i=0;i<(int)batches.size();i++)
	{
		printf("Unwrapping %d of %d (num textures %d)\n",i,(int)batches.size(),(int)unwrapper.getNewLightTextures().size());
		std::vector<SmartPointer<Batch> > to_append=unwrapper.Unwrap(Mat4f(),batches[i]);

		for (int j=0;j<(int)to_append.size();j++)
			ret.push_back(to_append[j]);
	}

	return ret;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
void Bake::Add(Mat4f T,SmartPointer<Batch> batch)
{
	//cannot export because not all infos are in the batch
	if (batch->primitive!=Batch::TRIANGLES || !batch->vertices || !batch->normals || !batch->texture1coords || !batch->texture1)
		return;

	//necessary condition!
	ReleaseAssert(batch->texture1->width==batch->texture1->height);

	//necessary condition for the ribbake to work
	ReleaseAssert(batch->texture1->filename.find(".tif")!=std::string::npos);

	//keep track of all texture1 textures inserted
	if (texture1_map.find(batch->texture1.get())==texture1_map.end())
		texture1_map[batch->texture1.get()]=std::vector<SmartPointer<Batch> >();

	//store internally a copy of batches and accumulate the matrices
	batch.reset(new Batch(*batch));
	Mat4f T1=T;
	Mat4f T2=batch->matrix;

	//accumulate matricces
	batch->matrix=T1*T2;

	//bounding box
	scene_box.add(batch->getBox());

	//add to texture1 map
	texture1_map[batch->texture1.get()].push_back(batch);
}




/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
bool Bake::Export()
{
	
	std::string rib_filename      =":main.rib";
	std::string rib_world_filename=":main.world.rib";
	std::string frame_template    =":main.%03d.rib";
	
	//_________________________________________________________________________ export the world
	{
		FILE* rib_world_file = fopen(FileSystem::FullPath(rib_world_filename).c_str(),"wt");
		if (!rib_world_file)
		{
			Log::printf("cannot save RIB world file %s\n",rib_world_filename.c_str());
			return false;
		}
		fprintf(rib_world_file,"ObjectBegin \"myworld\"\n");
		for (std::map<Texture*,std::vector<SmartPointer<Batch> > >::iterator it=texture1_map.begin();it!=texture1_map.end();it++)
		{
			ReleaseAssert(it->first->width==it->first->height);
			int texturedim=it->first->width;

			for (int I=0;I<(int)it->second.size();I++)
			{
				SmartPointer<Batch> batch=it->second[I];
				Mat4f matrix=batch->matrix;
				Mat4f inv   =matrix.invert();
				int nv=batch->vertices->size()/3;
				int nt=nv/3;
				float* vertex     = batch->vertices->mem();
				for (int i=0;i<nt;i++,vertex+=9)
				{
					Vec3f v0(vertex[0],vertex[1],vertex[2]);v0=matrix * v0; 
					Vec3f v1(vertex[3],vertex[4],vertex[5]);v1=matrix * v1; 
					Vec3f v2(vertex[6],vertex[7],vertex[8]);v2=matrix * v2; 
					fprintf(rib_world_file,"Polygon \"P\" [%+.8f %+.8f %+.8f %+.8f %+.8f %+.8f %+.8f %+.8f %+.8f]\n",v0.x,v0.y,v0.z, v1.x,v1.y,v1.z, v2.x,v2.y,v2.z);
				}
			}
		}
		fprintf(rib_world_file,"ObjectEnd\n");
		fclose(rib_world_file);
	}

	//_________________________________________________________________________ export the frames
	{
		int Frame=0;
		for (std::map<Texture* , std::vector<SmartPointer<Batch> > >::iterator it=texture1_map.begin();it!=texture1_map.end();it++,Frame++)
		{
			std::string frame_filename=FileSystem::FullPath(Utils::Format(frame_template.c_str(),Frame));

			FILE* frame_file = fopen(frame_filename.c_str(),"wt");
			if (!frame_file)
			{
				Log::printf("cannot save frame file %s\n",frame_filename.c_str());
				return false;
			}

			ReleaseAssert(it->first->width==it->first->height);
			int texturedim=it->first->width; 

			for (int I=0;I<(int)it->second.size();I++)
			{
				SmartPointer<Batch> batch=it->second[I];
				Mat4f matrix=batch->matrix ;
				Mat4f inv=matrix.invert();
				int nv=batch->vertices->size()/3;int nt=nv/3;
				float* vertex     = batch->vertices->mem();
				float* normal     = batch->normals ->mem();
				float* lightcoord = batch->texture1coords->mem();

				for (int i=0;i<nt;i++,vertex+=9,normal+=9)
				{
					Vec3f v0(vertex[0],vertex[1],vertex[2]) ; v0=matrix * v0; Vec4f _n0(normal[0],normal[1],normal[2],0.0);  _n0=_n0 * inv;  Vec3f n0=Vec3f(_n0.x,_n0.y,_n0.z).normalize();
					Vec3f v1(vertex[3],vertex[4],vertex[5]) ; v1=matrix * v1; Vec4f _n1(normal[3],normal[4],normal[5],0.0);  _n1=_n1 * inv;  Vec3f n1=Vec3f(_n1.x,_n1.y,_n1.z).normalize();
					Vec3f v2(vertex[6],vertex[7],vertex[8]) ; v2=matrix * v2; Vec4f _n2(normal[6],normal[7],normal[8],0.0);  _n2=_n2 * inv;  Vec3f n2=Vec3f(_n2.x,_n2.y,_n2.z).normalize();
					float s0=lightcoord[i*6+0],t0=lightcoord[i*6+1];
					float s1=lightcoord[i*6+2],t1=lightcoord[i*6+3];
					float s2=lightcoord[i*6+4],t2=lightcoord[i*6+5];

					// force the regeneration of float values (seems perfect  for opengl/ renderman scan line conversion!)
					s0=(0.5f/(float)texturedim)+(((int)(s0*(float)texturedim))/(float)texturedim);t0=(0.5f/(float)texturedim)+(((int)(t0*(float)texturedim))/(float)texturedim);
					s1=(0.5f/(float)texturedim)+(((int)(s1*(float)texturedim))/(float)texturedim);t1=(0.5f/(float)texturedim)+(((int)(t1*(float)texturedim))/(float)texturedim);
					s2=(0.5f/(float)texturedim)+(((int)(s2*(float)texturedim))/(float)texturedim);t2=(0.5f/(float)texturedim)+(((int)(t2*(float)texturedim))/(float)texturedim);
					
					fprintf(frame_file,"Polygon \"V\" [%+.8f %+.8f %+.8f %+.8f %+.8f %+.8f %+.8f %+.8f %+.8f] \"N\" [%+.8f %+.8f %+.8f %+.8f %+.8f %+.8f %+.8f %+.8f %+.8f] \"P\" [%+.8f %+.8f %+.8f %+.8f %+.8f %+.8f %+.8f %+.8f %+.8f] ",
						v0.x,v0.y,v0.z, v1.x,v1.y,v1.z, v2.x,v2.y,v2.z,
						n0.x,n0.y,n0.z, n1.x,n1.y,n1.z, n2.x,n2.y,n2.z,
						s0,t0,0.0,s1,t1,0.0,s2,t2,0.0);

					if (DebugMode)
					{
						//float R=frand(0,1),G=frand(0,1),B=frand(0,1);
						float R=1,G=1,B=1;
						fprintf(frame_file," \"W\" [%+.8f %+.8f %+.8f %+.8f %+.8f %+.8f %+.8f %+.8f %+.8f]",R,G,B,R,G,B,R,G,B);
					}
					
					fprintf(frame_file,"\n");
				}
			}
			fclose(frame_file);
		}
	}

	//_________________________________________________________________________ export the main RIB
	{
		FILE* rib_file = fopen(FileSystem::FullPath(rib_filename).c_str(),"wt");

		if (!rib_file)
		{
			Log::printf("cannot save RIB file %s\n",rib_filename.c_str());
			return false;
		}

		ReleaseAssert(rib_file);
		fprintf(rib_file,"version 3.03\n");
		fprintf(rib_file,Utils::Format("Option \"searchpath\" \"shader\" [\"shaders:%s:&:$RIBDIR\"]\n",FileSystem::FullPath(":shaders/rib").c_str()).c_str()); 

		int texturedim=texture1_map.begin()->first->width;
		fprintf(rib_file,Utils::Format("Format %d %d 1\n",texturedim,texturedim).c_str());
		fprintf(rib_file,"Attribute \"cull\" \"hidden\" [0] \"backfacing\" [0]\n");
		fprintf(rib_file,"Attribute \"dice\" \"rasterorient\" [0]\n");
		fprintf(rib_file,"ReadArchive \"%s\"\n",FileSystem::FullPath(rib_world_filename).c_str());

		//write points to the file
		if (PointOcclusion)
		{
			fprintf(rib_file,"AttributeBegin\n");

			//this is the matrix to see all the world from the rib
			Frustum frustum;
			frustum.guessBestPosition(scene_box);

			Mat4f world_fit_matrix=
				Mat4f::scale(1,1,-1)
				* Mat4f::lookat(
					frustum.pos.x              ,frustum.pos.y              ,frustum.pos.z,
					frustum.pos.x+frustum.dir.x,frustum.pos.y+frustum.dir.y,frustum.pos.z+frustum.dir.z,
					frustum.vup.x,frustum.vup.y,frustum.vup.z);

			fprintf(rib_file,Utils::Format("   Projection  \"perspective\"  \"fov\" [ %f ]\n",DEFAULT_FOV).c_str());
			fprintf(rib_file,Utils::Format("   ConcatTransform [ %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f ]\n",
				world_fit_matrix.a11(),world_fit_matrix.a21(),world_fit_matrix.a31(),world_fit_matrix.a41(),
				world_fit_matrix.a12(),world_fit_matrix.a22(),world_fit_matrix.a32(),world_fit_matrix.a42(),
				world_fit_matrix.a13(),world_fit_matrix.a23(),world_fit_matrix.a33(),world_fit_matrix.a43(),
				world_fit_matrix.a14(),world_fit_matrix.a24(),world_fit_matrix.a34(),world_fit_matrix.a44()).c_str());

			fprintf(rib_file,"   WorldBegin\n");
			fprintf(rib_file,"      Surface \"point_occlusion_write_shader\"\n");
			fprintf(rib_file,"      ObjectInstance \"myworld\"\n");
			fprintf(rib_file,"   WorldEnd\n");
			fprintf(rib_file,"AttributeEnd\n");
		}

		int Frame=0;
		for (std::map<Texture* , std::vector<SmartPointer<Batch> > >::iterator it=texture1_map.begin();it!=texture1_map.end();it++,Frame++)
		{
			ReleaseAssert(it->first->width==it->first->height);
			int texturedim=it->first->width; 
			
			fprintf(rib_file,"FrameBegin %d\n",Frame);
			fprintf(rib_file,"   Format %d %d 1\n",texturedim,texturedim);
			fprintf(rib_file,"   Hider \"raytrace\" \"int jitter\" [0]\n");
			fprintf(rib_file,"   PixelSamples 1 1\n");
			fprintf(rib_file,"   ShadingInterpolation \"constant\"\n");
			fprintf(rib_file,"   ShadingRate 1.0\n");
			fprintf(rib_file,"   PixelFilter \"box\" 1 1\n");  
			//fprintf(rib_file,"Hider \"hidden\" \"string depthfilter\" [\"midpoint\"]\n");  


			//Important for 3delight I must use TIFF file format!
			std::string rib_tif_filename=FileSystem::FullPath(it->first->filename);

			fprintf(rib_file,"   Display \"%s\"  \"file\" \"rgba\"\n"        ,rib_tif_filename.c_str()); 
			fprintf(rib_file,"   Display \"+%s\" \"framebuffer\" \"rgba\"\n" ,rib_tif_filename.c_str());
			fprintf(rib_file,"   Projection \"orthographic\"\n");
			fprintf(rib_file,"   Scale 2 2 2\n");

			//IMPORTANTE: se non metto questa trasformazione ho grossi problemi, un po' ho copiato da CGKIT
			//i problemi sono che vedo delle zone NERE dove non ci dovrebbero essere (depth fighting?)
			fprintf(rib_file,Utils::Format("   Translate -0.5 -0.5 %.8f\n",-scene_box.p2.z-1.0f).c_str());
			fprintf(rib_file,"   WorldBegin\n");

			fprintf(rib_file,"      AttributeBegin\n");

			fprintf(rib_file,"      Color [1 1 1]\n");
			fprintf(rib_file,"      Opacity [1 1 1]\n");

			std::string shader_name;
			if (DebugMode)
				shader_name="debug_shader";
			else if (!PointOcclusion)
				shader_name="occlusion_shader";
			else
				shader_name="point_occlusion_shader";

			fprintf(rib_file,"      Surface \"%s\" \"varying point V\" [0.0 0.0 0.0] ",shader_name.c_str());
			if (DebugMode) fprintf(rib_file,"\"varying color W\" [0.0 0.0 0.0]");
			fprintf(rib_file,"\n");

			//IMPORTANTE: se non metto questa trasformazione ho grossi problemi, un po' ho copiato da CGKIT
			//i problemi sono che vedo delle zone NERE dove non ci dovrebbero essere (depth fighting?)
			fprintf(rib_file,Utils::Format("      Translate 0 0 %.8f\n",scene_box.p2.z+1.5f).c_str());
			fprintf(rib_file,"      Declare \"V\" \"vertex point\"\n");
			fprintf(rib_file,"      Declare \"N\" \"varying normal\"\n");

			if (DebugMode) 
				fprintf(rib_file,"      Declare \"W\" \"varying color\"\n");

			std::string frame_filename=FileSystem::FullPath(Utils::Format(frame_template.c_str(),Frame)).c_str();
			fprintf(rib_file,"      ReadArchive \"%s\"\n",frame_filename.c_str());
			fprintf(rib_file,"      AttributeEnd\n");

			if (!PointOcclusion)
			{
				fprintf(rib_file,"      AttributeBegin\n");
				fprintf(rib_file,"         Attribute \"visibility\" \"string transmission\" [\"opaque\"]\n");
				fprintf(rib_file,"         Attribute \"visibility\" \"int trace\" [1]\n");
				fprintf(rib_file,"         Color   [1 1 1]\n");
				fprintf(rib_file,"         Opacity [0 0 0]\n"); // do not draw this one, seems to work with the shader occlusion!
				fprintf(rib_file,"         ObjectInstance \"myworld\"\n");
				fprintf(rib_file,"      AttributeEnd\n");
			}

			fprintf(rib_file,"   WorldEnd\n");
			fprintf(rib_file,"FrameEnd\n");
			
		}

		fclose(rib_file);
	}

	return true;
}


////////////////////////////////////////////////////////////////////
bool Bake::Run()
{
	std::string cmd;
	bool bOk;

	std::string DelightDir=getenv("DELIGHT");

	if (DelightDir=="")
		Utils::Error(HERE,"3dDelight seems not to be installed, the %%DELIGHT%% setting environment is not set");

	//try to start the licserver, please ignore error message if the server is already running in the background
	cmd=Utils::Format("\"%s\\bin\\licserver.exe\" -d \"%s\\license.dat\"",DelightDir.c_str(),DelightDir.c_str());
	cmd=Utils::Replace(cmd,"/","\\");
	Log::printf("Executing %s\n",cmd.c_str());
	bOk=ExecuteShellCmd(cmd.c_str(),false);


	cmd=Utils::Format("\"%s\\bin\\renderdl.exe\" \"%s\"",DelightDir.c_str(),FileSystem::FullPath(":main.rib").c_str());
	cmd=Utils::Replace(cmd,"/","\\");
	Log::printf("Executing %s\n",cmd.c_str());
	bOk=ExecuteShellCmd(cmd.c_str(),true);
	if (!bOk)
	{
		Log::printf("Failed to execute command");
		return false;
	}

	return true;
}


/////////////////////////////////////////////////////////////////////
bool Bake::PostProcess()
{
	for (std::map<Texture*, std::vector<SmartPointer<Batch> > >::iterator it=texture1_map.begin();it!=texture1_map.end();it++)
	{
		Texture* texture1=it->first;

		//load the texture from disk
		SmartPointer<Texture> rib_texture=Texture::open(texture1->filename,false,false); //not use cached, neither use cache

		//need a post processing
		ReleaseAssert(rib_texture->bpp==32 && rib_texture->width==rib_texture->height);
		int texturedim=rib_texture->width;
	    
		//rgb or gray (internally cause a regeneration!)
		texture1->change(texturedim,texturedim,DebugMode?24:8);

		for (int y=0;y<texturedim;y++)
		for (int x=0;x<texturedim;x++)
		{
			//the alpha channel should be 0 or 1
			ReleaseAssert(rib_texture->getPixel(x,y)[3]==0 || rib_texture->getPixel(x,y)[3]==255);
			float R=rib_texture->getPixel(x,y)[0],G=rib_texture->getPixel(x,y)[1],B=rib_texture->getPixel(x,y)[2];;

			if (DebugMode)
				{texture1->getPixel(x,y)[0]=R;texture1->getPixel(x,y)[1]=G;texture1->getPixel(x,y)[2]=B;} //simply copy the image
			else
				{texture1->getPixel(x,y)[0]= (unsigned char)((R+G+B)/3.0);} //average or RGB channel

			//full pixel, do nothing
			if (rib_texture->getPixel(x,y)[3]) continue;

			//should receive a black color
			ReleaseAssert(!R && !G && !B);
	        
			int nmean=0;
			R=0.0f,G=0.0f,B=0.0f;
			const int delta[4][2]={-1,0,+1,0,0,-1,0,+1};
			for (int d=0;d<4;d++)
			{
				int X=x+delta[d][0],Y=y+delta[d][1];
				//outside the range
				if (X<0 || X>=texturedim || Y<0 || Y>=texturedim) continue;
	            
				//full pixel since the alpha channel is non zero
				if (rib_texture->getPixel(X,Y)[3]) {R+=rib_texture->getPixel(X,Y)[0]; G+=rib_texture->getPixel(X,Y)[1]; B+=rib_texture->getPixel(X,Y)[2];nmean++;} 
			}
	            
			// the mean of near pixels is valid and can be used!
			if (nmean>0)
			{
				R/=nmean;G/=nmean;B/=nmean;
				if (DebugMode)
					{texture1->getPixel(x,y)[0]=R;texture1->getPixel(x,y)[1]=G;texture1->getPixel(x,y)[2]=B;}
				else
					{texture1->getPixel(x,y)[0]= (unsigned char)((R+G+B)/3.0);}
			}
		}

		//force a regeneration
		texture1->gpu.reset();

		//need to save for later load
		texture1->save(); 
	}

	return true;

}

/////////////////////////////////////////////////////////
int Bake::SelfTest()
{
	Log::printf("Testing Bake::..\n");

	Bake bake;
	bake.DebugMode=false;
	bake.PointOcclusion=false;
	std::vector< SmartPointer<Batch> > batches=bake.Unwrap
	(
		Plasm::getBatches(Plasm::open(":models/temple.hpc.xml")) ,
		5.0f,
		":models/temple.%02d.tif",
		1024
	);
	Batch::Save(":models/temple.ao.mesh.gz",batches);
	bake.Add(batches);
	bake.Export();
	bake.Run();
	bake.PostProcess();
	SmartPointer<Octree> octree(new Octree(batches));
	Viewer viewer(octree);
	viewer.Run();
	viewer.Wait();
	return 0;
}