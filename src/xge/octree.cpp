#include <xge/xge.h>
#include <xge/octree.h>


//! ordinary edge cube length at deep depth (see loose octree)
inline float Octree::edgeLen(int depth) const
	{return world_W/Utils::Pow2i(depth);}

//! loose edge cube length at deep depth
inline float Octree::edgeLooseLen(int depth) const
	{return LOOSE_K*edgeLen(depth);}



///////////////////////////////////////////////////////////////////////////////////////
Octree::Octree(Box3f world_box,int max_depth,float LOOSE_K)
{
	this->LOOSE_K=LOOSE_K;
	this->max_depth=max_depth;

	this->world_box=world_box;
	this->world_center=world_box.center();
	this->world_W=world_box.maxsize();
	
	Box3f octree_box=Box3f::buildFromCenter(this->world_center,Vec3f(this->world_W,this->world_W,this->world_W)).scale(LOOSE_K);
	this->root=new OctreeNode(octree_box);
}


///////////////////////////////////////////////////////////////////////////////////////
Octree::Octree(const std::vector<SmartPointer<Batch> >& batches,int max_depth,float LOOSE_K)
{
	this->LOOSE_K=LOOSE_K;
	this->max_depth=max_depth;


	for (int i=0;i<(int)batches.size();i++)
		this->world_box.add(batches[i]->getBox());

	this->world_center=this->world_box.center();
	this->world_W=this->world_box.maxsize();
	
	Box3f octree_box=Box3f::buildFromCenter(this->world_center,Vec3f(this->world_W,this->world_W,this->world_W)).scale(LOOSE_K);
	this->root=new OctreeNode(octree_box);

	Clock t1;
	Log::printf("Building octree from %d batches....\n",(int)batches.size());
	{
		for (int i=0;i<(int)batches.size();i++)
		{
			OctreeNode* node=this->getNode(batches[i]->getBox());
			node->batches.push_back(batches[i]);
		} 
	}
	this->printStatistics();
	Log::printf("...done in %d msec\n",(int)t1.msec());
}

///////////////////////////////////////////////////////////////////////////////////////
Octree::~Octree()
{
	if (this->root)
		delete this->root;
}


///////////////////////////////////////////////////////////////////////////////////////
int Octree::getNumNodes(OctreeNode* node)
{
	if (!node) node=this->root;
	int ret=1;//this node
	for (int i=0;i<8;i++) 
	{
		OctreeNode* child=node->childs[i];
		if (child)
			ret+=this->getNumNodes(child);
	}
	return ret;
}


///////////////////////////////////////////////////////////////////////////////////////
int Octree::getNumBatches(OctreeNode* node)
{
	if (!node) node=this->root;
	int ret=(int)node->batches.size();

	for (int i=0;i<8;i++) 
	{
		OctreeNode* child=node->childs[i];
		if (child) ret+=this->getNumBatches(child);
	}
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////
int Octree::getMaxDepth(OctreeNode* node)
{
	if (!node) node=this->root;
	int ret=node->depth;
	for (int i=0;i<8;i++) 
	{
		OctreeNode* child=node->childs[i];
		if (child) 
		{
			int temp=this->getMaxDepth(child);
			ret=max2(ret,temp);
		}
	}
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////
void Octree::printStatistics()
{
	Log::printf("Scene number of nodes of the octree %d\n",this->getNumNodes());
	Log::printf("Scene max depth                     %d\n",this->getMaxDepth());
	Log::printf("Scene number of batches             %d\n",this->getNumBatches());
}


///////////////////////////////////////////////////////////////////////////////////////
OctreeNode* Octree::getNode(const Box3f& obj_box,OctreeNode* node)
{
	//first call
	if (!node) node=this->root;

	int depth=node->depth;
	Box3f node_box=node->box;

	// Check child nodes to see if object fits in one of them.
	if ((depth+1) < max_depth) 
	{
		//dimension of box in ordinary octree (not the one stored in node, which is a loose octree dimension which uses LOOSE_K)
		float offset=0.25f*edgeLen(depth);

		
		
		Vec3f Cn = node_box .center(); //center of node
		Vec3f Co  =obj_box  .center(); //center of object

		//coordinates of child
		int i=(Co.x <= Cn.x)?0:1;
		int j=(Co.y <= Cn.y)?0:1;
		int k=(Co.z <= Cn.z)?0:1;

		//center of child
		Vec3f Cc(Cn.x + offset*((i==0)?-1:+1), Cn.y + offset*((j==0)?-1:+1), Cn.z + offset*((k==0)?-1:+1));

		//child box
		float child_loose_len=edgeLooseLen(depth+1);
		Box3f Cbox=Box3f::buildFromCenter(Cc,Vec3f(child_loose_len,child_loose_len,child_loose_len));

		//VERY important this box contains the child box
		DebugAssert(node_box.contains(Cbox));

		//entirely fits in the child
		if (Cbox.contains(obj_box)) 
		{
			int idx=(k<<2)+(j<<1)+i;

			OctreeNode* child=node->childs[idx];

			if (!child) 
				new OctreeNode(node,idx,Cbox);

			return getNode(obj_box,node->childs[idx]);
		}
	}
	// Keep object in this node
	if (!node_box.contains(obj_box))
		Log::printf("Warning from Octree, the inserted object is not contained in World box\n");

	return node;
}

