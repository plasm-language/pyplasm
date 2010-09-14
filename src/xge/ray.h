#ifndef _RAY_H__
#define _RAY_H__

#include <xge/xge.h>
#include <xge/vec.h>
#include <xge/plane.h>
#include <xge/box.h>

//==========================================
//! a ray class 3D
//==========================================
class XGE_API Ray3f
{
	

public:

	//! origin of the ray
	Vec3f origin; 

	//! normalized direction
	Vec3f dir   ; 

	//! default constructor origin in Zero, direction +Z
	/*! 
		@py 
		r=Ray3f()
		assert r.origin==Vec3f(0,0,0) and r.dir==Vec3f(0,0,1)
		@endpy
	*/
	inline Ray3f():origin(0,0,0),dir(0,0,1)
		{}

	//! constructor from origin and direction
	/*! 
		@py 
		r=Ray3f(Vec3f(1,2,3),Vec3f(0,1,0))
		assert r.origin==Vec3f(1,2,3) and r.dir==Vec3f(0,1,0)
		@endpy
	*/
	inline explicit Ray3f(Vec3f origin,Vec3f dir)
	{
		this->origin=origin;
		this->dir=dir.normalize(); //should alway be normalized
	}

	//g! et a point of ray at certain distance 
	/*! 
		@py 
		r=Ray3f()
		assert r.getPoint(1)==Vec3f(0,0,1)
		@endpy
	*/
	inline Vec3f getPoint(float t) const
		{return origin+t*dir;}

	//! set origin
	/*! 
		@py 
		r=Ray3f()
		r.setOrigin(Vec3f(1,2,3))
		assert r.origin==Vec3f(1,2,3)
		@endpy
	*/
	inline void setOrigin(const Vec3f& origin)
		{this->origin=origin;}

	//! set direction 
	/*! 
		@py 
		r=Ray3f()
		r.setDir(Vec3f(1,0,0))
		assert r.dir==Vec3f(1,0,0)
		@endpy
	*/
	inline void setDir(const Vec3f& dir,bool bNormalized=false)
		{this->dir=bNormalized?dir:dir.normalize();}

	// !get the intersection of this ray with a plane4 (return a points which is not always good)
	/*! 
		see (see http://www.siggraph.org/education/materials/HyperGraph/raytrace/rayplane_intersection.htm)

		@py 
		r=Ray3f()
		assert r.intersection(Plane4f(0,0,1,-1))==Vec3f(0,0,1)
		@endpy
	*/
	inline Vec3f intersection(const Plane4f& plane4) const
	{
		DebugAssert(fabs(dir.module()-1)<0.0001); //should be normalize
		float t= -plane4.getDistance(origin)/(plane4.getNormal()*dir);
		return origin+t*dir;
	}

	//! test strict equality
	/*! 
		@py 
		r=Ray3f()
		assert r==Ray3f()
		@endpy
	*/
	inline bool operator==(const Ray3f& b) const
	{
		const Ray3f& a=*this;
		return (a.origin==b.origin) && (a.dir==b.dir);
	}
	
	//! test strict disequality
	/*! 
		@py 
		assert Ray3f()!=Ray3f(Vec3f(0,0,0),Vec3f(1,0,0))
		@endpy
	*/
	inline bool operator!=(const Ray3f& b) const
	{
		const Ray3f& a=*this;
		return (a.origin!=b.origin) || (a.dir!=b.dir);
	}


	//! test if a ray on plane Z=0 intersect a line
	float intersectLine(float v0[2],float v1[2]);

	//! triangle ray intersection (return >=0 if intersection found, otherwise -1)
	/*! 
		see http://objectmix.com/graphics/132701-ray-line-segment-intersection-2d.html

		@py 
		assert fabs((Ray3f().intersectTriangle(Vec3f(0,0,1),Vec3f(1.01,0,1),Vec3f(1.01,1.01,1))-1.0)<0.001)
		assert fabs((Ray3f(Vec3f(0,0,0),Vec3f(1,1,1)).intersectTriangle(Vec3f(0,0,1),Vec3f(1.01,0,1),Vec3f(1.01,1.01,1))-sqrt(3.0))<0.001)
		assert Ray3f(Vec3f(0,0,0),Vec3f(1,0,0)).intersectTriangle(Vec3f(0,0,1),Vec3f(1.01,0,1),Vec3f(1.01,1.01,1))==-1
		@endpy
	*/
	float intersectTriangle(const Vec3f& v0,const Vec3f& v1,const Vec3f& v2) const;

	//! Ray-box intersection 
	/*!
		using IEEE numerical properties to ensure that the
		test is both robust and efficient, as described in:

		Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
		"An Efficient and Robust Ray-Box Intersection Algorithm"
		Journal of graphics tools, 10(1):49-54, 2005

		see http://www.cs.utah.edu/~awilliam/box/
	*/
	bool intersectBox(float& tmin,float& tmax,Box3f box) const;


	//! Ray box intersection
	/*! 
		@py 
		b=Box3f(Vec3f(-1,-1,-1),Vec3f(+1,+1,+1))
		I=Ray3f(Vec3f(0,0,0),Vec3f(0,0,1)).intersectBox(b); assert len(I)==2 and fabs(I[0]+1)<1e-4 and fabs(I[1]-1)<1e-4
		I=Ray3f(Vec3f(0,0,0),Vec3f(0,1,0)).intersectBox(b); assert len(I)==2 and fabs(I[0]+1)<1e-4 and fabs(I[1]-1)<1e-4
		I=Ray3f(Vec3f(0,0,0),Vec3f(1,0,0)).intersectBox(b); assert len(I)==2 and fabs(I[0]+1)<1e-4 and fabs(I[1]-1)<1e-4
		@endpy
	*/
	inline std::vector<float> intersectBox(const Box3f& box) const
	{
		std::vector<float> ret;
		float tmin,tmax;
		if (intersectBox(tmin,tmax,box))
		{
			ret.push_back(tmin);
			ret.push_back(tmax);
		}
		return ret;
	}


	//! python str()
	/*! 
		@py 
		assert eval(str(Ray3f()))==[[0,0,0],[0,0,1]]
		@endpy
	*/
	inline std::string str() const
	{
		return Utils::Format("[%s,%s]",origin.str().c_str(),dir.str().c_str());
	}

	//! python repr()
	/*! 
		@py 
		assert eval(repr(Ray3f()))==Ray3f()
		@endpy
	*/
	inline std::string repr() const
	{
		return Utils::Format("Ray3f(%s,%s)",origin.repr().c_str(),dir.repr().c_str());
	}

	//! self test code
	static int SelfTest();


}; //end class ray3



//=====================================================================
//! a ray in n-D
//=====================================================================
class XGE_API Rayf
{
	

public:

	//! origin of the ray
	Vecf origin; 

	//! normalized direction
	Vecf dir   ; 

	//! default constructor
	/*! 
		@py 
		r=Rayf()
		assert r.origin==Vecf([1]) and r.dir==Vecf([0])
		@endpy
	*/
	inline Rayf()
	{
		origin.mem[0]=1;
		dir   .mem[0]=0;
	}

	//! constructor from origin and direction
	/*! 
		@py 
		r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
		assert r.origin==Vecf([1,0,0,0]) and r.dir==Vecf([0,0,0,1])
		@endpy
	*/
	inline explicit Rayf(Vecf origin,Vecf dir)
	{
		DebugAssert(origin.dim==dir.dim);
		DebugAssert(origin[0] && !dir[0]); //first is a point, the second is a direction
		this->origin=origin;
		this->dir=dir.normalize(); //should alway be normalized
	}

	//! get a point of ray at certain distance 
	/*! 
		@py 
		r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
		assert r.getPoint(1)==Vecf([1,0,0,1])
		@endpy
	*/
	inline Vecf getPoint(float t) const
	{
		return origin+t*dir;
	}

	//! set origin 
	/*! 
		@py 
		r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
		r.setOrigin(Vecf(1,1,2,3)); assert r.origin==Vecf([1,1,2,3])
		@endpy
	*/
	inline void setOrigin(const Vecf& origin)
	{
		DebugAssert(origin.dim==this->origin.dim);
		DebugAssert(origin[0]); //must be a point
		this->origin=origin;
	}

	//! set direction
	/*! 
		@py 
		r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
		r.setDir(Vecf(0,1,0,0)); assert r.dir==Vecf([0,1,0,0])
		@endpy
	*/
	inline void setDir(Vecf dir,bool bNormalized=false)
	{
		DebugAssert(dir.dim==this->dir.dim);
		DebugAssert(!dir[0]); //must be a vector
		this->dir=bNormalized?dir:dir.normalize();
	}

	//! get the intersection of this ray with a plane4 (return a points which is not always good)
	/*! 
		see http://www.siggraph.org/education/materials/HyperGraph/raytrace/rayplane_intersection.htm

		@py 
		r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
		r.intersection(Planef(-1,0,0,1))==Vecf(1,0,0,1)
		@endpy
	*/
	inline Vecf intersection(const Planef& planef) const
	{
		DebugAssert(fabs(dir.module()-1)<0.0001); //should be normalize
		float t= -planef.getDistance(origin)/(planef*dir);
		return origin+t*dir;
	}

	//! strict equality
	/*! 
		@py 
		r=Rayf()
		assert r==Rayf()
		@endpy
	*/
	inline bool operator==(const Rayf& b)  const
	{
		const Rayf& a=*this;
		return (a.origin==b.origin) && (a.dir==b.dir);
	}
	
	//! strict disequality
	/*! 
		@py 
		r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
		assert r!=Rayf(Vecf(1,0,0,0),Vecf(0,0,1,0))
		@endpy
	*/
	inline bool operator!=(const Rayf& b) const
	{
		const Rayf& a=*this;
		return (a.origin!=b.origin) || (a.dir!=b.dir);
	}


	//! python str()
	/*! 
		@py 
		r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
		assert eval(str(r))==[[1,0,0,0],[0,0,0,1]]
		@endpy
	*/
	inline std::string str() const
	{
		return Utils::Format("[%s,%s]",origin.str().c_str(),dir.str().c_str());
	}

	//! python repr()
	/*! 
		@py 
		r=Rayf(Vecf(1,0,0,0),Vecf(0,0,0,1))
		assert eval(repr(r))==r
		@endpy
	*/
	inline std::string repr() const
	{
		return Utils::Format("Rayf(%s,%s)",origin.repr().c_str(),dir.repr().c_str());
	}

	//! self test code
	static int SelfTest();


}; //end class 

#endif //_RAY_H__


