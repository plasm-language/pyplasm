#include <xge/xge.h>
#include <xge/viewer.h>
#include <xge/engine.h>


///////////////////////////////////////////////////////////////////////////////
Viewer::Viewer()
	:Thread(2)
{
	this->octree.reset();
	this->m_close=false;
	this->DrawLines=false;
	this->DrawAxis =true;
	this->engine=0;
	this->debug_frustum=0;
	this->mouse_beginx   = 0;
	this->mouse_beginy   = 0;
	this->trackball_mode   =true;

	this->trackball_center=Vec3f(0,0,0);
	this->frustum.guessBestPosition(Box3f(Vec3f(-1,-1,-1),Vec3f(+1,+1,+1)));
}



///////////////////////////////////////////////////////////////////////////////
Viewer::Viewer(SmartPointer<Octree> octree)
	:Thread(2)
{	
	this->m_close=false;
	this->DrawLines=false;
	this->DrawAxis =true;
	this->engine=0;
	this->debug_frustum=0;
	this->mouse_beginx   = 0;
	this->mouse_beginy   = 0;
	this->trackball_mode   =true;
	this->octree=octree;
	this->trackball_mode   =true;

	if (this->octree)
	{
		this->trackball_center =this->octree->world_box.center();
		this->frustum.guessBestPosition(this->octree->world_box);
	}
	else
	{
		this->trackball_center=Vec3f(0,0,0);
		this->frustum.guessBestPosition(Box3f(Vec3f(-1,-1,-1),Vec3f(+1,+1,+1)));
	}
}

///////////////////////////////////////////////////////////////////////////////
Viewer::~Viewer()
{
	if (this->debug_frustum)
		delete this->debug_frustum;
}


//////////////////////////////////////////////////////////////////////////////////////
void Viewer::Redisplay()
{
	this->frustum.refresh();
	m_redisplay  = true;
}



///////////////////////////////////////////////////////////////////////////////
void Viewer::Mouse(MouseEvent args)
{	
	if (args.type==MouseEvent::MousePressed)
	{
		mouse_beginx = args.x;
		mouse_beginy = args.y;
	}
	else if (args.type==MouseEvent::MouseReleased)
	{
		mouse_beginx = args.x;
		mouse_beginy = args.y;
	}
	else if (args.type==MouseEvent::MouseMoved)
	{
		if (args.button!=MouseEvent::NoButton)
		{
			if (trackball_mode)
			{
				if (frustum.defaultMouseTrackballMode(args.button,mouse_beginx,mouse_beginy,args.x,args.y,trackball_center))
					this->Redisplay();
			}
			else
			{
				if (frustum.defaultMouseWalkingMode(args.button,mouse_beginx,mouse_beginy,args.x,args.y))
					this->Redisplay();
			}
		}

		mouse_beginx = args.x;
		mouse_beginy = args.y;
	}
	else if (args.type==MouseEvent::MouseWheel)
	{
		frustum.pos+=(args.delta>0?+1:-1) * frustum.dir * frustum.walk_speed;
		this->Redisplay();
	}
}

///////////////////////////////////////////////////////////////////////////////
void Viewer::Resize(int width,int height)
{
	if (!width || ! height)
		return;

	frustum.x=0;
	frustum.y=0;
	frustum.width=width;
	frustum.height=height;

	float zNear=0.001f;
	float zFar=1000;
	if (this->octree)
	{
		float maxdim=octree->world_box.maxsize();;
		zNear = maxdim / 50.0f ;
		zFar  = maxdim * 10;
	}

	frustum.projection_matrix=Mat4f::perspective(DEFAULT_FOV,width/(float)height,zNear,zFar);
	this->Redisplay();
}



///////////////////////////////////////////////////////////////////////////////
void Viewer::Keyboard(int key,int x,int y) 
{
	switch(key)
	{
		case 27:
		{
			this->Close();
			break;
		}

		case '+':case '=':
			this->frustum.walk_speed*=0.95f;
			break;

		case '-':case '_':
			this->frustum.walk_speed*=(1.0f/0.95f);
			break;

		case 'X':
			this->DrawAxis=!this->DrawAxis;
			this->Redisplay();
			break;

		case 'l':case 'L':
			this->DrawLines=!this->DrawLines;
			this->Redisplay();
			break;

		case 'F':case 'f':
		{
			if (!debug_frustum)
				debug_frustum=new Frustum(this->frustum);
			else
			{
				delete debug_frustum;
				debug_frustum=0;
			}
			
			this->Redisplay();
			break;
		}
		case ' ':
		{
			trackball_mode=!trackball_mode;
			if (!trackball_mode) this->frustum.fixVup();
			this->Redisplay();
			break;
		}
		default:
		{
			if (this->frustum.defaultKeyboard(key,x,y))	
				this->Redisplay();
			break;
		}
	}


}


///////////////////////////////////////////////////////////////////////////////
void Viewer::Render()
{
	//clear the screen
	engine->ClearScreen();
	engine->SetViewport(frustum.x,frustum.y,frustum.width,frustum.height);
	engine->SetProjectionMatrix(frustum.projection_matrix);
	engine->SetModelviewMatrix(frustum.getModelviewMatrix());
	engine->SetDefaultLight(this->frustum.pos);

	//debug mode
	if (this->debug_frustum) 
	{
		this->debug_frustum->Render(engine);
	}

	//reference axix
	if (this->DrawAxis)
	{
		float X=1;
		float Y=1;
		float Z=1;
		SmartPointer<Batch> reference(new Batch);
		reference->primitive=Batch::LINES;
		static float _vertices[]={0,0,0, X,0,0,   0,0,0, 0,Y,0,  0,0,0, 0,0,Z};reference->vertices .reset(new Vector(6*3,_vertices));
		static float _colors  []={1,0,0, 1,0,0,   0,1,0, 0,1,0,  0,0,1, 0,0,1};reference->colors   .reset(new Vector(6*3,_colors  ));
		engine->SetLineWidth(3);
		engine->Render(reference);
		engine->SetLineWidth(1);
	}

	if (this->octree)
	{
		std::vector<SmartPointer<Batch> > transparent;
		FrustumIterator it_frustum=this->octree->find(this->debug_frustum?this->debug_frustum:&this->frustum);
		Clock t1;
		bool bQuitRenderingLoop=false;
		for (;!bQuitRenderingLoop && !it_frustum.end();it_frustum.moveNext())
		{
			OctreeNode* node=it_frustum.getNode();
			std::vector<SmartPointer<Batch> >& v=node->batches;

			for (int i=0;!bQuitRenderingLoop && i<(int)v.size();i++) 
			{
				if ((this->debug_frustum?this->debug_frustum:&this->frustum)->intersect(v[i]->getBox()))
				{
					if (v[i]->diffuse.a<1)
					{
						transparent.push_back(v[i]);
					}
					else
					{
						engine->Render(v[i]);

						if (this->DrawLines && v[i]->primitive>=Batch::TRIANGLES)
						{
							//engine->SetPolygonOffset(2.0f);
							engine->SetDepthWrite(false);
							engine->SetLineWidth(2);
							engine->SetPolygonMode(Batch::LINES);

							Color4f ambient=v[i]->ambient;
							Color4f diffuse=v[i]->diffuse;
							v[i]->setColor(Color4f(0,0,0,0.05f));
							engine->Render(v[i]);
							v[i]->ambient=ambient;
							v[i]->diffuse=diffuse;

							engine->SetDepthWrite(true);
							engine->SetPolygonMode(Batch::POLYGON);
							engine->SetLineWidth(2);
							//engine->SetPolygonOffset(0.0f);
						}

						//draw in chunks of fps=30
						if (t1.msec()>30) 
						{
							t1.reset();
							bQuitRenderingLoop=m_redisplay;
							engine->FlushScreen();
						}
					}
				}	
			}
		}

		//draw transparent object in reverse order
		for (int i=(transparent.size()-1);!bQuitRenderingLoop && i>=0 ;i--)
		{
			engine->Render(transparent[i]);

			//draw in chunks of fps=30
			if (t1.msec()>30) 
			{
				t1.reset();
				bQuitRenderingLoop=m_redisplay; 
				engine->FlushScreen();
			}
		}
	}
	engine->FlushScreen();
}




///////////////////////////////////////////////////////////////////////////////
void Viewer::SelfTest(int nx,int ny,int nz)
{
	Log::printf("Testing viewer...\n");
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
	Viewer v(octree);
	v.Run();
	v.Wait();
}


//////////////////////////////////////////////////////////////////////////////////////
void Viewer::Close()
{
	m_close=true;
}



//////////////////////////////////////////////////////////////////////////////////////
void Viewer::Run()
{
#ifdef Darwin
	//macosx does not support GUI in multiple thread
	workers[1].running=false;
	pthread_create( &workers[1].thread, NULL, startfun, (void*) &workers[1]);
	workers[1].running=true; //the GUI must be in the main thread
	
	workers[0].running=true;
	doJob(0);
	workers[0].running=false;
#else
	Thread::Run();
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
void Viewer::Wait()
{
	Thread::Wait();
}


