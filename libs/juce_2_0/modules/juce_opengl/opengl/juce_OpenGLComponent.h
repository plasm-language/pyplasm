#ifndef __JUCE_OPENGLCOMPONENT_JUCEHEADER__
#define __JUCE_OPENGLCOMPONENT_JUCEHEADER__

#include "juce_OpenGLContext.h"

/////////////////////////////////////////////////////////////////////////////////////////
//VISUS: this class is missing from juce 2.0 but I need it!!!!!
/////////////////////////////////////////////////////////////////////////////////////////
class JUCE_API  OpenGLComponent  : public Component
{
public:

  OpenGLComponent ();

  /** Destructor. */
  ~OpenGLComponent();

  //setPixelFormat
  void setPixelFormat(const OpenGLPixelFormat& formatToUse);

  //shareWith
  void shareWith(OpenGLContext* contextToShareListsWith);

  //getShareContext
  OpenGLContext* getShareContext() const noexcept     
    {return contextToShareListsWith;}

  //swapBuffers
  void swapBuffers();

  //newOpenGLContextCreated
  virtual void newOpenGLContextCreated()=0;

  //getContext
  OpenGLContext* getContext() const noexcept           
    {return (OpenGLContext*)&context;}

  //makeCurrentContextActive
  bool makeCurrentContextActive();

  //makeCurrentContextInactive
  void makeCurrentContextInactive();

  //isActiveContext
  bool isActiveContext() const noexcept;

  //getContextLock
  CriticalSection& getContextLock() noexcept      
    {return contextLock;}

  //updateContext
  void updateContext();

  //paint
  void paint(Graphics& g);

private:

  class OpenGLComponentWatcher;
  friend class OpenGLComponentWatcher;
  friend class ScopedPointer <OpenGLComponentWatcher>;

  ScopedPointer<OpenGLComponentWatcher>       componentWatcher;
  OpenGLContext                               context;
  OpenGLContext*                              contextToShareListsWith;
  CriticalSection                             contextLock;
  OpenGLPixelFormat                           preferredPixelFormat;
  bool                                        needToDeleteContext;

  void updateContextPosition();
  void recreateContextAsync ();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLComponent);
};


#endif   // __JUCE_OPENGLCONTEXT_JUCEHEADER__


