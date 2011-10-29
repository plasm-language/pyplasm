#ifndef _MOUSE_EVENT_H__
#define _MOUSE_EVENT_H__

#include <xge/xge.h>

class XGE_API MouseEvent
{
public:

	//type of event
	enum
	{
        MousePressed,
        MouseMoved,
        MouseReleased,
        MouseWheel,
        MouseDoubleClick
	};
	int type;

	//! mouse button
	enum
	{
		NoButton         = 0,
		LeftButton       = 1048576,
		RightButton      = 2097152,
		MidButton        = 4194304
	};
	int button;

	//coordinates
	int x,y;

	//for mouse wheel
	int delta;

	//!default constructor
	inline MouseEvent()
	{
		type=0;
		button=NoButton;
		x=y=0;
		delta=0;
	}

	//constructor
	inline MouseEvent(int type,int button,int x,int y,int delta=0)
	{
		this->type=type;
		this->button=button;
		this->x=x;
		this->y=y;
		this->delta=delta;
	}
};


#endif //_MOUSE_EVENT_H__


