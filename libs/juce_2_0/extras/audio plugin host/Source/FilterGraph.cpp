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
#include "MainHostWindow.h"
#include "FilterGraph.h"
#include "InternalFilters.h"
#include "GraphEditorPanel.h"


//==============================================================================
FilterConnection::FilterConnection (FilterGraph& owner_)
    : owner (owner_)
{
}

FilterConnection::FilterConnection (const FilterConnection& other)
    : sourceFilterID (other.sourceFilterID),
      sourceChannel (other.sourceChannel),
      destFilterID (other.destFilterID),
      destChannel (other.destChannel),
      owner (other.owner)
{
}

FilterConnection::~FilterConnection()
{
}


//==============================================================================
const int FilterGraph::midiChannelNumber = 0x1000;

FilterGraph::FilterGraph()
    : FileBasedDocument (filenameSuffix,
                         filenameWildcard,
                         "Load a filter graph",
                         "Save a filter graph"),
      lastUID (0)
{
    InternalPluginFormat internalFormat;

    addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::audioInputFilter),
               0.5f, 0.1f);

    addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::midiInputFilter),
               0.25f, 0.1f);

    addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::audioOutputFilter),
               0.5f, 0.9f);

    setChangedFlag (false);
}

FilterGraph::~FilterGraph()
{
    graph.clear();
}

uint32 FilterGraph::getNextUID() noexcept
{
    return ++lastUID;
}

//==============================================================================
int FilterGraph::getNumFilters() const noexcept
{
    return graph.getNumNodes();
}

const AudioProcessorGraph::Node::Ptr FilterGraph::getNode (const int index) const noexcept
{
    return graph.getNode (index);
}

const AudioProcessorGraph::Node::Ptr FilterGraph::getNodeForId (const uint32 uid) const noexcept
{
    return graph.getNodeForId (uid);
}

void FilterGraph::addFilter (const PluginDescription* desc, double x, double y)
{
    if (desc != nullptr)
    {
        String errorMessage;

        AudioPluginInstance* instance
            = AudioPluginFormatManager::getInstance()->createPluginInstance (*desc, errorMessage);

        AudioProcessorGraph::Node* node = nullptr;

        if (instance != nullptr)
            node = graph.addNode (instance);

        if (node != nullptr)
        {
            node->properties.set ("x", x);
            node->properties.set ("y", y);
            changed();
        }
        else
        {
            AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                         TRANS("Couldn't create filter"),
                                         errorMessage);
        }
    }
}

void FilterGraph::removeFilter (const uint32 id)
{
    PluginWindow::closeCurrentlyOpenWindowsFor (id);

    if (graph.removeNode (id))
        changed();
}

void FilterGraph::disconnectFilter (const uint32 id)
{
    if (graph.disconnectNode (id))
        changed();
}

void FilterGraph::removeIllegalConnections()
{
    if (graph.removeIllegalConnections())
        changed();
}

void FilterGraph::setNodePosition (const int nodeId, double x, double y)
{
    const AudioProcessorGraph::Node::Ptr n (graph.getNodeForId (nodeId));

    if (n != nullptr)
    {
        n->properties.set ("x", jlimit (0.0, 1.0, x));
        n->properties.set ("y", jlimit (0.0, 1.0, y));
    }
}

void FilterGraph::getNodePosition (const int nodeId, double& x, double& y) const
{
    x = y = 0;

    const AudioProcessorGraph::Node::Ptr n (graph.getNodeForId (nodeId));

    if (n != nullptr)
    {
        x = (double) n->properties ["x"];
        y = (double) n->properties ["y"];
    }
}

//==============================================================================
int FilterGraph::getNumConnections() const noexcept
{
    return graph.getNumConnections();
}

const AudioProcessorGraph::Connection* FilterGraph::getConnection (const int index) const noexcept
{
    return graph.getConnection (index);
}

const AudioProcessorGraph::Connection* FilterGraph::getConnectionBetween (uint32 sourceFilterUID, int sourceFilterChannel,
                                                                          uint32 destFilterUID, int destFilterChannel) const noexcept
{
    return graph.getConnectionBetween (sourceFilterUID, sourceFilterChannel,
                                       destFilterUID, destFilterChannel);
}

bool FilterGraph::canConnect (uint32 sourceFilterUID, int sourceFilterChannel,
                              uint32 destFilterUID, int destFilterChannel) const noexcept
{
    return graph.canConnect (sourceFilterUID, sourceFilterChannel,
                             destFilterUID, destFilterChannel);
}

bool FilterGraph::addConnection (uint32 sourceFilterUID, int sourceFilterChannel,
                                 uint32 destFilterUID, int destFilterChannel)
{
    const bool result = graph.addConnection (sourceFilterUID, sourceFilterChannel,
                                             destFilterUID, destFilterChannel);

    if (result)
        changed();

    return result;
}

void FilterGraph::removeConnection (const int index)
{
    graph.removeConnection (index);
    changed();
}

void FilterGraph::removeConnection (uint32 sourceFilterUID, int sourceFilterChannel,
                                    uint32 destFilterUID, int destFilterChannel)
{
    if (graph.removeConnection (sourceFilterUID, sourceFilterChannel,
                                destFilterUID, destFilterChannel))
        changed();
}

void FilterGraph::clear()
{
    PluginWindow::closeAllCurrentlyOpenWindows();

    graph.clear();
    changed();
}

//==============================================================================
const String FilterGraph::getDocumentTitle()
{
    if (! getFile().exists())
        return "Unnamed";

    return getFile().getFileNameWithoutExtension();
}

const String FilterGraph::loadDocument (const File& file)
{
    XmlDocument doc (file);
    ScopedPointer<XmlElement> xml (doc.getDocumentElement());

    if (xml == nullptr || ! xml->hasTagName ("FILTERGRAPH"))
        return "Not a valid filter graph file";

    restoreFromXml (*xml);
    return String::empty;
}

const String FilterGraph::saveDocument (const File& file)
{
    ScopedPointer<XmlElement> xml (createXml());

    if (! xml->writeToFile (file, String::empty))
        return "Couldn't write to the file";

    return String::empty;
}

const File FilterGraph::getLastDocumentOpened()
{
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (appProperties->getUserSettings()
                                        ->getValue ("recentFilterGraphFiles"));

    return recentFiles.getFile (0);
}

void FilterGraph::setLastDocumentOpened (const File& file)
{
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (appProperties->getUserSettings()
                                        ->getValue ("recentFilterGraphFiles"));

    recentFiles.addFile (file);

    appProperties->getUserSettings()
        ->setValue ("recentFilterGraphFiles", recentFiles.toString());
}

//==============================================================================
static XmlElement* createNodeXml (AudioProcessorGraph::Node* const node) noexcept
{
    AudioPluginInstance* plugin = dynamic_cast <AudioPluginInstance*> (node->getProcessor());

    if (plugin == nullptr)
    {
        jassertfalse
        return nullptr;
    }

    XmlElement* e = new XmlElement ("FILTER");
    e->setAttribute ("uid", (int) node->nodeId);
    e->setAttribute ("x", node->properties ["x"].toString());
    e->setAttribute ("y", node->properties ["y"].toString());
    e->setAttribute ("uiLastX", node->properties ["uiLastX"].toString());
    e->setAttribute ("uiLastY", node->properties ["uiLastY"].toString());

    PluginDescription pd;
    plugin->fillInPluginDescription (pd);

    e->addChildElement (pd.createXml());

    XmlElement* state = new XmlElement ("STATE");

    MemoryBlock m;
    node->getProcessor()->getStateInformation (m);
    state->addTextElement (m.toBase64Encoding());
    e->addChildElement (state);

    return e;
}

void FilterGraph::createNodeFromXml (const XmlElement& xml)
{
    PluginDescription pd;

    forEachXmlChildElement (xml, e)
    {
        if (pd.loadFromXml (*e))
            break;
    }

    String errorMessage;

    AudioPluginInstance* instance
        = AudioPluginFormatManager::getInstance()->createPluginInstance (pd, errorMessage);

    if (instance == nullptr)
    {
        // xxx handle ins + outs
    }

    if (instance == nullptr)
        return;

    AudioProcessorGraph::Node::Ptr node (graph.addNode (instance, xml.getIntAttribute ("uid")));

    const XmlElement* const state = xml.getChildByName ("STATE");

    if (state != nullptr)
    {
        MemoryBlock m;
        m.fromBase64Encoding (state->getAllSubText());

        node->getProcessor()->setStateInformation (m.getData(), (int) m.getSize());
    }

    node->properties.set ("x", xml.getDoubleAttribute ("x"));
    node->properties.set ("y", xml.getDoubleAttribute ("y"));
    node->properties.set ("uiLastX", xml.getIntAttribute ("uiLastX"));
    node->properties.set ("uiLastY", xml.getIntAttribute ("uiLastY"));
}

XmlElement* FilterGraph::createXml() const
{
    XmlElement* xml = new XmlElement ("FILTERGRAPH");

    int i;
    for (i = 0; i < graph.getNumNodes(); ++i)
    {
        xml->addChildElement (createNodeXml (graph.getNode (i)));
    }

    for (i = 0; i < graph.getNumConnections(); ++i)
    {
        const AudioProcessorGraph::Connection* const fc = graph.getConnection(i);

        XmlElement* e = new XmlElement ("CONNECTION");

        e->setAttribute ("srcFilter", (int) fc->sourceNodeId);
        e->setAttribute ("srcChannel", fc->sourceChannelIndex);
        e->setAttribute ("dstFilter", (int) fc->destNodeId);
        e->setAttribute ("dstChannel", fc->destChannelIndex);

        xml->addChildElement (e);
    }

    return xml;
}

void FilterGraph::restoreFromXml (const XmlElement& xml)
{
    clear();

    forEachXmlChildElementWithTagName (xml, e, "FILTER")
    {
        createNodeFromXml (*e);
        changed();
    }

    forEachXmlChildElementWithTagName (xml, e, "CONNECTION")
    {
        addConnection ((uint32) e->getIntAttribute ("srcFilter"),
                       e->getIntAttribute ("srcChannel"),
                       (uint32) e->getIntAttribute ("dstFilter"),
                       e->getIntAttribute ("dstChannel"));
    }

    graph.removeIllegalConnections();
}
