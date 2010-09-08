#include <xge/xge.h>


#ifdef Darwin

#undef DebugAssert

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/glext.h>
#include <Carbon/Carbon.h>
#include <AGL/agl.h>







//////////////////////////////////////////////////////////////////////////////
void Engine::CreateContext() 
{
	//important: i do not have the bundle
	{
		ProcessSerialNumber psn;
		GetCurrentProcess(&psn);
		TransformProcessType(&psn, kProcessTransformToForegroundApplication);
		SetFrontProcess(&psn);
	}

	//set pixel format
	AGLPixelFormat pixelFormat = NULL;
	GLint attrib[64];
	int i = 0;
	attrib[i++] = AGL_RGBA;
	attrib[i++] = AGL_DOUBLEBUFFER; 
	attrib[i++] = AGL_ACCELERATED;
	attrib[i++]	= AGL_PIXEL_SIZE; 
	attrib[i++]	= 32; 
	attrib[i++]	= AGL_DEPTH_SIZE; // Il contesto usa un depth buffer...
	attrib[i++]	= 16; 	
	attrib[i++] = AGL_NONE;  //TODO: aggiungere opzione PFD_SWAP_COPY se vuoi il rendering progressivo!
	pixelFormat = aglChoosePixelFormat(NULL, 1, attrib);
	this->RC = (int64)aglCreateContext(pixelFormat, this==_shared_context?(AGLContext)0:(AGLContext)_shared_context->RC);
	ReleaseAssert(RC);
	aglDestroyPixelFormat(pixelFormat);
	
	//I do not share display contest
	if (this==_shared_context)
	{	
		WindowPtr window=NULL;
		Rect r = {0, 0, 50, 50};
		CreateNewWindow(kDocumentWindowClass, kWindowOpaqueForEventsAttribute | 
					kWindowStandardHandlerAttribute | 
					kWindowInWindowMenuAttribute | 
					kWindowStandardDocumentAttributes, &r, &window);
		this->DC=(int64)GetWindowPort(window);
	}
	else
	{
		//only one time!
		aglSetDrawable((AGLContext)this->RC,(AGLDrawable)this->DC);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
bool Engine::Bind()
{
	this->lock.Lock();
	bool ret=aglSetCurrentContext((AGLContext)this->RC);
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////
bool Engine::Unbind()
{
	bool ret=aglSetCurrentContext(NULL);
	this->lock.Unlock();
	return ret;
}


/////////////////////////////////////////////////////////////////////////////////////
int64 Engine::getCurrentContext()
{
	return (int64)aglGetCurrentContext();

}

////////////////////////////////////////////////////////////
void Engine::FlushScreen()
{
	aglSwapBuffers((AGLContext)RC);
}


/////////////////////////////////////////////////////////////////////////////////////
void Engine::DestroyContext()
{
	if (this!=_shared_context)
	{
		_shared_context->lock.Lock();
		aglDestroyContext((AGLContext)this->RC);
		_shared_context->lock.Unlock();
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
int ConvertKeyCode(int value)
{
	// Proper key detection seems to want a switch statement, unfortunately
	switch (value) 
	{
		case 0: return('a');
		case 1: return('s');
		case 2: return('d');
		case 3: return('f');
		case 4: return('h');
		case 5: return('g');
		case 6: return('z');
		case 7: return('x');
		case 8: return('c');
		case 9: return('v');
		// what is 10?
		case 11: return('b');
		case 12: return('q');
		case 13: return('w');
		case 14: return('e');
		case 15: return('r');
		case 16: return('y');
		case 17: return('t');
		case 18: return('1');
		case 19: return('2');
		case 20: return('3');
		case 21: return('4');
		case 22: return('6');
		case 23: return('5');
		case 24: return('=');
		case 25: return('9');
		case 26: return('7');
		case 27: return('-');
		case 28: return('8');
		case 29: return('0');
		case 30: return(']');
		case 31: return('o');
		case 32: return('u');
		case 33: return('[');
		case 34: return('i');
		case 35: return('p');
		case 36: return(13);
		case 37: return('l');
		case 38: return('j');
		case 39: return('k');
		case 41: return(';');
		case 42: return('\\');
		case 43: return(',');
		case 44: return('/');
		case 45: return('n');
		case 46: return('m');
		case 47: return('.');
		case 48: return('\t');
		case 49: return(' ');
		case 52: return(13);
		case 53: return(27);
		case 65: return('.');
		case 67: return('*');
		case 69: return('+');
		case 75: return('/');
		case 76: return(13);   // numberpad on full kbd
		case 78: return('-');
		case 81: return('=');
		case 82: return('0');
		case 83: return('1');
		case 84: return('2');
		case 85: return('3');
		case 86: return('4');
		case 87: return('5');
		case 88: return('6');
		case 89: return('7');
		case 91: return('8');
		case 92: return('9');
		
		/*
		case 71: return('CLEAR');
		//case 50: return('`');
		//case 51: return('DELETE');
		case 96: return('F5');
		case 97: return('F6');
		case 98: return('F7');
		case 99: return('F3');
		case 100: return('F8');
		case 101: return('F9');
		case 103: return('F11');
		case 105: return('F13');
		case 107: return('F14');
		case 109: return('F10');
		case 111: return('F12');
		case 113: return('F15');
		case 114: return('HELP');
		case 117: return('DELETE');  // full keyboard right side numberpad
		case 118: return('F4');
		case 120: return('F2');
		case 122: return('F1');
		*/
		
		case 123:case 115: return Keyboard::Key_Left;
		case 116:case 126: return Keyboard::Key_Up;
		case 121:case 125: return Keyboard::Key_Down;
		case 119:case 124: return Keyboard::Key_Right;

		default:
			return 0;
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
OSStatus viewer_dispatcher(EventHandlerCallRef myHandler, EventRef event, void* userData) 
{
	OSStatus result = eventNotHandledErr;
	UInt32 evtClass = GetEventClass(event);
    UInt32 evtKind  = GetEventKind(event);
	
	Viewer* viewer=(Viewer*)userData;
	WindowPtr window=(viewer)?(WindowPtr)viewer->window:0;
	
	switch (evtClass) 
	{
		case kEventClassKeyboard:
			switch (evtKind) 
			{
				case kEventRawKeyUp:
				{
					result = CallNextEventHandler(myHandler, event);
					
					if (eventNotHandledErr == result) 
					{
						UInt32 keyCode;
						GetEventParameter(event, kEventParamKeyCode, typeUInt32, NULL, sizeof(UInt32), NULL, &keyCode);
						keyCode=ConvertKeyCode(keyCode);
						viewer->Keyboard(keyCode,0,0);//doto mouse position
					}
						
					break;
				}
			}
			break;
			
		case kEventClassWindow:
			GetEventParameter(event, kEventParamDirectObject, typeWindowRef, NULL, sizeof(WindowRef), NULL, &window);
			
			switch (evtKind) 
			{
				case kEventWindowClose:
				case kEventWindowCloseAll:
				{
					viewer->Close();
					break;
				}
					
				case kEventWindowBoundsChanged:
				{
					Rect r;
					GetWindowPortBounds(window, &r);
					int width  = r.right - r.left;
					int height = r.bottom - r.top;
					
					
					if (viewer) viewer->Resize(width,height);
					break;
				}
			}
			break;
	
		case kEventClassMouse:
		{
			EventMouseButton button = 0;
			HIPoint location = {0.0f, 0.0f};
			UInt32  modifiers = 0;	
			long	wheelDelta = 0;
	
			GetEventParameter(event, kEventParamWindowRef, typeWindowRef, NULL, sizeof(WindowRef), NULL, &window);
			result = CallNextEventHandler(myHandler, event);	
			
			if (eventNotHandledErr == result) 
			{
				GetEventParameter(event, kEventParamMouseButton, typeMouseButton, NULL, sizeof(EventMouseButton), NULL, &button);
				GetEventParameter(event, kEventParamWindowMouseLocation, typeHIPoint, NULL, sizeof(HIPoint), NULL, &location);
				GetEventParameter(event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifiers);
				GetEventParameter(event, kEventParamMouseWheelDelta, typeLongInteger, NULL, sizeof(long), NULL, &wheelDelta);
				
				switch (evtKind) 
				{
					case kEventMouseDown:
					{
						MouseEvent args;
                        args.type  =MouseEvent::MousePressed;
                        args.button=((button==kEventMouseButtonPrimary)?MouseEvent::LeftButton:(button==kEventMouseButtonSecondary?MouseEvent::RightButton:MouseEvent::MidButton));
                        args.x=location.x;
                        args.y=location.y;
                        viewer->Mouse(args);
						break;	
					}
										
					case kEventMouseUp:
					{
						MouseEvent args;
                        args.type  =MouseEvent::MouseReleased;
                        args.button=((button==kEventMouseButtonPrimary)?MouseEvent::LeftButton:(button==kEventMouseButtonSecondary?MouseEvent::RightButton:MouseEvent::MidButton));
                        args.x=location.x;
                        args.y=location.y;
                        viewer->Mouse(args);					
						//printf("Mouse Up\n");
						break;		
					}		
							
					case kEventMouseDragged:
					{
						MouseEvent args;
                        args.type  =MouseEvent::MouseMoved;
                        args.button=((button==kEventMouseButtonPrimary)?MouseEvent::LeftButton:(button==kEventMouseButtonSecondary?MouseEvent::RightButton:MouseEvent::MidButton));
                        args.x=location.x;
                        args.y=location.y;
                        viewer->Mouse(args);
						break;
					}
						
					case kEventMouseMoved:
					{
						MouseEvent args;
                        args.type  =MouseEvent::MouseMoved;
                        args.button=MouseEvent::NoButton;
						args.x=location.x;
                        args.y=location.y;
                        viewer->Mouse(args);
						viewer->Redisplay();
						break;
					}
						
					case kEventMouseWheelMoved:
					{
						MouseEvent args;
                        args.type=MouseEvent::MouseWheel;
                        args.button=MouseEvent::NoButton;
                        args.delta=wheelDelta;
                        viewer->Mouse(args);
						break;
					}
				}
			}
			break;
		}
	}
	
		//a close signal!
	if (viewer && viewer->m_close)
		QuitApplicationEventLoop();
	
	return result;
	
}

//////////////////////////////////////////////////////////////////////////////////////
//viewer specific part
//////////////////////////////////////////////////////////////////////////////////////
void Viewer::doJob(int nworker)
{
	//***************************/***************************/***************************
	//this is the driver (which runs in the main loop, not in different thread, see Viewer.Run() for macosx!!!!)
	//***************************/***************************/***************************
	if (nworker==0)
	{	
		WindowPtr window=NULL;
		int x1=Utils::IntRand(50,120);
		int y1=Utils::IntRand(50,120);
		int width=800;
		int height=800;
		Rect r = {x1,y1, x1+width, y1+height};
		CreateNewWindow(kDocumentWindowClass, kWindowOpaqueForEventsAttribute | 
					kWindowStandardHandlerAttribute | 
					kWindowInWindowMenuAttribute | 
					kWindowStandardDocumentAttributes, &r, &window);
	
		SetWindowTitleWithCFString(window,  CFStringCreateWithCString(kCFAllocatorDefault, "Xge Viewer", CFStringGetSystemEncoding()));
		ShowWindow(window);

		//install handlers
		EventHandlerRef ref;
		EventTypeSpec _list[] = { { kEventClassMouse, kEventMouseDown },
							      { kEventClassMouse, kEventMouseUp }, 
							      { kEventClassMouse, kEventMouseDragged },
							      { kEventClassMouse, kEventMouseMoved },
							      { kEventClassMouse, kEventMouseWheelMoved },
								  { kEventClassWindow, kEventWindowClose },
							      { kEventClassWindow, kEventWindowCloseAll },
							      { kEventClassWindow, kEventWindowBoundsChanged },
							      { kEventClassKeyboard, kEventRawKeyUp } };
								  
		EventHandlerUPP _event_handler = NewEventHandlerUPP(viewer_dispatcher);
		InstallWindowEventHandler(window, _event_handler, GetEventTypeCount(_list), _list, this, &ref);

		//send the first resixe
		Resize(width,height);
		int64 DC=(int64)GetWindowPort(window);
		this->engine=new Engine(DC);
		this->window=(int64)window;
		
		//macosx does not support GUI in multiple thread
		RunApplicationEventLoop();
		
		DisposeEventHandlerUPP(_event_handler);
		DisposeWindow( window );
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
		
		//if the bounds have changed
		aglUpdateContext((AGLContext)engine->RC);
		
		this->Render();
	}
	this->engine->Unbind();
	delete this->engine;
	this->engine=0;
}


#endif //Darwin
