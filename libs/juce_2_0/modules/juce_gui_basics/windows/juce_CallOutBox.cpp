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

CallOutBox::CallOutBox (Component& contentComponent,
                        Component& componentToPointTo,
                        Component* const parent)
    : borderSpace (20), arrowSize (16.0f), content (contentComponent)
{
    addAndMakeVisible (&content);

    if (parent != nullptr)
    {
        parent->addChildComponent (this);

        updatePosition (parent->getLocalArea (&componentToPointTo, componentToPointTo.getLocalBounds()),
                        parent->getLocalBounds());

        setVisible (true);
    }
    else
    {
        if (! JUCEApplication::isStandaloneApp())
            setAlwaysOnTop (true); // for a plugin, make it always-on-top because the host windows are often top-level

        updatePosition (componentToPointTo.getScreenBounds(),
                        componentToPointTo.getParentMonitorArea());

        addToDesktop (ComponentPeer::windowIsTemporary);
    }
}

CallOutBox::~CallOutBox()
{
}

//==============================================================================
void CallOutBox::setArrowSize (const float newSize)
{
    arrowSize = newSize;
    borderSpace = jmax (20, (int) arrowSize);
    refreshPath();
}

void CallOutBox::paint (Graphics& g)
{
    if (background.isNull())
    {
        background = Image (Image::ARGB, getWidth(), getHeight(), true);
        Graphics g2 (background);
        getLookAndFeel().drawCallOutBoxBackground (*this, g2, outline);
    }

    g.setColour (Colours::black);
    g.drawImageAt (background, 0, 0);
}

void CallOutBox::resized()
{
    content.setTopLeftPosition (borderSpace, borderSpace);
    refreshPath();
}

void CallOutBox::moved()
{
    refreshPath();
}

void CallOutBox::childBoundsChanged (Component*)
{
    updatePosition (targetArea, availableArea);
}

bool CallOutBox::hitTest (int x, int y)
{
    return outline.contains ((float) x, (float) y);
}

enum { callOutBoxDismissCommandId = 0x4f83a04b };

void CallOutBox::inputAttemptWhenModal()
{
    const Point<int> mousePos (getMouseXYRelative() + getBounds().getPosition());

    if (targetArea.contains (mousePos))
    {
        // if you click on the area that originally popped-up the callout, you expect it
        // to get rid of the box, but deleting the box here allows the click to pass through and
        // probably re-trigger it, so we need to dismiss the box asynchronously to consume the click..
        postCommandMessage (callOutBoxDismissCommandId);
    }
    else
    {
        exitModalState (0);
        setVisible (false);
    }
}

void CallOutBox::handleCommandMessage (int commandId)
{
    Component::handleCommandMessage (commandId);

    if (commandId == callOutBoxDismissCommandId)
    {
        exitModalState (0);
        setVisible (false);
    }
}

bool CallOutBox::keyPressed (const KeyPress& key)
{
    if (key.isKeyCode (KeyPress::escapeKey))
    {
        inputAttemptWhenModal();
        return true;
    }

    return false;
}

void CallOutBox::updatePosition (const Rectangle<int>& newAreaToPointTo, const Rectangle<int>& newAreaToFitIn)
{
    targetArea = newAreaToPointTo;
    availableArea = newAreaToFitIn;

    Rectangle<int> newBounds (0, 0,
                              content.getWidth() + borderSpace * 2,
                              content.getHeight() + borderSpace * 2);

    const int hw = newBounds.getWidth() / 2;
    const int hh = newBounds.getHeight() / 2;
    const float hwReduced = (float) (hw - borderSpace * 3);
    const float hhReduced = (float) (hh - borderSpace * 3);
    const float arrowIndent = borderSpace - arrowSize;

    Point<float> targets[4] = { Point<float> ((float) targetArea.getCentreX(), (float) targetArea.getBottom()),
                                Point<float> ((float) targetArea.getRight(),   (float) targetArea.getCentreY()),
                                Point<float> ((float) targetArea.getX(),       (float) targetArea.getCentreY()),
                                Point<float> ((float) targetArea.getCentreX(), (float) targetArea.getY()) };

    Line<float> lines[4] = { Line<float> (targets[0].translated (-hwReduced, hh - arrowIndent),    targets[0].translated (hwReduced, hh - arrowIndent)),
                             Line<float> (targets[1].translated (hw - arrowIndent, -hhReduced),    targets[1].translated (hw - arrowIndent, hhReduced)),
                             Line<float> (targets[2].translated (-(hw - arrowIndent), -hhReduced), targets[2].translated (-(hw - arrowIndent), hhReduced)),
                             Line<float> (targets[3].translated (-hwReduced, -(hh - arrowIndent)), targets[3].translated (hwReduced, -(hh - arrowIndent))) };

    const Rectangle<float> centrePointArea (newAreaToFitIn.reduced (hw, hh).toFloat());

    float nearest = 1.0e9f;

    for (int i = 0; i < 4; ++i)
    {
        Line<float> constrainedLine (centrePointArea.getConstrainedPoint (lines[i].getStart()),
                                     centrePointArea.getConstrainedPoint (lines[i].getEnd()));

        const Point<float> centre (constrainedLine.findNearestPointTo (centrePointArea.getCentre()));
        float distanceFromCentre = centre.getDistanceFrom (centrePointArea.getCentre());

        if (! (centrePointArea.contains (lines[i].getStart()) || centrePointArea.contains (lines[i].getEnd())))
            distanceFromCentre *= 2.0f;

        if (distanceFromCentre < nearest)
        {
            nearest = distanceFromCentre;

            targetPoint = targets[i];
            newBounds.setPosition ((int) (centre.getX() - hw),
                                   (int) (centre.getY() - hh));
        }
    }

    setBounds (newBounds);
}

void CallOutBox::refreshPath()
{
    repaint();
    background = Image::null;
    outline.clear();

    const float gap = 4.5f;
    const float cornerSize = 9.0f;
    const float cornerSize2 = 2.0f * cornerSize;
    const float arrowBaseWidth = arrowSize * 0.7f;

    const Rectangle<float> area (content.getBounds().toFloat().expanded (gap, gap));
    const Point<float> target (targetPoint - getPosition().toFloat());

    outline.startNewSubPath (area.getX() + cornerSize, area.getY());

    const float targetLimitX = area.getX() + cornerSize + arrowBaseWidth;
    const float targetLimitW = area.getWidth() - cornerSize2 - arrowBaseWidth * 2.0f;

    const float targetLimitY = area.getY() + cornerSize + arrowBaseWidth;
    const float targetLimitH = area.getHeight() - cornerSize2 - arrowBaseWidth * 2.0f;

    if (Rectangle<float> (targetLimitX, 1.0f,
                          targetLimitW, area.getY() - 2.0f).contains (target))
    {
        outline.lineTo (target.x - arrowBaseWidth, area.getY());
        outline.lineTo (target.x, target.y);
        outline.lineTo (target.x + arrowBaseWidth, area.getY());
    }

    outline.lineTo (area.getRight() - cornerSize, area.getY());
    outline.addArc (area.getRight() - cornerSize2, area.getY(), cornerSize2, cornerSize2, 0, float_Pi * 0.5f);

    if (Rectangle<float> (area.getRight() + 1.0f, targetLimitY,
                          getWidth() - area.getRight() - 2.0f, targetLimitH).contains (target))
    {
        outline.lineTo (area.getRight(), target.y - arrowBaseWidth);
        outline.lineTo (target.x, target.y);
        outline.lineTo (area.getRight(), target.y + arrowBaseWidth);
    }

    outline.lineTo (area.getRight(), area.getBottom() - cornerSize);
    outline.addArc (area.getRight() - cornerSize2, area.getBottom() - cornerSize2, cornerSize2, cornerSize2, float_Pi * 0.5f, float_Pi);

    if (Rectangle<float> (targetLimitX, area.getBottom() + 1.0f,
                          targetLimitW, getHeight() - area.getBottom() - 2.0f).contains (target))
    {
        outline.lineTo (target.x + arrowBaseWidth, area.getBottom());
        outline.lineTo (target.x, target.y);
        outline.lineTo (target.x - arrowBaseWidth, area.getBottom());
    }

    outline.lineTo (area.getX() + cornerSize, area.getBottom());
    outline.addArc (area.getX(), area.getBottom() - cornerSize2, cornerSize2, cornerSize2, float_Pi, float_Pi * 1.5f);

    if (Rectangle<float> (1.0f, targetLimitY, area.getX() - 2.0f, targetLimitH).contains (target))
    {
        outline.lineTo (area.getX(), target.y + arrowBaseWidth);
        outline.lineTo (target.x, target.y);
        outline.lineTo (area.getX(), target.y - arrowBaseWidth);
    }

    outline.lineTo (area.getX(), area.getY() + cornerSize);
    outline.addArc (area.getX(), area.getY(), cornerSize2, cornerSize2, float_Pi * 1.5f, float_Pi * 2.0f - 0.05f);

    outline.closeSubPath();
}
