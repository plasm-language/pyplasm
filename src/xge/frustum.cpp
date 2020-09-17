
#include <xge/xge.h>
#include <xge/frustum.h>
#include <xge/mat.h>
#include <xge/quaternion.h>



////////////////////////////////////////////////////////////
Frustum::Frustum()
{
	//default values to be set from outside
	this->x      =0;
	this->y      =0;
	this->width  =1;
	this->height =1;

	//default camera position
	this->pos=Vec3f(+1,+1, +1);
	this->dir=Vec3f(-1,-1,-1).normalize();
	this->vup=Vec3f(0,0,+1);

	//set a projection matrix
	this->projection_matrix=Mat4f::perspective(DEFAULT_FOV,width/(float)height,0.001f,1000.0f);

	this->walk_speed=0.00f;
}


/// ////////////////////////////////////////////////////////////////////////////
/// set/get the world
/// /// /////////////////////////////////////////////////////////////////////////////////
void Frustum::guessProjectionMatrix(Box3f box,float fov)
{
	float zNear;
	float zFar;

	Plane4f h(this->dir,this->pos);

	//find the zFar
	Vec3f far_point ((this->dir.x>=0)? box.p2.x: box.p1.x,(this->dir.y>=0)? box.p2.y: box.p1.y,(this->dir.z>=0)? box.p2.z: box.p1.z);
	zFar =1.1f * h.getDistance(far_point);
	if (zFar<=0) zFar =100;

	//try to find the zNear
	Vec3f near_point((this->dir.x>=0)? box.p1.x: box.p2.x,(this->dir.y>=0)? box.p1.y: box.p2.y,(this->dir.z>=0)? box.p1.z: box.p2.z);
	zNear=0.9f* h.getDistance(near_point) ;


	//cannot be the zNear negative, so correct it
	//if the position is inside I want the near to be closest possible to the frustum.pos,
	//but not much to loose to much pixels in depth test precision
    //   see http://www.opengl.org/resources/faq/technical/depthbuffer.htm
    //   log2(zFar/zNear) bits of precision are lost
    //   since log2(1000) is quite 10 I loose almost 10 pixels of bith depth precision (24 are the total bits)
    //   be sure that zfar/znear<=1000 ->znear>=zfar/1000
	if (zNear<=0)
        zNear = zFar / 1000.0f; 

	//necessary
	XgeDebugAssert(zFar>0 && zNear>0 && zFar>zNear);

	//assign projectin matrix (remember to call refresh!)
	float aspect=this->width / (float)this->height;
	this->projection_matrix=Mat4f::perspective(fov, aspect, zNear, zFar);
}



////////////////////////////////////////////////////////////
void Frustum::refresh()
{
	//if the form has been hidden
	this->width =max2(this->width ,1);
	this->height=max2(this->height,1);

	//calculate RIGHT
	this->right=dir.cross(vup).normalize();

	//overall transformation matrix
	this->mat_dir           = (this->projection_matrix * this->getModelviewMatrix());
	this->mat_inv           = this->mat_dir.invert();

	//AABB and extract points
	aabb.reset();
	aabb.add(points[POINT_NEAR_BOTTOM_LEFT ]=( mat_inv * Vec3f(-1, -1, -1) ));
	aabb.add(points[POINT_NEAR_BOTTOM_RIGHT]=( mat_inv * Vec3f(+1, -1, -1) ));
	aabb.add(points[POINT_NEAR_TOP_RIGHT   ]=( mat_inv * Vec3f(+1, +1, -1) ));
	aabb.add(points[POINT_NEAR_TOP_LEFT    ]=( mat_inv * Vec3f(-1, +1, -1) ));
	aabb.add(points[POINT_FAR_BOTTOM_LEFT  ]=( mat_inv * Vec3f(-1, -1, +1) ));
	aabb.add(points[POINT_FAR_BOTTOM_RIGHT ]=( mat_inv * Vec3f(+1, -1, +1) ));
	aabb.add(points[POINT_FAR_TOP_RIGHT    ]=( mat_inv * Vec3f(+1, +1, +1) ));
	aabb.add(points[POINT_FAR_TOP_LEFT     ]=( mat_inv * Vec3f(-1, +1, +1) ));

	//extract planes
	//http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf
        Mat4f mat_t=this->mat_dir.transpose();
	const float* mat=mat_t.mat;
	this->planes[PLANE_LEFT  ]=(Plane4f(-(mat[ 3] + mat[ 0]),-(mat[ 7] + mat[ 4]),-(mat[11] + mat[ 8]),-(mat[15] + mat[12])));
	this->planes[PLANE_RIGHT ]=(Plane4f(-(mat[ 3] - mat[ 0]),-(mat[ 7] - mat[ 4]),-(mat[11] - mat[ 8]),-(mat[15] - mat[12])));
	this->planes[PLANE_TOP   ]=(Plane4f(-(mat[ 3] - mat[ 1]),-(mat[ 7] - mat[ 5]),-(mat[11] - mat[ 9]),-(mat[15] - mat[13])));
	this->planes[PLANE_BOTTOM]=(Plane4f(-(mat[ 3] + mat[ 1]),-(mat[ 7] + mat[ 5]),-(mat[11] + mat[ 9]),-(mat[15] + mat[13])));
	this->planes[PLANE_NEAR  ]=(Plane4f(-(mat[ 3] + mat[ 2]),-(mat[ 7] + mat[ 6]),-(mat[11] + mat[10]),-(mat[15] + mat[14])));
	this->planes[PLANE_FAR   ]=(Plane4f(-(mat[ 3] - mat[ 2]),-(mat[ 7] - mat[ 6]),-(mat[11] - mat[10]),-(mat[15] - mat[14])));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Frustum::guessBestPosition(const Box3f& box)
{   
	XgeDebugAssert(box.isValid());

  //special case, 2d object in Z plane
  if (box.p1.z==box.p2.z)
  {
	  this->pos=box.center()+2*Vec3f(0,0,box.maxsize());
	  this->dir=Vec3f(0,0,-1);
	  this->vup=Vec3f(0,1,0);
  }
  else
  {
	  this->pos=box.center()+1.5f*box.size();
	  this->dir=(box.center()-pos).normalize();
	  this->vup=Vec3f(0,0,1);
  }

	//default projection matrix
  float maxdim = box.maxsize();
	this->projection_matrix=Mat4f::perspective(DEFAULT_FOV,width/(float)height,maxdim / 50.0f,maxdim * 10);
	this->walk_speed=maxdim/100.0f;
}	

///////////////////////////////////////////////////////////
Vec3f Frustum::project(Vec3f P) 
{
	P=this->mat_dir * P;
	P.x = this->x+(1+P.x)*this->width/2;
	P.y = this->y+(1+P.y)*this->height/2;
	P.z = (1+P.z)/2;
	return P;
}


////////////////////////////////////////////////////////////
Ray3f Frustum::unproject(int x,int  y)
{
   float inx = ((x             ) - (float)this->x) * 2.0f / (float)this->width - 1.0f;
   float iny = ((this->height-y) - (float)this->y) * 2.0f / (float)this->height - 1.0f;

	Vec3f P1  = mat_inv* Vec3f(inx,iny,2 * 0 - 1.0);
	Vec3f P2  = mat_inv* Vec3f(inx,iny,2 * 1 - 1.0);

	Ray3f ray;
	ray.origin=P1;
	ray.dir   =(P2-P1).normalize();
	return ray;

}


////////////////////////////////////////////////////////////
void Frustum::fixVup()
{
	int idx=this->vup.Abs().maxidx();
	Vec3f vup=Vec3f(0,0,0);
	vup.set(idx,this->vup[idx]>0?1.0f:-1.0f);
	this->vup=vup;
}

////////////////////////////////////////////////////////////
void Frustum::render(GLCanvas* glcanvas)
{
	SmartPointer<Batch> batch(new Batch);
	batch->primitive=Batch::LINES;
	
	batch->setColor(Color4f::Black());
	batch->vertices.reset(new Array(24*3));
	
	static int indices[24]=
	{
		POINT_NEAR_BOTTOM_LEFT   ,POINT_NEAR_BOTTOM_RIGHT,
		POINT_NEAR_BOTTOM_RIGHT  ,POINT_NEAR_TOP_RIGHT,
		POINT_NEAR_TOP_RIGHT     ,POINT_NEAR_TOP_LEFT,
		POINT_NEAR_TOP_LEFT      ,POINT_NEAR_BOTTOM_LEFT,

		POINT_FAR_BOTTOM_LEFT    ,POINT_FAR_BOTTOM_RIGHT,
		POINT_FAR_BOTTOM_RIGHT   ,POINT_FAR_TOP_RIGHT,
		POINT_FAR_TOP_RIGHT      ,POINT_FAR_TOP_LEFT,
		POINT_FAR_TOP_LEFT       ,POINT_FAR_TOP_LEFT,

		POINT_NEAR_BOTTOM_LEFT   ,POINT_FAR_BOTTOM_LEFT,
		POINT_NEAR_BOTTOM_RIGHT  ,POINT_FAR_BOTTOM_RIGHT,
		POINT_NEAR_TOP_RIGHT     ,POINT_FAR_TOP_RIGHT,
		POINT_NEAR_TOP_LEFT      ,POINT_FAR_TOP_LEFT
	};

	float* v=(float*)batch->vertices->c_ptr();
	for (int i=0;i<24;i++)
	{
		*v++=points[indices[i]].x;
		*v++=points[indices[i]].y;
		*v++=points[indices[i]].z;
	}

	glcanvas->setLineWidth(4);
	glcanvas->renderBatch(batch);
	glcanvas->setLineWidth(1);
}


////////////////////////////////////////////////////////////////////////
float Frustum::getScreenDistance( Vec3f P1,  Vec3f P2)
{
    P1 = this->project(P1); P1.z = 0;
    P2 = this->project(P2); P2.z = 0;
    float distance = (P2 - P1).module();
    return distance;
}


////////////////////////////////////////////////////////////////////////
bool Frustum::nearOnScreen(Vec3f P1, Vec3f P2,float min_distance_in_pixel)
{
    return getScreenDistance(P1,P2)<min_distance_in_pixel;
}
