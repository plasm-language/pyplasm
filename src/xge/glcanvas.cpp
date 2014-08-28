#include <xge/xge.h>
#include <xge/glcanvas.h>

#if PYPLASM_APPLE
#include <Carbon/Carbon.h>
#endif

#define DONT_SET_USING_JUCE_NAMESPACE 1
#include <juce_2_0/juce.h>

#if PYPLASM_APPLE
#include <OpenGL/GL.h>
#include <OpenGL/GLU.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

//missing OpenGL definitions
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE        0x812f
#endif

#ifndef GL_NUM_EXTENSIONS
#define GL_NUM_EXTENSIONS       0x821d
#endif

#ifndef GL_BGRA_EXT
#define GL_BGRA_EXT             0x80e1
#endif

#ifndef GL_DEPTH24_STENCIL8
#define GL_DEPTH24_STENCIL8     0x88F0
#endif

#ifndef GL_RGBA8
#define GL_RGBA8                GL_RGBA
#endif


#ifndef GL_COLOR_ATTACHMENT0
#define GL_COLOR_ATTACHMENT0    0x8CE0
#endif

#ifndef GL_DEPTH_ATTACHMENT
#define GL_DEPTH_ATTACHMENT     0x8D00
#endif

#ifndef GL_FRAMEBUFFER
#define GL_FRAMEBUFFER          0x8D40
#endif

#ifndef GL_FRAMEBUFFER_BINDING
#define GL_FRAMEBUFFER_BINDING  0x8CA6
#endif

#ifndef GL_FRAMEBUFFER_COMPLETE
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#endif

#ifndef GL_RENDERBUFFER
#define GL_RENDERBUFFER         0x8D41
#endif

#ifndef GL_RENDERBUFFER_DEPTH_SIZE
#define GL_RENDERBUFFER_DEPTH_SIZE  0x8D54
#endif

#ifndef GL_STENCIL_ATTACHMENT
#define GL_STENCIL_ATTACHMENT   0x8D20
#endif

#ifndef GL_WRITE_ONLY
#define GL_WRITE_ONLY                 0x88B9
#endif

#if PYPLASM_WINDOWS
enum
{
  GL_OPERAND0_RGB                 = 0x8590,
  GL_OPERAND1_RGB                 = 0x8591,
  GL_OPERAND0_ALPHA               = 0x8598,
  GL_OPERAND1_ALPHA               = 0x8599,
  GL_SRC0_RGB                     = 0x8580,
  GL_SRC1_RGB                     = 0x8581,
  GL_SRC0_ALPHA                   = 0x8588,
  GL_SRC1_ALPHA                   = 0x8589,
  GL_TEXTURE0                     = 0x84C0,
  GL_TEXTURE1                     = 0x84C1,
  GL_TEXTURE2                     = 0x84C2,
  GL_COMBINE                      = 0x8570,
  GL_COMBINE_RGB                  = 0x8571,
  GL_COMBINE_ALPHA                = 0x8572,
  GL_PREVIOUS                     = 0x8578,
  GL_COMPILE_STATUS               = 0x8B81,
  GL_LINK_STATUS                  = 0x8B82,
  GL_SHADING_LANGUAGE_VERSION     = 0x8B8C,
  GL_FRAGMENT_SHADER              = 0x8B30,
  GL_VERTEX_SHADER                = 0x8B31,
  GL_ARRAY_BUFFER                 = 0x8892,
  GL_ELEMENT_ARRAY_BUFFER         = 0x8893,
  GL_STATIC_DRAW                  = 0x88E4,
  GL_DYNAMIC_DRAW                 = 0x88E8
};
#endif


SmartPointer<GLCanvas> GLCanvas::gl_shared;
GLCanvas*              GLCanvas::gl_current=0;


//////////////////////////////////////////////////////////////////////////////////////////////////////
class GLCanvas::Native : 
  public juce::DocumentWindow,
  public juce::Timer
{
protected:

 
  //GLComponent
  class GLComponent  : public juce::OpenGLComponent
  {
  public:

    typedef juce::OpenGLComponent JuceClass;

    GLCanvas*             owner;

    //constructor
    GLComponent() :owner(0)
    {
      setPixelFormat(juce::OpenGLPixelFormat(8,8,16,0));
      if (gl_shared)
      {
        this->setWantsKeyboardFocus(true);
        this->shareWith(gl_shared->getNative()->getContext());
        this->updateContext();
      } 
      setSize(1,1);
    }

    //destructor
    ~GLComponent()
      {}

    //newOpenGLContextCreated
    virtual void newOpenGLContextCreated()
    {
      juce::OpenGLContext* context=this->getContext();assert(context);

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

      context->extensions.glActiveTexture       (GL_TEXTURE1);
      context->extensions.glClientActiveTexture (GL_TEXTURE1);
      glTexParameteri       (GL_TEXTURE_2D , GL_TEXTURE_WRAP_S     ,GL_REPEAT);
      glTexParameteri       (GL_TEXTURE_2D , GL_TEXTURE_WRAP_T     ,GL_REPEAT);
      glTexParameterf       (GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER ,GL_LINEAR);
      glTexEnvf             (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE   ,GL_MODULATE);
      glTexParameteri       (GL_TEXTURE_2D  ,GL_TEXTURE_MIN_FILTER ,GL_NEAREST_MIPMAP_LINEAR);

      context->extensions.glActiveTexture       (GL_TEXTURE0);
      context->extensions.glClientActiveTexture (GL_TEXTURE0);
      glTexParameteri       (GL_TEXTURE_2D , GL_TEXTURE_WRAP_S     ,GL_REPEAT);
      glTexParameteri       (GL_TEXTURE_2D , GL_TEXTURE_WRAP_T     ,GL_REPEAT);
      glTexParameterf       (GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER ,GL_LINEAR);
      glTexEnvf             (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE   ,GL_MODULATE);
      glTexParameteri       (GL_TEXTURE_2D  ,GL_TEXTURE_MIN_FILTER ,GL_NEAREST_MIPMAP_LINEAR);
    }

    //resized
    virtual void resized()
    {
      JuceClass::resized();
      if (!owner) return;  
      owner->onResize(getWidth(),getHeight());
    }

    //visibilityChanged
    virtual void visibilityChanged()
    {
      JuceClass::visibilityChanged();
      if (!owner) return;
      owner->onResize(getWidth(),getHeight());
    }

    //keyPressed
    virtual bool keyPressed(const juce::KeyPress& key)
    {	
      JuceClass::keyPressed(key);
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
    virtual void mouseDown(const juce::MouseEvent& e)
    {
      JuceClass::mouseDown(e);
      if (!owner) return;
      int button;
      if      (e.mods.isLeftButtonDown  ()) button=1;
      else if (e.mods.isMiddleButtonDown()) button=2;
      else if (e.mods.isRightButtonDown ()) button=3;
      else return;
      owner->onMouseDown(button,e.x,e.y);
    }

    //mouseMove
    virtual void mouseMove(const juce::MouseEvent& e)
    {
      JuceClass::mouseMove(e);
      if (!owner) return;
      owner->onMouseMove(0,e.x,e.y);
    }

    //mouseDrag
    virtual void mouseDrag(const juce::MouseEvent& e)
    {
      JuceClass::mouseDrag(e);
      if (!owner) return;
      int button;
      if      (e.mods.isLeftButtonDown  ()) button=1;
      else if (e.mods.isMiddleButtonDown()) button=2;
      else if (e.mods.isRightButtonDown ()) button=3;
      else return;
      owner->onMouseMove(button,e.x,e.y);
    }

    //mouseUp
    virtual void mouseUp(const juce::MouseEvent& e)
    {
      JuceClass::mouseUp(e);
      if (!owner) return;
      int button;
      if      (e.mods.isLeftButtonDown  ()) button=1;
      else if (e.mods.isMiddleButtonDown()) button=2;
      else if (e.mods.isRightButtonDown ()) button=3;
      else return;
      owner->onMouseUp(button,e.x,e.y);
    }

    //mouseWheelMove
    virtual void mouseWheelMove(const juce::MouseEvent& e, float wheelIncrementX,float wheelIncrementY)
    {	
      JuceClass::mouseWheelMove(e,wheelIncrementX,wheelIncrementY);
      if (!owner) return;
      owner->onMouseWheel(wheelIncrementY>0?120:-120);
    }

    //renderAndSwapBuffers
    virtual bool renderAndSwapBuffers()
      {return true;}

    //renderOpenGL
    virtual void renderOpenGL()
      {}

  };

  GLCanvas*    owner;

  //closeButtonPressed
  virtual void closeButtonPressed()
    {if (owner) owner->close();}

  //timerCallback
  virtual void timerCallback()
    {if (owner) owner->onTimer();}

public:

  //constructor
  Native(GLCanvas* owner) : owner(0) ,juce::DocumentWindow(gl_shared?"PyPlasm":"gl_shared",juce::Colours::azure,juce::DocumentWindow::allButtons, true) 
  {
    centreWithSize(1, 1);
    setUsingNativeTitleBar(true);
    setResizable (true,true);
    GLComponent* glcomponent=new GLComponent();
    setContentOwned(glcomponent,true);
    setVisible(true);

    if (!gl_shared)
    {
      glcomponent->updateContext(); //force creation
      setVisible(false);      //avoid destruction
    }
    
    //dont' have the bundle
    #ifdef PYPLASM_APPLE
    {
      ProcessSerialNumber psn;
      GetCurrentProcess(&psn);
      TransformProcessType(&psn, kProcessTransformToForegroundApplication);
      SetFrontProcess(&psn);
    }
    #endif


    startTimer(30);

    //now I unblock the events
    this       ->owner = owner;
    glcomponent->owner = owner;
  }

  //destructor
  ~Native()
    {}

  //getContext
  inline juce::OpenGLContext* getContext()
    {return dynamic_cast<GLComponent*>(this->getContentComponent())->getContext();}

  //bind
  inline bool bind()
  {
    assert(!gl_current);
    if (!owner) return false;
    GLComponent* glcomponent=dynamic_cast<GLComponent*>(this->getContentComponent());
    glcomponent->updateContext();
    glcomponent->getContextLock().enter();
    bool bOk=glcomponent->makeCurrentContextActive();
    if (!bOk) {glcomponent->getContextLock().exit(); return false;}
    gl_current=owner;
    return true;
  }

  //unbind
  inline bool unbind()
  {
    assert(gl_current);
    if (!owner) return false;
    GLComponent* glcomponent=dynamic_cast<GLComponent*>(this->getContentComponent());
    glcomponent->makeCurrentContextInactive();
    glcomponent->getContextLock().exit();
    gl_current=0;
    return true;
  }

  //swapBuffers
  inline void swapBuffers()
  {
    GLComponent* glcomponent=dynamic_cast<GLComponent*>(this->getContentComponent());
    glcomponent->swapBuffers();
  }

   //NeedContext
  class NeedContext
  {
  public:

    juce::OpenGLContext* context;
    bool                 bUnbindShared;

    //constructor
    inline NeedContext() : bUnbindShared(false)
    {
      GLCanvas* glcanvas=GLCanvas::getCurrent();
      if (!glcanvas)
      {
        bUnbindShared=true;
        glcanvas=GLCanvas::getShared();
        glcanvas->bind();
      }
      this->context=glcanvas->getNative()->getContext();;
    }

    //destructor
    inline ~NeedContext()
    {
      if (bUnbindShared)
        GLCanvas::getShared()->unbind();
    }

    //getContext
    inline juce::OpenGLContext* getContext()
      {return this->context;}

  };


  //createArrayBuffer
  static void createArrayBuffer(SmartPointer<Array> array)
  {
    if (array->gpu)
      return;

    GLCanvas::Native::NeedContext bind_context;
    juce::OpenGLContext* context=bind_context.getContext();

    //can happen that meanwhile another thread has built the gpu
    int   size=array->memsize();
    void* data=(float*)array->c_ptr();

    GLuint bufferid;	
    context->extensions.glGenBuffers(1,&bufferid);XgeReleaseAssert(bufferid);
    context->extensions.glBindBuffer(GL_ARRAY_BUFFER, bufferid);
    context->extensions.glBufferData(GL_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW);

    context->extensions.glBindBuffer(GL_ARRAY_BUFFER, bufferid);
    void* gpu_data = context->extensions.glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);XgeReleaseAssert(gpu_data);
    memcpy(gpu_data,data,size);
    context->extensions.glUnmapBuffer(GL_ARRAY_BUFFER);
    context->extensions.glBindBuffer(GL_ARRAY_BUFFER,0);

    array->gpu=SmartPointer<Array::Gpu>(new Array::Gpu(bufferid));
  }


  //createTexture
  static void createTexture(SmartPointer<Texture> texture)
  {
    if (texture->gpu)
      return;

    GLCanvas::Native::NeedContext bind_context;
    juce::OpenGLContext* context=bind_context.getContext();

    unsigned int texid;
    glGenTextures(1,&texid);XgeReleaseAssert(texid); 
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glBindTexture (GL_TEXTURE_2D, texid);
    float maxsize;
    glGetFloatv(GL_MAX_TEXTURE_SIZE,&maxsize); 
    XgeDebugAssert (texture->width<=maxsize && texture->height<=maxsize);

    unsigned int format=(texture->bpp==24)?GL_RGB:(texture->bpp==32?GL_RGBA:GL_LUMINANCE);
    unsigned int type=GL_UNSIGNED_BYTE;

    gluBuild2DMipmaps(GL_TEXTURE_2D,texture->bpp/8,texture->width, texture->height,format, type, texture->buffer);

    texture->gpu=SmartPointer<Texture::Gpu>(new Texture::Gpu(texid));	
  }

  //destroyArrayGpu
  static void destroyArrayGpu(unsigned int id)
  {
    GLCanvas::Native::NeedContext bind_context;
    juce::OpenGLContext* context=bind_context.getContext();
    context->extensions.glBindBuffer(GL_ARRAY_BUFFER, id);
    context->extensions.glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
    context->extensions.glBindBuffer(GL_ARRAY_BUFFER, id);
    context->extensions.glDeleteBuffers(1, &id);
  }

  //destroyTextureGpu
  static void destroyTextureGpu(unsigned int id)
  {
    GLCanvas::Native::NeedContext bind_context;
    juce::OpenGLContext* context=bind_context.getContext();
    glDeleteTextures(1,&id);
  }

};



/////////////////////////////////////////////////////////////////////////////////////
Array::Gpu::~Gpu()
{
  GLCanvas::Native::destroyArrayGpu(id);
}

/////////////////////////////////////////////////////////////////////////////////////
Texture::Gpu::~Gpu()
{
  GLCanvas::Native::destroyTextureGpu(id);
}

///////////////////////////////////////////////////////////////////////////////////////////
GLCanvas::GLCanvas() : native(0)
{
  this->native=new Native(this);

  this->bProgressiveRendering = true;
  this->m_close               = false;
  this->m_redisplay           = true;
  this->draw_lines            = false;
  this->draw_axis             = true;
  this->mouse_beginx          = 0;
  this->mouse_beginy          = 0;
  this->trackball_mode        = true;
  this->trackball_center      = Vec3f(0,0,0);
  this->frustum               = SmartPointer<Frustum>(new Frustum());
  this->m_fix_lighting        = false;

  this->frustum->guessBestPosition(Box3f(Vec3f(-1,-1,-1),Vec3f(+1,+1,+1)));

  native->centreWithSize(1024,768);
}


/////////////////////////////////////////////////////////////////////////////////////
GLCanvas::~GLCanvas()
{
  delete native;
}

/////////////////////////////////////////////////////////////////////////////////////
void GLCanvas::setOctree(SmartPointer<Octree> octree)
{
  this->octree=octree;
  if (!octree) return;
	this->trackball_center =this->octree->world_box.center();
	this->frustum->guessBestPosition(this->octree->world_box);
  this->redisplay();
}

///////////////////////////////////////////////////////////////////////////////////////////
bool GLCanvas::bind()
{
  return native->bind();
}

///////////////////////////////////////////////////////////////////////////////////////////
bool GLCanvas::unbind()
{
  return native->unbind();
}

////////////////////////////////////////////////////////////
void GLCanvas::swapBuffers()
{
  native->swapBuffers();
}


///////////////////////////////////////////////////////////
void GLCanvas::runLoop()
{
  while (!this->m_close && juce::MessageManager::getInstance()->runDispatchLoopUntil(100))
    ;
}

///////////////////////////////////////////////////////////////////////////////////////////
void GLCanvas::close()
{
  m_close=true;
}


////////////////////////////////////////////////////////////
void GLCanvas::setViewport(int x,int y,int width,int height)
{
  glViewport(x,y,width,height);
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
{
  glClear((ClearColor?GL_COLOR_BUFFER_BIT:0) | (ClearDepth?GL_DEPTH_BUFFER_BIT:0));
}

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
///////////////////////////////////////////////////////////////
void GLCanvas::onMouseDown(int button,int x,int y)
{
  mouse_beginx = x;
  mouse_beginy = y;
}


///////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////
void GLCanvas::onMouseUp(int button,int x,int y)
{
  mouse_beginx = x;
  mouse_beginy = y;
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void GLCanvas::onMouseWheel(int delta)
{
  frustum->pos+=(delta>0?+1:-1) * frustum->dir * frustum->walk_speed;
  this->redisplay();
}

///////////////////////////////////////////////////////////////////////////////
void GLCanvas::onTimer()
{
  if (m_redisplay)
  {
    m_redisplay=false;
    this->bind();
    this->renderScene();
    this->unbind();
  }
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
  if (this->octree)
  {
    float maxdim=octree->world_box.maxsize();;
    zNear = maxdim / 50.0f ;
    zFar  = maxdim * 10;
  }

  frustum->projection_matrix=Mat4f::perspective(DEFAULT_FOV,width/(float)height,zNear,zFar);
  this->redisplay();
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void GLCanvas::renderBatch(SmartPointer<Batch> _batch)
{
  if (!_batch)
    return;

  juce::OpenGLContext* context=native->getContext();

#define BUFFER_OFFSET(_delta_)  ((char *)NULL + _delta_)

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
    if (!batch.vertices->gpu) 
      GLCanvas::Native::createArrayBuffer(batch.vertices);

    if (batch.vertices->gpu)
    {
      context->extensions.glBindBuffer(GL_ARRAY_BUFFER, batch.vertices->gpu->id);
      glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
      context->extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else
    {
      glVertexPointer(3, GL_FLOAT, 0, batch.vertices->c_ptr());
    }
    glEnableClientState(GL_VERTEX_ARRAY);
  }

  //normals
  if (batch.normals)
  {
    if (!batch.normals->gpu)  
      GLCanvas::Native::createArrayBuffer(batch.normals);

    if (batch.normals->gpu)
    {
      context->extensions.glBindBuffer(GL_ARRAY_BUFFER, batch.normals->gpu->id);
      glNormalPointer (GL_FLOAT, 0, BUFFER_OFFSET(0));
      context->extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else
    {
      glNormalPointer (GL_FLOAT, 0, batch.normals->c_ptr());
    }

    glEnableClientState(GL_NORMAL_ARRAY);
  }

  //colors
  if (batch.colors)
  {
    if (!batch.colors->gpu)  
      GLCanvas::Native::createArrayBuffer(batch.colors);

    if (batch.colors->gpu)
    {
      context->extensions.glBindBuffer(GL_ARRAY_BUFFER, batch.colors->gpu->id);
      glColorPointer(3,GL_FLOAT, 0, BUFFER_OFFSET(0));
      context->extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else
      glColorPointer(3,GL_FLOAT, 0, batch.colors->c_ptr());

    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_COLOR_MATERIAL);
  }

  //texture0
  if (batch.texture0 && batch.texture0coords)
  {
    //seems to be affected by the current color, since I normally use materials I do not care really
    glColor4f(1,1,1,1);

    //need generation
    if (!batch.texture0->gpu) 
      GLCanvas::Native::createTexture(batch.texture0);

    if (!batch.texture0coords->gpu) 
      GLCanvas::Native::createArrayBuffer(batch.texture0coords);

    context->extensions.glActiveTexture       (GL_TEXTURE0);
    context->extensions.glClientActiveTexture (GL_TEXTURE0);
    glBindTexture         (GL_TEXTURE_2D, batch.texture0->gpu->id);
    glEnable(GL_TEXTURE_2D);

    if (batch.texture0coords->gpu)
    {
      context->extensions.glBindBuffer(GL_ARRAY_BUFFER, batch.texture0coords->gpu->id);
      glTexCoordPointer (2, GL_FLOAT, 0,  BUFFER_OFFSET(0));
      context->extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else
      glTexCoordPointer (2, GL_FLOAT, 0,  batch.texture0coords->c_ptr());

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }


  //texture1
  if (batch.texture1 && batch.texture1coords)
  {
    //importante, se pre-lighting is calculated I do not need OpenGL lighting
    glDisable(GL_LIGHTING);
    glColor3f(1,1,1);

    if (!batch.texture1->gpu) 
      GLCanvas::Native::createTexture(batch.texture1);

    if (!batch.texture1coords->gpu) 
      GLCanvas::Native::createArrayBuffer(batch.texture1coords);

    context->extensions.glActiveTexture       (GL_TEXTURE1);
    context->extensions.glClientActiveTexture (GL_TEXTURE1);
    glBindTexture         (GL_TEXTURE_2D, batch.texture1->gpu->id);
    glEnable(GL_TEXTURE_2D);

    if (batch.texture1coords->gpu)
    {
      context->extensions.glBindBuffer(GL_ARRAY_BUFFER, batch.texture1coords->gpu->id);
      glTexCoordPointer (2, GL_FLOAT, 0,  BUFFER_OFFSET(0));
      context->extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else
      glTexCoordPointer (2, GL_FLOAT, 0,  batch.texture1coords->c_ptr());

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    context->extensions.glActiveTexture       (GL_TEXTURE0);
    context->extensions.glClientActiveTexture (GL_TEXTURE0);
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
    glDrawArrays(batch.primitive, 0, num_vertices);
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
    context->extensions.glClientActiveTexture(GL_TEXTURE1);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    context->extensions.glActiveTexture(GL_TEXTURE0);
    context->extensions.glClientActiveTexture(GL_TEXTURE0);
  }

  if (batch.texture0 && batch.texture0coords)
  {
    context->extensions.glActiveTexture(GL_TEXTURE0);
    context->extensions.glClientActiveTexture(GL_TEXTURE0);
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

#undef BUFFER_OFFSET
}



///////////////////////////////////////////////////////////////////////////////
void GLCanvas::renderScene()
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

  if (this->octree)
  {
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
              setLineWidth(2);
              setPolygonMode(Batch::LINES);
              Color4f ambient=v[i]->ambient;
              Color4f diffuse=v[i]->diffuse;
              v[i]->setColor(Color4f(0,0,0,0.05f));
              renderBatch(v[i]);
              v[i]->ambient=ambient;
              v[i]->diffuse=diffuse;
              setDepthWrite(true);
              setPolygonMode(Batch::POLYGON);
              setLineWidth(2);
            }

            //draw in chunks of fps=30
            /*if (bProgressiveRendering && t1.msec()>30) 
            {
              t1.reset();
              bQuitRenderingLoop=m_redisplay;
              swapBuffers();
            }*/
          }
        }	
      }
    }

    //draw transparent object in reverse order
    for (int i=(transparent.size()-1);!bQuitRenderingLoop && i>=0 ;i--)
    {
      renderBatch(transparent[i]);

      //draw in chunks of fps=30
      //if (bProgressiveRendering && t1.msec()>30) 
      //{
      //  t1.reset();
      //  bQuitRenderingLoop=m_redisplay; 
      //  swapBuffers();
      //}
    }
  }

  swapBuffers();
}


