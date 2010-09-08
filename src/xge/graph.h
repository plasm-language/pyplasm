#ifndef _GRAPH_H__
#define _GRAPH_H__

#include <xge/xge.h>
#include <xge/mempool.h>
#include <xge/mat.h>
#include <xge/plane.h>
#include <xge/box.h>
#include <xge/ball.h>
#include <xge/batch.h>
#include <xge/archive.h>

//predeclaration
class GraphKMem;
class GraphArch;
class GraphNode;
class GraphIterator;
class GraphListIterator;
class GraphNavigator;
class Graph;

//====================================================================================================================================
//! Class for fast memory allocation and (more important) reallocation since it is unsigned int based (see Graph code)
/*
Important: i didn't use C++ templates because here I need that the dimension
of a single item could change at run time (example: the geometric database of points
could change its size in a embedding operation)

Example in C++:

\code
	unsigned int Ki,Kj;
	GraphKMem m(sizeof(int));
	Ki=m.alloc();
	*((int*)m[Ki])=10;
	Kj=m.alloc();
	*((int*)m[Kj])=30;
	m.free(Ki);
	m.free(Kj);

\endcode
*/
//====================================================================================================================================


class XGE_API GraphKMem
{
public:
	void Write(Archive& ar)
	{
		ar.WriteInt("itemsize" ,this->m_itemsize);
		ar.WriteUint("max"     ,this->m_max);
		ar.WriteUint("num"     ,this->m_num);
		ar.WriteUint("ffree"   ,this->m_ffree);
		ar.WriteRaw ("mem",(char*)m_mem,(int)memsize());
	}

	void Read(Archive& ar)
	{
		int old_memsize=(int)memsize();
		this->m_itemsize=ar.ReadInt("itemsize");
		this->m_max     =ar.ReadUint("max"    );
		this->m_num     =ar.ReadUint("num"    );
		this->m_ffree   =ar.ReadUint("ffree"  );
		int new_memsize=(int)memsize();
		this->m_mem=MemPool::getSingleton()->realloc(old_memsize,this->m_mem,new_memsize);
		ar.ReadRaw("mem",(char*)m_mem,new_memsize);

	}

public:

	//!default constructor
	GraphKMem();

	//! constructor specifying item size
	GraphKMem(int itemsize);

	//! copy constructor
	GraphKMem(const GraphKMem& src);

	//! destructor
	~GraphKMem();

	//! assignment operator
	GraphKMem& operator=(const GraphKMem& src);

	//! flush memory
	inline void flush();

	//! access an item 
	#ifndef SWIG
	void* operator[](unsigned int K);
	#endif

	//! lock a new item
	unsigned int alloc();

	//! release a previous locked item
	void free(unsigned int K);

	//!make sure there is enough memory for a certain number of items
	/*!
		if you do not call needed every time you "block" a needed(1) is automatically called.
		This function speeds up the allocation for a bunch of items

		\param n is the number of items I need to block
	*/
	void needed(unsigned int n);

	//! number of elements currenty blocked
	unsigned int getNBlocked();

	//! number of m_max elements
	unsigned int getMax();

	//! size of single item
	size_t itemsize() const;

	//! return memsize
	size_t memsize() const;

	//! resize (change the item size)
	void resize(int new_itemsize);

	//! internal use print statistics
	void PrintStatistics();

	//! self test
	static int SelfTest();

protected:

	//! item size
	int                  m_itemsize;

	//! max items 
	unsigned int         m_max;

	//! number of locked elements
	unsigned int         m_num;
	
	//! first free item
	unsigned int         m_ffree;

	//! internal memory location
	void*                m_mem;

};


//=======================================================
//! Arch connecting graph nodes 
//=======================================================
class XGE_API GraphArch
{	
public:

	//!down node
	unsigned int   N0;
	
	//!up node
	unsigned int N1;

	//! ring of connection around N0
	unsigned int   N0_prev;
	
	//! ring of connection around N0
	unsigned int N0_next;

	//! ring of connection around N1
	unsigned int N1_prev;
	
	//! ring of connection around N1
	unsigned int N1_next;

	//! internal use info
	unsigned int   Info; 
};


//=====================================================
//!definition for a graph node
//=====================================================
class XGE_API GraphNode
{
public:

	//! level (example is zero for vertices)
	unsigned char  Level;
	
	//! sign (internal used by split algorithm)
	unsigned char Sign;

	//!first down arch
	unsigned int Dw;
	
	//!first up arch
	unsigned int Up;

	//! number of down nodes
	unsigned short Ndw;
	
	//! number of up arches
	unsigned short Nup;

	//! internal use only, to store list of nodes for a certain level (see Graph::nodelists and GraphListIterator())
	unsigned int Prev;

	//! internal use only, to store list of nodes for a certain level (see Graph::nodelists and GraphListIterator())
	unsigned int Next; 

	//! node Id
	unsigned int Id;

	//! id of geometric information of the geometry database
	unsigned int Info; 

	//! for dynamic information (used for example by GraphNavigator)
	unsigned int Tmp;  
}; 


//=================================================
//! class for navigating up and down nodes
//=================================================
class XGE_API GraphIterator
{
	friend class Graph;

protected:

	//! direction of the visit (up or down)
	int       dir;

	//! number of links (==arches) visited
	unsigned int    cont;

	//! total number of links (==arches) to visit
	unsigned int    max;

	//! current arch
	unsigned int    A;

	//! current ggraph
	Graph*   g;

public:
	
	//! default constructor
	GraphIterator();

	//! copy constructor
	GraphIterator(const GraphIterator& src);

	//! when all the arches have been visited
	bool end();

	//!access the current GraphNode
	unsigned int operator*();

	//!access the current GraphNode
	unsigned int getNode();

	//! access the current GraphArch
	unsigned int getArch();

	//! get the next arch to visit
	GraphIterator next();

	//! get the previous arch
	GraphIterator prev();

	//! move forward to the next arch
	GraphIterator& operator++(int);

	//! move forward to the next arch
	void goForward();

	//! move backward to the next arch
	GraphIterator& operator--(int);

	//! move backward to the next arch
	void goBackward();
};



//==============================================================
//! class to visit all cells at a certain level
//==============================================================
class XGE_API GraphListIterator
{
	friend class Graph;

protected:

	//! previous GraphNode
	unsigned int Prev;
	
	//! actual GraphNode
	unsigned int Cursor;

	//! current ggraph
	Graph* g;

public:

	//! default constructor
	GraphListIterator();

	//! copy constructor
	GraphListIterator(const GraphListIterator& src);

	//! when all nodes have been visited
	bool		end();

	//! access the actual GraphNode
	unsigned int	operator*();

	//! access the actual GraphNode
	unsigned int  getNode();

	//! get the previous node
	unsigned int	getPrevNode();

	//! move forward to the next node
	GraphListIterator& operator++(int);

	//! move forward to the next node
	void goForward();

	//! move forward without side effect
	GraphListIterator next();
};





//========================================================
//! a class which permits finding node in a Graph
//========================================================
class XGE_API GraphNavigator
{
	friend class Graph;

protected:

	//! (hidden) constructor copy (not allowed)
	GraphNavigator(GraphNavigator& src)
		{Utils::Error(HERE,"GraphNavigator has no copy constructor");} 

public:

	//! C array of number of nodes found for each level
	unsigned int*  nnav;

	//! C array of found nodes for each level
	unsigned int*  nav[GRAPH_MAX_GEOMETRIC_DIMENSION+2];

	//! default constructor
	GraphNavigator();

	//! destructor
	~GraphNavigator();

	//! return number of cells found at a certain level
	unsigned int getNCells(int level);

	//! get the found Node at a certain level with i in the range [0,nnav[level])
	unsigned int getCell(int level,int i);
};



//=====================================================
//! class which stores the full lattice (or hasse diagram) used by plasm
//=====================================================
class XGE_API Graph
{	
public:

	void Write(Archive& ar)
	{
		static int GRAPH_VERSION=1;
		ar.WriteInt("version",GRAPH_VERSION);
		ar.WriteInt("pointdim",pointdim);
		ar.Push("db")    ;db.Write(ar)     ;ar.Pop("db");
		ar.Push("arches");arches.Write(ar);ar.Pop("arches");
		ar.Push("nodes" );nodes.Write  (ar);ar.Pop("nodes");
		ar.WriteRaw("nodelists_v",(char*)nodelists_v,sizeof(nodelists_v));
		ar.WriteRaw("nodelists_n",(char*)nodelists_n,sizeof(nodelists_n));

		//do not write the batch
		//ar.Write("batch"   ,batch);
	}

	void Read(Archive& ar)
	{
		int version=ar.ReadInt("version");
		this->pointdim=ar.ReadInt("pointdim");
		ar.Push("db");db.Read(ar);ar.Pop("db");
		ar.Push("arches");arches.Read(ar);ar.Pop("arches");
		ar.Push("nodes");nodes.Read(ar);ar.Pop("nodes");
		ar.ReadRaw("nodelists_v",(char*)nodelists_v,sizeof(nodelists_v));
		ar.ReadRaw("nodelists_n",(char*)nodelists_n,sizeof(nodelists_n));

		//do not read the batch
		//ar.Read("batch"   ,batch);
	}


protected:

	//friendship
	friend class GraphIterator;
	friend class GraphNavigator;
	friend class GraphListIterator;



	//! point dimension
	int pointdim; 

	//! geometric db for 
	/*!
		<ul>
		<li>hyperplanes i.e. (pointdim-1)-cells for graph.pointdim>=2 
		<li>vertices i.e. 0-cells 
		<li>bounding-balls i.e. (pointdim()-cells for graph.pointdim>=2
		</ul>
	*/
	GraphKMem db; 

	//! db of all arches
	GraphKMem arches;

	//! db of all nodes
	GraphKMem nodes;

	//! cached return of a Graph::getBatch()
	SmartPointer<Batch> batch;

	//! nodelist of vertices, each cell can belong to only one list. The list is implicitly stored in node.Prev and node.Next variables
	unsigned int nodelists_v[256];

	//! number of nodes for each level
	unsigned int nodelists_n[256];

	//! /internal utility
	static void graph_cell_union        (Graph* g,bool bUp,unsigned int& ndstcells,unsigned int* dstcells,const unsigned int nsrccells,const unsigned int* srccells);

	///! internal utility
	static void graph_cell_intersection (Graph* g,bool bUp,unsigned int& ndstcells,unsigned int* dstcells,const unsigned int nsrccells,const unsigned int* srccells);

public:


	//! direction of an arch, can go onli upwward, downward or in both direction
	enum Direction 
	{
		DIRECTION_DOWN       =1,
		DIRECTION_UP         =2,
		DIRECTION_UP_AND_DOWN=3
	};


	//! sign for a cell (used in split to classify cells)
	enum SignCell
	{
		SIGN_ZER=0x00,
		SIGN_POS=0x01,
		SIGN_NEG=0x02,
		SIGN_UNK=0x03,
		SIGN_BOT=0x04
	};

	//! split result, the most important algorithm
	enum SplitResult
	{
		SPLIT_FLAT       =  0,
		SPLIT_OK         = +1,
		SPLIT_IMPOSSIBLE = -1
	};

	//! default constructor (0-DIM graph)
	Graph();

	//! contructor (pointdim-DIM graph)
	Graph(int pointdim);

	//! copy constructor
	Graph(const Graph& src);

	//! destructor
	~Graph();

	//! return pointdim, which is the dimension of points inside the geometric dayabase
	int getPointDim() const;

	//! fast clone of a graph
	SmartPointer<Graph> clone() const;

	//! assignment operator
	Graph& operator=(const Graph& src);

	//! clear memory (==clear all the graph, so remove all cells, links,geometric database and so on)
	void clear();

	//! return the GraphArch data knowing its id
	GraphArch&        getArch         (unsigned int A);


	//! return the GraphNode data knowing its id
	GraphNode&        getNode         (unsigned int N);

	//! return number of down arch of a GraphNode
	unsigned short    getNDw          (unsigned int N);

	//! get number of up arches of a GraphNode
	unsigned short    getNUp          (unsigned int N);

	//! return total number of nodes used in this graph
	unsigned int            getNumNode      ()        ;

	//! return total number of arches used in this graph
	unsigned int            getNumArch      ()        ;

	//! return total number of locked nodes 
	unsigned int            getMaxNode      ()        ;
	
	//! return total number of locked arches
	unsigned int            getMaxArch      ()        ;

	//! return the first down arch of a node, or 0 if not exists
	unsigned int            getFirstDwArch  (unsigned int N);

	//! return the first up arch of a node, or 0 if not exists
	unsigned int            getFirstUpArch  (unsigned int N);

	//! return the last down arch of a node, or 0 if not exists
	unsigned int            getLastDwArch   (unsigned int N);

	//! return the last up arch of a node, or 0 if not exists
	unsigned int            getLastUpArch   (unsigned int N);

	//! return the first down node, or 0 if not exist
	unsigned int            getFirstDwNode  (unsigned int N);

	//! return the first up node, or 0 if not exists
	unsigned int            getFirstUpNode  (unsigned int N);

	//! return the last down node, or 0 if not exists
	unsigned int            getLastDwNode   (unsigned int N);

	//! return the last up node, or 0 if not exists
	unsigned int            getLastUpNode   (unsigned int N);

	//! return the next down arch (given the actual GraphArch A), or 0 if not exists
	unsigned int            getNextDwArch   (unsigned int A);

	//! return the previous down arch (given the actual GraphArch A), or 0 if not exists
	unsigned int            getPrevDwArch   (unsigned int A);

	//! return the next up arch (given the actual GraphArch A), or 0 if not exists
	unsigned int            getNextUpArch   (unsigned int A);

	//! return the previous up arch (given the actual GraphArch A), or 0 if not exists
	unsigned int            getPrevUpArch   (unsigned int A);

	//! return the starting GraphNode (N0) of an arch
	unsigned int            getN0           (unsigned int A);

	//! return the ending GraphNode (N1) of an arch
	unsigned int            getN1           (unsigned int A);


	//! return the level of a GraphNode
	const unsigned char&      Level           (unsigned int N);

	//! return the sign of a GraphNode
	unsigned char&            Sign            (unsigned int N);

	//! return the data of a GraphNode
	unsigned int&           NodeData        (unsigned int N);

	//! return the data of a GraphArch
	unsigned int&           ArchData        (unsigned int A);

	//! return the temporary infos about a GraphNode
	unsigned int&           NodeTmp         (unsigned int N);

	//! remove an arch (eventually specifying which direction to remove)
	void		            remArch         (unsigned int N0,unsigned int N1,Direction dir=DIRECTION_UP_AND_DOWN) ;

	//! get number of cells at a certain level
	int                     getNCells       (int L);

	//! internal function only
	unsigned int            getNextUpBrother(unsigned int A);

	//! internal function only
	unsigned int            getNextDwBrother(unsigned int A);

	//! internal function only
	unsigned int            getPrevUpBrother(unsigned int A);

	//! internal function only
	unsigned int            getPrevDwBrother(unsigned int A);

	//! find all down arches/node of a GraphNode
	/*!
		@cpp
		for (GraphIterator it=graph.goDw(Node);!it.end();it++)
		{
			unsigned int Arch    =it.getArch();
			unsigned int DownNode=*it; //or it.getNode()
		}
		@endcpp
	*/
	GraphIterator goDw(unsigned int N);

	//! find all up arches/node of a GraphNode
	/*!
		@cpp
		for (GraphIterator it=graph.goUp(Node);!it.end();it++)
		{
			unsigned int Arch   =it.getArch();
			unsigned int UpNode=*it; //or it.getNode()
		}
		@endcpp
	*/
	GraphIterator goUp(unsigned int N);

	//! visit all cell at a certain level
	/*!
		@cpp
		//visit all 0-cells which are vertices
		for (GraphListIterator it=graph.each(0);!it.end();it++) 
		{
			unsigned int Vertex=*it;
		}
		@endcpp
	*/
	GraphListIterator each(unsigned char L) ;

	//! add a node at a certain level
	/*!
		Remember that you must add geometric information (NodeData) if
		<ul>
			<li> (L==0                                    ) because the new cell is a 0-dim cell and so it is a vertex (in this case it's better to use Graph::addVertex())
			<li> (L==graph.pointdim-1 && graph.pointdim>=2) because the new cell is a boundary plane ad so it needs the plane equation (in this case it's better to use Graph::addPlane())
			<li> (L==graph.pointdim   && graph.pointdim>=2) because the new cell should contain the bounding ball infos
		</ul>
	*/
	unsigned int addNode(unsigned char L);

	//!add a vertex, its dimension must be compatible with graph.pointdim
	unsigned int addVertex(const Vecf& v);

	//! add a vertex using a C float array
	unsigned int addVertex(const float* src);

	//! add an edge between two nodes 
	unsigned int addEdge(unsigned int N0,unsigned int N1);

	//! add all necessary 1-dim cell (edges) and one 2-dim cell (face) using vertices id.
	unsigned int addFace2d(const std::vector<int>& verts);

	//!add a (pointdim-1) cell, which is a boundary plane for the graph
	unsigned int addPlane(const Vecf& h);

	//! add arch from N0 to N1, eventually specifying it's direction
	/*!
		\param _A0 internal use only
		\param _A1 internal use only
	*/
	unsigned int addArch(unsigned int N0,unsigned int N1,Direction dir=DIRECTION_UP_AND_DOWN,unsigned int _A0=0,unsigned int _A1=0);

	//! add an arch in only one direction (internal use only)
	void addArchDirection(unsigned int A,Direction dir,unsigned int _A0=0,unsigned int _A1=0);

	//! find an arch from N0 to N1, return 0 if not found
	unsigned int findArch(unsigned int N0,unsigned int N1,Direction dir=DIRECTION_UP_AND_DOWN);

	//! slow find a vertex, looking in all 0-cells of the graph
	unsigned int findVertex(const Vecf& v);

	//! find a common node, in up or down direction, which is linked both to N and M
	unsigned int findFirstCommonNode(unsigned int N,unsigned int M,Direction dir=DIRECTION_UP_AND_DOWN);

	//! remove an arch, eventually specifying the direction you want to remove
	void remArch(unsigned int arch,Direction dir=DIRECTION_UP_AND_DOWN);

	//! swap arch order (internal use only)
	void swapDwOrder(unsigned int A,unsigned int B);

	//! change the level of a cell (internal use only)
	void changeLevel(unsigned int N,unsigned char Lnew);

	//! return max-dim cells
	int getMaxDimCells();

	//! plasm join operation (used for example by Graph::simplex()), returns a new Graph without modifying the current one
	SmartPointer<Graph> join(unsigned int N,float coord);

	//! order a 2-face (internal use only)
	bool orderFace2d(unsigned int face2);

	//! debug print
	void Print           (unsigned int N,char* space="") ;

	//! debug print
	void Print           (char* space="") ;

	//! print Graph statistics 
	void PrintStatistics () ;

	//! find all cells at a certain level
	int findCells(int level,unsigned int Cell,GraphNavigator& Nav,bool bFast=true);

	//! remove a node from the graph (and all arches "entering" this Node)
	/*!
		\param recursive if true then all nodes which becomes orfans (no arches entering it) will be removed too
	*/
	void remNode(unsigned int N,bool recursive=true);

	//! access the geometric Id of a cell
	/*!
		Basically the geometric information is available only for vertices, planes and bounding ball
		(the last two only then graph.pointdim>=2)
	*/
	float* getGeometry(unsigned int N,bool bCreateIfNotExists=false);

	//! get the geometry of a cell
	Vecf getVecf(unsigned int N);
	
	//! set the geometric information of a cell
	/*!
		Basically the geometric information is available only for vertices, planes and bounding ball
		(the last two only then graph.pointdim>=2)
	*/
	void setVecf(unsigned int N,Vecf v);

	//! release the geomemetric information of a cell
	void releaseGeom(unsigned int GraphNode);


	//! build a new graph containing a cuboid
	static SmartPointer<Graph> cuboid(int pointdim,float From=0.0f,float To=1.0f);

	//! build a new graph containing a cuboid
	static SmartPointer<Graph> cuboid(int pointdim,Boxf box);

	//! build a new graph simplex a simplex
	static SmartPointer<Graph> simplex(int pointdim);

	//! get the bounding box of a cell  (if C!=0) or of the all graph (if C==0)
	/*!
		\param vmat apply vmat to all vertices
		\param hmat apply hmat to all planes   
	*/
	Boxf getBoundingBox(unsigned int C=0,SmartPointer<Matf> vmat=SmartPointer<Matf>(),SmartPointer<Matf> hmat=SmartPointer<Matf>());

	//! get the bounding ball of a cell (C>0 otherwise error)
	/*!
		\param vmat apply vmat to all vertices
		\param hmat apply hmat to all planes   
	*/
	Ballf getBoundingBall(unsigned int C,SmartPointer<Matf> vmat=SmartPointer<Matf>(),SmartPointer<Matf> hmat=SmartPointer<Matf>());

	//! get the best fitting plane of a cell (C>0 otherwise error)
	/*!
		\param vmat apply vmat to all vertices
		\param hmat apply hmat to all planes   
	*/
	Planef getFittingPlane(unsigned int C,SmartPointer<Matf> vmat=SmartPointer<Matf>(),SmartPointer<Matf> hmat=SmartPointer<Matf>());

	//! transform the graph to be contained into unit box [-1,-1,-1...] [+1 +1 +1...] (side effect)
	/*!
		\param vmat the vertex transformation which has been applied
		\param hmat the plane  transformation which has been applied 
	*/
	void toUnitBox(Matf& vmat,Matf& hmat);

	//! fix boundary plane orientation of face F which must be a (pointdim-1)-cell with graph.pointdim>=2
	/*!
		\param Face if Face=0 then fix all boundary faces
	*/
	void fixBoundaryFaceOrientation(unsigned int Face);

	
	//! translate a graph
	void translate   (Vecf vt);

	//! scale a graph
	void scale       (Vecf vs);

	//! rotate a graph
	void rotate      (int ax1,int ax2,float alpha);

	//! transform a graph applying vertex and plane matrix transformation)
	void transform   (SmartPointer<Matf> pmat,SmartPointer<Matf> hmat);

	//! permutate geometric information (internal use only)
	void permutate   (const std::vector<int>& permutation);

	//! triangulate cell C 
	/*!
		\param C if C==0 then triangulate all cells at all level
	*/
	void triangulate (unsigned int C);

	//! embed the graph in pointdim (implies a change in geometric database, internal use only)
	void embed       (int pointdim);

	//! extrude a graph (used by Graph::cuboid()) 
	SmartPointer<Graph> extrude(float From,float To);

	//! mkpol builds a new graph 
	/*!
		\param VmatT will be the "embedding" vertex transformation matrix if the result it's not fully dimensional i.e. spacedim!=pointdim
		\param HmatT will be the "embedding" plane  transformation matrix if the result it's not fully dimensional i.e. spacedim!=pointdim
		\param pointdim point dimension
		\param npoints  number of points
		\param points is a C float array without the homo component. for example if pointdim==3 x1 y1 z1 x2 y1 z2 ...
		\param tolerance used by QHull
	*/
	static SmartPointer<Graph> mkpol(Matf& Vmat,Matf& Hmat,int pointdim,int npoints,const float* points,float tolerance);

	//! mkpol builds a new graph 
	/*!
		\param VmatT will be the "embedding" vertex transformation matrix if the result it's not fully dimensional i.e. spacedim!=pointdim
		\param HmatT will be the "embedding" plane  transformation matrix if the result it's not fully dimensional i.e. spacedim!=pointdim
		\param pointdim point dimension
		\param npoints  number of points
		\param points is a STL std::vector<float>without the homo component. for example if pointdim==3 x1 y1 z1 x2 y1 z2 ...
		\param tolerance used by QHull
	*/
	static SmartPointer<Graph> mkpol(Matf& Vmat,Matf& Hmat,int pointdim,int npoints,std::vector<float> points,float tolerance);

	//! plasm power
	/*!
		\param VmatT will be the "embedding" vertex transformation matrix if the result it's not fully dimensional i.e. spacedim!=pointdim
		\param HmatT will be the "embedding" plane  transformation matrix if the result it's not fully dimensional i.e. spacedim!=pointdim
		\param _g1 the first argument
		\param _g1vmat the vertex embedding transformation matrix if _g1 is not fully dimensional 
		\param _g1hmat the plane  embedding transformation matrix if _g1 is not fully dimensional 
		\param _g2 the second argument
		\param _g2vmat the vertex embedding transformation matrix if _g2 is not fully dimensional 
		\param _g2hmat the plane  embedding transformation matrix if _g2 is not fully dimensional 
	*/
	static SmartPointer<Graph> power(Matf& VmatT,Matf& HmatT,
						              const SmartPointer<Graph> _g1,SmartPointer<Matf> _g1vmat,SmartPointer<Matf> _g1hmat,
						              const SmartPointer<Graph> _g2,SmartPointer<Matf> _g2vmat,SmartPointer<Matf> _g2hmat);

	//! split a cell (most important algorithm, should be robust enough)
	/*!
		\param navigator it is used to find cells and must be provided from outside
		\param cell the full! cell to split (should be a graph.pointdim-cell even if it is supported the splitting of not full cells)
		\param _h the splitting plane
		\param start_tolerance the start tolerance to do cell classification
		\param max_try if split fails it will perturbate the plane and change the tolerance, up to max_try times (i.e. if the cell is too scrambled, the split can fail
		\param Cb return the splitted cell in below space
		\param Ca return the splitted cell in above space
		\param Ce return the cell on the plane (if the split has been done)
		\param Id internal use to keep track of cell origins
		\return the result of the split
	*/
	SplitResult split(GraphNavigator& navigator,const unsigned int cell,const Planef& _h,float start_tolerance,const int max_try,unsigned int& Cb,unsigned int& Ca,unsigned int& Ce,unsigned int Id);

	//! display in opengl all the cells at a certain level, very slow only for debugging and internal use
	//void render(int view_cell_level,bool bDisplayOffetLines,bool bShowNormals,bool bDisplayCellId);


	//! safety check of a node (internal use only)
	void check(unsigned int N);

	//! safety check of a graph (internal use only)
	void check();

	
	//! test if a graph.point-cell (i.e. a full cell) contains a point (if C==0 then test all cells)
	/*!
		probably need a little more test\n
		i choosed not to assume faces are correctly oriented because I can call this function on cells which have internal faces and cannot be oriented\n
		\param tolerance: when the centroid is too near the boundary hyperplanes i cannot orient correctly the face and so I'm confused by in/out classification
	*/
	bool contains(unsigned int C,const Vecf& point,float tolerance);

	//! import svg file (beta version)
	static SmartPointer<Graph> open_svg(std::string filename,bool bFull);
	
	//! self test
	static int SelfTest();

	//! get Batch for fast rendering (the Batch will contain only vertex and normals, the only information a Graph have)
	/*!
		Internally a Graph cache a Batch, so if you call this function two times, the second time it will return the same cached result
		This can cause problems if the graph is evolving in time. It's not the case of Plasm library in which Graph do not modify after 
		they have been created.
	*/	
	SmartPointer<Batch> getBatch();

};


//inlines
#include <xge/graph.hxx>


#endif //_GRAPH_H__
