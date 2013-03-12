/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-9 by Raw Material Software Ltd.

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

#include "../JuceLibraryCode/JuceHeader.h"
#include "GraphEditorPanel.h"
#include "InternalFilters.h"
#include "MainHostWindow.h"


//==============================================================================
class PluginWindow;
static Array <PluginWindow*> activePluginWindows;

PluginWindow::PluginWindow (Component* const uiComp,
                            AudioProcessorGraph::Node* owner_,
                            const bool isGeneric_)
    : DocumentWindow (uiComp->getName(), Colours::lightblue,
                      DocumentWindow::minimiseButton | DocumentWindow::closeButton),
      owner (owner_),
      isGeneric (isGeneric_)
{
    setSize (400, 300);

    setContentOwned (uiComp, true);

    setTopLeftPosition (owner->properties.getWithDefault ("uiLastX", Random::getSystemRandom().nextInt (500)),
                        owner->properties.getWithDefault ("uiLastY", Random::getSystemRandom().nextInt (500)));
    setVisible (true);

    activePluginWindows.add (this);
}

void PluginWindow::closeCurrentlyOpenWindowsFor (const uint32 nodeId)
{
    for (int i = activePluginWindows.size(); --i >= 0;)
        if (activePluginWindows.getUnchecked(i)->owner->nodeId == nodeId)
            delete activePluginWindows.getUnchecked(i);
}

void PluginWindow::closeAllCurrentlyOpenWindows()
{
    for (int i = activePluginWindows.size(); --i >= 0;)
        delete activePluginWindows.getUnchecked(i);
}

PluginWindow* PluginWindow::getWindowFor (AudioProcessorGraph::Node* node,
                                          bool useGenericView)
{
    for (int i = activePluginWindows.size(); --i >= 0;)
        if (activePluginWindows.getUnchecked(i)->owner == node
             && activePluginWindows.getUnchecked(i)->isGeneric == useGenericView)
            return activePluginWindows.getUnchecked(i);

    AudioProcessorEditor* ui = nullptr;

    if (! useGenericView)
    {
        ui = node->getProcessor()->createEditorIfNeeded();

        if (ui == nullptr)
            useGenericView = true;
    }

    if (useGenericView)
    {
        ui = new GenericAudioProcessorEditor (node->getProcessor());
    }

    if (ui != nullptr)
    {
        AudioPluginInstance* const plugin = dynamic_cast <AudioPluginInstance*> (node->getProcessor());

        if (plugin != nullptr)
            ui->setName (plugin->getName());

        return new PluginWindow (ui, node, useGenericView);
    }

    return nullptr;
}

PluginWindow::~PluginWindow()
{
    activePluginWindows.removeValue (this);
    clearContentComponent();
}

void PluginWindow::moved()
{
    owner->properties.set ("uiLastX", getX());
    owner->properties.set ("uiLastY", getY());
}

void PluginWindow::closeButtonPressed()
{
    delete this;
}

//==============================================================================
class PinComponent   : public Component,
                       public SettableTooltipClient
{
public:
    PinComponent (FilterGraph& graph_,
                  const uint32 filterID_, const int index_, const bool isInput_)
        : filterID (filterID_),
          index (index_),
          isInput (isInput_),
          graph (graph_)
    {
        const AudioProcessorGraph::Node::Ptr node (graph.getNodeForId (filterID_));

        if (node != nullptr)
        {
            String tip;

            if (isInput)
                tip = node->getProcessor()->getInputChannelName (index_);
            else
                tip = node->getProcessor()->getOutputChannelName (index_);

            if (tip.isEmpty())
            {
                if (index_ == FilterGraph::midiChannelNumber)
                    tip = isInput ? "Midi Input" : "Midi Output";
                else
                    tip = (isInput ? "Input " : "Output ") + String (index_ + 1);
            }

            setTooltip (tip);
        }

        setSize (16, 16);
    }

    void paint (Graphics& g)
    {
        const float w = (float) getWidth();
        const float h = (float) getHeight();

        Path p;
        p.addEllipse (w * 0.25f, h * 0.25f, w * 0.5f, h * 0.5f);

        p.addRectangle (w * 0.4f, isInput ? (0.5f * h) : 0.0f, w * 0.2f, h * 0.5f);

        g.setColour (index == FilterGraph::midiChannelNumber ? Colours::red : Colours::green);
        g.fillPath (p);
    }

    void mouseDown (const MouseEvent& e)
    {
        getGraphPanel()->beginConnectorDrag (isInput ? 0 : filterID,
                                             index,
                                             isInput ? filterID : 0,
                                             index,
                                             e);
    }

    void mouseDrag (const MouseEvent& e)
    {
        getGraphPanel()->dragConnector (e);
    }

    void mouseUp (const MouseEvent& e)
    {
        getGraphPanel()->endDraggingConnector (e);
    }

    const uint32 filterID;
    const int index;
    const bool isInput;

private:
    FilterGraph& graph;

    GraphEditorPanel* getGraphPanel() const noexcept
    {
        return findParentComponentOfClass ((GraphEditorPanel*) nullptr);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PinComponent);
};

//==============================================================================
class FilterComponent    : public Component
{
public:
    FilterComponent (FilterGraph& graph_,
                     const uint32 filterID_)
        : graph (graph_),
          filterID (filterID_),
          numInputs (0),
          numOutputs (0),
          pinSize (16),
          font (13.0f, Font::bold),
          numIns (0),
          numOuts (0)
    {
        shadow.setShadowProperties (2.5f, 0.5f, -1, 0);
        setComponentEffect (&shadow);

        setSize (150, 60);
    }

    ~FilterComponent()
    {
        deleteAllChildren();
    }

    void mouseDown (const MouseEvent& e)
    {
        originalPos = localPointToGlobal (Point<int>());

        toFront (true);

        if (e.mods.isPopupMenu())
        {
            PopupMenu m;
            m.addItem (1, "Delete this filter");
            m.addItem (2, "Disconnect all pins");
            m.addSeparator();
            m.addItem (3, "Show plugin UI");
            m.addItem (4, "Show all parameters");

            const int r = m.show();

            if (r == 1)
            {
                graph.removeFilter (filterID);
                return;
            }
            else if (r == 2)
            {
                graph.disconnectFilter (filterID);
            }
            else if (r == 3 || r == 4)
            {
                AudioProcessorGraph::Node::Ptr f (graph.getNodeForId (filterID));

                if (f != nullptr)
                {
                    PluginWindow* const w = PluginWindow::getWindowFor (f, r == 4);

                    if (w != nullptr)
                        w->toFront (true);
                }
            }
        }
    }

    void mouseDrag (const MouseEvent& e)
    {
        if (! e.mods.isPopupMenu())
        {
            Point<int> pos (originalPos + Point<int> (e.getDistanceFromDragStartX(), e.getDistanceFromDragStartY()));

            if (getParentComponent() != nullptr)
                pos = getParentComponent()->getLocalPoint (nullptr, pos);

            graph.setNodePosition (filterID,
                                   (pos.getX() + getWidth() / 2) / (double) getParentWidth(),
                                   (pos.getY() + getHeight() / 2) / (double) getParentHeight());

            getGraphPanel()->updateComponents();
        }
    }

    void mouseUp (const MouseEvent& e)
    {
        if (e.mouseWasClicked() && e.getNumberOfClicks() == 2)
        {
            const AudioProcessorGraph::Node::Ptr f (graph.getNodeForId (filterID));

            if (f != nullptr)
            {
                PluginWindow* const w = PluginWindow::getWindowFor (f, false);

                if (w != nullptr)
                    w->toFront (true);
            }
        }
        else if (! e.mouseWasClicked())
        {
            graph.setChangedFlag (true);
        }
    }

    bool hitTest (int x, int y)
    {
        for (int i = getNumChildComponents(); --i >= 0;)
            if (getChildComponent(i)->getBounds().contains (x, y))
                return true;

        return x >= 3 && x < getWidth() - 6 && y >= pinSize && y < getHeight() - pinSize;
    }

    void paint (Graphics& g)
    {
        g.setColour (Colours::lightgrey);

        const int x = 4;
        const int y = pinSize;
        const int w = getWidth() - x * 2;
        const int h = getHeight() - pinSize * 2;

        g.fillRect (x, y, w, h);

        g.setColour (Colours::black);
        g.setFont (font);
        g.drawFittedText (getName(),
                          x + 4, y + 2, w - 8, h - 4,
                          Justification::centred, 2);

        g.setColour (Colours::grey);
        g.drawRect (x, y, w, h);
    }

    void resized()
    {
        for (int i = 0; i < getNumChildComponents(); ++i)
        {
            PinComponent* const pc = dynamic_cast <PinComponent*> (getChildComponent(i));

            if (pc != nullptr)
            {
                const int total = pc->isInput ? numIns : numOuts;
                const int index = pc->index == FilterGraph::midiChannelNumber ? (total - 1) : pc->index;

                pc->setBounds (proportionOfWidth ((1 + index) / (total + 1.0f)) - pinSize / 2,
                               pc->isInput ? 0 : (getHeight() - pinSize),
                               pinSize, pinSize);
            }
        }
    }

    void getPinPos (const int index, const bool isInput, float& x, float& y)
    {
        for (int i = 0; i < getNumChildComponents(); ++i)
        {
            PinComponent* const pc = dynamic_cast <PinComponent*> (getChildComponent(i));

            if (pc != nullptr && pc->index == index && isInput == pc->isInput)
            {
                x = getX() + pc->getX() + pc->getWidth() * 0.5f;
                y = getY() + pc->getY() + pc->getHeight() * 0.5f;
                break;
            }
        }
    }

    void update()
    {
        const AudioProcessorGraph::Node::Ptr f (graph.getNodeForId (filterID));

        if (f == nullptr)
        {
            delete this;
            return;
        }

        numIns = f->getProcessor()->getNumInputChannels();
        if (f->getProcessor()->acceptsMidi())
            ++numIns;

        numOuts = f->getProcessor()->getNumOutputChannels();
        if (f->getProcessor()->producesMidi())
            ++numOuts;

        int w = 100;
        int h = 60;

        w = jmax (w, (jmax (numIns, numOuts) + 1) * 20);

        const int textWidth = font.getStringWidth (f->getProcessor()->getName());
        w = jmax (w, 16 + jmin (textWidth, 300));
        if (textWidth > 300)
            h = 100;

        setSize (w, h);

        setName (f->getProcessor()->getName());

        {
            double x, y;
            graph.getNodePosition (filterID, x, y);
            setCentreRelative ((float) x, (float) y);
        }

        if (numIns != numInputs || numOuts != numOutputs)
        {
            numInputs = numIns;
            numOutputs = numOuts;

            deleteAllChildren();

            int i;
            for (i = 0; i < f->getProcessor()->getNumInputChannels(); ++i)
                addAndMakeVisible (new PinComponent (graph, filterID, i, true));

            if (f->getProcessor()->acceptsMidi())
                addAndMakeVisible (new PinComponent (graph, filterID, FilterGraph::midiChannelNumber, true));

            for (i = 0; i < f->getProcessor()->getNumOutputChannels(); ++i)
                addAndMakeVisible (new PinComponent (graph, filterID, i, false));

            if (f->getProcessor()->producesMidi())
                addAndMakeVisible (new PinComponent (graph, filterID, FilterGraph::midiChannelNumber, false));

            resized();
        }
    }

    FilterGraph& graph;
    const uint32 filterID;
    int numInputs, numOutputs;

private:
    int pinSize;
    Point<int> originalPos;
    Font font;
    int numIns, numOuts;
    DropShadowEffect shadow;

    GraphEditorPanel* getGraphPanel() const noexcept
    {
        return findParentComponentOfClass ((GraphEditorPanel*) nullptr);
    }

    FilterComponent (const FilterComponent&);
    FilterComponent& operator= (const FilterComponent&);
};

//==============================================================================
class ConnectorComponent   : public Component,
                             public SettableTooltipClient
{
public:
    ConnectorComponent (FilterGraph& graph_)
        : sourceFilterID (0),
          destFilterID (0),
          sourceFilterChannel (0),
          destFilterChannel (0),
          graph (graph_),
          lastInputX (0),
          lastInputY (0),
          lastOutputX (0),
          lastOutputY (0)
    {
        setAlwaysOnTop (true);
    }

    ~ConnectorComponent()
    {
    }

    void setInput (const uint32 sourceFilterID_, const int sourceFilterChannel_)
    {
        if (sourceFilterID != sourceFilterID_ || sourceFilterChannel != sourceFilterChannel_)
        {
            sourceFilterID = sourceFilterID_;
            sourceFilterChannel = sourceFilterChannel_;
            update();
        }
    }

    void setOutput (const uint32 destFilterID_, const int destFilterChannel_)
    {
        if (destFilterID != destFilterID_ || destFilterChannel != destFilterChannel_)
        {
            destFilterID = destFilterID_;
            destFilterChannel = destFilterChannel_;
            update();
        }
    }

    void dragStart (int x, int y)
    {
        lastInputX = (float) x;
        lastInputY = (float) y;
        resizeToFit();
    }

    void dragEnd (int x, int y)
    {
        lastOutputX = (float) x;
        lastOutputY = (float) y;
        resizeToFit();
    }

    void update()
    {
        float x1, y1, x2, y2;
        getPoints (x1, y1, x2, y2);

        if (lastInputX != x1
             || lastInputY != y1
             || lastOutputX != x2
             || lastOutputY != y2)
        {
            resizeToFit();
        }
    }

    void resizeToFit()
    {
        float x1, y1, x2, y2;
        getPoints (x1, y1, x2, y2);

        const Rectangle<int> newBounds ((int) jmin (x1, x2) - 4,
                                        (int) jmin (y1, y2) - 4,
                                        (int) fabsf (x1 - x2) + 8,
                                        (int) fabsf (y1 - y2) + 8);

        if (newBounds != getBounds())
            setBounds (newBounds);
        else
            resized();

        repaint();
    }

    void getPoints (float& x1, float& y1, float& x2, float& y2) const
    {
        x1 = lastInputX;
        y1 = lastInputY;
        x2 = lastOutputX;
        y2 = lastOutputY;

        GraphEditorPanel* const hostPanel = getGraphPanel();

        if (hostPanel != nullptr)
        {
            FilterComponent* srcFilterComp = hostPanel->getComponentForFilter (sourceFilterID);

            if (srcFilterComp != nullptr)
                srcFilterComp->getPinPos (sourceFilterChannel, false, x1, y1);

            FilterComponent* dstFilterComp = hostPanel->getComponentForFilter (destFilterID);

            if (dstFilterComp != nullptr)
                dstFilterComp->getPinPos (destFilterChannel, true, x2, y2);
        }
    }

    void paint (Graphics& g)
    {
        if (sourceFilterChannel == FilterGraph::midiChannelNumber
             || destFilterChannel == FilterGraph::midiChannelNumber)
        {
            g.setColour (Colours::red);
        }
        else
        {
            g.setColour (Colours::green);
        }

        g.fillPath (linePath);
    }

    bool hitTest (int x, int y)
    {
        if (hitPath.contains ((float) x, (float) y))
        {
            double distanceFromStart, distanceFromEnd;
            getDistancesFromEnds (x, y, distanceFromStart, distanceFromEnd);

            // avoid clicking the connector when over a pin
            return distanceFromStart > 7.0 && distanceFromEnd > 7.0;
        }

        return false;
    }

    void mouseDown (const MouseEvent&)
    {
        dragging = false;
    }

    void mouseDrag (const MouseEvent& e)
    {
        if ((! dragging) && ! e.mouseWasClicked())
        {
            dragging = true;

            graph.removeConnection (sourceFilterID, sourceFilterChannel, destFilterID, destFilterChannel);

            double distanceFromStart, distanceFromEnd;
            getDistancesFromEnds (e.x, e.y, distanceFromStart, distanceFromEnd);
            const bool isNearerSource = (distanceFromStart < distanceFromEnd);

            getGraphPanel()->beginConnectorDrag (isNearerSource ? 0 : sourceFilterID,
                                                 sourceFilterChannel,
                                                 isNearerSource ? destFilterID : 0,
                                                 destFilterChannel,
                                                 e);
        }
        else if (dragging)
        {
            getGraphPanel()->dragConnector (e);
        }
    }

    void mouseUp (const MouseEvent& e)
    {
        if (dragging)
            getGraphPanel()->endDraggingConnector (e);
    }

    void resized()
    {
        float x1, y1, x2, y2;
        getPoints (x1, y1, x2, y2);

        lastInputX = x1;
        lastInputY = y1;
        lastOutputX = x2;
        lastOutputY = y2;

        x1 -= getX();
        y1 -= getY();
        x2 -= getX();
        y2 -= getY();

        linePath.clear();
        linePath.startNewSubPath (x1, y1);
        linePath.cubicTo (x1, y1 + (y2 - y1) * 0.33f,
                          x2, y1 + (y2 - y1) * 0.66f,
                          x2, y2);

        PathStrokeType wideStroke (8.0f);
        wideStroke.createStrokedPath (hitPath, linePath);

        PathStrokeType stroke (2.5f);
        stroke.createStrokedPath (linePath, linePath);

        const float arrowW = 5.0f;
        const float arrowL = 4.0f;

        Path arrow;
        arrow.addTriangle (-arrowL, arrowW,
                           -arrowL, -arrowW,
                           arrowL, 0.0f);

        arrow.applyTransform (AffineTransform::identity
                                .rotated (float_Pi * 0.5f - (float) atan2 (x2 - x1, y2 - y1))
                                .translated ((x1 + x2) * 0.5f,
                                             (y1 + y2) * 0.5f));

        linePath.addPath (arrow);
        linePath.setUsingNonZeroWinding (true);
    }

    uint32 sourceFilterID, destFilterID;
    int sourceFilterChannel, destFilterChannel;

private:
    FilterGraph& graph;
    float lastInputX, lastInputY, lastOutputX, lastOutputY;
    Path linePath, hitPath;
    bool dragging;

    GraphEditorPanel* getGraphPanel() const noexcept
    {
        return findParentComponentOfClass ((GraphEditorPanel*) nullptr);
    }

    void getDistancesFromEnds (int x, int y, double& distanceFromStart, double& distanceFromEnd) const
    {
        float x1, y1, x2, y2;
        getPoints (x1, y1, x2, y2);

        distanceFromStart = juce_hypot (x - (x1 - getX()), y - (y1 - getY()));
        distanceFromEnd = juce_hypot (x - (x2 - getX()), y - (y2 - getY()));
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConnectorComponent);
};


//==============================================================================
GraphEditorPanel::GraphEditorPanel (FilterGraph& graph_)
    : graph (graph_),
      draggingConnector (nullptr)
{
    graph.addChangeListener (this);
    setOpaque (true);
}

GraphEditorPanel::~GraphEditorPanel()
{
    graph.removeChangeListener (this);
    deleteAndZero (draggingConnector);
    deleteAllChildren();
}

void GraphEditorPanel::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void GraphEditorPanel::mouseDown (const MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        PopupMenu m;

        MainHostWindow* const mainWindow = findParentComponentOfClass ((MainHostWindow*) nullptr);

        if (mainWindow != nullptr)
        {
            mainWindow->addPluginsToMenu (m);

            const int r = m.show();

            createNewPlugin (mainWindow->getChosenType (r), e.x, e.y);
        }
    }
}

void GraphEditorPanel::createNewPlugin (const PluginDescription* desc, int x, int y)
{
    graph.addFilter (desc, x / (double) getWidth(), y / (double) getHeight());
}

FilterComponent* GraphEditorPanel::getComponentForFilter (const uint32 filterID) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        FilterComponent* const fc = dynamic_cast <FilterComponent*> (getChildComponent (i));

        if (fc != nullptr && fc->filterID == filterID)
            return fc;
    }

    return nullptr;
}

ConnectorComponent* GraphEditorPanel::getComponentForConnection (const AudioProcessorGraph::Connection& conn) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        ConnectorComponent* const c = dynamic_cast <ConnectorComponent*> (getChildComponent (i));

        if (c != nullptr
             && c->sourceFilterID == conn.sourceNodeId
             && c->destFilterID == conn.destNodeId
             && c->sourceFilterChannel == conn.sourceChannelIndex
             && c->destFilterChannel == conn.destChannelIndex)
        {
            return c;
        }
    }

    return nullptr;
}

PinComponent* GraphEditorPanel::findPinAt (const int x, const int y) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        FilterComponent* const fc = dynamic_cast <FilterComponent*> (getChildComponent (i));

        if (fc != nullptr)
        {
            PinComponent* const pin
                = dynamic_cast <PinComponent*> (fc->getComponentAt (x - fc->getX(),
                                                                    y - fc->getY()));

            if (pin != nullptr)
                return pin;
        }
    }

    return nullptr;
}

void GraphEditorPanel::resized()
{
    updateComponents();
}

void GraphEditorPanel::changeListenerCallback (ChangeBroadcaster*)
{
    updateComponents();
}

void GraphEditorPanel::updateComponents()
{
    int i;
    for (i = getNumChildComponents(); --i >= 0;)
    {
        FilterComponent* const fc = dynamic_cast <FilterComponent*> (getChildComponent (i));

        if (fc != nullptr)
            fc->update();
    }

    for (i = getNumChildComponents(); --i >= 0;)
    {
        ConnectorComponent* const cc = dynamic_cast <ConnectorComponent*> (getChildComponent (i));

        if (cc != nullptr && cc != draggingConnector)
        {
            if (graph.getConnectionBetween (cc->sourceFilterID, cc->sourceFilterChannel,
                                            cc->destFilterID, cc->destFilterChannel) == nullptr)
            {
                delete cc;
            }
            else
            {
                cc->update();
            }
        }
    }

    for (i = graph.getNumFilters(); --i >= 0;)
    {
        const AudioProcessorGraph::Node::Ptr f (graph.getNode (i));

        if (getComponentForFilter (f->nodeId) == 0)
        {
            FilterComponent* const comp = new FilterComponent (graph, f->nodeId);
            addAndMakeVisible (comp);
            comp->update();
        }
    }

    for (i = graph.getNumConnections(); --i >= 0;)
    {
        const AudioProcessorGraph::Connection* const c = graph.getConnection (i);

        if (getComponentForConnection (*c) == 0)
        {
            ConnectorComponent* const comp = new ConnectorComponent (graph);
            addAndMakeVisible (comp);

            comp->setInput (c->sourceNodeId, c->sourceChannelIndex);
            comp->setOutput (c->destNodeId, c->destChannelIndex);
        }
    }
}

void GraphEditorPanel::beginConnectorDrag (const uint32 sourceFilterID, const int sourceFilterChannel,
                                           const uint32 destFilterID, const int destFilterChannel,
                                           const MouseEvent& e)
{
    delete draggingConnector;
    draggingConnector = dynamic_cast <ConnectorComponent*> (e.originalComponent);

    if (draggingConnector == nullptr)
        draggingConnector = new ConnectorComponent (graph);

    draggingConnector->setInput (sourceFilterID, sourceFilterChannel);
    draggingConnector->setOutput (destFilterID, destFilterChannel);

    addAndMakeVisible (draggingConnector);
    draggingConnector->toFront (false);

    dragConnector (e);
}

void GraphEditorPanel::dragConnector (const MouseEvent& e)
{
    const MouseEvent e2 (e.getEventRelativeTo (this));

    if (draggingConnector != nullptr)
    {
        draggingConnector->setTooltip (String::empty);

        int x = e2.x;
        int y = e2.y;

        PinComponent* const pin = findPinAt (x, y);

        if (pin != nullptr)
        {
            uint32 srcFilter = draggingConnector->sourceFilterID;
            int srcChannel = draggingConnector->sourceFilterChannel;
            uint32 dstFilter = draggingConnector->destFilterID;
            int dstChannel = draggingConnector->destFilterChannel;

            if (srcFilter == 0 && ! pin->isInput)
            {
                srcFilter = pin->filterID;
                srcChannel = pin->index;
            }
            else if (dstFilter == 0 && pin->isInput)
            {
                dstFilter = pin->filterID;
                dstChannel = pin->index;
            }

            if (graph.canConnect (srcFilter, srcChannel, dstFilter, dstChannel))
            {
                x = pin->getParentComponent()->getX() + pin->getX() + pin->getWidth() / 2;
                y = pin->getParentComponent()->getY() + pin->getY() + pin->getHeight() / 2;

                draggingConnector->setTooltip (pin->getTooltip());
            }
        }

        if (draggingConnector->sourceFilterID == 0)
            draggingConnector->dragStart (x, y);
        else
            draggingConnector->dragEnd (x, y);
    }
}

void GraphEditorPanel::endDraggingConnector (const MouseEvent& e)
{
    if (draggingConnector == nullptr)
        return;

    draggingConnector->setTooltip (String::empty);

    const MouseEvent e2 (e.getEventRelativeTo (this));

    uint32 srcFilter = draggingConnector->sourceFilterID;
    int srcChannel = draggingConnector->sourceFilterChannel;
    uint32 dstFilter = draggingConnector->destFilterID;
    int dstChannel = draggingConnector->destFilterChannel;

    deleteAndZero (draggingConnector);

    PinComponent* const pin = findPinAt (e2.x, e2.y);

    if (pin != nullptr)
    {
        if (srcFilter == 0)
        {
            if (pin->isInput)
                return;

            srcFilter = pin->filterID;
            srcChannel = pin->index;
        }
        else
        {
            if (! pin->isInput)
                return;

            dstFilter = pin->filterID;
            dstChannel = pin->index;
        }

        graph.addConnection (srcFilter, srcChannel, dstFilter, dstChannel);
    }
}


//==============================================================================
class TooltipBar   : public Component,
                     private Timer
{
public:
    TooltipBar()
    {
        startTimer (100);
    }

    void paint (Graphics& g)
    {
        g.setFont (getHeight() * 0.7f, Font::bold);
        g.setColour (Colours::black);
        g.drawFittedText (tip, 10, 0, getWidth() - 12, getHeight(), Justification::centredLeft, 1);
    }

    void timerCallback()
    {
        Component* const underMouse = Desktop::getInstance().getMainMouseSource().getComponentUnderMouse();
        TooltipClient* const ttc = dynamic_cast <TooltipClient*> (underMouse);

        String newTip;

        if (ttc != nullptr && ! (underMouse->isMouseButtonDown() || underMouse->isCurrentlyBlockedByAnotherModalComponent()))
            newTip = ttc->getTooltip();

        if (newTip != tip)
        {
            tip = newTip;
            repaint();
        }
    }

private:
    String tip;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TooltipBar);
};

//==============================================================================
GraphDocumentComponent::GraphDocumentComponent (AudioDeviceManager* deviceManager_)
    : deviceManager (deviceManager_)
{
    addAndMakeVisible (graphPanel = new GraphEditorPanel (graph));

    graphPlayer.setProcessor (&graph.getGraph());

    keyState.addListener (&graphPlayer.getMidiMessageCollector());

    addAndMakeVisible (keyboardComp = new MidiKeyboardComponent (keyState,
                                                                 MidiKeyboardComponent::horizontalKeyboard));

    addAndMakeVisible (statusBar = new TooltipBar());

    deviceManager->addAudioCallback (&graphPlayer);
    deviceManager->addMidiInputCallback (String::empty, &graphPlayer.getMidiMessageCollector());

    graphPanel->updateComponents();
}

GraphDocumentComponent::~GraphDocumentComponent()
{
    deviceManager->removeAudioCallback (&graphPlayer);
    deviceManager->removeMidiInputCallback (String::empty, &graphPlayer.getMidiMessageCollector());

    deleteAllChildren();

    graphPlayer.setProcessor (nullptr);
    keyState.removeListener (&graphPlayer.getMidiMessageCollector());

    graph.clear();
}

void GraphDocumentComponent::resized()
{
    const int keysHeight = 60;
    const int statusHeight = 20;

    graphPanel->setBounds (0, 0, getWidth(), getHeight() - keysHeight);
    statusBar->setBounds (0, getHeight() - keysHeight - statusHeight, getWidth(), statusHeight);
    keyboardComp->setBounds (0, getHeight() - keysHeight, getWidth(), keysHeight);
}

void GraphDocumentComponent::createNewPlugin (const PluginDescription* desc, int x, int y)
{
    graphPanel->createNewPlugin (desc, x, y);
}
