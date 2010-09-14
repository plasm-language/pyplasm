
///////////////////////////////////////////////////////// inline
inline Graph::Graph() :nodes(sizeof(GraphNode)),arches(sizeof(GraphArch)),pointdim(0),db(sizeof(float)*(0+1))
{
	memset(nodelists_v,0,sizeof(nodelists_v));
	memset(nodelists_n,0,sizeof(nodelists_n));
}

///////////////////////////////////////////////////////// inline
inline Graph::Graph(int _pointdim)
	:nodes(sizeof(GraphNode)),arches(sizeof(GraphArch)),pointdim(_pointdim),db(sizeof(float)*(_pointdim+1))
{
	memset(nodelists_v,0,sizeof(nodelists_v));
	memset(nodelists_n,0,sizeof(nodelists_n));
}

///////////////////////////////////////////////////////// inline
inline Graph::Graph(const Graph& src)	
	:arches(src.arches),nodes(src.nodes),pointdim(src.pointdim),db(src.db)
{
	memcpy(this->nodelists_v,src.nodelists_v,sizeof(this->nodelists_v));
	memcpy(this->nodelists_n,src.nodelists_n,sizeof(this->nodelists_n));
}

///////////////////////////////////////////////////////// inline
inline Graph::~Graph()
{
	;
}

///////////////////////////////////////////////////////// inline
inline int Graph::getPointDim() const
{
	return this->pointdim;
}

///////////////////////////////////////////////////////// inline
inline SmartPointer<Graph> Graph::clone() const
{
	SmartPointer<Graph> ret(new Graph(*this));
	return ret;
}

///////////////////////////////////////////////////////// inline
inline Graph& Graph::operator=(const Graph& src)
{
	this->arches     =src.arches;
	this->nodes      =src.nodes;

	memcpy(this->nodelists_v,src.nodelists_v,sizeof(this->nodelists_v));
	memcpy(this->nodelists_n,src.nodelists_n,sizeof(this->nodelists_n));

	this->pointdim=src.pointdim;
	this->db=src.db;

	//do not copy cached geometry
	this->batch.reset();

	return *this;
}

///////////////////////////////////////////////////////// inline
inline void Graph::clear()
{
	this->nodes.flush();
	this->arches.flush();

	memset(this->nodelists_v,0,sizeof(nodelists_v));
	memset(this->nodelists_n,0,sizeof(nodelists_n));
	this->db.flush();

	//cached geomety
	this->batch.reset();
}

///////////////////////////////////////////////////////// inline
inline GraphArch&              Graph::getArch         (unsigned int A)                 {return *((GraphArch*)arches[A]);}
inline GraphNode&              Graph::getNode         (unsigned int N)                 {return *((GraphNode*)nodes[N]);}
inline unsigned short          Graph::getNDw          (unsigned int node)              {return getNode(node).Ndw;}
inline unsigned int            Graph::getNumNode      ()                               {return nodes.getNBlocked();}
inline unsigned int            Graph::getNumArch      ()                               {return arches.getNBlocked();}
inline unsigned int            Graph::getMaxNode      ()                               {return nodes.getMax();}
inline unsigned int            Graph::getMaxArch      ()                               {return arches.getMax();}
inline unsigned short          Graph::getNUp          (unsigned int node)              {return getNode(node).Nup;}
inline unsigned int            Graph::getFirstDwArch  (unsigned int N)                 {return getNode(N).Dw;}
inline unsigned int            Graph::getFirstUpArch  (unsigned int N)                 {return getNode(N).Up;}
inline unsigned int            Graph::getLastDwArch   (unsigned int N)                 {return getArch(getFirstDwArch(N)).N1_prev;}
inline unsigned int            Graph::getLastUpArch   (unsigned int N)                 {return getArch(getFirstUpArch(N)).N0_prev;}
inline unsigned int            Graph::getFirstDwNode  (unsigned int N)                 {return getArch(getFirstDwArch(N)).N0;}
inline unsigned int            Graph::getFirstUpNode  (unsigned int N)                 {return getArch(getFirstUpArch(N)).N1;};
inline unsigned int            Graph::getLastDwNode   (unsigned int N)                 {return getArch(getLastDwArch (N)).N0;}
inline unsigned int            Graph::getLastUpNode   (unsigned int N)                 {return getArch(getLastUpArch (N)).N1;}
inline unsigned int            Graph::getNextDwArch   (unsigned int A)                 {return getArch(A).N1_next;}
inline unsigned int            Graph::getPrevDwArch   (unsigned int A)                 {return getArch(A).N1_prev;}
inline unsigned int            Graph::getNextUpArch   (unsigned int A)                 {return getArch(A).N0_next;}
inline unsigned int            Graph::getPrevUpArch   (unsigned int A)                 {return getArch(A).N0_prev;}
inline const unsigned char&    Graph::Level           (unsigned int N)                 {return getNode(N).Level;}
inline unsigned char&          Graph::Sign            (unsigned int N)                 {return getNode(N).Sign;}
inline unsigned int            Graph::getN0           (unsigned int arch)              {return getArch(arch).N0;}
inline unsigned int            Graph::getN1           (unsigned int arch)              {return getArch(arch).N1;}
inline unsigned int&           Graph::NodeData        (unsigned int node)              {return getNode(node).Info;}
inline unsigned int&           Graph::ArchData        (unsigned int arch)              {return getArch(arch).Info;}
inline unsigned int&           Graph::NodeTmp         (unsigned int node)              {return getNode(node).Tmp;}
inline unsigned int            Graph::getNextUpBrother(unsigned int A)                 {return getN0(getNextDwArch(A));}
inline unsigned int            Graph::getNextDwBrother(unsigned int A)                 {return getN1(getNextUpArch(A));}
inline unsigned int            Graph::getPrevUpBrother(unsigned int A)                 {return getN0(getPrevDwArch(A));}
inline unsigned int            Graph::getPrevDwBrother(unsigned int A)                 {return getN1(getPrevUpArch(A));}
inline int                     Graph::getNCells       (int L)					       {return (int)nodelists_n[L];}



///////////////////////////////////////////////////////// inline
inline void Graph::remArch(unsigned int N0,unsigned int N1,Direction dir) 
	{remArch(findArch(N0,N1,dir),dir);}


///////////////////////////////////////////////////////// inline
inline GraphIterator Graph::goDw(unsigned int N)
{
	GraphIterator ret;
	ret.g    = this;
	ret.cont = 0;
	ret.dir  = DIRECTION_DOWN;
	ret.max  = (getNode(N).Ndw);
	ret.A    = (getNode(N). Dw);
	return ret;
}

///////////////////////////////////////////////////////// inline
inline GraphIterator Graph::goUp(unsigned int N)
{
	GraphIterator ret;
	ret.g    = this;
	ret.cont = 0;
	ret.dir  = DIRECTION_UP;
	ret.max  = (getNode(N).Nup);
	ret.A    = (getNode(N). Up);
	return ret;	
}

///////////////////////////////////////////////////////// inline
inline GraphListIterator Graph::each(unsigned char L) 
{
	GraphListIterator ret;
	ret.g=this;
	ret.Prev=0;
	ret.Cursor=this->nodelists_v[L];
	return ret;
};

///////////////////////////////////////////////////////// inline
inline unsigned int Graph::addNode(unsigned char L)
{
	unsigned int N=nodes.alloc();
	GraphNode& node=getNode(N);
	node.Ndw=node.Nup=0;
	node. Dw=node. Up=0;
	node.Info	=0;
	node.Level	=L;
	node.Id		=0;
	if (nodelists_v[L]) getNode(nodelists_v[L]).Prev=N;
	node.Next=nodelists_v[L];
	node.Prev=0;
	nodelists_v[L]=N;
	++nodelists_n[L];
	return N;
}

///////////////////////////////////////////////////////// inline
inline unsigned int Graph::addVertex(const Vecf& v)
{
	DebugAssert(v.dim==this->pointdim);
	unsigned int V=addNode(0);
	float* coord=getGeometry(V,true);
	memcpy(coord,v.mem,db.itemsize());
	return V;
}

///////////////////////////////////////////////////////// inline
inline unsigned int Graph::addVertex(const float* src)
{
	unsigned int V=addNode(0);
	float* coord=getGeometry(V,true);
	memcpy(coord,src,db.itemsize());
	return V;
}

///////////////////////////////////////////////////////// inline
inline unsigned int Graph::addEdge(unsigned int N0,unsigned int N1)
{
	unsigned int E=addNode(1);
	addArch(N0,E);
	addArch(N1,E);
	return E;
}

///////////////////////////////////////////////////////// inline
inline unsigned int Graph::addFace2d(const std::vector<int>& verts)
{
	int n=(int)verts.size();
		
	unsigned int F=addNode(2);
	
	for (int i=0;i<n;i++)
	{	
		unsigned int p1=verts[i      ];
		unsigned int p2=verts[(i+1)%n];

		unsigned int E;

		if (!(E=this->findFirstCommonNode(p1,p2,DIRECTION_UP)))
		{
			E=addNode(1);
			addArch(p1,E);
			addArch(p2,E);
		}
		addArch(E,F);
	}
	return F;
}

///////////////////////////////////////////////////////// inline
inline unsigned int Graph::addPlane(const Vecf& h)
{
	DebugAssert(h.dim==this->pointdim);
	unsigned int H=addNode(pointdim-1);
	float* coord=getGeometry(H,true);
	memcpy(coord,h.mem,db.itemsize());
	return H;
}


///////////////////////////////////////////////////////// inline
inline unsigned int Graph::addArch(unsigned int N0,unsigned int N1,Direction dir,unsigned int _A0,unsigned int _A1)
{
	unsigned int K=arches.alloc();

	GraphArch& a=getArch(K);

	//important when a.N0_prev=A.N0_next=0 means that the arch exists only for dir=DIRECTION_DOWN
	//               a.N1_prev=A.N1_next=0 means that the arch exists only for dir=DIRECTION_UP

	memset(&a,0,sizeof(GraphArch)); 
	
	GraphNode& n0=getNode(N0);
	GraphNode& n1=getNode(N1);

	a.N0=N0;
	a.N1=N1;

	if (dir!=DIRECTION_DOWN)
	{
		if (!(n0.Nup++)) 
			{n0.Up=a.N0_prev=a.N0_next=K;}
		else
		{
			unsigned int A0=_A0;
			if (!A0) A0=n0.Up;
			a.N0_next=A0;
			a.N0_prev=getArch(A0).N0_prev;
			getArch(a.N0_prev).N0_next=getArch(A0).N0_prev=K;
			if (_A0 && _A0==n0.Up) n0.Up=K;
		}
	}
	else
	{
		a.N0_prev=a.N0_next=0;
	}

	if (dir!=DIRECTION_UP)
	{
		if (!(n1.Ndw++)) 
			{n1.Dw=a.N1_prev=a.N1_next=K;}
		else
		{
			unsigned int A1=_A1;
			if (!A1) A1=n1.Dw;
			a.N1_next=A1;
			a.N1_prev=getArch(A1).N1_prev;
			getArch(a.N1_prev).N1_next=getArch(A1).N1_prev=K;
			if (_A1 && _A1==n1.Dw) n1.Dw=K;
		}
	}
	else
	{
		a.N1_prev=a.N1_next=0;
	}

	return K;
}

///////////////////////////////////////////////////////// inline
inline void Graph::addArchDirection(unsigned int A,Direction dir,unsigned int _A0,unsigned int _A1)
{
	const unsigned int& K=A;
	GraphArch& a =getArch(A);
	GraphNode& n0=getNode(a.N0);
	GraphNode& n1=getNode(a.N1);

	//important when a.N0_prev=A.N0_next=0 means that the arch exists only for dir=DIRECTION_DOWN
	//               a.N1_prev=A.N1_next=0 means that the arch exists only for dir=DIRECTION_UP
	DebugAssert(dir!=DIRECTION_UP_AND_DOWN);

	if (dir==DIRECTION_UP)
	{
		DebugAssert(!a.N0_prev && !a.N0_next);

		if (!(n0.Nup++)) 
			{n0.Up=a.N0_prev=a.N0_next=K;}
		else
		{
			unsigned int A0=_A0;
			if (!A0) A0=n0.Up;
			a.N0_next=A0;
			a.N0_prev=getArch(A0).N0_prev;
			getArch(a.N0_prev).N0_next=getArch(A0).N0_prev=K;
			if (_A0 && _A0==n0.Up) n0.Up=K;
		}
	}


	if (dir==DIRECTION_DOWN)
	{
		DebugAssert(!a.N1_prev && !a.N1_next);

		if (!(n1.Ndw++)) 
			{n1.Dw=a.N1_prev=a.N1_next=K;}
		else
		{
			unsigned int A1=_A1;
			if (!A1) A1=n1.Dw;
			a.N1_next=A1;
			a.N1_prev=getArch(A1).N1_prev;
			getArch(a.N1_prev).N1_next=getArch(A1).N1_prev=K;
			if (_A1 && _A1==n1.Dw) n1.Dw=K;
		}
	}
}

///////////////////////////////////////////////////////// inline
inline unsigned int Graph::findArch(unsigned int N0,unsigned int N1,Direction dir)
{
	GraphNode& n0=getNode(N0);
	GraphNode& n1=getNode(N1);

	unsigned short i;
	unsigned int K;
	GraphArch* a;

	if (dir==DIRECTION_UP || (dir==DIRECTION_UP_AND_DOWN && n0.Nup<=n1.Ndw))
	{
		for (i=0,a=&getArch(K=n0.Up);i!=n0.Nup;i++,a=&getArch(K=a->N0_next))
		{
			if (a->N1==N1) 
				return K;
		}
	}
	else
	{
		for (i=0,a=&getArch(K=n1.Dw);i!=n1.Ndw;i++,a=&getArch(K=a->N1_next))
		{
			if (a->N0==N0) 
				return K;
		}
	}

	return 0;	
}

///////////////////////////////////////////////////////// inline
inline unsigned int Graph::findVertex(const Vecf& to_compare) 
{
	for (GraphListIterator it=this->each(0);!it.end();it++)
	{
		unsigned int V=*it;
		float* coords=getGeometry(V);
		if (!memcmp(to_compare.mem,coords,db.itemsize())) return V;
	}

	return 0;//not found
}

///////////////////////////////////////////////////////// inline
inline unsigned int Graph::findFirstCommonNode(unsigned int N,unsigned int M,Direction dir)
{
	DebugAssert(N!=M);

	if (dir & DIRECTION_UP)
	{
		for (GraphIterator it=goUp(N  );!it.end();it++)
		for (GraphIterator jt=goDw(*it);!jt.end();jt++)
			if (*jt==M) return *it;
	}
	if (dir & DIRECTION_DOWN)
	{
		for (GraphIterator it=goDw(N  );!it.end();it++)
		for (GraphIterator jt=goUp(*it);!jt.end();jt++)
			if (*jt==M) return *it;
	}
	return 0;  //not found
}

///////////////////////////////////////////////////////// inline
inline void Graph::remArch(unsigned int arch,Direction dir)
{
	GraphArch&  a=getArch(arch);
	GraphNode& n0=getNode(a.N0);
	GraphNode& n1=getNode(a.N1);

	//if the arch goes up (== is inserted in the node)
	if ((dir & DIRECTION_UP) && (a.N0_prev && a.N0_next))
	{
		DebugAssert(n0.Nup>0);

		if (!(--n0.Nup))
			{n0.Up=0;}
		else
		{
			if (n0.Up==arch) n0.Up=a.N0_next;
			getArch(a.N0_prev).N0_next=a.N0_next;
			getArch(a.N0_next).N0_prev=a.N0_prev;
		}

		a.N0_prev=a.N0_next=0;
	}
	 
	if ((dir & DIRECTION_DOWN)  && (a.N1_prev && a.N1_next)) 
	{
		DebugAssert(n1.Ndw>0);

		if (!(--n1.Ndw))
			{n1.Dw=0;}
		else
		{
			if (n1.Dw==arch) n1.Dw=a.N1_next;
			getArch(a.N1_prev).N1_next=a.N1_next;
			getArch(a.N1_next).N1_prev=a.N1_prev;
		} 

		a.N1_prev=a.N1_next=0;
	}

	//doesn't link in DOWN and UP direction
	if (!a.N0_next && !a.N1_next)
		arches.free(arch);
}


///////////////////////////////////////////////////////// inline
inline void Graph::swapDwOrder(unsigned int A,unsigned int B)
{
	if (A==B) return;

	GraphArch& a=getArch(A);
	GraphArch& b=getArch(B);
	DebugAssert(a.N1==b.N1);

	unsigned int Ap=a.N1_prev,An=a.N1_next;
	unsigned int Bp=b.N1_prev,Bn=b.N1_next;	

	#define link(A,B) {getArch(A).N1_next=(B);getArch(B).N1_prev=(A);}
	
	if (An!=B || Bn!=A)
	{
		if (Ap!=B) {link(Ap,B);}
		if (An!=B) {link(B,An);} else {link(B,A);}
		if (Bp!=A) {link(Bp,A);}
		if (Bn!=A) {link(A,Bn);} else {link(A,B);}
	}

	if (getNode(a.N1).Dw==A)
		getNode(a.N1).Dw=B;

	#undef link
}

///////////////////////////////////////////////////////// inline
inline void Graph::changeLevel(unsigned int N,unsigned char Lnew)
{
	GraphNode& n=getNode(N);
	unsigned char Lold=n.Level;

	//useless call
	if (Lnew==Lold) 
		return;

	//togli dalla lista <nodelists>
	if (n.Prev) getNode(n.Prev).Next=n.Next;
	if (n.Next) getNode(n.Next).Prev=n.Prev;
	if (nodelists_v[Lold]==N) nodelists_v[Lold]=n.Next;
	--nodelists_n[Lold];

	//aggiungi alla lista <nodelist>
	if (nodelists_v[Lnew]) getNode(nodelists_v[Lnew]).Prev=N;
	n.Next=nodelists_v[Lnew];
	n.Prev=0;
	nodelists_v[Lnew]=N;
	++nodelists_n[Lnew];
}



///////////////////////////////////////////////////////// inline
inline void Graph::remNode(unsigned int N,bool recursive)
{
	//remove geometry
	if  (NodeData(N))
		db.free(NodeData(N));

	//remove arch up
	for (unsigned int A;A=getFirstUpArch(N);) 
		remArch(A);
	
	//remove arch down
	for (unsigned int A;A=getFirstDwArch(N);) 
	{
		unsigned int Down=getN0(A);
		remArch(A);
		if (recursive && !getNUp(Down)) remNode(Down);
	}

	GraphNode& n=getNode(N);

	//togli dalla lista <nodelists>
	unsigned char L=n.Level;
	if (n.Prev) getNode(n.Prev).Next=n.Next;
	if (n.Next) getNode(n.Next).Prev=n.Prev;

	if (nodelists_v[L]==N) nodelists_v[L]=n.Next;
	--nodelists_n[L];
	nodes.free(N);
}

//access the geom of a cell
inline float* Graph::getGeometry(unsigned int N,bool bCreateIfNotExists)
{
	unsigned int ND=NodeData(N);

	if (ND) return (float*)db[ND];

	if (!bCreateIfNotExists) 
		return 0; //failed
		
	NodeData(N)=ND=db.alloc();
	float* coord=(float*)db[ND];
	memset(coord,0,db.itemsize());
	coord[0]=1; //is a point by default
	return coord;
}


inline Vecf Graph::getVecf(unsigned int N)
{
	float* coord=getGeometry(N);
	DebugAssert(coord!=0);
	return Vecf(this->pointdim,coord);
}

inline void Graph::setVecf(unsigned int N,Vecf v)
{
	float* coord=getGeometry(N,true);
	memcpy(coord,v.mem,db.itemsize());
}

///////////////////////////////////////////////////////// inline
inline void Graph::releaseGeom(unsigned int GraphNode)
{
	unsigned int& Info=NodeData(GraphNode);
	if (Info) db.free(Info);
	Info=0;
}



///////////////////////////////////////////////////////// inline
inline int Graph::getMaxDimCells()
{
	int ret=0;
	while (getNCells(ret+1)) ret++;
	return ret;
}

///////////////////////////////////////////////////////// inline
inline bool Graph::contains(unsigned int C,const Vecf& point,float tolerance)
{
	DebugAssert(point.dim==this->pointdim);
	DebugAssert(tolerance>0);

	//test all cells
	if (!C)
	{
		for (GraphListIterator jt=each(pointdim);!jt.end();jt++)
			if (contains(*jt,point,tolerance)) return true;
		return false;
	}

	//special case for pointdim 0
	if (this->pointdim==0)
	{
		//it's enough if I have one point
		return getNCells(0)>=1;
	}

	//special case for pointdim 1
	if (this->pointdim==1)
	{
		float value=point[1];
		DebugAssert(getNDw(C)==2);
		unsigned int V0=getFirstDwNode(C);float v0=getGeometry(V0)[1];
		unsigned int V1=getLastDwNode (C);float v1=getGeometry(V1)[1];
		if (v0>v1) {Swap(float,v0,v1);}
		return (value>=v0 && value<=v1)?true:false;
	}

	//pointdim>=2
	Vecf centroid =getBoundingBall(C).center;

	//for each pointdim-1 cell
	for (GraphIterator it=goDw(C);!it.end();it++)
	{
		unsigned int F=*it;
		Planef h(pointdim,getGeometry(*it));
		float dist=h.getDistance(centroid);

		if (fabs(dist)<tolerance)
			return false; //cannot give a right orientation, probably the cell is too small!
		else
		{
			if (dist>0) h*=-1;//reverse face since the centroid should be classified in the below space
			if (h.getDistance(point)>0)
				return false;
		}
	}

	//all faces gave correct result
	return true;
}





inline					GraphIterator::GraphIterator()					           {}
inline					GraphIterator::GraphIterator(const GraphIterator& src)     {*this=src;}
inline bool				GraphIterator::end()								       {return cont>=max;}
inline unsigned int			GraphIterator::getNode()							       {return (dir==Graph::DIRECTION_UP)?(g->getArch(A).N1):(g->getArch(A).N0);}
inline unsigned int			GraphIterator::getArch()							       {return A;}
inline unsigned int			GraphIterator::operator*()						           {return getNode();}
inline GraphIterator	GraphIterator::next()								       {return GraphIterator(*this)++;}
inline GraphIterator	GraphIterator::prev()								       {return GraphIterator(*this)--;}
inline GraphIterator&   GraphIterator::operator++(int)                             {cont++;A=(dir==Graph::DIRECTION_UP)?(g->getArch(A).N0_next):(g->getArch(A).N1_next);return *this;}
inline GraphIterator&   GraphIterator::operator--(int)                             {cont++;A=(dir==Graph::DIRECTION_UP)?(g->getArch(A).N0_prev):(g->getArch(A).N1_prev);return *this;}
inline void             GraphIterator::goForward()                                 {this->operator++(1);}
inline void             GraphIterator::goBackward()                                {this->operator--(1);}


inline                       GraphListIterator::GraphListIterator()									    {}
inline			             GraphListIterator::GraphListIterator(const GraphListIterator& src)	        {*this=src;}
inline bool		             GraphListIterator::end()													{return Cursor==0;}
inline unsigned int	             GraphListIterator::operator*()												{return Cursor;}
inline GraphListIterator     GraphListIterator::next()													{return GraphListIterator(*this)++;}
inline GraphListIterator&    GraphListIterator::operator++(int)											{Prev=Cursor;Cursor=g->getNode(Cursor).Next;return *this;}
inline unsigned int                GraphListIterator::getNode()                                               {return this->operator*();}
inline unsigned int	             GraphListIterator::getPrevNode()										    {return Prev;}
inline void                  GraphListIterator::goForward()                                             {this->operator++(1);}

inline unsigned int                GraphNavigator::getNCells (int level      )                                {return nnav[level];}
inline unsigned int                GraphNavigator::getCell   (int level,int i)                                {DebugAssert(i>=0 && i<(int)nnav[level]);return nav[level][i];}

//constructor
inline GraphKMem::GraphKMem(int itemsize)
{
	//should at least contain a pointer to the next element inside mem
	DebugAssert(itemsize>=sizeof(unsigned int));
	this->m_itemsize=itemsize;
	this->m_mem=0;
	flush();
}

//copy constructor
inline GraphKMem::GraphKMem(const GraphKMem& src)
{
	this->m_itemsize=src.m_itemsize;
	this->m_max     =src.m_max;
	this->m_num     =src.m_num;
	this->m_ffree   =src.m_ffree;
	this->m_mem     =MemPool::getSingleton()->malloc(m_itemsize*this->m_max);
	memcpy(this->m_mem,src.m_mem,m_itemsize*this->m_max);
}

//destructor
inline GraphKMem::~GraphKMem()
{
	int old_size=m_max*m_itemsize;
	MemPool::getSingleton()->free(old_size,this->m_mem);
	this->m_mem=0;
}

//copy
inline GraphKMem& GraphKMem::operator=(const GraphKMem& src)
{
	int old_m_max=m_max;
	int old_size =m_max*m_itemsize;

	this->m_itemsize=src.m_itemsize;
	this->m_max     =src.m_max;
	this->m_num     =src.m_num;
	this->m_ffree   =src.m_ffree;

	int new_size = m_itemsize*m_max;

	this->m_mem=MemPool::getSingleton()->realloc(old_size,m_mem,new_size);
	memcpy(this->m_mem,src.m_mem,m_itemsize*this->m_max);
	return *this;
}

inline GraphKMem::GraphKMem()
{
	this->m_itemsize=0;
	this->m_max=0;
	this->m_num=0;
	this->m_ffree=0;
	this->m_mem=0;
}

//fast copy
//inline GraphKMem* GraphKMem::clone()
//{
//	return new GraphKMem(*this);
//}

//flush memory
inline void GraphKMem::flush()
{
	int old_size=m_max*m_itemsize;

	//dispose old memory
	if (this->m_mem) 
		MemPool::getSingleton()->free(old_size,m_mem);
	
	this->m_mem=MemPool::getSingleton()->malloc(m_itemsize*2);

	m_max    = 2;
	m_num    = 1; //block first item
	m_ffree  = 1;

	*((unsigned int*)(((unsigned char*)m_mem)+1*m_itemsize))=0;
}

//access an item 
inline void* GraphKMem::operator[](unsigned int K)
{
	return (void*)(((unsigned char*)m_mem)+K*m_itemsize);
}


//get (and block) a new item
inline unsigned int GraphKMem::alloc()
{
	if (m_max==m_num) needed(1);
	unsigned int K=m_ffree;
	m_ffree=*((unsigned int*)(((unsigned char*)m_mem)+K*m_itemsize));
	++m_num;
	return K;
}

//release a previous blocked item
inline void GraphKMem::free(unsigned int K)
{
	*((unsigned int*)(((unsigned char*)m_mem)+K*m_itemsize))=m_ffree;
	m_ffree=K;
	--m_num;
}

//make sure there is enough memory
//<n> is the number of items I need to block
//if you do not call memory every time you "block" a memory(1) is automatically called
//the memory(<n>) speed up the allocation for a bunch of items
inline void GraphKMem::needed(unsigned int n)
{
	//useless call
	if ((m_max-m_num)>=n) 
		return;
	
	unsigned int old_m_max  = m_max;
	int    old_size   = m_max*m_itemsize;

	//double the memory until it reaches the wanted size
	if (!m_max) m_max=n;
	while ((m_max-m_num)<n) m_max<<=1;
	m_max=max2(32,m_max);

	int    new_size=m_itemsize*m_max;

	this->m_mem=MemPool::getSingleton()->realloc(old_size,m_mem,new_size);

	for (unsigned int i=old_m_max;i<m_max-1;i++) 
		*((unsigned int*)(((unsigned char*)m_mem)+i*m_itemsize))=(i+1);

	*((unsigned int*)(((unsigned char*)m_mem)+(m_max-1)*m_itemsize))=m_ffree;
	m_ffree=old_m_max;
}

//number of elements currenty blocked
inline unsigned int GraphKMem::getNBlocked()
{
	//do not count item 0
	return m_num-1;
} 

//number of m_max elements
inline unsigned int GraphKMem::getMax()
{
	return m_max;  
}

//size of single item
inline size_t GraphKMem::itemsize() const
{
	return m_itemsize;
}

//return memsize
inline size_t GraphKMem::memsize() const
{
	return m_itemsize*m_max;
}

//resize (change the item size)
inline void GraphKMem::resize(int new_itemsize)
{
	if (new_itemsize==m_itemsize)
		return;

	DebugAssert(new_itemsize>=sizeof(unsigned int));

	int old_size=m_max*m_itemsize;
	int new_size=m_max*new_itemsize;

	unsigned char* new_mem=(unsigned char*)MemPool::getSingleton()->malloc(new_size);
	memset(new_mem,0,new_itemsize*m_max);

	unsigned char* p1=(unsigned char*)this->m_mem;
	unsigned char* p2=(unsigned char*)new_mem;

	int minsize=min2(m_itemsize,new_itemsize);

	for (unsigned int i=0;i<m_max;i++)
	{
		memcpy(p2,p1,minsize);
		p1+=m_itemsize;
		p2+=new_itemsize;
	}

	MemPool::getSingleton()->free(old_size,m_mem);

	this->m_itemsize=new_itemsize;
	this->m_mem=new_mem;
}

