#ifndef _KEYBOARD_H__
#define _KEYBOARD_H__

#include <xge/xge.h>


class XGE_API Keyboard
{
public:

	enum
	{
		Key_Left  =0x25,
		Key_Up    =0x26,
		Key_Right =0x27,
		Key_Down  =0x28
	};


	static bool isShiftPressed();
	static bool isAltPressed();
	static bool isControlPressed();
};


#endif //_KEYBOARD_H__

