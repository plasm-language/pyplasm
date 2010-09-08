#include <xge/xge.h>
#include <xge/engine.h>
#include <GL/glew.h>

#if ENGINE_ENABLE_FBO


/////////////////////////////////////////////////////////////////////////////////////
EngineFbo::EngineFbo(SmartPointer<Texture> tex,bool bEnableDepthText)
{
	//do not instantiate inside a context
	int64 RC=(int64)wglGetCurrentContext();
	ReleaseAssert(!RC);

	//supported only these modality (or luminance or RGB)
	DebugAssert(tex->bpp==8 || tex->bpp==24);

	this->bDepthText = bDepthText;
	this->tex        = tex;

	//create a virtual window
	HINSTANCE hInstance = GetModuleHandle(NULL);
	WNDCLASS  wc;
	wc.style         = 0;                           
	wc.lpfnWndProc   = (WNDPROC)DefWindowProc;         
	wc.cbClsExtra    = 0;                           
	wc.cbWndExtra    = 0;                           
	wc.hInstance     = hInstance;                  
	wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO); 
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW); 
	wc.hbrBackground = NULL;                        
	wc.lpszMenuName  = NULL;                        
	wc.lpszClassName = L"EngineFbo";              
	BOOL ok=RegisterClass(&wc);

	//can be already registered
	//ReleaseAssert(ok);
	
	HWND hWnd=CreateWindow(L"EngineFbo",L"EngineFbo",
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,NULL,hInstance,NULL);		

	ReleaseAssert(hWnd);

	this->Window=(int64)hWnd;
	
	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);
	HDC hDC=GetDC(hWnd);

	//create the engine
	this->engine=new Engine((int64)hDC);

	if (this->engine->Bind())
	{
		#define glewGetContext() ((GLEWContext*)engine->WC)
		this->framebuffer=createFrameBuffer();
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer->id);
		this->renderbuffer=createRenderBuffer(GL_COLOR_ATTACHMENT0_EXT,(tex->bpp==8)?GL_LUMINANCE8:GL_RGBA8, tex->width, tex->height);
		this->depthBuffer =createRenderBuffer(GL_DEPTH_ATTACHMENT_EXT ,GL_DEPTH_COMPONENT24                 ,tex->width, tex->height);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		#undef glewGetContext
		this->engine->Unbind();
	}
	
}


/////////////////////////////////////////////////////////////////////////////////////
EngineFbo::~EngineFbo()
{
	HDC DC=(HDC)this->engine->GetDC();
	delete this->engine;
	ReleaseDC((HWND)this->Window,DC);

	DestroyWindow((HWND)this->Window);
	this->Window=0;
}


/////////////////////////////////////////////////////////////////////////////////////
SmartPointer<EngineResource> EngineFbo::createFrameBuffer()
{
	//I have the lock here (see EngineFbo contructor)
	#define glewGetContext() ((GLEWContext*)this->engine->WC)
	unsigned int _frame_buffer_id;
	glGenFramebuffersEXT(1, &_frame_buffer_id);
	ReleaseAssert(_frame_buffer_id);
	return SmartPointer<EngineResource>(new EngineResource(EngineResource::RESOURCE_FRAME_BUFFER,_frame_buffer_id,0));
	#undef glewGetContext
}


/////////////////////////////////////////////////////////////////////////////////////
SmartPointer<EngineResource> EngineFbo::createRenderBuffer(int target,int storage_type,int width,int height)
{
	//I have the lock here (see EngineFbo contructor)
	#define glewGetContext() ((GLEWContext*)this->engine->WC)
	unsigned int render_buffer_id;
	glGenRenderbuffersEXT(1, &render_buffer_id); 
	ReleaseAssert(render_buffer_id);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, render_buffer_id); 
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, storage_type,width,height ); 
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, target, GL_RENDERBUFFER_EXT, render_buffer_id); 
	 //assume size for RGB texture, even if can be not this way
	return SmartPointer<EngineResource>(new EngineResource(EngineResource::RESOURCE_RENDER_BUFFER,render_buffer_id,width*height*3));
	#undef glewGetContext
}

/////////////////////////////////////////////////////////////////////////////////////
void EngineFbo::Begin()
{
	#define glewGetContext() ((GLEWContext*)this->engine->WC)
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer->id);
	glViewport(0,0,tex->width,tex->height);
	if (!bDepthText) glDisable(GL_DEPTH_TEST);
	glPixelStorei(GL_PACK_ALIGNMENT, 1); 
	#undef glewGetContext
}

/////////////////////////////////////////////////////////////////////////////////////
void EngineFbo::End()
{
	glReadPixels(0,0,tex->width,tex->height,tex->bpp==8?GL_LUMINANCE:GL_RGB,GL_UNSIGNED_BYTE,tex->buffer);

	#define glewGetContext() ((GLEWContext*)this->engine->WC)
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glPopAttrib();
	#undef glewGetContext

	
}



/////////////////////////////////////////////////////////////////
class FboTest:public Viewer
{
	SmartPointer<Texture> texture;

public:

	//constructor
	FboTest()
	{
		int W=800;
		int H=800;
		texture.reset(new Texture(W,H,24));
		EngineFbo fbo(texture,true); 
	
		if (fbo.engine->Bind())
		{
			fbo.Begin();
			{
				fbo.engine->ClearScreen();
				fbo.engine->SetViewport(0,0,W,H);
				fbo.engine->SetProjectionMatrix(Mat4f::ortho(0,W,0,H, -1.0f,1.0f));
				fbo.engine->SetModelviewMatrix(Mat4f());
				fbo.engine->SetDefaultLight(Vec3f(0,0,1));

				//this one should not be covered (better depth value)
				SmartPointer<Batch> white=Batch::Quad(W/4,H/4,3*W/4,3*H/4,1);
				fbo.engine->Render(white);

				SmartPointer<Batch> R=Batch::Quad(0  ,  0,W/2,  H/2,0);R->setColor(Color4f(1,0,0));fbo.engine->Render(R);
				SmartPointer<Batch> G=Batch::Quad(W/2,  0,W  ,  H/2,0);G->setColor(Color4f(0,1,0));fbo.engine->Render(G);
				SmartPointer<Batch> B=Batch::Quad(0  ,H/2,W/2,  H  ,0);B->setColor(Color4f(0,0,1));fbo.engine->Render(B);
				SmartPointer<Batch> Y=Batch::Quad(W/2,H/2,W  ,  H  ,0);Y->setColor(Color4f(1,1,0));fbo.engine->Render(Y);

				fbo.engine->FlushScreen();
			}
			fbo.End();
			fbo.engine->Unbind();
		}
		else
		{
			Utils::Error(HERE,"Cannot bind to the frame buffer object");
		}

		texture->save(":temp/fbo.bmp");
	}


	//display
	virtual void Render()
	{
		int W=this->frustum.width;
		int H=this->frustum.height;

		SmartPointer<Batch> batch=Batch::Quad(0,0,W,H);
		batch->texture0=texture;

		this->engine->ClearScreen();
		this->engine->SetViewport(0,0,W,H);
		this->engine->SetProjectionMatrix(Mat4f::ortho(0,W,0,H, -1.0f, 1.0f));
		this->engine->SetModelviewMatrix(Mat4f());
		this->engine->SetDefaultLight(Vec3f(0,0,1));

		this->engine->Render(batch);
		this->engine->FlushScreen();
	}
};

/////////////////////////////////////////////////////////////////////////////////////
int EngineFbo::SelfTest()
{
	Log::printf("Testing Fbo...\n");
	FboTest v;
	v.Run();
	v.Wait();
	return 0;
}


#endif //ENGINE_ENABLE_FBO