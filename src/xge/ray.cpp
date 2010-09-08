#include <xge/xge.h>
#include <xge/ray.h>


///////////////////////////////////////////////////////////////////////////////////////////////
float Ray3f::intersectLine(float v0[2],float v1[2])
{
	DebugAssert(origin.z==0 && dir.z==0);

	#define PERP(v)   Vec3f(v.y,-v.x,0)
	#define DOT(a,b) ((a)*(b))

	Vec3f P0(origin[0],origin[1],0.0f);
	Vec3f D0(dir[0],dir[1],0.0f);

	Vec3f P1(v0[0],v0[1],0.0             );
	Vec3f D1(v1[0]-v0[0],v1[1]-v0[1],0.0f);

	float den=DOT(PERP(D1),D0);

	if (!den)
		return -1;

	float s = DOT(PERP(D1),P1-P0)/den;
	float t = DOT(PERP(D0),P1-P0)/den;

	if (t<0 || t>1) return -1;
	if (s<0       ) return -1;

	return s;

	#undef DOT
	#undef PERP

}

///////////////////////////////////////////////////////////////////////////////////////////////
float Ray3f::intersectTriangle(const Vec3f& v0,const Vec3f& v1,const Vec3f& v2) const
{
	#define CROSS(dst,a,b)      dst[0]=a[1] * b[2] - b[1] * a[2]; dst[1]= a[2] * b[0] - b[2] * a[0];dst[2]= a[0] * b[1] - b[0] * a[1];
	#define PRODUCT(a,b)        (a[0]*b[0]+a[1]*b[1]+a[2]*b[2])
	#define DIFF(dst,a,b)       dst[0]=a[0]-b[0];dst[1]=a[1]-b[1];dst[2]=a[2]-b[2];
	#define MODULE(a)           sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2])

	DebugAssert(fabs(1-MODULE(dir))<0.001); //should be normalized

	float e1[3];DIFF(e1,v1,v0);
	float e2[3];DIFF(e2,v2,v0);

	float h[3];CROSS(h,dir,e2);
	float a=PRODUCT(e1,h);

	float f = 1/a;
	float s[3];DIFF(s,origin,v0);
	float u = PRODUCT(s,h)*f;
	if (u < 0.0 || u > 1.0) return -1;
	
	float q[3];CROSS(q,s,e1);
	float v =PRODUCT(dir,q)* f ;
	if (v < 0.0 || u + v > 1.0) return -1;

	// at this stage we can compute t to find out where 
	// the intersection point is on the line
	float Distance = PRODUCT(e2,q)* f ;

	return Distance>=0?Distance:-1; 

	#undef CROSS
	#undef PRODUCT
	#undef DIFF
	#undef MODULE
}


///////////////////////////////////////////////////////////////////////////////////////////////
bool Ray3f::intersectBox(float& tmin,float& tmax,Box3f box) const
{
	Vec3f inv_direction(1/this->dir.x, 1/this->dir.y, 1/this->dir.z);

	float txmin  = ((inv_direction.x>=0? box.p1.x : box.p2.x) - this->origin.x) * inv_direction.x;
	float txmax  = ((inv_direction.x>=0? box.p2.x : box.p1.x) - this->origin.x) * inv_direction.x;

	tmin=txmin;
	tmax=txmax;

	float tymin = ((inv_direction.y>=0? box.p1.y : box.p2.y) - this->origin.y) * inv_direction.y;
	float tymax = ((inv_direction.y>=0? box.p2.y : box.p1.y) - this->origin.y) * inv_direction.y;

	if (!Utils::Overlap1d(tmin,tmax,tymin,tymax)) return false;
	if (tymin > tmin) tmin = tymin;
	if (tymax < tmax) tmax = tymax;

	float tzmin = ((inv_direction.z>=0? box.p1.z : box.p2.z) - this->origin.z) * inv_direction.z;
	float tzmax = ((inv_direction.z>=0? box.p2.z : box.p1.z) - this->origin.z) * inv_direction.z;

	if (!Utils::Overlap1d(tmin,tmax,tzmin,tzmax)) return false;
	if (tzmin > tmin) tmin = tzmin;
	if (tzmax < tmax) tmax = tzmax;

	//safety check
	DebugAssert(tmin<=tmax);
	return true;
}





////////////////////////////////////////////////////////////////////////////
int Ray3f::SelfTest()
{
	Log::printf("Testing Ray3f...\n");


	Ray3f r(Vec3f(0,0,0),Vec3f(1,1,1));
	ReleaseAssert((r.dir.module()-1)<0.0001f);

	Ray3f r2(r);
	r2.setOrigin(r.origin+Vec3f(0.0001f,0,0));
	ReleaseAssert(r==r && r!=r2);

	ReleaseAssert(r.getPoint(0).fuzzyEqual(Vec3f(0,0,0)) && r.getPoint(1).fuzzyEqual(Vec3f(1,1,1).normalize())) ;

	//intersection
	Vec3f i=Ray3f(Vec3f(0,0,0),Vec3f(0,0,1)).intersection(Plane4f(Vec3f(0,0,1),+2));
	ReleaseAssert(i.fuzzyEqual(Vec3f(0,0,2)));


	//intersect line
	{
		Ray3f ray(Vec3f(0,0,0),Vec3f(1,0,0));
		float p0[]={1,-1.01f};
		float p1[]={1,+1.01f};
		float dist=ray.intersectLine(p0,p1);
		ReleaseAssert(fabs(dist-1.0f)<0.001f);

		ray=Ray3f(Vec3f(0,0,0),Vec3f(1,1,0));
		dist=ray.intersectLine(p0,p1);
		ReleaseAssert(fabs(dist-sqrt(2.0f))<0.001f);

		ray=Ray3f(Vec3f(0,0,0),Vec3f(0,1,0));
		dist=ray.intersectLine(p0,p1);
		ReleaseAssert(dist==-1);
	}

	//intertect triangle
	{

		Ray3f ray(Vec3f(0,0,0),Vec3f(0,0,1));
		Vec3f p0(0,0,1);
		Vec3f p1(1.01f,0,1);
		Vec3f p2(1.01f,1.01f,1);

		 float dist=ray.intersectTriangle(p0,p1,p2);
		 ReleaseAssert(fabs(dist-1.0)<0.001f);

		ray=Ray3f(Vec3f(0,0,0),Vec3f(1,1,1));
		dist=ray.intersectTriangle(p0,p1,p2);
		ReleaseAssert(fabs(dist-sqrt(3.0f))<0.001f);

		ray=Ray3f(Vec3f(0,0,0),Vec3f(1,0,0));
		dist=ray.intersectTriangle(p0,p1,p2);
		ReleaseAssert(dist==-1);
	}

	//intersect box
	{
		Box3f box;
		float tmin,tmax;
		
		box=Box3f(Vec3f(-1,-1,-1),Vec3f(+1,+1,+1));
		ReleaseAssert(Ray3f(Vec3f(0,0,0),Vec3f(0,0,1)).intersectBox(tmin,tmax,box) && Utils::FuzzyEqual(tmin,-1) && Utils::FuzzyEqual(tmax,+1));
		ReleaseAssert(Ray3f(Vec3f(0,0,0),Vec3f(0,1,0)).intersectBox(tmin,tmax,box) && Utils::FuzzyEqual(tmin,-1) && Utils::FuzzyEqual(tmax,+1));
		ReleaseAssert(Ray3f(Vec3f(0,0,0),Vec3f(1,0,0)).intersectBox(tmin,tmax,box) && Utils::FuzzyEqual(tmin,-1) && Utils::FuzzyEqual(tmax,+1));

		box=Box3f(Vec3f(0,0,0),Vec3f(1,1,1));

		//test limit cases
		ReleaseAssert(Ray3f(Vec3f(-1,+1e-4f,+1e-4f),Vec3f(1,0,0)).intersectBox(tmin,tmax,box) && Utils::FuzzyEqual(tmin,1) && Utils::FuzzyEqual(tmax,+2));
		ReleaseAssert(Ray3f(Vec3f(+1e-4f,-1,+1e-4f),Vec3f(0,1,0)).intersectBox(tmin,tmax,box) && Utils::FuzzyEqual(tmin,1) && Utils::FuzzyEqual(tmax,+2));
		ReleaseAssert(Ray3f(Vec3f(+1e-4f,+1e-4f,-1),Vec3f(0,0,1)).intersectBox(tmin,tmax,box) && Utils::FuzzyEqual(tmin,1) && Utils::FuzzyEqual(tmax,+2));

		ReleaseAssert(!Ray3f(Vec3f(-1,-1e-4f,-1e-4f),Vec3f(1,0,0)).intersectBox(tmin,tmax,box));
		ReleaseAssert(!Ray3f(Vec3f(-1e-4f,-1,-1e-4f),Vec3f(0,1,0)).intersectBox(tmin,tmax,box));
		ReleaseAssert(!Ray3f(Vec3f(-1e-4f,-1e-4f,-1),Vec3f(0,0,1)).intersectBox(tmin,tmax,box));

		ReleaseAssert(!Ray3f(Vec3f(-1,0,0),Vec3f(0,1,0)).intersectBox(tmin,tmax,box));
		ReleaseAssert(!Ray3f(Vec3f(-1,0,0),Vec3f(0,0,1)).intersectBox(tmin,tmax,box));

		ReleaseAssert(!Ray3f(Vec3f(0,-1,0),Vec3f(1,0,0)).intersectBox(tmin,tmax,box));
		ReleaseAssert(!Ray3f(Vec3f(0,-1,0),Vec3f(0,0,1)).intersectBox(tmin,tmax,box));

		ReleaseAssert(!Ray3f(Vec3f(0,0,-1),Vec3f(1,0,0)).intersectBox(tmin,tmax,box));
		ReleaseAssert(!Ray3f(Vec3f(0,0,-1),Vec3f(0,1,0)).intersectBox(tmin,tmax,box));
	}

	return 0;
}




////////////////////////////////////////////////////////////////////////////
int Rayf::SelfTest()
{
	Log::printf("Testing Rayf...\n");

	//test intersection
	for (int dim=1;dim<16;dim++)
	{
		//the plane
		Planef plane(dim);

		//x+y+z+...>=1
		plane.mem[0]=-1;
		for (int i=1;i<=dim;i++) plane.mem[i]=1;

		//a point in the origin
		Vecf Origin(dim);
		Origin.mem[0]=1.0f; //is a point

		const int ntests=100;
		for (int i=0;i<ntests;i++)
		{
			Vecf Dir(dim);
			Dir.mem[0]=0; //is a vector
			for (int i=1;i<=dim;i++) Dir.mem[i]=Utils::FloatRand(0,1);

			Rayf ray(Origin,Dir);
			Vecf point=ray.intersection(plane);
			ReleaseAssert(fabs(plane.getDistance(point))<0.0001f);
		}
	}
	

	return 0;
}