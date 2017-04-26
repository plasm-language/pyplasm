#include <xge/xge.h>
#include <xge/glcanvas.h>
#include "xge_gl.h"

#if PYPLASM_APPLE
#include <Carbon/Carbon.h>
#endif

#include <sstream>

#include <JUCE/AppConfig.h>
#include <JUCE/modules/juce_opengl/juce_opengl.h>


/////////////////////////////////////////////////////////////////////////////
class GLCanvas::Pimpl :  
  public juce::Component , 
  public juce::OpenGLRenderer,
  public juce::OpenGLContext
{
public:

  //__________________________________________________________
  class OwnedWindow : public juce::DocumentWindow
  {
  public:

    GLCanvas* owner;

    //constructor
    OwnedWindow(GLCanvas* owner_,juce::String title,juce::Colour background_color,int requiredButtons,bool addToDesktop) 
      : owner(owner_),juce::DocumentWindow(title,juce::Colours::white,requiredButtons,addToDesktop)
    {
      setUsingNativeTitleBar(true);
      setResizable (true,true);
    }

    //destructor
    virtual ~OwnedWindow()
    {}

    //closeButtonPressed
    virtual void closeButtonPressed() override
    {if (owner) owner->close();}

  };

  GLCanvas*    owner;
  OwnedWindow* owned_win;

  //constructor
  Pimpl(GLCanvas* owner_) : owned_win(nullptr),owner(owner_)
  {
    bPyPlasmMainSharedContext=owner->isShared();
    setPixelFormat(juce::OpenGLPixelFormat(8,8,16,0));

    if (owner->isShared())
    {
      owned_win=new OwnedWindow(owner,"GLShared",juce::Colours::white,false,true);
      attachTo(*owned_win);
      owned_win->setSize(256,256);
      owned_win->setVisible(true ); //force construction...
      owned_win->setVisible(false); //... but I dont'want to see it (note: I made a modification in juce_OpenGLContext.cpp to not flush it)
      assert(getRawContext()!=nullptr);
    }
    else
    {
      setWantsKeyboardFocus(true);

      setComponentPaintingEnabled(false);
      setContinuousRepainting(false);

      //sharing...
      void* raw_context=((juce::OpenGLContext*)GLCanvas::getShared()->getGLContext())->getRawContext(); assert(raw_context);
      setNativeSharedContext(raw_context);
      setRenderer(this);
      attachTo(*this);

      this->setSize(1024,768);

      owned_win=new OwnedWindow(owner,"PyPlasm",juce::Colours::azure,juce::DocumentWindow::allButtons, true);
      owned_win->setContentNonOwned(this,true);
      owned_win->centreWithSize(1024,768);
      owned_win->setVisible(true);
      
      //dont' have the bundle
      #ifdef PYPLASM_APPLE
      {
        ProcessSerialNumber psn;
        GetCurrentProcess(&psn);
        TransformProcessType(&psn, kProcessTransformToForegroundApplication);
        SetFrontProcess(&psn);
      }
      #endif
    }
    
  }

  //destructor
  virtual ~Pimpl()
  {
    detach();
    setRenderer(nullptr);
    delete owned_win;
  }

  //makeCurrent
  bool makeCurrent()
  {assert(owner->isShared());return juce::OpenGLContext::makeActive();}

  //doneCurrent
  void doneCurrent()
  {assert(owner->isShared());juce::OpenGLContext::deactivateCurrentContext();}

  //getGLContext
  juce::OpenGLContext* getGLContext()
  {return (juce::OpenGLContext*)this;}

private:

  //newOpenGLContextCreated 
  virtual void newOpenGLContextCreated() override
  {}

  //openGLContextClosing 
  virtual void openGLContextClosing() override
  {}

  //renderOpenGL 
  virtual void renderOpenGL() override
  {
    if (!owner)
      return;
    
    assert(!owner->isShared());
    
    if (!isShowing() || !isActive()) 
      return;

    GLDestroyLater::flush(*owner);

    
    {
      glEnable(GL_LIGHTING);
      glEnable(GL_POINT_SMOOTH);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_NORMALIZE);
      glShadeModel(GL_SMOOTH);
      glDepthFunc(GL_LEQUAL);
      glDisable(GL_COLOR_MATERIAL);
      glDisable(GL_CULL_FACE);
      glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
      glClearDepth(1.0f);
      glClearColor(0.3f,0.4f,0.5f, 0.00f);

      glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
      static float white[]={+1.00f,+1.00f,+1.00f,+1.00f};
      glLightfv(GL_LIGHT0,GL_AMBIENT ,white);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,white);
      glLightfv(GL_LIGHT0,GL_SPECULAR,white);
      glLightfv(GL_LIGHT0, GL_EMISSION, white);

      this->extensions.glActiveTexture(GL_TEXTURE1);
      glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S,GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T,GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER ,GL_LINEAR);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE   ,GL_MODULATE);
      glTexParameteri(GL_TEXTURE_2D  ,GL_TEXTURE_MIN_FILTER ,GL_NEAREST_MIPMAP_LINEAR);

      this->extensions.glActiveTexture(GL_TEXTURE0);
      glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S,GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T,GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER ,GL_LINEAR);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE   ,GL_MODULATE);
      glTexParameteri(GL_TEXTURE_2D  ,GL_TEXTURE_MIN_FILTER ,GL_NEAREST_MIPMAP_LINEAR);
     
      owner->renderOpenGL();
    }
  }

  //resized
  virtual void resized() override
  {
    juce::Component::resized();
    if (!owner) return;  
    owner->onResize(getWidth(),getHeight());
  }

  //visibilityChanged
  virtual void visibilityChanged() override
  {
    juce::Component::visibilityChanged();
    if (!owner) return;
    owner->onResize(getWidth(),getHeight());
  }

  //keyPressed
  virtual bool keyPressed(const juce::KeyPress& key) override
  {	
    juce::Component::keyPressed(key);
    if (!owner) return false;
    int code=key.getKeyCode();
    if (code==juce::KeyPress::leftKey)  code=Keyboard::Key_Left ;
    else if (code==juce::KeyPress::rightKey) code=Keyboard::Key_Right;
    else if (code==juce::KeyPress::upKey)    code=Keyboard::Key_Up   ;
    else if (code==juce::KeyPress::downKey)  code=Keyboard::Key_Down ; 
    owner->onKeyboard(code,this->getMouseXYRelative().getX(),this->getMouseXYRelative().getY());
    return false;
  }

  //mouseDown
  virtual void mouseDown(const juce::MouseEvent& e) override
  {
    juce::Component::mouseDown(e);
    if (!owner) return;
    int button;
    if      (e.mods.isLeftButtonDown  ()) button=1;
    else if (e.mods.isMiddleButtonDown()) button=2;
    else if (e.mods.isRightButtonDown ()) button=3;
    else return;
    owner->onMouseDown(button,e.x,e.y);
  }

  //mouseMove
  virtual void mouseMove(const juce::MouseEvent& e) override
  {
    juce::Component::mouseMove(e);
    if (!owner) return;
    owner->onMouseMove(0,e.x,e.y);
  }

  //mouseDrag
  virtual void mouseDrag(const juce::MouseEvent& e) override
  {
    juce::Component::mouseDrag(e);
    if (!owner) return;
    int button;
    if      (e.mods.isLeftButtonDown  ()) button=1;
    else if (e.mods.isMiddleButtonDown()) button=2;
    else if (e.mods.isRightButtonDown ()) button=3;
    else return;
    owner->onMouseMove(button,e.x,e.y);
  }

  //mouseUp
  virtual void mouseUp(const juce::MouseEvent& e) override
  {
    juce::Component::mouseUp(e);
    if (!owner) return;
    int button;
    if      (e.mods.isLeftButtonDown  ()) button=1;
    else if (e.mods.isMiddleButtonDown()) button=2;
    else if (e.mods.isRightButtonDown ()) button=3;
    else return;
    owner->onMouseUp(button,e.x,e.y);
  }

  //mouseWheelMove
  virtual void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& details) override
  {	
    juce::Component::mouseWheelMove(e,details);
    if (!owner) return;
    owner->onMouseWheel(details.deltaY>0?120:-120);
  }


};

/////////////////////////////////////////////////////////////////////////
static juce::CriticalSection                      destroy_later_lock;
static std::vector< std::pair<int,unsigned int> > destroy_later_v;

void GLDestroyLater::push_back(int type,unsigned int id)
{
  destroy_later_lock.enter();
  destroy_later_v.push_back(std::make_pair(type,id));
  destroy_later_lock.exit();
}

void GLDestroyLater::flush(GLCanvas& gl)
{
  juce::OpenGLContext* context=(juce::OpenGLContext*)gl.getGLContext();
  destroy_later_lock.enter();
  {
    for (int I=0;I<(int)destroy_later_v.size();I++)
    {
      int type=destroy_later_v[I].first;
      unsigned int id=destroy_later_v[I].second;
      switch(type)
      {

      case DestroyArrayBuffer:
        context->extensions.glBindBuffer(GL_ARRAY_BUFFER, id);
        context->extensions.glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
        context->extensions.glBindBuffer(GL_ARRAY_BUFFER, id);
        context->extensions.glDeleteBuffers(1, &id);
        break;

      case DestroyTexture:
        glDeleteTextures(1,&id);
        break;
      }
    }
    destroy_later_v.clear();
  }
  destroy_later_lock.exit();
}

///////////////////////////////////////////////////////////////////////////////////////////
GLCanvas::GLCanvas() : pimpl(nullptr)
{
  if (!getShared())
    getShared()=this;

  this->bProgressiveRendering = true;
  this->draw_lines            = false;
  this->draw_axis             = true;
  this->mouse_beginx          = 0;
  this->mouse_beginy          = 0;
  this->trackball_mode        = true;
  this->trackball_center      = Vec3f(0,0,0);
  this->frustum               = SmartPointer<Frustum>(new Frustum());
  this->m_fix_lighting        = false;
  this->batch_line_width      = 1;

  this->pimpl=new Pimpl(this);

  this->frustum->guessBestPosition(Box3f(Vec3f(-1,-1,-1),Vec3f(+1,+1,+1)));
}


/////////////////////////////////////////////////////////////////////////////////////
GLCanvas::~GLCanvas()
{delete pimpl;}

/////////////////////////////////////////////////////////////////////////
void* GLCanvas::getGLContext()
{return pimpl? pimpl->getGLContext() : nullptr;}


/////////////////////////////////////////////////////////////////////////////////////
void GLCanvas::redisplay()
{
  frustum->refresh();
  if (pimpl) pimpl->triggerRepaint();
}

/////////////////////////////////////////////////////////////////////////////////////
void GLCanvas::setOctree(SmartPointer<Octree> octree)
{
  this->octree=octree;
  Box3f world_box=getWorldBox();
	this->trackball_center =world_box.center();
	this->frustum->guessBestPosition(world_box);
  this->redisplay();
}

///////////////////////////////////////////////////////////////////////////////////////////
bool GLCanvas::makeCurrent()
{return pimpl->makeCurrent();}

///////////////////////////////////////////////////////////////////////////////////////////
void GLCanvas::doneCurrent()
{pimpl->doneCurrent();}


///////////////////////////////////////////////////////////////////////////////////////////
#if 1

static bool bExitRunLoop=false;

void GLCanvas::runLoop()
{
  bExitRunLoop=false;

  while (!bExitRunLoop)
    juce::MessageManager::getInstance()->runDispatchLoopUntil(200);
}

void GLCanvas::close()
{bExitRunLoop=true;}

#else //this version does not work on linux

void GLCanvas::runLoop()
{
  juce::MessageManager::getInstance()->runDispatchLoop();
  juce::MessageManager::getInstance()->hasStopMessageBeenSent()=false;
}

void GLCanvas::close()
{
  try
  {juce::MessageManager::getInstance()->stopDispatchLoop();}
  catch(...){}
}

#endif 

////////////////////////////////////////////////////////////
void GLCanvas::setViewport(int x,int y,int width,int height)
{
  double scale=pimpl->getRenderingScale();
  glViewport((int)(scale*x),(int)(scale*y),(int)(scale*width),(int)(scale*height));
}

////////////////////////////////////////////////////////////
void GLCanvas::setProjectionMatrix(Mat4f mat)
{
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(mat.transpose().mat);
  glMatrixMode( GL_MODELVIEW );
}

////////////////////////////////////////////////////////////
void GLCanvas::setModelviewMatrix(Mat4f mat)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(mat.transpose().mat);
}


////////////////////////////////////////////////////////////
void GLCanvas::clearScreen(bool ClearColor,bool ClearDepth)
{glClear((ClearColor?GL_COLOR_BUFFER_BIT:0) | (ClearDepth?GL_DEPTH_BUFFER_BIT:0));}

////////////////////////////////////////////////////////////
void GLCanvas::setDefaultLight(Vec3f pos,Vec3f dir)
{
  glEnable(GL_LIGHTING);
  
  //problem of triangle winding wrong
  if (m_fix_lighting)
  {
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
    float ambient [4] ={ 0.3f, 0.3f, 0.3f, 0.3f };
    float diffuse [4] ={ 0.8f, 0.8f, 0.8f, 0.8f };
    float specular[4] ={ 0.0f, 0.0f, 0.0f, 0.0f };
    float emission[4] ={ 0.0f, 0.0f, 0.0f, 0.0f };
    
    float light_pos0[4]={+dir.x, +dir.y, +dir.z,0};
    float light_pos1[4]={-dir.x, -dir.y, -dir.z,0};
  
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_EMISSION, emission);
  
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT1, GL_EMISSION, emission);
  }
  //default lighting
  else
  {
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
    float white[4] ={ 1, 1, 1, 1 };
    float light_pos[4]={pos.x, pos.y, pos.z,1};
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, white);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glLightfv(GL_LIGHT0, GL_EMISSION, white);
    glDisable(GL_LIGHT1);
  }
}


//////////////////////////////////////////////////////////////////
void GLCanvas::pushModelviewMatrix(Mat4f mat,bool Accumulate)
{
  glPushMatrix();

  if (Accumulate)
    glMultMatrixf(mat.transpose().mat);
  else
    glLoadMatrixf(mat.transpose().mat);
}


//////////////////////////////////////////////////////////////////
void GLCanvas::popModelviewMatrix()
{
  glPopMatrix();
}

//////////////////////////////////////////////////////////////////
void GLCanvas::pushProjectionMatrix(Mat4f mat,bool Accumulate)
{
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();

  if (Accumulate)
    glMultMatrixf(mat.transpose().mat);
  else
    glLoadMatrixf(mat.transpose().mat);

  glMatrixMode(GL_MODELVIEW);	
}


//////////////////////////////////////////////////////////////////
void GLCanvas::popProjectionMatrix()
{
  glMatrixMode(GL_PROJECTION);	
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);	
}

//////////////////////////////////////////////////////////////////
void GLCanvas::setDepthTest(bool value)
{
  if (value)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);
}

//////////////////////////////////////////////////////////////////
void GLCanvas::setDepthWrite(bool value)
{
  if (value)
    glDepthMask(true);
  else
    glDepthMask(false);	

}


//////////////////////////////////////////////////////////////////
void GLCanvas::setPolygonMode(int value)
{
  switch(value)
  {
  case Batch::POINTS:
    glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
    break;
  case Batch::LINES:	
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    break;
  default:
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    break;
  }	

}

////////////////////////////////////////////////////////////
void GLCanvas::setPointSize(float value)
{
  glPointSize(value);
}

////////////////////////////////////////////////////////////
void GLCanvas::setLineWidth(float value)
{
  glLineWidth(value);
}


////////////////////////////////////////////////////////////
void GLCanvas::setPolygonOffset(float value)
{
  if (value)
  {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset( 1.0f, value);	
  }
  else
  {
    glDisable(GL_POLYGON_OFFSET_FILL);
  }
}

////////////////////////////////////////////////////////////
bool GLCanvas::onKeyboard(int key,int x,int y)
{
  switch(key)
  {
  case 27:
    {
      this->close();
      return true;
    }

  case '+':case '=':
    this->frustum->walk_speed*=0.95f;
    return true;

  case '-':case '_':
    this->frustum->walk_speed*=(1.0f/0.95f);
    return true;

  case 'X':
    this->draw_axis=!this->draw_axis;
    this->redisplay();
    return true;

  case 'l':case 'L':
    this->draw_lines=!this->draw_lines;
    this->redisplay();
    return true;

  case 'v':case 'V':
    this->batch_line_width=std::max(0,this->batch_line_width-1);
    this->redisplay();
    return true;

  case 'b':case 'B':
    this->batch_line_width=std::max(0,this->batch_line_width+1);
    this->redisplay();
    return true;


  case 'p': case 'P':
  {
	  std::cout<<"def MYVIEW(obj):"<<std::endl;
    std::cout<<"  glcanvas=GLCanvas()"<<std::endl;
    std::cout<<"  glcanvas.setOctree(Octree(Plasm.getBatches(obj)))"<<std::endl;
    std::cout<<"  glcanvas.frustum.pos="<<frustum->pos.repr()<<std::endl;
    std::cout<<"  glcanvas.frustum.dir="<<frustum->dir.repr()<<std::endl;
    std::cout<<"  glcanvas.frustum.vup="<<frustum->vup.repr()<<std::endl;
    std::cout<<"  glcanvas.redisplay()"<<std::endl;
	  std::cout<<"  glcanvas.runLoop()"<<std::endl;
    std::cout<<""<<std::endl;
    return true;
  }

  case 'F':case 'f':
    {
      if (!debug_frustum)
        debug_frustum=SmartPointer<Frustum>(new Frustum(*this->frustum));
      else
      {
        debug_frustum.reset();
      }

      this->redisplay();
      return true;
    }
  case ' ':
    {
      trackball_mode=!trackball_mode;
      if (!trackball_mode) this->frustum->fixVup();
      this->redisplay();
      return true;
    }

  //disable progression
  case 'r':
    bProgressiveRendering=!bProgressiveRendering;
    this->redisplay();
    return true;

  case 'a':
  case 'A':
  case Keyboard::Key_Left :
    frustum->pos-=frustum->right*frustum->walk_speed;
    this->redisplay();
    return true;

  case 'd':
  case 'D':
  case Keyboard::Key_Right:
    frustum->pos+=frustum->right*frustum->walk_speed;
    this->redisplay();
    return true;
      
  case 't':
  case 'T':
      m_fix_lighting=!m_fix_lighting;
      this->redisplay();
      return true;

  case 'w':
  case 'W':
    {
      Ray3f ray=frustum->unproject(x,y);
      frustum->pos+=ray.dir*frustum->walk_speed;
      //frustum->pos+=frustum->dir*frustum->walk_speed;
      this->redisplay();
      return true;
    }

  case 's':
  case 'S':
    {
      Ray3f ray=frustum->unproject(x,y);
      frustum->pos-=ray.dir*frustum->walk_speed;
      this->redisplay();
      return true;
    }

  case Keyboard::Key_Up   :
    frustum->pos+=frustum->vup*frustum->walk_speed;
    this->redisplay();
    return true;

  case Keyboard::Key_Down :
    frustum->pos-=frustum->vup*frustum->walk_speed;
    this->redisplay();
    return true;
  }


  return false;
}


///////////////////////////////////////////////////////////////
void GLCanvas::onMouseDown(int button,int x,int y)
{
  mouse_beginx = x;
  mouse_beginy = y;
}


///////////////////////////////////////////////////////////////
void GLCanvas::onMouseMove(int button,int x,int y)
{
  if (button)
  {
    if (!trackball_mode)
    {
	    float dx=0.5*M_PI*(x-mouse_beginx)/(float)frustum->width;
	    float dy=0.5*M_PI*(y-mouse_beginy)/(float)frustum->height;

	    if (button==1)
	    {
		    Vec3f dir=frustum->dir.rotate(frustum->vup,-dx);
		    Vec3f rot_axis=dir.cross(frustum->vup).normalize();
		    dir=dir.rotate(rot_axis,-dy).normalize();
		    frustum->dir=dir;
	    }
	    else if (button==2)
	    {
		    frustum->pos=frustum->pos + frustum->vup*-dy*frustum->walk_speed*10 + frustum->right*dx*frustum->walk_speed*10;
	    }

      this->redisplay();
    }
    else
    {
	    Mat4f vmat=Mat4f::lookat(
        frustum->pos.x,frustum->pos.y,frustum->pos.z,
        frustum->pos.x+frustum->dir.x,frustum->pos.y+frustum->dir.y,frustum->pos.z+frustum->dir.z,
        frustum->vup.x,frustum->vup.y,frustum->vup.z) * Mat4f::translate(+1*trackball_center);

	    Quaternion rotation=Quaternion(vmat);
	    Vec3f translate=Vec3f(vmat[3],vmat[7],vmat[11])-trackball_center;

	    float deltax = (float)(x - mouse_beginx);   
	    float deltay = (float)(mouse_beginy - y);
	    int W=frustum->width;
	    int H=frustum->height;

	    if (button==1)
	    {
		    float Min = (float)(W < H ? W : H)*0.5f;
		    Vec3f offset(W/2.f, H/2.f, 0);
		    Vec3f a=(Vec3f((float)(mouse_beginx), (float)(H-mouse_beginy), 0)-offset)/Min;
		    Vec3f b=(Vec3f((float)(           x), (float)(H-           y), 0)-offset)/Min;
		    a.set(2, pow(2.0f, -0.5f * a.module()));
		    b.set(2, pow(2.0f, -0.5f * b.module()));
		    a = a.normalize();
		    b = b.normalize();
		    Vec3f axis = a.cross(b).normalize();
		    float angle = acos(a*b);
		    const float TRACKBALLSCALE=1.0f;
		    rotation = Quaternion(axis, angle * TRACKBALLSCALE) * rotation;
	    }
	    else if (button==2)
	    {
		    translate -= Vec3f(0,0,deltay) * frustum->walk_speed;

	    }
	    else if (button==3) 
	    {
		    translate += Vec3f(deltax, deltay, 0) * frustum->walk_speed;
	    }

	    vmat=Mat4f::translate(translate) * 
        Mat4f::translate(+1*trackball_center)* 
        Mat4f::rotate(rotation.getAxis(),rotation.getAngle())* 
        Mat4f::translate(-1*trackball_center);

	    vmat=vmat.invert();	
	    frustum->pos=Vec3f(  vmat[3], vmat[7], vmat[11]);
	    frustum->dir=Vec3f( -vmat[2],-vmat[6],-vmat[10]);
	    frustum->vup=Vec3f(  vmat[1], vmat[5], vmat[ 9]);

      this->redisplay();
    }
  }

  mouse_beginx = x;
  mouse_beginy = y;
}


///////////////////////////////////////////////////////////////
void GLCanvas::onMouseUp(int button,int x,int y)
{
  mouse_beginx = x;
  mouse_beginy = y;
}


///////////////////////////////////////////////////////////////
void GLCanvas::onMouseWheel(int delta)
{
  frustum->pos+=(delta>0?+1:-1) * frustum->dir * frustum->walk_speed;
  this->redisplay();
}



///////////////////////////////////////////////////////////////////////////////
void GLCanvas::onResize(int width,int height)
{
  if (!width || ! height)
    return;

  frustum->x=0;
  frustum->y=0;
  frustum->width=width;
  frustum->height=height;

  float zNear=0.001f;
  float zFar=1000;

  Box3f world_box=getWorldBox();
  float maxdim=world_box.maxsize();;
  zNear = maxdim / 50.0f ;
  zFar  = maxdim * 10;

  frustum->projection_matrix=Mat4f::perspective(DEFAULT_FOV,width/(float)height,zNear,zFar);
  this->redisplay();
}


///////////////////////////////////////////////////////////////
void GLCanvas::renderBatch(SmartPointer<Batch> _batch,int first,int last)
{
  if (!_batch)
    return;

  juce::OpenGLContext* context=pimpl->getGLContext();

  //reset Error
  glGetError();

  Batch& batch=(*_batch);
  XgeReleaseAssert(batch.primitive>=0 && batch.vertices);

  //material
  if (!batch.colors)
  {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , &batch.ambient.r );
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , &batch.diffuse.r );
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , &batch.specular.r);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION , &batch.emission.r);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS,  batch.shininess );

    if (batch.diffuse[3]<1)
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
  }

  //vertices
  if (batch.vertices)
  {
    batch.vertices->uploadIfNeeded(*this);
    context->extensions.glBindBuffer(GL_ARRAY_BUFFER, batch.vertices->gpu->id);
    glVertexPointer(3, GL_FLOAT, 0, nullptr);
    context->extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableClientState(GL_VERTEX_ARRAY);
  }

  //normals
  if (batch.normals)
  {
    batch.normals->uploadIfNeeded(*this);
    context->extensions.glBindBuffer(GL_ARRAY_BUFFER, batch.normals->gpu->id);
    glNormalPointer (GL_FLOAT, 0, nullptr);
    context->extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableClientState(GL_NORMAL_ARRAY);
  }

  //colors
  if (batch.colors)
  {
    batch.colors->uploadIfNeeded(*this);
    context->extensions.glBindBuffer(GL_ARRAY_BUFFER, batch.colors->gpu->id);
    glColorPointer(3,GL_FLOAT, 0, nullptr);
    context->extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_COLOR_MATERIAL);
  }

  //texture0 (seems to be affected by the current color, since I normally use materials I do not care really)
  if (batch.texture0 && batch.texture0coords)
  {
    batch.texture0->uploadIfNeeded(*this);
    batch.texture0coords->uploadIfNeeded(*this);
    glColor4f(1,1,1,1);
    context->extensions.glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, batch.texture0->gpu->id);
    glEnable(GL_TEXTURE_2D);
    context->extensions.glBindBuffer(GL_ARRAY_BUFFER, batch.texture0coords->gpu->id);
    glTexCoordPointer(2, GL_FLOAT, 0,  nullptr);
    context->extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  //texture1 (pre-lighting is calculated I do not need OpenGL lighting)
  if (batch.texture1 && batch.texture1coords)
  {
    batch.texture1->uploadIfNeeded(*this);
    batch.texture1coords->uploadIfNeeded(*this);
    glDisable(GL_LIGHTING);
    glColor3f(1,1,1);
    context->extensions.glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, batch.texture1->gpu->id);
    glEnable(GL_TEXTURE_2D);
    context->extensions.glBindBuffer(GL_ARRAY_BUFFER, batch.texture1coords->gpu->id);
    glTexCoordPointer (2, GL_FLOAT, 0,  nullptr);
    context->extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    context->extensions.glActiveTexture(GL_TEXTURE0);
  }

  //draw the primitives
  glPushMatrix();
  {
    const float* _mat=batch.matrix.mat;
    float _m[16] ={
      _mat[0],_mat[4],_mat[ 8],_mat[12],
      _mat[1],_mat[5],_mat[ 9],_mat[13],
      _mat[2],_mat[6],_mat[10],_mat[14],
      _mat[3],_mat[7],_mat[11],_mat[15]
    };
    glMultMatrixf(_m);

    int num_vertices=batch.vertices->size()/3;

    if (last==-1)
      last=num_vertices;

    setLineWidth(batch_line_width);
    glDrawArrays(batch.primitive, first, last);
    setLineWidth(1);
  }
  glPopMatrix();

  if (batch.diffuse[3]<1) 
  {
    glDisable(GL_BLEND);
  }

  //important to do do in reverse order
  if (batch.texture1 && batch.texture1coords)
  {
    context->extensions.glActiveTexture(GL_TEXTURE1);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    context->extensions.glActiveTexture(GL_TEXTURE0);
  }

  if (batch.texture0 && batch.texture0coords)
  {
    context->extensions.glActiveTexture(GL_TEXTURE0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
  }

  if (batch.colors) 
  {
    glDisableClientState(GL_COLOR_ARRAY);
    glDisable(GL_COLOR_MATERIAL);
  }

  if (batch.normals) 
  {
    glDisableClientState(GL_NORMAL_ARRAY);
  }

  if (batch.vertices) 
  {
    glDisableClientState(GL_VERTEX_ARRAY);
  }
}

///////////////////////////////////////////////////////////////////////////////
void GLCanvas::renderModel()
{
  if (!this->octree)
    return;

  std::vector<SmartPointer<Batch> > transparent;
  SmartPointer<Frustum> frustum=debug_frustum?debug_frustum:this->frustum;
  FrustumIterator it_frustum=this->octree->find(frustum);
  Clock t1;
  bool bQuitRenderingLoop=false;
  for (;!bQuitRenderingLoop && !it_frustum.end();it_frustum.moveNext())
  {
    OctreeNode* node=it_frustum.getNode();
    std::vector<SmartPointer<Batch> >& v=node->batches;

    for (int i=0;!bQuitRenderingLoop && i<(int)v.size();i++) 
    {
      if (frustum->intersect(v[i]->getBox()))
      {
        if (v[i]->diffuse.a<1)
        {
          transparent.push_back(v[i]);
        }
        else
        {
          renderBatch(v[i]);

          if (this->draw_lines && v[i]->primitive>=Batch::TRIANGLES)
          {
            setDepthWrite(false);
            setLineWidth(batch_line_width);
            setPolygonMode(Batch::LINES);
            Color4f ambient=v[i]->ambient;
            Color4f diffuse=v[i]->diffuse;
            v[i]->setColor(Color4f(0,0,0,0.05f));
            renderBatch(v[i]);
            v[i]->ambient=ambient;
            v[i]->diffuse=diffuse;
            setDepthWrite(true);
            setPolygonMode(Batch::POLYGON);
            setLineWidth(1);
          }
        }
      }	
    }
  }

  //draw transparent object in reverse order
  for (int i=(transparent.size()-1);!bQuitRenderingLoop && i>=0 ;i--)
    renderBatch(transparent[i]);

}

///////////////////////////////////////////////////////////////////////////////
void GLCanvas::renderOpenGL()
{
  clearScreen();
  setViewport(frustum->x,frustum->y,frustum->width,frustum->height);
  setProjectionMatrix(frustum->projection_matrix);
  setModelviewMatrix(frustum->getModelviewMatrix());
  setDefaultLight(this->frustum->pos,this->frustum->dir);

  //debug_frustum mode
  if (debug_frustum) 
    debug_frustum->render(this);

  //reference axix
  if (this->draw_axis)
  {
    float X=1;
    float Y=1;
    float Z=1;
    SmartPointer<Batch> reference(new Batch);
    reference->primitive=Batch::LINES;
    static float _vertices[]={0,0,0, X,0,0,   0,0,0, 0,Y,0,  0,0,0, 0,0,Z};reference->vertices .reset(new Array(6*3,_vertices));
    static float _colors  []={1,0,0, 1,0,0,   0,1,0, 0,1,0,  0,0,1, 0,0,1};reference->colors   .reset(new Array(6*3,_colors  ));
    setLineWidth(3);
    renderBatch(reference);
    setLineWidth(1);
  }

  renderModel();
}


