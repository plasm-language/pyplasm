#ifndef _LOCALSYSTEM_H__
#define _LOCALSYSTEM_H__

#include <xge/xge.h>
#include <xge/vec.h>

class XGE_API LocalSystem
{
public:
	Vec3f pos;
    Vec3f dir;
    Vec3f vup;

	//constructor
	LocalSystem()
	{
		pos = Vec3f(0, 0, 0);
		dir = Vec3f(1, 0, 0);
		vup = Vec3f(0, 0, 1);
	}
};


#endif //_LOCALSYSTEM_H__

