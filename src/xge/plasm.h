#ifndef _TRS_PLASM_H__
#define _TRS_PLASM_H__

#include <xge/xge.h>
#include <xge/archive.h>
#include <xge/clock.h>
#include <xge/mat.h>
#include <xge/graph.h>
#include <xge/octree.h>

//! counter of all allocated hpcs (for debugging purpouse)
extern "C" XGE_API unsigned long xge_total_hpc;


//! typedef for Plasm properties inside Hpc
typedef std::map<std::string,std::string> PropertySet;

//! some common properties name
#define HPC_PROP_NAME           "name"
#define HPC_PROP_RGB_COLOR      "RGBcolor"
#define HPC_PROP_VRML_MATERIAL  "VRMLmaterial"
#define HPC_PROP_VRML_TEXTURE   "VRMLtexture"
#define HPC_PROP_VRML_CREASE    "VRMLcrease"
#define HPC_PROP_VRML_CAMERA    "VRMLcamera"
#define HPC_PROP_VRML_LIGHT     "VRMLlight"


//===================================================
//! hpc plasm node, a plasm "object" is a tree of interconnected hpc nodes
//===================================================
class XGE_API Hpc 
{
public:
	void Write(Archive& ar)
	{
		static int HPC_VERSION=1;

		ar.WriteInt("version",HPC_VERSION);
		ar.WriteInt("spacedim",spacedim);
		ar.WriteInt("pointdim",pointdim);

		ar.Push("vmat");
		ar.WriteSmartPointer(vmat);
		ar.Pop("vmat");

		ar.Push("hmat");
		ar.WriteSmartPointer(hmat);
		ar.Pop("hmat");

		//properties
		ar.Push("prop");
		{
			int num=this->prop ? (int)this->prop->size() : 0;
			ar.WriteInt("num",num);
			if (this->prop)
			{
				for (std::map<std::string,std::string>::iterator it=this->prop->begin();it!=this->prop->end();it++) 
				{
					ar.Push("item");
					ar.WriteString("key"  ,it->first);
					ar.WriteString("value",it->second);
					ar.Pop("item");
				}
			}
		}
		ar.Pop("prop");

		ar.Push("g");
		ar.WriteSmartPointer(g);
		ar.Pop("g");

		ar.Push("batches");
		{
			ar.WriteInt("num",(int)batches.size());
			for (int i=0;i<(int)batches.size();i++) 
			{
				ar.Push("batch");
				ar.WriteSmartPointer(batches[i]);
				ar.Pop("batch");
			}
		}
		ar.Pop("batches");

		ar.Push("childs");
		{
			ar.WriteInt("num",(int)childs.size());
			for (int i=0;i<(int)childs.size();i++) 
			{
				ar.Push("Hpc");
				ar.WriteSmartPointer(childs[i]);
				ar.Pop("Hpc");
			}
		}
		ar.Pop("childs");
	}

	void Read(Archive& ar)
	{
		int version=ar.ReadInt("version");
		this->spacedim=ar.ReadInt("spacedim");
		this->pointdim=ar.ReadInt("pointdim");
		ar.Push("vmat");vmat=ar.ReadSmartPointer<Matf>();ar.Pop("vmat");
		ar.Push("hmat");hmat=ar.ReadSmartPointer<Matf>();ar.Pop("hmat");

		//properties
		ar.Push("prop");
		{
			int num=ar.ReadInt("num");

			if (num)
				this->prop.reset(new PropertySet);
			else
				this->prop.reset();

			for (int i=0;i<num;i++) 
			{
				ar.Push("item");
				std::string key=ar.ReadString("key");
				std::string value=ar.ReadString("value");
				(*this->prop)[key]=value;
				ar.Pop("item");
			}
		}
		ar.Pop("prop");

		ar.Push("g");
		g=ar.ReadSmartPointer<Graph>();
		ar.Pop("g");

		ar.Push("batches");
		{
			int num=ar.ReadInt("num");
			this->batches=std::vector<SmartPointer<Batch> >(num);

			for (int i=0;i<num;i++)
			{
				ar.Push("batch");
				batches[i]=ar.ReadSmartPointer<Batch>();
				ar.Pop("batch");
			}
		}
		ar.Pop("batches");

		ar.Push("childs");
		{
			int num=ar.ReadInt("num");
			this->childs=std::vector<SmartPointer<Hpc> >(num);

			for (int i=0;i<num;i++) 
			{
				ar.Push("Hpc");
				childs[i]=ar.ReadSmartPointer<Hpc>();
				ar.Pop("Hpc");
			}
		}
		ar.Pop("childs");
	}

public:
	
	//! typedef for iterating in child hpcs
	typedef std::vector<SmartPointer<Hpc> >::const_iterator const_iterator;
	
	//! spacedim
	int spacedim;

	//! pointdim
	int pointdim;

	//! vertex transformation matrix
	SmartPointer<Matf> vmat;

	//! plane transformation matrix
	SmartPointer<Matf> hmat;

	//! properties in this node
	SmartPointer<PropertySet>   prop;

	//! hpc childs, only for not leaf nodes
	std::vector<SmartPointer<Hpc> >  childs;

	//! geometric graph, only for leaf nodes
	SmartPointer<Graph> g;

	//! compiled gpubatches for fast rendering
	/*!
		NOTA: in gpu_batches di un HPc ci sono tutte le informazioni di un normale Batch tranne quelle
		che non possono essere condivide dal grafo (ad esempio: box,matrix,material,... dipendono dalla gerarchia...)
		Infatti considera sempre che un hpc puo' far parte di piu' complessi simpliciali e quindi sarebbe sbagliato
		memorizzare qui informazioni che riguardano un poliedral complex solo.
		Al contrario quando queste informazioni sono necessarie (ad esempio per l'applicazione delle texture maps)
		deve essere creato un nuovo hpc copia in cui tutti i nodi hpc hanno un solo padre, e quindi sono "esclusivi"
	*/

	std::vector<SmartPointer<Batch> > batches;

public:

	//! default constructor
	inline Hpc()
	{
		++xge_total_hpc;
		this->spacedim=0;
		this->pointdim=0;
	}

	//! copy constructor
	inline Hpc(const Hpc& src)
	{
		++xge_total_hpc;
		this->spacedim=src.spacedim;
		this->pointdim=src.pointdim;
		this->vmat=src.vmat;
		this->hmat=src.hmat;
		this->prop=src.prop;
		this->g=src.g;
		this->batches=src.batches;
	}

	//! constructor from geometric graph and transformation matrices, so builds a leaf hpc
	inline explicit Hpc(SmartPointer<Graph> g,SmartPointer<Matf> vmat=SmartPointer<Matf>(),SmartPointer<Matf> hmat=SmartPointer<Matf>())
	{
		++xge_total_hpc;
		this->spacedim=0;
		this->pointdim=0;
		this->vmat=vmat;
		this->hmat=hmat;
		//this->prop.reset();
		this->g=g;
		//this->mesh.batches.clear();
		fixDimensions();
	}

	//! constructor from transformation matrices, build an intermediate node
	inline explicit Hpc(int spacedim,int pointdim,SmartPointer<Matf> vmat=SmartPointer<Matf>(),SmartPointer<Matf> hmat=SmartPointer<Matf>())
	{	
		++xge_total_hpc;
		this->spacedim=spacedim;
		this->pointdim=pointdim;
		this->vmat=vmat;
		this->hmat=hmat;
		//this->prop.reset();
		//this->g.reset();
		fixDimensions();
	}
	
	//! destructor
	inline ~Hpc()
	{
		--xge_total_hpc;;
		//automatic deallocation of childs
		this->childs.clear();
	}

	//! get number of childs
	inline int getNumberOfChilds() const 
		{return (int)childs.size();}

	//! link a child hpc to a parent hpc, do also dimension fix
	inline void add(SmartPointer<Hpc> child)
	{
		DebugAssert(!this->g); //final node!
		this->spacedim=max2(this->spacedim,child->spacedim);
		this->pointdim=max2(this->pointdim,child->pointdim);
		this->childs.push_back(child);
		fixDimensions();
	}

	//! correct dimensions (internal use only)
	inline void fixDimensions()
	{
		//dimension of ggraph
		if (g)
		{
			this->spacedim=max2(this->spacedim,g->getPointDim());
			this->pointdim=max2(this->pointdim,g->getPointDim());
		}

		//spacedim>=pointdim
		this->spacedim=max2(this->spacedim,this->pointdim);

		//spacedim==vmat.dim
		if (this->vmat) 
			this->spacedim=max2(this->spacedim,this->vmat->dim);

		if (this->vmat && this->vmat->dim!=this->spacedim)
		{
			DebugAssert(this->vmat->dim<this->spacedim);
			*(this->vmat)=this->vmat->extract(this->spacedim);
			*(this->hmat)=this->hmat->extract(this->spacedim);
		}
	}
};





//===========================================================
//! Plasm class
//===========================================================
class XGE_API Plasm
{
protected:

	//no buildable
	Plasm()
		{}

	//! a navigator for Graph (ad example needed by Graph::split())
	static GraphNavigator navigator;

	class Statistic
	{
	public:
		Clock t1;
		int global_msec,global_ncalls,local_msec,local_ncalls;
		inline Statistic() {global_msec=local_msec=global_ncalls=local_ncalls=0;}
	};
	static Statistic stats[2048];

	//! stacks of operations for statistic purpouse
	static std::stack<int> current_ops;

	//! stop benchmark (internal use)
	static inline void Plasm::STOP(int opcode,bool bInnerCall=false)
	{
		DebugAssert(opcode==current_ops.top());
		int msec=(int)(Clock()-stats[opcode].t1);
		stats[opcode].local_msec+=msec;
		stats[opcode].global_msec+=msec;
		if (!bInnerCall)
		{
			current_ops.pop();
			if (current_ops.size()) stats[current_ops.top()].t1=Clock();
		}
	}

	//! start benchmark (internal use)
	static inline void Plasm::START(int opcode)
	{
		//recursive call (do not think it supports recursive calls of the same function)
		if (current_ops.size()) STOP(current_ops.top(),true); //accumulate timing here
		++stats[opcode].local_ncalls ;
		++stats[opcode].global_ncalls;
		current_ops.push(opcode);
		stats[opcode].t1=Clock();
	}

public:


	//! enumeration of ops (IMPORTANT, any change here should be done in Plasm.cpp too!!!)
	enum PlasmOpCode
	{
		PLASM_DESTROY,
		PLASM_CUBE,PLASM_SIMPLEX,PLASM_MKPOL,
		PLASM_STRUCT,
		PLASM_COPY,
		PLASM_GETSPACEDIM,PLASM_GETPOINTDIM,
		PLASM_TRANSFORM,PLASM_SCALE,PLASM_TRANSLATE,PLASM_ROTATE,PLASM_EMBED,
		PLASM_LIMITS,
		PLASM_JOIN,PLASM_POWER,PLASM_BOOLOP,
		PLASM_SKELETON,
		PLASM_UKPOL,PLASM_UKPOLF,
		PLASM_ADDPROPERTY,PLASM_GETPROPERTY,
		PLASM_SKIN,
		PLASM_SHRINK,
		PLASM___END
	};


	//!destructor
	static void dereference(SmartPointer<Hpc>* pc);

	//! cube builder
	/*!	
		@py 
		Plasm.View(Plasm.cube(3,0,1))
		@endpy
	*/
	static SmartPointer<Hpc> cube(int dim,float From=0.0f,float To=1.0f);

	//! simplex builder
	/*! 
		@py 
		Plasm.View(Plasm.simplex(3))
		@endpy
	*/
	static SmartPointer<Hpc> simplex(int dim);

	//! mkpol
	/*! 
		@py 
		Plasm.View(Plasm.mkpol(2,[0,0, 1,0 ,1,1, 0,1],[[0,1,2],[2,0,3]]))
		Plasm.View(Plasm.mkpol(3,[0,0,0,1,0,0,1,1,0,0,1,0, 0,0,1,1,0,1,1,1,1,0,1,1],[[0,1,2,3,4,5,6,7]]))
		@endpy
	*/
	static SmartPointer<Hpc> mkpol(int pointdim,const std::vector<float >& points,const std::vector<std::vector<int> >& hulls,float tolerance=PLASM_DEFAULT_TOLERANCE);

	//! ukpol (by vertices) return pointdim (npoints will be points.size()/returned_value)
	static int ukpol(SmartPointer<Hpc> node,std::vector<float>& points,std::vector<std::vector<int> >& hulls);

	//! ukpolf (by faces)   return pointdim (inplanes will be planes.size()/(returned_value+1)
	static int ukpolf(SmartPointer<Hpc> node,std::vector<float>& planes,std::vector<std::vector<int> >& hulls);

	//!get space dim
	/*! 
		@py 
		assert Plasm.getSpaceDim(Plasm.cube(2))==2
		@endpy
	*/
	static int getSpaceDim(SmartPointer<Hpc> node);

	//!get point dim
	/*! 
		@py 
		assert Plasm.getPointDim(Plasm.cube(2))==2
		@endpy
	*/
	static int getPointDim(SmartPointer<Hpc> node);

	//! limits
	/*! 
		@py 
		assert Plasm.limits(Plasm.cube(3))==Boxf(Vecf(1,0,0,0),Vecf(1,1,1,1))
		@endpy
	*/
	static Boxf limits(SmartPointer<Hpc> node);

	//! print
	/*! 
		@py 
		Plasm.Print(Plasm.cube(2))
		@endpy
	*/
	static void Print(SmartPointer<Hpc> node,int nrec=0);

	//! add property
	/*! 
		@py 
		assert Plasm.getProperty(Plasm.addProperty(Plasm.cube(2),"pname","pvalue"),"pname")=="pvalue"
		@endpy
	*/
	static SmartPointer<Hpc> addProperty(SmartPointer<Hpc> node,std::string pname,std::string pvalue);

	//!get property pname
	/*! 
		@py 
		assert Plasm.getProperty(Plasm.addProperty(Plasm.cube(2),"pname","pvalue"),"pname")=="pvalue"
		@endpy
	*/
	static std::string getProperty (SmartPointer<Hpc> node,std::string pname);

	//! print statistics
	static void statistics(bool bReset);



	//! reduce the hierarchy to 2-level hierarchy (father and final-childs)
	static SmartPointer<Hpc> shrink(SmartPointer<Hpc> node,bool bCloneGeometry);

	//! open from disk
	static SmartPointer<Hpc> open(std::string filename);



	//! save to disk
	static bool save(SmartPointer<Hpc> node,std::string filename);

	//! struct
	/*! 
		@py 
		args=[Plasm.cube(0),Plasm.translate(Plasm.cube(1),3,1,1),Plasm.translate(Plasm.cube(2),3,1,2),Plasm.translate(Plasm.cube(3),3,1,3)]
		Plasm.View(Plasm.Struct(args))
		@endpy
	*/
	static SmartPointer<Hpc> Struct(std::vector<SmartPointer<Hpc> > pols);

	//! copy pol complex
	/*! 
		@py 
		Plasm.View(Plasm.copy(Plasm.cube(3)))
		@endpy
	*/
	static SmartPointer<Hpc> copy(SmartPointer<Hpc> src);
	
	//! join (equivalent to calculate the convex hulls of all points of all pols)
	/*! 
		@py 
		args=[Plasm.cube(0),Plasm.translate(Plasm.cube(1),3,1,1),Plasm.translate(Plasm.cube(2),3,1,2),Plasm.translate(Plasm.cube(3),3,1,3)]
		Plasm.View(Plasm.join(args))
		@endpy
	*/
	static SmartPointer<Hpc> join(std::vector<SmartPointer<Hpc> >  pols,float tolerance=PLASM_DEFAULT_TOLERANCE);

	//! transform a pol complex
	/*! 
		@py 
		vmat=Matf([1,0,0,0, 0,1,0,1, 0,0,1,1, 0,0,0,1])
		Plasm.View(Plasm.transform(Plasm.cube(3),vmat,vmat.invert()))
		@endpy
	*/
	static SmartPointer<Hpc> transform(SmartPointer<Hpc> child,SmartPointer<Matf> vmat,SmartPointer<Matf> hmat);

	//! scale a pol complex
	/*! 
		@py 
		Plasm.View(Plasm.scale(Plasm.cube(3),Vecf(0.0, 1.0,2.0,3.0)))
		@endpy
	*/
	static SmartPointer<Hpc> scale(SmartPointer<Hpc> child,Vecf vs);

	//! scale 
	static inline SmartPointer<Hpc> scale(SmartPointer<Hpc> child,int spacedim,int ref,float factor)
	{
		DebugAssert(ref>=1 && ref<=spacedim);
		Vecf v(spacedim);
		v.set(0,0.0);
		for (int i=1;i<=spacedim;i++) 
			v.set(i,1.0f);
		v.set(ref,factor);
		return scale(child,v);
	}

	//! translate
	/*! 
		@py 
		Plasm.View(Plasm.translate(Plasm.cube(3),Vecf(0.0, 1.0,2.0,3.0)))
		@endpy
	*/
	static SmartPointer<Hpc> translate(SmartPointer<Hpc> child,Vecf vt);

	//! translate
	static inline SmartPointer<Hpc> translate(SmartPointer<Hpc> child,int spacedim,int ref,float factor)
	{
		DebugAssert(ref>=1 && ref<=spacedim);
		Vecf v(spacedim);
		v.set(0,0.0);
		v.set(ref,factor);
		return translate(child,v);
	}

	//! rotate
	/*! 
		@py
		Plasm.View(Plasm.Struct([Plasm.cube(3),Plasm.rotate(Plasm.cube(3),3,1,2,pi)]))
		@endpy
	*/
	static SmartPointer<Hpc> rotate(SmartPointer<Hpc> child,int spacedim,int i,int j,float angle);

	//! embed
	/*! 
		@py 
		Plasm.View(Plasm.embed(Plasm.cube(2),3))
		@endpy
	*/
	static SmartPointer<Hpc> embed(SmartPointer<Hpc> child,int spacedim);

	//!power
	/*! 
		@py 
		Plasm.View(Plasm.power(Plasm.cube(1),Plasm.cube(2)))
		@endpy
	*/
	static SmartPointer<Hpc> power(SmartPointer<Hpc> arg1 ,SmartPointer<Hpc> arg2);

	//!skeleton
	/*! 
		@py 
		Plasm.View(Plasm.skeleton(Plasm.cube(3),1))
		@endpy
	*/
	static SmartPointer<Hpc> skeleton(SmartPointer<Hpc> node,int level,float tolerance=PLASM_DEFAULT_TOLERANCE);

	
	//! boolean operation codes
	enum BoolOpCode 
	{
		BOOL_CODE_OR  ,
		BOOL_CODE_AND,
		BOOL_CODE_DIFF,
		BOOL_CODE_XOR
	};

	//! bool op
	/*! 
		@py 
		Plasm.View(Plasm.boolop(Plasm.BOOL_CODE_AND,[Plasm.cube(2,-0.5,0.5),Plasm.rotate(Plasm.cube(2,-0.5,0.5),2,1,2,pi/4)]))
		@endpy
	*/

	static SmartPointer<Hpc> boolop(BoolOpCode operation,std::vector<SmartPointer<Hpc> > pols,float tolerance=PLASM_DEFAULT_TOLERANCE,int maxnumtry=PLASM_MAX_NUM_SPLIT);

	//! apply texture map
	static SmartPointer<Hpc> Skin(SmartPointer<Hpc> src,std::string url,SmartPointer<Matf> project_uv);

	//! return all triangles transformed in World applying 3d matrices, if possible (internal use)
	static SmartPointer<Vector> getTriangles(SmartPointer<Hpc> src);

	//! insert batches in a mesh
	static std::vector<SmartPointer<Batch> > getBatches(SmartPointer<Hpc> src,bool bOptimize=true);

	//! view an hpc
	static void View(SmartPointer<Hpc> src,bool bBackground=true);

	//! self test
	static int SelfTest();

	//! self test of boolean operations
	static int boolop_selftest();

	//! internal use only
	static void convertOldXml(char* input_filename,char* output_filename,char* prefix_texture_filename);

}; //end class


#endif //_TRS_PLASM_H__