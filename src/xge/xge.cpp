#include <xge/xge.h>
#include <xge/glcanvas.h>

#include <JUCE/AppConfig.h>
#include <JUCE/modules/juce_opengl/juce_opengl.h>


///////////////////////////////////////////////////////////////////////////////////////////
class XgeApplication : public juce::JUCEApplication
{
public:

  //constructor
  XgeApplication()                                 
  {}

  //destrutor
  virtual ~XgeApplication()                                 
  {}

  //getApplicationName
  virtual const juce::String getApplicationName() override                          
  {return "PyPlasm";}

  //getApplicationVersion
  const juce::String getApplicationVersion() override
  {return "1.0";}

  //moreThanOneInstanceAllowed
  virtual bool moreThanOneInstanceAllowed() override
  {return true;}

  //anotherInstanceStarted
  virtual void anotherInstanceStarted(const juce::String&) override 
  {}

  //initialise
  virtual void initialise(const juce::String& commandLine)  override
  {
    GLCanvas::createShared();
  }

  //shutdown
  virtual void shutdown() override
  {
    GLCanvas::destroyShared();
  }

};


//see START_JUCE_APPLICATION(XgeApplication)
#if PYPLASM_APPLE
namespace juce {extern void initialiseNSApplication();}
#endif

static juce::ScopedPointer< juce::JUCEApplicationBase > app;

//////////////////////////////////////////////////////////////////
void XgeModule::init()
{
  if (app) return;
  juce::JUCEApplicationBase::createInstance = (juce::JUCEApplicationBase::CreateInstanceFunction)-1;//to avoid a juce assert

  #if PYPLASM_APPLE
  juce::initialiseNSApplication();
  new juce::ScopedAutoReleasePool();
  #endif
  juce::initialiseJuce_GUI();
  app=new XgeApplication();
  app->initialiseApp();
}

//////////////////////////////////////////////////////////////////
void XgeModule::shutdown()
{
  if (!app) return;
  app->shutdownApp();
  juce::shutdownJuce_GUI();
}

