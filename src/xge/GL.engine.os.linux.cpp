#include <xge/xge.h>
#include <xge/engine.h>
#include <GL/glew.h>

#ifdef Linux

//////////////////////////////////////////////////////////////////////////////
void Engine::CreateContext() 
{

	//if a do not have a device context I must create a new one
	if (this==_shared_contex)
	{
		Utils::Error("TODO);
	}

	//setup the pixel format
	int list[32];
	int n = 0;
	list[n++] = GLX_RGBA;
	list[n++] = GLX_GREEN_SIZE;
	list[n++] = 8;
	list[n++] = GLX_ALPHA_SIZE;
	list[n++] = 8;
	list[n++] = GLX_DOUBLEBUFFER;
	list[n++] = GLX_DEPTH_SIZE; 
	list[n++] = 24;

#if defined(GLX_VERSION_1_1) && defined(GLX_SGIS_multisample)
	list[n++] = GLX_SAMPLES_SGIS;
	list[n++] = 4; // value Glut uses
#endif
	
	list[n] = 0;

	//setup pixel format
	fl_open_display();
	XVisualInfo *visp = glXChooseVisual(fl_display, fl_screen, (int *)list);
	this->context = glXCreateContext(fl_display, vis, shared_ctx, 1);

	GLXContext  context = glXCreateContext(display, vis, bShareContext?_shared_context->RC:0, 1);
	ReleaseAssert(context);
	this->RC=(int64)context;

	//I do not share display contest
	if (this==_shared_context)
	{
		//todo
		//BOOL ok=wglMakeCurrent((HDC)DC,(HGLRC)context);
		//ReleaseAssert(ok);
		//{

		//	BOOL ok=wglUseFontBitmapsW((HDC)DC, 0, 256, FONT_DISPLAY_LIST_BASE);
		//	ReleaseAssert(ok);
		//}
		//wglMakeCurrent(0,0);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
bool Engine::Bind()
{
	this->lock.Lock();
	Bool ok=glXMakeCurrent(fl_display, this->Window, RC);
	return ok;
}

///////////////////////////////////////////////////////////////////////////////////////////
bool Engine::Unbind()
{
	Bool ok=glXMakeCurrent(fl_display, 0, 0);
	this->lock.Unlock();
	return ok;
}



/////////////////////////////////////////////////////////////////////////////////////
int64 Engine::getCurrentContext()
{
	return (int64)glXGetCurrentContext();
}


////////////////////////////////////////////////////////////
void Engine::FlushScreen()
{
	glXSwapBuffers(dpy,drawable);
}


/////////////////////////////////////////////////////////////////////////////////////
void Engine::DestroyContext()
{
	ReleaseAssert(this->RC);
	glXDestroyContext(fl_display, this->RC);
}


Viewer* viewer=(Viewer*)GetWindowLongPtr(hWnd, GWL_USERDATA);
	
	switch(uMsg) 
	{
		case WM_CLOSE:
		{
			viewer->Close();
			return 0; 
		}

		case WM_NCPAINT:
		{
			viewer->Redisplay();
			break; 
		}

		case WM_SIZE:
		{
			int width=LOWORD(lParam);
			int height=HIWORD(lParam);
			viewer->Resize(width,height);
			break; 
		}


		case WM_KEYDOWN:
		{
			POINT point;
			GetCursorPos(&point);
			ScreenToClient(hWnd, &point);
			
			int key=LOWORD(wParam);
			if (key>=Keyboard::Key_Left && key<=Keyboard::Key_Down)
			{
				viewer->Keyboard(key,point.x,point.y);
				return 0;
			}
			return 0; 
		}
		case WM_CHAR:
			{
				POINT point;
				GetCursorPos(&point);
				ScreenToClient(hWnd, &point);

				int key=wParam;
				viewer->Keyboard(key,point.x,point.y);
				return 0; 
			}
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
			viewer->Mouse(args);
			return 0; 
		}
		case WM_MOUSEMOVE:
		{
			MouseEvent args;
			args.type=MouseEvent::MouseMoved;

			args.button=
			   ((wParam & MK_LBUTTON)?MouseEvent::LeftButton :0)
			 | ((wParam & MK_MBUTTON)?MouseEvent::MidButton  :0)
			 | ((wParam & MK_RBUTTON)?MouseEvent::RightButton:0);

			args.x=LOWORD(lParam);
			args.y=HIWORD(lParam);
			viewer->Mouse(args);
			return 0; 
		}			
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam); 
} 





//////////////////////////////////////////////////////////////////////////////////////
//viewer specific part
//////////////////////////////////////////////////////////////////////////////////////
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

		//can be already registered
		//ReleaseAssert(ok);
		
		HWND hWnd=CreateWindow(L"XgeViewer",L"XgeViewer",
			WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			NULL,NULL,hInstance,NULL);

		ReleaseAssert(hWnd);

		//need an arg for the callback
		SetWindowLongPtr(hWnd,GWLP_USERDATA, (LONG_PTR)this);

		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
		HDC hDC=GetDC(hWnd);
		this->engine=new Engine((int64)hDC);	

		while (true)
		{
			//signal to close the window
			if (m_close)
			{
				//wait for the rendering thread destruction
				while (this->engine)
					Thread::Sleep(10);
				break;
			}

			MSG msg;
			if (GetMessage(&msg, hWnd, 0, 0)) 
			{
				TranslateMessage(&msg); 
				DispatchMessage (&msg);
			}

			Thread::Sleep(10);//CPU polite
		}

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
	delete this->engine;
	this->engine=0;
}



#endif //Linux