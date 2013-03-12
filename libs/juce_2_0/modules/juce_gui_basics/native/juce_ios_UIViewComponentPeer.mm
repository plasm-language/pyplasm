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

class UIViewComponentPeer;

//==============================================================================
} // (juce namespace)

#define JuceUIView MakeObjCClassName(JuceUIView)

@interface JuceUIView : UIView <UITextViewDelegate>
{
@public
    UIViewComponentPeer* owner;
    UITextView* hiddenTextView;
}

- (JuceUIView*) initWithOwner: (UIViewComponentPeer*) owner withFrame: (CGRect) frame;
- (void) dealloc;

- (void) drawRect: (CGRect) r;

- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event;
- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event;
- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event;
- (void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event;

- (BOOL) becomeFirstResponder;
- (BOOL) resignFirstResponder;
- (BOOL) canBecomeFirstResponder;

- (BOOL) textView: (UITextView*) textView shouldChangeTextInRange: (NSRange) range replacementText: (NSString*) text;
@end


#define JuceUIViewController MakeObjCClassName(JuceUIViewController)

@interface JuceUIViewController : UIViewController
{
}

- (BOOL) shouldAutorotateToInterfaceOrientation: (UIInterfaceOrientation) interfaceOrientation;
- (void) didRotateFromInterfaceOrientation: (UIInterfaceOrientation) fromInterfaceOrientation;
@end

//==============================================================================
#define JuceUIWindow MakeObjCClassName(JuceUIWindow)

@interface JuceUIWindow : UIWindow
{
@private
    UIViewComponentPeer* owner;
    bool isZooming;
}

- (void) setOwner: (UIViewComponentPeer*) owner;
- (void) becomeKeyWindow;
@end

namespace juce
{

//==============================================================================
class UIViewComponentPeer  : public ComponentPeer,
                             public FocusChangeListener
{
public:
    UIViewComponentPeer (Component* const component,
                         const int windowStyleFlags,
                         UIView* viewToAttachTo);

    ~UIViewComponentPeer();

    //==============================================================================
    void* getNativeHandle() const;
    void setVisible (bool shouldBeVisible);
    void setTitle (const String& title);
    void setPosition (int x, int y);
    void setSize (int w, int h);
    void setBounds (int x, int y, int w, int h, bool isNowFullScreen);

    Rectangle<int> getBounds() const;
    Rectangle<int> getBounds (const bool global) const;
    Point<int> getScreenPosition() const;
    Point<int> localToGlobal (const Point<int>& relativePosition);
    Point<int> globalToLocal (const Point<int>& screenPosition);
    void setAlpha (float newAlpha);
    void setMinimised (bool shouldBeMinimised);
    bool isMinimised() const;
    void setFullScreen (bool shouldBeFullScreen);
    bool isFullScreen() const;
    bool contains (const Point<int>& position, bool trueIfInAChildWindow) const;
    BorderSize<int> getFrameSize() const;
    bool setAlwaysOnTop (bool alwaysOnTop);
    void toFront (bool makeActiveWindow);
    void toBehind (ComponentPeer* other);
    void setIcon (const Image& newIcon);

    virtual void drawRect (CGRect r);

    virtual bool canBecomeKeyWindow();
    virtual bool windowShouldClose();

    virtual void redirectMovedOrResized();
    virtual CGRect constrainRect (CGRect r);

    //==============================================================================
    virtual void viewFocusGain();
    virtual void viewFocusLoss();
    bool isFocused() const;
    void grabFocus();
    void textInputRequired (const Point<int>& position);

    virtual BOOL textViewReplaceCharacters (const Range<int>& range, const String& text);
    void updateHiddenTextContent (TextInputTarget* target);
    void globalFocusChanged (Component*);

    virtual BOOL shouldRotate (UIInterfaceOrientation interfaceOrientation);
    virtual void displayRotated();

    void handleTouches (UIEvent* e, bool isDown, bool isUp, bool isCancel);

    //==============================================================================
    void repaint (const Rectangle<int>& area);
    void performAnyPendingRepaintsNow();

    //==============================================================================
    UIWindow* window;
    JuceUIView* view;
    JuceUIViewController* controller;
    bool isSharedWindow, fullScreen, insideDrawRect;
    static ModifierKeys currentModifiers;

    static int64 getMouseTime (UIEvent* e)
    {
        return (Time::currentTimeMillis() - Time::getMillisecondCounter())
                + (int64) ([e timestamp] * 1000.0);
    }

    static Rectangle<int> rotatedScreenPosToReal (const Rectangle<int>& r)
    {
        const Rectangle<int> screen (convertToRectInt ([UIScreen mainScreen].bounds));

        switch ([[UIApplication sharedApplication] statusBarOrientation])
        {
            case UIInterfaceOrientationPortrait:
                return r;

            case UIInterfaceOrientationPortraitUpsideDown:
                return Rectangle<int> (screen.getWidth() - r.getRight(), screen.getHeight() - r.getBottom(),
                                       r.getWidth(), r.getHeight());

            case UIInterfaceOrientationLandscapeLeft:
                return Rectangle<int> (r.getY(), screen.getHeight() - r.getRight(),
                                       r.getHeight(), r.getWidth());

            case UIInterfaceOrientationLandscapeRight:
                return Rectangle<int> (screen.getWidth() - r.getBottom(), r.getX(),
                                       r.getHeight(), r.getWidth());

            default: jassertfalse; // unknown orientation!
        }

        return r;
    }

    static Rectangle<int> realScreenPosToRotated (const Rectangle<int>& r)
    {
        const Rectangle<int> screen (convertToRectInt ([UIScreen mainScreen].bounds));

        switch ([[UIApplication sharedApplication] statusBarOrientation])
        {
            case UIInterfaceOrientationPortrait:
                return r;

            case UIInterfaceOrientationPortraitUpsideDown:
                return Rectangle<int> (screen.getWidth() - r.getRight(), screen.getHeight() - r.getBottom(),
                                       r.getWidth(), r.getHeight());

            case UIInterfaceOrientationLandscapeLeft:
                return Rectangle<int> (screen.getHeight() - r.getBottom(), r.getX(),
                                       r.getHeight(), r.getWidth());

            case UIInterfaceOrientationLandscapeRight:
                return Rectangle<int> (r.getY(), screen.getWidth() - r.getRight(),
                                       r.getHeight(), r.getWidth());

            default: jassertfalse; // unknown orientation!
        }

        return r;
    }

    MultiTouchMapper<UITouch*> currentTouches;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIViewComponentPeer);
};

//==============================================================================
} // (juce namespace)

@implementation JuceUIViewController

- (BOOL) shouldAutorotateToInterfaceOrientation: (UIInterfaceOrientation) interfaceOrientation
{
    JuceUIView* juceView = (JuceUIView*) [self view];
    jassert (juceView != nil && juceView->owner != nullptr);
    return juceView->owner->shouldRotate (interfaceOrientation);
}

- (void) didRotateFromInterfaceOrientation: (UIInterfaceOrientation) fromInterfaceOrientation
{
    JuceUIView* juceView = (JuceUIView*) [self view];
    jassert (juceView != nil && juceView->owner != nullptr);
    juceView->owner->displayRotated();
}

@end

@implementation JuceUIView

- (JuceUIView*) initWithOwner: (UIViewComponentPeer*) owner_
                    withFrame: (CGRect) frame
{
    [super initWithFrame: frame];
    owner = owner_;

    hiddenTextView = [[UITextView alloc] initWithFrame: CGRectMake (0, 0, 0, 0)];
    [self addSubview: hiddenTextView];
    hiddenTextView.delegate = self;

    hiddenTextView.autocapitalizationType = UITextAutocapitalizationTypeNone;
    hiddenTextView.autocorrectionType = UITextAutocorrectionTypeNo;

    return self;
}

- (void) dealloc
{
    [hiddenTextView removeFromSuperview];
    [hiddenTextView release];

    [super dealloc];
}

//==============================================================================
- (void) drawRect: (CGRect) r
{
    if (owner != nullptr)
        owner->drawRect (r);
}

//==============================================================================
bool KeyPress::isKeyCurrentlyDown (const int keyCode)
{
    return false;
}

ModifierKeys UIViewComponentPeer::currentModifiers;

ModifierKeys ModifierKeys::getCurrentModifiersRealtime() noexcept
{
    return UIViewComponentPeer::currentModifiers;
}

void ModifierKeys::updateCurrentModifiers() noexcept
{
    currentModifiers = UIViewComponentPeer::currentModifiers;
}

juce::Point<int> juce_lastMousePos;

//==============================================================================
- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{
    if (owner != nullptr)
        owner->handleTouches (event, true, false, false);
}

- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{
    if (owner != nullptr)
        owner->handleTouches (event, false, false, false);
}

- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
    if (owner != nullptr)
        owner->handleTouches (event, false, true, false);
}

- (void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event
{
    if (owner != nullptr)
        owner->handleTouches (event, false, true, true);

    [self touchesEnded: touches withEvent: event];
}

//==============================================================================
- (BOOL) becomeFirstResponder
{
    if (owner != nullptr)
        owner->viewFocusGain();

    return true;
}

- (BOOL) resignFirstResponder
{
    if (owner != nullptr)
        owner->viewFocusLoss();

    return true;
}

- (BOOL) canBecomeFirstResponder
{
    return owner != nullptr && owner->canBecomeKeyWindow();
}

- (BOOL) textView: (UITextView*) textView shouldChangeTextInRange: (NSRange) range replacementText: (NSString*) text
{
    return owner->textViewReplaceCharacters (Range<int> (range.location, range.location + range.length),
                                             nsStringToJuce (text));
}

@end

//==============================================================================
@implementation JuceUIWindow

- (void) setOwner: (UIViewComponentPeer*) owner_
{
    owner = owner_;
    isZooming = false;
}

- (void) becomeKeyWindow
{
    [super becomeKeyWindow];

    if (owner != nullptr)
        owner->grabFocus();
}

@end

//==============================================================================
//==============================================================================
namespace juce
{

//==============================================================================
UIViewComponentPeer::UIViewComponentPeer (Component* const component,
                                          const int windowStyleFlags,
                                          UIView* viewToAttachTo)
    : ComponentPeer (component, windowStyleFlags),
      window (nil),
      view (nil),
      controller (nil),
      isSharedWindow (viewToAttachTo != nil),
      fullScreen (false),
      insideDrawRect (false)
{
    CGRect r = convertToCGRect (component->getLocalBounds());

    view = [[JuceUIView alloc] initWithOwner: this withFrame: r];

    view.multipleTouchEnabled = YES;
    view.hidden = ! component->isVisible();
    view.opaque = component->isOpaque();
    view.backgroundColor = [[UIColor blackColor] colorWithAlphaComponent: 0];

    if (isSharedWindow)
    {
        window = [viewToAttachTo window];
        [viewToAttachTo addSubview: view];
    }
    else
    {
        controller = [[JuceUIViewController alloc] init];
        controller.view = view;

        r = convertToCGRect (rotatedScreenPosToReal (component->getBounds()));
        r.origin.y = [UIScreen mainScreen].bounds.size.height - (r.origin.y + r.size.height);

        window = [[JuceUIWindow alloc] init];
        window.frame = r;
        window.opaque = component->isOpaque();
        window.backgroundColor = [[UIColor blackColor] colorWithAlphaComponent: 0];

        [((JuceUIWindow*) window) setOwner: this];

        if (component->isAlwaysOnTop())
            window.windowLevel = UIWindowLevelAlert;

        [window addSubview: view];
        view.frame = CGRectMake (0, 0, r.size.width, r.size.height);

        window.hidden = view.hidden;
    }

    setTitle (component->getName());

    Desktop::getInstance().addFocusChangeListener (this);
}

UIViewComponentPeer::~UIViewComponentPeer()
{
    Desktop::getInstance().removeFocusChangeListener (this);

    view->owner = nullptr;
    [view removeFromSuperview];
    [view release];
    [controller release];

    if (! isSharedWindow)
    {
        [((JuceUIWindow*) window) setOwner: nil];
        [window release];
    }
}

//==============================================================================
void* UIViewComponentPeer::getNativeHandle() const
{
    return view;
}

void UIViewComponentPeer::setVisible (bool shouldBeVisible)
{
    view.hidden = ! shouldBeVisible;

    if (! isSharedWindow)
        window.hidden = ! shouldBeVisible;
}

void UIViewComponentPeer::setTitle (const String& title)
{
    // xxx is this possible?
}

void UIViewComponentPeer::setPosition (int x, int y)
{
    setBounds (x, y, component->getWidth(), component->getHeight(), false);
}

void UIViewComponentPeer::setSize (int w, int h)
{
    setBounds (component->getX(), component->getY(), w, h, false);
}

void UIViewComponentPeer::setBounds (int x, int y, int w, int h, const bool isNowFullScreen)
{
    fullScreen = isNowFullScreen;
    w = jmax (0, w);
    h = jmax (0, h);

    if (isSharedWindow)
    {
        CGRect r = CGRectMake ((CGFloat) x, (CGFloat) y, (CGFloat) w, (CGFloat) h);

        if (view.frame.size.width != r.size.width
             || view.frame.size.height != r.size.height)
            [view setNeedsDisplay];

        view.frame = r;
    }
    else
    {
        const Rectangle<int> bounds (rotatedScreenPosToReal (Rectangle<int> (x, y, w, h)));
        window.frame = convertToCGRect (bounds);
        view.frame = CGRectMake (0, 0, (CGFloat) bounds.getWidth(), (CGFloat) bounds.getHeight());

        handleMovedOrResized();
    }
}

Rectangle<int> UIViewComponentPeer::getBounds (const bool global) const
{
    CGRect r = view.frame;

    if (global && view.window != nil)
    {
        r = [view convertRect: r toView: nil];
        CGRect wr = view.window.frame;

        const Rectangle<int> windowBounds (realScreenPosToRotated (convertToRectInt (wr)));

        r.origin.x += windowBounds.getX();
        r.origin.y += windowBounds.getY();
    }

    return convertToRectInt (r);
}

Rectangle<int> UIViewComponentPeer::getBounds() const
{
    return getBounds (! isSharedWindow);
}

Point<int> UIViewComponentPeer::getScreenPosition() const
{
    return getBounds (true).getPosition();
}

Point<int> UIViewComponentPeer::localToGlobal (const Point<int>& relativePosition)
{
    return relativePosition + getScreenPosition();
}

Point<int> UIViewComponentPeer::globalToLocal (const Point<int>& screenPosition)
{
    return screenPosition - getScreenPosition();
}

CGRect UIViewComponentPeer::constrainRect (CGRect r)
{
    if (constrainer != nullptr)
    {
        CGRect mainScreen = [UIScreen mainScreen].bounds;

        CGRect current = window.frame;
        current.origin.y = mainScreen.size.height - current.origin.y - current.size.height;

        r.origin.y = mainScreen.size.height - r.origin.y - r.size.height;

        Rectangle<int> pos (convertToRectInt (r));
        Rectangle<int> original (convertToRectInt (current));

        constrainer->checkBounds (pos, original,
                                  Desktop::getInstance().getAllMonitorDisplayAreas().getBounds(),
                                  pos.getY() != original.getY() && pos.getBottom() == original.getBottom(),
                                  pos.getX() != original.getX() && pos.getRight()  == original.getRight(),
                                  pos.getY() == original.getY() && pos.getBottom() != original.getBottom(),
                                  pos.getX() == original.getX() && pos.getRight()  != original.getRight());

        r.origin.x = pos.getX();
        r.origin.y = mainScreen.size.height - r.size.height - pos.getY();
        r.size.width = pos.getWidth();
        r.size.height = pos.getHeight();
    }

    return r;
}

void UIViewComponentPeer::setAlpha (float newAlpha)
{
    [view.window setAlpha: (CGFloat) newAlpha];
}

void UIViewComponentPeer::setMinimised (bool shouldBeMinimised)
{
}

bool UIViewComponentPeer::isMinimised() const
{
    return false;
}

void UIViewComponentPeer::setFullScreen (bool shouldBeFullScreen)
{
    if (! isSharedWindow)
    {
        Rectangle<int> r (shouldBeFullScreen ? Desktop::getInstance().getMainMonitorArea()
                                             : lastNonFullscreenBounds);

        if ((! shouldBeFullScreen) && r.isEmpty())
            r = getBounds();

        // (can't call the component's setBounds method because that'll reset our fullscreen flag)
        if (! r.isEmpty())
            setBounds (r.getX(), r.getY(), r.getWidth(), r.getHeight(), shouldBeFullScreen);

        component->repaint();
    }
}

bool UIViewComponentPeer::isFullScreen() const
{
    return fullScreen;
}

namespace
{
    Desktop::DisplayOrientation convertToJuceOrientation (UIInterfaceOrientation interfaceOrientation)
    {
        switch (interfaceOrientation)
        {
            case UIInterfaceOrientationPortrait:            return Desktop::upright;
            case UIInterfaceOrientationPortraitUpsideDown:  return Desktop::upsideDown;
            case UIInterfaceOrientationLandscapeLeft:       return Desktop::rotatedClockwise;
            case UIInterfaceOrientationLandscapeRight:      return Desktop::rotatedAntiClockwise;
            default:                                        jassertfalse; // unknown orientation!
        }

        return Desktop::upright;
    }
}

BOOL UIViewComponentPeer::shouldRotate (UIInterfaceOrientation interfaceOrientation)
{
    return Desktop::getInstance().isOrientationEnabled (convertToJuceOrientation (interfaceOrientation));
}

void UIViewComponentPeer::displayRotated()
{
    const Rectangle<int> oldArea (component->getBounds());
    const Rectangle<int> oldDesktop (Desktop::getInstance().getMainMonitorArea());
    Desktop::getInstance().refreshMonitorSizes();

    if (fullScreen)
    {
        fullScreen = false;
        setFullScreen (true);
    }
    else if (! isSharedWindow)
    {
        const float l = oldArea.getX() / (float) oldDesktop.getWidth();
        const float r = oldArea.getRight() / (float) oldDesktop.getWidth();
        const float t = oldArea.getY() / (float) oldDesktop.getHeight();
        const float b = oldArea.getBottom() / (float) oldDesktop.getHeight();

        const Rectangle<int> newDesktop (Desktop::getInstance().getMainMonitorArea());

        setBounds ((int) (l * newDesktop.getWidth()),
                   (int) (t * newDesktop.getHeight()),
                   (int) ((r - l) * newDesktop.getWidth()),
                   (int) ((b - t) * newDesktop.getHeight()),
                   false);
    }
}

bool UIViewComponentPeer::contains (const Point<int>& position, bool trueIfInAChildWindow) const
{
    if (! (isPositiveAndBelow (position.getX(), component->getWidth())
            && isPositiveAndBelow (position.getY(), component->getHeight())))
        return false;

    UIView* v = [view hitTest: CGPointMake ((CGFloat) position.getX(), (CGFloat) position.getY())
                    withEvent: nil];

    if (trueIfInAChildWindow)
        return v != nil;

    return v == view;
}

BorderSize<int> UIViewComponentPeer::getFrameSize() const
{
    return BorderSize<int>();
}

bool UIViewComponentPeer::setAlwaysOnTop (bool alwaysOnTop)
{
    if (! isSharedWindow)
        window.windowLevel = alwaysOnTop ? UIWindowLevelAlert : UIWindowLevelNormal;

    return true;
}

void UIViewComponentPeer::toFront (bool makeActiveWindow)
{
    if (isSharedWindow)
        [[view superview] bringSubviewToFront: view];

    if (window != nil && component->isVisible())
        [window makeKeyAndVisible];
}

void UIViewComponentPeer::toBehind (ComponentPeer* other)
{
    UIViewComponentPeer* const otherPeer = dynamic_cast <UIViewComponentPeer*> (other);
    jassert (otherPeer != nullptr); // wrong type of window?

    if (otherPeer != nullptr)
    {
        if (isSharedWindow)
        {
            [[view superview] insertSubview: view belowSubview: otherPeer->view];
        }
        else
        {
            // don't know how to do this
        }
    }
}

void UIViewComponentPeer::setIcon (const Image& /*newIcon*/)
{
    // to do..
}

//==============================================================================
void UIViewComponentPeer::handleTouches (UIEvent* event, const bool isDown, const bool isUp, bool isCancel)
{
    NSArray* touches = [[event touchesForView: view] allObjects];

    for (unsigned int i = 0; i < [touches count]; ++i)
    {
        UITouch* touch = [touches objectAtIndex: i];

        if ([touch phase] == UITouchPhaseStationary)
            continue;

        CGPoint p = [touch locationInView: view];
        const Point<int> pos ((int) p.x, (int) p.y);
        juce_lastMousePos = pos + getScreenPosition();

        const int64 time = getMouseTime (event);
        const int touchIndex = currentTouches.getIndexOfTouch (touch);

        ModifierKeys modsToSend (currentModifiers);

        if (isDown)
        {
            if ([touch phase] != UITouchPhaseBegan)
                continue;

            currentModifiers = currentModifiers.withoutMouseButtons().withFlags (ModifierKeys::leftButtonModifier);
            modsToSend = currentModifiers;

            // this forces a mouse-enter/up event, in case for some reason we didn't get a mouse-up before.
            handleMouseEvent (touchIndex, pos, modsToSend.withoutMouseButtons(), time);
            if (! isValidPeer (this)) // (in case this component was deleted by the event)
                return;
        }
        else if (isUp)
        {
            if (! ([touch phase] == UITouchPhaseEnded || [touch phase] == UITouchPhaseCancelled))
                continue;

            modsToSend = modsToSend.withoutMouseButtons();
            currentTouches.clearTouch (touchIndex);

            if (! currentTouches.areAnyTouchesActive())
                isCancel = true;
        }

        if (isCancel)
        {
            currentTouches.clear();
            currentModifiers = currentModifiers.withoutMouseButtons();
        }

        handleMouseEvent (touchIndex, pos, modsToSend, time);
        if (! isValidPeer (this)) // (in case this component was deleted by the event)
            return;

        if (isUp || isCancel)
        {
            handleMouseEvent (touchIndex, Point<int> (-1, -1), currentModifiers, time);
            if (! isValidPeer (this))
                return;
        }
    }
}

//==============================================================================
static UIViewComponentPeer* currentlyFocusedPeer = nullptr;

void UIViewComponentPeer::viewFocusGain()
{
    if (currentlyFocusedPeer != this)
    {
        if (ComponentPeer::isValidPeer (currentlyFocusedPeer))
            currentlyFocusedPeer->handleFocusLoss();

        currentlyFocusedPeer = this;

        handleFocusGain();
    }
}

void UIViewComponentPeer::viewFocusLoss()
{
    if (currentlyFocusedPeer == this)
    {
        currentlyFocusedPeer = nullptr;
        handleFocusLoss();
    }
}

bool UIViewComponentPeer::isFocused() const
{
    return isSharedWindow ? this == currentlyFocusedPeer
                          : (window != nil && [window isKeyWindow]);
}

void UIViewComponentPeer::grabFocus()
{
    if (window != nil)
    {
        [window makeKeyWindow];
        viewFocusGain();
    }
}

void UIViewComponentPeer::textInputRequired (const Point<int>&)
{
}

void UIViewComponentPeer::updateHiddenTextContent (TextInputTarget* target)
{
    view->hiddenTextView.text = juceStringToNS (target->getTextInRange (Range<int> (0, target->getHighlightedRegion().getStart())));
    view->hiddenTextView.selectedRange = NSMakeRange (target->getHighlightedRegion().getStart(), 0);
}

BOOL UIViewComponentPeer::textViewReplaceCharacters (const Range<int>& range, const String& text)
{
    TextInputTarget* const target = findCurrentTextInputTarget();

    if (target != nullptr)
    {
        const Range<int> currentSelection (target->getHighlightedRegion());

        if (range.getLength() == 1 && text.isEmpty()) // (detect backspace)
            if (currentSelection.isEmpty())
                target->setHighlightedRegion (currentSelection.withStart (currentSelection.getStart() - 1));

        if (text == "\r" || text == "\n" || text == "\r\n")
            handleKeyPress (KeyPress::returnKey, text[0]);
        else
            target->insertTextAtCaret (text);

        updateHiddenTextContent (target);
    }

    return NO;
}

void UIViewComponentPeer::globalFocusChanged (Component*)
{
    TextInputTarget* const target = findCurrentTextInputTarget();

    if (target != nullptr)
    {
        Component* comp = dynamic_cast<Component*> (target);

        Point<int> pos (component->getLocalPoint (comp, Point<int>()));
        view->hiddenTextView.frame = CGRectMake (pos.getX(), pos.getY(), 0, 0);

        updateHiddenTextContent (target);
        [view->hiddenTextView becomeFirstResponder];
    }
    else
    {
        [view->hiddenTextView resignFirstResponder];
    }
}


//==============================================================================
void UIViewComponentPeer::drawRect (CGRect r)
{
    if (r.size.width < 1.0f || r.size.height < 1.0f)
        return;

    CGContextRef cg = UIGraphicsGetCurrentContext();

    if (! component->isOpaque())
        CGContextClearRect (cg, CGContextGetClipBoundingBox (cg));

    CGContextConcatCTM (cg, CGAffineTransformMake (1, 0, 0, -1, 0, view.bounds.size.height));
    CoreGraphicsContext g (cg, view.bounds.size.height);

    insideDrawRect = true;
    handlePaint (g);
    insideDrawRect = false;
}

bool UIViewComponentPeer::canBecomeKeyWindow()
{
    return (getStyleFlags() & juce::ComponentPeer::windowIgnoresKeyPresses) == 0;
}

bool UIViewComponentPeer::windowShouldClose()
{
    if (! isValidPeer (this))
        return YES;

    handleUserClosingWindow();
    return NO;
}

void UIViewComponentPeer::redirectMovedOrResized()
{
    handleMovedOrResized();
}

//==============================================================================
void Desktop::setKioskComponent (Component* kioskModeComponent, bool enableOrDisable, bool allowMenusAndBars)
{
    [[UIApplication sharedApplication] setStatusBarHidden: enableOrDisable
                                            withAnimation: UIStatusBarAnimationSlide];

    Desktop::getInstance().refreshMonitorSizes();

    ComponentPeer* const peer = kioskModeComponent->getPeer();

    if (peer != nullptr)
        peer->setFullScreen (enableOrDisable);
}

//==============================================================================
class AsyncRepaintMessage  : public CallbackMessage
{
public:
    UIViewComponentPeer* const peer;
    const Rectangle<int> rect;

    AsyncRepaintMessage (UIViewComponentPeer* const peer_, const Rectangle<int>& rect_)
        : peer (peer_), rect (rect_)
    {
    }

    void messageCallback()
    {
        if (ComponentPeer::isValidPeer (peer))
            peer->repaint (rect);
    }
};

void UIViewComponentPeer::repaint (const Rectangle<int>& area)
{
    if (insideDrawRect || ! MessageManager::getInstance()->isThisTheMessageThread())
    {
        (new AsyncRepaintMessage (this, area))->post();
    }
    else
    {
        [view setNeedsDisplayInRect: convertToCGRect (area)];
    }
}

void UIViewComponentPeer::performAnyPendingRepaintsNow()
{
}

ComponentPeer* Component::createNewPeer (int styleFlags, void* windowToAttachTo)
{
    return new UIViewComponentPeer (this, styleFlags, (UIView*) windowToAttachTo);
}

//==============================================================================
const int KeyPress::spaceKey        = ' ';
const int KeyPress::returnKey       = 0x0d;
const int KeyPress::escapeKey       = 0x1b;
const int KeyPress::backspaceKey    = 0x7f;
const int KeyPress::leftKey         = 0x1000;
const int KeyPress::rightKey        = 0x1001;
const int KeyPress::upKey           = 0x1002;
const int KeyPress::downKey         = 0x1003;
const int KeyPress::pageUpKey       = 0x1004;
const int KeyPress::pageDownKey     = 0x1005;
const int KeyPress::endKey          = 0x1006;
const int KeyPress::homeKey         = 0x1007;
const int KeyPress::deleteKey       = 0x1008;
const int KeyPress::insertKey       = -1;
const int KeyPress::tabKey          = 9;
const int KeyPress::F1Key           = 0x2001;
const int KeyPress::F2Key           = 0x2002;
const int KeyPress::F3Key           = 0x2003;
const int KeyPress::F4Key           = 0x2004;
const int KeyPress::F5Key           = 0x2005;
const int KeyPress::F6Key           = 0x2006;
const int KeyPress::F7Key           = 0x2007;
const int KeyPress::F8Key           = 0x2008;
const int KeyPress::F9Key           = 0x2009;
const int KeyPress::F10Key          = 0x200a;
const int KeyPress::F11Key          = 0x200b;
const int KeyPress::F12Key          = 0x200c;
const int KeyPress::F13Key          = 0x200d;
const int KeyPress::F14Key          = 0x200e;
const int KeyPress::F15Key          = 0x200f;
const int KeyPress::F16Key          = 0x2010;
const int KeyPress::numberPad0      = 0x30020;
const int KeyPress::numberPad1      = 0x30021;
const int KeyPress::numberPad2      = 0x30022;
const int KeyPress::numberPad3      = 0x30023;
const int KeyPress::numberPad4      = 0x30024;
const int KeyPress::numberPad5      = 0x30025;
const int KeyPress::numberPad6      = 0x30026;
const int KeyPress::numberPad7      = 0x30027;
const int KeyPress::numberPad8      = 0x30028;
const int KeyPress::numberPad9      = 0x30029;
const int KeyPress::numberPadAdd            = 0x3002a;
const int KeyPress::numberPadSubtract       = 0x3002b;
const int KeyPress::numberPadMultiply       = 0x3002c;
const int KeyPress::numberPadDivide         = 0x3002d;
const int KeyPress::numberPadSeparator      = 0x3002e;
const int KeyPress::numberPadDecimalPoint   = 0x3002f;
const int KeyPress::numberPadEquals         = 0x30030;
const int KeyPress::numberPadDelete         = 0x30031;
const int KeyPress::playKey         = 0x30000;
const int KeyPress::stopKey         = 0x30001;
const int KeyPress::fastForwardKey  = 0x30002;
const int KeyPress::rewindKey       = 0x30003;
