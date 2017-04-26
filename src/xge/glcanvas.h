#ifndef _GL_CANVAS_H__
#define _GL_CANVAS_H__

#include <xge/xge.h>

class GLCanvas;


///////////////////////////////////////////////////////////////////////////////////////////
class GLDestroyLater
{
public:

  //destroyLater
  enum Type {DestroyArrayBuffer=1,DestroyTexture=2};

  //push_back
  static void push_back(int type,unsigned int id);

  //flush
  static void flush(GLCanvas& gl);

};

///////////////////////////////////////////////////////////////////////////////////////////
class GLCanvas
{
public:

  class Pimpl;
  
  //m_fix_lighting
  bool m_fix_lighting;

	//! navigation using trackball
	bool  trackball_mode;
	Vec3f trackball_center;

	//! internal mouse position
	int  mouse_beginx,mouse_beginy;

	//if to draw lines or not
	bool draw_lines;
	bool draw_axis;

	//! current frustum
	SmartPointer<Frustum> frustum;

	//! for debugging purpouse, fix a frustum and navigate to see the frustum
	SmartPointer<Frustum> debug_frustum;

	//octree
	SmartPointer<Octree> octree;

	//enable disable progressive rendering
	bool bProgressiveRendering;

	//constructor
	GLCanvas();

	//~destructor
	virtual ~GLCanvas();

  //getShader
  static GLCanvas*& getShared()
  {static GLCanvas* ret=nullptr;return ret;;}

  //isShared
  bool isShared() const
  {return getShared()==this;}

  //getGLContext
  void* getGLContext();

  //getWorldBox
  virtual Box3f getWorldBox() {
    return octree? this->octree->world_box : Box3f(Vec3f(-1,-1,-1),Vec3f(+1,+1,+1));
  }

  //setOctree
  virtual void setOctree(SmartPointer<Octree> octree);

	//! makeCurrent
	virtual bool makeCurrent();

	//! doneCurrent
	virtual void doneCurrent();

	//!clearScreen
	virtual void  clearScreen(bool ClearColor=true,bool ClearDepth=true);

  //!setViewport
	virtual void  setViewport(int x,int y,int width,int height);

  //!setProjectionMatrix
	virtual void  setProjectionMatrix(Mat4f mat);

  //!setModelviewMatrix
	virtual void  setModelviewMatrix(Mat4f mat);

  //!setDefaultLight
	virtual void  setDefaultLight (Vec3f pos,Vec3f dir);
	
	//! pushModelviewMatrix
	virtual void pushModelviewMatrix(Mat4f mat,bool AccumulateWithCurrent=true);

  //popModelviewMatrix
	virtual void popModelviewMatrix();

  //! pushProjectionMatrix
	virtual void pushProjectionMatrix(Mat4f mat,bool AccumulateWithCurrent=true);

  //! popProjectionMatrix
	virtual void popProjectionMatrix();
	
	//! setDepthTest
	virtual void setDepthTest(bool value);

  //! setDepthWrite
	virtual void setDepthWrite(bool vaue);

  //! setPolygonMode (use Batch::POINTS,Batch::LINES or Batch::POLYGON)
	virtual void setPolygonMode(int value);

  //! setPointSize
	virtual void setPointSize(float value);

  //! setLineWidth
	virtual void setLineWidth(float value);

  //! setPolygonOffset
	virtual void setPolygonOffset(float value);

	//renderBatch
	virtual void  renderBatch(SmartPointer<Batch> batch,int first=0,int last=-1);

  //! renderOpenGL
	virtual void renderOpenGL();

	//! redisplay
	virtual void redisplay();

	//! virtuals
	virtual bool onKeyboard(int key,int x,int y) ;

  //onMouseDown
	virtual void onMouseDown(int button,int x,int y);

  //onMouseMove
	virtual void onMouseMove(int button,int x,int y);

  //onMouseUp
	virtual void onMouseUp(int button,int x,int y);

  //onMouseWheel
  virtual void onMouseWheel(int delta);

  //onResize
	virtual void onResize(int width,int height);

	// close
	virtual void close();
	
	//runLoop
	virtual void runLoop();

  //getNativeHandle
  inline Pimpl* getNativeHandle()
  {return pimpl;}

protected:

  friend class Pimpl;
  Pimpl* pimpl;

  int batch_line_width;

  //renderModel
  virtual void renderModel();

};


#endif //_GL_CANVAS_H__

