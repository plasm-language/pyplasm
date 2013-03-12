/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-11 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

class OpenGLContext::CachedImage  : public CachedComponentImage,
                                    public Thread
{
public:
    CachedImage (OpenGLContext& context_,
                 Component& component_,
                 const OpenGLPixelFormat& pixelFormat,
                 const OpenGLContext* contextToShareWith)
        : Thread ("OpenGL Rendering"),
          context (context_), component (component_),
         #if JUCE_OPENGL_ES
          shadersAvailable (true),
         #else
          shadersAvailable (false),
         #endif
          needsUpdate (true)
    {
        nativeContext = new NativeContext (component, pixelFormat,
                                           contextToShareWith != nullptr ? contextToShareWith->nativeContext
                                                                         : nullptr);

        if (nativeContext->createdOk())
        {
            context.nativeContext = nativeContext;

           #if ! JUCE_ANDROID
            startThread (6);
           #endif
        }
        else
        {
            nativeContext = nullptr;
        }
    }

    ~CachedImage()
    {
       #if ! JUCE_ANDROID
        stopThread (10000);
       #endif
    }

    //==============================================================================
    void paint (Graphics&)
    {
        ComponentPeer* const peer = component.getPeer();

        if (peer != nullptr)
            peer->addMaskedRegion (peer->getComponent()->getLocalArea (&component, component.getLocalBounds()));
    }

    void invalidateAll()
    {
        validArea.clear();
        triggerRepaint();
    }

    void invalidate (const Rectangle<int>& area)
    {
        validArea.subtract (area);
        triggerRepaint();
    }

    void releaseResources() {}

    void triggerRepaint()
    {
        needsUpdate = true;

       #if JUCE_ANDROID
        if (nativeContext != nullptr)
            nativeContext->triggerRepaint();
       #else
        notify();
       #endif
    }

    //==============================================================================
    bool ensureFrameBufferSize (int width, int height)
    {
        const int fbW = cachedImageFrameBuffer.getWidth();
        const int fbH = cachedImageFrameBuffer.getHeight();

        if (fbW != width || fbH != height || ! cachedImageFrameBuffer.isValid())
        {
            if (! cachedImageFrameBuffer.initialise (context, width, height))
                return false;

            validArea.clear();
            JUCE_CHECK_OPENGL_ERROR
        }

        return true;
    }

    void clearRegionInFrameBuffer (const RectangleList& list)
    {
        glClearColor (0, 0, 0, 0);
        glEnable (GL_SCISSOR_TEST);

        const GLuint previousFrameBufferTarget = OpenGLFrameBuffer::getCurrentFrameBufferTarget();
        cachedImageFrameBuffer.makeCurrentRenderingTarget();

        for (RectangleList::Iterator i (list); i.next();)
        {
            const Rectangle<int>& r = *i.getRectangle();
            glScissor (r.getX(), component.getHeight() - r.getBottom(), r.getWidth(), r.getHeight());
            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }

        glDisable (GL_SCISSOR_TEST);
        context.extensions.glBindFramebuffer (GL_FRAMEBUFFER, previousFrameBufferTarget);
        JUCE_CHECK_OPENGL_ERROR
    }

    bool renderFrame()
    {
        if (! context.makeActive())
            return false;

        NativeContext::Locker locker (*nativeContext);

        JUCE_CHECK_OPENGL_ERROR
        glViewport (0, 0, component.getWidth(), component.getHeight());

        if (context.renderer != nullptr)
        {
            context.renderer->renderOpenGL();
            clearGLError();
        }

        if (context.renderComponents)
            paintComponent();

        context.swapBuffers();
        return true;
    }

    void paintComponent()
    {
        if (needsUpdate)
        {
            MessageManagerLock mm (this);
            if (! mm.lockWasGained())
                return;

            needsUpdate = false;

            // you mustn't set your own cached image object when attaching a GL context!
            jassert (get (component) == this);

            const Rectangle<int> bounds (component.getLocalBounds());
            if (! ensureFrameBufferSize (bounds.getWidth(), bounds.getHeight()))
                return;

            RectangleList invalid (bounds);
            invalid.subtract (validArea);
            validArea = bounds;

            if (! invalid.isEmpty())
            {
                clearRegionInFrameBuffer (invalid);

                {
                    ScopedPointer<LowLevelGraphicsContext> g (createOpenGLGraphicsContext (context, cachedImageFrameBuffer));
                    g->clipToRectangleList (invalid);
                    paintOwner (*g);
                    JUCE_CHECK_OPENGL_ERROR
                }

                context.makeActive();
            }

            JUCE_CHECK_OPENGL_ERROR
        }

       #if ! JUCE_ANDROID
        glEnable (GL_TEXTURE_2D);
        clearGLError();
       #endif
        context.extensions.glActiveTexture (GL_TEXTURE0);
        glBindTexture (GL_TEXTURE_2D, cachedImageFrameBuffer.getTextureID());

        const Rectangle<int> cacheBounds (cachedImageFrameBuffer.getWidth(), cachedImageFrameBuffer.getHeight());
        context.copyTexture (cacheBounds, cacheBounds, context.getWidth(), context.getHeight());
        glBindTexture (GL_TEXTURE_2D, 0);
        JUCE_CHECK_OPENGL_ERROR
    }

    void paintOwner (LowLevelGraphicsContext& context)
    {
        Graphics g (&context);

       #if JUCE_ENABLE_REPAINT_DEBUGGING
        g.saveState();
       #endif

        JUCE_TRY
        {
            component.paintEntireComponent (g, false);
        }
        JUCE_CATCH_EXCEPTION

       #if JUCE_ENABLE_REPAINT_DEBUGGING
        // enabling this code will fill all areas that get repainted with a colour overlay, to show
        // clearly when things are being repainted.
        g.restoreState();

        static Random rng;
        g.fillAll (Colour ((uint8) rng.nextInt (255),
                           (uint8) rng.nextInt (255),
                           (uint8) rng.nextInt (255),
                           (uint8) 0x50));
       #endif
    }

    //==============================================================================
    void run()
    {
        {
            // Allow the message thread to finish setting-up the context before using it..
            MessageManagerLock mml (this);
            if (! mml.lockWasGained())
                return;
        }

        nativeContext->makeActive();
        initialiseOnThread();

       #if JUCE_USE_OPENGL_SHADERS && ! JUCE_OPENGL_ES
        shadersAvailable = OpenGLShaderProgram::getLanguageVersion() > 0;
       #endif

        while (! threadShouldExit())
        {
            const uint32 frameRenderStartTime = Time::getMillisecondCounter();

            if (renderFrame())
                waitForNextFrame (frameRenderStartTime);
        }

        shutdownOnThread();
    }

    void initialiseOnThread()
    {
        associatedObjectNames.clear();
        associatedObjects.clear();

        nativeContext->initialiseOnRenderThread();
        glViewport (0, 0, component.getWidth(), component.getHeight());

        context.extensions.initialise();

        if (context.renderer != nullptr)
            context.renderer->newOpenGLContextCreated();
    }

    void shutdownOnThread()
    {
        if (context.renderer != nullptr)
            context.renderer->openGLContextClosing();

        nativeContext->shutdownOnRenderThread();

        associatedObjectNames.clear();
        associatedObjects.clear();
    }

    void waitForNextFrame (const uint32 frameRenderStartTime)
    {
        const int defaultFPS = 60;

        const int elapsed = (int) (Time::getMillisecondCounter() - frameRenderStartTime);
        wait (jmax (1, (1000 / defaultFPS) - elapsed));
    }

    //==============================================================================
    static CachedImage* get (Component& c) noexcept
    {
        return dynamic_cast<CachedImage*> (c.getCachedComponentImage());
    }

    //==============================================================================
    ScopedPointer<NativeContext> nativeContext;

    OpenGLContext& context;
    Component& component;

    OpenGLFrameBuffer cachedImageFrameBuffer;
    RectangleList validArea;

    StringArray associatedObjectNames;
    ReferenceCountedArray<ReferenceCountedObject> associatedObjects;

    WaitableEvent canPaintNowFlag, finishedPaintingFlag;
    bool volatile shadersAvailable;
    bool volatile needsUpdate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CachedImage);
};

//==============================================================================
#if JUCE_ANDROID
void OpenGLContext::NativeContext::contextCreatedCallback()
{
    isInsideGLCallback = true;

    CachedImage* const c = CachedImage::get (component);
    jassert (c != nullptr);

    if (c != nullptr)
        c->initialiseOnThread();

    isInsideGLCallback = false;
}

void OpenGLContext::NativeContext::renderCallback()
{
    isInsideGLCallback = true;

    CachedImage* const c = CachedImage::get (component);

    if (c != nullptr)
        c->renderFrame();

    isInsideGLCallback = false;
}
#endif

//==============================================================================
class OpenGLContext::Attachment  : public ComponentMovementWatcher
{
public:
    Attachment (OpenGLContext& context_, Component& comp)
       : ComponentMovementWatcher (&comp), context (context_)
    {
        if (canBeAttached (comp))
            attach();
    }

    ~Attachment()
    {
        detach();
    }

    void componentMovedOrResized (bool /*wasMoved*/, bool /*wasResized*/)
    {
        Component* const comp = getComponent();

        if (isAttached (*comp) != canBeAttached (*comp))
            componentVisibilityChanged();

        context.width  = comp->getWidth();
        context.height = comp->getHeight();

        if (comp->getWidth() > 0 && comp->getHeight() > 0
             && context.nativeContext != nullptr)
        {
            context.nativeContext->updateWindowPosition (comp->getTopLevelComponent()
                                                            ->getLocalArea (comp, comp->getLocalBounds()));
        }
    }

    void componentPeerChanged()
    {
        detach();
        componentVisibilityChanged();
    }

    void componentVisibilityChanged()
    {
        Component* const comp = getComponent();

        if (canBeAttached (*comp))
        {
            if (! isAttached (*comp))
                attach();
        }
        else
        {
            detach();
        }
    }

   #if JUCE_DEBUG || JUCE_LOG_ASSERTIONS
    void componentBeingDeleted (Component& component)
    {
        /* You must call detach() or delete your OpenGLContext to remove it
           from a component BEFORE deleting the component that it is using!
        */
        jassertfalse;

        ComponentMovementWatcher::componentBeingDeleted (component);
    }
   #endif

private:
    OpenGLContext& context;

    static bool canBeAttached (const Component& comp) noexcept
    {
        return comp.getWidth() > 0 && comp.getHeight() > 0 && comp.isShowing();
    }

    static bool isAttached (const Component& comp) noexcept
    {
        return comp.getCachedComponentImage() != nullptr;
    }

    void attach()
    {
        Component* const comp = getComponent();
        comp->setCachedComponentImage (new CachedImage (context, *comp,
                                                        context.pixelFormat,
                                                        context.contextToShareWith));
    }

    void detach()
    {
        getComponent()->setCachedComponentImage (nullptr);
        context.nativeContext = nullptr;
    }
};

//==============================================================================
OpenGLContext::OpenGLContext()
    : nativeContext (nullptr), renderer (nullptr), contextToShareWith (nullptr),
      width (0), height (0), renderComponents (true)
{
}

OpenGLContext::~OpenGLContext()
{
    detach();
}

void OpenGLContext::setRenderer (OpenGLRenderer* rendererToUse) noexcept
{
    // This method must not be called when the context has already been attached!
    // Call it before attaching your context, or use detach() first, before calling this!
    jassert (nativeContext == nullptr);

    renderer = rendererToUse;
}

void OpenGLContext::setComponentPaintingEnabled (bool shouldPaintComponent) noexcept
{
    // This method must not be called when the context has already been attached!
    // Call it before attaching your context, or use detach() first, before calling this!
    jassert (nativeContext == nullptr);

    renderComponents = shouldPaintComponent;
}

void OpenGLContext::setPixelFormat (const OpenGLPixelFormat& preferredPixelFormat) noexcept
{
    // This method must not be called when the context has already been attached!
    // Call it before attaching your context, or use detach() first, before calling this!
    jassert (nativeContext == nullptr);

    pixelFormat = preferredPixelFormat;
}

void OpenGLContext::setContextToShareWith (const OpenGLContext* context) noexcept
{
    // This method must not be called when the context has already been attached!
    // Call it before attaching your context, or use detach() first, before calling this!
    jassert (nativeContext == nullptr);

    contextToShareWith = context;
}

void OpenGLContext::attachTo (Component& component)
{
    component.repaint();

    if (getTargetComponent() != &component)
    {
        detach();

        width  = component.getWidth();
        height = component.getHeight();

        attachment = new Attachment (*this, component);
    }
}

void OpenGLContext::detach()
{
    attachment = nullptr;
    nativeContext = nullptr;
    width = height = 0;
}

bool OpenGLContext::isAttached() const noexcept
{
    return nativeContext != nullptr;
}

Component* OpenGLContext::getTargetComponent() const noexcept
{
    return attachment != nullptr ? attachment->getComponent() : nullptr;
}

OpenGLContext* OpenGLContext::getCurrentContext()
{
   #if JUCE_ANDROID
    NativeContext* const nc = NativeContext::getActiveContext();
    if (nc == nullptr)
        return nullptr;

    CachedImage* currentContext = CachedImage::get (nc->component);
   #else
    CachedImage* currentContext = dynamic_cast <CachedImage*> (Thread::getCurrentThread());
   #endif

    return currentContext != nullptr ? &currentContext->context : nullptr;
}

bool OpenGLContext::makeActive() const noexcept     { return nativeContext != nullptr && nativeContext->makeActive(); }
bool OpenGLContext::isActive() const noexcept       { return nativeContext != nullptr && nativeContext->isActive(); }

void OpenGLContext::triggerRepaint()
{
    CachedImage* const currentContext
            = dynamic_cast <CachedImage*> (Thread::getCurrentThread());

    if (currentContext != nullptr)
    {
        currentContext->triggerRepaint();
        currentContext->component.repaint();
    }
}

void OpenGLContext::swapBuffers()
{
    if (nativeContext != nullptr)
        nativeContext->swapBuffers();
}

unsigned int OpenGLContext::getFrameBufferID() const noexcept
{
    return nativeContext != nullptr ? nativeContext->getFrameBufferID() : 0;
}

bool OpenGLContext::setSwapInterval (int numFramesPerSwap)
{
    return nativeContext != nullptr && nativeContext->setSwapInterval (numFramesPerSwap);
}

int OpenGLContext::getSwapInterval() const
{
    return nativeContext != nullptr ? nativeContext->getSwapInterval() : 0;
}

void* OpenGLContext::getRawContext() const noexcept
{
    return nativeContext != nullptr ? nativeContext->getRawContext() : nullptr;
}

OpenGLContext::CachedImage* OpenGLContext::getCachedImage() const noexcept
{
    Component* const comp = getTargetComponent();
    return comp != nullptr ? CachedImage::get (*comp) : nullptr;
}

bool OpenGLContext::areShadersAvailable() const
{
    CachedImage* const c = getCachedImage();
    return c != nullptr && c->shadersAvailable;
}

ReferenceCountedObject* OpenGLContext::getAssociatedObject (const char* name) const
{
    jassert (name != nullptr);

    CachedImage* const c = getCachedImage();

    // This method must only be called from an openGL rendering callback.
    jassert (c != nullptr && nativeContext != nullptr);
    jassert (getCurrentContext() != nullptr);

    const int index = c->associatedObjectNames.indexOf (name);
    return index >= 0 ? c->associatedObjects.getUnchecked (index) : nullptr;
}

void OpenGLContext::setAssociatedObject (const char* name, ReferenceCountedObject* newObject)
{
    jassert (name != nullptr);

    CachedImage* const c = getCachedImage();

    // This method must only be called from an openGL rendering callback.
    jassert (c != nullptr && nativeContext != nullptr);
    jassert (getCurrentContext() != nullptr);

    const int index = c->associatedObjectNames.indexOf (name);

    if (index >= 0)
    {
        c->associatedObjects.set (index, newObject);
    }
    else
    {
        c->associatedObjectNames.add (name);
        c->associatedObjects.add (newObject);
    }
}

void OpenGLContext::copyTexture (const Rectangle<int>& targetClipArea,
                                 const Rectangle<int>& anchorPosAndTextureSize,
                                 const int contextWidth, const int contextHeight)
{
    if (contextWidth <= 0 || contextHeight <= 0)
        return;

    JUCE_CHECK_OPENGL_ERROR
    glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);

   #if JUCE_USE_OPENGL_SHADERS
    if (areShadersAvailable())
    {
        struct OverlayShaderProgram  : public ReferenceCountedObject
        {
            OverlayShaderProgram (OpenGLContext& context)
                : program (context), builder (program), params (program)
            {}

            static const OverlayShaderProgram& select (OpenGLContext& context)
            {
                static const char programValueID[] = "juceGLComponentOverlayShader";
                OverlayShaderProgram* program = static_cast <OverlayShaderProgram*> (context.getAssociatedObject (programValueID));

                if (program == nullptr)
                {
                    program = new OverlayShaderProgram (context);
                    context.setAssociatedObject (programValueID, program);
                }

                program->program.use();
                return *program;
            }

            struct ProgramBuilder
            {
                ProgramBuilder (OpenGLShaderProgram& program)
                {
                    program.addShader ("attribute " JUCE_HIGHP " vec2 position;"
                                       "uniform " JUCE_HIGHP " vec2 screenSize;"
                                       "varying " JUCE_HIGHP " vec2 pixelPos;"
                                       "void main()"
                                       "{"
                                        "pixelPos = position;"
                                        JUCE_HIGHP " vec2 scaled = position / (0.5 * screenSize.xy);"
                                        "gl_Position = vec4 (scaled.x - 1.0, 1.0 - scaled.y, 0, 1.0);"
                                       "}",
                                       GL_VERTEX_SHADER);

                    program.addShader ("uniform sampler2D imageTexture;"
                                       "uniform " JUCE_HIGHP " float textureBounds[4];"
                                       "varying " JUCE_HIGHP " vec2 pixelPos;"
                                       "void main()"
                                       "{"
                                        JUCE_HIGHP " vec2 texturePos = (pixelPos - vec2 (textureBounds[0], textureBounds[1]))"
                                                                         "/ vec2 (textureBounds[2], textureBounds[3]);"
                                        "gl_FragColor = texture2D (imageTexture, vec2 (texturePos.x, 1.0 - texturePos.y));"
                                       "}",
                                       GL_FRAGMENT_SHADER);
                    program.link();
                }
            };

            struct Params
            {
                Params (OpenGLShaderProgram& program)
                    : positionAttribute (program, "position"),
                      screenSize (program, "screenSize"),
                      imageTexture (program, "imageTexture"),
                      textureBounds (program, "textureBounds")
                {}

                void set (const float targetWidth, const float targetHeight, const Rectangle<float>& bounds) const
                {
                    const GLfloat m[] = { bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight() };
                    textureBounds.set (m, 4);
                    imageTexture.set (0);
                    screenSize.set (targetWidth, targetHeight);
                }

                OpenGLShaderProgram::Attribute positionAttribute;
                OpenGLShaderProgram::Uniform screenSize, imageTexture, textureBounds;
            };

            OpenGLShaderProgram program;
            ProgramBuilder builder;
            Params params;
        };

        const GLshort left   = (GLshort) targetClipArea.getX();
        const GLshort top    = (GLshort) targetClipArea.getY();
        const GLshort right  = (GLshort) targetClipArea.getRight();
        const GLshort bottom = (GLshort) targetClipArea.getBottom();
        const GLshort vertices[] = { left, bottom, right, bottom, left, top, right, top };

        const OverlayShaderProgram& program = OverlayShaderProgram::select (*this);
        program.params.set ((float) contextWidth, (float) contextHeight, anchorPosAndTextureSize.toFloat());

        extensions.glVertexAttribPointer (program.params.positionAttribute.attributeID, 2, GL_SHORT, GL_FALSE, 4, vertices);
        extensions.glEnableVertexAttribArray (program.params.positionAttribute.attributeID);
        JUCE_CHECK_OPENGL_ERROR

        glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);

        extensions.glUseProgram (0);
        extensions.glDisableVertexAttribArray (program.params.positionAttribute.attributeID);
    }
    #if JUCE_USE_OPENGL_FIXED_FUNCTION
    else
    #endif
   #endif

   #if JUCE_USE_OPENGL_FIXED_FUNCTION
    {
        glEnable (GL_SCISSOR_TEST);
        glScissor (targetClipArea.getX(), contextHeight - targetClipArea.getBottom(),
                   targetClipArea.getWidth(), targetClipArea.getHeight());

        JUCE_CHECK_OPENGL_ERROR
        glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
        glDisableClientState (GL_COLOR_ARRAY);
        glDisableClientState (GL_NORMAL_ARRAY);
        glEnableClientState (GL_VERTEX_ARRAY);
        glEnableClientState (GL_TEXTURE_COORD_ARRAY);
        OpenGLHelpers::prepareFor2D (contextWidth, contextHeight);
        JUCE_CHECK_OPENGL_ERROR

        const GLfloat textureCoords[] = { 0, 0, 1.0f, 0, 0, 1.0f, 1.0f, 1.0f };
        glTexCoordPointer (2, GL_FLOAT, 0, textureCoords);

        const GLshort left   = (GLshort) anchorPosAndTextureSize.getX();
        const GLshort right  = (GLshort) anchorPosAndTextureSize.getRight();
        const GLshort top    = (GLshort) (contextHeight - anchorPosAndTextureSize.getY());
        const GLshort bottom = (GLshort) (contextHeight - anchorPosAndTextureSize.getBottom());
        const GLshort vertices[] = { left, bottom, right, bottom, left, top, right, top };
        glVertexPointer (2, GL_SHORT, 0, vertices);

        glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);
        glDisable (GL_SCISSOR_TEST);
    }
   #endif

    JUCE_CHECK_OPENGL_ERROR
}
