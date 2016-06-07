
#include <xge/xge.h>
#include <xge/graph.h>


#include <algorithm>


#if PYPLASM_WINDOWS
#define NOMINMAX 1
#include <windows.h>
#else
#define CALLBACK 
#endif



//I need qhull here
extern "C"
{
#include "qhull/qset.h"
#include "qhull/qhull.h"
}


//note: i need two extra levels
static SinglePool pool_nnav(sizeof(unsigned int)*(GRAPH_MAX_GEOMETRIC_DIMENSION+2));
static SinglePool pool_nav (sizeof(unsigned int)*GRAPH_MAX_CELLS_PER_LEVEL);
 
////////////////////////////////////////////////////////////////
GraphNavigator::GraphNavigator()
{
	//alloc using pool of ojects
	this->nnav=(unsigned int*)pool_nnav.malloc();
	for (int i=0;i<(GRAPH_MAX_GEOMETRIC_DIMENSION+2);++i) 
		this->nav[i]=(unsigned int*)pool_nav.malloc();
}


////////////////////////////////////////////////////////////////
GraphNavigator::~GraphNavigator()
{
	pool_nnav.free(this->nnav);

	for (int i=0;i<(GRAPH_MAX_GEOMETRIC_DIMENSION+2);++i) 
		pool_nav.free(this->nav[i]);
}

////////////////////////////////////////////////////////////////
// important function, find the DISTINCT union of reachable cells
// between two levels
////////////////////////////////////////////////////////////////
void Graph::graph_cell_union(Graph* g,bool bUp,unsigned int& ndstcells,unsigned int* dstcells,const unsigned int nsrccells,const unsigned int* srccells)
{
	XgeDebugAssert(dstcells!=srccells);
	unsigned int i;

	ndstcells=0;

	// i want the cell only one time
	for (i=0;i<nsrccells;++i)
	{
		for(GraphIterator it=(bUp)?g->goUp(srccells[i]):g->goDw(srccells[i]);!it.end();it++)
			g->NodeTmp(*it)=0;
	}

	for (i=0;i<nsrccells;++i)
	{
		for(GraphIterator it=(bUp)?g->goUp(srccells[i]):g->goDw(srccells[i]);!it.end();it++)
		{
			//seems wrong to me this version, but it was in Plasm6
			//if ((bUp   && ++nodeTmp(*it)==getNDw(*it)) ||
			//	 (!bUp  && ++nodeTmp(*it)==1         ))

			if (++(g->NodeTmp(*it))==1) 
				dstcells[ndstcells++]=*it;

			XgeDebugAssert(g->NodeTmp(*it)<=nsrccells); //should not happen
		}	
	}

	XgeDebugAssert(ndstcells<=GRAPH_MAX_CELLS_PER_LEVEL);
}



////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void Graph::graph_cell_intersection(Graph* g,bool bUp,unsigned int& ndstcells,unsigned int* dstcells,const unsigned int nsrccells,const unsigned int* srccells)
{
	ndstcells=0;

	for (unsigned int i=0;i<nsrccells;++i)
		for(GraphIterator it=(bUp)?g->goUp(srccells[i]):g->goDw(srccells[i]);!it.end();it++) 
			g->NodeTmp(*it)=0;

	for (unsigned int i=0;i<nsrccells;++i)
		for(GraphIterator it=(bUp)?g->goUp(srccells[i]):g->goDw(srccells[i]);!it.end();it++)
			if (++(g->NodeTmp(*it))==nsrccells) dstcells[ndstcells++]=*it;

	XgeDebugAssert(ndstcells<=GRAPH_MAX_CELLS_PER_LEVEL);
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
SmartPointer<Batch> Graph::getBatch()
{
	Graph* g=this;

	//always return a new instance of Batch (for example if you want to modify the material
	//i do not want to have this->batch modified)
	if (this->batch)
		return SmartPointer<Batch>(new Batch(*this->batch));
	
	//create a new one
	this->batch.reset(new Batch());
	this->batch->primitive=Batch::POINTS;
	int view_cell_level=min2(2,g->getMaxDimCells());

	//cannot view
	if (view_cell_level<0 || view_cell_level>2 || !g->getNCells(view_cell_level))
		return SmartPointer<Batch>(new Batch(*this->batch));

	//export only points
	if (!view_cell_level)
	{
		batch->primitive=Batch::POINTS;
		batch->vertices .reset(new Array(g->getNCells(0)*3));
		float* pv=(float*)batch->vertices->c_ptr();

		for (GraphListIterator it=g->each(0);!it.end();it++)
		{
			float* coord=g->getGeometry(*it);
			*pv++=(g->pointdim>=1)?(coord[1]/coord[0]):0.0f;
			*pv++=(g->pointdim>=2)?(coord[2]/coord[0]):0.0f;
			*pv++=(g->pointdim>=3)?(coord[3]/coord[0]):0.0f;
		}
		return SmartPointer<Batch>(new Batch(*this->batch));
	}

	//export only lines
	if (view_cell_level==1)
	{
		batch->primitive=Batch::LINES;
		batch->vertices.reset(new Array(g->getNCells(1)*6));
		float* pv=(float*)batch->vertices->c_ptr();

		for (GraphListIterator it=g->each(1);!it.end();it++)
		{
			unsigned int V0=g->getFirstDwNode(*it);float* coord0=g->getGeometry(V0);
			unsigned int V1=g->getLastDwNode (*it);float* coord1=g->getGeometry(V1);

			*pv++=(g->pointdim>=1)?(coord0[1]/coord0[0]):0.0f;
			*pv++=(g->pointdim>=2)?(coord0[2]/coord0[0]):0.0f;
			*pv++=(g->pointdim>=3)?(coord0[3]/coord0[0]):0.0f;

			*pv++=(g->pointdim>=1)?(coord1[1]/coord1[0]):0.0f;
			*pv++=(g->pointdim>=2)?(coord1[2]/coord1[0]):0.0f;
			*pv++=(g->pointdim>=3)?(coord1[3]/coord1[0]):0.0f;
		}
		return SmartPointer<Batch>(new Batch(*this->batch));
	}

	//export triangles
	XgeDebugAssert(view_cell_level==2);

	//necessary condition for normals to be correctly orientated
	fixBoundaryFaceOrientation(0);

	batch->primitive=Batch::TRIANGLES;

	//count triangles
	bool bFull=g->getNCells(3)>0;
	int ntriangles=0;
	for (GraphListIterator it=g->each(2);!it.end();it++)
	{
		//filter boundary
		if (bFull && g->getNUp(*it)==2) continue;
		ntriangles+=(g->getNDw(*it)-2);
	}

	#if 0
	//with full representation I can export only boundary faces
	//otherwise I export the triangle two times
	//by default face culling is enabled for triangles (seee plasm.display)
	//need to export two triangles (back and front)
	if (!bFull) 
		ntriangles*=2;
	#endif

	batch->vertices .reset(new Array(ntriangles*9));float* pv=(float*)batch->vertices->c_ptr();
	batch->normals  .reset(new Array(ntriangles*9));float* pn=(float*)batch->normals ->c_ptr();

	for (GraphListIterator it=g->each(2);!it.end();it++)
	{
		//filter boundary faces
		if (bFull && g->getNUp(*it)==2) continue;

		//collects oriented triangles
		unsigned int Prev0=g->getFirstDwNode(g->getLastDwNode(*it)),P0;
		unsigned int Prev1=g->getLastDwNode (g->getLastDwNode(*it)),P1;
		unsigned int V0=0,V1=0,V2=0;
		for (GraphIterator IE=g->goDw(*it);!IE.end();IE++,Prev0=P0,Prev1=P1)
		{
			P0=g->getFirstDwNode(*IE);
			P1=g->getLastDwNode (*IE);
			if (P0!=Prev0 && P0!=Prev1) {Swap(unsigned int,P0,P1);}
			if (!V0)  {V0=P0;continue;}
			if (!V1)  {V1=P0;continue;}
			V2=P0;

			float* c0=g->getGeometry(V0);Vec3f v0( g->pointdim>=1?(c0[1]/c0[0]):0.0f , g->pointdim>=2?c0[2]/c0[0]:0.0f , g->pointdim>=3?c0[3]/c0[0]:0.0f );
			float* c1=g->getGeometry(V1);Vec3f v1( g->pointdim>=1?(c1[1]/c1[0]):0.0f , g->pointdim>=2?c1[2]/c1[0]:0.0f , g->pointdim>=3?c1[3]/c1[0]:0.0f );
			float* c2=g->getGeometry(V2);Vec3f v2( g->pointdim>=1?(c2[1]/c2[0]):0.0f , g->pointdim>=2?c2[2]/c2[0]:0.0f , g->pointdim>=3?c2[3]/c2[0]:0.0f );

			Vec3f normal=(g->pointdim==3 && g->NodeData(*it))?Vec3f(g->getGeometry(*it)+1):Vec3f(0,0,1);

			XgeDebugAssert(normal.isValid());

			//mantain good orientation (depending on the exported normal)
			if (((v1-v0).cross(v2-v0) * normal)<0) {Swap(Vec3f,v0,v2);}

			*pv++=v0.x;*pv++=v0.y;*pv++=v0.z;  *pn++=normal.x;*pn++=normal.y;*pn++=normal.z;
			*pv++=v1.x;*pv++=v1.y;*pv++=v1.z;  *pn++=normal.x;*pn++=normal.y;*pn++=normal.z;
			*pv++=v2.x;*pv++=v2.y;*pv++=v2.z;  *pn++=normal.x;*pn++=normal.y;*pn++=normal.z;

			//export the back triangle only if not full
			#if 0
			if (!bFull)
			{
				*pv++=v2.x;*pv++=v2.y;*pv++=v2.z;  *pn++=-normal.x;*pn++=-normal.y;*pn++=-normal.z;
				*pv++=v1.x;*pv++=v1.y;*pv++=v1.z;  *pn++=-normal.x;*pn++=-normal.y;*pn++=-normal.z;
				*pv++=v0.x;*pv++=v0.y;*pv++=v0.z;  *pn++=-normal.x;*pn++=-normal.y;*pn++=-normal.z;
			}
			#endif
			
			V1=V2;
		}
	}

	//safety check
	//XgeDebugAssert(pv==batch->vertices->c_ptr()+ntriangles*9);
	//XgeDebugAssert(pn==batch->normals->mem   ()+ntriangles*9);

	return SmartPointer<Batch>(new Batch(*this->batch));
}


///////////////////////////////////////////////////////////////////////////////////////////////
Boxf Graph::getBoundingBox(unsigned int C,SmartPointer<Matf> vmat,SmartPointer<Matf> hmat)
{
	//safety check
	XgeDebugAssert(!vmat || vmat->dim>=pointdim);

	GraphNavigator navigator;
	int npoints=findCells(0,C,navigator);

	int Dim=vmat?(vmat->dim):(pointdim);
	Boxf bbox(Dim);

	for (int i=0;i<npoints;i++)
	{
		Vecf v(Dim); //initialized to 0 IMPORTANT since the point can be embedded in spacedim
		memcpy(v.mem,getGeometry(navigator.nav[0][i]),sizeof(float)*(pointdim+1));
		if (vmat) v=(*vmat) * v; //transform the point
		bbox.add(v);
	}

	return bbox;
}

////////////////////////////////////////////////////////////////////////
Planef Graph::getFittingPlane(unsigned int C,SmartPointer<Matf> vmat,SmartPointer<Matf> hmat)
{
	//safety check
	XgeDebugAssert(!vmat || vmat->dim>=pointdim);

	//get cached result if possible
	if (pointdim>=2 && Level(C)==(pointdim-1) && !vmat && NodeData(C))  
	{
		float* h=getGeometry(C);
		return Planef(pointdim,h);
	}

	GraphNavigator navigator;
	int npoints=findCells(0,C,navigator);
	int Dim=vmat?(vmat->dim):(pointdim);
	std::vector<float> points(npoints*Dim);

	for (int i=0;i<(int)npoints;i++) 
	{
		Vecf v(Dim); //initialized to 0 IMPORTANT since the point can be embedded in spacedim
		memcpy(v.mem,getGeometry(navigator.nav[0][i]),sizeof(float)*(pointdim+1));
		if (vmat) v=(*vmat) * v; //transform the point

		//remove the homogeneous coordinatw
		if (v[0]!=1 && v[0]!=0) {v/=v[0];v[0]=1;}
		memcpy(&points[i*Dim],v.mem+1,sizeof(float)*Dim);
	}
	
	Planef plane=Planef::bestFittingPlane(Dim,npoints,&points[0]);
	XgeDebugAssert(plane.dim==Dim);

	//cache results
	if (pointdim>=2 && Level(C)==(pointdim-1) && !vmat)
	{
		float* dst=getGeometry(C,true);
		memcpy(dst,plane.mem,sizeof(float)*(pointdim+1));
	}

	return plane;
}


////////////////////////////////////////////////////////////////////////
Ballf Graph::getBoundingBall(unsigned int C,SmartPointer<Matf> vmat,SmartPointer<Matf> hmat)
{
	XgeDebugAssert(!vmat || vmat->dim>=pointdim);

	float* b=0;

	//get cached result if possible
	if (pointdim>=2 && Level(C)==pointdim && !vmat && NodeData(C) && (b=getGeometry(C))[0]>=0)  //-1 means not valid radius
	{
		Vecf center(pointdim,b);
		center.mem[0]=1; //is a point
		float radius=sqrt(b[0]); //I store internally the squared radius
		return Ballf(radius,center);
	}

	GraphNavigator navigator;
	int npoints=findCells(0,C,navigator);
	int Dim=vmat?(vmat->dim):(pointdim);
	std::vector<float> points(npoints*(Dim+1));
		
	for (int i=0;i<npoints;i++)
	{
		Vecf v(Dim); //initialized to 0 IMPORTANT since the point can be embedded in spacedim
		memcpy(v.mem,getGeometry(navigator.nav[0][i]),sizeof(float)*(pointdim+1));
		if (vmat) v=(*vmat) * v; //transform the point
		memcpy(&points[i*(Dim+1)],v.mem,sizeof(float)*(Dim+1));
	}

	Ballf bball=Ballf::bestFittingBall(Dim,points);
	XgeDebugAssert(bball.isValid() && bball.dim()==Dim);

	//cache results if possible
	if (pointdim>=2 && Level(C)==pointdim && !vmat)
	{
		float* dst=getGeometry(C,true);
		memcpy(dst,bball.center.mem,sizeof(float)*(pointdim+1));
		dst[0]=bball.radius*bball.radius; //i store the squared radius here! Not the radius!
	}

	return bball;
}


///////////////////////////////////////////////////////////////////////////////////////////////
void Graph::toUnitBox(Matf& vmat,Matf& hmat)
{
	//safety check
	XgeDebugAssert(vmat.dim==this->pointdim && hmat.dim==this->pointdim);

	Boxf bbox=getBoundingBox(0); //get overall bounding box

	Vecf d=bbox.size();


	Vecf t(pointdim);
	Vecf s(pointdim);

	for (int i=1;i<=pointdim;i++)
	{
		t.set(i, (float)(-0.5*(bbox.p1[i]+bbox.p2[i])));
		s.set(i, (float)(+2.0f/d[i])); 
	}

	//here I apply the transformation
	translate(t);
	scale    (s);

	//returns the matrices which transforms back to the original position
	vmat=Matf::translateH(t)*Matf::scaleH(s); //this H instead the V is CORRECT, because I need to return the inverse
	hmat=Matf::scaleV(s)*Matf::translateV(t);
}



////////////////////////////////////////////////////////////////////////
void Graph::Print(char* spaces)  
{	int L;

	Log::printf("%s-------------------------------------------------\n",spaces);
	for ( L=255;L>=0;L--)
	{
		int cont=0;
		for (GraphListIterator it=each(L);!it.end();it++)
		{
			if (!cont) Log::printf("%sLevel %d ncells %d\n",spaces,(int)L,(int)nodelists_n[L]);
			unsigned int N=*it;
			Print(N,spaces);
			Log::printf(")\n");
			++cont;
		}
		if (cont) Log::printf("\n");
	}
	Log::printf("%s-------------------------------------------------\n",spaces);
}


////////////////////////////////////////////////////////////////////////
void Graph::Print(unsigned int N,char* space)
{

	Log::printf("%s[%4d] ",space,(int)N);

	Log::printf("down(");

	GraphIterator i;

	for (i=goDw(N);!i.end();i++)
	{
		Log::printf("%d ",(int)*i);
	}
	Log::printf(") ");

	Log::printf(" up(");
	for (i=goUp(N);!i.end();i++)
	{
		Log::printf("%d ",(int)*i);
		
	}
	Log::printf(") Level(%d) ",(int)getNode(N).Level);

	//Log::printf("data(%d) sign(%d)",(int)NodeData(N),(int)getSign(N));

	if (NodeData(N))
	{
		float* g=getGeometry(N);
		Log::printf(" geom(");
		for (int i=0;i<=pointdim;i++) Log::printf("%e ",(float)g[i]);
		Log::printf(")");
	}
}

////////////////////////////////////////////////////////////////////////
void Graph::PrintStatistics()
{
	int L;
	Log::printf("Graph(%d)------------------\n",(int)pointdim);
	Log::printf("ncells\t");

	for ( L=255;L>=0;L--)
	{
		for (GraphListIterator it=each(L);!it.end();it++)
		{
			Log::printf("%d(%d) ",(int)L,nodelists_n[L]);
			break;
		}
	}
	Log::printf("\n");

	size_t t=sizeof(Graph)+sizeof(nodelists_v)+sizeof(nodelists_n)+nodes.memsize()+arches.memsize()+db.memsize();
	Log::printf("Total memory %dkb \n",(int) t/1024);

	Log::printf("Node ");
	nodes.PrintStatistics();

	Log::printf("Arch ");
	arches.PrintStatistics();

	Log::printf("Db ");
	db.PrintStatistics();
}



//////////////////////////////////////////////////////////////////////////
int Graph::findCells(int level,unsigned int Cell,GraphNavigator& navigator,bool bFast)
{
	//if  it is  double linked....
	if (bFast && Cell && Level(Cell)==pointdim && level==0 && getNUp(Cell)>0)
	{
		unsigned int* dst=navigator.nav[level];
		for (GraphIterator it=goUp(Cell);!it.end();it++) *dst++=*it;
		navigator.nnav[level]=getNUp(Cell);
		return navigator.nnav[level];
	}

	//since it's double linked....
	if (bFast && Cell && level==pointdim && Level(Cell)==0 && getNDw(Cell)>0)
	{
		unsigned int* dst=navigator.nav[level];
		for (GraphIterator it=goDw(Cell);!it.end();it++) *dst++=*it;
		navigator.nnav[level]=getNDw(Cell);
		return navigator.nnav[level];
	}

	//slow method
	if (!Cell)
	{
		unsigned int* dst=navigator.nav[level];
		for (GraphListIterator it=each(level);!it.end();it++) *dst++=*it;
		navigator.nnav[level]=getNCells(level);
		return navigator.nnav[level];
	}

	int celllvl=Level(Cell);
	navigator.nav[celllvl][0]=Cell;
	navigator.nnav[celllvl]=1;

	//the cell itself
	if (celllvl==level) 
		return 1;

	if (level<celllvl)
	{
		for (int i=celllvl;i>=(level+1);i--) 
			graph_cell_union(this,false,navigator.nnav[i-1],navigator.nav[i-1],navigator.nnav[i],navigator.nav[i]);
	}
	else
	{
		for (int i=celllvl;i<=(level-1);i++) 
			graph_cell_union(this,true,navigator.nnav[i+1],navigator.nav[i+1],navigator.nnav[i],navigator.nav[i]);
	}

	return navigator.nnav[level];
}



/////////////////////////////////////////////////////////////////////////////////
void Graph::fixBoundaryFaceOrientation(unsigned int Face)
{
	GraphNavigator navigator;

	//no planes
	if (pointdim<2) 
		return;

	// meas to fix all boundary faces
	if (!Face)
	{
		//for each (pointdim-1) cell, which is a plane
		for (GraphListIterator it=each(pointdim-1);!it.end();it++)
		{
			//calculate only for boundary face: because I can get what is internal and external only for boundaries
			if (getNUp(*it)==1)  
				fixBoundaryFaceOrientation(*it);
		}

		return ;
	}

	//only boundary face
	XgeDebugAssert(getNUp(Face)==1);

	//must calculate the plane because it has not already been computed
	if (!NodeData(Face)) 
	{
		Planef plane=getFittingPlane(Face);
		memcpy(getGeometry(Face,true),plane.mem,sizeof(float)*(pointdim+1));
	}

	//since it is a boundary face it has only one up cell
	//eventually reverse the orientation if wrong
	XgeDebugAssert(getNUp(Face)==1);
	Ballf bball=getBoundingBall(getFirstUpNode(Face));

	if (bball.isValid())
	{
		Planef plane(this->pointdim,getGeometry(Face));
		XgeDebugAssert(bball.center[0]==1); //must be a point
		plane=plane.forceBelow(bball.center);
		memcpy(getGeometry(Face),plane.mem,sizeof(float)*(pointdim+1));
	}
}



///////////////////////////////////////////////////////////////////
SmartPointer<Graph> Graph::cuboid(int pointdim,float From,float To)
{
	SmartPointer<Graph> ret;

	if (!pointdim)
	{
		ret.reset(new Graph(pointdim));
		ret->getGeometry(ret->addNode(0),true);
	}
	else
	{
		SmartPointer<Graph> cub=cuboid(pointdim-1,From,To);
		ret=cub->extrude(From,To);
		cub.reset();
	}

	return ret;
}

SmartPointer<Graph> Graph::cuboid(int pointdim,Boxf box)
{
	SmartPointer<Graph> ret;

	if (!pointdim)
	{
		ret.reset(new Graph(pointdim));
		ret->getGeometry(ret->addNode(0),true);
	}
	else
	{
		SmartPointer<Graph> cub=cuboid(pointdim-1,box);
		ret=cub->extrude(box.p1[pointdim],box.p2[pointdim]);
		cub.reset();
	}
	return ret;
}

///////////////////////////////////////////////////////////////////
SmartPointer<Graph> Graph::simplex(int pointdim)
{
	if (pointdim<=1)
		return cuboid(pointdim);

	SmartPointer<Graph> ret;
	SmartPointer<Graph> tmp=simplex(pointdim-1);
	unsigned int N=*(tmp->each(pointdim-1));
	ret=tmp->join(N,1);
	tmp.reset();
	return ret;
}




////////////////////////////////////////////////////////////////
void Graph::check()
{
	//check all cells
	for (int k=0;k<256;k++)
	{
		for (GraphListIterator i=each(k);!i.end();i++)
			check(*i);
	}
}



//////////////////////////////////////////////////////////////////////////
void Graph::check(unsigned int N)
{
	GraphIterator i,j;

	unsigned int last;

	if (!goDw(N).end())
	{
		for (i=goDw(N);!i.end();i++)
		{
			XgeDebugAssert(getArch(i.getArch()).N1==N);
			last=*i;
		}

		unsigned int _tmp=getLastDwNode(N);
		XgeDebugAssert(last==_tmp);
	}

	if (!goUp(N).end())
	{
		for (i=goUp(N);!i.end();i++)
		{
			XgeDebugAssert(getArch(i.getArch()).N0==N);
			last=*i;
		}
		XgeDebugAssert(last==getLastUpNode(N));
	}

	unsigned int cont;
	for (cont=0,i=goDw(N);!i.end();i++,cont++)
	{
		unsigned int M=*i;

		for (j=goUp(M);!j.end();j++)
		{
			if (getN1(j.getArch())==N)
			{
				XgeDebugAssert(j.getArch()==i.getArch());
				break;
			}
		}

		XgeDebugAssert(!j.end());
	}
	XgeDebugAssert(cont==getNDw(N));

	for (cont=0,i=goUp(N);!i.end();i++,cont++)
	{
		unsigned int M=*i;

		for (j=goDw(M);!j.end();j++)
		{
			if (getN0(j.getArch())==N)
			{
				XgeDebugAssert(j.getArch()==i.getArch());
				break;
			}
		}

		XgeDebugAssert(!j.end());
	}
	XgeDebugAssert(cont==getNUp(N));



	int lvl=Level(N);

	if (lvl>0)
		XgeDebugAssert(getNDw(N)>Level(N));

	if (lvl==1)
		XgeDebugAssert(getNDw(N)==2);

	if (lvl==2)
	{
		unsigned int prev0=getFirstDwNode(getLastDwNode(N)),p1;
		unsigned int prev1=getLastDwNode (getLastDwNode(N)),p2;

		for (i=goDw(N);!i.end();i++,prev0=p1,prev1=p2)
		{
			p1=getFirstDwNode(*i);
			p2=getLastDwNode (*i);

			if (p1!=prev0 && p1!=prev1) 
				Swap(unsigned int,p1,p2);
			
			XgeDebugAssert(p1==prev0 || p1==prev1) ;

			if (pointdim==3 && NodeData(N))
			{	
				float* vertex = getGeometry(p1);
				float* plane  = getGeometry(N);

				float acc=0;
				for (int ii=0;ii<=pointdim;ii++)
				{
					XgeDebugAssert(  !isnan(plane[ii]) && !isnan(vertex[ii])  );
					acc+=plane[ii]*vertex[ii];
				}
				XgeDebugAssert(fabs(acc)<1e-1);
			}
		}
	}

	if (Level(N)==(unsigned int)(pointdim-1))
		XgeDebugAssert(getNUp(N)<=2);
}




////////////////////////////////////////////////////////////////
bool Graph::orderFace2d(unsigned int face)
{
	XgeDebugAssert(Level(face)==2);

	unsigned int first   = getFirstDwArch(face);
	unsigned int tomatch = first;
	unsigned int actual  = getNextDwArch(tomatch);
	int nmatched= 0;

	std::set<unsigned int> reached;

	while(actual!=tomatch)
	{	
		unsigned int E_tomatch = getN0(tomatch);
		unsigned int E_actual  = getN0(actual );

		unsigned int A=getFirstDwNode (E_actual);
		unsigned int B=getLastDwNode  (E_actual);
		unsigned int C=getFirstDwNode (E_tomatch);
		unsigned int D=getLastDwNode  (E_tomatch);

		unsigned int N=(A==C || A==D)? (A) : ((B==C || B==D)?(B):(0)) ;

		//found?
		if (N)
		{
			//already reached
			if (reached.find(N)!=reached.end())
				return false;

			reached.insert(N);
			++nmatched;
			
			if (actual==first) //finished
			{
				//not all has been reached
				if (nmatched!=getNDw(face))
					return false;

				return true;
			}
			
			swapDwOrder(getNextDwArch(tomatch),actual);
			actual=tomatch=getNextDwArch(tomatch);
		}
		actual=getNextDwArch(actual);
	}

	//failed
	return false;
}







//////////////////////////////////////////////////////////////////////////
//very slow display only for debugging purpouses
//////////////////////////////////////////////////////////////////////////

#if 0
void Graph::render(int view_cell_level,bool bDisplayOffetLines,bool bShowNormals,bool bDisplayCellId)
{
	//cannot view
	if (view_cell_level<0 || view_cell_level>2)
		return ;

	Graph* g=this;

	//display max dim cells starting from 2
	switch(view_cell_level)
	{
	// **** POINTS *****
	case 0:
		{
			if (g->each(0).end()) break;
			SmartPointer<Batch> batch(new Batch);
			batch->primitive=Batch::POINTS;
			batch->pointsize=5;
			batch->setColor(Color4f(0.8f,0.5f,0.5f));
			batch->vertices.reset(new Array(g->getNCells(0)*3));
			float* v=(float*)batch->vertices->c_ptr();

			for (GraphListIterator  it=g->each(0);!it.end();it++)
			{
				float* P=g->getGeometry(*it);
				*v++=g->pointdim>=1?P[1]:0;
				*v++=g->pointdim>=2?P[2]:0;
				*v++=g->pointdim>=3?P[3]:0;
			}
			glcanvas->renderBatch(batch,0);
			return;
		}

	// **** LINES *****
	case 1:
		{
			if (g->each(1).end()) break;

			SmartPointer<Batch> batch(new Batch);
			batch->primitive=Batch::LINES;
			batch->linewidth=3;
			batch->setColor(Color4f(0.5f,0.8f,0.5f));
			batch->vertices.reset(new Array(g->getNCells(1)*6));
			float* v=(float*)batch->vertices->c_ptr();

			for (GraphListIterator it=g->each(1);!it.end();it++)
			{
				float* P0=g->getGeometry(g->getFirstDwNode(*it));
				float* P1=g->getGeometry(g->getLastDwNode (*it));

				*v++=g->pointdim>=1?P0[1]:0;
				*v++=g->pointdim>=2?P0[2]:0;
				*v++=g->pointdim>=3?P0[3]:0;
				*v++=g->pointdim>=1?P1[1]:0;
				*v++=g->pointdim>=2?P1[2]:0;
				*v++=g->pointdim>=3?P1[3]:0;

			}

			glcanvas->renderBatch(batch,0);
	

			//display normals
			if (bShowNormals)
			{
				batch->setColor(Color4f(1,1,0));
				v=(float*)batch->vertices->c_ptr();

				for (GraphListIterator it=g->each(1);!it.end();it++)
				{
					unsigned int E=*it;
					float* normal=g->getGeometry(E)+1;
					float* coord =g->getBoundingBall(E).center.mem+1;

					float coeff=0.05f;

					*v++=coord[0];
					*v++=coord[1];
					*v++=0;
					*v++=coord[0]+coeff*normal[0];
					*v++=coord[1]+coeff*normal[1];
					*v++=0;
				}

				glcanvas->renderBatch(batch,0);
			}
			return;
		}

	// **** FACES *****
	case 2:
		{
			if (g->each(2).end()) break;
			unsigned int FirstFace=*(g->each(2));
			bool bHasNormals=(g->pointdim==3) && g->NodeData(FirstFace)>0;

			throw "TODO";

			/*
			if (bHasNormals) 
				glEnable(GL_LIGHTING); 
			else 
				glDisable(GL_LIGHTING);

			if (bDisplayOffetLines)
			{
				glPolygonOffset(1.0,2);
				glEnable(GL_POLYGON_OFFSET_FILL);
			}

			glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
			
			for (GraphListIterator it=g->each(2);!it.end();it++)
			{
				unsigned int F=*it;

				//show normals only when working in 3d (TODO: this is not correct! when using embeding matrices...)
				if (bHasNormals)
				{
					if (g->NodeData(F)) //only when it has normals, should have!
					{
						float* N=g->getGeometry(F);XgeDebugAssert(N);
						glNormal3f(N[1],N[2],N[3]);
					}
					else
						glNormal3f(0,1,0);
				}

				unsigned int prev0=g->getFirstDwNode(g->getLastDwNode(F)),p1;
				unsigned int prev1=g->getLastDwNode (g->getLastDwNode(F)),p2;

				glBegin(GL_POLYGON);
				for (GraphIterator jt=g->goDw(F);!jt.end();jt++,prev0=p1,prev1=p2)
				{
					p1=g->getFirstDwNode(*jt);
					p2=g->getLastDwNode (*jt);
					if (p1!=prev0 && p1!=prev1) Swap(unsigned int,p1,p2);
					float* P=g->getGeometry(p1);
					glVertex3f(g->pointdim>=1?P[1]:0,g->pointdim>=2?P[2]:0,g->pointdim>=3?P[3]:0);
				}
				glEnd();
			}

			//now use the black for lines
			if (bDisplayOffetLines)
			{
				glColor3f(0,0,0);
				glEnable(GL_COLOR_MATERIAL);
				glDisable(GL_POLYGON_OFFSET_FILL);
				glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
				glLineWidth(2);
				glBegin(GL_LINES);

				for (GraphListIterator it=g->each(1);!it.end();it++)
				{
					unsigned int E=*it;

					//comment/uncomment if you want all lines
					//if (g->getNUp(E)==2) continue;

					float* p1=g->getGeometry(g->getFirstDwNode(E));
					float* p2=g->getGeometry(g->getLastDwNode (E));

					glVertex3f(g->pointdim>=1?p1[1]:0,g->pointdim>=2?p1[2]:0,g->pointdim>=3?p1[3]:0);
					glVertex3f(g->pointdim>=1?p2[1]:0,g->pointdim>=2?p2[2]:0,g->pointdim>=3?p2[3]:0);
				}

				glEnd();
				glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
				glDisable(GL_COLOR_MATERIAL);
			}

		
			//display normals
			if (bShowNormals && (this->pointdim==2 || this->pointdim==3))
			{
				glDisable(GL_LIGHTING);
				glColor3f(1,1,0);
				glBegin(GL_LINES);
				for (GraphListIterator it=g->each(2);!it.end();it++)
				{
					unsigned int F=*it;
					float* coord =g->getBoundingBall(F).center.mem+1;

					if (this->pointdim==2)
					{
						float normal[]={0,0,1};
						glVertex3f(coord[0],coord[1],0);
						glVertex3f(coord[0],coord[1],1);
					}
					else
					{
						float* normal=g->getGeometry(F)+1;
						glVertex3f(coord[0]          ,coord[1]          ,coord[2]          );
						glVertex3f(coord[0]+normal[0],coord[1]+normal[1],coord[2]+normal[2]);
					}
				}
				glEnd();
				glColor3f(1,1,1);
			}
			glEnable(GL_LIGHTING);
			*/
			return;
		}

	}//end switch


}

#endif


//////////////////////////////////////////////////////////////////////////
void Graph::embed(int new_pointdim)
{
	//cannot embed, useless call
	if (new_pointdim<=pointdim)
		return;

	//remove old planes and eventually bounding balls
	if (pointdim>=2)
	{
		for (GraphListIterator it=each(pointdim);!it.end();it++)
		{
			unsigned int& Info=NodeData(*it);
			if (Info) db.free(Info);
			Info=0;
		}

		for (GraphListIterator it=each(pointdim-1);!it.end();it++)
		{
			unsigned int& Info=NodeData(*it);
			if (Info) db.free(Info);
			Info=0;
		}
	}

	//remove links from pointdim-cells to vertices
	//they are not now full cells (since the point dim has been increased)
	//and double link are only for max dim cels
	for (GraphListIterator it=each(pointdim);!it.end();it++)
	{
		unsigned int A;
		while ((A=getFirstUpArch(*it))) remArch(A);
	}

	//resize the geometry
	this->pointdim=new_pointdim;
	db.resize(sizeof(float)*(this->pointdim+1));
	
	//the maxdim cells are now embedded in higher space
	//all the pointdim-1 cells are now faces
	//set up new planes due to the embedding (xn>=0)
	//don't care abount bounding balls
	if (pointdim>=2)
	{
		for (GraphListIterator it=each(pointdim-1);!it.end();it++)
		{
			float* gtmp=getGeometry(*it,true);
			gtmp[0]=0;
			gtmp[pointdim]=1; //xn>=0
		}
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////
SmartPointer<Graph> Graph::extrude(float From,float To)
{
	SmartPointer<Graph> dcom(new Graph(this->pointdim+1));

	if (From>To) 
		Swap(float,From,To);

	GraphListIterator i;

	//find all zero-cells
	GraphNavigator navigator;
	findCells(0,0,navigator); 

	//going up to find all cells starting from vertices (in this way I make sure I do not reach "ghost" cells)
	//can't I just use findCells at each level?
	for (int u=0;u<this->pointdim;++u)
		graph_cell_union(this,true,navigator.nnav[u+1],navigator.nav[u+1],navigator.nnav[u],navigator.nav[u]);

	#ifdef _DEBUG
	//safety check
	for (int i=0;i<=this->pointdim;i++) XgeDebugAssert(this->getNCells(i)==navigator.nnav[i]);
	#endif

	unsigned int* fullcells=navigator.nav[pointdim];
	const unsigned int& nfullcells=navigator.nnav[pointdim];

	for (int u=0;u<(int)nfullcells;u++)
	{
		unsigned int scell=fullcells[u];
		unsigned int NewUpCell=dcom->addNode(dcom->pointdim); 
		this->NodeTmp(scell)=NewUpCell; 
	}

	for (int k=0;k<=this->pointdim;++k)
	{
		unsigned int* kcells=navigator.nav[k];
		const unsigned int& nkcells=navigator.nnav[k];

		for (unsigned int i=0;i<nkcells;++i)
		{
			unsigned int scell = kcells[i];
			unsigned int dcell1=dcom->addNode(k);
			unsigned int dcell2=dcom->addNode(k);
			unsigned int dlinkcell=(k==pointdim)? this->NodeTmp(scell) : dcom->addNode(k+1);

			/* link cell->vertices */
			if (!k)
			{
				if (!this->pointdim)
				{
					dcom->addArch(dlinkcell,dcell1);
					dcom->addArch(dlinkcell,dcell2);
				}
				else
				{
					for (GraphIterator u=goDw(scell);!u.end();u++)
					{
						unsigned int _scell=*u;
						unsigned int NewUpCell=this->NodeTmp(_scell);
						dcom->addArch(NewUpCell,dcell1);
						dcom->addArch(NewUpCell,dcell2);
					}
				}
			}

			/* save map  scell->dcell1->dcell2->dlinkcell */

			this->NodeTmp(scell )=dcell1;
			dcom->NodeTmp(dcell1)=dcell2;
			dcom->NodeTmp(dcell2)=dlinkcell;

			/*  geometry  */
			
			if (!k) /* vertices */
			{	
				float* g1=dcom->getGeometry(dcell1,true);memcpy(g1,this->getGeometry(scell),this->db.itemsize());g1[this->pointdim+1]=From;
				float* g2=dcom->getGeometry(dcell2,true);memcpy(g2,this->getGeometry(scell),this->db.itemsize());g2[this->pointdim+1]=To;
			}
			else if (dcom->pointdim>=2 && (k)==(dcom->pointdim-1)) /* dcell1, dcell2 */
			{
				float* g1=dcom->getGeometry(dcell1,true);g1[0]=+From;g1[dcom->pointdim]=-1; /* xn<=from */
				float* g2=dcom->getGeometry(dcell2,true);g2[0]=  -To;g2[dcom->pointdim]=+1; /* xn>=to */		
			}
			
			/* linked cell. embed original plane */
			if (dcom->pointdim>=2 && (k+1)==(dcom->pointdim-1) && NodeData(scell)) 
			{
				/* scell is a vertex */
				if (pointdim==1) 
				{
					float* s1  = getGeometry(scell);
					float  sign=(float)((getNUp(scell)==2) || (getGeometry(getNextUpBrother(getFirstUpArch(scell)))[1]<s1[1])?+1:-1);

					float* g1=dcom->getGeometry(dlinkcell,true);
					g1[0]=-sign*s1[1];
					g1[1]=+sign;	
					g1[2]=0;
				}
				/* scell is a plane */
				else 
				{
					float* g1=dcom->getGeometry(dlinkcell,true);
					memcpy(g1,this->getGeometry(scell),this->db.itemsize());
					g1[dcom->pointdim]=0;
				}
			}

			/* connectivity */
			unsigned int Before=0;
			dcom->addArch(dcell1,dlinkcell,DIRECTION_UP_AND_DOWN,0,Before); //mantain order for 2-faces

			if (k)
			{
				for (GraphIterator i=this->goDw(scell);!i.end();i++)
				{
					unsigned int ssubface = (*i);

					unsigned int dc1      = this->NodeTmp(ssubface);
					unsigned int dc2      = dcom->NodeTmp(dc1);
					unsigned int cnew     = dcom->NodeTmp(dc2);

					dcom->addArch(dc1,dcell1);
					dcom->addArch(dc2,dcell2);
					Before=dcom->addArch(cnew,dlinkcell);
				}
			}

			dcom->addArch(dcell2,dlinkcell,DIRECTION_UP_AND_DOWN,0,Before); //mantain order for 2-faces
		}
	}

	return dcom;
}


///////////////////////////////////////////////////////////////////////////
SmartPointer<Graph> Graph::join(unsigned int N,float coord)
{
	

	//find all zero-cells
	GraphNavigator navigator;
	findCells(0,N,navigator);

	//going up to find all cells
	for (int u=0;u<this->pointdim;++u)
		graph_cell_union(this,true,navigator.nnav[u+1],navigator.nav[u+1],navigator.nnav[u],navigator.nav[u]);

	#ifdef _DEBUG
	//safety check
	for (int i=0;i<=this->pointdim;i++) XgeDebugAssert(this->getNCells(i)==navigator.nnav[i]);
	#endif

	SmartPointer<Graph> ret(new Graph(pointdim+1));
	
	unsigned int Njoin=ret->addNode(0);
	float* _coords=ret->getGeometry(Njoin,true);
	_coords[pointdim+1]=coord;
	XgeDebugAssert(getNode(N).Level==pointdim);

	unsigned int Cjoin=ret->addNode(pointdim+1);
	ret->addArch(Cjoin,Njoin);

	for (int k=0;k<=pointdim;k++)
	{
		unsigned int* kcells=navigator.nav[k];
		const unsigned int& nkcells=navigator.nnav[k];
		for (unsigned int cont=0;cont<nkcells;cont++)
		{
			unsigned int S   =kcells[cont];
			unsigned int Ck  =ret->addNode(k ) ;
			unsigned int Ckup=(k==pointdim)?Cjoin:ret->addNode(k+1);

			/*  geometry  */
			if (!k) /* vertices */
			{	
				memcpy(ret->getGeometry(Ck,true),this->getGeometry(S),this->db.itemsize());
			}
			else if (k==ret->pointdim-1) /* plane for (k-cell) */
			{
				float* gtmp=ret->getGeometry(Ck,true);	
				gtmp[pointdim+1]=-1; //Zn<=0
				gtmp[0]=0;
			}
			
			if ((k+1)==(ret->pointdim-1)) /* plane for (k+1)-cell */
			{
				if (pointdim==1)  
				{
					float X=getGeometry(S)[1];
					float  sign=(float)((getNUp(S)==2) || (getGeometry(getNextUpBrother(getFirstUpArch(S)))[1]<X)?+1:-1);
					float* gtmp=ret->getGeometry(Ckup,true);
					gtmp[0]=sign*(-coord*X);
					gtmp[1]=sign*(+coord);
					gtmp[2]=sign*(+X);
				}
				else 
				{
					float* gtmp=ret->getGeometry(Ckup,true);
					memcpy(gtmp,this->getGeometry(S),this->db.itemsize());
					gtmp[pointdim+1]=-gtmp[0]/coord;
				}
			}

			/* connectivity */
			NodeTmp(S)=Ck;
			ret->NodeTmp(Ck)=Ckup;

			ret->addArch(Ck,Ckup);

			if (!k)
			{
				ret->addArch(Njoin,Ckup);
				ret->addArch(Cjoin,Ck  );
			}
			else
			{
				for (GraphIterator i=goDw(S);!i.end();i++)
				{
					unsigned int map_ck  =NodeTmp(*i);
					unsigned int map_ckup=ret->NodeTmp(map_ck);
					ret->addArch(map_ck,Ck);
					ret->addArch(map_ckup,Ckup);
				}
			}	
		}
	}
	return ret;
}




////////////////////////////////////////////////////////////////
//new symbolic version, should never fail
////////////////////////////////////////////////////////////////


SmartPointer<Graph> Graph::power(
								   Matf& VmatT,
								   Matf& HmatT,
								   const SmartPointer<Graph> g1,SmartPointer<Matf> _g1vmat,SmartPointer<Matf> _g1hmat,
								   const SmartPointer<Graph> g2,SmartPointer<Matf> _g2vmat,SmartPointer<Matf> _g2hmat )
{
	int pdim1=g1->pointdim;
	int pdim2=g2->pointdim;
	SmartPointer<Graph> g(new Graph(pdim1+pdim2));

	std::map< std::pair<unsigned int,unsigned int> ,unsigned int> curmap,prvmap,zermap;

	for (int L=0;L<=(pdim1+pdim2);L++)
	{
		//find all cells at any level for which the sum of their level is equal to L
		for (int L1=0;L1<=pdim1;L1++)
		{
			int L2=L-L1;
			if (!(L2>=0 && L2<=pdim2)) continue; //out of good range for g2

			XgeDebugAssert((L1+L2)==L);
			for (GraphListIterator it1=g1->each(L1);!it1.end();it1++)
			for (GraphListIterator it2=g2->each(L2);!it2.end();it2++)
			{
				unsigned int C1=*it1,C2=*it2;
				unsigned int C=g->addNode(L);
				curmap[std::pair<unsigned int,unsigned int>(C1,C2)]=C;

				//store geometric coordinates
				if (!L)
				{
					XgeDebugAssert(!L1 && !L2);
					float* coord1 = g1->getGeometry(C1);XgeDebugAssert(coord1[0]==1.0f);
					float* coord2 = g2->getGeometry(C2);XgeDebugAssert(coord2[0]==1.0f);

					float*  coord=g->getGeometry(C,true);
					coord[0]=1.0f;
					memcpy(coord+1      ,coord1+1,sizeof(float)*pdim1);
					memcpy(coord+1+pdim1,coord2+1,sizeof(float)*pdim2);
				}
				else
				{
					//0 cells have no down links (apart for optimization reason)
					if (L1)
					{
						for (GraphIterator it1sub=g1->goDw(C1);!it1sub.end();it1sub++)
						{
							std::pair<unsigned int,unsigned int> sub1(*it1sub,C2);
							XgeDebugAssert(prvmap.find(sub1)!=prvmap.end());
							g->addArch(prvmap[sub1],C);
						}
					}

					if (L2)
					{
						for (GraphIterator it2sub=g2->goDw(C2);!it2sub.end();it2sub++)
						{
							std::pair<unsigned int,unsigned int> sub2(C1,*it2sub);
							XgeDebugAssert(prvmap.find(sub2)!=prvmap.end());
							g->addArch(prvmap[sub2],C);
						}
					}


					//for 2d faces
					if (L==2) 
					{
						bool bok=g->orderFace2d(C);
						XgeDebugAssert(bok);
					}

					//produce information about planes (only when DIM>=2, when DIM=1 the following condition never occur)
					if (L && L==(pdim1+pdim2-1))
					{
						Planef plane=g->getFittingPlane(C);
						memcpy(g->getGeometry(C,true),plane.mem,sizeof(float)*(pdim1+pdim2+1));
					}

					//double connectivity (only for dimension>0 and max dim cells)
					if (L && L==(pdim1+pdim2))
					{
						XgeDebugAssert(L1==pdim1 && L2==pdim2); //max dim cells

						//casi speciali quando L1==0 o L2==0 in quel caso non double linked!
						if (!L1 && !L2)
						{
							std::pair<unsigned int,unsigned int> V(C1,C2);
							XgeDebugAssert(zermap.find(V)!=zermap.end());
							g->addArch(C,zermap[V]);
						}
						else if (!L1)
						{
							XgeDebugAssert(L2);
							for (GraphIterator it2sub=g2->goUp(C2);!it2sub.end();it2sub++)
							{
								std::pair<unsigned int,unsigned int> V(C1,*it2sub);
								XgeDebugAssert(zermap.find(V)!=zermap.end());
								g->addArch(C,zermap[V]);
							}
						}
						else if (!L2)
						{
							XgeDebugAssert(L1);
							for (GraphIterator it1sub=g1->goUp(C1);!it1sub.end();it1sub++)
							{
								std::pair<unsigned int,unsigned int> V(*it1sub,C2);
								XgeDebugAssert(zermap.find(V)!=zermap.end());
								g->addArch(C,zermap[V]);
							}
						}
						else
						{
							XgeDebugAssert(L1 && L2);
							for (GraphIterator it1sub=g1->goUp(C1);!it1sub.end();it1sub++)
							for (GraphIterator it2sub=g2->goUp(C2);!it2sub.end();it2sub++)
							{
								std::pair<unsigned int,unsigned int> V(*it1sub,*it2sub);
								XgeDebugAssert(zermap.find(V)!=zermap.end());
								g->addArch(C,zermap[V]);
							}
						}
					}
				}
			}
		}
		
		if (!L) zermap=curmap;
		prvmap=curmap;
		curmap.clear();
	}

	//fix orientation of all boundary faces
	g->fixBoundaryFaceOrientation(0);

	#ifdef _DEBUG
	g->check();
	#endif

	//create the bigger matrix and take care of effects of embedding matrices for g1 and g2
	int sdim1=_g1vmat? (_g1vmat->dim) : (pdim1);
	int sdim2=_g2vmat? (_g2vmat->dim) : (pdim2);

	VmatT=Matf(sdim1+sdim2); Matf g1vmat(pdim1),g1hmat(pdim1); if (_g1vmat) {XgeDebugAssert(_g1vmat->dim>=pdim1);g1vmat=(*_g1vmat);g1hmat=(*_g1hmat);}
	HmatT=Matf(sdim1+sdim2); Matf g2vmat(pdim2),g2hmat(pdim2); if (_g2vmat) {XgeDebugAssert(_g2vmat->dim>=pdim2);g2vmat=(*_g2vmat);g2hmat=(*_g2hmat);}

	//copy matrix from g1 (first the submatrix without homo components then homo components)
	for (int r=1,R=1;R<=sdim1;r++,R++) 
	{
		for (int c=1,C=1;C<=sdim1;c++,C++) 
			{VmatT.set(r,c, g1vmat(R,C));HmatT.set(r,c,g1hmat(R,C));}
		VmatT.set(r,0,g1vmat(R,0));HmatT.set(r,0,g1hmat(R,0)); //translation
	}

	//copy matrix from g2 (first the submatrix without homo components then homo components)
	for (int r=sdim1+1,R=1;R<=sdim2;r++,R++)
	{
		for (int c=sdim1+1,C=1;C<=sdim2;c++,C++){VmatT.set(r,c,g2vmat(R,C)); HmatT.set(r,c,g2hmat(R,C));}
		VmatT.set(r,0, g2vmat(R,0));HmatT.set(r,0, g2hmat(R,0));  //translation
	}

	//embed by permutation matrix
	std::vector<int> permutation;
	permutation.push_back(0);//leave the homogeneous coordinate where it is
	for (int i=1      ;i<=pdim1;i++) permutation.push_back(i);      ; //the full space of g1
	for (int i=1      ;i<=pdim2;i++) permutation.push_back(sdim1+i); //the full space of g2
	for (int i=pdim1+1;i<=sdim1;i++) permutation.push_back(i)      ; //the embedding space of g1
	for (int i=pdim2+1;i<=sdim2;i++) permutation.push_back(sdim1+i); //the embedding space of g2=
	VmatT=VmatT.swapCols(permutation);
	HmatT=HmatT.swapRows(permutation);

	return g;
}




////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
Graph::SplitResult Graph::split(GraphNavigator& navigator,
				  const unsigned int cell,
				  const Planef& _h,
				  float start_tolerance,const int max_try,
				  unsigned int& Cb,unsigned int& Ca,unsigned int& Ce,
				  unsigned int Id)
{
	//in case of errors!
	static int nactivation=0;
	++nactivation;

	int pointdim  = this->pointdim;
	int celllvl   = Level(cell);

	int i;
	float acc;

	//is this necessary?
	Planef h=_h;
	h.normalize();

	/* FIRST STEP: classification */
	int ntry=0;

	for (float tolerance=start_tolerance;tolerance && ntry<max_try;tolerance/=10.0f,ntry++) 
	{
		int npos=0,nneg=0,nzer=0,npos_strict=0,nneg_strict=0;

		findCells(0,cell,navigator,true); //allow fast find

		const unsigned int& npoints=navigator.nnav[0];
		const unsigned int *points =navigator.nav [0];

		for (i=0;i<(int)npoints;i++)
		{
			unsigned int v=points[i];

			float* coords=getGeometry(v);
			acc=(float)h.getDistance(coords);

			//strict classification
			if (acc>=0)
			{
				++npos_strict;
			}
			else 
			{
				++nneg_strict;
			}

			//tolerance classification
			if (acc>tolerance)
			{
				++npos;
				getNode(v).Sign=SIGN_POS;
			}
			
			else if (acc<-tolerance) 
			{
				++nneg;
				getNode(v).Sign=SIGN_NEG;
			}

			else /* mi segno che e' un elemento preesistente */                    
			{
				++nzer;
				getNode(v).Sign=SIGN_ZER;
				NodeTmp(v)=0;
			}
		}
	
		// zero cell!
		if (nzer && !npos && !nneg)
		{
			Ca=Cb=Ce=0;

#if 1
			//even when if flat try to use the strict classification 
			if (npos_strict==(int)npoints)
			{
				XgeDebugAssert(!nneg_strict);
				Ca=cell;
				return SPLIT_OK;
			}

			if (nneg_strict==(int)npoints)
			{
				XgeDebugAssert(!npos_strict);
				Cb=cell;
				return SPLIT_OK;
			}
#endif
			
			return SPLIT_FLAT;
		}

		//positive
		else if (npos && !nneg)
		{
			Ca =cell;
			Cb=Ce=0;
			return SPLIT_OK;
		}

		//negative
		else if (nneg && !npos)
		{
			Cb=cell;
			Ca=Ce=0;
			return SPLIT_OK;
		}

		/* preparo per la navigazione a tutti i livelli (prima era il fast mode) */
		findCells(0,cell,navigator,false);


		/* uso il (celllvl+1)-nav per segnarmi le celle [=] create in caso di abort */
		unsigned int& nnewcells=navigator.nnav[celllvl+1];
		unsigned int* newcells =navigator.nav [celllvl+1];
		nnewcells=0;

		/* inizia il loop di verifica */
		for (int d=1;d<=celllvl;++d)
		{
			unsigned int& nnavd = navigator.nnav[d];
			unsigned int  *navd = navigator.nav [d];
		   int newnum=0;

			for (int i=0;i<(int)nnavd;++i)
			{
				/* classificazione in senso stretto, senza constraint */
				unsigned int face=navd[i];

				XgeDebugAssert(getNDw(face)>d && (d!=1 || getNDw(face)==2));

				if (d!=celllvl)
				{
					for (GraphIterator linkj=goUp(face);!linkj.end();linkj++)
						getNode(*linkj).Sign=SIGN_UNK;
				}

				int npos=0,nneg=0,nzer=0,ntosplit=0;
				for (GraphIterator linkj=goDw(face);!linkj.end();linkj++)
				{
					switch(getNode(*linkj).Sign)
					{
						case SIGN_POS:++npos;break;
						case SIGN_NEG:++nneg;break;
						case SIGN_ZER:++nzer;break; 
						case SIGN_BOT:
							++npos;
							++nneg;
							++ntosplit;
							break; 
					}
				}

				if (!npos) 
				{
					getNode(face).Sign=(!nneg)?SIGN_ZER:SIGN_NEG;
				}
				else  if (!nneg)
				{
					getNode(face).Sign=SIGN_POS;
				}
				else
				{
					if (nzer) /* caso npos, nneg, nzer. non lo posso permettere */
						goto ERROR_IN_CLASSIFICATION;

					getNode(face).Sign=SIGN_BOT;
					navd[newnum++]=face;
				}

				/* simula la creazione dell'intersezione per poi controllarla dopo! */
				XgeDebugAssert(getNode(face).Sign!=SIGN_UNK);

				if (getNode(face).Sign!=SIGN_BOT) 
					continue;
				
				/* ulteriore controllo (le due celle che verranno create devono essere piene) */
				XgeDebugAssert(npos && nneg && !nzer);

				if ((npos+1)<=d || (nneg+1)<=d)
					goto ERROR_IN_CLASSIFICATION;

				unsigned int mapface=addNode(Level(face)-1);
				getNode(mapface).Id=Id;

				getNode(mapface).Sign=SIGN_ZER;
				NodeTmp(face   )= mapface;
				NodeTmp(mapface)=face; /* mi segno che è un elemento new */

				/* mi segno la faccia in caso di abort */
				newcells[nnewcells++]=mapface;

				if (d==1) 
					continue;

				for (GraphIterator linkm=goDw(face);!linkm.end();linkm++)
				{
					unsigned int down=*linkm;
					XgeDebugAssert(getNode(down).Sign!=SIGN_ZER && getNode(down).Sign!=SIGN_UNK);

					switch (getNode(down).Sign)
					{
						case SIGN_POS:
						case SIGN_NEG:
						{
							for (GraphIterator linkn=goDw(down);!linkn.end();linkn++)
							{
								unsigned int downdown=*linkn;
								XgeDebugAssert(getNode(downdown).Sign==getNode(down).Sign || getNode(downdown).Sign==SIGN_ZER);

								if (getNode(downdown).Sign==SIGN_ZER && !findArch(downdown,mapface))
									addArch(downdown,mapface);
							}
							break;
						}

						case SIGN_BOT: /* deve avere per forza una cella [=] creata nuova */
							addArch(NodeTmp(down),mapface);
							break;
					}
				}
				
				
				if (
					    (getNDw(mapface)<=Level(mapface))          /* at least k-dimensional */
					 || (Level(mapface)==1 && getNDw(mapface)!=2)  /* not good since an edge must have two vertices */
					 || (Level(mapface)==2 && !orderFace2d(mapface)) /* not good since a 2-face must be an ordered polygon*/
				    ) 
					goto ERROR_IN_CLASSIFICATION;
			}
			
			nnavd=newnum;

			if (d==celllvl && nnavd!=1)
				goto ERROR_IN_CLASSIFICATION;
		}

		/* classification successfull (both sign) */	
		goto DO_SYMBOLIC_SPLIT;

ERROR_IN_CLASSIFICATION:

		/* devo togliere i link che non devono esserci */
		for (i=0;i<(int)nnewcells;++i)
		{
			unsigned int mapface=newcells[i];

			unsigned int A;
			for (;(A=getFirstUpArch(mapface));) remArch(A);
			for (;(A=getFirstDwArch(mapface));) remArch(A);
			remNode(mapface);
		}

		// i will try with a lower perturbation...

	} /* for (tolerance=....) */

	//cannot find a good classification
	Log::printf("                        Graph::split FAILED ntimes(%d) start_tolerance(%e)\n",ntry+1,start_tolerance);
	return SPLIT_IMPOSSIBLE;

DO_SYMBOLIC_SPLIT:

	// i tried with several tolerance
	if (ntry)
	{
		Log::printf("                        Graph::split SUCCEDED ntimes(%d) start_tolerance(%e)\n",ntry+1,start_tolerance);
	}

	/* create two new cells and connect them to vertices */
	Cb= addNode (celllvl); 
	Ca =addNode (celllvl); 

	if (celllvl==pointdim)
	{
		while (getNUp(cell))
		{
			unsigned int v=getFirstUpNode(cell);
			remArch(getFirstUpArch(cell));

			switch (getNode(v).Sign)
			{
				case SIGN_POS:addArch(Ca,v);break;
				case SIGN_NEG:addArch(Cb,v);break;
				case SIGN_ZER:addArch(Ca ,v);addArch(Cb,v);break;
				default:XgeDebugAssert(0);break;
			}			
		}
	}

	for (int k=1;k<=celllvl;++k)
	{		
		unsigned int* navk       =navigator.nav[k];
		const unsigned int& nnavk=navigator.nnav[k];

		for (i=0;i<(int)nnavk;++i)
		{
			unsigned int face=navk[i];

			XgeDebugAssert(getNode(face).Sign==SIGN_BOT);

			/* must split in two cells cell+ and cell-. create a k-1 cell which split the cell */
			unsigned int cminus = (k!=celllvl)?addNode(k):Cb ;getNode(cminus).Sign = SIGN_NEG;
			unsigned int cplus  = (k!=celllvl)?addNode(k):Ca  ;getNode(cplus ).Sign = SIGN_POS;

			getNode(cminus).Id=getNode(face).Id;
			getNode(cplus ).Id=getNode(face).Id;

			/* create the new = cell (which is a vertex for k=1) and connect to the new cells */
			unsigned int newface=NodeTmp(face);

			if (k==celllvl) 
				Ce=newface;

			XgeDebugAssert(getNode(newface).Sign==SIGN_ZER);

			/* plane information */
			if (k==(pointdim-1))
			{
				float* _c1=getGeometry(cminus,true);memcpy(_c1,this->getGeometry(face),this->db.itemsize());
				float* _c2=getGeometry(cplus ,true);memcpy(_c2,this->getGeometry(face),this->db.itemsize());
			}

			/* plane information */
			else if (k==pointdim)
			{
				float* gtmp=getGeometry(newface,true);
				memcpy(gtmp,h.mem,(pointdim+1)*sizeof(float));
			}

			/* it's a vertex... create the new coordinates */
			if (k==1)
			{
				// find the new point
				float* gtmp = getGeometry(newface,true);
				gtmp[0]=1;

				unsigned int p1   = getFirstDwNode(face);float* gv1=getGeometry(p1);
				unsigned int p2   = getLastDwNode (face);float* gv2=getGeometry(p2);

				float valuev1=h.getDistance(gv1);
				float valuev2=h.getDistance(gv2);

				if (valuev1<0) valuev1*=-1;
				if (valuev2<0) valuev2*=-1;

				float alpha =1/(valuev1+valuev2);
				float beta  =valuev1*alpha;
				alpha *=valuev2;

				switch (pointdim)
				{
					case 3:gtmp[3]=(float)(alpha*gv1[3]+beta*gv2[3]);
					case 2:gtmp[2]=(float)(alpha*gv1[2]+beta*gv2[2]);
					case 1:gtmp[1]=(float)(alpha*gv1[1]+beta*gv2[1]);break;
					default:
					{
						for (int j=1;j<=pointdim;++j) gtmp[j]=(float)(alpha*gv1[j]+beta*gv2[j]);
						break;
					}
				}

				/* add new new vertex to the new faces */ 
				if (celllvl==pointdim)
				{
					addArch(Ca,newface);
					addArch(Cb,newface);
				}

				/*
				importante: devo collegare il nuovo vertice a tutte le dim-celle che sono incidenti
				sull'edge.... siccome non vado in alto per stabilirle, passo dai vertici.
				Le dim-celle incidenti sull'edge sono tutte quelle che appartengono a
				INTERSEZIONE (dim-celle che incidono su P1,dim-celle che incidono su P2)
				*/
				for (GraphIterator linkkk=goDw(p1);!linkkk.end();linkkk++)
				for (GraphIterator linkhh=goDw(p2);!linkhh.end();linkhh++)
				{
					if (*linkkk==*linkhh)
					{
						addArch(*linkkk,newface);
						break;
					}	
				}
			}

			/* link to sub faces with the same sign. */
			bool linked=false;
			unsigned int Arch;
			while((Arch=getFirstDwArch(face)))
			{
				unsigned int archinfo=ArchData(Arch);
				unsigned int facedown=getN0(Arch);

				switch(getNode(facedown).Sign)
				{
					case SIGN_POS:ArchData(addArch(facedown,cplus ))=archinfo;break;
					case SIGN_NEG:ArchData(addArch(facedown,cminus))=archinfo;break;
					default:XgeDebugAssert(0);break;
				}

				/* voglio mantenere l'ordinamento per le 2-facce */
				if (k==2 && !linked)
				{
					unsigned int nextedge=getN0(getNextDwArch(Arch)); 
					unsigned char s1=Sign(getFirstDwNode(nextedge));
					unsigned char s2=Sign(getLastDwNode (nextedge));
					unsigned char s3=Sign(getFirstDwNode(facedown));
					unsigned char s4=Sign(getLastDwNode (facedown));

					if ((s3==SIGN_ZER && s1==SIGN_ZER) || (s3==SIGN_ZER && s2==SIGN_ZER) || (s4==SIGN_ZER && s1==SIGN_ZER) || (s4==SIGN_ZER && s2==SIGN_ZER))
					{
						addArch(newface,cplus );
						addArch(newface,cminus);
						linked=true;
					}
				}

				remArch(Arch);
			}

			XgeDebugAssert(k!=2 || linked);

			if (k!=2)
			{
				addArch(newface,cplus );
				addArch(newface,cminus);
			}

			/* link all up faces to cplus and cminus */
			if (celllvl!=pointdim || k<pointdim)
			{
				while (getNUp(face))
				{	
					unsigned int archup	 = getFirstUpArch(face);
					unsigned int faceup	 = getN1(archup);
					unsigned int archinfo = ArchData(archup);
					
					/* qui devo fare un inserimento opportuno nei links per mantenere ordinamento */
					unsigned int aup0,aup1;

					if (Level(faceup)==2) 
					{	
						unsigned int nextarch=getNextDwArch(getFirstUpArch(face));
						unsigned int nextface=getN0(nextarch);

						bool goodorder=findFirstCommonNode(cplus,nextface,DIRECTION_DOWN)!=0;
						aup0=addArch(goodorder?cminus:cplus ,faceup,DIRECTION_UP_AND_DOWN,0,nextarch);
						aup1=addArch(goodorder?cplus :cminus,faceup,DIRECTION_UP_AND_DOWN,0,nextarch);
					}
					else
					{
						aup0=addArch(cplus ,faceup);
						aup1=addArch(cminus,faceup);
					}

					ArchData(aup0)=archinfo;
					ArchData(aup1)=archinfo;
					remArch(getFirstUpArch(face));
				}
			}

			XgeDebugAssert(Level(newface)!=1 || getNDw(newface)==2);
			remNode(face);
		}
	}

	return SPLIT_OK;
}



///////////////////////////////////////////////////////////////////////////////////////////////
void Graph::transform(SmartPointer<Matf> vmat,SmartPointer<Matf> hmat)
{
	//only full dimension
	XgeDebugAssert(!vmat || vmat->dim==this->pointdim);
	XgeDebugAssert(!hmat || hmat->dim==this->pointdim);

	//useless call
	if (!vmat && !hmat)
		return ;

	if (vmat)
	{
		for (GraphListIterator i=each(0);!i.end();i++)
		{
			float* point=getGeometry(*i);
			Vecf v(pointdim,point);
			v = (*vmat) * v;
			XgeDebugAssert(v[0]);
			v/=v[0];
			v.mem[0]=1;
			memcpy(point,v.mem,sizeof(float)*(pointdim+1));
		}
	}

	if (hmat && pointdim>=2)
	{
		for (GraphListIterator i=each(pointdim-1);!i.end();i++)
		{
			if (NodeData(*i))
			{
				float* plane=getGeometry(*i);
				Planef h(pointdim,plane);
				h= h * (*hmat);
				h.normalize();
				memcpy(plane,h.mem,sizeof(float)*(pointdim+1));
			}
		}
	}

	//invalidate radius for bounding ball
	if (pointdim>=2)
	{
		for (GraphListIterator i=each(pointdim);!i.end();i++)
			if (NodeData(*i)) 
				getGeometry(*i)[0]=-1;
	}

	return ;
}

void Graph::translate(Vecf vt)
{
	XgeDebugAssert(vt[0]==0 && vt.dim==(this->pointdim));
	SmartPointer<Matf> vmat(new Matf(Matf::translateV(vt)));
	SmartPointer<Matf> hmat(new Matf(Matf::translateH(vt)));
	transform(vmat,hmat);
}


void Graph::scale(Vecf vs)
{	
	XgeDebugAssert(vs[0]==0 && vs.dim==(this->pointdim));
	SmartPointer<Matf> vmat(new Matf(Matf::scaleV(vs)));
	SmartPointer<Matf> hmat(new Matf(Matf::scaleH(vs)));
	transform(vmat,hmat);
}

void Graph::rotate(int ax1,int ax2,float alpha)
{
	XgeDebugAssert(pointdim>=2 && ax1>=1 && ax2>=1);
	SmartPointer<Matf> vmat(new Matf(Matf::rotateV(pointdim,ax1,ax2,alpha)));
	SmartPointer<Matf> hmat(new Matf(Matf::rotateH(pointdim,ax1,ax2,alpha)));
	transform(vmat,hmat);
}


void Graph::permutate(const std::vector<int>& P)
{
	int dim=(int)(P.size()-1);
	XgeDebugAssert(dim==(this->pointdim));
	//convert to a good transformation matrix
	std::vector<int> perm(dim+1);
	for (int i=0;i<=dim;i++) perm[P[i]]=i;
	SmartPointer<Matf> vmat(new Matf(Matf(dim).swapCols(perm)));
	SmartPointer<Matf> hmat(new Matf(Matf(dim).swapRows(perm)));
	XgeDebugAssert(((*vmat)*(*hmat)).almostIdentity());
	transform(vmat,hmat);
}




///////////////////////////////////////////////////////////////////////////
void Graph::triangulate(unsigned int N)
{
	//triangulate all cells
	if (!N)
	{
		for (int lvl=2;lvl<=pointdim;lvl++)
		{
			for (GraphListIterator it=each(lvl),next;!it.end();it=next)
			{
				next=it.next();
				unsigned int C=*it;
				triangulate(C);
			}
		}
		return ;
	}


	GraphIterator i;
	
	unsigned char celllvl=getNode(N).Level;

	//not necessary for points and edges
	if (celllvl<=1)
		return ;

	//not necessary it is already a simplex (example for celllvl==2 it has already three edges)
	if (getNDw(N)==(celllvl+1))
		return ;

	GraphNavigator navigator;

	//in celllvl+1 I store new faces created
	unsigned int* newfaces  =navigator.nav [celllvl+1];
	unsigned int& nnewfaces =navigator.nnav[celllvl+1];
	nnewfaces=0;

	//special case in 2Dim (do not add point but make a smart visit)
	if (celllvl==2)
	{
		unsigned int F=N;
		unsigned int T[2],nt=0;

		unsigned int Prev0=getFirstDwNode(getLastDwNode(F)),P1;
		unsigned int Prev1=getLastDwNode (getLastDwNode(F)),P2;
		unsigned int FirstNode=0;

		for (GraphIterator i=goDw(F);!i.end();i++,Prev0=P1,Prev1=P2)
		{
			unsigned int E=*i;
			P1=getFirstDwNode(E);
			P2=getLastDwNode (E);
			if (P1!=Prev0 && P1!=Prev1) Swap(unsigned int,P1,P2);
			if (!FirstNode) FirstNode=P1;

			T[nt++]=E;

			if (nt==2)
			{
				unsigned int NewFace=addNode(2);
				newfaces[nnewfaces++]=NewFace;

				if (pointdim!=2)
				{
					for (GraphIterator jt=goUp(F);!jt.end();jt++)
						addArch(NewFace,*jt);
				}
				
				addArch(T[0],NewFace);
				addArch(T[1],NewFace);

				unsigned int ToFirstEdge;
				
				if (i.next().next().end()) 
				{
					ToFirstEdge=*i.next();
					addArch(ToFirstEdge,NewFace);
					break;
				}
				
				ToFirstEdge=addNode(1);
				addArch(FirstNode,ToFirstEdge);
				addArch(P2       ,ToFirstEdge);

				addArch(ToFirstEdge,NewFace);
				nt=0;
				T[nt++]=ToFirstEdge;
			}
		}
	}
	else
	{
		//general case, add a centroid node (slow mode since I need all cells at all levels)
		findCells(0,N,navigator,false);

		unsigned int* points=navigator.nav[0];
		const unsigned int& npoints=navigator.nnav[0];

		unsigned int Centroid=addNode(0);
		float* vg=this->getGeometry(Centroid,true);

		float coeff=(float)(1.0/npoints);

		for (unsigned int cont=0;cont<npoints;cont++)
		{
			float* gtmp=getGeometry(points[cont]);
			for (int n=1;n<=pointdim;n++) vg[n]+=coeff*gtmp[n];
		}

		for (int k=0;k<celllvl;k++)
		{
			unsigned int* cells=navigator.nav[k];
			const unsigned int& ncells=navigator.nnav[k];

			for (unsigned int cont=0;cont<ncells;cont++)
			{
				unsigned int S   =cells[cont];
				unsigned int Ckup=addNode(k+1);

				//new celllvl-cells
				if (k==(celllvl-1)) 
					newfaces[nnewfaces++]=Ckup;

				if (celllvl!=pointdim && (k+1)==celllvl)
				{
					for (i=goUp(N);!i.end();i++)
						addArch(Ckup,*i);
				}

				NodeTmp(S)=Ckup;
				addArch(S,Ckup);

				if (!k)
				{
					addArch(Centroid,Ckup);
				}
				else
				{
					for (i=goDw(S);!i.end();i++)
						addArch(NodeTmp(*i),Ckup);
				}

				//DO NOT STORE plane equation because they probably are useless since they  are internal faces
			}
		}
	}

	remNode(N);

	//link simplex to points
	if (pointdim>=2 && celllvl==pointdim)
	{
		for (unsigned int cont=0;cont<nnewfaces;cont++)
		{
			unsigned int NewCell = newfaces[cont];
			XgeDebugAssert(Level(NewCell)==celllvl && getNUp(NewCell)==0);
			findCells(0,NewCell,navigator,false); //cannot do fast mode since I need here to make the list double linked 
			unsigned int npoints=navigator.nnav[0];
			unsigned int *points=navigator.nav[0];
			for (int n=0;n<(int)npoints;n++) 
				addArch(NewCell,points[n]);
			
			XgeDebugAssert(getNUp(NewCell)==(celllvl+1) && getNDw(NewCell)==(celllvl+1));
		}
	}
}







/////////////////////////////////////////////
void GraphKMem::PrintStatistics()
{
	Log::printf("n=%d m=%d (%d%%) single=%d bytes tot=%d kb\n",
		(int)m_num-1,(int)m_max,(int)(m_num*100.f)/m_max,(int)m_itemsize,(int)(m_itemsize*m_max/1024.0f));
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOTE: the links goes downward from face to vertices
// bidirectional link exists only for boundary faces (== pointdim-1 faces) at least at run time of qhull
// returns a new created cell (or 0 if not found) which down link to vertices of the closure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline unsigned int mkpol_find_closure(SmartPointer<Graph> g,unsigned int face)
{
	//typedef std::vector<unsigned int, boost::pool_allocator<unsigned int> > StdVector;
	typedef std::vector<unsigned int> StdVector;
	StdVector fclosure;

	int nverts=g->getNDw(face);

	// set zero the counter to all faces reached by vertices of potential_face
	for (GraphIterator st=g->goDw(face);!st.end();st++)
	{
		for(GraphIterator it=g->goUp(*st);!it.end();it++) 
			g->NodeTmp(*it)=0;
	}

	// increment counter, if the counter for a face is equal the number of vertices this means the face contains all vertices
	for (GraphIterator st=g->goDw(face);!st.end();st++)
	{
		for(GraphIterator it=g->goUp(*st);!it.end();it++)
			if (++(g->NodeTmp(*it))==nverts) 
				fclosure.push_back(*it);
	}

	// set zero the counter of all vertices reached by faces of fclosure
	for (StdVector::iterator st=fclosure.begin();st!=fclosure.end();st++)
	{
		for(GraphIterator it=g->goDw(*st);!it.end();it++)  
			g->NodeTmp(*it)=0;
	}

	unsigned int vclosure=g->addNode(1);

	// increment counter, if the counter for a vertex is equal the number of faces this means the vertex is referenced by all faces
	for (StdVector::iterator st=fclosure.begin();st!=fclosure.end();st++)
	{
		for(GraphIterator it=g->goDw(*st);!it.end();it++)
			if (++(g->NodeTmp(*it))==fclosure.size()) 
				g->addArch(*it,vclosure,Graph::DIRECTION_DOWN);
	}

	//not found
	if (!g->getNDw(vclosure))
	{
		g->remNode(vclosure);
		vclosure=0;
	}

	return vclosure;

}


////////////////////////////////////////////////////////////////////////////////
static SmartPointer<Graph> mkpol_inner(Matf& VmatT,Matf& HmatT,int pointdim,int npoints,const float* points,float tolerance,int nrec)
{
  int error_qhull;

	//base case, 0 dimension
	if (!pointdim)
	{
		SmartPointer<Graph> g(new Graph(0));
		Vecf v(0);v.mem[0]=1.0f;
		g->addVertex(v);
		VmatT=Matf(0); //identity matrices
		HmatT=Matf(0);
		return g;
	}

	//other cases, cannot be a full cell!
	if (npoints<(pointdim+1))
		goto FAILED_IN_THIS_DIMENSION;

	//base case, 1 dimension
	if (pointdim==1)
	{
		float m=points[0],M=points[0];
		for (int i=0;i<npoints;i++){m=min2(m,points[i]);M=max2(M,points[i]);}

		//minimum and maxima are the same
		if (m==M)
			goto FAILED_IN_THIS_DIMENSION;
  
		SmartPointer<Graph>  g(new Graph(1));
		Vecf v1(1);v1.mem[0]=1.0f;v1.mem[1]=m;unsigned int V0=g->addVertex(v1);
		Vecf v2(1);v2.mem[0]=1.0f;v2.mem[1]=M;unsigned int V1=g->addVertex(v2);
		unsigned int E=g->addNode(1);
		g->addArch(V0,E);g->addArch(E,V0);
		g->addArch(V1,E);g->addArch(E,V1);
		VmatT=Matf(1);//identity matrices
		HmatT=Matf(1);
		return g;
	}

	//to avoid warnings from qhull
	#if PYPLASM_WINDOWS
	static FILE* __devnull=fopen("nul","w");
	#else
	static FILE* __devnull=fopen("/dev/null","w");
	#endif

	//*** TODO: is possible to handle more hulls for the same set of points?
	//*** to get the qhull original point you can use qh_pointid
	//*** the problems seems in the algorithm of the paper 

	//OPTIONS: i==vertices incidence Pp=does not print warnings, n=print hyperplane normals with offsets, En max roundoff error, Qs search all points for initial simplex,QbB scale input to unit cube centered at the origin
	char qhull_opts[256];sprintf(qhull_opts,"qhull i Pp E%e Qs",tolerance);
	error_qhull=qh_new_qhull(pointdim,npoints,(float*)points,(boolT)false,qhull_opts, NULL, __devnull);

	if (error_qhull)	
	{
		qh_freeqhull(qh_ALL);

		FAILED_IN_THIS_DIMENSION:
		
		//SINCE IT HAS FAILED I NEED TO PROJECT POINTS:
		const int Dim=pointdim; 
		Matf T=Matf::getProjectionMatrix(pointdim,npoints,points);

		//the vmat in ggraph is the inverted matrix
		Matf vmat=T.invert(); 

		//only for the first time I need to make a copy of points to change them, otherwise
		//i work on the already created memory
		int projected_points_size=sizeof(float)*npoints*(pointdim-1);
		float* projected_points=(!nrec)?(float*)MemPool::getSingleton()->malloc(projected_points_size):(float*)points;

		// project points for the next level!
		const float* cursor_src=points;
		float *cursor_dst=projected_points;

		for (int it=0;it<npoints;it++,cursor_src+=pointdim,cursor_dst+=(pointdim-1))
		{
			Vecf vg= T * Vecf(Dim,1,cursor_src);

			//make sure the last coordinates gets cancelled (== points are in plane xn=0)
			XgeDebugAssert(fabs(vg[Dim])<1e-4 && vg[0]); 
			if (vg[0]!=1) {vg/=vg[0];vg.mem[0]=1;}
			memcpy(cursor_dst,&vg.mem[1],sizeof(float)*(pointdim-1)); //exclude the homogeneous coordinate
		}	

		//try in lower dimension
		SmartPointer<Graph> g=mkpol_inner(VmatT,HmatT,pointdim-1,npoints,projected_points,tolerance,nrec+1);
		XgeDebugAssert(g);

		XgeDebugAssert(VmatT.dim==HmatT.dim && VmatT.dim==(pointdim-1));

		//apply the projection
		VmatT=vmat * VmatT.extract(pointdim);
		HmatT=VmatT.invert();

		//need to dealloc
		if (!nrec)
			MemPool::getSingleton()->free(projected_points_size,projected_points);

		return g;
	}

	//initialize matrices, in this dimension is ok!
	VmatT=Matf(pointdim);
	HmatT=Matf(pointdim);

	//loop through the faces and build vertex/facet incidence matrix
	SmartPointer<Graph> g(new Graph(pointdim)); 

	//build vertices
	int num_boundary_vertices=qh num_vertices;
	unsigned int first_boundary_vertex=0;
	unsigned int last_boundary_vertex=0;
	std::map<unsigned int,unsigned int> mapv;


	
	int nv=0;
	vertexT *vertex;FORALLvertices
	{
		Vecf point(pointdim,1.0f,vertex->point);

		unsigned int V=g->addVertex(point);
		if (!first_boundary_vertex) first_boundary_vertex=V;
		last_boundary_vertex=V;
		XgeDebugAssert(V==++nv); //it is necessary for keeping the stability between g and g (both ggraph)
		mapv[vertex->id]=V;
	}
	XgeDebugAssert(first_boundary_vertex && last_boundary_vertex && last_boundary_vertex>first_boundary_vertex);

	//build faces
	int num_boundary_faces=qh num_facets;
	unsigned int first_boundary_face=0;
	unsigned int last_boundary_face =0;
	facetT* facet;FORALLfacets 
	{
		unsigned int F=g->addPlane(Vecf(pointdim,facet->offset,facet->normal));

		if (!first_boundary_face) first_boundary_face=F;
		last_boundary_face=F;

		vertexT *vertex, **vertexp;
		FOREACHvertex_(facet->vertices) 
		{
			unsigned int V=mapv[vertex->id];
			g->addArch(V,F,Graph::DIRECTION_UP_AND_DOWN); //this link should be bidirectional!
		}
	}
	XgeDebugAssert(first_boundary_face && last_boundary_face && last_boundary_face>first_boundary_face);

	//do not need anymore
	qh_freeqhull(qh_ALL);

	// be a little smart when you have all!
	if (pointdim==2)
	{
		unsigned int Up=g->addNode(pointdim);

		for (GraphListIterator it=g->each(pointdim-1);!it.end();it++)
			g->addArch(*it,Up);

		for (GraphListIterator it=g->each(0);!it.end();it++)
			g->addArch(Up,*it);

		bool bOk=g->orderFace2d(Up);
		XgeDebugAssert(bOk);
		return g;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//see "Computing the face lattice of a polytone from its vertex-facet indices", Kaibel and Pfetsch
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////

	//could i optimize for 3 dim? probably yes
	//............................

	//the stack
	std::stack< std::pair<unsigned int,unsigned int> > Q;

	//faces created in the hasse diagram, this seems to be the fastest way to find a face
	

	//typedef std::set<unsigned int,std::less<unsigned int> , boost::fast_pool_allocator<unsigned int>  > stl_face_t;
	typedef std::set<unsigned int> stl_face_t;


	stl_face_t stl_face;
	std::map<stl_face_t ,unsigned int> stl_face_created; 
	std::map<stl_face_t ,unsigned int>::iterator stl_face_finder;

	// in the incidence matrix I will store all faces and their links to node
	int num_closure_faces=0;
	for (GraphListIterator it=g->each(0);!it.end();it++)
	{
		unsigned int V=*it;
		unsigned int vv=g->addNode(1);
		g->addArch(V,vv,Graph::DIRECTION_DOWN); //only direction down to not slow down the search for the closure
		Q.push(std::pair<unsigned int,unsigned int>(V,vv));
		++num_closure_faces;
	}

	//start the loop
	while (Q.size())
	{
		std::pair<unsigned int,unsigned int> item=Q.top();
		Q.pop();

		unsigned int     N=item.first ; //hasse node
		unsigned int  face=item.second; //the set of vertices
		XgeDebugAssert(N);

		// labels
		const int FAILED   =0;
		const int CANDIDATE=1;
		const int MINIMAL  =2;

		// from V -> (face in g,label)
		std::map<unsigned int,std::pair<unsigned int,int> > closures;

		//for all vertices
		for (GraphListIterator it=g->each(0);!it.end();it++)
		{
			unsigned int V=*it; //the potentially vertex to add

			// the face does already contain the vertex
			if (g->findArch(V,face,Graph::DIRECTION_DOWN))
				continue;
			
			//add the potential vertex
			g->addArch(V,face,Graph::DIRECTION_DOWN);
			unsigned int vclosure=mkpol_find_closure(g,face);
			g->remArch(V,face,Graph::DIRECTION_DOWN);

			//not found a good vclosure, cannot be a new face
			if (!vclosure) 
				continue;

			++num_closure_faces;

			//a possible closure
			closures [V] = std::pair<unsigned int,int>(vclosure,CANDIDATE);
		}

		//find minimal set 
		for (std::map<unsigned int,std::pair<unsigned int,int> >::iterator IT=closures.begin();IT!=closures.end();IT++)
		{
			unsigned int        V = IT->first;
			unsigned int vclosure = IT->second.first;
			int&    label   = IT->second.second;

			XgeDebugAssert(label==CANDIDATE);

			for (GraphIterator JT=g->goDw(vclosure);!JT.end();JT++)
			{
				unsigned int W=*JT;

				if (W==V) 
					continue;

				//in the current set!
				if (closures.find(W)!=closures.end() && (closures[W].second==MINIMAL || closures[W].second==CANDIDATE))
				{
					label=FAILED;
					break;
				}
			}

			//not erased the label, from candidate->minimal
			if (label!=FAILED) 
			{
				XgeDebugAssert(label==CANDIDATE);
				label=MINIMAL; 
			}
		}
		//deallocate all vclosure failed
		for (std::map<unsigned int,std::pair<unsigned int,int> >::iterator IT=closures.begin();IT!=closures.end();IT++)
		{
			unsigned int V        =IT->first;
			unsigned int vclosure =IT->second.first;
			int    label   = IT->second.second;

			if (label==MINIMAL)
			{
				// convert to something STL can handle, here I can use the algorithm in the paper which to me seems slower (but didn't try)
				stl_face.clear();
				for (GraphIterator _it=g->goDw(vclosure);!_it.end();_it++) stl_face.insert(*_it);
				stl_face_finder=stl_face_created.find(stl_face);

				unsigned int L; 

				if (stl_face_finder==stl_face_created.end())
				{
					int lvl=g->Level(N)+1;
					L=g->addNode(lvl);
					stl_face_created[stl_face]=L; //add to the face created
					Q.push(std::pair<unsigned int,unsigned int>(L,vclosure));

					//should copy the plane geometry
					if (lvl==(pointdim-1))
					{
						int nverts=g->getNDw(vclosure);

						// should be only one face which contains all the vertices of vclosure
						for (GraphIterator st=g->goDw(vclosure);!st.end();st++)
							for(GraphIterator it=g->goUp(*st);!it.end();it++) g->NodeTmp(*it)=0;

						// increment counter, if the counter for a face is equal the number of vertices this means the face contains all vertices
						for (GraphIterator st=g->goDw(vclosure);!st.end() && !g->NodeData(L);st++)
						{
							for(GraphIterator it=g->goUp(*st);!it.end();it++)
							{
								//found it, copy from the face
								if (++(g->NodeTmp(*it))==nverts) 
								{
									memcpy(g->getGeometry(L,true),g->getGeometry(*it), sizeof(float)*(g->getPointDim()+1));
									break;
								}
							}
						}
						//must have find it
						XgeDebugAssert(g->NodeData(L));
					}
				}
				else
				{
					L=stl_face_finder->second;

					//don't need since it yet exists
					g->remNode(vclosure);
					--num_closure_faces;
				}

				//anyway add the arch (only direction down to now slow down the search for a closure)
				g->addArch(N,L,Graph::DIRECTION_DOWN);
			}
			else if (label==FAILED)
			{
				//dealloc the node
				g->remNode(vclosure);
				--num_closure_faces;
			}
			else
			{
				Utils::Error(HERE,"internal error in mkpol, label cannot be candidate anymore!");
			}
		}

		//done with this face, remove the cell from incidence matrix
		g->remNode(face);
		--num_closure_faces;
	}

	//safety check
	XgeDebugAssert(!num_closure_faces);

	//remove faces boundary created in the first stage
	for (unsigned int F=first_boundary_face;F<=last_boundary_face;F++)
		g->remNode(F,false); //not recursive otherwise vertices will be deallocated

	//complete with the only full cell
	unsigned int Up=g->addNode(pointdim);
	for (GraphListIterator it=g->each(pointdim-1);!it.end();it++)
		g->addArch(*it,Up,Graph::DIRECTION_DOWN);

	//complete one-directional links
	for (int lvl=1;lvl<=pointdim;lvl++)
	{
		for (GraphListIterator it=g->each(lvl);!it.end();it++)
		{
			for (GraphIterator jt=g->goDw(*it);!jt.end();jt++)
				g->addArchDirection(jt.getArch(),Graph::DIRECTION_UP);

			//order 2 faces
			if (lvl==2)
			{
				bool bOk=g->orderFace2d(*it);
				XgeDebugAssert(bOk);
			}
		}
	}

	//double link full cell <-> vertices
	for (GraphListIterator it=g->each(0);!it.end();it++)
		g->addArch(Up,*it);

	return g;
}

SmartPointer<Graph> Graph::mkpol(Matf& Vmat,Matf& Hmat,int pointdim,int npoints,std::vector<float> points,float tolerance)
{
	XgeDebugAssert((int)points.size()==pointdim*npoints);
	return mkpol(Vmat,Hmat,pointdim,npoints,&points[0],tolerance);
}

SmartPointer<Graph> Graph::mkpol(Matf& VmatT,Matf& HmatT,int pointdim,int npoints,const float* __points,float tolerance)
{
	//a rescale in the range [-1,+1] is needed to obtain better precision (and to avoid deadlock in qhull)
	float* p;
	float* points=(float*)MemPool::getSingleton()->malloc(sizeof(float)*pointdim*npoints);
	memcpy(points,__points,sizeof(float)*pointdim*npoints);

	Vecf m(pointdim);m.set(+FLT_MAX);m.mem[0]=1.0f;
	Vecf M(pointdim);M.set(-FLT_MAX);M.mem[0]=1.0f;

	p=points;
	for (int P=0;P <npoints ;P++)
	for (int D=1;D<=pointdim;D++,p++)
	{		
		m.mem[D]=min2(m[D],*p);
		M.mem[D]=max2(M[D],*p);
	}

	Vecf translate(pointdim);
	Vecf scale    (pointdim);

	for (int D=1;D<=pointdim;D++,p++)
	{
		translate.mem[D]= -0.5f*(m[D]+M[D]);
		scale    .mem[D]= (M[D]-m[D])>0?(2.0f/(M[D]-m[D])):1.0f;
	}

	p=points;
	for (int P=0;P<npoints  ;P++)
	for (int D=1;D<=pointdim;D++,p++)	
		(*p)=((*p)+translate[D]) * scale[D];

	//static int __cont=0;
	//Log::printf("Cont %d\n",__cont);
	//__cont++;

	SmartPointer<Graph> ret= mkpol_inner(VmatT,HmatT,pointdim,npoints,points,tolerance,0);

	MemPool::getSingleton()->free(sizeof(float)*pointdim*npoints,points);

	//apply inverse transformation of the rescaling
	VmatT = Matf::translateH(translate) * Matf::scaleH(scale) * VmatT;
	HmatT = HmatT * Matf::scaleV(scale) * Matf::translateV(translate);

	return ret;

}


////////////////////////////////////////////////////////////////////////////////
std::vector< std::vector<int> > Graph::qhull(int pointdim,
	                                          const std::vector<float>& pointdb,
															const std::vector<int  >& indices,
															float tolerance,
															bool bVerbose)
{
	bool BOK=true;
	std::vector< std::vector<int> > ret;

	#if PYPLASM_WINDOWS
	static FILE* __devnull=fopen("nul","w");
	#else
	static FILE* __devnull=fopen("/dev/null","w");
	#endif

	if (bVerbose) printf("Doing qhull...\n");

	//compact points
	std::vector<float> points;
	int npoints=(int)indices.size();
	points.resize(npoints*pointdim);
	for (int i=0;i<npoints;i++)
	{
		int idx=indices[i]*pointdim;

		if (!(idx>=0 && (idx+pointdim)<=(int)pointdb.size()))
		{
			printf("   PROBLEM: index %d is out of valid range",indices[i]);
			ret.clear();
			return ret;
		}
		memcpy(&points[i*pointdim],&pointdb[indices[i]*pointdim],pointdim*sizeof(float));
	}

	//OPTIONS: 
	//   i==vertices incidence 
	//   Pp=does not print warnings
	//   n=print hyperplane normals with offsets
	//   En max roundoff error, 
	//   Qs search all points for initial simplex
	//   QbB scale input to unit cube centered at the origin
	char qhull_opts[256];
	sprintf(qhull_opts,"qhull i Pp E%e Qs",tolerance);
	int error_qhull=qh_new_qhull(pointdim,npoints,&points[0],(boolT)false,qhull_opts, NULL, __devnull);

	#define POINTID(vertex) \
		((qh_pointid(vertex->point)>=0)? (indices[qh_pointid(vertex->point)]) : (qh_pointid(vertex->point)))

	//success
	if (!error_qhull)	
	{
		//verbose print points
		if (bVerbose)
		{
			vertexT *vertex;
			FORALLvertices
			{
				int ID=POINTID(vertex);
				Vecf coords(pointdim,1.0f,vertex->point);
				bool VertexOk=(ID>=0 && ID<((int)pointdb.size()/pointdim)) && memcmp(&pointdb[0]+ID*pointdim,vertex->point,sizeof(float)*pointdim)==0;
				printf("   Point %s id(%3d) coords(%s)\n",VertexOk?"OK":"ERROR",ID,coords.str().c_str());
				BOK=BOK && VertexOk;
			}
		}
	
		//build faces
		facetT* facet;
		FORALLfacets 
		{
			std::vector<int> f;
			bool FaceOk=true;
			vertexT *vertex,**vertexp;
			FOREACHvertex_(facet->vertices) 
			{
				int ID=POINTID(vertex);
				f.push_back(ID);
				FaceOk=FaceOk && (ID>=0 && ID<((int)pointdb.size()/pointdim));
				BOK=BOK && FaceOk;
			}
			ret.push_back(f);

			if (bVerbose) 
			{
				printf("   Face  %s vertices[",FaceOk?"OK":"ERROR");
				for (int i=0;i<(int)f.size();i++) printf("%d ",f[i]);
				printf("] plane(%s)\n",Vecf(pointdim,facet->offset,facet->normal).str().c_str());
			}
		}
	}

	qh_freeqhull(qh_ALL);
	if (!BOK) ret.clear();
	if (bVerbose) printf("...done qhull %s\n",BOK?"OK":"ERROR");
	return ret;
}


#if 0

////////////////////////////////////////////////////////////////////////////////
static SmartPointer<Graph> mkpolf_inner(Matf& VmatT,Matf& HmatT,int pointdim,int nplanes,const float* _planes,float* interior_point,float tolerance,int nrec)
{
	int error_qhull;

	//base case, 0 dimension
	if (!pointdim)
	{
		SmartPointer<Graph> g(new Graph(0));
		Vecf v(0);v.mem[0]=1.0f;
		g->addVertex(v);
		VmatT=Matf(0); //identity matrices
		HmatT=Matf(0);
		return g;
	}

	//other cases, cannot be a full cell!
	if (nplanes<(pointdim+1))
		return SmartPointer<Graph>();

	//QHULL need homo in last position and correct orientation
    int planes_size=sizeof(float)*(pointdim+1)*nplanes;
	float* planes=(float*)MemPool::getSingleton()->malloc(planes_size);
	{
		Vecf InteriorP(pointdim,1.0f,interior_point);
		const float* src=_planes;
		float*       dst= planes;
		for (int P=0;P<nplanes;P++,src+=(pointdim+1),dst+=(pointdim+1))
		{	
			//qhull needs plane with correct orientation
			Planef h(pointdim,src);
			h=h.forceBelow(InteriorP);

			memcpy(&dst[0],&h.mem[1],sizeof(float)*pointdim);
			dst[pointdim]=h.mem[0]; //homo in last position
		}
	}

	//base case, 1 dimension (not supported, todo!)
	if (pointdim==1)
	{
		MemPool::getSingleton()->free(planes_size,planes);
		return SmartPointer<Graph>();
	}

	//to avoid warnings from qhull
	#if PYPLASM_WINDOWS
	static FILE* __devnull=fopen("nul","w");
	#else
	static FILE* __devnull=fopen("/dev/null","w");
	#endif

	if (__devnull==0) __devnull=stdout;

	//OPTIONS: H==qhalf  Fp==intersection coordinates Pp=does not print warnings, En max roundoff error 
	char qhull_opts[256];sprintf(qhull_opts,"qhull H Fp Pp E%e ",tolerance);

	//interior point
	strcat(qhull_opts," H");
	for (int N=0;N<pointdim;N++) sprintf(qhull_opts+strlen(qhull_opts),"%.e%s",interior_point[N],N<(pointdim-1)?",":"");

	error_qhull=qh_new_qhull(pointdim+1,nplanes,(float*)planes,(boolT)false,qhull_opts, __devnull, __devnull);

	if (error_qhull)	
	{
		qh_freeqhull(qh_ALL);
		MemPool::getSingleton()->free(planes_size,planes);
		return SmartPointer<Graph>();
	}

	//initialize matrices, in this dimension is ok!
	VmatT=Matf(pointdim);
	HmatT=Matf(pointdim);

	//loop through the faces and build vertex/facet incidence matrix
	SmartPointer<Graph> g(new Graph(pointdim)); 

	//build vertices
	int num_boundary_vertices=qh num_vertices;
	int num_boundary_faces   =qh num_facets;
	unsigned int first_boundary_vertex=0,last_boundary_vertex=0;
	unsigned int first_boundary_face  =0,last_boundary_face  =0;
	std::map<unsigned int,unsigned int> mapv; //map for vertices
	std::map<unsigned int,unsigned int> mapf; //map for faces

	//add vertices
	{
		int nv=0;
		facetT* facet;FORALLfacets  //NOTE: in qhull they are listed in FORALLfacets
		{
			Vecf point(pointdim,1.0f,facet->normal);

			unsigned int V=g->addVertex(point);
			if (!first_boundary_vertex) first_boundary_vertex=V;
			last_boundary_vertex=V;
			XgeDebugAssert(V==++nv); //it is necessary for keeping the stability between g and g (both ggraph)
			mapv[facet->id]=V;
		}
		XgeDebugAssert(first_boundary_vertex && last_boundary_vertex && last_boundary_vertex>first_boundary_vertex);
	}


	//add faces
	{
		vertexT *vertex;FORALLvertices //NOTE: in qhull they are listed in FORALLvertices
		{
			unsigned int F=g->addNode(pointdim-1); //I do not add plane equation, QHULL return only the the normal! not the offset
			if (!first_boundary_face) first_boundary_face=F;
			last_boundary_face=F;
			mapf[vertex->id]=F;
		}
		XgeDebugAssert(first_boundary_face && last_boundary_face && last_boundary_face>first_boundary_face);
	}

	//add connection point-faces
	{
		facetT* facet;FORALLfacets  //NOTE: these are the vertices
		{
			vertexT *vertex, **vertexp; FOREACHvertex_(facet->vertices)  //NOTE: these are the faces
			{
				unsigned int V=mapv[facet->id ];
				unsigned int F=mapf[vertex->id];
				g->addArch(V,F,Graph::DIRECTION_UP_AND_DOWN); //this link should be bidirectional!
			}
		}
	}

	//do not need anymore
	qh_freeqhull(qh_ALL);

	//full lattice representation
	setFullLattice(g,first_boundary_face,last_boundary_face);

	//necessary since I do not have the offsets for planes
	g->fixBoundaryFaceOrientation(0);

	MemPool::getSingleton()->free(planes_size,planes);
	return g;
}

SmartPointer<Graph> Graph::mkpolf(Matf& VmatT,Matf& HmatT,int pointdim,int nplanes,const float* planes,float* interior_point,float tolerance)
{
	return mkpolf_inner(VmatT,HmatT,pointdim,nplanes,planes,interior_point,tolerance,0);
}

SmartPointer<Graph> Graph::mkpolf(Matf& Vmat,Matf& Hmat,int pointdim,int nplanes,std::vector<float> planes,std::vector<float> interior_point,float tolerance)
{
	XgeDebugAssert((int)planes.size()==((pointdim+1)*nplanes) && (int)interior_point.size()==pointdim);
	return mkpolf_inner(Vmat,Hmat,pointdim,nplanes,&planes[0],&interior_point[0],tolerance,0);
}


#endif