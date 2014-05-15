#ifndef _GL_CANVAS_H__
#define _GL_CANVAS_H__

#include <xge/xge.h>



///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
class GLCanvas
{
public:

  class Native;
  
  //m_fix_lighting
  bool m_fix_lighting;

  //close signal
  bool m_close;

  //redisplay signal
	bool m_redisplay;

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

  //setOctree
  virtual void setOctree(SmartPointer<Octree> octree);

	//! bind
	virtual bool bind();
  
  //swapBuffers
  virtual void  swapBuffers();

	//! unbind
	virtual bool unbind();

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
	virtual void  renderBatch (SmartPointer<Batch> batch);

  //! renderScene
	virtual void renderScene();

	//! redisplay
	virtual void redisplay()
    {frustum->refresh();m_redisplay=true;}

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

  //onTimer
  virtual void onTimer();

	// close
	virtual void close();
	
	//runLoop
	virtual void runLoop();

  //getNative
  inline Native* getNative()
    {return native;}

  //getShader
  static GLCanvas* getShared()
    {assert(gl_shared);return gl_shared.get();}

  //setShared
  static void setShared(SmartPointer<GLCanvas> value)
    {gl_shared=value;}

  //getCurrent
  static GLCanvas* getCurrent()
    {return gl_current;}


protected:

  friend class Native;
  Native* native;

  static SmartPointer<GLCanvas> gl_shared;
  static GLCanvas*              gl_current;

};


#endif //_GL_CANVAS_H__

