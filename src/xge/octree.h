#ifndef _OCTREE_H__
#define _OCTREE_H__

#include <stack>
#include <queue>
#include <set>

#include <xge/xge.h>
#include <xge/archive.h>
#include <xge/box.h>
#include <xge/frustum.h>
#include <xge/ray.h>


//====================================================
//! stores a single node of the loose octree of Scene
//! the entire class is thread safe (protected by lock) if OCTREE_USE_LOCK
//====================================================



class XGE_API OctreeNode
{
public:

	//! constructor for root
	explicit OctreeNode(Box3f world_box) ;
	
	//! constructor for a child inserted in the slot <slot> insie the parent
	explicit OctreeNode(OctreeNode* parent,int slot,Box3f box) ;
	
	//~ destructor
	~OctreeNode();

	//! get the unique id for the node, important key for storing objects
	uint64 getId();

	//! retrieve a node from it's id
	OctreeNode* getNode(uint64 id);

public:

	//! octree depth (root has depth 0, child of root has depth 1 etc etc), it is needed for precedence of nodes (see operator <)
	int  depth;

	//! parent node, if this is the root of the treee (i.e. its depth is 0) then the parent is null
	OctreeNode* parent;

	//! scene node child
#ifndef SWIG
	OctreeNode* childs[8];
#endif

	inline OctreeNode* getChild(int i)
	{
		return childs[i];
	}


	//! bounding box for this scene node
	Box3f  box;

	std::vector<SmartPointer<Batch> > batches;



}; //end class



/////////////////////////////////////////////////////////////////////////
//inlines for OctreeNode
/////////////////////////////////////////////////////////////////////////
inline OctreeNode::OctreeNode(Box3f world_box) 
{
	this->parent=0;
	this->depth =0;
	memset(childs,0,sizeof(childs));
	this->box=world_box;
}

inline OctreeNode::OctreeNode(OctreeNode* parent,int slot,Box3f box) 
{
	ReleaseAssert(parent && slot>=0 && slot<8 && box.isValid());

	this->parent =parent;
	this->depth  =parent->depth+1;
	this->box=box;
	memset(this->childs,0,sizeof(childs));

	//set the parent child
	ReleaseAssert(parent->childs[slot]==0);
	parent->childs[slot]=this;
}


inline OctreeNode::~OctreeNode()
{
	//avoid deadlock disconnect from parent
	if (this->parent)
	{
		for (int i=0;i<8;i++)
		{
			if (parent->childs[i]==this)
			{
				parent->childs[i]=0;
				break;
			}
		}
		this->parent=0;
	}

	//recursive destructor
	for (int i=0;i<8;i++)
	{
		if (this->childs[i])
		{
			//safe instruction without a lock since I'm destroying it
			this->childs[i]->parent=0;
			delete this->childs[i];
		}
	}
}

inline uint64 OctreeNode::getId()
{
	uint64 ret=(uint64)this;
	return ret;
}

inline OctreeNode* OctreeNode::getNode(uint64 id)
{
	return (OctreeNode*)id;
}




//==========================================
//! find objects in scene by frustum intersection
//==========================================

class XGE_API FrustumIterator
{
	
private:

	//! class for priority queue
	class PqItem
	{
	public:
		
		OctreeNode* node;
		float t; //! approximate object bounding box distance from the frustum
		int depth;
	
		//! constructor from node and ray
		inline explicit PqItem(OctreeNode* node,Ray3f ray,bool bFrontToBack)
		{
			this->node=node;
			this->depth=node->depth;

			Box3f box=node->box;

			if (box.contains(ray.origin))
				{this->t=0;return;}

			Vec3f P;
			if (bFrontToBack)
			{
				//nearest point
				P=Vec3f(
					(ray.dir.x>=0)? box.p1.x: box.p2.x,
					(ray.dir.y>=0)? box.p1.y: box.p2.y,
					(ray.dir.z>=0)? box.p1.z: box.p2.z);

				this->t=+1*Plane4f(ray.dir,ray.origin).getDistance(P);
			}
			else
			{
				//farest point
				P=Vec3f(
					(ray.dir.x>=0)? box.p2.x: box.p1.x,
					(ray.dir.y>=0)? box.p2.y: box.p1.y,
					(ray.dir.z>=0)? box.p2.z: box.p1.z);

				this->t=-1*Plane4f(ray.dir,ray.origin).getDistance(P);
			}
		}
	
		//! comparison operator for priority queue
		#ifndef SWIG
		friend bool operator<(const PqItem& a,const PqItem& b) 
		{
			return (a.t >  b.t) || (a.t == b.t && a.depth > b.depth);
		}
		#endif
		
	};  //end class Item


	//! the current frustum
	Frustum* frustum;

	//! the main ray of the frustum
	Ray3f ray;
	
	//! next nodes to visit (ordered by distance)
	std::priority_queue<PqItem> pq;

	//! visit ordering
	bool bFrontToBack;

public:

	//! default constructor
	FrustumIterator()
		{;}

	//! constructor  from root and frustum 
	explicit FrustumIterator(OctreeNode* root,Frustum* frustum,bool bFrontToBack=true)
	{
		this->bFrontToBack=bFrontToBack;
		this->frustum=frustum;
		this->ray=frustum->getRay();
		
		if (!root || !frustum->intersect(root->box)) 
			return;
			
		pq.push(PqItem(root,ray,bFrontToBack));
	}

	//! test if no more objects to visit
	bool end()
		{return pq.size()==0;}

	//! current node intersecting frustum
	OctreeNode* getNode()
	{
		DebugAssert(!end());
		return pq.top().node;
	}

	//! get the actual min distance
	inline float getDistance()
	{
		DebugAssert(!end());
		return pq.top().t;
	}

	//! go to the next node intersecting the frustum
	void moveNext()
	{
		DebugAssert(!end());
	
		PqItem current=pq.top();
		pq.pop();

		for (int i=0;i<8;i++)
		{
			OctreeNode* child=current.node->childs[i];
			if (child && frustum->intersect(child->box)) 
				pq.push(PqItem(child,ray,bFrontToBack));
		}
	}
};


//==========================================
//! find objects in scene by ray intersection
//==========================================

class XGE_API RayIterator
{
	
private:

	class XGE_API PqItem
	{
	public:
	
		//! current node
		OctreeNode* node;
	
		//! minimum distance from ray
		float tmin;
	
		//! constructor from node and minimum distance
		inline explicit PqItem(OctreeNode* node,float tmin)
		{
			this->node=node;
			this->tmin=tmin;
		}
	
		//! internal use operator from scene ray intersection STL priority queue
		#ifndef SWIG
		friend bool operator<(const PqItem& a,const PqItem& b)  
		{
			return (a.tmin > b.tmin);
		}
		#endif
	};  
	
	//! current ray
	Ray3f ray;

	//! minimum distance from ray
	float min_distance;

	//! maximum distance from ray
	float max_distance;
	
	//! next nodes to visit
	std::priority_queue<PqItem> pq;
	

public:

	//! constructor from scene root, ray and valid range (minimum/maximum distance)
	explicit RayIterator(OctreeNode* root,Ray3f ray,float min_distance=0.0f,float max_distance=+1e18f)
	{
		//config
		this->ray=ray;
		this->min_distance=min_distance;
		this->max_distance=max_distance;

		float tmin=0,tmax=1e18f;
		
		//NOTA: posso inserire oggetti che sono piu' grandi del bounding box del world
		//in questo caso voglio che almeno il nodo root sia sempre restituito, perche'
		//e' proprio sul root che inserirei gli oggetti piu' grandi
		#if 0
		if (!root || !ray.intersectBox(tmin,tmax,root->box)) return; //cannot find any solution
		if (!overlap1d(tmin,tmax,min_distance,max_distance)) return; //cannot find any solution
		pq.push(PqItem(root,tmin));
		#else

		if (!root) return;
		pq.push(PqItem(root,tmin));

		#endif
	}

	//! test if no more objects intersecting ray
	inline bool end()
		{return pq.size()==0;}

	//! get the current node
	inline OctreeNode* getNode()
	{
		DebugAssert(!end());
		return pq.top().node;
	}

	//! get the actual min distance
	inline float getMinDistance()
	{
		DebugAssert(!end());
		return pq.top().tmin;
	}

	//! go to the next intersecting GdbNode
	void moveNext()
	{
		DebugAssert(!end());
		
		PqItem current=pq.top();
		pq.pop();

		//push in next
		for (int i=0;i<8;i++)
		{
			OctreeNode* child=current.node->childs[i];
			if (child)
			{
				float tmin,tmax;
				if (ray.intersectBox(tmin,tmax,child->box) && Utils::Overlap1d(tmin,tmax,min_distance,max_distance))
					pq.push(PqItem(child,tmin));
			}
		}
	}	
};


//==========================================================================================
//! The main database for geometric objects
/*!
	This geometric database can handle a lot of objects. Think of it as an huge octree
	where objects are placed in scene nodes and you can find objects by doing basic queries
	(as box intersection, frustum intersection, ray intersection).

	Since geometric objects are stored in a efficient way, it is possible for example
	to render the scene in a "progressive" way (from near to far); therefore even if the
	scene is very large (and you would get a very poor FPS), using this DB you will
	always have interactivity. As you move the SceneViewer will stop the current display
	and start a new one.

	The DB is a standard Octree but with Loose object placement:

	see Game Programming Gems, Loose Octree  by Thatcher Ulrich

	see http://www.gamedev.net/community/forums/topic.asp?topic_id=254839

	see http://yayamoose.homelinux.com/~ripper/mirrors/tulrich.com/geekstuff/partitioning.html

*/
//==========================================================================================
class XGE_API Octree 
{
public:

	//! the world box of the entire scene
	/*!
		Must be setup once at the beginning and cannot change.  If the world box changes
		the entire loose octree must be recalculated.

		If an object is outside the world box, it will be added in the root 
		(which is not so good since it will be considered very high-priority node).
	*/
	Box3f world_box;



	//! root node
	OctreeNode* root;

	//! the center of the world box (stored for efficiency)
	Vec3f world_center;

	//! max dimension of the world box
	float world_W;

	//! max depth
	int max_depth;
	
	//! loose factor for loose octree (should be tuned depending on the scene
	float LOOSE_K;

	//! constructor from workd box and max depth (looseK=1 means ordinary ordinary octree, without loosing factor)
	/*!
		\param world_box the bounding box of the entire scene which should not change
		\param max_depth it's not obvious how to set this value. A very big value (example max_depth 255 produce inefficient octrees, to tune)
		\param LOOSE_K the loose factor, use 1 if you want to try ordinary octree, it's not to clear how to choose the value. 2 seems good enough
	*/
	explicit Octree(Box3f world_box,int max_depth=OCTREE_MAX_DEPTH_DEFAULT,float LOOSE_K=OCTREE_LOOSE_K_DEFAULT);


	//! constructor from batches
	explicit Octree(const std::vector<SmartPointer<Batch> >& batches,int max_depth=OCTREE_MAX_DEPTH_DEFAULT,float LOOSE_K=OCTREE_LOOSE_K_DEFAULT); 

	//! destructor, destroys all GdbNodes inside
	//! Note: all objects inside nodes should be deallocated manually from outside
	~Octree();

	//if you can have collision
	inline void Lock  () {m_lock.Lock();}
	inline void Unlock() {m_lock.Unlock();}

	//get world box
	inline Box3f getBox()
		{return world_box;}

	//! total number of nodes inside this scene node
	int getNumNodes(OctreeNode* node=0);

	//! maximum depth, counting from this node
	int getMaxDepth(OctreeNode* node=0);

	//! maximum depth, counting from this node
	int getNumBatches(OctreeNode* node=0);

	//print debug info
	void printStatistics();

	//! get the sub-node given a child box
	//! note that obj is not deallocated after the ~Octree, but MUST be manually deallocated
	OctreeNode* getNode(const Box3f& obj_box,OctreeNode* node=0);

	//! find objects intersecting frustum LOOSELY ordered by ray-distance (i.e. distance of object-bounding-box nearest point and the main ray of the frustum (see Frustum::getRay())
	FrustumIterator find(Frustum* frustum,bool bFrontToBack=true) {return FrustumIterator(this->root,frustum,bFrontToBack);}

	//! find objects  intersecting ray STRICTLY ordered by distance from ray.position
	RayIterator find(const Ray3f& ray,float min_distance=0,float max_distance=+1e18f) {return RayIterator(this->root,ray,min_distance,max_distance);}


private:

	//! for collisions, use Lock/Unlock if you need!
	SpinLock m_lock;

	//! ordinary edge cube length at deep depth (see loose octree)
	float edgeLen(int depth) const;

	//! loose edge cube length at deep depth
	float edgeLooseLen(int depth) const;


}; //end class

#endif  //_OCTREE_H__




