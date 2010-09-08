#ifndef _ENGINE_H__
#define _ENGINE_H__

#include <xge/xge.h>


//predeclaration
class Engine;
class EngineResource;


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
class XGE_API Engine
{
public:

	enum
	{
		CULL_DISABLE,
		CULL_CCW,
		CULL_CW
	};

	//! build a rendering context based on an existing control
	Engine(int64 DC);

	//~destructor
	~Engine();
	
	//! explicit destructor (for .net garbace collector)
	void Destroy();

	//! get the DC
	inline int64 GetDC()
		{return this->DC;}

	//! bind/unbind this engine (only one thread can bind, remember to unbind)
	bool Bind();

	//! unbind
	bool Unbind();
	
	//! return an estimation of memory locked on gpu
	static void PrintStatistics();

	//! for shutting down
	static void Shutdown();

	//some top-level utils to setup the scene
	void  ClearScreen          (bool ClearColor=true,bool ClearDepth=true);
	void  SetViewport          (int x,int y,int width,int height);
	void  SetProjectionMatrix  (Mat4f mat);
	void  SetModelviewMatrix   (Mat4f mat);
	void  SetDefaultLight      (Vec3f pos);
	void  FlushScreen          ();

	//! set the state of  the engine
	void PushModelviewMatrix(Mat4f mat,bool AccumulateWithCurrent=true);
	void PopModelviewMatrix();

	void PushProjectionMatrix(Mat4f mat,bool AccumulateWithCurrent=true);
	void PopProjectionMatrix();
	
	//! state of the engine
	void SetDepthTest(bool value);
	void SetDepthWrite(bool vaue);
	void SetCulling(int value); //use CULL_xxx
	void SetPolygonMode(int value); //use Batch::POINTS,Batch::LINES or Batch::POLYGON
	void SetPointSize(float value);
	void SetLineWidth(float value);
	void SetPolygonOffset(float value);

	//some rendering utils
	void  Render  (std::string text,Vec3f pos,Color4f color,bool DisableDepthTest);

	//the frustum is needed only for ORTHO based rendered
	void  Render  (SmartPointer<Batch> batch);

	//! access the current context
	static int64 getCurrentContext();

private:

	//coordinates of the engine
	int64  DC,RC,WC;

	//from RC to WC
	static std::map<int64,int64> wcs;

	//for Bind/Unbind
	SpinLock lock;

	//! must have the Lock on this Engine to call it
	void createArrayBuffer (SmartPointer<Vector>  vector );
	void createTexture     (SmartPointer<Texture> texture);
	
	//! deallocate from GPU
	static void removeFromGpu(EngineResource* resource);

	//the shared context
	static Engine* _shared_context;

	//create the shared context
	Engine();

	//os specific function
	void CreateContext() ;

	//internal function (use Destroy)
	void DestroyContext();

	//for text rendering
	static int FONT_DISPLAY_LIST_BASE;

	//! first init setup
	void Initialize();
	
	//
	friend class Viewer;

	//! friendship
	friend class EngineResource;
	friend class EngineFbo;
	friend class EngineTessellator;
	friend class EngineShader;
	friend class EngineShadowShader;

};



///////////////////////////////////////////////////////////////
class XGE_API EngineResource
{
public:

	//type of resources which can be created
	enum 
	{
		RESOURCE_UNKNOWN=0,
		RESOURCE_ARRAY_BUFFER,
		RESOURCE_RENDER_BUFFER,
		RESOURCE_FRAME_BUFFER,
		RESOURCE_SHADER,
		RESOURCE_PROGRAM,
		RESOURCE_TEXTURE,
		RESOURCE_END
	};

	int          type;
	unsigned int id;
	int          size;

	//! constructor
	EngineResource(int type,unsigned int id,int size);

	//! destructor
	inline ~EngineResource()
	{
		Engine::removeFromGpu(this);
	}
	
}; //EngineResource




//=========================================================================
//! class which uses OpenGL tessellator to build Graph
/*!
see http://www.sysworks.com.au/disk$axpdocmar961/decw$book/d37vaa32.p56.decw$book

Esempio Python:

	t=EngineTessellator()
	t.newPolygon()
	t.addContour([Vec3f(0.0,0.0),Vec3f(1.0,0.0),Vec3f(1.0,1.0),Vec3f(0.0,1.0)])
	t.addContour([Vec3f(0.2,0.2),Vec3f(0.8,0.2),Vec3f(0.8,0.8),Vec3f(0.2,0.8)])
	t.newPolygon()
	t.addContour([Vec3f(2.0,2.0),Vec3f(3.0,2.0),Vec3f(3.0,3.0),Vec3f(2.0,3.0)])
	t.addContour([Vec3f(2.2,2.2),Vec3f(2.8,2.2),Vec3f(2.8,2.8),Vec3f(2.2,2.8)])
	graph=t.getGraph()

*/
//=========================================================================

#if ENGINE_ENABLE_TESSELLATOR

class XGE_API EngineTessellator
{
public:

	enum WindingRule
	{
		TESS_WINDING_ODD, 
		TESS_WINDING_NONZERO, 
		TESS_WINDING_POSITIVE, 
		TESS_WINDING_NEGATIVE, 
		TESS_WINDING_ABS_GEQ_TWO
	};

	//!  typedef
	typedef std::vector<Vec3f> contour_t;

	//!  typedef
	typedef std::vector<contour_t> polygon_t;

	//!  typedef
	typedef std::vector<polygon_t> polygon_set_t;

	//! current polygon
	polygon_t     current;

	//! all polygons
	polygon_set_t polygons;

	//! add new polygon (a polygon contains multiple contours)
	inline void AddPolygon()
	{
		//the current polygon is still empty
		if (current.size()==0) return;
		polygons.push_back(current);
		current.clear();
	}

	//! add a new countour to the current polygon
	void AddContour(const std::vector<Vec3f>& contour)
	{
		if (contour.size())
			current.push_back(contour);
	}

	//! get the current solution i.e. the Graph
	void Run(double windingRule=TESS_WINDING_ODD, bool ccw=true);

	//! after a run() this will contain all raw triangles
	inline std::vector<Vec3f> GetTriangles()
	{
		return this->m_raw_triangles;
	}
	//! after a run() this will contain the projection matrix (which should be the identity if the vertices are already on Z=0!)
	inline Mat4f GetMatrix()
	{
		return this->m_matrix;
	}

	//! after a run() this will contains the pointer to the GGraph (which is connected)
	inline SmartPointer<Graph> GetGraph()
	{
		return this->m_g;
	}

protected:

	//! after a run() this will contain all raw triangles
	std::vector<Vec3f> m_raw_triangles;

	//! after a run() this will contain the projection matrix (which should be the identity if the vertices are already on Z=0!)
	Mat4f m_matrix;

	//! after a run() this will contains the pointer to the GGraph (which is connected)
	SmartPointer<Graph> m_g;
};


#endif //ENGINE_ENABLE_TESSELLATOR




//===========================================================
//!offline FrameBuffer (for internal use)
/*!
Esempio di utilizzo in OpenGL:

\code
	SmartPointer<Texture> tex(new Texture(1024,1024,24)); //crea una texture 1024*1024 RBG
	EngineFbo fbo(tex,true); //depth test=true

	//in your  code (outside!!! a engine.Lock()/engine.Unlock)
	if (fbo.engine->Bind()):
		fbo.Begin()
			fbo.engine.ClearScreen();
			...
		fbo.End()
		fbo.engine.Unbind()
		tex->save(filename); //adesso nella texture hai il frame buffer salvato
		tex->reset()
\endcode

\endcode
*/
//===========================================================

#if ENGINE_ENABLE_FBO

class  XGE_API EngineFbo
{
public:

	//! the engine
	Engine* engine;

	//! default constructor
	/*!
		\param tex il puntatore della texture che conterra' lo snapshot di quando disegnato
		\param bEnableDepthText enable depth test or not 
	*/
	EngineFbo(SmartPointer<Texture> tex,bool bEnableDepthText=true);

	//! destructor
	~EngineFbo();

	//! bind the frame buffer object before paint
	void Begin();

	//! unbind the frame buffer object after paint
	void End();


	//! SelfTest
	static int SelfTest();

protected:



	//! OpenGL frame buffer object
	SmartPointer<EngineResource>  framebuffer;

	//! OpenGL render buffer object
	SmartPointer<EngineResource>  renderbuffer;

	//! OpenGL depth buffer object
	SmartPointer<EngineResource>  depthBuffer;

	//! if the depth buffer is enabled
	bool bDepthText;

	//! output texture where to store the snapshot
	SmartPointer<Texture>  tex;

	//! the virtual window
	int64 Window;

	//create frame buffr and render buffer
	SmartPointer<EngineResource> createFrameBuffer();
	SmartPointer<EngineResource> createRenderBuffer(int target,int storage_type,int width,int height);

	friend class EngineShadowShader;

}; //end class

#endif //ENGINE_ENABLE_FBO






//=================================================================
//! La classe per gli shader OpenGL. Attualmente non utilizzata
/*!
	E' una classe che veniva utilizzata per le ombre e per il 
	supershader. Considerando pero' l'attuale rendering progressivo
	questa classe non viene piu' utilizzata.
*/
//=================================================================

#if ENGINE_ENABLE_SHADERS

class XGE_API EngineShader
{
public:

	SmartPointer<EngineResource> vertex_shader;
	SmartPointer<EngineResource> fragment_shader;
	SmartPointer<EngineResource> program;

	//! constructor
	EngineShader(std::string vertex_shared_filename,std::string fragment_shader_filename);

	//! bind
	void Bind(Engine* engine);
	
	//! unbind
	void Unbind (Engine* engine);

	//! self test
	static void SelfTest();


protected:

	friend class Engine;
	friend class EngineShadowShader;

	static void Set1b  (Engine* engine,int program,const char* pname,bool bvalue                );
	static void Set1i  (Engine* engine,int program,const char* pname,int ivalue                 );
	static void Set1f  (Engine* engine,int program,const char* pname,float fvalue               );
	static void Set2i  (Engine* engine,int program,const char* pname,int value1  ,int value2    );
	static void Set2f  (Engine* engine,int program,const char* pname,float value1,float value2  );
	static void Set4fv (Engine* engine,int program,const char* pname,int count,float* value     );

	static SmartPointer<EngineResource> createShader  (int target,std::string filename);
	static SmartPointer<EngineResource> createProgram (std::vector< SmartPointer<EngineResource> > shaders);

	

}; //end class

#endif //#if ENGINE_ENABLE_SHADERS

///////////////////////////////////////////////////////////////////
/*
Example:
	Engine::clearScreen();
	Engine::setFrustum(frustum);
	shadowmap->setup(this->light_dir,this->frustum,model_box)
	for (int S=0;S<shadowmap->numberOfSplits();S++))
	{
		shadowmap->begin(S);
		displayModel();
		shadowmap->end(S);
	}

	Engine::clearScreen();
	Engine::setFrustum(frustum);
	shadowmap->begin();
	displayModel();
	shadowmap->end();
*/
///////////////////////////////////////////////////////////////////

#if ENGINE_ENABLE_SHADOW_SHADER

class XGE_API EngineShadowShader
{
public:

	//constructor
	EngineShadowShader();

	//!begin
	void Setup(Engine* engine,Vec3f light_dir,Frustum* frustum,Box3f model_box);

	//return the number of splits
	inline int numberOfSplits()
		{return SHADOWMAP_NUM_SPLITS;}

	//the draw for each CurrentSplit
	void BeginSplit(Engine* engine,int S) ;
	void EndSplit  (Engine* engine,int S) ;

	//bind inside the current shader
	void Bind   (Engine* engine);
	void Unbind (Engine* engine);

	//! self test
	static void SelfTest();

protected:

	//off screen rendering
	unsigned int depth_fb,depth_rb,depth_tex;

	//! matrices
	Mat4f CM,CMI,CP,LM,LP ;
	float LM_11, LM_12, LM_13,LM_21, LM_22, LM_23,LM_31, LM_32, LM_33;

	//bounds
	float       frustum_bound [SHADOWMAP_MAX_SPLITS];
	Vec3f       frustum_min   [SHADOWMAP_MAX_SPLITS];
	Vec3f       frustum_max   [SHADOWMAP_MAX_SPLITS];


}; //end class


#endif //ENGINE_ENABLE_SHADOW_SHADER



#endif //_ENGINE_H__

