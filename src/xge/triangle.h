#ifndef _TRIANGLE2I_H__
#define _TRIANGLE2I_H__

#include <xge/xge.h>
#include <xge/vec.h>

class XGE_API Triangle2i
{
public:

	Vec2i p0,p1,p2;

	//constructors
	inline          Triangle2i()                                                 {;}
	inline          Triangle2i(const Triangle2i& src)                            {this->p0=src.p0;this->p1=src.p1;this->p2=src.p2;}
	inline explicit Triangle2i(const Vec2i& p0,const Vec2i& p1,const Vec2i& p2)  {this->p0=p0;this->p1=p1;this->p2=p2;}

	//get point
	inline Vec2i getPoint(int ref) const
	{
		DebugAssert(ref>=0 && ref<=2);
		if (ref==0) return p0;
		if (ref==1) return p1;
		            return p2;
	}

	//get point
	inline Vec2i& getRefPoint(int ref)
	{
		DebugAssert(ref>=0 && ref<=2);
		if (ref==0) return p0;
		if (ref==1) return p1;
		            return p2;
	}

	//left/top
	inline int left   () const {return min3(p0.x,p1.x,p2.x);}
	inline int right  () const {return max3(p0.x,p1.x,p2.x);}
	inline int bottom () const {return min3(p0.y,p1.y,p2.y);}
	inline int top    () const {return max3(p0.y,p1.y,p2.y);}

	//equality operator
	inline bool operator==(const Triangle2i& src) const {return p0==src.p0 && p1==src.p1 && p2==src.p2;}

	//scale/translate
	inline Triangle2i scale    (int scalex,int scaley) const {return Triangle2i(p0.scale(scalex,scaley),p1.scale(scalex,scaley),p2.scale(scalex,scaley));}
	inline Triangle2i translate(int deltax,int deltay) const {return Triangle2i(p0.translate(deltax,deltay),p1.translate(deltax,deltay),p2.translate(deltax,deltay));}

	//center x
	inline int centerx() const
	{
		if (p0.x>=min2(p1.x,p2.x) && p0.x<=max2(p1.x,p2.x)) return p0.x;
		if (p1.x>=min2(p0.x,p2.x) && p1.x<=max2(p0.x,p2.x)) return p1.x;
		if (p2.x>=min2(p0.x,p1.x) && p2.x<=max2(p0.x,p1.x)) return p2.x;
		DebugAssert(0);
		return 0;
	}

	//center y
	inline int centery() const
	{
		if (p0.y>=min2(p1.y,p2.y) && p0.y<=max2(p1.y,p2.y)) return p0.y;
		if (p1.y>=min2(p0.y,p2.y) && p1.y<=max2(p0.y,p2.y)) return p1.y;
		if (p2.y>=min2(p0.y,p1.y) && p2.y<=max2(p0.y,p1.y)) return p2.y;
		DebugAssert(0);
		return 0;
	}

}; //end class


#endif //_TRIANGLE2I_H__


