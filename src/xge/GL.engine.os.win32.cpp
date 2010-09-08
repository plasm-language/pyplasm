#include <xge/xge.h>
#include <xge/engine.h>
#include <GL/glew.h>

#ifdef _WINDOWS



//////////////////////////////////////////////////////////////////////////////
void Engine::CreateContext() 
{
	//create also the window
	if (this==_shared_context)
	{
		//create a new window
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
		//ReleaseAssert(ok);can be already registered
		
		HWND hWnd=CreateWindow(L"_xge_engine_dc",L"_xge_engine_dc",
			WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			NULL,NULL,hInstance,NULL);		

		ReleaseAssert(hWnd);
		this->DC = (int64)::GetDC(hWnd);
	}

	//setup the pixel format
	{
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
	}

	//create the Windows HGLRC context
	HGLRC context = wglCreateContext((HDC)this->DC);
	ReleaseAssert(context);
	this->RC=(int64)context;

	//I do not share display contest
	if (this==_shared_context)
	{
		BOOL ok=wglMakeCurrent((HDC)DC,(HGLRC)context);
		ReleaseAssert(ok);
		{
			BOOL ok=wglUseFontBitmapsW((HDC)DC, 0, 256, FONT_DISPLAY_LIST_BASE);
			ReleaseAssert(ok);
		}
		wglMakeCurrent(0,0);
	}
	else
	{
		ReleaseAssert(_shared_context);
		_shared_context->lock.Lock();
		{
			BOOL ok=wglShareLists((HGLRC)_shared_context->RC,context);
			ReleaseAssert(ok);
		}
		_shared_context->lock.Unlock();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
bool Engine::Bind()
{
	this->lock.Lock();
	ReleaseAssert(DC && RC);
	BOOL ok=wglMakeCurrent((HDC)DC,(HGLRC)RC);
	return (ok!=0);
}

///////////////////////////////////////////////////////////////////////////////////////////
bool Engine::Unbind()
{
	bool ret;
	ReleaseAssert(DC && RC);
	BOOL ok=wglMakeCurrent(NULL,NULL);
	ret=(ok!=0);
	this->lock.Unlock();
	return ret;
}

/////////////////////////////////////////////////////////////////////////////////////
int64 Engine::getCurrentContext()
{
	return (int64)wglGetCurrentContext();
}


////////////////////////////////////////////////////////////
void Engine::FlushScreen()
{
	wglSwapLayerBuffers((HDC)this->DC, WGL_SWAP_MAIN_PLANE);
}


/////////////////////////////////////////////////////////////////////////////////////
void Engine::DestroyContext()
{
	ReleaseAssert(this->RC);
	wglDeleteContext((HGLRC)this->RC);
}



//////////////////////////////////////////////////////////////////////////////////////
static LONG WINAPI viewer_dispatcher(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
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
		SetForegroundWindow(hWnd);
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



#endif // _WINDOWS