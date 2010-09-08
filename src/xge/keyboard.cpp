#include <xge/xge.h>
#include <xge/keyboard.h>


////////////////////////////////////////////////////////////////////
bool Keyboard::isShiftPressed()
{
	#ifdef _WINDOWS
	return (GetKeyState(VK_SHIFT  ) & 0x8000)?true:false;
	#else
	//TODO!
	return false;
	#endif
}

////////////////////////////////////////////////////////////////////
bool Keyboard::isAltPressed()
{
	#ifdef _WINDOWS
	return (GetKeyState(VK_MENU   ) & 0x8000)?true:false;
	#else
	//TODO!
	return false;
	#endif	
}


////////////////////////////////////////////////////////////////////
bool Keyboard::isControlPressed()
{
	#ifdef _WINDOWS
	return (GetKeyState(VK_CONTROL) & 0x8000)?true:false;
	#else
	//TODO!
	return false;
	#endif
}