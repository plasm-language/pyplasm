#ifndef _PICK_H__
#define _PICK_H__

#include <xge/xge.h>
#include <xge/vec.h>
#include <xge/mat.h>
#include <xge/plane.h>

//=========================================================
//! stores an opengl pick using rays
/*!
	When an Object is hit by a ray it can return 
	an instance of this class, full of information about 
	the pick
*/
//=========================================================
class XGE_API Pick
{
public:

	//! if valid or not
	bool valid;

	//! the distance of the pick from the ray
	float distance;

	//! the position of the first hit point
	Vec3f position;

	//! the normal of the point hit by the ray
	Vec3f normal;

	//! eventually a trasformation matrix (for example the matrix which set the right position in Editor3d of a symbol)
	Mat4f  matrix; 

	//! argomento per rintracciare la primitiva all'interno dell'oggetto
	std::vector<int> path; 

	//! default constructor (build an invalid pick)
	inline Pick()
	{
		valid=false;
		distance=1e18f;
	}

	//! copy constructor
	inline Pick(const Pick& src)
	{
		this->valid=src.valid;
		this->distance=src.distance;
		this->position=src.position;
		this->normal=src.normal;
		this->matrix=src.matrix;
		this->path=src.path;

	}

	//! test if valid
	inline bool isValid()
		{return valid;}

	//!fix a pick filling up matrix
	void guessMatrix(Vec3f viewer_position)
	{
		//not a valid pick
		if (!this->valid)
			return;

		// a little trich if the normal goes away from the viewer position (seems to be a problem of orientation of normals coming from Plasm)
		Plane4f h(this->normal, this->normal * this->position);

		if (h.getDistance(viewer_position) < 0) 
			this->normal *=-1.0;

		Vec3f N = this->normal.normalize();
		int max_index=N.Abs().maxidx();

		Vec3f xt,yt,zt;
		ReleaseAssert(max_index>=0 && max_index<=2);

		switch(max_index)
		{
		case 0:
			yt=N;
			zt=(Vec3f(0,N[0]>=0?-1.0f:+1.0f,0)).cross(yt).normalize();
			break;

		case 1:
			yt=N;
			zt=Vec3f(N[1]>=0?+1.0f:-1.0f,0,0).cross(yt).normalize();
			break;

		case 2:
			zt=N;
			yt=Vec3f(-1,0,0).cross(zt).normalize();
			break;
		}

		xt = yt.cross(zt).normalize();

		float _T2[16]=
		{
			xt.x, yt.x, zt.x, 0, 
			xt.y, yt.y, zt.y, 0, 
			xt.z, yt.z, zt.z, 0, 
			0,   0,    0, 1
		};
		Mat4f T2 (_T2);

		//mat3
		float _T3[16]=
		{
			1, 0, 0, this->position.x, 
			0, 1, 0, this->position.y, 
			0, 0, 1, this->position.z, 
			0, 0, 0, 1
		};
		Mat4f T3 (_T3);
		this->matrix=T3 * T2;
	}

	//! internal use, for Scene to order picks by a priority queue
	inline bool operator<(const Pick& other) const 
		{return (this->distance > other.distance);}

}; //end class




#endif //_PICK_H__

