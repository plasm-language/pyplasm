#include <xge/xge.h>
#include <xge/ray.h>


///////////////////////////////////////////////////////////////////////////////////////////////
float Ray3f::intersectLine(float v0[2],float v1[2])
{
	XgeDebugAssert(origin.z==0 && dir.z==0);

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

	XgeDebugAssert(fabs(1-MODULE(dir))<0.001); //should be normalized

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
	XgeDebugAssert(tmin<=tmax);
	return true;
}






