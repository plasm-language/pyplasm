#if PLATFORM_Darwin
#include <Carbon/Carbon.h>
#endif

#include <xge/xge.h>
#include <xge/engine.h>


#include <GL/glew.h>

//check opengl error
#define CheckGL() \
{\
	GLenum __error_code= glGetError();\
	if (__error_code!= GL_NO_ERROR) Utils::Error(HERE,"ERROR IN OPENGL %s\n",(char*)gluErrorString(__error_code));\
}\
/* */

//for display list
int Engine::FONT_DISPLAY_LIST_BASE=1000;

//for statistics
static int _locked_memory_size=0;
static int _locked_objects[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//mapping from RC to GLEWContext
std::map<int64,int64> Engine::wcs;

//special shutting down signals
static bool _shutting_down=false;

#ifndef USE_JUCE
static Engine* _shared_context=new Engine();

#else //USE_JUCE

#define DONT_SET_USING_JUCE_NAMESPACE 1
#include <juce/juce_amalgamated.h>

//create as Juce has been properly initialized
static Engine* _shared_context=0;

#endif //USE_JUCE


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void Engine::initializeGL()
{
	ReleaseAssert(this->RC);
	this->WC =(int64)(new GLEWContext());
	Engine::wcs[this->RC]=this->WC;

	bool ok=this->Bind();
	ReleaseAssert(ok);

	#define glewGetContext() ((GLEWContext*)this->WC)

	#ifdef PLATFORM_Darwin
	static bool _glew_init_needed=true;
	#else
	bool _glew_init_needed=true;
	#endif

	if (_glew_init_needed)
	{
		_glew_init_needed=false;
		int retcode=glewInit();
		ReleaseAssert(retcode==GLEW_OK);
		ReleaseAssert(glewIsSupported("GL_VERSION_2_0"));
	}
	
	glEnable(GL_LIGHTING);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_CULL_FACE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearDepth(1.0f);
	glClearColor(0.3f,0.4f,0.5f, 0.00f);

	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,0);
	static float white[]={+1.00f,+1.00f,+1.00f,+1.00f};
	glLightfv(GL_LIGHT0,GL_AMBIENT ,white);
	glLightfv(GL_LIGHT0,GL_DIFFUSE ,white);
	glLightfv(GL_LIGHT0,GL_SPECULAR,white);
	glLightfv(GL_LIGHT0, GL_EMISSION, white);

	glActiveTexture       (GL_TEXTURE1);
	glClientActiveTexture (GL_TEXTURE1);
	glTexParameteri       (GL_TEXTURE_2D , GL_TEXTURE_WRAP_S     ,GL_REPEAT);
	glTexParameteri       (GL_TEXTURE_2D , GL_TEXTURE_WRAP_T     ,GL_REPEAT);
	glTexParameterf       (GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER ,GL_LINEAR);
	glTexEnvf             (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE   ,GL_MODULATE);
	glTexParameteri       (GL_TEXTURE_2D  ,GL_TEXTURE_MIN_FILTER ,GL_NEAREST_MIPMAP_LINEAR);

	glActiveTexture       (GL_TEXTURE0);
	glClientActiveTexture (GL_TEXTURE0);
	glTexParameteri       (GL_TEXTURE_2D , GL_TEXTURE_WRAP_S     ,GL_REPEAT);
	glTexParameteri       (GL_TEXTURE_2D , GL_TEXTURE_WRAP_T     ,GL_REPEAT);
	glTexParameterf       (GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER ,GL_LINEAR);
	glTexEnvf             (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE   ,GL_MODULATE);
	glTexParameteri       (GL_TEXTURE_2D  ,GL_TEXTURE_MIN_FILTER ,GL_NEAREST_MIPMAP_LINEAR);

	#undef glewGetContext

	this->Unbind();
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
Engine::Engine()
{
	#ifdef USE_JUCE
	{ 
		this->DC=0;
		this->RC=0;
		this->WC=0;
	}
	#else
	{
		ReleaseAssert(!_shared_context);
		_shared_context=this;
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
		wc.lpszClassName = L"_xge_engine_dc";              
		BOOL ok=RegisterClass(&wc);
		HWND hWnd=CreateWindow(L"_xge_engine_dc",L"_xge_engine_dc",WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,NULL,NULL,hInstance,NULL);		ReleaseAssert(hWnd);
		this->DC = (int64)::GetDC(hWnd);ReleaseAssert(this->DC);
		PIXELFORMATDESCRIPTOR pfd;
		ZeroMemory( &pfd, sizeof( pfd ) );
		pfd.nSize = sizeof( pfd );
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |  PFD_DOUBLEBUFFER | PFD_SWAP_COPY;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cAlphaBits = 8;
		pfd.cDepthBits = 24;
		pfd.iLayerType = PFD_MAIN_PLANE;
		int format = ChoosePixelFormat((HDC)this->DC, &pfd );
		ok=SetPixelFormat((HDC)this->DC, format, &pfd );
		ReleaseAssert(ok);
		HGLRC context = wglCreateContext((HDC)this->DC);
		ReleaseAssert(context);
		this->RC=(int64)context;ReleaseAssert(this->RC);
		ok=wglMakeCurrent((HDC)DC,(HGLRC)context);
		ReleaseAssert(ok);
		ok=wglUseFontBitmapsW((HDC)DC, 0, 256, FONT_DISPLAY_LIST_BASE);
		ReleaseAssert(ok);
		wglMakeCurrent(0,0);
		initializeGL();
		
	}
	#endif
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef USE_JUCE

Engine::Engine(int64 DC)
{
	ReleaseAssert(DC);
	this->DC=DC;

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |  PFD_DOUBLEBUFFER | PFD_SWAP_COPY;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int format = ChoosePixelFormat((HDC)this->DC, &pfd );
	BOOL ok=SetPixelFormat((HDC)this->DC, format, &pfd );
	ReleaseAssert(ok);

	//create the Windows HGLRC context
	HGLRC context = wglCreateContext((HDC)this->DC);
	ReleaseAssert(context);
	this->RC=(int64)context;ReleaseAssert(this->RC);

	//shared context
	ReleaseAssert(_shared_context);
	_shared_context->lock.Lock();
	{
		BOOL ok=wglShareLists((HGLRC)_shared_context->RC,context);
		ReleaseAssert(ok);
	}
	_shared_context->lock.Unlock();

	initializeGL();
}

#endif



///////////////////////////////////////////////////////////////////////////////////////////
bool Engine::Bind()
{
	bool ret;
	this->lock.Lock();
	ReleaseAssert(DC && RC);

	#ifndef USE_JUCE
		BOOL ok=wglMakeCurrent((HDC)DC,(HGLRC)RC);
		ret=(ok!=0);
	#else
		juce::OpenGLContext* context=(juce::OpenGLContext*)this->RC;
		ret=context && context->makeActive();
	#endif

	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////
bool Engine::Unbind()
{
	bool ret;
	ReleaseAssert(DC && RC);

	#ifndef USE_JUCE
		BOOL ok=wglMakeCurrent(NULL,NULL);
		ret=(ok!=0);
	#else
		juce::OpenGLContext* context=(juce::OpenGLContext*)this->RC;
		ret=context && context->makeInactive();
	#endif

	this->lock.Unlock();
	return ret;
}

/////////////////////////////////////////////////////////////////////////////////////
int64 Engine::getCurrentContext()
{
	int64 ret;
	#ifndef USE_JUCE
		ret=(int64)wglGetCurrentContext();
	#else
		ret=(int64)juce::OpenGLContext::getCurrentContext();
	#endif
	return ret;
}


////////////////////////////////////////////////////////////
void Engine::FlushScreen()
{
	#ifndef USE_JUCE
		wglSwapLayerBuffers((HDC)this->DC, WGL_SWAP_MAIN_PLANE);
	#else
		juce::OpenGLContext* context=(juce::OpenGLContext*)this->RC;
		if (context) context->swapBuffers();
	#endif
}


/////////////////////////////////////////////////////////////////////////////////////
void Engine::DestroyContext()
{
	#ifndef USE_JUCE
		ReleaseAssert(this->RC);
		wglDeleteContext((HGLRC)this->RC);
	#else
		;//nothing to do
	#endif
}


///////////////////////////////////////////////////////////////////////////////////////////
void Engine::Shutdown()
{
	_shutting_down=true;
}


///////////////////////////////////////////////////////////////////////////////////////////
void Engine::PrintStatistics()
{
	Log::printf("[OpenGL Engine] statistics\n");
	Log::printf("[OpenGL Engine] total locked memory   %d\n",_locked_memory_size);
	Log::printf("[OpenGL Engine] object array buffer   %d\n",_locked_objects[EngineResource::RESOURCE_ARRAY_BUFFER]);
	Log::printf("[OpenGL Engine] object render buffer  %d\n",_locked_objects[EngineResource::RESOURCE_RENDER_BUFFER]);
	Log::printf("[OpenGL Engine] object frame buffer   %d\n",_locked_objects[EngineResource::RESOURCE_FRAME_BUFFER]);
	Log::printf("[OpenGL Engine] object shader         %d\n",_locked_objects[EngineResource::RESOURCE_SHADER]);
	Log::printf("[OpenGL Engine] object program        %d\n",_locked_objects[EngineResource::RESOURCE_PROGRAM]);
	Log::printf("[OpenGL Engine] object texture        %d\n",_locked_objects[EngineResource::RESOURCE_TEXTURE]);
	Log::printf("\n");
}



//////////////////////////////////////////////////////////////////////////////////////
#ifndef USE_JUCE

static LONG WINAPI viewer_dispatcher(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
	Viewer* viewer=(Viewer*)GetWindowLongPtr(hWnd, GWL_USERDATA);

	POINT point;
	GetCursorPos(&point);ScreenToClient(hWnd, &point);
	
	switch(uMsg) 
	{
		case WM_CLOSE  : viewer->Close()                              ;return 0; 
		case WM_NCPAINT: viewer->Redisplay()                          ;break; 
		case WM_SIZE   : viewer->Resize(LOWORD(lParam),HIWORD(lParam));break; 
		case WM_CHAR   : viewer->Keyboard(wParam,point.x,point.y)     ;return 0; 

		case WM_KEYDOWN:
			if (LOWORD(wParam)>=Keyboard::Key_Left && LOWORD(wParam)<=Keyboard::Key_Down)
				viewer->Keyboard(LOWORD(wParam),point.x,point.y);
			return 0; 

		case WM_LBUTTONDOWN:case WM_MBUTTONDOWN:case WM_RBUTTONDOWN:
		{
			MouseEvent args;
			args.type  =MouseEvent::MousePressed;
			args.button=((uMsg==WM_LBUTTONDOWN)?MouseEvent::LeftButton:(uMsg==WM_RBUTTONDOWN?MouseEvent::RightButton:MouseEvent::MidButton));
			args.x=LOWORD(lParam);
			args.y=HIWORD(lParam);
			viewer->Mouse(args);
			return 0; 
		}
		case WM_LBUTTONUP:case WM_MBUTTONUP:case WM_RBUTTONUP:
		{
			MouseEvent args;
			args.type=MouseEvent::MouseReleased;
			args.button=((uMsg==WM_LBUTTONDOWN)?MouseEvent::LeftButton:(uMsg==WM_RBUTTONDOWN?MouseEvent::RightButton:MouseEvent::MidButton));
			args.x=LOWORD(lParam);
			args.y=HIWORD(lParam);
			viewer->Mouse(args);
			return 0; 
		}
		case WM_MOUSEWHEEL:
		{
			MouseEvent args;
			args.type=MouseEvent::MouseWheel;
			args.button=MouseEvent::NoButton;
			args.delta=GET_WHEEL_DELTA_WPARAM(wParam);
			args.x=point.x;args.y=point.y;
			viewer->Mouse(args);
			return 0; 
		}
		case WM_MOUSEMOVE:
		{
			MouseEvent args;
			args.type=MouseEvent::MouseMoved;
			args.button=((wParam & MK_LBUTTON)?MouseEvent::LeftButton :0) | ((wParam & MK_MBUTTON)?MouseEvent::MidButton  :0) | ((wParam & MK_RBUTTON)?MouseEvent::RightButton:0);
			args.x=LOWORD(lParam);args.y=HIWORD(lParam);
			viewer->Mouse(args);
			return 0; 
		}			
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam); 
} 


void Viewer::doJob(int nworker)
{
	//***************************
	//this is the driver
	//***************************
	if (nworker==0)
	{
		//it's important to create the window in this thread otherwise it does not work
		HINSTANCE hInstance = GetModuleHandle(NULL);
		WNDCLASS  wc;
		wc.style         = 0;                           
		wc.lpfnWndProc   = (WNDPROC)viewer_dispatcher;         
		wc.cbClsExtra    = 0;                           
		wc.cbWndExtra    = 0;                           
		wc.hInstance     = hInstance;                  
		wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO); 
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW); 
		wc.hbrBackground = NULL;                        
		wc.lpszMenuName  = NULL;                        
		wc.lpszClassName = L"XgeViewer";              
		BOOL ok=RegisterClass(&wc);

		HWND hWnd=CreateWindow(L"XgeViewer",L"XgeViewer",WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,NULL,NULL,hInstance,NULL);
		ReleaseAssert(hWnd);

		SetWindowLongPtr(hWnd,GWLP_USERDATA, (LONG_PTR)this);
		ShowWindow(hWnd, SW_SHOW);
		SetForegroundWindow(hWnd);
		UpdateWindow(hWnd);
		HDC hDC=GetDC(hWnd);
		this->engine=new Engine((int64)hDC);	

		while (true)
		{
			//signal to close the window
			if (m_close)
				break;

			MSG msg;
			if (GetMessage(&msg, hWnd, 0, 0)) 
			{
				TranslateMessage(&msg); 
				DispatchMessage (&msg);
			}

			Thread::Sleep(10);//CPU polite
		}

		this->m_close=true;
		Thread::Wait(1);
		delete this->engine;
		this->engine=0;

		ReleaseDC(hWnd,hDC);
		DestroyWindow(hWnd);
		hWnd=0;
		return;
	}

	//************************
	//*** rendering thread ***
	//************************
	
	while (!this->engine) Thread::Sleep(10);
	this->engine->Bind();
	while (true)
	{
		while (!m_redisplay && !m_close) Thread::Sleep(10);
		if (m_close) break;
		m_redisplay=false;
		this->Render();
	}
	this->engine->Unbind();
}

#else //USE_JUCE


//////////////////////////////////////////////////////////////////////////////////////////////////////
class Juce_OpenGLComponent  : public juce::OpenGLComponent
{
public:
	Viewer* viewer;

	Juce_OpenGLComponent(bool isSharedContext=false) :viewer(0)
	{
		//force the creation of the context
		if (isSharedContext)
		{
			this->context=this->createContext();
			this->makeCurrentContextActive (); 
			this->makeCurrentContextInactive();
		}
	}

    virtual void newOpenGLContextCreated()
		{;}

	virtual void resized()
		{if (viewer)  viewer->Resize(getWidth(),getHeight());}

	virtual void mouseMove(const juce::MouseEvent& e)
	{
		if (!viewer) return;
		MouseEvent args;
		args.type=MouseEvent::MouseMoved;
		args.x=e.x;args.y=e.y;
		viewer->Mouse(args);
	}

	virtual void mouseDrag(const juce::MouseEvent& e)
	{
		if (!viewer) return;
		MouseEvent args;
		args.type  =MouseEvent::MouseMoved;
		     if (e.mods.isLeftButtonDown  ()) args.button=MouseEvent::LeftButton;
		else if (e.mods.isMiddleButtonDown()) args.button=MouseEvent::MidButton;
		else if (e.mods.isRightButtonDown ()) args.button=MouseEvent::RightButton;
		else return;
		args.x=e.x;args.y=e.y;
		viewer->Mouse(args);
	}

	virtual void mouseDown(const juce::MouseEvent& e)
	{
		if (!viewer) return;
		MouseEvent args;
		args.type  =MouseEvent::MousePressed;
		     if (e.mods.isLeftButtonDown  ()) args.button=MouseEvent::LeftButton;
		else if (e.mods.isMiddleButtonDown()) args.button=MouseEvent::MidButton;
		else if (e.mods.isRightButtonDown ()) args.button=MouseEvent::RightButton;
		else return;
		args.x=e.x;args.y=e.y;
		viewer->Mouse(args);
	}

	virtual void mouseUp(const juce::MouseEvent& e)
	{
		if (!viewer) return;
		MouseEvent args;
		args.type  =MouseEvent::MouseReleased;
		     if (e.mods.isLeftButtonDown  ()) args.button=MouseEvent::LeftButton;
		else if (e.mods.isMiddleButtonDown()) args.button=MouseEvent::MidButton;
		else if (e.mods.isRightButtonDown ()) args.button=MouseEvent::RightButton;
		else return;
		args.x=e.x;
		args.y=e.y;
		viewer->Mouse(args);
	}

	virtual void mouseWheelMove(const juce::MouseEvent& e, float wheelIncrementX,float wheelIncrementY)
	{	
		if (!viewer) return;
		MouseEvent args;
		args.type=MouseEvent::MouseWheel;
		args.button=MouseEvent::NoButton;
		args.delta=wheelIncrementY>0?120:-120;
		viewer->Mouse(args);
	}

	virtual bool keyPressed (const juce::KeyPress& key)
	{	
		if (!viewer) return false;
		int code=key.getKeyCode();
		     if (code==juce::KeyPress::leftKey)  code=Keyboard::Key_Left ;
		else if (code==juce::KeyPress::rightKey) code=Keyboard::Key_Right;
		else if (code==juce::KeyPress::upKey)    code=Keyboard::Key_Up   ;
		else if (code==juce::KeyPress::downKey)  code=Keyboard::Key_Down ; 
		viewer->Keyboard(code,this->getMouseXYRelative().getX(),this->getMouseXYRelative().getY());
		return false;
	}

	virtual bool renderAndSwapBuffers()
	{
		if (!viewer) return false;
		viewer->Redisplay();
		return true;
	}

    virtual void renderOpenGL()
		{throw "internal error";}
};

class Juce_Component  : public juce::Component
{
public:
	Juce_Component  ()  {}
    ~Juce_Component ()  {deleteAllChildren();}
    void resized    ()  
	{
		juce::Component* first_child=this->getChildComponent(0);
		if (!first_child) return;
		first_child->setBounds (0, 0, getWidth(), getHeight());
	}

};

class Juce_DocumentWindow  : public juce::DocumentWindow
{
public:

	Viewer* viewer;

	Juce_DocumentWindow  (Viewer* viewer,juce::String title=T("PyPlasm Viewer")) 
		:DocumentWindow(title,juce::Colours::azure,juce::DocumentWindow::allButtons, true) 
		{this->viewer=viewer;}
	
	~Juce_DocumentWindow() 
		{}

    void closeButtonPressed()
		{if (viewer) viewer->Close();}
};

 
class Juce_Application : public juce::JUCEApplication
{
public:
	Juce_Application                                ()                                 {}
	~Juce_Application                               ()                                 {}
    void                 shutdown                   ()                                 {}
	const juce::String   getApplicationName         ()                                 {return "PyPlasm Viewer";}
    const juce::String   getApplicationVersion      ()                                 {return "1.0";}
    bool                 moreThanOneInstanceAllowed ()                                 {return true;}
	void                 anotherInstanceStarted     (const juce::String&)              {}
	void                 initialise                 (const juce::String& commandLine)  {}
};


//////////////////////////////////////////////////////////////////////////////////////////////////////



void Viewer::doJob(int nworker)
{
	//***************************
	//this is the driver
	//***************************
	if (nworker==0)
	{
		//need to initialize
		if (!_shared_context)
		{
			juce::initialiseJuce_GUI();
			juce::JUCEApplication::createInstance = (juce::JUCEApplication::CreateInstanceFunction)-1;//need not to be 0 
			Juce_Application* app=new Juce_Application();
			bool bOk=app->initialiseApp("dummy");
			ReleaseAssert(bOk);

#if 0	
			//this works only in windows!
			Juce_OpenGLComponent* glcomponent=new Juce_OpenGLComponent(true);
			glcomponent->setPixelFormat(juce::OpenGLPixelFormat(/*RGB*/8,/*alpha*/8,/*depth*/16,0));

			Engine* engine=new Engine();
			engine->DC= (int64)-1; //fake DC
			engine->RC= (int64)(glcomponent->getCurrentContext());
			ReleaseAssert(engine->RC);

			engine->initializeGL();
			_shared_context=engine;
#else

			Juce_DocumentWindow*  win         = new Juce_DocumentWindow(0,"shared context");
			Juce_Component*       component   = new Juce_Component();
			Juce_OpenGLComponent* glcomponent = new Juce_OpenGLComponent();
		
			glcomponent->viewer=0;
			glcomponent->setPixelFormat(juce::OpenGLPixelFormat(/*RGB*/8,/*alpha*/8,/*depth*/16,0));
			win->setUsingNativeTitleBar(true);		
			win->setResizable (true,true);
			win->setContentComponent(component);
			win->setVisible(true);
			component->addAndMakeVisible (glcomponent);
;
			glcomponent->makeCurrentContextActive (); 
			glcomponent->makeCurrentContextInactive();

			win->setVisible(false);
			win->centreWithSize (1024, 768);

			Engine* engine=new Engine();
			engine->DC= (int64)win;
			engine->RC= (int64)glcomponent->getCurrentContext();ReleaseAssert(engine->RC);
			engine->initializeGL();
			_shared_context=engine;

#endif
		}

		//create the window
		Juce_DocumentWindow*  win         = new Juce_DocumentWindow(this);
		Juce_Component*       component   = new Juce_Component();
		Juce_OpenGLComponent* glcomponent = new Juce_OpenGLComponent();
		
		glcomponent->viewer=this;
		glcomponent->setPixelFormat(juce::OpenGLPixelFormat(/*RGB*/8,/*alpha*/8,/*depth*/16,0));
		glcomponent->setWantsKeyboardFocus(true);

		win->setUsingNativeTitleBar(true);		
		win->setResizable (true,true);
		win->setContentComponent(component);
		win->setVisible(true);
		component->addAndMakeVisible (glcomponent);

		//share context
		_shared_context->lock.Lock();
		glcomponent->shareWith((juce::OpenGLContext*)_shared_context->RC);
		_shared_context->lock.Unlock();

		//force the creation of the context (shared!)
		glcomponent->makeCurrentContextActive (); 
		glcomponent->makeCurrentContextInactive();
		win->centreWithSize (1024, 768);

		//dont' have the bundle
		#ifdef PLATFORM_Darwin
		{
			ProcessSerialNumber psn;
			GetCurrentProcess(&psn);
			TransformProcessType(&psn, kProcessTransformToForegroundApplication);
			SetFrontProcess(&psn);
		}
		#endif

		//this is the new engine
		Engine* engine=new Engine();
		engine->DC= (int64)win;
		engine->RC= (int64)glcomponent->getCurrentContext();ReleaseAssert(engine->RC);
		engine->initializeGL();

		this->engine=engine;

		//creation done! ENTER in the main loop
		while (!this->m_close && juce::MessageManager::getInstance()->runDispatchLoopUntil(100))
			;

		//wait for the threading task to safely quit
		this->m_close=true;
		Thread::Wait(1);

		delete this->engine;
		this->engine=0;
		delete win;
		return;
	}

	//************************
	//*** rendering thread ***
	//************************
	
	while (!this->engine) Thread::Sleep(10);
	this->engine->Bind();
	while (true)
	{
		while (!m_redisplay && !m_close)  Thread::Sleep(10);
		if (m_close) break;
		m_redisplay=false;
		this->Render();
	}
	this->engine->Unbind();
}

#endif //USE_JUCE




///////////////////////////////////////////////////////////////////////////////////////////
EngineResource::EngineResource(int type,unsigned int id,int size)
{
	ReleaseAssert(type>RESOURCE_UNKNOWN);
	this->type=type;
	this->id  =id;
	this->size=size;
	_locked_memory_size+=size;
	_locked_objects[type]++;
}



/////////////////////////////////////////////////////////////////////////////////////
void Engine::Destroy()
{
	//already destroyed
	if (!this->RC)
		return;

	//destroy glew context
	{
		GLEWContext* glew_context=(GLEWContext*)this->WC;
		delete glew_context;
		this->WC=0;
		
		this->wcs.erase(this->RC);
	}

	//to call only one time
	this->DestroyContext();
	this->RC=0;
}




/////////////////////////////////////////////////////////////////////////////////////
Engine::~Engine()
{
	Destroy();
}


////////////////////////////////////////////////////////////
void Engine::SetViewport(int x,int y,int width,int height)
{
	glViewport(x,y,width,height);
}

////////////////////////////////////////////////////////////
void Engine::SetProjectionMatrix(Mat4f mat)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(mat.transpose().mat);
	glMatrixMode( GL_MODELVIEW );
}

////////////////////////////////////////////////////////////
void Engine::SetModelviewMatrix(Mat4f mat)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(mat.transpose().mat);
}


////////////////////////////////////////////////////////////
void Engine::ClearScreen(bool ClearColor,bool ClearDepth)
{
	glClear((ClearColor?GL_COLOR_BUFFER_BIT:0) | (ClearDepth?GL_DEPTH_BUFFER_BIT:0));
}

////////////////////////////////////////////////////////////
void Engine::SetDefaultLight(Vec3f pos)
{
	glEnable(GL_LIGHTING);
	float white[4] ={ 1, 1, 1, 1 };
	float light_pos[4]={pos.x, pos.y, pos.z,1};
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightfv(GL_LIGHT0, GL_EMISSION, white);
}


//////////////////////////////////////////////////////////////////
void Engine::PushModelviewMatrix(Mat4f mat,bool Accumulate)
{
	glPushMatrix();

	if (Accumulate)
		glMultMatrixf(mat.transpose().mat);
	else
		glLoadMatrixf(mat.transpose().mat);
}


//////////////////////////////////////////////////////////////////
void Engine::PopModelviewMatrix()
{
	glPopMatrix();
}

//////////////////////////////////////////////////////////////////
void Engine::PushProjectionMatrix(Mat4f mat,bool Accumulate)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	if (Accumulate)
		glMultMatrixf(mat.transpose().mat);
	else
		glLoadMatrixf(mat.transpose().mat);

	glMatrixMode(GL_MODELVIEW);	
}


//////////////////////////////////////////////////////////////////
void Engine::PopProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);	
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);	
}



//////////////////////////////////////////////////////////////////
void Engine::SetDepthTest(bool value)
{
	if (value)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}



//////////////////////////////////////////////////////////////////
void Engine::SetDepthWrite(bool value)
{
	if (value)
		glDepthMask(true);
	else
		glDepthMask(false);	
		
}

//////////////////////////////////////////////////////////////////
void Engine::SetCulling(int value)
{
	switch(value)
	{
	case CULL_CW:
		glEnable(GL_CULL_FACE); 
		glFrontFace(GL_CCW ); 
		break;

	case CULL_CCW:
		glEnable(GL_CULL_FACE); 
		glFrontFace(GL_CW ); 
		break;

	default:
		glDisable(GL_CULL_FACE); 
		break; 
	};

}


//////////////////////////////////////////////////////////////////
void Engine::SetPolygonMode(int value)
{
	switch(value)
	{
	case Batch::POINTS:
		glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
		break;
	case Batch::LINES:	
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		break;
	default:
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		break;
	}	
		
}

////////////////////////////////////////////////////////////
void Engine::SetPointSize(float value)
{
	glPointSize(value);
}

////////////////////////////////////////////////////////////
void Engine::SetLineWidth(float value)
{
	glLineWidth(value);
}


////////////////////////////////////////////////////////////
void Engine::SetPolygonOffset(float value)
{
	if (value)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset( 1.0f, value);	
	}
	else
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}


////////////////////////////////////////////////////////////
void Engine::Render(std::string text,Vec3f pos,Color4f color,bool DisableDepthTest)
{
	glDisable(GL_LIGHTING);

	if (color.a<1) 	
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	if (DisableDepthTest)
		glDisable(GL_DEPTH_TEST);

	glColor4f(color.r,color.g,color.b,color.a);
	glRasterPos3f(pos.x, pos.y, pos.z);
	glListBase(FONT_DISPLAY_LIST_BASE);
	glCallLists(text.length(), GL_UNSIGNED_BYTE, text.c_str());
	glListBase(0);
	
	if (DisableDepthTest)
		glEnable(GL_DEPTH_TEST);

	if (color.a<1) 
		glDisable(GL_BLEND);

	glEnable(GL_LIGHTING);
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void Engine::Render(SmartPointer<Batch> _batch)
{
	if (!_batch)
		return;

	#define glewGetContext() ((GLEWContext*)this->WC)
	#define BUFFER_OFFSET(_delta_)  ((char *)NULL + _delta_)

	//reset Error
	glGetError();

	Batch& batch=(*_batch);
	ReleaseAssert(batch.primitive>=0 && batch.vertices);

	//material
	if (!batch.colors)
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , &batch.ambient.r );
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , &batch.diffuse.r );
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &batch.specular.r);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION , &batch.emission.r);
		glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS,  batch.shininess );

		if (batch.diffuse[3]<1)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
	}
	
	//vertices
	if (batch.vertices)
	{
		#if ENGINE_AUTOMATIC_VBO_CREATION
		if (!batch.vertices->gpu) createArrayBuffer(batch.vertices);
		#endif 

		if (batch.vertices->gpu)
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, batch.vertices->gpu->id);
			glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		}
		else
		{
			glVertexPointer(3, GL_FLOAT, 0, batch.vertices->mem());
		}
		glEnableClientState(GL_VERTEX_ARRAY);
	}

	//normals
	if (batch.normals)
	{
		#if ENGINE_AUTOMATIC_VBO_CREATION
		if (!batch.normals->gpu)  createArrayBuffer(batch.normals);
		#endif 

		if (batch.normals->gpu)
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, batch.normals->gpu->id);
			glNormalPointer (GL_FLOAT, 0, BUFFER_OFFSET(0));
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		}
		else
		{
			glNormalPointer (GL_FLOAT, 0, batch.normals->mem());
		}
		
		glEnableClientState(GL_NORMAL_ARRAY);
	}

	//colors
	if (batch.colors)
	{
		#if ENGINE_AUTOMATIC_VBO_CREATION
		if (!batch.colors->gpu)  createArrayBuffer(batch.colors);
		#endif

		if (batch.colors->gpu)
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, batch.colors->gpu->id);
			glColorPointer(3,GL_FLOAT, 0, BUFFER_OFFSET(0));
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		}
		else
			glColorPointer(3,GL_FLOAT, 0, batch.colors->mem());

		glEnableClientState(GL_COLOR_ARRAY);
		glEnable(GL_COLOR_MATERIAL);
	}

	//texture0
	if (batch.texture0 && batch.texture0coords)
	{
		//seems to be affected by the current color, since I normally use materials I do not care really
		glColor4f(1,1,1,1);

		//need generation
		if (!batch.texture0      ->gpu) createTexture    (batch.texture0      );

		#if ENGINE_AUTOMATIC_VBO_CREATION
		if (!batch.texture0coords->gpu) createArrayBuffer(batch.texture0coords);
		#endif

		glActiveTexture       (GL_TEXTURE0);
		glClientActiveTexture (GL_TEXTURE0);
		glBindTexture         (GL_TEXTURE_2D, batch.texture0->gpu->id);
		glEnable(GL_TEXTURE_2D);
		
		if (batch.texture0coords->gpu)
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, batch.texture0coords->gpu->id);
			glTexCoordPointer (2, GL_FLOAT, 0,  BUFFER_OFFSET(0));
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		}
		else
			glTexCoordPointer (2, GL_FLOAT, 0,  batch.texture0coords->mem());

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}


	//texture1
	if (batch.texture1 && batch.texture1coords)
	{
		//importante, se pre-lighting is calculated I do not need OpenGL lighting
		glDisable(GL_LIGHTING);
		glColor3f(1,1,1);

		if (!batch.texture1      ->gpu) createTexture     (batch.texture1      );

		#if ENGINE_AUTOMATIC_VBO_CREATION
		if (!batch.texture1coords->gpu) createArrayBuffer(batch.texture1coords);
		#endif

		glActiveTexture       (GL_TEXTURE1);
		glClientActiveTexture (GL_TEXTURE1);
		glBindTexture         (GL_TEXTURE_2D, batch.texture1->gpu->id);
		glEnable(GL_TEXTURE_2D);
		
		if (batch.texture1coords->gpu)
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, batch.texture1coords->gpu->id);
			glTexCoordPointer (2, GL_FLOAT, 0,  BUFFER_OFFSET(0));
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		}
		else
			glTexCoordPointer (2, GL_FLOAT, 0,  batch.texture1coords->mem());

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glActiveTexture       (GL_TEXTURE0);
		glClientActiveTexture (GL_TEXTURE0);
	}

	//setup shader
	#if ENGINE_ENABLE_SHADERS
	{
		GLint current_program;
		glGetIntegerv(GL_CURRENT_PROGRAM,&current_program);
		if (current_program)
		{
			EngineShader::Set1i(this,current_program,"texture0"          ,(int)0);
			EngineShader::Set1b(this,current_program,"skin_enabled"  ,batch.texture0 && batch.texture0coords?true:false);
			EngineShader::Set1i(this,current_program,"texture1"         ,(int)1);
			EngineShader::Set1b(this,current_program,"texture1_enabled"  ,batch.texture1 && batch.texture1coords?true:false);
		}
	}
	#endif


	//TODO 
	if (glIsEnabled(GL_CULL_FACE))
	{
		//take care of the inversion matrix
		/*int front_face=(batch.mode & Batch::FRONT_FACE_CCW)?GL_CCW:GL_CW;
		Mat4f& M=batch.matrix;
		float det3x3=M(0,0)*(M(1,1)*M(2,2)-M(2,1)*M(1,2)) +M(0,1)*(M(1,2)*M(2,0)-M(2,2)*M(1,0))+M(0,2)*(M(1,0)*M(2,1)-M(2,0)*M(1,1));
		if (det3x3<0) front_face=front_face==GL_CCW?GL_CW:GL_CCW;
		glEnable(GL_CULL_FACE); 
		glFrontFace(front_face); */
	}

	//draw the primitives
	glPushMatrix();
	{
		const float* _mat=batch.matrix.mat;
		float _m[16] ={
			  _mat[0],_mat[4],_mat[ 8],_mat[12],
			  _mat[1],_mat[5],_mat[ 9],_mat[13],
			  _mat[2],_mat[6],_mat[10],_mat[14],
			  _mat[3],_mat[7],_mat[11],_mat[15]
		};
		glMultMatrixf(_m);

		int num_vertices=batch.vertices->size()/3;
		glDrawArrays(batch.primitive, 0, num_vertices);
	}
	glPopMatrix();

	if (batch.diffuse[3]<1) 
	{
		glDisable(GL_BLEND);
	}

	//important to do do in reverse order
	if (batch.texture1 && batch.texture1coords)
	{
		glActiveTexture(GL_TEXTURE1);
		glClientActiveTexture(GL_TEXTURE1);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glActiveTexture(GL_TEXTURE0);
		glClientActiveTexture(GL_TEXTURE0);
	}

	if (batch.texture0 && batch.texture0coords)
	{
		glActiveTexture(GL_TEXTURE0);
		glClientActiveTexture(GL_TEXTURE0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}

	if (batch.colors) 
	{
		glDisableClientState(GL_COLOR_ARRAY);
		glDisable(GL_COLOR_MATERIAL);
	}
	
	if (batch.normals) 
	{
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	
	if (batch.vertices) 
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	CheckGL();

	#undef BUFFER_OFFSET
	#undef glewGetContext
}


/////////////////////////////////////////////////////////////////////////////////////
void Engine::createArrayBuffer(SmartPointer<Vector> vector)
{
	#define glewGetContext() ((GLEWContext*)this->WC)
	
	//dont want to loose the actual context, do I do not do a Bind
	_shared_context->lock.Lock();
	{
		//can happen that meanwhile another thread has built the gpu
		if (!vector->gpu)
		{
			int size=vector->memsize();
			void* data=vector->mem();

			GLuint ret;

			//generate
			
			glGenBuffersARB(1,&ret);
			ReleaseAssert(ret);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, ret);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, size, 0, GL_STATIC_DRAW_ARB);

			glBindBufferARB(GL_ARRAY_BUFFER_ARB, ret);
			void* gpu_data = glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
			ReleaseAssert(gpu_data);
			memcpy(gpu_data,data,size);
			glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);

			//can happen that meanwhile another thread has built the gpu+=size;

			SmartPointer<EngineResource> gpu(new EngineResource(EngineResource::RESOURCE_ARRAY_BUFFER,ret,size));

			//TODO: this operation MUST BE atomic but is not probably not
			vector->gpu=gpu;
			
		}
	}
	_shared_context->lock.Unlock();

	#undef glewGetContext
}


/////////////////////////////////////////////////////////////////////////////////////
void Engine::createTexture(SmartPointer<Texture> texture)
{
	#define glewGetContext() ((GLEWContext*)this->WC)

	//dont want to loose the actual context, do I do not do a Bind
	_shared_context->lock.Lock();
	{
		//can happen that meanwhile another thread has built the gpu
		if (!texture->gpu)
		{
			unsigned int _texture_id;
			glGenTextures(1,&_texture_id);
			ReleaseAssert(_texture_id); //problem happened

			glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
			glBindTexture (GL_TEXTURE_2D, _texture_id);
			float maxsize;
			glGetFloatv(GL_MAX_TEXTURE_SIZE,&maxsize); 
			DebugAssert (texture->width<=maxsize && texture->height<=maxsize);

			unsigned int format=(texture->bpp==24)?GL_RGB:(texture->bpp==32?GL_RGBA:GL_LUMINANCE);
			unsigned int type=GL_UNSIGNED_BYTE;

			gluBuild2DMipmaps(GL_TEXTURE_2D,texture->bpp/8,texture->width, texture->height,format, type, texture->buffer);
			SmartPointer<EngineResource> gpu(new EngineResource(EngineResource::RESOURCE_TEXTURE,_texture_id,texture->memsize()));

			//TODO: this operation MUST BE atomic but is not probably not
			texture->gpu=gpu;	
		}
	}
	_shared_context->lock.Unlock();

	#undef glewGetContext
}




/////////////////////////////////////////////////////////////////////////////////////
void Engine::removeFromGpu(EngineResource* resource)
{	
	//problem on shutting down, do not try to release resources since the context are not valid
	if (_shutting_down)
	{
		//Log::printf("WARNING:: The engine is in shutdown, so I'm not releasing EngineResource on GPU");
		resource->id=0;
		resource->type=EngineResource::RESOURCE_UNKNOWN;
		resource->size=0;
		return;
	}
 
	int64 RC=getCurrentContext();
	bool bValidContext=(RC!=0);

	if (!bValidContext) 
	{
		if (!_shared_context->Bind())
		{
			Log::printf("ERROR:: cannot bind the actual DC, so I'm not releasing EngineResource on GPU");
			resource->id=0;
			resource->type=EngineResource::RESOURCE_UNKNOWN;
			resource->size=0;
			return;
		}

		RC=_shared_context->RC;
	}
	else 
	{
				
		//I need the lock anyway to avoid collisions
		_shared_context->lock.Lock();
	}

	ReleaseAssert(wcs.find(RC)!=wcs.end());
	int64 glew_context=Engine::wcs[RC];
	#define glewGetContext() ((GLEWContext*)glew_context)

	switch (resource->type)
	{
	case EngineResource::RESOURCE_ARRAY_BUFFER:
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, resource->id);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, 0, 0, GL_STATIC_DRAW_ARB);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, resource->id);
		glDeleteBuffersARB(1, &resource->id);
		break;

	case EngineResource::RESOURCE_RENDER_BUFFER:
		glDeleteRenderbuffersEXT(1, &resource->id);
		break;

	case EngineResource::RESOURCE_FRAME_BUFFER:
		glDeleteFramebuffersEXT (1, &resource->id);
		break;

	case EngineResource::RESOURCE_SHADER:
		glDeleteShader(resource->id);
		break;

	case EngineResource::RESOURCE_PROGRAM:
		glDeleteProgram(resource->id);
		break;

	case EngineResource::RESOURCE_TEXTURE:
		glDeleteTextures(1,&resource->id);
		break;
	}

	--_locked_objects[resource->type];

	//free the memory from the counter
	_locked_memory_size-=resource->size;

	//invalidate the resource
	resource->id=0;
	resource->type=EngineResource::RESOURCE_UNKNOWN;
	resource->size=0;

	if (!bValidContext) 
		_shared_context->Unbind(); 
	else 
		_shared_context->lock.Unlock();
}


