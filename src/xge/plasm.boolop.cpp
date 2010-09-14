#include <xge/xge.h>
#include <xge/plasm.h>
#include <xge/clock.h>

#include <set>

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class PlasmBoolOperation
{
public:

	//constructor
	PlasmBoolOperation
	(
		Plasm::BoolOpCode     code, //see BOOL_CODE_XXXX
		int                   pointdim, //space for the output
		bool                  USE_OCTREE, //use octree
		bool                  BOUNDARY_REP, //consider arguments in full dimension or not
		int                   G_SPLIT_MAXNUMTRY, //for splitting cells
		float                 G_SPLIT_TOLERANCE, //split tolerance
		float                 G_CONTAINS_TOLERANCE //for test of containment
	);

	//destructor
	~PlasmBoolOperation();

	//add a argument (hpc will not be modified, I will construct a copy for the boolean op)
	void add(SmartPointer<Hpc> hpc);

	//run the algorithm
	void run();

	//get the solution 
	inline const SmartPointer<Graph> getGraph()
		{return this->gout;}

protected:

	//_________________________________________
	//internal benchmark
	enum
	{
		TIME_FETCH,
		TIME_RESCALE,
		TIME_COLLECT,
		TIME_OCTREE,
		TIME_BOUNDARY,
		TIME_CLASSIFY,
		TIME_END
	};

	struct
	{
		Clock  start;
		float diff;
	}
	timings[TIME_END];

	inline void START(int T) 
	{
		timings[T].start =(Clock());
	}

	inline void END(int T)
	{
		timings[T].diff +=((Clock()-timings[T].start));
	}

	/////////////////////////////////////////////////
	//definition of a face (faster than STL vector)
	/////////////////////////////////////////////////

	int tot_alloc_faces;

	typedef struct _face_t_
	{
		Graph   *g;//Graph
		unsigned int              F;//id of the face
		Boxf              *box;//bounding box of the face (valid only for pointdim>=2)
		struct _face_t_     *next;
	}
	face_t;

	void    face_set_push    (face_t**& prev,Graph* g,unsigned int F,Boxf* box);
	face_t  face_set_pop     (face_t*& head);
	face_t* face_set_copy    (face_t* src);


protected:

	//input arguments
	int pointdim;

	//caching of bboxes (to speed up classify)
	std::vector<Boxf*>* boxes_hpc;
	std::vector<Boxf*>* boxes_ggraph;

	//input arguments
	std::vector<SmartPointer<Hpc> > args;

	//code, see BOOL_CODE_XXXX
	Plasm::BoolOpCode code;

	//final solution
	SmartPointer<Graph> gout;

	//for findCells
	GraphNavigator nav;

	//configuration
	struct
	{
		bool    bBoundary;            //the cells are full or not
		bool    bUseOctree;           //use octree or not (better to use)
		int     g_split_maxnumtry;    //how many try to split a cell (see Graph::split)
		float   g_split_tolerance;    //which is the tolerance for split (see Graph::split)
		float   g_contains_tolerance; //see Graph::contains
	}
	config;

	//classify a point 
	enum {CELL_OUT=0,CELL_IN =1};
	unsigned short classify(const Vecf& point);

	//find solution using boundary faces or octree
	void doBoundary(unsigned int C,const Boxf& aabb,face_t* faces);
	void doOctree  (unsigned int C,const Boxf& aabb,face_t* faces);

	//test if g contains a point
	inline bool contains(Graph* g,const Vecf& point);
};



/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
SmartPointer<Hpc> Plasm::boolop(Plasm::BoolOpCode operation,std::vector<SmartPointer<Hpc> > pols,float tolerance,int maxnumtry)
{
	START(PLASM_BOOLOP);

	//get max spacedimension
	int spacedim=0;
	for (int i=0;i<(int)pols.size();i++) 
		spacedim=max2(spacedim,getSpaceDim(pols[i]));
	
	PlasmBoolOperation bop((BoolOpCode)operation,spacedim,true,false,maxnumtry,tolerance,tolerance);

	for (int i=0;i<(int)pols.size();i++) 
	{
		if (getSpaceDim(pols[i])==getPointDim(pols[i]) && spacedim==getPointDim(pols[i])) 
			bop.add(pols[i]);
	}
	
	bop.run();
	SmartPointer<Graph> g(bop.getGraph()->clone());
	SmartPointer<Hpc> hpc(new Hpc(g));
	STOP(PLASM_BOOLOP);

	return hpc;
}


//////////////////////////////////////////////////////////////////////////////
//INLINES for face set
//////////////////////////////////////////////////////////////////////////////
inline void PlasmBoolOperation::face_set_push(face_t**& prev,Graph* g,unsigned int F,Boxf* box)
{
	face_t* ret=(face_t*)MemPool::getSingleton()->malloc(sizeof(face_t));
	ret->g=g;
	ret->F=F;
	ret->box=box;
	ret->next=0;
	(*prev)=ret;
	prev=&(ret->next);
	++tot_alloc_faces;
}

inline PlasmBoolOperation::face_t PlasmBoolOperation::face_set_pop(face_t*& head)
{
	DebugAssert(head);
	face_t ret=(*head);
	ret.next=0;
	face_t* tmp=head;
	head=head->next;
	MemPool::getSingleton()->free(sizeof(face_t),tmp);
	--tot_alloc_faces;
	return ret;
}

inline PlasmBoolOperation::face_t* PlasmBoolOperation::face_set_copy(face_t* src)
{
	face_t*  ret=0;
	face_t** prev=&(ret);
	for (face_t* cursor=src;cursor;cursor=cursor->next)
		face_set_push(prev,cursor->g,cursor->F,cursor->box);
	return ret;
}




//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
PlasmBoolOperation::PlasmBoolOperation(Plasm::BoolOpCode code,int pointdim,bool USE_OCTREE,bool BOUNDARY_REP,int G_SPLIT_MAXNUMTRY,float G_SPLIT_TOLERANCE,float G_CONTAINS_TOLERANCE)
{
	for (int I=0;I<TIME_END;I++) timings[I].diff=0;

	boxes_hpc   =0;
	boxes_ggraph=0;
	
	START(TIME_FETCH);

	DebugAssert(code==Plasm::BOOL_CODE_OR || code==Plasm::BOOL_CODE_AND || code==Plasm::BOOL_CODE_DIFF || code==Plasm::BOOL_CODE_XOR);
	this->code=code;
	//this->gout.reset(0);
	this->pointdim=pointdim;
	this->tot_alloc_faces=0;

	// *** configuration ****
	this->config.bBoundary            = BOUNDARY_REP;
	this->config.bUseOctree           = USE_OCTREE && (pointdim>=2);
	this->config.g_split_maxnumtry    = G_SPLIT_MAXNUMTRY;
	this->config.g_split_tolerance    = G_SPLIT_TOLERANCE;
	this->config.g_contains_tolerance = G_CONTAINS_TOLERANCE;

	DebugAssert(!this->config.bBoundary || (pointdim==2 || pointdim==3)); //todo other cases
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
PlasmBoolOperation::~PlasmBoolOperation()
{
	//print statistics
	float tot=0;
	for (int I=0;I<TIME_END;I++)
		tot+=timings[I].diff;

	if (tot_alloc_faces)
	{
		Log::printf("WARNING!!!!!!!!!!!!!!!!!!!!!\n");
		Log::printf("Memory bug in PlasmBoolOperation.tot_alloc_faces>0");
		DebugAssert(!tot_alloc_faces);
	}

	#ifdef _DEBUG
	Log::printf("Boolean op statistics\n");
	Log::printf("TIME_FETCH      %.2f (%.0f)\n",timings[TIME_FETCH   ].diff,100.0f*timings[TIME_FETCH      ].diff/tot);
	Log::printf("TIME_RESCALE    %.2f (%.0f)\n",timings[TIME_RESCALE ].diff,100.0f*timings[TIME_RESCALE    ].diff/tot);
	Log::printf("TIME_COLLECT    %.2f (%.0f)\n",timings[TIME_COLLECT ].diff,100.0f*timings[TIME_COLLECT    ].diff/tot);
	Log::printf("TIME_OCTREE     %.2f (%.0f)\n",timings[TIME_OCTREE  ].diff,100.0f*timings[TIME_OCTREE     ].diff/tot);
	Log::printf("TIME_BOUNDARY   %.2f (%.0f)\n",timings[TIME_BOUNDARY].diff,100.0f*timings[TIME_BOUNDARY   ].diff/tot);
	Log::printf("TIME_CLASSIFY   %.2f (%.0f)\n",timings[TIME_CLASSIFY].diff,100.0f*timings[TIME_CLASSIFY   ].diff/tot);
	Log::printf("TIME TOTAL      %.2f       \n",tot);
	Log::printf("\n");
	#endif

	//remove cached bounding box
	for (std::vector<Boxf*>::iterator it=boxes_hpc->begin();it!=boxes_hpc->end();it++) delete *it;
	delete boxes_hpc;
	
	for (std::vector<Boxf*>::iterator it=boxes_ggraph->begin();it!=boxes_ggraph->end();it++) delete *it;
	delete boxes_ggraph;
	
	//remove solution
	//if (gout)  delete gout;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void PlasmBoolOperation::add(SmartPointer<Hpc> hpc)
{		
	//should be full dimensional
	DebugAssert(hpc->spacedim==hpc->pointdim);

	hpc=Plasm::shrink(hpc,true); //clone geometry, since all gs should be indepentent to apply matrices
	DebugAssert(!hpc->vmat && !hpc->hmat);

	//move matrices inside g if full dimensional
	for (Hpc::const_iterator it=hpc->childs.begin();it!=hpc->childs.end();it++)
	{
		SmartPointer<Hpc> child=*it;DebugAssert(child->pointdim==child->spacedim && child->g && child->g->getPointDim()==child->pointdim && child->vmat && child->hmat);
		SmartPointer<Graph> g=child->g;

		//apply matrices to g, and remove from hpc
		DebugAssert(child->vmat->dim==child->spacedim && child->hmat->dim==child->spacedim);
		g->transform(child->vmat,child->hmat);
		child->vmat.reset();
		child->hmat.reset();
	}

	args.push_back(hpc);
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void PlasmBoolOperation::run()
{
	END(TIME_FETCH);

	//should be the only call
	DebugAssert(!gout);

	// calculate overall bounding box
	START(TIME_RESCALE);

	//find bounding box
	Boxf overall_box(pointdim);

	for (int H=0;H<(int)args.size();H++)
	{
		Boxf hpc_box(pointdim);

		for (Hpc::const_iterator it=args[H]->childs.begin();it!=args[H]->childs.end();it++)
		{
			SmartPointer<Hpc> child=*it;DebugAssert(!child->getNumberOfChilds()  && !child->vmat && !child->hmat && child->g);
			SmartPointer<Graph> g=child->g;

			//add the bouding box
			Boxf box=g->getBoundingBox();
			if (box.isValid()) hpc_box.add(box); 
		}

		if (hpc_box.isValid())
			overall_box.add(hpc_box);
	}

	//failed to have a good bounding box
	if (!overall_box.isValid())
	{
		END(TIME_RESCALE);
		this->gout.reset(new Graph(pointdim));
		return;
	}

	// rescale to the normalized [-1,+1] to get more precision
	Vecf T=-0.5f*(overall_box.p1+overall_box.p2);T.mem[0]=0.0f;
	Vecf S=Vecf(pointdim); S.mem[0]=0.0f;
	for (int I=1;I<=pointdim;I++) S.mem[I]=2.0f/overall_box.size()[I];

	SmartPointer<Matf> vmat(new Matf(Matf::scaleV(S) * Matf::translateV(T)));
	SmartPointer<Matf> hmat(new Matf(Matf::translateH(T) * Matf::scaleH(S)));
	DebugAssert(((*vmat) * (*hmat)).almostIdentity(0.01f));

	for (int H=0;H<(int)args.size();H++)
	{
		for (Hpc::const_iterator it=args[H]->childs.begin();it!=args[H]->childs.end();it++)
		{
			SmartPointer<Hpc> child=*it;DebugAssert(!child->getNumberOfChilds()  && !child->vmat && !child->hmat && child->g);
			SmartPointer<Graph> g=child->g;
			g->transform(vmat,hmat);
		}
	}

	//new bounding box where to work
	overall_box=Boxf(pointdim,-1.01f,+1.01f); 

	//cache bounding box
	DebugAssert(!boxes_hpc && !boxes_ggraph);

	boxes_hpc   =new std::vector<Boxf*>;
	boxes_ggraph=new std::vector<Boxf*>;

	for (int H=0;H<(int)args.size();H++)
	{
		boxes_hpc->push_back(new Boxf(Plasm::limits(args[H])));

		for (Hpc::const_iterator it=args[H]->childs.begin();it!=args[H]->childs.end();it++)
		{
			SmartPointer<Hpc> child=*it;DebugAssert(!child->getNumberOfChilds() && !child->vmat && !child->hmat && child->g);
			SmartPointer<Graph> g=child->g;
			DebugAssert(!child->vmat && !child->hmat);
			boxes_ggraph->push_back(new Boxf(g->getBoundingBox()));
		}
	}

	END(TIME_RESCALE);

	//solve the system
	Clock t1;

	//************************ special case for pointdim=0
	if (pointdim==0)
	{
		Vecf point(0);
		point.mem[0]=1.0f;
		if (classify(point)==CELL_IN)
			this->gout=Graph::cuboid(pointdim,Boxf(0));
		else
			this->gout.reset(new Graph(0));
	}
	//************************  special case for pointdim=1
	else if (pointdim==1)
	{
		//here I have points, sort all points
		std::set<float> values;
		for (int H=0;H<(int)args.size();H++)
		{
			for (Hpc::const_iterator it=args[H]->childs.begin();it!=args[H]->childs.end();it++)
			{
				SmartPointer<Hpc> child=*it;DebugAssert(!child->getNumberOfChilds() && !child->vmat && !child->hmat && child->g);
				SmartPointer<Graph> g=child->g;
				for (GraphListIterator jt=g->each(0);!jt.end();jt++) 
					values.insert(g->getGeometry(*jt)[1]);
			}
		}
		this->gout.reset(new Graph(1));
		unsigned int V0=0;
		for (std::set<float>::iterator it=values.begin();it!=values.end();it++)
		{
			float Where=*it;

			Vecf point(1);
			point.mem[0]=1.0f;
			point.mem[1]=Where;

			unsigned int V1=gout->addVertex(point);

			if (V0)
			{
				float a=gout->getGeometry(V0)[1];
				float b=Where;
				DebugAssert(a<b);

				Vecf point(1);
				point.mem[0]=1.0f;
				point.mem[1]=0.5f*(a+b);
				

				if (classify(point)==CELL_IN)
				{
					unsigned int E=gout->addEdge(V0,V1);
					gout->addArch(E,V0); //double connectivity!
					gout->addArch(E,V1);
				}
			}
			V0=V1;
		}
		//clean up points with no connection
		for (GraphListIterator it=gout->each(0),itnext;!it.end();it=itnext)
		{
			itnext=it.next();

			if (!gout->getNUp(*it))
				gout->remNode(*it);
		}
	}
	//************************************  pointdim>=2
	else
	{
		// collect faces
		START(TIME_COLLECT);

		face_t*  overall_faces=0;
		face_t** overall_faces_prev=&overall_faces;

		for (int H=0;H<(int)args.size();H++)
		{
			for (Hpc::const_iterator it=args[H]->childs.begin();it!=args[H]->childs.end();it++)
			{
				SmartPointer<Hpc> child=*it;DebugAssert(!child->getNumberOfChilds()  && !child->vmat && !child->hmat && child->g);
				SmartPointer<Graph> g=child->g;
				DebugAssert(!child->vmat && !child->hmat);

				//add doBoundary faces
				for (GraphListIterator jt=g->each(g->getPointDim()-1);!jt.end();jt++)
				{
					unsigned int F=*jt;

					//filter internal faces which are useless!
					if (g->getNUp(F)==2) continue;

					//add the face
					face_set_push(overall_faces_prev,g.get(),F,new Boxf(g->getBoundingBox(F)));
				}
			}
		}
		DebugAssert(*overall_faces_prev==0);
		END(TIME_COLLECT);

		this->gout=Graph::cuboid(pointdim,overall_box);
		DebugAssert(this->gout->getNCells(pointdim)==1);
		unsigned int C=*(this->gout->each(pointdim));

		if (config.bUseOctree)
			doOctree  (C,overall_box,face_set_copy(overall_faces));
		else
			doBoundary(C,overall_box,face_set_copy(overall_faces));

		//deallocate boxes of overall faces
		while (overall_faces)
		{
			face_t face=face_set_pop(overall_faces);
			delete face.box;
		}
	}

	//need to apply the inverse transformation for the rescaling
	//is it right this order of hmat, vmat since I need the inversion here!
	gout->transform(hmat,vmat); 

	Log::printf("...done in %d msec\n",(int)(Clock()-t1));
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
inline bool PlasmBoolOperation::contains(Graph* g,const Vecf& point)
{
	//boundary case
	if (config.bBoundary)
	{
		int nhit=0;

		//for point dim==2
		if (g->getPointDim()==2)
		{
			Ray3f ray(Vec3f(point[1],point[2],0),Vec3f(Utils::FloatRand(-0.5f,+0.5f),Utils::FloatRand(-0.5f,+0.5f),0));

			//find intersection with lines
			for (GraphListIterator it=g->each(1);!it.end();it++)
			{
				unsigned int E=*it;
				DebugAssert(g->getNDw(E)==2);
				float* v0=g->getGeometry(g->getFirstDwNode(E))+1;
				float* v1=g->getGeometry(g->getLastDwNode (E))+1;
				if (ray.intersectLine(v0,v1)>=0) ++nhit;
			}
			
		}
		//for point dim==3
		else if (g->getPointDim()==3)
		{
			Ray3f ray(Vec3f(point[1],point[2],point[3]),Vec3f(Utils::FloatRand(-0.5f,+0.5f),Utils::FloatRand(-0.5f,+0.5f),Utils::FloatRand(-0.5f,+0.5f)));

			GraphListIterator it;
			GraphIterator jt;

			//find all faces
			for (it=g->each(2);!it.end();it++)
			{
				unsigned int F=*it;
				DebugAssert(g->getNDw(F)==3); //TODO all other cases (when it's not triangulated)

				unsigned int prev0=g->getFirstDwNode(g->getLastDwNode(F)),p1;
				unsigned int prev1=g->getLastDwNode (g->getLastDwNode(F)),p2;

				float *v0=0,*v1=0,*v2=0;

				for (GraphIterator jt=g->goDw(F);!jt.end();jt++,prev0=p1,prev1=p2)
				{
					p1=g->getFirstDwNode(*jt);
					p2=g->getLastDwNode (*jt);
					if (p1!=prev0 && p1!=prev1) Swap(unsigned int,p1,p2);
					float* P=g->getGeometry(p1);
					if      (!v0) v0=P+1;
					else if (!v1) v1=P+1;
					else          v2=P+1;
				}
				if (ray.intersectTriangle(Vec3f(v0),Vec3f(v1),Vec3f(v2))>=0) ++nhit;
			}
		}
		//pointdim>3 or pointdim=0
		else
		{
			Utils::Error(HERE,"TODO, not implemented!");
		}

		return (nhit &1 ); //dispari!
	}
	else
	{
		return g->contains(0,point,config.g_contains_tolerance);
	}
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
unsigned short PlasmBoolOperation::classify(const Vecf& point)
{		
	START(TIME_CLASSIFY);

	DebugAssert(point.dim==pointdim);
	int nfull=0,nempty=0;

	int NG=0;
	for (int H=0;H<(int)args.size();H++)
	{
		bool bFull=false;
		
		if ((*boxes_hpc)[H]->contains(point))
		{
			int IG=0;
			for (Hpc::const_iterator it=args[H]->childs.begin();!bFull && it!=args[H]->childs.end();it++,IG++)
			{
				SmartPointer<Hpc> child=*it;DebugAssert(!child->getNumberOfChilds()  && !child->vmat && !child->hmat && child->g);
				SmartPointer<Graph> g=child->g;
				bFull=(*boxes_ggraph)[NG+IG]->contains(point) && contains(g.get(),point);
			}
		}
		NG+=(int)args[H]->getNumberOfChilds();

		if (bFull) ++nfull; else ++nempty;

		switch (code)
		{
			case Plasm::BOOL_CODE_OR://OR, any hpc can be full
				if (nfull) 
				{
					END(TIME_CLASSIFY);
					return CELL_IN;
				}
				break;

			case Plasm::BOOL_CODE_AND://AND, all hpc should be full
				if (nempty) 
				{
					END(TIME_CLASSIFY);
					return CELL_OUT;
				}
				break;

			case Plasm::BOOL_CODE_DIFF://DIFF, the first should be full, the other should not be full
				if ((!H && !bFull) || (H && bFull)) 
				{
					END(TIME_CLASSIFY);
					return CELL_OUT;
				}
				break;

			case Plasm::BOOL_CODE_XOR://XOR, bisogna contare i dispari
				break;
		}
	}
	DebugAssert((nempty+nfull)==args.size());

	END(TIME_CLASSIFY);

	switch (code)
	{
		case Plasm::BOOL_CODE_OR://no one is full
			DebugAssert(nempty==args.size());
			return CELL_OUT;

		case Plasm::BOOL_CODE_AND://all full
			DebugAssert(nfull==args.size());
			return CELL_IN;

		case Plasm::BOOL_CODE_DIFF://the first is full and all other are empty
			DebugAssert(nfull==1);
			return CELL_IN;

		case Plasm::BOOL_CODE_XOR://zero full or one full
			return (nfull & 1) ?CELL_IN:CELL_OUT;
	}

	//should not reach here!
	DebugAssert(0);
	return CELL_OUT;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void PlasmBoolOperation::doBoundary(unsigned int C,const Boxf& aabb,face_t* faces)
{
	DebugAssert(pointdim>=2);

	//cannot do anything, the cell does not exist
	if (!C)
	{
		while (faces) face_set_pop(faces);
		return;
	}

	START(TIME_BOUNDARY);
	
	//get the first good face (which potentially intersect)
	Planef h(pointdim);
	while (true)
	{
		//no more doBoundary faces
		if (!faces)
		{
			Vecf centroid=gout->getBoundingBall(C).center;

			END(TIME_BOUNDARY);

			if (classify(centroid)==CELL_OUT)
				gout->remNode(C);

			return;
		}

		//get the first face and remove from the list
		face_t face=face_set_pop(faces);

		//(STEP 1) if the two bounding box does not overlap....
		if (!aabb.overlap(*(face.box)))  
			continue;

		//(STEP 2) if face F leaves the bounding box of cell C in the above/below space
		h=Planef(pointdim,face.g->getGeometry(face.F));

		//NOTE: prefer not to test if the face really intersect (using CPQ code) 
		//because it does not seem to make a real difference
		if (aabb.isAbove(h) || aabb.isBelow(h))
			continue;

		//potentially intersect (does not mean it intersect really)
		break;
	}

	//get the plane of the face and split the cell
	unsigned int Ca,Cb,Ce;
	int retcode=gout->split(nav,C,h,config.g_split_tolerance,config.g_split_maxnumtry,Cb,Ca,Ce,0);

	//cannot do anymore, the cell is too flat, STOP HERE with faces
	if (retcode==Graph::SPLIT_FLAT)
	{
		while (faces) face_set_pop(faces);

		END(TIME_BOUNDARY);

		if (classify(gout->getBoundingBall(C).center)==CELL_OUT)
			gout->remNode(C);

		return;
	}

	//cannot classify using this face, try with the next one
	if (retcode==Graph::SPLIT_IMPOSSIBLE)
	{
		END(TIME_BOUNDARY);
		return doBoundary(C,aabb,faces);
	}

	//the splitting is ok
	DebugAssert(retcode==Graph::SPLIT_OK);

	END(TIME_BOUNDARY);

	if (Cb && Ca)
	{
		face_t* faces_below=faces; //faces below is just faces itself (it's useless to copy and destroy)
		face_t* faces_above=face_set_copy(faces);
		doBoundary(Cb,gout->getBoundingBox(Cb),faces_below);
		doBoundary(Ca,gout->getBoundingBox(Ca),faces_above);	
	}
	else if (Cb)
		doBoundary(Cb,gout->getBoundingBox(Cb),faces);

	else if (Ca)
		doBoundary(Ca,gout->getBoundingBox(Ca),faces);

	else
		while (faces) face_set_pop(faces);

}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void PlasmBoolOperation::doOctree(unsigned int C,const Boxf& box,face_t* faces)
{
	DebugAssert(pointdim>=2);

	if (!faces)
		return doBoundary(C,box,faces);

	START(TIME_OCTREE);

	//cut on the max dimension
	int Ref=box.maxsizeidx();
	DebugAssert(Ref>=1 && Ref<=pointdim);

	float from   = box.p1[Ref];
	float to     = box.p2[Ref];
	float middle = (from+to)/2.0f;

	//box below and above
	Boxf box_minus=box;box_minus.p2.mem[Ref]=middle;
	Boxf box_plus =box;box_plus .p1.mem[Ref]=middle;

	//the splitting hyperplane
	Planef h(pointdim);
	memset(h.mem,0,sizeof(float)*(pointdim+1));
	h.mem[0]=-middle;h.mem[Ref]=1;//x_ref>=middle

	//assign faces to below and above
	face_t* faces_below=0;face_t** prev_face_below=&faces_below;int nface_below=0;
	face_t* faces_above=0;face_t** prev_face_above=&faces_above;int nface_above=0;

	int nfaces=0;
	for (face_t* face=faces;face;face=face->next,nfaces++)
	{
		Planef h(pointdim,face->g->getGeometry(face->F));

		//limit the bbox of faces to the current bbox
		Boxf b=*(face->box);
		b.p1=b.p1.Max(box.p1);
		b.p2=b.p2.Min(box.p2);
		DebugAssert(b.isValid());

		//since I'm using doOctree I can use this fast (potential) intersection
		if (box_minus.overlap(b) && !(box_minus.isBelow(h) || box_minus.isAbove(h)))
		{
			face_set_push(prev_face_below,face->g,face->F,face->box);
			++nface_below;
		}

		if (box_plus.overlap(b) && !(box_plus.isBelow(h) || box_plus.isAbove(h)))
		{
			face_set_push(prev_face_above,face->g,face->F,face->box);
			++nface_above;
		}
	}

	//to avoid deadlock shoul be strict decreasing
	bool bSplitMore=(nface_below<nfaces && nface_above<nfaces) || (!nface_below) || (!nface_above);

	unsigned int Ca,Cb,Ce;

	if (!bSplitMore || (!(gout->split(nav,C,h,config.g_split_tolerance,1,Cb,Ca,Ce,0)==Graph::SPLIT_OK && Cb && Ca)))
	{
		//dealloc just created list
		while (faces_below) face_set_pop(faces_below);
		while (faces_above) face_set_pop(faces_above);

		END(TIME_OCTREE);
		return doBoundary(C,box,faces);
	}

	//dealloc list of faces
	while (faces) face_set_pop(faces);

	END(TIME_OCTREE);

	doOctree(Cb,box_minus,faces_below);
	doOctree(Ca,box_plus ,faces_above );
}




///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
#if 0
Graph* open_svg(const char* filename,bool bFull);
Graph* open_ply(const char* filename);
void main()
{
	if (true)
	{
		Graph* g=open_svg("examples/svg/edificicleaned.svg",true);
		SmartPointer<Hpc> hpc=(SmartPointer<Hpc>)plasm->make(g);
		//Matf vmat(2),hmat(2);
		//g->toUnitBox(vmat,hmat);

		const bool bUseOctree=true;
		PlasmBoolOperation bop(BOOL_CODE_OR,2,bUseOctree);
		bop.add(hpc);
		bop.run();

		plasm->view((SmartPointer<Hpc>)plasm->make(bop.getGraph()->clone()));
	}

	//boundary 3dim
	if (true)
	{
		SmartPointer<Hpc> c1=(SmartPointer<Hpc>)plasm->make(open_ply("C:/home/demos/data/fields/dinosaur.ply"));

		PlasmBoolOperation OR(BOOL_CODE_OR,3,true,true);
		OR.add(c1);
		OR.run();
		Graph::Node* pol=plasm->make(OR.getGraph()->clone());
		plasm->view(pol,"PLY",true);
	}
	return;
}

#endif



//////////////////////////////////////////////////////////////////////
int Plasm::boolop_selftest()
{
	Log::printf("Testing PlasmBoolOperation...\n");
	{
		SmartPointer<Hpc> c1=Plasm::cube(0);
		SmartPointer<Hpc> c2=Plasm::cube(0);

		std::vector<SmartPointer<Hpc> > pols;
		pols.push_back(c1);
		pols.push_back(c2);

		std::vector<SmartPointer<Hpc> >args;

 		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_OR  ,pols),2,2,0.0f));
		args.push_back(Plasm::translate(Plasm::cube(1,-0.5,+0.5),2,2,0.5));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_AND ,pols),2,2,1.0f));
		args.push_back(Plasm::translate(Plasm::cube(1,-0.5,+0.5),2,2,1.5));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_DIFF,pols),2,2,2.0f));
		args.push_back(Plasm::translate(Plasm::cube(1,-0.5,+0.5),2,2,2.5));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_XOR ,pols),2,2,3.0f));
		args.push_back(Plasm::translate(Plasm::cube(1,-0.5,+0.5),2,2,3.5));

		SmartPointer<Hpc> Out=Plasm::Struct(args);
		Plasm::View(Out,false);
	}

	ReleaseAssert(!xge_total_hpc);


	//test in 1-dim

	{
		SmartPointer<Hpc> c1=Plasm::cube(1,0,3);
		SmartPointer<Hpc> c2=Plasm::cube(1,1,4);
		SmartPointer<Hpc> c3=Plasm::cube(1,2,5);

		std::vector<SmartPointer<Hpc> > pols;
		pols.push_back(c1);
		pols.push_back(c2);
		pols.push_back(c3);

		std::vector<SmartPointer<Hpc> >args;

		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_OR   ,pols),2,2,0.0f));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_AND  ,pols),2,2,1.0f));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_DIFF ,pols),2,2,2.0f));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_XOR  ,pols),2,2,3.0f));

		SmartPointer<Hpc> Out=Plasm::Struct(args);
		Plasm::View(Out,false);
	}

	ReleaseAssert(!xge_total_hpc);

	
	{
		SmartPointer<Hpc> base_cube=Plasm::translate(Plasm::translate(Plasm::translate(Plasm::cube(3),3,1,-0.5),3,2,-0.5),3,3,-0.5);

		std::vector<SmartPointer<Hpc> > pols;

		const int npols=4;
		for (int i=0;i<npols;i++)
			pols.push_back(Plasm::rotate(base_cube,3,1,2,i*(float)M_PI/(npols*2)));

		std::vector<SmartPointer<Hpc> > args;
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_OR  ,pols),3,1,0.0f));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_AND ,pols),3,1,1.5f));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_DIFF,pols),3,1,3.0f));
		args.push_back(Plasm::translate(Plasm::boolop(BOOL_CODE_XOR ,pols),3,1,4.5f));

		SmartPointer<Hpc> Out=Plasm::Struct(args);
		Plasm::View(Out,false);	
	}
	
	ReleaseAssert(!xge_total_hpc);

	//test edifici (only in release mode! about 15 seconds)
#if 0
	{
		Graph* g=Graph::open_svg("examples/svg/edificicleaned.svg",true);

		//Log::printf("%d %d %d\n",g->getNCells(0),g->getNCells(1),g->getNCells(2));
		//Matf vmat(2),hmat(2);
		//g->toUnitBox(vmat,hmat);
		//g->viewer(1,&g,2);

		SmartPointer<Hpc> pols[]={Plasm::make(g)};
		SmartPointer<Hpc> Out=(SmartPointer<Hpc>)Plasm::boolop(BOOL_CODE_OR,1,pols);
		Plasm::viewer(Out,"Boolean operation");	
	}

	ReleaseAssert(!xge_total_hpc);
#endif

	return 0;
}


