#ifndef _PLANE_H__
#define _PLANE_H__

#include <xge/xge.h>
#include <xge/vec.h>


//==============================================
//! a plane to work in 3D
/*!
	it's component are x,y,z,w and the plane is: x*X + y*Y +z*Z + w>=0

	The internal vector must be normalized (x*x+y*y+z*z)=1
*/
//==============================================
class XGE_API Plane4f: public Vec4f
{
	
public:

	//!default constructor (Plane4f z=0)
	/*! 
		@py 
		h=Plane4f()
		assert h[0]==0 and h[1]==0 and h[2]==1 and h[3]==0 and h.get(0)==0 and h.get(1)==0 and h.get(2)==1 and h.get(3)==0
		@endpy
	*/
	inline Plane4f() :Vec4f(0,0,1,0)
		{}

	//! constructor from Vec4f
	inline Plane4f(const Vec4f& src): Vec4f(src)
	{
		normalize();
	}

	//!constructor from 4 floats
	/*! 
		@py 
		h=Plane4f(1,0,0,1)
		assert h[0]==1 and h[1]==0 and h[2]==0 and h[3]==1
		@endpy
	*/
	inline explicit Plane4f(float a,float b,float c,float d) :Vec4f(a,b,c,d)
	{
		normalize();
	}

	//!constructor from STL vector
	/*! 
		@py 
		h=Plane4f([1,0,0,1]); assert h[0]==1 and h[1]==0 and h[2]==0 and h[3]==1 
		@endpy
	*/
	inline explicit Plane4f(const std::vector<float>& src) :Vec4f(src)
	{
		normalize();
	}


	//! constructor from normal and point
	/*! 
		@py 
		h=Plane4f(Vec3f(0,0,1),Vec3f(0,0,1))
		assert h[0]==0 and h[1]==0 and h[2]==1 and h[3]==-1 
		@endpy
	*/
	inline explicit Plane4f(Vec3f n,Vec3f p)
	{
		n=n.normalize();
		this->x=n.x;
		this->y=n.y;
		this->z=n.z;
		this->w=-(n*p);
	}
		
	//! constructor from normal and distance
	/*! 
		see http://www.gamedev.net/community/forums/topic.asp?topic_id=358624

		@py 
		h=Plane4f(Vec3f(0,0,1),10) 
		assert h[0]==0 and h[1]==0 and h[2]==1 and h[3]==-10
		@endpy
	*/
	inline explicit Plane4f(Vec3f n,float d)
	{
		n=n.normalize();
		this->x=n.x;
		this->y=n.y;
		this->z=n.z;
		this->w=-d; 
	}

	//! constructor from 3 points
	/*! 
		@py 
		h=Plane4f(Vec3f(0,0,0),Vec3f(1,0,0),Vec3f(0,1,0))
		assert h[0]==0 and h[1]==0 and h[2]==1 and h[3]==0
		@endpy
	*/
	inline explicit Plane4f(const Vec3f& v0,const Vec3f& v1,const Vec3f& v2)
		{set(v0,v1,v2);}

	//! set a Plane4f from 3 points
	/*! 
		@py 
		h=Plane4f()
		h.set(Vec3f(0,0,0),Vec3f(1,0,0),Vec3f(0,1,0)); assert h[0]==0 and h[1]==0 and h[2]==1 and h[3]==0
		@endpy
	*/
	inline void set(const Vec3f& v0,const Vec3f& v1,const Vec3f& v2)
	{
		Vec3f n=(v1-v0).cross(v2-v0).normalize();
		this->x=n.x;
		this->y=n.y;
		this->z=n.z;
		this->w=-1*(n*v0);
	}

	//! normalize the plane (side effect)
	/*! 
		@py 
		h=Plane4f(1,1,1,0)
		assert fabs(1-sqrt(h[0]*h[0]+h[1]*h[1]+h[2]*h[2]))<1e-4 and h[0]==h[1] and h[1]==h[2]
		@endpy
	*/
	inline void normalize() 
	{
		float m=getNormal().module();
		DebugAssert(m && !isnan(m) && finite(m));
		this->x/=m;
		this->y/=m;
		this->z/=m;
		this->w/=m;
	}

	//! return the projection matrix
	Mat4f getProjectionMatrix(Vec3f Center);

	//! project a point on a plane
	inline Vec3f projectPoint(Vec3f P)
	{
		//http://www.9math.com/book/projection-point-plane
		Vec3f N=this->getNormal();
		return P- (N * this->getDistance(P));
	}

	//! project a vector on a plane
	inline Vec3f projectVector(Vec3f V)
	{
		//http://www.gamedev.net/community/forums/topic.asp?topic_id=345149&whichpage=1&#2255698
		Vec3f N=this->getNormal();
		return V - (N * (V*N) );
	}

	//! find best fitting Plane4f, note points should be in the format x,y,z,x,y,z,...
	static Plane4f bestFittingPlane(int npoints,const float* points);

	//! find best fitting plane using a STL vector of points 
	/*! 
		@py 
		h=Plane4f.bestFittingPlane([  0,0,0,  1,0,0,  0,1,0  ])
		assert h[0]==0 and h[1]==0 and h[2]==1 and h[3]==0
		@endpy
	*/
	static Plane4f bestFittingPlane(const std::vector<float>& points)
	{
		int npoints=(int)points.size()/3;
		ReleaseAssert(npoints*3==points.size());
		return bestFittingPlane(npoints,&points[0]);
	}

	//! get normal as a Vec3f
	/*! 
	
		@py 
		h=Plane4f(0,0,1,0)
		assert h.getNormal()==Vec3f(0,0,1)
		@endpy
	*/
	inline Vec3f getNormal() const
		{return Vec3f(x,y,z);}

	//! classify point
	/*! 
		@py 
		h=Plane4f(0,0,1,0)
		assert h.getDistance(Vec3f(0,0,5))==5
		@endpy
	*/
	inline float getDistance(const Vec3f& point) const
		{return this->x*point.x + this->y*point.y + this->z*point.z + this->w; }	

	//! classify point
	/*! 
		@py 
		h=Plane4f(0,0,1,0)
		assert h.getDistance(0,0,5)==5
		@endpy
	*/
	inline float getDistance(float x,float y,float z) const
		{return this->x*x + this->y*y + this->z*z + this->w; }	

	//! reverse the plane
	/*! 
		@py 
		h=Plane4f(0,0,1,0).reverse()
		assert h[0]==0 and h[1]==0 and h[2]==-1 and h[3]==0
		@endpy
	*/
	inline Plane4f reverse() const
		{return Plane4f(-x,-y,-z,-w);}

	//! force point in the below space (side effect)
	/*! 
		@py 
		h=Plane4f(0,0,1,0).forceBelow(Vec3f(0,0,5))
		assert h[0]==0 and h[1]==0 and h[2]==-1 and h[3]==0
		@endpy
	*/
	inline Plane4f forceBelow(const Vec3f& point) const
		{if (getDistance(point)>0) return reverse(); else return *this;}

	//! force point in the above space (side effect)
	/*! 
		@py 
		h=Plane4f(0,0,1,0).forceAbove(Vec3f(0,0,-5))
		assert h[0]==0 and h[1]==0 and h[2]==-1 and h[3]==0
		@endpy
	*/
	inline Plane4f forceAbove(const Vec3f& point) const
		{if (getDistance(point)<0) return reverse(); else return *this;}

	//! python repr()
	/*! 
		@py 
		assert eval(repr(Plane4f(0,0,1,0)))==Plane4f(0,0,1,0)
		@endpy
	*/
	inline std::string repr() const
	{
		return Utils::Format("Plane4f(%e,%e,%e,%e)",x,y,z,w);
	}

	//python str()
	/*! 
		@py 
		assert eval(str(Plane4f(0,0,1,0)))==[0,0,1,0]
		@endpy
	*/
	inline std::string str() const
	{
		return Utils::Format("[%f,%f,%f,%f]",x,y,z,w);
	}

	//!SelfTest
	static int SelfTest();

}; //end class Plane4f



//==========================================================
//! plane in n-dim
/*!
	The first component is the homogeneous component
*/
//==========================================================
class XGE_API Planef: public Vecf
{

public:

	//! default constructor (build a plane in 1-dim)
	/*! 
		@py 
		h=Planef()
		assert h.dim==1 and h[0]==0 and h[1]==1
		@endpy
	*/
	inline Planef(): Vecf(1)
	{
		this->mem[0]=0;
		this->mem[1]=1.0f;
	}

	//! copy constructor
	/*! 
		@py 
		h=Planef(Planef(0,0,0,1))
		assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==1
		@endpy
	*/
	inline Planef(const Planef& src):Vecf(src)
	{
		;
	}

	//!constructor of a plane in dim-D
	/*! 
		@py 
		h=Planef(3)
		assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==1
		@endpy
	*/
	inline explicit Planef(int dim):Vecf(dim)
	{
		DebugAssert(dim>=1); //the plane should be in the form D+ax+... 
		this->mem[0  ]=0;
		this->mem[dim]=1;// Xlast>=0
	}

	//! constructor from stl vector
	/*! 
		@py 
		h=Planef([0,0,0,1])
		assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==1
		@endpy
	*/
	inline explicit Planef(const std::vector<float>& src) :Vecf(src)
	{
		normalize();
	}

	//! constructor of 2D plane
	/*! 
		@py 
		h=Planef(0,0,1)
		assert h.dim==2 and h[0]==0 and h[1]==0 and h[2]==1
		@endpy
	*/
	inline explicit Planef(float a,float b,float c):Vecf(a,b,c)
	{
		normalize();
	}

	//! constructor of 3D plane
	/*! 
		@py 
		h=Planef(0,0,0,1)
		assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==1
		@endpy
	*/
	inline explicit Planef(float a,float b,float c,float d):Vecf(a,b,c,d)
	{
		normalize();
	}

	//! constructor of dim-D plane from a C array of float
	inline explicit Planef(int dim,const float* src): Vecf(dim,src)
	{
		DebugAssert(dim>=1);
		normalize();
	}

	//! constructor of dim-D plane from a C array of double
	inline explicit Planef(int dim,const double* src): Vecf(dim,src)
	{
		DebugAssert(dim>=1);
		normalize();
	}

	//! constructor (from a Vecf)
	/*! 
		@py 
		h=Planef(Vecf(0,0,0,1))
		assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==1
		@endpy
	*/
	inline Planef(const Vecf& src):Vecf(src)
	{
		DebugAssert(dim>=1);
		normalize();
	}


	//! constructor from normal and distance
	/*! 
		see http://www.gamedev.net/community/forums/topic.asp?topic_id=358624

		@py 
		h=Planef(Vecf([0,0,0,1]),1)
		assert h.dim==3 and h[0]==-1 and h[1]==0 and h[2]==0 and h[3]==1
		@endpy
	*/
	inline explicit Planef(Vecf n,float d)
		:Vecf(n.dim)
	{
		DebugAssert(!n[0]); //should be a vector
		DebugAssert(dim>=1);
		this->mem[0]=-d; 
		n=n.normalize();
		for (int i=1;i<=dim;i++) this->mem[i]=n[i];
	}

	//!constructor by its normal and the condition that the plane contains the point
	/*! 
		@py 
		h=Planef(Vecf([0,0,0,1]),Vecf([1,0,0,0]))
		assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==1
		@endpy
	*/
	inline explicit Planef(Vecf n,Vecf p):Vecf(n.dim)
	{
		DebugAssert(!n[0] && n.dim==p.dim && dim>=1);
		n=n.normalize();
		for (int i=1;i<=dim;i++) this->mem[i]=n[i];
		this->mem[0]=-(float)getDistance(p);
	}

	//! normalize
	/*! 
		@py 
		h=Planef(0,1,1,1)
		assert h.dim==3 and h[0]==0 and fabs(1-sqrt(h[1]*h[1]+h[2]*h[2]+h[3]*h[3]))<1e-4
		@endpy
	*/
	inline void normalize() 
	{
		float m=getNormal().module();

//#ifdef _DEBUG
//		DebugAssert(m && !isnan(m) && finite(m));
//		for (int i=0;i<=dim;i++) this->mem[i]/=m;
//#else //_DEBUG
		if (m && !isnan(m) && finite(m))
		{
			for (int i=0;i<=dim;i++) 
				this->mem[i]/=m;
		}
//#endif  //_DEBUG
	}
 

	//! find best fitting Planef (internal use)
	/*!
		in 2D pointdim should be 2 (and this Planef should have dimension 3!), and points will be x,y,x,y,x,y...WITHOUT homougeneous coordinates\n
		in 3D pointdim should be 3 (and this Planef should have dimension 4!), and points will be x,y,z,x,y,z,x,y,z...WITHOUT homougeneous coordinates\n
	*/
	static Planef bestFittingPlane(int pointdim,int npoints,const float* points);

	static Planef bestFittingPlane(int pointdim,const std::vector<float>& points)
	{
		int npoints=(int)points.size()/pointdim;
		ReleaseAssert(npoints*pointdim==(int)points.size());
		return bestFittingPlane(pointdim,npoints,&points[0]);
	}

	//! get normal, the returned vector has 0 in the first position since it is a vector
	/*! 
		@py 
		h=Planef([10, 0,0,1]);
		assert h.getNormal()==Vecf([0,0,0,1])
		@endpy
	*/
	inline Vecf getNormal() const
	{
		Vecf ret=*this;
		ret.mem[0]=0.0f;//should be a vector
		return ret;
	}

	//! classify point
	inline float getDistance(const float* point) const
	{
		float ret;
		switch (dim)
		{
			case 1:ret= this->mem[0]*point[0]+this->mem[1]*point[1];break;
			case 2:ret= this->mem[0]*point[0]+this->mem[1]*point[1]+this->mem[2]*point[2];break;
			case 3:ret= this->mem[0]*point[0]+this->mem[1]*point[1]+this->mem[2]*point[2]+this->mem[3]*point[3];break;
			default:{ret=0;for (int j=0;j<=dim;++j) ret+=this->mem[j]*point[j];break;}
		}
		return ret;
	}	

	//! getDistance of a point
	/*! 
		@py 
		h=Planef([0, 0,0,1])
		assert h.getDistance(Vecf([1, 0,0,10]))==10
		@endpy
	*/
	inline float getDistance(const Vecf& point) const
	{
		DebugAssert(point.dim==this->dim);
		return getDistance(point.mem);
	}	

	//! reverse plane (no side effect)
	/*! 
		@py 
		h=Planef([0, 0,0,1]).reverse()
		assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==-1
		@endpy
	*/
	inline Planef reverse() const
	{
		return (*this) * -1;
}

	//! force point to be in the below space (no side effect)
	/*! 
		@py 
		h=Planef([0, 0,0,1]).forceBelow(Vecf(1,0,0,1))
		assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==-1
		@endpy
	*/
	inline Planef forceBelow(const Vecf& point) const
		{if (getDistance(point)>0) return reverse(); else return (*this);}

	//! force point to be in the above space (no side effect)
	/*! 
		@py 
		h=Planef([0, 0,0,1]).forceAbove(Vecf(1,0,0,-1))
		assert h.dim==3 and h[0]==0 and h[1]==0 and h[2]==0 and h[3]==-1
		@endpy
	*/
	inline Planef forceAbove(const Vecf& point) const
		{if (getDistance(point)<0) return reverse(); else return (*this);}

	//! return the matrix which project points to a lower dimension
	Matf getProjectionMatrix(Vecf Center);

	//! project a point on the plane
	/*! 
		@py 
		h=Planef([0, 0,0,1])
		assert h.projectPoint(Vecf([1,0,0,10]))==Vecf([1,0,0,0])
		@endpy
	*/
	inline Vecf projectPoint(Vecf p)
	{
		DebugAssert(p[0]==1.0f); //should be a point
		Vecf ret=p-getDistance(p)*getNormal();
		DebugAssert(getDistance(ret)<0.001f);
		return ret;
	}

	//! get a random plane
	/*! 
		@py 
		h=Planef.getRandomPlane(3)
		assert h.dim==3 and fabs(1-h.getNormal().module())<1e-3
		@endpy
	*/
	static inline Planef getRandomPlane(int dim)
	{
		Planef h(dim);
		for (int I=0;I<=dim;I++) 
			h.set(I,Utils::FloatRand(-1,+1));
		h.normalize();
		return h;
	}

	//! get a random point on a plane
	/*! 
		@py 
		h=Planef.getRandomPlane(3)
		assert fabs(h.getDistance(h.getRandomPoint()))<1e-3
		@endpy
	*/
	inline Vecf getRandomPoint()
	{
		Vecf Point(dim);Point.set(0,1.0f); //should be a point
		for (int i=1;i<=dim;i++) Point.set(i,Utils::FloatRand(-1,+1));
		return projectPoint(Point);
	}

	

	//! return main direction (the component which has the maximum absolute value)
	/*! 
		@py 
		h=Planef([0,0,0,1])
		assert h.getMainDirection()==3
		@endpy
	*/
	inline int getMainDirection() const
	{
		DebugAssert(this->dim>=1);
		int ret=1;
		for (int i=2;i<=dim;i++) if (fabs((*this)[i])>fabs((*this)[ret])) ret=i;
		return ret;
	}

	//! python repr()
	/*! 
		@py 
		h=Planef([0,0,0,1])
		assert eval(repr(h))==Planef([0,0,0,1])
		@endpy
	*/
	inline std::string repr() const
	{
		std::string ret="Planef(";
		for (int i=0;i<=dim;i++) ret+=(i?",":"")+Utils::Format("%e",(*this)[i]);
		ret+=")";
		return ret;
	}

	//! python str()
	/*! 
		@py 
		h=Planef([0,0,0,1])
		assert eval(str(h))==[0,0,0,1]
		@endpy
	*/
	inline std::string str() const
	{
		std::string ret="[";
		for (int i=0;i<=dim;i++) ret+=(i?",":"")+Utils::Format("%f",(*this)[i]);
		ret+="]";
		return ret;
	}

	//!SelfTest
	static int SelfTest();

}; //end class Planef


#endif //_PLANE_H__

