
#include <xge/xge.h>
#include <xge/plasm.h>
#include <xge/clock.h>
#include <xge/octree.h>




extern "C" unsigned long xge_total_hpc=0;


Plasm::Statistic Plasm::stats[2048];
std::stack<int> Plasm::current_ops;
GraphNavigator Plasm::navigator;


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void Plasm::statistics(bool bReset)
{
	static char* function_names[]=
	{
		"destroy",
		"cube","simplex","mkpol","mkpolf",
		"struct",
		"copy",
		"getspacedim","getpointdim",
		"transform","scale","translate","rotate","embed",
		"limits",
		"join","power","boolop",
		"skeleton",
		"ukpol","ukpolf",
		"addproperty","getproperty",
		"texture0",
		"shrink",
		"__END"
	};

	int global_msec=0;
	int global_ncalls=0;
	int local_msec=0;
	int local_ncalls=0;


	Log::printf("# of hpcs    %d\n",(int)xge_total_hpc);
	Log::printf("total memory %d\n",(int)xge_total_memory);

	for (int i=0;i<PLASM___END;i++)
	{
		if (stats[i].local_msec || stats[i].global_msec || stats[i].local_ncalls || stats[i].global_ncalls)
		{
			local_msec   +=stats[i].local_msec;
			global_msec  +=stats[i].global_msec;
			local_ncalls +=stats[i].local_ncalls;
			global_ncalls+=stats[i].global_ncalls;
			Log::printf("[%32s] %6d/%6d msec %6d/%6d ncalls\n",function_names[i],stats[i].local_msec,stats[i].global_msec,stats[i].local_ncalls,stats[i].global_ncalls);
		}
	
		if (bReset)
		{
			stats[i].local_msec  =0;
			stats[i].local_ncalls=0;
		}
	}

	Log::printf("[%32s] \n","---------------------");
	Log::printf("[%32s] %6d/%6d msec %6d/%6d ncalls\n\n","Total",local_msec,global_msec,local_ncalls,global_ncalls);
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void Plasm::dereference(SmartPointer<Hpc>* pc)
{
	START(PLASM_DESTROY);
	//nullify its reference (which cause automatic deallocation)
	pc->reset();
	STOP(PLASM_DESTROY);
	return;
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
SmartPointer<Hpc> Plasm::cube(int dim,float From,float To)
{
	START(PLASM_CUBE);
	SmartPointer<Graph> g(Graph::cuboid(dim,From,To));
	SmartPointer<Hpc> ret(new Hpc(g));
	STOP(PLASM_CUBE);
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
SmartPointer<Hpc> Plasm::simplex(int dim)
{
	START(PLASM_SIMPLEX);
	SmartPointer<Graph> g(Graph::simplex(dim));
	SmartPointer<Hpc> ret(new Hpc(g));
	STOP(PLASM_SIMPLEX);
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
SmartPointer<Hpc> Plasm::Struct(std::vector<SmartPointer<Hpc> > pols)
{
	START(PLASM_STRUCT);
	SmartPointer<Hpc> ret(new Hpc());
	for (int I=0;I<(int)pols.size();I++) ret->add(pols[I]);
	STOP(PLASM_STRUCT);
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
#if 0
SmartPointer<Hpc> Plasm::Struct(SmartPointer<Hpc> first,...)
{
	START(PLASM_STRUCT);
	va_list L;
	va_start(L,first);
	SmartPointer<Hpc> ret(new Hpc());
	ret->add(first);
	for(;;)
	{ 
		SmartPointer<Hpc> p=va_arg(L,SmartPointer<Hpc>);
	    if(!p) break;
		ret->add(p);
	}
	va_end(L);
	STOP(PLASM_STRUCT);
	return ret;
}
#endif

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
SmartPointer<Hpc> Plasm::copy(SmartPointer<Hpc> src)
{
	START(PLASM_COPY);
	SmartPointer<Hpc> ret(new Hpc());
	ret->add(src);
	STOP(PLASM_COPY);
	return ret;
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
int Plasm::getSpaceDim(SmartPointer<Hpc> node)
{
	START(PLASM_GETSPACEDIM);
	int ret= node->spacedim;
	STOP(PLASM_GETSPACEDIM);
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
int Plasm::getPointDim(SmartPointer<Hpc> node)
{
	START(PLASM_GETPOINTDIM);
	int ret= node->pointdim;
	STOP(PLASM_GETPOINTDIM);
	return ret;
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
SmartPointer<Hpc> Plasm::transform(SmartPointer<Hpc> child,SmartPointer<Matf> vmat,SmartPointer<Matf> hmat)
{
	START(PLASM_TRANSFORM);
	XgeDebugAssert(vmat && hmat && vmat->dim==hmat->dim);
	int spacedim=vmat->dim;
	SmartPointer<Hpc> ret(new Hpc(spacedim,0,vmat,hmat));
	ret->add(child);
	STOP(PLASM_TRANSFORM);
	return ret;
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
SmartPointer<Hpc> Plasm::scale(SmartPointer<Hpc> child,Vecf vs)
{
	START(PLASM_SCALE);
	SmartPointer<Matf> Vmat(new Matf(Matf::scaleV(vs)));
	SmartPointer<Matf> Hmat(new Matf(Matf::scaleH(vs)));
	SmartPointer<Hpc> ret(new Hpc(vs.dim,0,Vmat,Hmat));
	ret->add(child);
	STOP(PLASM_SCALE);
	return ret;
}





///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
SmartPointer<Hpc> Plasm::translate(SmartPointer<Hpc> child,Vecf vt)
{
	XgeDebugAssert(vt[0]==0.0f);
	START(PLASM_TRANSLATE);
	SmartPointer<Matf> Vmat(new Matf(Matf::translateV(vt)));
	SmartPointer<Matf> Hmat(new Matf(Matf::translateH(vt)));
	SmartPointer<Hpc> ret(new Hpc(vt.dim,0,Vmat,Hmat));
	ret->add(child);
	STOP(PLASM_TRANSLATE);
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
SmartPointer<Hpc> Plasm::rotate(SmartPointer<Hpc> child,int spacedim,int i,int j,float angle)
{
	START(PLASM_ROTATE);
	SmartPointer<Matf> Vmat(new Matf(Matf::rotateV(spacedim,i,j,angle)));
	SmartPointer<Matf> Hmat(new Matf(Matf::rotateH(spacedim,i,j,angle)));
	XgeDebugAssert(i>=1 && i<=spacedim && j>=1 && j<=spacedim);
	SmartPointer<Hpc> ret(new Hpc(spacedim,0,Vmat,Hmat));
	ret->add(child);
	STOP(PLASM_ROTATE);
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
SmartPointer<Hpc> Plasm::embed(SmartPointer<Hpc> child,int spacedim)
{
	START(PLASM_EMBED);
	SmartPointer<Matf> Vmat(new Matf(Matf(spacedim)));
	SmartPointer<Matf> Hmat(new Matf(Matf(spacedim)));
	SmartPointer<Hpc> ret(new Hpc(spacedim,0,Vmat,Hmat));
	ret->add(child);
	STOP(PLASM_EMBED);
	return ret;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
Boxf Plasm::limits(SmartPointer<Hpc> node)
{
	START(PLASM_LIMITS);
	SmartPointer<Hpc> Temp=Plasm::shrink(node,false);
	int spacedim=getSpaceDim(Temp);
	Boxf box(spacedim);

	for (Hpc::const_iterator it=Temp->childs.begin();it!=Temp->childs.end();it++)
	{
		SmartPointer<Hpc>    child=(SmartPointer<Hpc>)*it;
		SmartPointer<Graph> g=child->g;
		XgeDebugAssert(child->vmat->dim==spacedim);

		if (g)
		{
			box.add(g->getBoundingBox(0,child->vmat,child->hmat)); //apply matrices for all cells
		}
		else if (child->batches.size())
		{
			for (std::vector<SmartPointer<Batch> >::iterator ct=child->batches.begin();ct!=child->batches.end();ct++)
			{
				float* p=(float*)(*ct)->vertices->c_ptr();

				for (int I=0;I<(int)(*ct)->vertices->size();I+=3)
					box.add((*child->vmat)*Vecf(1.0f,p[I+0],p[I+1],p[I+2]));
			}
		}
	}

	STOP(PLASM_LIMITS);
	return box;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
SmartPointer<Hpc>  Plasm::mkpol(int pointdim,const std::vector<float>& points,const std::vector<std::vector<int> >& hulls,float tolerance)
{

	START(PLASM_MKPOL);
	SmartPointer<Hpc> ret(new Hpc());

	//for each hull I need a subset of points
	int npoints=(int)points.size()/pointdim;
	XgeDebugAssert(npoints*pointdim==(int)points.size());
	float* compact_points=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(int)points.size());

	for (int I=0;I<(int)hulls.size();I++)
	{
		int nv=(int)hulls[I].size(); //number of points inside the hull

		XgeDebugAssert(nv<=npoints); //no more than available points

		if (!nv) continue;

		//compact points
		float* p=compact_points;
		for (int J=0;J<nv;J++)
		{
			int idx=hulls[I][J];
			XgeDebugAssert(idx>=0 && idx<npoints); //safety check
			memcpy(p,&points[idx*pointdim],sizeof(float)*pointdim); 
			p+=pointdim;
	
		}

		SmartPointer<Matf> vmat(new Matf(pointdim));
		SmartPointer<Matf> hmat(new Matf(pointdim));
		SmartPointer<Graph> g(Graph::mkpol(*vmat,*hmat,pointdim,nv,compact_points,tolerance));
		SmartPointer<Hpc> new_child(new Hpc(g,vmat,hmat));
		ret->add(new_child);
	}

	//free compact points
	MemPool::getSingleton()->free(sizeof(float)*(int)points.size(),compact_points);
	STOP(PLASM_MKPOL);
	return ret;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SmartPointer<Hpc>  Plasm::mkpolf(int pointdim,const std::vector<float>& planes,const std::vector<std::vector<int> >& hulls,float tolerance)
//{
//	START(PLASM_MKPOLF);
//	SmartPointer<Hpc> ret(new Hpc());
//
//	//for each hull I need a subset of points
//	int nplanes=(int)planes.size()/(pointdim+1);
//	XgeDebugAssert(nplanes*(pointdim+1)==(int)planes.size());
//	float* compact_planes=(float*)MemPool::getSingleton()->malloc(sizeof(float)*(int)planes.size());
//
//	for (int I=0;I<(int)hulls.size();I++)
//	{
//		int nh=(int)hulls[I].size(); //number of points inside the hull
//
//		XgeDebugAssert(nh<=nplanes); //no more than available planes
//
//		if (!nh) continue;
//
//		//compact points
//		float* p=compact_planes;
//		for (int J=0;J<nh;J++)
//		{
//			int idx=hulls[I][J];
//			XgeDebugAssert(idx>=0 && idx<nplanes); //safety check
//			memcpy(p,&planes[idx*(pointdim+1)],sizeof(float)*(pointdim+1)); 
//			p+=(pointdim+1);
//	
//		}
//
//		SmartPointer<Matf> vmat(new Matf(pointdim));
//		SmartPointer<Matf> hmat(new Matf(pointdim));
//		SmartPointer<Graph> g(Graph::mkpolf(*vmat,*hmat,pointdim,nh,compact_planes,tolerance));
//		SmartPointer<Hpc> new_child(new Hpc(g,vmat,hmat));
//		ret->add(new_child);
//	}
//
//	//free compact points
//	MemPool::getSingleton()->free(sizeof(float)*(int)planes.size(),compact_planes);
//
//	STOP(PLASM_MKPOLF);
//	return ret;
//}




/////////////////////////////////////////////////////////////
//ukpol
/////////////////////////////////////////////////////////////
int Plasm::ukpol(SmartPointer<Hpc> node,std::vector<float>& points,std::vector<std::vector<int> >& hulls)
{
	START(PLASM_UKPOL);

	SmartPointer<Hpc> Tmp=Plasm::shrink(node,false);
	int pointdim=getSpaceDim(node);
	
	//find number of points and number of hulls and max pointdim
	int ID=0;
	for (Hpc::const_iterator it=Tmp->childs.begin();it!=Tmp->childs.end();it++)
	{
		SmartPointer<Hpc> child=*it; 
		XgeDebugAssert(child->childs.size()==0 && child->vmat && child->hmat && child->g);
		SmartPointer<Graph> g=child->g;
		SmartPointer<Matf> V=child->vmat;
		SmartPointer<Matf> H=child->hmat;

		//embed in the final dimension
		XgeDebugAssert(V->dim<=pointdim);

		//I can change the matrices here
		(*V) = V->extract(pointdim);

		//I use map since I cannot use nodeTmp() because it is needed by the navigator
		std::map<unsigned int,int> mapv;

		//embed all points
		for (GraphListIterator it=g->each(0);!it.end();it++)
		{
			//assign uniqie id
			mapv[*it]=ID++;

			Vecf P(pointdim);//important the initialization to zero
			memcpy(P.mem,g->getGeometry(*it),sizeof(float)*(g->getPointDim()+1));
			P = (*V) * P; //transform using matrix
			XgeDebugAssert(P[0]==1.0f); //is a point, probably here I need to do the division by the homo coordinates
			for (int I=1;I<=pointdim;I++) points.push_back(P[I]); //ignore homo coorinate
		}

		//find max level cells
		int Lvl;
		for(Lvl=pointdim;Lvl && g->each(Lvl).end(); Lvl--)  ;

		//fill points and hulls
		for (GraphListIterator it=g->each(Lvl);!it.end();it++)
		{
			
			if (Lvl==2)
			{
				unsigned int F=*it;

				//maintain the inner order (if I'm using UKPOL for a map...)
				navigator.nnav[0]=0;

				unsigned int prev0=g->getFirstDwNode(g->getLastDwNode(F)),p1;
				unsigned int prev1=g->getLastDwNode (g->getLastDwNode(F)),p2;

				for (GraphIterator jt=g->goDw(F);!jt.end();jt++,prev0=p1,prev1=p2)
				{
					p1=g->getFirstDwNode(*jt);
					p2=g->getLastDwNode (*jt);
					if (p1!=prev0 && p1!=prev1) Swap(unsigned int,p1,p2);
					navigator.nav[0][navigator.nnav[0]++]=p1;
				}
			}
			else
			{
				//use the navigator to find 0 cells
				g->findCells(0,*it,navigator,true);
			}
			if (navigator.nnav[0])
			{
				std::vector<int> hull;
				for (unsigned int k=0;k<navigator.nnav[0];k++) hull.push_back(mapv[navigator.nav[0][k]]);//I use the map
				hulls.push_back(hull);
			}
		}
	}

	int npoints=(int)points.size()/pointdim;
	XgeDebugAssert(ID==npoints && npoints*pointdim==(int)points.size());
	STOP(PLASM_UKPOL);
	return pointdim;
}




/////////////////////////////////////////////////////////////////////////////////////////
//very sperimental TODO
/////////////////////////////////////////////////////////////////////////////////////////
int Plasm::ukpolf(SmartPointer<Hpc> node,std::vector<float>& planes,std::vector<std::vector<int> >& hulls)
{
	START(PLASM_UKPOLF);

	int dim=getSpaceDim(node);

	//for  dim<2 xge does not store faces 
	if (dim<2)
	{
		STOP(PLASM_UKPOLF);
		return dim;
	}

	int ID=0;
	SmartPointer<Hpc> Temp=Plasm::shrink(node,false); 
	for (Hpc::const_iterator it=Temp->childs.begin();it!=Temp->childs.end();it++)
	{
		SmartPointer<Hpc>    child=(SmartPointer<Hpc>)*it;
		SmartPointer<Graph> g=child->g;
		
		//not full dimension
		if (g->getNCells(dim)==0) 
			continue;
				
		//for each boundary face
		for (GraphListIterator IF=g->each(dim-1);!IF.end();IF++)
		{
			Planef h(dim,g->getGeometry(*IF));
			if (child->hmat) h=h* (*(child->hmat));
			for (int I=0;I<=dim;I++) planes.push_back(h.mem[I]);
			g->NodeTmp(*IF)=ID++;
		}
		
		//for each full cell
		for (GraphListIterator it=g->each(dim);!it.end();it++)
		{
			std::vector<int> hull;
			for (GraphIterator jt=g->goDw(*it);!jt.end();jt++)  hull.push_back(g->NodeTmp(*jt));
			hulls.push_back(hull);
		}
	}
	int nplanes=(int)planes.size()/(dim+1);
	XgeDebugAssert(ID==nplanes && nplanes*(dim+1)==(int)planes.size());
	STOP(PLASM_UKPOLF);
	return dim;
}




/////////////////////////////////////////////////////////////
//join (==mkpol with all points)
/////////////////////////////////////////////////////////////
SmartPointer<Hpc> Plasm::join(std::vector<SmartPointer<Hpc> > pols,float tolerance)
{
	START(PLASM_JOIN);

	int npols=(int)pols.size();
	int spacedim=0;
	std::vector<SmartPointer<Hpc> > temp(npols);

	//extract number of points and spacedim
	int npoints=0;
	for (int i=0;i<(int)npols;i++)
	{
		temp[i]=Plasm::shrink(pols[i],false);
		spacedim=max2(spacedim,getSpaceDim(temp[i]));

		for (Hpc::const_iterator it=temp[i]->childs.begin();it!=temp[i]->childs.end();it++)
		{
			SmartPointer<Hpc>    c=(SmartPointer<Hpc>)*it;
			SmartPointer<Graph> g= c->g;
			npoints+=g->getNCells(0);
		}
	}

	//I need a temporay buffer where to store the embedded points
	float* points=(float*)MemPool::getSingleton()->malloc(sizeof(float)*npoints*spacedim);
	float* p=points;

	for (int i=0;i<npols;i++)
	{
		for (Hpc::const_iterator it=temp[i]->childs.begin();it!=temp[i]->childs.end();it++)
		{
			SmartPointer<Hpc>    c=*it;
			SmartPointer<Graph> g=c->g;

			//I can change the matrix here since it is not shared
			(*c->vmat)=c->vmat->extract(spacedim);

			//embed all points
			for (GraphListIterator it=g->each(0);!it.end();it++)
			{
				Vecf P(spacedim);//important is set to 0
				memcpy(P.mem,g->getGeometry(*it),sizeof(float)*(g->getPointDim()+1));
				XgeDebugAssert(P[0]==1.0f);
				P=(*c->vmat) * P; //embed the vertex
				XgeDebugAssert(P[0]==1.0f); //probably here I need to consider a change in the homo coordinates
				memcpy(p,P.mem+1,sizeof(float)*spacedim); //do not copy homo coorinate
				p+=spacedim;
			}
		}
	}
	XgeDebugAssert(p==(points+npoints*spacedim));

	//build the mkpol with new points
	SmartPointer<Matf> vmat(new Matf(spacedim));
	SmartPointer<Matf> hmat(new Matf(spacedim));
	SmartPointer<Graph> g(Graph::mkpol(*vmat,*hmat,spacedim,npoints,points,tolerance));

	SmartPointer<Hpc> hpc(new Hpc(g,vmat,hmat));
	MemPool::getSingleton()->free(sizeof(float)*npoints*spacedim,points);

	STOP(PLASM_JOIN);
	return hpc;
}



/////////////////////////////////////////////////////////////
//power
/////////////////////////////////////////////////////////////
SmartPointer<Hpc> Plasm::power(SmartPointer<Hpc> arg1,SmartPointer<Hpc> arg2)
{
	START(PLASM_POWER);

	SmartPointer<Hpc> arg1_simplify=Plasm::shrink(arg1,false);
	SmartPointer<Hpc> arg2_simplify=Plasm::shrink(arg2,false);

	//makeHpc the container
	SmartPointer<Hpc> dir(new Hpc());

	//for each couple of geometries....
	int num=0;
	for (Hpc::const_iterator A1=arg1_simplify->childs.begin();A1!=arg1_simplify->childs.end();A1++)
	for (Hpc::const_iterator A2=arg2_simplify->childs.begin();A2!=arg2_simplify->childs.end();A2++)
	{
		SmartPointer<Hpc> hpc1=*A1; SmartPointer<Graph> g1=hpc1->g;
		SmartPointer<Hpc> hpc2=*A2; SmartPointer<Graph> g2=hpc2->g;

		int spacedim1=(hpc1->spacedim);
		SmartPointer<Matf> v1=hpc1->vmat;
		SmartPointer<Matf> h1=hpc1->hmat;

		int spacedim2=(hpc2->spacedim);
		SmartPointer<Matf> v2=hpc2->vmat;
		SmartPointer<Matf> h2=hpc2->hmat;

		SmartPointer<Matf> vmat(new Matf(spacedim1+spacedim2));
		SmartPointer<Matf> hmat(new Matf(spacedim1+spacedim2));
		SmartPointer<Graph> g(Graph::power(*vmat.get(),*hmat.get(),g1,v1,h1,g2,v2,h2));

		SmartPointer<Hpc> new_child(new Hpc(g,vmat,hmat));

		dir->add(new_child);
	}

	STOP(PLASM_POWER);
	return dir;
}



/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/*
SmartPointer<Hpc> Plasm::boolop(int operation,SmartPointer<Hpc> first,...)
{
	//do not collect time
	std::vector<SmartPointer<Hpc> > pols;
	pols.push_back(first);
	va_list L;
	va_start(L,first);
	for(;;)
	{ 
		SmartPointer<Hpc> p=va_arg(L,SmartPointer<Hpc>);
	    if(!p) break;
		pols.push_back(p);
	}
	va_end(L);
	SmartPointer<Hpc> ret=boolop(operation,(int)pols.size(),&pols[0]);
	return ret;
}
*/

/////////////////////////////////////////////////////////////
// skeleton
/////////////////////////////////////////////////////////////
SmartPointer<Hpc> Plasm::skeleton(SmartPointer<Hpc> Src,int level,float tolerance)
{
	START(PLASM_SKELETON);

	SmartPointer<Hpc> Tmp=Plasm::shrink(Src,false); 

	//makeHpc the container
	SmartPointer<Hpc> dir(new Hpc());

	//for each geometry file...
	int cont=0;
	for (Hpc::const_iterator it=Tmp->childs.begin();it!=Tmp->childs.end();it++)
	{
		SmartPointer<Hpc> c=(SmartPointer<Hpc>)*it;
		XgeDebugAssert(c->childs.size()==0 && c->g && c->vmat && c->hmat);
		
		SmartPointer<Graph> g=c->g;
		SmartPointer<Matf>   V=c->vmat;;
		SmartPointer<Matf>   H=c->hmat;

		int ncells=g->getNCells(level);

		if (!ncells) continue;

		//build the new skeleton for this child
		SmartPointer<Matf> sub_vmat(new Matf(*V));
		SmartPointer<Matf> sub_hmat(new Matf(*H));
		SmartPointer<Hpc> sub(new Hpc(0,0,sub_vmat,sub_hmat));
		
		cont++;

		//already in requeste dimension
		if (level==g->getPointDim())
		{
			XgeDebugAssert(g->getNCells(level)==ncells);
			SmartPointer<Hpc> subsub(new Hpc(g));
			sub->add(subsub);
		}
		//should make the skeleton
		else
		{
			int pointdim=g->getPointDim();
			XgeDebugAssert (level<pointdim);

			for (GraphListIterator IT=g->each(level);!IT.end();IT++)
			{
				unsigned int C=*IT;
				int npoints=(int)g->findCells(0,C,navigator);
				XgeDebugAssert(npoints);
				int size=sizeof(float)*npoints*pointdim;
				float* points=(float*)MemPool::getSingleton()->malloc(size); //exclude the homogeneous coordinate

				for (int I=0;I<npoints;I++)
				{
					float* vg=g->getGeometry(navigator.nav[0][I]);
					memcpy(points+I*pointdim,vg+1,sizeof(float)*pointdim); //exclude homogeneous coordinate
				}

				SmartPointer<Matf> sub_vmat(new Matf(pointdim));
				SmartPointer<Matf> sub_hmat(new Matf(pointdim));
				SmartPointer<Graph> sub_g=g->mkpol(*sub_vmat.get(),*sub_hmat.get(),pointdim,npoints,points,tolerance);
				MemPool::getSingleton()->free(size,points); 
				SmartPointer<Hpc> sub_sub(new Hpc(sub_g,sub_vmat,sub_hmat));
				sub->add(sub_sub);
			}
		}

		dir->add(sub);
	}

	STOP(PLASM_SKELETON);
	return dir;
}



/////////////////////////////////////////////////////////////
// PROPERTIES
/////////////////////////////////////////////////////////////

SmartPointer<Hpc> Plasm::addProperty(SmartPointer<Hpc> src,std::string _pname,std::string _pvalue)
{
	START(PLASM_ADDPROPERTY);
	const char* pname=_pname.c_str();
	const char* pvalue=_pvalue.c_str();

	SmartPointer<Hpc> ret;

	if (!strcmp(pname,HPC_PROP_NAME))
	{
		ret=src;
	}
	else
	{
		ret.reset(new Hpc());
		ret->add(src);
	}

	if (!ret->prop) ret->prop.reset(new PropertySet);
	(*ret->prop)[_pname]=_pvalue;
	STOP(PLASM_ADDPROPERTY);
	return ret;
}


/////////////////////////////////////////////////////////////
std::string  Plasm::getProperty(SmartPointer<Hpc> node,std::string pname)
{
	START(PLASM_GETPROPERTY);
	std::string ret;
	if (node->prop && node->prop->find(pname)!=node->prop->end()) ret=(*node->prop)[pname];
	STOP(PLASM_GETPROPERTY);
	return ret;
}

/////////////////////////////////////////////////////////////
void Plasm::Print(SmartPointer<Hpc> node,int nrec)
{
	SmartPointer<Hpc>    hpc=(SmartPointer<Hpc>)node;
	SmartPointer<Matf>   v=hpc->vmat;
	SmartPointer<Matf>   h=hpc->hmat;

	for (int i=0;i<nrec;i++) Log::printf(" ");
	Log::printf("hpc nchilds(%d) spacedim(%d) pointdim(%d) ",(int)hpc->getNumberOfChilds(),hpc->spacedim,hpc->pointdim);

	if (getProperty(node,HPC_PROP_VRML_MATERIAL)!="")
		Log::printf("material(%s) ",getProperty(node,HPC_PROP_VRML_MATERIAL).c_str());

	if (getProperty(node,HPC_PROP_RGB_COLOR)!="")
		Log::printf("color(%s) ",getProperty(node,HPC_PROP_RGB_COLOR).c_str());

	if (v) 
		Log::printf("vmat(%s)",v->str().c_str());	

	if (node->batches.size())
		Log::printf("Batch(num=%d) ",node->batches.size());

	Log::printf("\n");

	if (node->g)
	{
		if (node->g)
			node->g->Print();

	}

	for (Hpc::const_iterator it=node->childs.begin();it!=node->childs.end();it++)
		Print((SmartPointer<Hpc>)*it,nrec+1);
}


/////////////////////////////////////////////////////////////
void innerShrink(SmartPointer<Hpc> dst, SmartPointer<Hpc> src,SmartPointer<Matf> vacc,SmartPointer<Matf> hacc,const SmartPointer<PropertySet> pacc,bool bCloneGeometry)			  
{
	//************ accumulate matrices ************ 
	if (!vacc) vacc.reset(new Matf(src->spacedim));
	if (!hacc) hacc.reset(new Matf(src->spacedim));

	SmartPointer<Matf>  v=vacc;
	SmartPointer<Matf>  h=hacc;

	if (src->vmat)
	{
		XgeDebugAssert(src->vmat->dim<=vacc->dim);
		v.reset( new Matf((*vacc) * src->vmat->extract(vacc->dim)));
		h.reset( new Matf(src->hmat->extract(hacc->dim) * (*hacc)));
	}
	
	//************  accumulate properties ************ 
	SmartPointer<PropertySet> p=(src->prop)?src->prop:pacc;
	
	if (src->prop && pacc)
	{
		//merge
		p.reset(new PropertySet(*pacc));
		for (PropertySet::const_iterator it=src->prop->begin();it!=src->prop->end();it++) (*p)[it->first]=it->second;
	}

	// ************  final node ************  
	if (!src->getNumberOfChilds())
	{
		SmartPointer<Hpc> new_node(new Hpc(*src)); //copy all the vertices, normals etc, doen not make copy of them
		new_node->vmat .reset( new Matf(*v));
		new_node->hmat .reset( new Matf(*h));
		new_node->prop .reset( p?new PropertySet(*p):0);

		if (bCloneGeometry)
			new_node->g=src->g->clone();
		else
			new_node->g=src->g;

		new_node->fixDimensions();
		dst->add(new_node);
	}
	else
	{
		XgeDebugAssert(!src->g);

		for (Hpc::const_iterator it=src->childs.begin();it!=src->childs.end();it++)
			innerShrink(dst,*it,v,h,p,bCloneGeometry);
	}
}

SmartPointer<Hpc> Plasm::shrink(SmartPointer<Hpc> node,bool bCloneGeometry)
{
	START(PLASM_SHRINK);

	if (!node)
		return SmartPointer<Hpc>();

	SmartPointer<Hpc> ret(new Hpc());
	SmartPointer<Matf> vacc;
	SmartPointer<Matf> hacc;
	SmartPointer<PropertySet> pacc;
	innerShrink(ret,node,vacc,hacc,pacc,bCloneGeometry);
	STOP(PLASM_SHRINK);
	return ret;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SmartPointer<Hpc> SkinInner(SmartPointer<Hpc> src,const std::string& url,SmartPointer<Matf> project_uv,SmartPointer<Matf> vacc)
{
	//a node with spacedim more than 3d... cannot apply texture mapping so return the src itself which will be shared
	if (src->spacedim>3)
		return src;

	//copy from 
	SmartPointer<Hpc>ret(new Hpc(*src));

	//******* accumulate matrices ****************
	if (!vacc) vacc.reset(new Matf(src->spacedim));
	SmartPointer<Matf>  v=vacc;
	if (src->vmat)
	{
		XgeDebugAssert(src->vmat->dim<=vacc->dim);
		v.reset(new Matf( (*vacc) * src->vmat->extract(vacc->dim)));
	}
	
	//there is some geometry to appy the texture coordinates
	if (ret->g || ret->batches.size()>0)
	{
		//I will transform only triangles, i need a 4x4 matrix so I can embed the matrix (for example from 2d to 3d)
		XgeDebugAssert(v->dim<=3);//safety check
		(*v)=v->extract(3);

		//automatic generation of compiled geometry since it has not already been compiled
		if (!ret->batches.size())
		{
			// I cannot produce triangles here so return the exact copy of the node
			SmartPointer<Batch> batch=ret->g->getBatch();

			if (batch->primitive==Batch::TRIANGLES) 
				ret->batches.push_back(batch);
		}

		//iterate in all compiled geometry
		for (std::vector<SmartPointer<Batch> >::iterator ct=ret->batches.begin();ct!=ret->batches.end();ct++)
		{
			//cannot apply texture mapping
			if ((*ct)->primitive!=Batch::TRIANGLES)
				continue;

			//overwrite texture0 and texture coordinates (this is an exclusive copy, I'm not changing src argument)
			(*ct)->texture0=Texture::open(url);

			int nv=(*ct)->vertices->size()/3;
			(*ct)->texture0coords.reset(new Array(nv*2));

			float* pv=(float*)(*ct)->vertices  ->c_ptr();
			float* pt=(float*)(*ct)->texture0coords->c_ptr();

			for (int I=0;I<nv;I++,pv+=3,pt+=2)
			{
				Vecf vertex=(*project_uv) * ((*v) * Vecf(1.0,pv[0],pv[1],pv[2]));
				pt[0]=(float)vertex[1];
				pt[1]=(float)vertex[2];
			}
		}
		return ret;
	}

	//iterative call
	for (Hpc::const_iterator child=src->childs.begin();child!=src->childs.end();child++)
		ret->add(SkinInner(*child,url,project_uv,v));

	return ret;
}


SmartPointer<Hpc> Plasm::Skin(SmartPointer<Hpc> src,std::string url,SmartPointer<Matf> project_uv)
{
	START(PLASM_SKIN);
	SmartPointer<Matf> vacc;
	SmartPointer<Hpc> ret=SkinInner(src,url,project_uv,vacc);
	STOP(PLASM_SKIN);
	return ret;
}



////////////////////////////////////////////////////////////////////////////
SmartPointer<Array> Plasm::getTriangles(SmartPointer<Hpc> src)
{
	SmartPointer<Hpc> node=Plasm::shrink(src,false);

	//count number of triangles
	int ntriangles=0;
	for (Hpc::const_iterator it=node->childs.begin();it!=node->childs.end();it++)
	{
		SmartPointer<Hpc> c=*it;

		if (!(c->spacedim>=0 && c->spacedim<=3)) 
			continue;

		//compilation
		if (!c->batches.size())
		{
			SmartPointer<Batch> batch=c->g->getBatch();

			if (batch->primitive==Batch::TRIANGLES) 
				c->batches.push_back(batch);
		}

		for (std::vector<SmartPointer<Batch> >::iterator ct=c->batches.begin();ct!=c->batches.end();ct++)
		{
			if ((*ct)->primitive!=Batch::TRIANGLES) 
				continue;
			
			ntriangles+=(*ct)->vertices->size()/9;;
		}
	}

	//no triangles
	if (!ntriangles)
		return SmartPointer<Array>();

	//step 2, fill triangles
	SmartPointer<Array > ret(new Array(ntriangles*9));
	float* p=(float*)ret->c_ptr();

	for (Hpc::const_iterator it=node->childs.begin();it!=node->childs.end();it++)
	{
		SmartPointer<Hpc> c=*it;

		if (!(c->spacedim>=0 && c->spacedim<=3)) 
			continue;

		Matf vmat=c->vmat->extract(3);

		for (std::vector<SmartPointer<Batch> >::iterator ct=c->batches.begin();ct!=c->batches.end();ct++)
		{
			if ((*ct)->primitive!=Batch::TRIANGLES) 
				continue;

			float* pv=(float*)(*ct)->vertices->c_ptr();
			int nt=(*ct)->vertices->size()/9;

			for (int T=0;T<nt;T++,pv+=9)
			{
				Vecf v0(vmat*Vecf(1.0,pv[0],pv[1],pv[2])); *p++=v0[1];*p++=v0[2];*p++=v0[3];
				Vecf v1(vmat*Vecf(1.0,pv[3],pv[4],pv[5])); *p++=v1[1];*p++=v1[2];*p++=v1[3];
				Vecf v2(vmat*Vecf(1.0,pv[6],pv[7],pv[8])); *p++=v2[1];*p++=v2[2];*p++=v2[3];
			}
		}
	}

	return ret;
}




/////////////////////////////////////////////////////////////
SmartPointer<Hpc> Plasm::open(std::string Filename)
{
	Clock t1;
	Log::printf("Opening file %s\n",Filename.c_str());

	Archive ar;
	if (!ar.Open(Filename,false))
	{
		Log::printf("Cannot open file %s\n",Filename.c_str());
		return SmartPointer<Hpc>();
	}

	SmartPointer<Hpc> ret(new Hpc());
	ar.Push("Hpc");
	ret->Read(ar);
	ar.Pop("Hpc");
	Log::printf("opened file %s in %d msec\n",Filename.c_str(),(int)(Clock()-t1));
	return ret;
}



/////////////////////////////////////////////////////////////
bool Plasm::save(SmartPointer<Hpc> node,std::string Filename)
{
	Clock t1;
	Log::printf("Saving file %s\n",Filename.c_str());

	Archive ar;
	if (!ar.Open(Filename,true))
	{
		Log::printf("Cannot save file %s\n",Filename.c_str());
		return false;
	}

	ar.Push("Hpc");
	node->Write(ar);
	ar.Pop("Hpc");
	Log::printf("saved file %s in %d msec\n",Filename.c_str(),(int)(Clock()-t1));
	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
std::vector<SmartPointer<Batch> > Plasm::getBatches(SmartPointer<Hpc> src,bool bOptimize)
{
	Clock t1;
	Log::printf("Building batches from Hpc....\n");

	std::vector<SmartPointer<Batch> > batches;

	SmartPointer<Hpc> temp=Plasm::shrink(src,false);

	if (!temp)
		return batches;

	for (Hpc::const_iterator it=temp->childs.begin();it!=temp->childs.end();it++)
	{
		SmartPointer<Hpc> child=(*it);

		//cannot show anything
		if (!(child->spacedim>=0 && child->spacedim<=3))
			continue;

		//matrix
		Mat4f matrix(child->vmat->toMat4f());

		//material
		SmartPointer<PropertySet> prop=child->prop;
		std::string mtl=Plasm::getProperty(child,HPC_PROP_VRML_MATERIAL);
		std::string rgb=Plasm::getProperty(child,HPC_PROP_RGB_COLOR    );

		Color4f ambient=Batch::DefaultAmbientColor;
		Color4f diffuse=Batch::DefaultDiffuseColor;
		Color4f specular=Batch::DefaultSpecularColor;
		Color4f emission=Batch::DefaultEmissionColor;
		float   shininess=Batch::DefaultShininess;

		if (mtl.length()) 
		{
			sscanf(mtl.c_str(),"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
				&ambient [0],&ambient [1],&ambient [2],&ambient[3],
				&diffuse [0],&diffuse [1],&diffuse [2],&diffuse[3],
				&specular[0],&specular[1],&specular[2],&specular[3],
				&emission[0],&emission[1],&emission[2],&emission[3],
				&shininess);
		}
		else if (rgb.length())
		{
			float r=0,g=0,b=0,a=1;
			sscanf(rgb.c_str(),"%f %f %f %f",&r,&g,&b,&a);
			ambient[0]*=r;ambient[1]*=g;ambient[2]*=b;ambient[3]*=a;
			diffuse[0]*=r;diffuse[1]*=g;diffuse[2]*=b;ambient[3]*=a;
		}


		if (child->batches.size())
		{
			for (std::vector<SmartPointer<Batch> >::iterator ct=child->batches.begin();ct!=child->batches.end();ct++)
			{
				//i get a copy I can modify
				SmartPointer<Batch> batch(new Batch(**ct));

				batch->matrix  = matrix;

				batch->ambient=ambient;
				batch->diffuse=diffuse;
				batch->specular=specular;
				batch->emission=emission;
				batch->shininess=shininess;

				Box3f box=batch->getBox();

				if (box.isValid())
					batches.push_back(batch);

				#if 1
				//TODO: in mzplasm I need to force the regeneration texture
				if (batch->texture0 ) batch->texture0->gpu.reset();
				if (batch->texture1 ) batch->texture1->gpu.reset();
				#endif
			}
		}
		else if (child->g)
		{
			SmartPointer<Batch> batch=child->g->getBatch();

			if (batch->primitive>=Batch::TRIANGLES)
			{
				XgeReleaseAssert(batch->normals);
			}

			batch->matrix = matrix;

			batch->ambient=ambient;
			batch->diffuse=diffuse;
			batch->specular=specular;
			batch->emission=emission;
			batch->shininess=shininess;

			Box3f box=batch->getBox();

			if (box.isValid())
				batches.push_back(batch);
		}
		else
		{
			//nothing to show
			continue;
		}
	}
	Log::printf("...done in %d msec\n",(int)t1.msec());

	if (bOptimize)
		batches=Batch::Optimize(batches);
	
	return batches;
}


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void Plasm::view(SmartPointer<Hpc> src)
{
	GLCanvas glcanvas;
  glcanvas.setOctree(SmartPointer<Octree>(new Octree(getBatches(src))));
	glcanvas.runLoop();
}
