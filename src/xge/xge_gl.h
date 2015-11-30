#ifndef _XGE_GL_H__
#define _XGE_GL_H__

#include <xge/xge.h>


#define NOMINMAX //otherwise min() max() macro are declared

#if PYPLASM_APPLE
  #ifndef GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
  #define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED 1
  #endif
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <OpenGL/glext.h>
#elif PYPLASM_WINDOWS
  #include <Windows.h>
  #include <GL/GL.h>
  #include <GL/GLU.h>
  #include "glext.win32.h"

#elif PYPLASM_LINUX
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/glx.h>
#endif

#endif //_XGE_GL_H__

