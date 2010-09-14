
#ifndef _FRUSTUM_H__
#define _FRUSTUM_H__

#include <xge/xge.h>
#include <xge/vec.h>
#include <xge/ray.h>
#include <xge/box.h>

//predeclaration
class Batch;
class Engine;

//==================================================
//!  perspective frustum
/*!
	Questa classe serve in generale ai viewer 
	per impostare il punto di vista e le matrici di 
	trasformazione di modeling e di projection.

	Attraverso il frustum e' possibile sapere dove e'
	la posizione della persona che naviga, la direzione etc etc

	In piu' la classe offre molte utility per testare se un oggetto
	e' all'interno del view frustum o no (in modo da poter essere
	scartato in fase di rendering).
*/
//==================================================
class  XGE_API Frustum
{
	
public:

	//! walk movement (to be set from outside)
	float walk_speed;

	//! viewport
	int x,y,width,height;

	//! modelview matrix expressed by this parameters
	Vec3f pos,dir,vup,right;

	//! projection matrix (can be perspective or ortho)
	Mat4f projection_matrix;

	//the overall direct matrix (projection * modelview) and its inverse
	Mat4f mat_dir,mat_inv; 

	//! the axis aligned bounding box of the frustum (floating coordinates!)
	/*!
		@py
		f=Frustum()
		box=f.aabb
		@endpy
	*/
	Box3f aabb;

	//! i planes del view frustum

	enum 
	{
		PLANE_LEFT,
		PLANE_RIGHT,
		PLANE_TOP,
		PLANE_BOTTOM,
		PLANE_NEAR,
		PLANE_FAR
	};

	Plane4f planes[6];

	//!default constructor
	/*!
		By default, the camera is situated at the origin, points down the negative z-axis, and has an up-vector of (0, 1, 0)
	*/
	Frustum();

	//! return the main ray
	/*!
		Il raggio principale e' il raggio che viene sparato dalla posizione corrente
		della camera in direzione della direzione di vista

		\return il raggio principale

		@py
		f=Frustum()
		f.getRay()==Ray3f(Vec3f(0,0,0),Vec3f(0,0,-1))
		@endpy
	*/
	inline Ray3f getRay() const
		{return Ray3f(this->pos,this->dir);}

	//! test strict equality
	/*!
		\param src il frustum sorgente da confrontare
		\return true se sono uguali, false altrimenti
	*/
	inline bool operator==(const Frustum& src) const
	{
		return (this==&src); //only if same addresss
	}

	//! return the frustum points
	enum 
	{
		POINT_NEAR_BOTTOM_LEFT,
		POINT_NEAR_BOTTOM_RIGHT,
		POINT_NEAR_TOP_RIGHT,
		POINT_NEAR_TOP_LEFT,
		POINT_FAR_BOTTOM_LEFT,
		POINT_FAR_BOTTOM_RIGHT,
		POINT_FAR_TOP_RIGHT,
		POINT_FAR_TOP_LEFT
	};

	Vec3f points[8];

	//! refresh
	/*! 
		Forza un refresh di tutti i parametri, da chiamare OGNI volta che si modifica qualcosa del frustum
	*/
	void refresh();


	//! intersection of the frustum box with an Axis Aligned Bounding Box (AABB)
	/*!
		L'algoritmo utilizzato si trova all'url:
		http://www.gamedev.net/community/forums/topic.asp?topic_id=487066

		Non sono sicuro che se la funzione torna true, effettivamente esiste una intersezione.
		Ma e' sufficiente che potenzialmente potrebbero intersecarsi e poi utilizzare funzioni
		di intersezione piu' precise (ad esempio basate sui triangoli).

		\param aabb il box da testare
		\return true se esiste intersezione, false altrimenti

		@py
		f=Frustum()
		assert not f.intersect(Box3f(Vec3f(1,1,1),Vec3f(2,2,2)))
		@endpy
	*/
	inline bool intersect(const Box3f& aabb) const
	{
		if (!this->aabb.isValid() || !aabb.isValid())
			return false;

		//trivial and fast check if they overlap
		if (!this->aabb.overlap(aabb))
			return false;

		//classify near point
		if ((planes[0].w+planes[0].x*(planes[0].x<0?aabb.p2.x:aabb.p1.x)+planes[0].y*(planes[0].y<0?aabb.p2.y:aabb.p1.y)+planes[0].z*(planes[0].z<0?aabb.p2.z:aabb.p1.z))>=0) return false;
		if ((planes[1].w+planes[1].x*(planes[1].x<0?aabb.p2.x:aabb.p1.x)+planes[1].y*(planes[1].y<0?aabb.p2.y:aabb.p1.y)+planes[1].z*(planes[1].z<0?aabb.p2.z:aabb.p1.z))>=0) return false;
		if ((planes[2].w+planes[2].x*(planes[2].x<0?aabb.p2.x:aabb.p1.x)+planes[2].y*(planes[2].y<0?aabb.p2.y:aabb.p1.y)+planes[2].z*(planes[2].z<0?aabb.p2.z:aabb.p1.z))>=0) return false;
		if ((planes[3].w+planes[3].x*(planes[3].x<0?aabb.p2.x:aabb.p1.x)+planes[3].y*(planes[3].y<0?aabb.p2.y:aabb.p1.y)+planes[3].z*(planes[3].z<0?aabb.p2.z:aabb.p1.z))>=0) return false;
		if ((planes[4].w+planes[4].x*(planes[4].x<0?aabb.p2.x:aabb.p1.x)+planes[4].y*(planes[4].y<0?aabb.p2.y:aabb.p1.y)+planes[4].z*(planes[4].z<0?aabb.p2.z:aabb.p1.z))>=0) return false;
		if ((planes[5].w+planes[5].x*(planes[5].x<0?aabb.p2.x:aabb.p1.x)+planes[5].y*(planes[5].y<0?aabb.p2.y:aabb.p1.y)+planes[5].z*(planes[5].z<0?aabb.p2.z:aabb.p1.z))>=0) return false;

		//NOTE: there could be false positive
		//classyfy near point respect to aabb, solved using inverse frustum culling (==use planes of aabb box)
		//BUT THE FIRST CALL TO OVERLAP COVER THIS CASES!
		/*
		                  _____
					     |     |
                         |     |
			 ___________ |     |
 			 \         / |     |
              \       /  |_____|
			   \     /
			    \___/
		*/

		//is only a potential intersection, not sure if they really intersect for any case
		//there should be the need to consider cross of edges
		//http://realtimecollisiondetection.net/pubs/GDC07_Ericson_Physics_Tutorial_SAT.ppt#272,16,Code fragment
		//but to be fast enough I do not test them
		//IMPORTANT: since the second polyhedron is AABB is that true?

		return true;
	}

	//! test if a point is inside frustum
	/*!
		\param p il punto da testare
		\return true se il punto e' dentro al frustum, altrimenti false

		@py
		f=Frustum()
		assert f.contains(Vec3f(0,0,-3))
		@endpy
	*/
	inline bool contains(const Vec3f& p)
	{
		return 
			 (planes[0].getDistance(p)<=0) &&
			 (planes[1].getDistance(p)<=0) &&
			 (planes[2].getDistance(p)<=0) &&
			 (planes[3].getDistance(p)<=0) &&
			 (planes[4].getDistance(p)<=0) &&
			 (planes[5].getDistance(p)<=0);
	}

	//!retur the modelview matrix
	inline Mat4f getModelviewMatrix() const
	{
		return Mat4f::lookat(
			this->pos.x,this->pos.y,this->pos.z,
			this->pos.x+this->dir.x,this->pos.y+this->dir.y,this->pos.z+this->dir.z,
			this->vup.x,this->vup.y,this->vup.z);
	}

	//! if you want to display the frustum
	void Render(Engine* engine);

	//! project (important, the frustum should be properly set and a refresh() is needed)
	Vec3f project(Vec3f P);

	//!unproject from screen coordinates (important, the frustum should be properly set and a refresh() is needed)
    Ray3f unproject(int x,int  y);

	//! guess best position
	void guessBestPosition(const Box3f& box);

	//! guess best projection matrix ()
	void guessProjectionMatrix(Box3f box,float fov=DEFAULT_FOV);

	//! automatic handle of mouse, modify the frustum after a mouse movement
	bool defaultMouseWalkingMode(int button,int mouse_beginx,int mouse_beginy,int x,int y);

	//! automatic handle of mouse, modify the frustum after a mouse movement
	bool defaultMouseTrackballMode(int button,int mouse_beginx,int mouse_beginy,int x,int y,Vec3f center);

	//! for basic movement inside the scene
	bool defaultKeyboard(int x,int y,int key);

	//! fix vup (==force to be a coordinate axis)
	void fixVup();

	//!get screen distance
	float getScreenDistance( Vec3f P1,  Vec3f P2);

	//! test if two points are near on the screen after projection)
	bool nearOnScreen(Vec3f P1, Vec3f P2,float min_distance_in_pixel=5);

	//! python repr()
	/*!
		\return la python repr() del frustum
	*/
	inline std::string repr() const
	{
		return Utils::Format("Frustum(pos=%s,dir=%s,vup=%s)",pos.repr().c_str(),dir.repr().c_str(),vup.repr().c_str());
	}

	//! python str()
	/*!
		\return la python str() del frustum
	*/
	inline std::string str() const
	{
		return repr();
	}

	//int SelfTest()

}; //end class


#endif  //_FRUSTUM_H__


