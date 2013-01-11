
//==============================================================================
//Visus: this class has been recycled from juce 1_53
//==============================================================================

class OpenGLComponent::OpenGLComponentWatcher  : public ComponentMovementWatcher
{
public:

  OpenGLComponent* const owner;

  //constructor
  OpenGLComponentWatcher(OpenGLComponent* const owner_): ComponentMovementWatcher (owner_),owner (owner_)
    {}

  //componentMovedOrResized
  void componentMovedOrResized (bool,bool)
    {owner->updateContextPosition();}

  //componentPeerChanged
  void componentPeerChanged()
    {owner->recreateContextAsync();}

  //componentVisibilityChanged
  void componentVisibilityChanged()
    {owner->recreateContextAsync();}

};

//////////////////////////////////////////////////////////////////////////////////////////////
OpenGLComponent::OpenGLComponent() : contextToShareListsWith(nullptr),needToDeleteContext(false)
{
  setOpaque(true);
  componentWatcher=new OpenGLComponentWatcher(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////
OpenGLComponent::~OpenGLComponent()
{
  if (context.nativeContext)
  {
    context.nativeContext=nullptr;
    delete context.nativeContext;
  }
  
  componentWatcher=nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////
void OpenGLComponent::setPixelFormat(const OpenGLPixelFormat& formatToUse)
{
  if (preferredPixelFormat!=formatToUse)
  {
    const ScopedLock sl(contextLock);
    preferredPixelFormat = formatToUse;
    recreateContextAsync();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
void OpenGLComponent::shareWith(OpenGLContext* c)
{
  if (contextToShareListsWith!=c)
  {
    const ScopedLock sl(contextLock);
    contextToShareListsWith=c;
    recreateContextAsync();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
void OpenGLComponent::recreateContextAsync()
{
  const ScopedLock sl (contextLock);
  needToDeleteContext=true;
  repaint();
}

//////////////////////////////////////////////////////////////////////////////////////////////
bool OpenGLComponent::makeCurrentContextActive()
{
  return isVisible() && context.makeActive();
}

//////////////////////////////////////////////////////////////////////////////////////////////
void OpenGLComponent::makeCurrentContextInactive()
{
  //visus NOT SUPPORTED ANYMORE in juce 2.0
  //context.makeInactive();
}

//////////////////////////////////////////////////////////////////////////////////////////////
bool OpenGLComponent::isActiveContext() const noexcept
{
  return context.isActive();
}

//////////////////////////////////////////////////////////////////////////////////////////////
void OpenGLComponent::swapBuffers()
{
  context.swapBuffers();
}

//////////////////////////////////////////////////////////////////////////////////////////////
void OpenGLComponent::updateContext()
{
  //request to delete the context
  if (needToDeleteContext)
  {
    if (context.nativeContext!=nullptr)
    {
      context.nativeContext->shutdownOnRenderThread();
      delete context.nativeContext;
      context.nativeContext=nullptr;
    }
    needToDeleteContext = false;
  }

  //do not create the context if not visible!
  if (!this->isVisible()) 
    return;

  //recreate native context
  if (context.nativeContext==nullptr)
  {
    const ScopedLock sl(contextLock);
    if (context.nativeContext==nullptr)
    {
      context.nativeContext = new OpenGLContext::NativeContext (*this, preferredPixelFormat,
        contextToShareListsWith != nullptr ? contextToShareListsWith->nativeContext: nullptr);

      if (!context.nativeContext->createdOk())
      {
        delete context.nativeContext;
        context.nativeContext=nullptr;
      }

      if (context.nativeContext!=nullptr)
      {
        context.nativeContext->initialiseOnRenderThread();
        updateContextPosition();
        glViewport (0, 0, getWidth(), getHeight());
        
        if (context.makeActive())
        {
          context.extensions.initialise();
          newOpenGLContextCreated();
          makeCurrentContextInactive();
        }
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
void OpenGLComponent::updateContextPosition()
{
  if (this->isVisible() && getWidth()>0 && getHeight()>0)
  {
    Component* const topComp = getTopLevelComponent();
    if (topComp->getPeer()!=nullptr)
    {
      const ScopedLock sl(contextLock);
      if (context.nativeContext!=nullptr)
        context.nativeContext->updateWindowPosition(topComp->getLocalArea(this,getLocalBounds()));
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
void OpenGLComponent::paint(Graphics&)
{
  if (!this->isVisible() || getWidth()<=0 || getHeight()<=0)
    return; 
  
  ComponentPeer* const peer = getPeer();

  updateContext();
  if (peer!=nullptr)
  {
    const Point<int> topLeft (getScreenPosition() - peer->getScreenPosition());
    peer->addMaskedRegion(juce::Rectangle<int>(topLeft.getX(), topLeft.getY(), getWidth(), getHeight()));
  }
}
