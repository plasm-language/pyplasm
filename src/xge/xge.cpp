#include <xge/xge.h>
#include <xge/glcanvas.h>

#define FREEIMAGE_LIB
#include <FreeImage.h>

#define DONT_SET_USING_JUCE_NAMESPACE 1
#include <juce_2_0/juce.h>

///////////////////////////////////////////////////////////////////////////////////////////
class XgeApplication : public juce::JUCEApplication
{
public:
  XgeApplication                                  ()                                 {}
  ~XgeApplication                                 ()                                 {}
  void                 shutdown                   ()                                 {}
  const juce::String   getApplicationName         ()                                 {return "PyPlasm GLCanvas";}
  const juce::String   getApplicationVersion      ()                                 {return "1.0";}
  bool                 moreThanOneInstanceAllowed ()                                 {return true;}
  void                 anotherInstanceStarted     (const juce::String&)              {}
  void                 initialise                 (const juce::String& commandLine)  {}
};

static SmartPointer<XgeApplication> app;


//////////////////////////////////////////////////////////////////
void XgeModule::init()
{
  if (!app)
  {
	  Log::printf("XgeModule::init\n");
	  FreeImage_Initialise();

    juce::initialiseJuce_GUI();
    juce::JUCEApplication::createInstance = (juce::JUCEApplication::CreateInstanceFunction)-1;//need not to be 0 
    app=SmartPointer<XgeApplication>(new XgeApplication());
    app->initialiseApp("dummy");
    GLCanvas::setShared(SmartPointer<GLCanvas>(new GLCanvas));
  }
}

//////////////////////////////////////////////////////////////////
void XgeModule::shutdown()
{
  if (app)
  {
    Log::printf("XgeModule::shutdown\n");
    GLCanvas::setShared(SmartPointer<GLCanvas>());
    app.reset();
    FreeImage_DeInitialise();
  }
}