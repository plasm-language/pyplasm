#ifndef _XGE_H__
#define _XGE_H__

// DLL related, inport export
#ifdef XGE_EXPORTS
#ifdef _WINDOWS
#define XGE_API __declspec(dllexport)
#else
#define XGE_API __attribute__((visibility("default")))
#endif
#else
#ifdef _WINDOWS
#define XGE_API __declspec(dllimport)
#else
#define XGE_API
#endif
#endif

//64 bit support
#if defined(_WINDOWS)
typedef __int64           			int64;
typedef unsigned __int64 			uint64;
#else
typedef signed long long int        int64;
typedef unsigned long long int      uint64;
#endif

//windows specific stuff
#if defined(_WINDOWS)
#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#include <windows.h>
#include <direct.h>
#pragma warning (disable:4251) // class 'std::tr1::shared_ptr<_Ty>' needs to have dll-interface to be used by clients of class
#pragma warning (disable:4244) // 'initializing' : conversion from 'double' to 'float', possible loss of data
#define strcmpi(s1,s2) _strcmpi(s1,s2)
#define isnan(a)       _isnan(a)
#define finite(a)      _finite(a)
#define getcwd         _getcwd
#endif 

//Darwin specific stuff
#ifdef Darwin
#include <tr1/memory>
#include <stdexcept>
#include <strings.h>
#include <sys/time.h>
#define strcmpi strcasecmp
#endif //Darwin

// general headers
#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <stdarg.h>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <limits> 
#include <algorithm>
#include <cctype>

//Some defines
#define SmartPointer std::tr1::shared_ptr

// DebugAssert and ReleaseAssert
#define __stringify__(x) #x
#define __to_string__(x) __stringify__(x)
#define HERE __FILE__ ":" __to_string__(__LINE__)
#define ReleaseAssert(_Expression) \
{\
	if (!(_Expression)) \
		Utils::Error(HERE,#_Expression);\
}\
/* */


#ifdef _DEBUG
#define DebugAssert(_Expression) ReleaseAssert(_Expression)
#else
#define DebugAssert(_Expression) ((void)0)
#endif //_DEBUG

//all XGE headers here
#include <xge/config.h>
#include <xge/utils.h>
#include <xge/log.h>
#include <xge/clock.h>
#include <xge/keyboard.h>
#include <xge/spinlock.h>
#include <xge/mempool.h>
#include <xge/archive.h>
#include <xge/encoder.h>
#include <xge/filesystem.h>
#include <xge/thread.h>
#include <xge/mouseevent.h>
#include <xge/vec.h>
#include <xge/triangle.h>
#include <xge/plane.h>
#include <xge/mat.h>
#include <xge/ball.h>
#include <xge/box.h>
#include <xge/ray.h>
#include <xge/quaternion.h>
#include <xge/localsystem.h>
#include <xge/vector.h>
#include <xge/color4f.h>
#include <xge/frustum.h>
#include <xge/texture.h>
#include <xge/manipulator.h>
#include <xge/batch.h>
#include <xge/pick.h>
#include <xge/graph.h>
#include <xge/engine.h>
#include <xge/viewer.h>
#include <xge/octree.h>
#include <xge/unwrapper.h>
#include <xge/bake.h>
#include <xge/plasm.h>

#endif //_XGE_H__

