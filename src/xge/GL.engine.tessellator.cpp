
#include <xge/xge.h>
#include <xge/engine.h>
#include <GL/glew.h>

#if ENGINE_ENABLE_TESSELLATOR

#ifndef _WINDOWS
#define CALLBACK
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TESSELLATION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	//counter clock wise
	bool ccw;

	//for the result
	std::vector<Vec3f> current;
	std::vector<Vec3f> result;

	//to accumulate input vertices
	std::vector<double*> to_dealloc; 

	//tess
	GLUtesselator *tess;

	//type of the primitive
	int type;
}
tessellate_t;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void CALLBACK callback_error(unsigned int errorCode)
{
   const GLubyte *estring;
   estring = gluErrorString(errorCode);
   Log::printf("Tessellation Fatal Error: %s\n", estring);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void CALLBACK callback_begin(unsigned int type,tessellate_t* t)
{
	t->type=type;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void CALLBACK callback_vertex(double* v,tessellate_t* t)
{
	t->current.push_back(Vec3f(v));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void CALLBACK callback_combine(double coords[3],void *d[4], float w[4], void **dataOut,tessellate_t* t) 
{ 
	double* v=new double[3];
	v[0]=coords[0];v[1]=coords[1];v[2]=coords[2];
	t->to_dealloc.push_back(v);
	*dataOut = v;
} 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void CALLBACK callback_end(tessellate_t* t)
{
	//triangle fan
	if (t->type == GL_TRIANGLE_FAN)
	{
		std::vector<Vec3f> current;

		int first = 0;
		int last  = 1;
		for (int next=2;next<(int)t->current.size();next++)
		{
			current.push_back(t->current[first]);
			current.push_back(t->current[last ]);
			current.push_back(t->current[next ]);
			last = next;
		}
		t->current=current;
	}
	//triangle strip
	else if (t->type == GL_TRIANGLE_STRIP)
	{
		std::vector<Vec3f> current;

		for (int marker=0;marker<(int)(t->current.size()-2);marker++)
		{
			if (marker % 2) // odd
			{
				current.push_back(t->current[marker  ]);
				current.push_back(t->current[marker+1]);
				current.push_back(t->current[marker+2]);
			}
			else
			{
				current.push_back( t->current[marker+1]);
				current.push_back( t->current[marker  ]);
				current.push_back( t->current[marker+2]);
			}
		}
		t->current=current;
	}
	//triangles
	else if (t->type == GL_TRIANGLES)
	{
		; //nothing to do!
	}
	else
	{
		Utils::Error(HERE,"An unsupported geometry type was encountered in the polygon tessellator end callback");
	}
	
	if (!t->ccw) 
		std::reverse(t->current.begin(), t->current.end());

	//merge current into result
	for (int I=0;I<(int)t->current.size();I++) t->result.push_back(t->current[I]);
	t->current.clear();
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EngineTessellator::Run(double windingRule, bool ccw)
{
	//make sure the last polygon is inserted
	if (current.size())
	{
		polygons.push_back(current);
		current.clear();
	}

	tessellate_t* t=new tessellate_t;
	t->ccw = ccw;
	t->tess = gluNewTess();
	DebugAssert(t->tess);

	gluTessCallback( t->tess, GLU_TESS_ERROR       ,(GLvoid  (CALLBACK*)())callback_error);
	gluTessCallback( t->tess, GLU_TESS_VERTEX_DATA ,(GLvoid  (CALLBACK*)())callback_vertex);
	gluTessCallback( t->tess, GLU_TESS_BEGIN_DATA  ,(GLvoid  (CALLBACK*)())callback_begin);
	gluTessCallback( t->tess, GLU_TESS_END_DATA    ,(GLvoid  (CALLBACK*)())callback_end);
	gluTessCallback( t->tess, GLU_TESS_COMBINE_DATA,(GLvoid  (CALLBACK*)())callback_combine);
	gluTessProperty( t->tess, GLU_TESS_WINDING_RULE, windingRule );
	gluTessProperty( t->tess, GLU_TESS_BOUNDARY_ONLY,GL_FALSE); 

	for (polygon_set_t::const_iterator IT=polygons.begin();IT!=polygons.end();IT++)
	{
		const polygon_t& polygon=*IT;

		gluTessBeginPolygon(t->tess,t);
		for (polygon_t::const_iterator JT=polygon.begin();JT!=polygon.end();JT++)
		{
			const contour_t& contour=*JT;
			gluTessBeginContour(t->tess);
			for (int V=0;V<(int)contour.size();V++)
			{
				double* v=new double[3];
				t->to_dealloc.push_back(v);
				v[0]=contour[V].x;
				v[1]=contour[V].y;
				v[2]=contour[V].z;
				gluTessVertex(t->tess,v,v);
			}
			gluTessEndContour(t->tess);
		}
		gluTessEndPolygon(t->tess);
	}

	this->m_raw_triangles=std::vector<Vec3f>();
	for (int I=0;I<(int)t->result.size();I+=3)
	{
		Vec3f& v0=t->result[I+0];this->m_raw_triangles.push_back(Vec3f(v0.x,v0.y,v0.z));
		Vec3f& v1=t->result[I+1];this->m_raw_triangles.push_back(Vec3f(v1.x,v1.y,v1.z));
		Vec3f& v2=t->result[I+2];this->m_raw_triangles.push_back(Vec3f(v2.x,v2.y,v2.z));	
	}

	//the projection matrix
	Mat4f T=Mat4f::getProjectionMatrix(t->result);
	Mat4f Ti=T.invert();

	this->m_matrix=Mat4f(Ti);

	//the graph
	m_g.reset(new Graph(2));

	for (int I=0;I<(int)t->result.size();I+=3)
	{
		Vec3f _v0=T * t->result[I+0];
		Vec3f _v1=T * t->result[I+1];
		Vec3f _v2=T * t->result[I+2];

		//after the projection they should have one less coordinate (which should be v.z)
		Vecf v0=Vecf(1.0f,_v0.x,_v0.y);
		Vecf v1=Vecf(1.0f,_v1.x,_v1.y);
		Vecf v2=Vecf(1.0f,_v2.x,_v2.y);

		//*** vertices *** 
		unsigned int N0=m_g->findVertex(v0); 
		unsigned int N1=m_g->findVertex(v1); 
		unsigned int N2=m_g->findVertex(v2);

		if (!N0) N0=m_g->addVertex(v0);
		if (!N1) N1=m_g->addVertex(v1);
		if (!N2) N2=m_g->addVertex(v2);

		//***  edges *** 
		unsigned int E01=0,E12=0,E20=0;

		E01=m_g->findFirstCommonNode(N0,N1,Graph::DIRECTION_UP);
		E12=m_g->findFirstCommonNode(N1,N2,Graph::DIRECTION_UP);
		E20=m_g->findFirstCommonNode(N2,N0,Graph::DIRECTION_UP);

		if (!E01) {E01=m_g->addEdge(N0,N1);memcpy(m_g->getGeometry(E01,true),m_g->getFittingPlane(E01).mem,sizeof(float)*3);}
		if (!E12) {E12=m_g->addEdge(N1,N2);memcpy(m_g->getGeometry(E12,true),m_g->getFittingPlane(E12).mem,sizeof(float)*3);}
		if (!E20) {E20=m_g->addEdge(N2,N0);memcpy(m_g->getGeometry(E20,true),m_g->getFittingPlane(E20).mem,sizeof(float)*3);}

		//***  triangle *** 
		unsigned int T=0;

		if (!T) {T=m_g->findFirstCommonNode(E01,E12,Graph::DIRECTION_UP);DebugAssert(!T || m_g->findArch(E20,T,Graph::DIRECTION_UP)!=0);}
		if (!T) {T=m_g->findFirstCommonNode(E12,E20,Graph::DIRECTION_UP);DebugAssert(!T || m_g->findArch(E01,T,Graph::DIRECTION_UP)!=0);}
		if (!T) {T=m_g->findFirstCommonNode(E20,E01,Graph::DIRECTION_UP);DebugAssert(!T || m_g->findArch(E12,T,Graph::DIRECTION_UP)!=0);}

		//if the triangle does not exists
		if (!T)
		{
			unsigned int T=m_g->addNode(2);
			m_g->addArch(E01,T);
			m_g->addArch(E12,T);
			m_g->addArch(E20,T);
			m_g->addArch(T,N0);//double connectivity
			m_g->addArch(T,N1);
			m_g->addArch(T,N2);
		}
	}

	//deallocate tessellator
	gluDeleteTess(t->tess);
	for (int i=0;i<(int)t->to_dealloc.size();i++)  delete [] t->to_dealloc[i];
	delete t;

}



#endif //ENGINE_ENABLE_TESSELLATOR

