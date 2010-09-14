#ifndef _VEC_H__
#define _VEC_H__

#include <xge/xge.h>
#include <xge/archive.h>

//predeclaration
class Mat4f;
class Matf;

//========================================================================
//! 2D integer vector
//========================================================================
class XGE_API Vec2i
{
public:
	int x,y;

	//constructors
	inline          Vec2i(                           ) {this->x=0;this->y=0;}
	inline          Vec2i(const Vec2i& src           ) {this->x=src.x;this->y=src.y;}
	inline explicit Vec2i(int x,int y                ) {this->x=x;this->y=y;}
	inline explicit Vec2i(const std::vector<int>& src) {DebugAssert (src.size()==2);this->x=src[0];this->y=src[1];}

	//assignment operators
	inline Vec2i& operator=(const Vec2i& src){this->x=src.x;this->y=src.y;return *this;}
	inline Vec2i& operator=(const std::vector<int>& src) {DebugAssert (src.size()==2);this->x=src[0];this->y=src[1];return *this;}

	//scale & translate
	inline Vec2i scale    (int scalex,int scaley) const {return Vec2i(this->x*scalex,this->y*scaley); }
	inline Vec2i translate(int deltax,int deltay) const {return Vec2i(this->x+deltax,this->y+deltay);}

	//equality operator
	inline bool  operator==(const Vec2i&  b) const{const Vec2i& a=*this;return a.x==b.x && a.y==b.y;}

	//get/set
	inline int        get(int i          ){DebugAssert (i>=0 && i<=1);if (!i  ) return x;else      return y;}
	inline void       set(int i,int value){DebugAssert (i>=0 && i<=1);if (!i  )  x=value;else      y=value;}

	//operator []
	inline int&       operator[](int i)   {DebugAssert (i>=0 && i<=1);if (!i  ) return x;else      return y;}
	inline const int& operator[](int i) const {DebugAssert (i>=0 && i<=1);if (!i  ) return x;else      return y;}
}; 


//========================================================================
//! 2D float vector
//========================================================================
class XGE_API Vec2f
{
	

public:

	//! X position
	float x;
	
	//! Y position
	float y;

	//!default constructor
	/*! 
		@py 
		v=Vec2f();assert v.x==0 and v.y==0
		@endpy
	*/
	inline Vec2f()
	{
		this->x=0;
		this->y=0;
	}

	//! copy constructor
	/*! 
		@py 
		v=Vec2f(Vec2f(1,2)); assert v.x==1 and v.y==2
		@endpy
	*/
	inline Vec2f(const Vec2f& src)
	{
		this->x=src.x;
		this->y=src.y;
	}


	//! constructor from two float
	/*! 
		@py 
		v=Vec2f(1,2);assert v.x==1 and v.y==2
		@endpy
	*/
	inline explicit Vec2f(float x,float y)
	{
		this->x=x;
		this->y=y;
	}

	//! constructor from STL std::vector<float>
	/*! 
		@py 
		assert Vec2f([1,2])==Vec2f(1,2) 
		@endpy
	*/
	inline explicit Vec2f(const std::vector<float>& src)
	{	
		if (src.size()!=2)
			Utils::Error(HERE,"Vec2f::Vec2f(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be 2",(int)src.size());

		this->x=src[0];
		this->y=src[1];
	}


	//! assignment operator
	/*! 
		@py 
		v=Vec2f(1,2)
		v.assign(Vec2f(10,11))
		assert v.x==10 and v.y==11
		@endpy
	*/
	inline Vec2f& operator=(const Vec2f& src)
	{
		this->x=src.x;
		this->y=src.y;
		return *this;
	}


	//! assignment operator from STL std::vector<float>
	/*! 
		@py 
		assert Vec2f().assign([1,2])==Vec2f(1,2)
		@endpy
	*/
	inline Vec2f& operator=(const std::vector<float>& src)
	{
		if (src.size()!=2)
			Utils::Error(HERE,"Vec2f& operator=(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be 2",(int)src.size());

		this->x=src[0];
		this->y=src[1];
		return *this;
	}


	//! test if two vector are strictly the same
	/*! 
		@py 
		assert Vec2f(1,2)==Vec2f(1,2) and Vec2f(1,2)!=Vec2f(3,4)
		@endpy
	*/
	inline bool  operator==(const Vec2f&  b)  const
	{
		const Vec2f& a=*this;
		return a.x==b.x && a.y==b.y;
	}


	//! get a single component
	/*! 
		@py 
		assert Vec2f(1,2).get(0)==1 and Vec2f(1,2).get(1)==2
		@endpy
	*/ 
	inline float get(int i)
	{
		if (i<0 || i>1)
			Utils::Error(HERE,"float Vec2f::get(int i) argument i=%d ouside valid range [0..1]",i);

		if (!i  ) return x;
		else      return y;
	}

	//! set a single component
	/*! 
		@py 
		v=Vec2f(1,2);v.set(1,10);assert v==Vec2f(1,10) 
		@endpy
	*/
	inline void set(int i,float value)
	{
		if (i<0 || i>1)
			Utils::Error(HERE,"void Vec2f::set(int i,float value) argument i=%d ouside valid range [0..1]",i);

		if (!i  ) x=value;
		else      y=value;
	}

	//! access operator by []
	/*! 
		@py 
		v=Vec2f(1,2); assert v[0]==1 and v[1]==2
		@endpy
	*/
	inline float& operator[](int i)
	{
		if (i<0 || i>1)
			Utils::Error(HERE,"float Vec2f::operator[](int i) argument i=%d ouside valid range [0..1]",i);

		if (!i  ) return x;
		else      return y;

	}

	//! access operator by []
	/*! 
		@py 
		v=Vec2f(1,2); assert v[0]==1 and v[1]==2
		@endpy
	*/
	inline const float& operator[](int i) const
	{
		if (i<0 || i>1)
			Utils::Error(HERE,"float Vec2f::operator[](int i) argument i=%d ouside valid range [0..1]",i);

		if (!i  ) return x;
		else      return y;

	}


	//! python str()
	/*! 
		@py 
		assert eval(str(Vec2f(1,2)))==[1,2]
		@endpy
	*/
	std::string str() const
	{
		return Utils::Format("[%f,%f]",x,y);
	}

	//! python repr()
	/*! 
		@py 
		assert(eval(repr(Vec2f(1,2))))==Vec2f(1,2)
		@endpy
	*/
	std::string repr() const
	{
		return Utils::Format("Vec2f(%f,%f)",x,y);
	}

}; //end class


//========================================================================
//! 3D float vector
//========================================================================
class XGE_API Vec3f
{
public:
	void Write(Archive& ar)
	{
		ar.WriteFloat("x",x);
		ar.WriteFloat("y",y);
		ar.WriteFloat("z",z);
	}

	void Read(Archive& ar)
	{
		this->x=ar.ReadFloat("x");
		this->y=ar.ReadFloat("y");
		this->z=ar.ReadFloat("z");
	}

public:

	//! X position
	float x;
	
	//! Y position 
	float y;
	
	//! Z position
	float z;

	//! default constructor
	/*! 
		@py 
		v=Vec3f(); assert v.x==0 and v.y==0 and v.z==0
		@endpy
	*/
	inline Vec3f() :x(0),y(0),z(0) 
		{}
	
	//! copy constructor
	inline Vec3f(const Vec3f& v) :x(v.x),y(v.y),z(v.z) 
		{}


	//! constructor from 2 or 3 C floats
	/*! 
		@py 
		v=Vec3f(1,2); assert v.x==1 and v.y==2 and v.z==0
		v=Vec3f(1,2,3); assert v.x==1 and v.y==2 and v.z==3
		@endpy
	*/
	inline explicit Vec3f(float _x,float _y,float _z=0) : x(_x),y(_y),z(_z) 
		{}


	//! constructor from C array of floats
	inline explicit Vec3f(const float v[3]) :x(v[0]),y(v[1]),z(v[2])
		{}

	//! constructor from STL std::vector<float>
	/*! 
		@py 
		assert Vec3f([1,2,3])==Vec3f(1,2,3) 
		@endpy
	*/
	inline explicit Vec3f(const std::vector<float>& src) 
	{
		if (src.size()!=3)
			Utils::Error(HERE,"Vec3f::Vec3f(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be 3",(int)src.size());

		x = src[0];
		y = src[1];
		z = src[2];
	}

	//! constructor from C double array
	inline explicit Vec3f(const double v[3]) :x((float)v[0]),y((float)v[1]),z((float)v[2])
		{}

	//! assignment operator
	/*! 
		@py 
		assert Vec3f().assign(Vec3f(1,2,3))==Vec3f(1,2,3)
		@endpy
	*/
	inline Vec3f& operator=(const Vec3f& v)   
		{x=v.x;y=v.y;z=v.z;return *this;}


	//! assignment operator using STL std::vector<float>
	/*! 
		@py 
		assert Vec3f().assign([1,2,3])==Vec3f(1,2,3)
		@endpy
	*/
	inline Vec3f& operator=(const std::vector<float>& src)
	{
		if (src.size()!=3)
			Utils::Error(HERE,"Vec3f& operator=(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be 3",(int)src.size());

		this->x=src[0];
		this->y=src[1];
		this->z=src[2];
		return *this;
	}


	//! store in C float array
	inline void store(float* dst) 
	{
		dst[0]=x;
		dst[1]=y;
		dst[2]=z;
	}

	//! squared module 
	/*! 
		@py 
		assert(Vec3f(1,2,3).module2()==14)
		@endpy
	*/
	inline float module2() const 
		{return (float)(x*x+y*y+z*z);}
	
	//! module
	/*! 
		@py 
		assert(Vec3f(2,0,0).module()==2)
		@endpy
	*/
	inline float module()  const 
		{return (float)sqrt(x*x+y*y+z*z);}

	//! distance between two points
	/*! 
		@py 
		assert(fabs(Vec3f(1,1,1).distance(Vec3f(2,2,2))-sqrt(3))<0.001)
		@endpy
	*/
	inline float distance(const Vec3f p) const 
		{return sqrt((p.x-x)*(p.x-x)+(p.y-y)*(p.y-y)+(p.z-z)*(p.z-z));}
	
	//! normalize a vector (if the normalization fails return the same vector)
	/*! 
		@py 
		assert(fabs(Vec3f(1,1,1).normalize().module()-1)<0.001)
		@endpy
	*/
	inline Vec3f normalize() const 
	{
		float len=module();
		if (!len) len=1;
		return Vec3f(x/len,y/len,z/len);
	}

	//! operator  *=coeff
	/*! 
		@py 
		v=Vec3f(1,2,3);v*=10;assert(v.x==10 and v.y==20 and v.z==30)
		@endpy
	*/
	inline Vec3f& operator*=(float s)   
		{x*=s;y*=s;z*=s;return *this;}

	//! operator /=coeff
	/*! 
		@py 
		v=Vec3f(10,20,30);v/=10;assert(v.x==1 and v.y==2 and v.z==3)
		@endpy
	*/
	inline Vec3f& operator/=(float s)   
		{ReleaseAssert(s);x/=s;y/=s;z/=s;return *this;}
	
	//! operator +=Vec3f
	/*! 
		@py 
		v=Vec3f(10,20,30);v+=Vec3f(1,2,3);assert(v.x==11 and v.y==22 and v.z==33)
		@endpy
	*/
	inline Vec3f& operator+=(const Vec3f& v)  
		{x+=v.x;y+=v.y;z+=v.z;return *this;}
	
	//! operator -=Vec3f
	/*! 
		@py 
		v=Vec3f(11,22,33);v-=Vec3f(1,2,3);assert(v.x==10 and v.y==20 and v.z==30)
		@endpy
	*/
	inline Vec3f& operator-=(const Vec3f& v)  
		{x-=v.x;y-=v.y;z-=v.z;return *this;}
	
	//! cross product
	/*! 
		@py 
		v1=Vec3f(1,0,0);v2=Vec3f(0,1,0);v3=Vec3f(0,0,1)
		assert(v1.cross(v2)==v3 and v1.cross(v2)==v3 and v3.cross(v1)==v2)
		@endpy
	*/
	inline Vec3f cross(const Vec3f& v) const 
	{
		return Vec3f(
			y * v.z - v.y * z, 
			z * v.x - v.z * x, 
			x * v.y - v.x * y
			);
	}

	//! test if this vector is valid (==all numbers are valid)
	/*! 
		@py 
		v=Vec3f(1,1,1)/0
		assert(not v.isValid());
		@endpy
	*/
	inline bool isValid() const
	{
		return 
			   !isnan(this->x) && finite(this->x)
			&& !isnan(this->y) && finite(this->y)
			&& !isnan(this->z) && finite(this->z);
	}

	//! test fuzzy equality (error tolerance)
	/*! 
		@py 
		assert Vec3f(1,2,3).fuzzyEqual(Vec3f(1.0001,2.0001,3.0001),0.001)
		assert not Vec3f(1,2,3).fuzzyEqual(Vec3f(1.0001,2.0001,3.0001),0.00001)
		@endpy
	*/
	inline bool  fuzzyEqual(const Vec3f& v,float Epsilon=0.001f) const 
	{
		return (x > (v.x - Epsilon) && x < (v.x + Epsilon) && 
				y > (v.y - Epsilon) && y < (v.y + Epsilon) &&
				z > (v.z - Epsilon) && z < (v.z + Epsilon));
	};

	//! C conversion operator 
	//inline operator const float*() const
	//	{return &x;}

	//! C conversion operator 
	//inline operator float*()
	//	{return &x;}

	//! operator Vec3f+Vec3f
	/*! 
		@py 
		assert (Vec3f(1,2,3) + Vec3f(10,20,30)) == Vec3f(11,22,33)
		@endpy
	*/
	inline Vec3f  operator+(const Vec3f&  b)  const
	{
		const Vec3f&  a=*this;
		return Vec3f(a.x+b.x,a.y+b.y,a.z+b.z);
	}

	//! operator Vec3f-Vec3f
	/*! 
		@py 
		assert (Vec3f(11,22,33) - Vec3f(1,2,3)) == Vec3f(10,20,30)
		@endpy
	*/
	inline Vec3f  operator-(const Vec3f&  b) const
	{
		const Vec3f&  a=*this;
		return Vec3f(a.x-b.x,a.y-b.y,a.z-b.z);
	}

	//! operator Vec3f * Vec3f (scalar product)
	/*! 
		@py 
		assert (Vec3f(1,1,1) * Vec3f(10,20,30)) == 60
		@endpy
	*/
	inline float operator*(const Vec3f&  b) const
	{
		const Vec3f&  a=*this;
		return a.x*b.x+a.y*b.y+a.z*b.z;
	}

	//! operator Vec3f * scalar
	/*! 
		@py 
		assert (Vec3f(1,2,3) * 10) == Vec3f(10,20,30)
		@endpy
	*/
	inline  Vec3f  operator*(float s) const
	{
		const Vec3f&  v=*this;
		return Vec3f(v.x*s,v.y*s,v.z*s);
	}

	//! friend operator scalar * Vec3f
	#ifndef SWIG
	inline friend Vec3f  operator*(float s,const Vec3f&  v) 
		{return Vec3f(v.x*s,v.y*s,v.z*s);}
	#endif

	//!  operator Vec3f / scalar
	/*! 
		@py 
		assert (Vec3f(10,20,30)/10) == Vec3f(1,2,3)
		@endpy
	*/
	inline  Vec3f  operator/(float s) const
	{
		const Vec3f&  v=*this;
		return Vec3f(v.x/s,v.y/s,v.z/s);
	}

	//! test if two vector are strictly the same
	/*! 
		@py 
		assert Vec3f(1,2,3) == Vec3f(1,2,3)
		@endpy
	*/
	inline  bool  operator==(const Vec3f&  b)  const
	{
		const Vec3f& a=*this;
		return a.x==b.x && a.y==b.y && a.z==b.z;
	}
	
	//! test if two vector are strictly different
	/*! 
		@py 
		assert Vec3f(1,2,3) != Vec3f(1,2,4)
		@endpy
	*/
	inline bool  operator!=(const Vec3f&  b)const
	{
		const Vec3f& a=*this;
		return a.x!=b.x || a.y!=b.y || a.z!=b.z;
	}

	//! returns the vector made of fabs() of each coordinate
	/*! 
		@py 
		assert Vec3f(-1,-2,3).Abs()==Vec3f(1,2,3)
		@endpy
	*/
	inline Vec3f Abs() const
		{return Vec3f(fabs(x),fabs(y),fabs(z));}

	//! min for each component 
	/*! 
		@py 
		assert Vec3f(1,2,3).Min(Vec3f(-1,3,-4))==Vec3f(-1,2,-4)
		@endpy
	*/
	inline Vec3f Min(const Vec3f& v) const
		{return Vec3f(min2(x,v.x),min2(y,v.y),min2(z,v.z));}

	//! max for each component
	/*! 
		@py 
		assert Vec3f(1,2,3).Max(Vec3f(-1,3,-4))==Vec3f(1,3,3)
		@endpy
	*/
	inline Vec3f Max(const Vec3f& v) const
		{return Vec3f(max2(x,v.x),max2(y,v.y),max2(z,v.z));}

	//! max component
	/*! 
		@py 
		assert Vec3f(1,2,3).Max()==3
		@endpy
	*/
	inline float Max()
		{return max3(x, y, z);}

	//! min component
	/*! 
		@py 
		assert Vec3f(1,2,3).Min()==1
		@endpy
	*/
	inline float Min()
		{return min3(x, y, z);}

	//! max index
	/*! 
		@py 
		assert Vec3f(1,2,3).maxidx()==2
		@endpy
	*/
    inline int maxidx() const 
	{
             if (x>=y && x>=z) return 0;
        else if (y>=z        ) return 1;
        else                   return 2;
	}

	//! min index
	/*! 
		@py 
		assert Vec3f(1,2,3).minidx()==0
		@endpy
	*/
    inline int minidx() const 
	{
             if (x<=y && x<=z) return 0;
        else if (y<=z        ) return 1;
        else                   return 2;
	}


	//! get a single component
	/*! 
		@py 
		v=Vec3f(1,2,3);v.set(1,10);assert v.get(0)==1 and v.get(1)==10 and v.get(2)==3
		@endpy
	*/
	inline float get(int i)
	{
		if (i<0 || i>2)
			Utils::Error(HERE,"float Vec3f::get(int i) argument i=%d ouside valid range [0..2]",i);

		if      (!i  ) return x;
		else if (i==1) return y;
		else           return z;
	}

	//! set a single component
	/*! 
		@py 
		v=Vec3f(1,2,3);v.set(1,10);assert v.get(0)==1 and v.get(1)==10 and v.get(2)==3
		@endpy
	*/
	inline void set(int i,float value)
	{
		if (i<0 || i>2)
			Utils::Error(HERE,"void Vec3f::set(int i,float value) argument i=%d ouside valid range [0..2]",i);

		if      (!i  ) x=value;
		else if (i==1) y=value;
		else           z=value;
	}

	


	//! access operator by []
	/*! 
		@py 
		assert Vec3f(1,2,3)[0]==1 and Vec3f(1,2,3)[1]==2 and Vec3f(1,2,3)[2]==3
		@endpy
	*/
	inline float& operator[](int i) 
	{
		if (i<0 || i>2)
			Utils::Error(HERE,"float& Vec3f::operator[](int i) argument i=%d ouside valid range [0..2]",i);

		if (!i  ) return x;
		if (i==1) return y;
		return z;
	}

	inline const float& operator[](int i)  const
	{
		if (i<0 || i>2)
			Utils::Error(HERE,"float& Vec3f::operator[](int i) argument i=%d ouside valid range [0..2]",i);

		if (!i  ) return x;
		if (i==1) return y;
		return z;
	}

	//! rotate a vector of a certain angle along axis (which passes through Zero), angle should be in radiant
	/*! 
		@py 
		assert Vec3f(4,0,0).rotate(Vec3f(0,0,1),pi/2).fuzzyEqual(Vec3f(0,1,0))
		@endpy
	*/
	inline Vec3f rotate(const Vec3f& axis,float angle,bool bNormalize=true)
	{
		float c = (float)cos(angle);
		float s = (float)sin(angle);

		Vec3f ret( 
			(axis.x*axis.x*(1-c) +        c)	* x+(axis.x*axis.y*(1-c) - axis.z*s)	* y+(axis.x*axis.z*(1-c) + axis.y*s)	* z,
			(axis.y*axis.x*(1-c) + axis.z*s)	* x+(axis.y*axis.y*(1-c) +      c  )	* y+(axis.y*axis.z*(1-c) - axis.x*s)	* z,
			(axis.x*axis.z*(1-c) - axis.y*s)	* x+(axis.y*axis.z*(1-c) + axis.x*s)	* y+(axis.z*axis.z*(1-c) +       c  )	* z);

		if (bNormalize)  ret=ret.normalize();

		return ret;
	}


	//! python repr()
	/*! 
		@py 
		v=Vec3f(1,2,3);assert(eval(repr(v)))==Vec3f(1,2,3)
		@endpy
	*/
	inline std::string repr() const
	{
		return Utils::Format("Vec3f(%e,%e,%e)",x,y,z);
	}

	//! python str()
	/*! 
		@py 
		v=Vec3f(1,2,3);assert eval(str(v))==[1,2,3]
		@endpy
	*/
	inline std::string str() const
	{
		return Utils::Format("[%f,%f,%f]",x,y,z);
	}


	//SelfTest
	static int SelfTest();

}; //end Vec3f



//========================================================================
//! 4D float vector
//========================================================================
class XGE_API Vec4f
{

public:

	//! X position 
	float x;
	
	//! Y position 
	float y;
	
	//! Z position 
	float z;
	
	//! W position 
	float w;

	//! default constructor
	/*! 
		@py 
		v=Vec4f()
		assert v.x==0 and v.y==0 and v.z==0 and v.w==0
		@endpy
	*/
	inline Vec4f() :x(0),y(0),z(0),w(0) 
		{}

	//! copy constructor
	/*! 
		@py 
		v=Vec4f(Vec4f(1,2,3,4))
		assert v.x==1 and v.y==2 and v.z==3 and v.w==4
		@endpy
	*/
	inline Vec4f(const Vec4f& v)  :x(v.x),y(v.y),z(v.z),w(v.w) 
		{}

	//! constructor from 4 floats
	/*! 
		@py 
		v=Vec4f(1,2,3,4)
		assert v.x==1 and v.y==2 and v.z==3 and v.w==4
		@endpy
	*/
	inline explicit Vec4f(float _x,float _y,float  _z,float _w) : x(_x),y(_y),z(_z),w(_w) 
		{}

	//! constructor from C float array 
	inline explicit Vec4f(const float v[4]) :x(v[0]),y(v[1]),z(v[2]),w(v[3]) 
		{}

	//! constructor from an C double array
	inline explicit Vec4f(const double v[4]) :x((float)v[0]),y((float)v[1]),z((float)v[2]),w((float)v[3]) 
		{}

	//! constructor from std::vector<float>
	/*! 
		@py 
		assert Vec4f([1,2,3,4])==Vec4f(1,2,3,4) 
		@endpy
	*/
	inline explicit Vec4f(const std::vector<float>& src)
	{
		if (src.size()!=4)
			Utils::Error(HERE,"Vec4f::Vec4f(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be 4",(int)src.size());

		x=src[0];
		y=src[1];
		z=src[2];
		w=src[3];
	}


	//! C conversion operator
	//inline  operator const float*() const
	//	{return &x;}

	//! C conversion operator
	//inline  operator float*()
	//	{return &x;}

	//! squared module
	/*! 
		@py
		assert(Vec4f(1,2,3,4).module2()==30)
		@endpy
	*/
	inline float module2() const 
		{return (float)    (x*x+y*y+z*z+w*w);}

	//! module
	/*! 
		@py 
		assert(Vec4f(2,2,2,2).module()==4)
		@endpy
	*/
	inline float module () const 
		{return (float)sqrt(x*x+y*y+z*z+w*w);}

	//! normalize
	/*! 
		@py 
		assert(fabs(Vec4f(1,1,1,1).normalize().module()-1)<0.001)
		@endpy
	*/
	inline Vec4f normalize() const 
	{
		float len=module();
		if (!len) len=1;
		return Vec4f(x/len,y/len,z/len,w/len);
	}



	//! get single component
	/*! 
		@py 
		v=Vec4f(1,2,3,4);v.set(1,10);assert v.get(0)==1 and v.get(1)==10 and v.get(2)==3  and v.get(3)==4
		@endpy
	*/
	inline float get(int i)
	{
		if (i<0 || i>3)
			Utils::Error(HERE,"float Vec4f::get(int i) argument i=%d ouside valid range [0..3]",i);

		if      (!i  ) return x;
		else if (i==1) return y;
		else if (i==2) return z;
		else           return w;
	}



	//! set single component
	/*! 
		@py 
		v=Vec4f(1,2,3,4);v.set(1,10);assert v.get(0)==1 and v.get(1)==10 and v.get(2)==3  and v.get(3)==4
		@endpy
	*/
	inline void set(int i,float value)
	{
		if (i<0 || i>3)
			Utils::Error(HERE,"void Vec4f::set(int i,float value) argument i=%d ouside valid range [0..3]",i);

		if      (!i  ) x=value;
		else if (i==1) y=value;
		else if (i==2) z=value;
		else           w=value;
	}


	//! access operator by []
	/*! 
		@py 
		v=Vec4f(1,2,3,4);assert v[0]==1 and v[1]==2 and v[2]==3  and v[3]==4
		@endpy
	*/
	inline float& operator[](int i)
	{
		if (i<0 || i>3)
			Utils::Error(HERE,"float& Vec4f::operator[](int i) argument i=%d ouside valid range [0..3]",i);

		if      (!i)   return x;
		else if (i==1) return y;
		else if (i==2) return z;
		else           return w;
	}

	//! access operator by []
	/*! 
		@py 
		v=Vec4f(1,2,3,4);assert v[0]==1 and v[1]==2 and v[2]==3  and v[3]==4
		@endpy
	*/
	inline const float& operator[](int i) const
	{
		if (i<0 || i>3)
			Utils::Error(HERE,"const float& Vec4f::operator[](int i) const argument i=%d ouside valid range [0..3]",i);

		if      (!i)   return x;
		else if (i==1) return y;
		else if (i==2) return z;
		else           return w;
	}


	//! operator *=coeff
	/*! 
		@py 
		v=Vec4f(1,2,3,4);v*=10;assert(v.x==10 and v.y==20 and v.z==30 and v.w==40)
		@endpy
	*/
	inline Vec4f& operator*=(float s)
		{x*=s;y*=s;z*=s;w*=s;return *this;}

	//! operator /=coeff
	/*! 
		@py 
		v=Vec4f(10,20,30,40);v/=10;assert(v.x==1 and v.y==2 and v.z==3 and v.w==4)
		@endpy
	*/
	inline Vec4f& operator/=(float s) 
		{DebugAssert(s);x/=s;y/=s;z/=s;w/=s;return *this;}

	//! operator +=Vec4f
	/*! 
		@py 
		v=Vec4f(10,20,30,40);v+=Vec4f(1,2,3,4);assert(v.x==11 and v.y==22 and v.z==33 and v.w==44)
		@endpy
	*/
	inline Vec4f& operator+=(const Vec4f& v)
		{x+=v.x;y+=v.y;z+=v.z;w+=v.w;return *this;}

	//! operator -=Vec4f
	/*! 
		@py 
		v=Vec4f(11,22,33,44);v-=Vec4f(1,2,3,4);assert(v.x==10 and v.y==20 and v.z==30 and v.w==40)
		@endpy
	*/
	inline Vec4f& operator-=(const Vec4f& v)
		{x-=v.x;y-=v.y;z-=v.z;w-=v.w;return *this;}

	//! assignment operator
	/*! 
		@py 
		v=Vec4f();v.assign(Vec4f(1,2,3,4));assert(v==Vec4f(1,2,3,4))
		@endpy
	*/
	inline Vec4f& operator=(const Vec4f& v)
		{x=v.x;y=v.y;z=v.z;w=v.w;return *this;}

	//! assignment operator from std::vector<float>
	/*! 
		@py 
		assert Vec4f().assign([1,2,3,4])==Vec4f(1,2,3,4)
		v=Vec4f(1,2,3,4);w=v.assign([5,6,7,8]); assert v.x==5 and v.y==6 and v.z==7 and v.w==8
		@endpy
	*/
	inline Vec4f& operator=(const std::vector<float>& src)
	{
		if (src.size()!=4)
			Utils::Error(HERE,"Vec4f& operator=(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be 4",(int)src.size());

		this->x=src[0];
		this->y=src[1];
		this->z=src[2];
		this->w=src[3];
		return *this;
	}


	//! min for each component
	/*! 
		@py 
		assert Vec4f(1,2,3,4).Min(Vec4f(-1,3,-4,5))==Vec4f(-1,2,-4,4)
		@endpy
	*/
	inline Vec4f Min(const Vec4f& v) const
		{return Vec4f(min2(x,v.x),min2(y,v.y),min2(z,v.z),min2(w,v.w));}

	//! max for each component
	/*! 
		@py 
		assert Vec4f(1,2,3,4).Max(Vec4f(-1,3,-4,5))==Vec4f(1,3,3,5)
		@endpy
	*/
	inline Vec4f Max(const Vec4f& v) const
		{return Vec4f(max2(x,v.x),max2(y,v.y),max2(z,v.z),max2(w,v.w));}

	//! operator Vec4f * Vec4f (scalar product)
	/*! 
		@py 
		assert (Vec4f(1,1,1,1) * Vec4f(10,20,30,40)) == 100
		@endpy
	*/
	inline float operator*(const Vec4f& b) const
	{
		const Vec4f& a=*this;
		return a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w;
	}

	//! operator Vec4f + Vec4f
	/*! 
		@py 
		assert (Vec4f(1,2,3,4) + Vec4f(10,20,30,40)) == Vec4f(11,22,33,44)
		@endpy
	*/
	inline Vec4f  operator+(const Vec4f& b)  const
	{
		const Vec4f& a=*this;
		return Vec4f(a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w);
	}

	//! operator Vec4f - Vec4f
	/*! 
		@py 
		assert (Vec4f(11,22,33,44) - Vec4f(1,2,3,4)) == Vec4f(10,20,30,40)
		@endpy
	*/
	 inline Vec4f operator-(const Vec4f& b) const
	{
		const Vec4f& a=*this;
		return Vec4f(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w);
	}
	
	//! friend operator coeff * vector
#ifndef SWIG
	friend inline Vec4f  operator*  (const float coeff,const Vec4f& a) 
		{return Vec4f(coeff*a.x,coeff*a.y,coeff*a.z,coeff*a.w);}
#endif
	
	//! operator Vec4f * coeff
	/*! 
		@py 
		assert (Vec4f(1,2,3,4) * 10) == Vec4f(10,20,30,40)
		@endpy
	*/
	 inline Vec4f  operator* (const float coeff)  const
	{
		const Vec4f& a=*this;
		return Vec4f(coeff*a.x,coeff*a.y,coeff*a.z,coeff*a.w);
	}

	//! operator Vec4f / coeff
	/*! 
		@py 
		assert (Vec4f(10,20,30,40)/10) == Vec4f(1,2,3,4)
		@endpy
	*/
	inline Vec4f operator/(const float coeff) const
	{
		const Vec4f& a=*this;
		DebugAssert(coeff);
		return Vec4f(a.x/coeff,a.y/coeff,a.z/coeff,a.w/coeff);
	}
	
	//! test strict equality
	/*! 
		@py 
		assert(Vec4f(1,2,3,4) == Vec4f(1,2,3,4))
		@endpy
	*/
	inline bool operator==(const Vec4f& b) const
	{
		const Vec4f& a=*this;
		return a.x==b.x && a.y==b.y && a.z==b.z && a.w==b.w;
	}
	
	//! test not strict equality
	/*! 
		@py 
		assert Vec4f(1,2,3,4) != Vec4f(1,2,4,4)
		@endpy
	*/
	inline bool operator!= (const Vec4f& b) const
	{
		const Vec4f& a=*this;
		return a.x!=b.x || a.y!=b.y || a.z!=b.z || a.w!=b.w;
	}

	//! friend operator vector * matrix
	/*! 
		@py 
		m=Mat4f()
		assert Vec4f(1,0,0,0)*m==m.row(0) and Vec4f(0,1,0,0)*m==m.row(1) and Vec4f(0,0,1,0)*m==m.row(2) and Vec4f(0,0,0,1)*m==m.row(3)  
		@endpy
	*/
	Vec4f operator*(const Mat4f& m) const;

	//! test fuzzy equality
	/*! 
		@py 
		assert Vec4f(1,2,3,4).fuzzyEqual(Vec4f(1.0001,2.0001,3.0001,4.0001),0.001)
		assert not Vec4f(1,2,3,4).fuzzyEqual(Vec4f(1.0001,2.0001,3.0001,4.0001),0.00001)
		@endpy
	*/
	inline bool  fuzzyEqual(const Vec4f& v,float Epsilon=0.001f) const {
		return (x > (v.x - Epsilon) && x < (v.x + Epsilon) && 
				y > (v.y - Epsilon) && y < (v.y + Epsilon) &&
				z > (v.z - Epsilon) && z < (v.z + Epsilon) &&
				w > (v.w - Epsilon) && w < (v.w + Epsilon));
	};

	//! python repr()
	/*! 
		@py 
		v=Vec4f(1,2,3,4);assert(eval(repr(v)))==Vec4f(1,2,3,4)
		@endpy
	*/
	inline std::string repr() const
	{
		return Utils::Format("Vec4f(%e,%e,%e,%e)",x,y,z,w);
	}

	//! python str()
	/*! 
		@py 
		v=Vec4f(1,2,3,4);assert eval(str(v))==[1,2,3,4]
		@endpy
	*/
	inline std::string str() const
	{
		return Utils::Format("[%f,%f,%f,%f]",x,y,z,w);
	}


	//self test
	static int SelfTest();


}; //end class Vec4f



//========================================================================
//! n-dim float vector
/*!
	the homogeneous term is in first position  (plasm notation)
	Example: when working with n==3, this class will allocate a 4 float vector where the first position is 1.0 for points, and 0.0 for vectors
*/
//========================================================================

class XGE_API Vecf
{
	
public:

	//! vector coordinates, first component is homo
	/*!
		if dim==3 it means that the memory allocated is sizeof(float)*(3+1) to handle the first homogeneous component
	*/
	float* mem; 

	//! dimension
	int dim;

	//!default constructor (vector 0-dim) 
	/*! 
		@py 
		v=Vecf()
		assert v.dim==0 and v[0]==0
		@endpy
	*/
	inline Vecf()
	{
		this->dim=0;
		this->mem=(float*)MemPool::getSingleton()->calloc(this->dim+1,sizeof(float));
	}

	//! copy constructor
	/*! 
		@py 
		v=Vecf(Vecf([1,2,3]))
		assert v==Vecf(1,2,3)
		@endpy
	*/
	inline Vecf(const Vecf& v)  
	{
		this->dim=v.dim;
		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1));
		memcpy(this->mem,v.mem,sizeof(float)*(dim+1));
	}


	//! constructor from STL std::vector<float>
	/*! 
		@py 
		v=Vecf([1,2,3])
		assert v==Vecf(1,2,3)
		@endpy
	*/
	inline explicit Vecf(const std::vector<float>& src)
	{
		if (!src.size())
			Utils::Error(HERE,"Vecf::Vecf(const std::vector<float>& src) invalid argument src, src.size()=%d but it's size must be greater than 0",(int)src.size());

		this->dim=(int)(src.size()-1);
		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(this->dim+1));
		memcpy(this->mem,&src[0],sizeof(float)*(this->dim+1));
	}



	//! constructor of a vector in dim-D, all components to zero
	/*! 
		@py 
		v=Vecf(3)
		assert v.dim==3 and v[0]==0 and v[1]==0 and v[2]==0
		@endpy
	*/
	inline explicit Vecf(int dim) 
	{
		this->dim=dim;
		this->mem=(float*)MemPool::getSingleton()->calloc(dim+1,sizeof(float));
	}

	//! constructor of dim-D vector, first homo components and other values
	inline explicit Vecf(int dim,float first,const float* others) 
	{
		this->dim=dim;
		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1));
		this->mem[0]=first;
		memcpy(&(this->mem[1]),others,sizeof(float)*dim);
	}

	//! constructor of dim-D vector from C float array
	inline explicit Vecf(int dim,const float* src)
	{
		this->dim=dim;
		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1));
		memcpy(this->mem,src,sizeof(float)*(dim+1));
	}

	//! constructor of dim-D vector from C double array
	inline explicit Vecf(int dim,const double* src)
	{
		this->dim=dim;
		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1));
		for (int i=0;i<=dim;i++) 
			this->mem[i]=(float)src[i];
	}

	

	//! constructor 2-dim (3 floats)
	/*! 
		@py 
		v=Vecf(10.0,20.0,30.0)
		assert v.dim==2 and v[0]==10 and v[1]==20 and v[2]==30 
		@endpy
	*/
	inline explicit Vecf(float a0,float a1,float a2)
	{
		this->dim=2;
		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1));
		this->mem[0]=a0;
		this->mem[1]=a1;
		this->mem[2]=a2;
	}

	//! constructor 3-dim (4 floats)
	/*! 
		@py 
		v=Vecf(10.0,20.0,30.0,40.0)
		assert v.dim==3 and v[0]==10 and v[1]==20 and v[2]==30 and v[3]==40.0
		@endpy
	*/
	inline explicit Vecf(float a0,float a1,float a2,float a3)
	{
		this->dim=3;
		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1));
		this->mem[0]=a0;
		this->mem[1]=a1;
		this->mem[2]=a2;
		this->mem[3]=a3;
	}

	//! constructor  4-dim (5 floats)
	/*! 
		@py 
		assert Vecf(1,2,3,4,5)==Vecf([1,2,3,4,5])
		@endpy
	*/
	inline explicit Vecf(float a0,float a1,float a2,float a3,float a4)
	{
		this->dim=4;
		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1));
		this->mem[0]=a0;
		this->mem[1]=a1;
		this->mem[2]=a2;
		this->mem[3]=a3;
		this->mem[4]=a4;
	}

	//! constructor 5-dim (6 floats)
	/*! 
		@py 
		assert Vecf(1,2,3,4,5,6)==Vecf([1,2,3,4,5,6])
		@endpy
	*/
	inline explicit Vecf(float a0,float a1,float a2,float a3,float a4,float a5)
	{
		this->dim=5;
		this->mem=(float*)MemPool::getSingleton()->malloc(6*sizeof(float)*(dim+1));
		this->mem[0]=a0;
		this->mem[1]=a1;
		this->mem[2]=a2;
		this->mem[3]=a3;
		this->mem[4]=a4;
		this->mem[5]=a5;
	}

	//! constructor for 6-dim point (7 floats)
	/*! 
		@py 
		assert Vecf(1,2,3,4,5,6,7)==Vecf([1,2,3,4,5,6,7])		
		@endpy
	*/
	inline explicit Vecf(float a0,float a1,float a2,float a3,float a4,float a5,float a6)

	{
		this->dim=6;
		this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1));
		this->mem[0]=a0;
		this->mem[1]=a1;
		this->mem[2]=a2;
		this->mem[3]=a3;
		this->mem[4]=a4;
		this->mem[5]=a5;
		this->mem[6]=a6;
	}

	//! destructor
	inline ~Vecf()
	{
		MemPool::getSingleton()->free(sizeof(float)*(dim+1),mem);
	}


	//! get a single component
	/*! 
		@py 
		v=Vecf(1,2,3,4)
		v.set(1,10);assert v.get(0)==1 and v.get(1)==10 and v.get(2)==3  and v.get(3)==4
		@endpy
	*/
	inline float get(int i)
	{
		if (i<0 || i>dim)
			Utils::Error(HERE,"float Vec4f::get(int i) argument i=%d ouside valid range [0..%d]",i,dim);

		return mem[i];
	}

	//! set a single component
	/*! 
		@py 
		v=Vecf(1,2,3,4)
		v.set(1,10)
		assert v.get(0)==1 and v.get(1)==10 and v.get(2)==3  and v.get(3)==4
		@endpy
	*/
	inline void set(int i,float value)
	{
		if (i<0 || i>dim)
			Utils::Error(HERE,"void Vec4f::set(int i,float value) argument i=%d ouside valid range [0..%d]",i,dim);

		mem[i]=value;
	}


	//! set all values to <value>
	/*! 
		@py 
		v=Vecf(2)
		v.set(1.0)
		assert v[0]==1 and v[1]==1 and v[2]==1
		@endpy
	*/
	inline void set(float value)
	{
		for (int i=0;i<=dim;i++) 
			this->mem[i]=value;
	}



	//! access component by []
	/*! 
		@py 
		v=Vecf(1,2,3)
		assert v[0]==1 and v[1]==2 and v[2]==3
		@endpy
	*/
	inline float& operator[](int i) 
	{
		if (i<0 || i>dim)
			Utils::Error(HERE,"float Vecf::operator[](int i) argument i=%d ouside valid range [0..%d]",i,dim);

		return mem[i];
	}

	inline const float& operator[](int i) const
	{
		if (i<0 || i>dim)
			Utils::Error(HERE,"const float& Vecf::operator[](int i) const argument i=%d ouside valid range [0..%d]",i,dim);

		return mem[i];
	}

	//! C conversion operator
	//inline  operator const float* () const
	//	{return this->mem;}

	 //! C conversion operator
	//inline  operator float* ()
	//	{return this->mem;}

	//! operator *=coeff
	inline Vecf& operator*=(float s)
	{
		for (int i=0;i<=dim;i++) this->mem[i]*=s;
		return *this;
	}

	//! operator /=coeff
	/*! 
		@py 
		v=Vecf(10,20,30)
		v/=10
		assert v==Vecf(1,2,3)
		@endpy
	*/
	inline Vecf& operator/=(float s) 
	{
		for (int i=0;i<=dim;i++) this->mem[i]/=s;
		return *this;
	}

	//! operator +=Vecf
	/*! 
		@py 
		v=Vecf([1.0,2.0])
		v+=Vecf([3.0,4.0])
		assert v==Vecf([4.0,6.0])
		@endpy
	*/
	inline Vecf& operator+=(const Vecf& v)
	{
		DebugAssert(this->dim==v.dim);
		for (int i=0;i<=dim;i++) this->mem[i]+=v.mem[i];
		return *this;
	}

	//! operator -=Vecf
	/*! 
		@py 
		v=Vecf([1.0,2.0])
		v-=Vecf([3.0,4.0])
		assert v==Vecf([-2.0,-2.0])
		@endpy
	*/
	inline Vecf& operator-=(const Vecf& v)
	{
		DebugAssert(this->dim==v.dim);
		for (int i=0;i<=dim;i++) this->mem[i]-=v.mem[i];
		return *this;
	}

	//! assignment operator
	/*! 
		@py 
		v=Vecf()
		v.assign(Vecf([1,2]))
		assert v.dim==1 and v[0]==1 and v[1]==2
		@endpy
	*/
	inline Vecf& operator=(const Vecf& v)
	{
		if (&v==this) return *this;
			
		if (this->dim==v.dim)
		{
			memcpy(this->mem,v.mem,sizeof(float)*(dim+1));
		}
		else
		{
			//need a reallocation
			MemPool::getSingleton()->free(sizeof(float)*(dim+1),mem);
			this->dim=v.dim;
			this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(dim+1));
			memcpy(this->mem,v.mem,sizeof(float)*(dim+1));
		}
		return *this;
	}

	//!assignment operator
	/*! 
		@py 
		v=Vecf()
		v.assign([1,2,3,4])
		assert v==Vecf([1,2,3,4])
		v=Vecf([1,2,3,4])
		v.assign([5,6,7,8])
		assert v[0]==5 and v[1]==6 and v[2]==7 and v[3]==8
		@endpy
	*/
	inline Vecf& operator=(const std::vector<float>& src)
	{
		if (!src.size())
			Utils::Error(HERE,"Vecf& operator=(const std::vector<float>& v) invalid argument src, src.size()=%d but it's size must be greater than 0",(int)src.size());

		if ((this->dim+1)==src.size())
		{
			memcpy(this->mem,&src[0],sizeof(float)*(this->dim+1));
		}
		else
		{
			//need a reallocation
			MemPool::getSingleton()->free(sizeof(float)*(this->dim+1),mem);
			this->dim=(int)src.size()-1;
			this->mem=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(this->dim+1));
			memcpy(this->mem,&src[0],sizeof(float)*(this->dim+1));
		}

		return *this;
	}

	//! operator Vecf*Vecf (dot product)
	/*! 
		@py 
		assert Vecf([1,2]) * Vecf([3,4]) == 11
		@endpy
	*/
	inline float operator*(const Vecf& b) const 
	{
		const Vecf& a=*this;
		DebugAssert(a.dim==b.dim);
		int dim=a.dim;
		float ret=0;
		for (int i=0;i<=dim;i++) ret+=a.mem[i]*b.mem[i];
		return ret;
	}

	//! operator Vecf + Vecf
	/*! 
		@py 
		assert (Vecf([1.0,2.0]) + Vecf([3.0,4.0]))==Vecf([4.0,6.0])
		@endpy
	*/
	inline Vecf operator+(const Vecf& b) const
	{
		const Vecf& a=*this;
		Vecf ret(a);ret+=b;
		return ret;
	}
	
	//! operator Vecf - Vecf
	/*! 
		@py 
		assert Vecf([1.0,2.0]) - Vecf([3.0,4.0])==Vecf([-2.0,-2.0])
		@endpy
	*/
	inline Vecf operator-(const Vecf& b) const
	{
		const Vecf& a=*this;
		Vecf ret(a);ret-=b ;return ret;
	}
	
	//! friend operator coeff * Vecf
	#ifndef SWIG
	friend inline Vecf operator*(const float coeff,const Vecf& a) 
		{Vecf ret(a);ret*=coeff;return ret;}
	#endif
	
	//! operator Vecf * coeff
	/*! 
		@py 
		assert Vecf([1.0,2.0])*10==Vecf([10.0,20.0])
		@endpy
	*/
	inline Vecf operator*(const float coeff) const
	{
		const Vecf& a=*this;
		Vecf ret(a);ret*=coeff;return ret;
	}
	
	//! operator Vecf / coeff
	/*! 
		@py 
		assert Vecf([10.0,20.0])/10==Vecf([1.0,2.0])
		@endpy
	*/
	inline Vecf operator/(const float coeff) const
	{
		const Vecf& a=*this;
		Vecf ret(a);ret/=coeff;return ret;
	}

	//! test strict equality
	/*! 
		@py 
		assert Vecf([1,2,3,4])==Vecf([1,2,3,4]) 
		@endpy
	*/
	inline bool operator==(const Vecf& b) const
	{
		const Vecf& a=*this;
		return a.dim==b.dim && !memcmp(a.mem,b.mem,sizeof(float)*(a.dim+1));
	}
	
	//! test strict disequality
	/*! 
		@py 
		assert Vecf([1,2,3,4])!=Vecf([1,2,3,5]) 
		@endpy
	*/
	inline bool operator!=(const Vecf& b)  const  
	{
		const Vecf& a=*this;
		return a.dim!=b.dim ||  memcmp(a.mem,b.mem,sizeof(float)*(a.dim+1));
	}

	//! min for each component 
	/*! 
		@py 
		assert Vecf([1,2,3]).Min(Vecf([-1,3,-4]))==Vecf([-1,2,-4])
		@endpy
	*/
	inline Vecf Min(const Vecf& v) const
	{
		DebugAssert(v.dim==this->dim);
		Vecf ret(dim);
		for (int i=0;i<=dim;i++) 
			ret.mem[i]=min2((*this)[i],v[i]);
		return ret;
	}

	//! max for each component 
	/*! 
		@py 
		assert Vecf([1,2,3]).Max(Vecf([-1,3,-4]))==Vecf([1,3,3])
		@endpy
	*/
	inline Vecf Max(const Vecf& v) const
	{
		DebugAssert(v.dim==this->dim);
		Vecf ret(dim);
		for (int i=0;i<=dim;i++) 
			ret.mem[i]=max2((*this)[i],v[i]);
		return ret;
	}

	//! squared module
	/*! 
		@py 
		assert(Vecf([1,2,3,4]).module2()==30)
		@endpy
	*/
	inline float module2() const
	{
		float acc=0.0f;
		for (int k=0;k<=dim;k++) acc+=this->mem[k]*this->mem[k];
		return acc;
	}

	//! module
	/*! 
		@py 
		assert(Vecf([2,2,2,2]).module()==4)
		@endpy
	*/
	inline float module() const
	{
		return sqrt(module2());
	}

	//! normalize
	/*! 
		@py 
		assert(fabs(Vecf([1,1,1,1]).normalize().module()-1)<0.001)
		@endpy
	*/
	inline Vecf normalize() const
	{
		float m=module();
		if (!m || isnan(m) || !finite(m)) m=1;
		return Vecf(*this)/m;
	}

	//! test if fuzzy equal
	/*! 
		@py 
		assert Vecf([1,2,3,4]).fuzzyEqual(Vecf([1.0001,2.0001,3.0001,4.0001]),0.001)
		assert not Vecf(1,2,3,4).fuzzyEqual(Vecf([1.0001,2.0001,3.0001,4.0001]),0.00001)
		@endpy
	*/
	inline bool fuzzyEqual(const Vecf& v,float Epsilon=0.001f) const 
	{
		bool equals=(dim==v.dim);
		if (!equals) return false;
		for (int i=0;equals && i<=dim;i++) 
			equals=(mem[i] > (v.mem[i] - Epsilon)) && (mem[i] < (v.mem[i] + Epsilon));
		return equals;
	};

	//! permutation
	inline Vecf permutate(int n,const int* perm) const
	{
		DebugAssert(n==this->dim);
		Vecf ret(dim);
		for (int n=0;n<=dim;n++) 
			ret.mem[n]=(*this)[perm[n]];
		return ret;
	}


	//! Vecf*matrix (i.e. plane transformation)
	/*! 
		@py 
		assert (Vecf([1,0]) * Matf([1,0,0,1]))==Vecf([1,0])
		@endpy
	*/
	Vecf operator*(const Matf& m) const;

	//! python repr()
	/*! 
		@py 
		v=Vecf([1,2,3,4])
		assert(eval(repr(v)))==Vecf([1,2,3,4])
		@endpy
	*/
	inline std::string repr() const
	{
		std::string ret="Vecf([";
		for (int i=0;i<=dim;i++)
		{
			if (i) ret+=",";
			ret+=Utils::Format("%e",this->mem[i]);
		}
		ret+="])";
		return ret;
	}

	//! python str()
	/*! 
		@py 
		v=Vecf([1,2,3,4])
		assert eval(str(v))==[1,2,3,4]
		@endpy
	*/
	inline std::string str() const
	{
		std::string ret="[";
		for (int i=0;i<=dim;i++)
		{
			if (i) ret+=",";
			ret+=Utils::Format("%f",this->mem[i]);
		}
		ret+="]";
		return ret;
	}

	//! internal self test
	static int SelfTest();

}; //end class Vecf


#endif //_VEC_H__

