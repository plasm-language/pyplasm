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

#ifndef __JUCER_TABBEDCOMPONENTHANDLER_JUCEHEADER__
#define __JUCER_TABBEDCOMPONENTHANDLER_JUCEHEADER__


//==============================================================================
/**
*/
class TabbedComponentHandler  : public ComponentTypeHandler
{
public:
    //==============================================================================
    TabbedComponentHandler()
        : ComponentTypeHandler ("Tabbed Component", "TabbedComponent", typeid (TabbedComponent), 200, 150)
    {}

    //==============================================================================
    Component* createNewComponent (JucerDocument*)
    {
        TabbedComponent* const t = new TabbedComponent (TabbedButtonBar::TabsAtTop);
        t->setName ("new tabbed component");

        for (int i = 3; --i >= 0;)
            addNewTab (t);

        return t;
    }

    XmlElement* createXmlFor (Component* comp, const ComponentLayout* layout)
    {
        TabbedComponent* const t = dynamic_cast <TabbedComponent*> (comp);
        XmlElement* const e = ComponentTypeHandler::createXmlFor (comp, layout);

        if (t->getOrientation() == TabbedButtonBar::TabsAtTop)
            e->setAttribute ("orientation", "top");
        else if (t->getOrientation() == TabbedButtonBar::TabsAtBottom)
            e->setAttribute ("orientation", "bottom");
        else if (t->getOrientation() == TabbedButtonBar::TabsAtLeft)
            e->setAttribute ("orientation", "left");
        else if (t->getOrientation() == TabbedButtonBar::TabsAtRight)
            e->setAttribute ("orientation", "right");

        e->setAttribute ("tabBarDepth", t->getTabBarDepth());
        e->setAttribute ("initialTab", t->getCurrentTabIndex());

        for (int i = 0; i < t->getNumTabs(); ++i)
            e->addChildElement (getTabState (t, i));

        return e;
    }

    bool restoreFromXml (const XmlElement& xml, Component* comp, const ComponentLayout* layout)
    {
        if (! ComponentTypeHandler::restoreFromXml (xml, comp, layout))
            return false;

        TabbedComponent* const t = dynamic_cast <TabbedComponent*> (comp);

        if (xml.getStringAttribute ("orientation") == "top")
            t->setOrientation (TabbedButtonBar::TabsAtTop);
        else if (xml.getStringAttribute ("orientation") == "bottom")
            t->setOrientation (TabbedButtonBar::TabsAtBottom);
        else if (xml.getStringAttribute ("orientation") == "left")
            t->setOrientation (TabbedButtonBar::TabsAtLeft);
        else if (xml.getStringAttribute ("orientation") == "right")
            t->setOrientation (TabbedButtonBar::TabsAtRight);

        TabbedComponent defaultTabComp (TabbedButtonBar::TabsAtTop);

        t->setTabBarDepth (xml.getIntAttribute ("tabBarDepth", defaultTabComp.getTabBarDepth()));

        t->clearTabs();

        forEachXmlChildElement (xml, e)
        {
            addNewTab (t);
            restoreTabState (t, t->getNumTabs() - 1, *e);
        }

        t->setCurrentTabIndex (xml.getIntAttribute ("initialTab", 0));

        return true;
    }

    void getEditableProperties (Component* component, JucerDocument& document, Array <PropertyComponent*>& properties)
    {
        ComponentTypeHandler::getEditableProperties (component, document, properties);

        TabbedComponent* const t = dynamic_cast <TabbedComponent*> (component);

        properties.add (new TabOrientationProperty (t, document));
        properties.add (new TabDepthProperty (t, document));

        if (t->getNumTabs() > 0)
            properties.add (new TabInitialTabProperty (t, document));

        properties.add (new TabAddTabProperty (t, document));

        if (t->getNumTabs() > 0)
            properties.add (new TabRemoveTabProperty (t, document));
    }

    void addPropertiesToPropertyPanel (Component* comp,
                                       JucerDocument& document,
                                       PropertyPanel& panel)
    {
        ComponentTypeHandler::addPropertiesToPropertyPanel (comp, document, panel);

        TabbedComponent* const t = dynamic_cast <TabbedComponent*> (comp);

        for (int i = 0; i < t->getNumTabs(); ++i)
        {
            Array <PropertyComponent*> properties;

            properties.add (new TabNameProperty (t, document, i));
            properties.add (new TabColourProperty (t, document, i));

            properties.add (new TabContentTypeProperty (t, document, i));

            if (isTabUsingJucerComp (t, i))
                properties.add (new TabJucerFileProperty (t, document, i));
            else
                properties.add (new TabContentClassProperty (t, document, i));

            properties.add (new TabContentConstructorParamsProperty (t, document, i));

            properties.add (new TabMoveProperty (t, document, i, t->getNumTabs()));

            panel.addSection ("Tab " + String (i), properties);
        }
    }

    const String getCreationParameters (Component* comp)
    {
        TabbedComponent* const t = dynamic_cast <TabbedComponent*> (comp);

        switch (t->getOrientation())
        {
        case TabbedButtonBar::TabsAtTop:
            return "TabbedButtonBar::TabsAtTop";
        case TabbedButtonBar::TabsAtBottom:
            return "TabbedButtonBar::TabsAtBottom";
        case TabbedButtonBar::TabsAtLeft:
            return "TabbedButtonBar::TabsAtLeft";
        case TabbedButtonBar::TabsAtRight:
            return "TabbedButtonBar::TabsAtRight";
        default:
            jassertfalse
            break;
        }

        return String::empty;
    }

    void fillInCreationCode (GeneratedCode& code, Component* component, const String& memberVariableName)
    {
        TabbedComponent* const t = dynamic_cast <TabbedComponent*> (component);

        ComponentTypeHandler::fillInCreationCode (code, component, memberVariableName);

        code.constructorCode
            << memberVariableName << "->setTabBarDepth (" << t->getTabBarDepth() << ");\n";

        for (int i = 0; i < t->getNumTabs(); ++i)
        {
            String contentClassName;

            if (isTabUsingJucerComp (t, i))
            {
                ScopedPointer<JucerDocument> doc
                    (ObjectTypes::loadDocumentFromFile (code.document->getFile()
                                                            .getSiblingFile (getTabJucerFile (t, i)), false));

                if (doc != 0)
                {
                    code.includeFilesCPP.add (getTabJucerFile (t, i).replace (".cpp", ".h"));

                    contentClassName = doc->getClassName();
                }
            }
            else
            {
                contentClassName = getTabClassName (t, i);
            }

            code.constructorCode
                << memberVariableName
                << "->addTab ("
                << quotedString (t->getTabNames() [i])
                << ", "
                << colourToCode (t->getTabBackgroundColour (i));

            if (contentClassName.isNotEmpty())
            {
                code.constructorCode << ", new " << contentClassName;

                if (getTabConstructorParams (t, i).trim().isNotEmpty())
                    code.constructorCode << " ";

                code.constructorCode << "(" << getTabConstructorParams (t, i).trim() << "), true);\n";
            }
            else
            {
                code.constructorCode << ", 0, false);\n";
            }
        }

        code.constructorCode
            << memberVariableName << "->setCurrentTabIndex (" << t->getCurrentTabIndex() << ");\n";

        code.constructorCode << "\n";
    }

    //==============================================================================
    static void addNewTab (TabbedComponent* tc, const int insertIndex = -1)
    {
        tc->addTab ("Tab " + String (tc->getNumTabs()), Colours::lightgrey,
                    new TabDemoContentComp(), true, insertIndex);
    }

    //==============================================================================
    static XmlElement* getTabState (TabbedComponent* tc, int tabIndex)
    {
        XmlElement* xml = new XmlElement ("TAB");
        xml->setAttribute ("name", tc->getTabNames() [tabIndex]);
        setColourXml (*xml, "colour", tc->getTabBackgroundColour (tabIndex));

        TabDemoContentComp* const tdc = dynamic_cast <TabDemoContentComp*> (tc->getTabContentComponent (tabIndex));
        jassert (tdc != 0);

        if (tdc != 0)
        {
            xml->setAttribute ("useJucerComp", tdc->isUsingJucerComp);
            xml->setAttribute ("contentClassName", tdc->contentClassName);
            xml->setAttribute ("constructorParams", tdc->constructorParams);
            xml->setAttribute ("jucerComponentFile", tdc->jucerComponentFile);
        }

        return xml;
    }

    static void restoreTabState (TabbedComponent* tc, int tabIndex, const XmlElement& xml)
    {
        tc->setTabName (tabIndex, xml.getStringAttribute ("name", "Tab"));
        tc->setTabBackgroundColour (tabIndex, getColourXml (xml, "colour", Colours::lightgrey));

        TabDemoContentComp* const tdc = dynamic_cast <TabDemoContentComp*> (tc->getTabContentComponent (tabIndex));
        jassert (tdc != 0);

        if (tdc != 0)
        {
            tdc->isUsingJucerComp = xml.getBoolAttribute ("useJucerComp", false);
            tdc->contentClassName = xml.getStringAttribute ("contentClassName");
            tdc->constructorParams = xml.getStringAttribute ("constructorParams");
            tdc->jucerComponentFile = xml.getStringAttribute ("jucerComponentFile");

            tdc->updateContent();
        }
    }

    //==============================================================================
    static bool isTabUsingJucerComp (TabbedComponent* tc, int tabIndex)
    {
        TabDemoContentComp* const tdc = dynamic_cast <TabDemoContentComp*> (tc->getTabContentComponent (tabIndex));
        jassert (tdc != 0);

        return tdc != 0 && tdc->isUsingJucerComp;
    }

    static void setTabUsingJucerComp (TabbedComponent* tc, int tabIndex, const bool b)
    {
        TabDemoContentComp* const tdc = dynamic_cast <TabDemoContentComp*> (tc->getTabContentComponent (tabIndex));
        jassert (tdc != 0);

        if (tdc != 0)
        {
            tdc->isUsingJucerComp = b;
            tdc->updateContent();
        }
    }

    static const String getTabClassName (TabbedComponent* tc, int tabIndex)
    {
        TabDemoContentComp* const tdc = dynamic_cast <TabDemoContentComp*> (tc->getTabContentComponent (tabIndex));
        jassert (tdc != 0);

        return tdc != 0 ? tdc->contentClassName : String::empty;
    }

    static void setTabClassName (TabbedComponent* tc, int tabIndex, const String& newName)
    {
        TabDemoContentComp* const tdc = dynamic_cast <TabDemoContentComp*> (tc->getTabContentComponent (tabIndex));
        jassert (tdc != 0);

        if (tdc != 0)
        {
            tdc->contentClassName = newName;
            tdc->updateContent();
        }
    }

    static const String getTabConstructorParams (TabbedComponent* tc, int tabIndex)
    {
        TabDemoContentComp* const tdc = dynamic_cast <TabDemoContentComp*> (tc->getTabContentComponent (tabIndex));
        jassert (tdc != 0);

        return tdc != 0 ? tdc->constructorParams : String::empty;
    }

    static void setTabConstructorParams (TabbedComponent* tc, int tabIndex, const String& newParams)
    {
        TabDemoContentComp* const tdc = dynamic_cast <TabDemoContentComp*> (tc->getTabContentComponent (tabIndex));
        jassert (tdc != 0);

        if (tdc != 0)
        {
            tdc->constructorParams = newParams;
            tdc->updateContent();
        }
    }

    static const String getTabJucerFile (TabbedComponent* tc, int tabIndex)
    {
        TabDemoContentComp* const tdc = dynamic_cast <TabDemoContentComp*> (tc->getTabContentComponent (tabIndex));
        jassert (tdc != 0);

        return tdc != 0 ? tdc->jucerComponentFile : String::empty;
    }

    static void setTabJucerFile (TabbedComponent* tc, int tabIndex, const String& newFile)
    {
        TabDemoContentComp* const tdc = dynamic_cast <TabDemoContentComp*> (tc->getTabContentComponent (tabIndex));
        jassert (tdc != 0);

        if (tdc != 0)
        {
            tdc->jucerComponentFile = newFile;
            tdc->updateContent();
        }
    }

private:
    //==============================================================================
    class TabDemoContentComp : public Component
    {
    public:
        TabDemoContentComp()
            : isUsingJucerComp (false)
        {
            setSize (2048, 2048);
        }

        ~TabDemoContentComp()
        {
            deleteAllChildren();
        }

        void paint (Graphics& g)
        {
            if (getNumChildComponents() == 0)
            {
                g.fillCheckerBoard (getLocalBounds(), 50, 50,
                                    Colour::greyLevel (0.9f).withAlpha (0.4f),
                                    Colour::greyLevel (0.8f).withAlpha (0.4f));
            }
        }

        void resized()
        {
            Component* const c = getChildComponent (0);

            if (c != 0)
            {
                c->setBounds (0, 0, getWidth(), getHeight());
                setOpaque (c->isOpaque());
            }
        }

        void updateContent()
        {
            if (isUsingJucerComp)
            {
                TestComponent* jucerComp = dynamic_cast <TestComponent*> (getChildComponent (0));

                if (jucerComp == 0
                    || jucerComp->getOwnerDocument() == 0
                    || jucerComp->getFilename() != jucerComponentFile)
                {
                    deleteAllChildren();

                    TestComponent* tc = new TestComponent (ComponentTypeHandler::findParentDocument (this), 0, false);
                    tc->setFilename (jucerComponentFile);
                    tc->setToInitialSize();

                    addAndMakeVisible (tc);
                }
            }
            else
            {
                deleteAllChildren();
            }

            resized();
        }

        void parentHierarchyChanged()
        {
            updateContent();
        }

        bool isUsingJucerComp;
        String contentClassName, constructorParams;
        String jucerComponentFile;
    };

    //==============================================================================
    class TabOrientationProperty  : public ComponentChoiceProperty <TabbedComponent>
    {
    public:
        TabOrientationProperty (TabbedComponent* comp, JucerDocument& document)
            : ComponentChoiceProperty <TabbedComponent> ("tab position", comp, document)
        {
            choices.add ("Tabs at top");
            choices.add ("Tabs at bottom");
            choices.add ("Tabs at left");
            choices.add ("Tabs at right");
        }

        void setIndex (int newIndex)
        {
            const TabbedButtonBar::Orientation orientations[] = { TabbedButtonBar::TabsAtTop,
                                                                  TabbedButtonBar::TabsAtBottom,
                                                                  TabbedButtonBar::TabsAtLeft,
                                                                  TabbedButtonBar::TabsAtRight };

            document.perform (new TabOrienationChangeAction (component, *document.getComponentLayout(), orientations [newIndex]),
                              "Change TabComponent orientation");
        }

        int getIndex() const
        {
            switch (component->getOrientation())
            {
            case TabbedButtonBar::TabsAtTop:
                return 0;
            case TabbedButtonBar::TabsAtBottom:
                return 1;
            case TabbedButtonBar::TabsAtLeft:
                return 2;
            case TabbedButtonBar::TabsAtRight:
                return 3;
            default:
                jassertfalse
                break;
            }

            return 0;
        }

    private:
        class TabOrienationChangeAction  : public ComponentUndoableAction <TabbedComponent>
        {
        public:
            TabOrienationChangeAction (TabbedComponent* const comp, ComponentLayout& layout, const TabbedButtonBar::Orientation newState_)
                : ComponentUndoableAction <TabbedComponent> (comp, layout),
                  newState (newState_)
            {
                oldState = comp->getOrientation();
            }

            bool perform()
            {
                showCorrectTab();
                getComponent()->setOrientation (newState);
                changed();
                return true;
            }

            bool undo()
            {
                showCorrectTab();
                getComponent()->setOrientation (oldState);
                changed();
                return true;
            }

            TabbedButtonBar::Orientation newState, oldState;
        };
    };

    //==============================================================================
    class TabInitialTabProperty  : public ComponentChoiceProperty <TabbedComponent>
    {
    public:
        TabInitialTabProperty (TabbedComponent* comp, JucerDocument& document)
            : ComponentChoiceProperty <TabbedComponent> ("initial tab", comp, document)
        {
            for (int i = 0; i < comp->getNumTabs(); ++i)
                choices.add ("Tab " + String (i) + ": \"" + comp->getTabNames() [i] + "\"");
        }

        void setIndex (int newIndex)
        {
            document.perform (new InitialTabChangeAction (component, *document.getComponentLayout(), newIndex),
                              "Change initial tab");
        }

        int getIndex() const
        {
            return component->getCurrentTabIndex();
        }

    private:
        class InitialTabChangeAction  : public ComponentUndoableAction <TabbedComponent>
        {
        public:
            InitialTabChangeAction (TabbedComponent* const comp, ComponentLayout& layout, const int newValue_)
                : ComponentUndoableAction <TabbedComponent> (comp, layout),
                  newValue (newValue_)
            {
                oldValue = comp->getCurrentTabIndex();
            }

            bool perform()
            {
                showCorrectTab();
                getComponent()->setCurrentTabIndex (newValue);
                changed();
                return true;
            }

            bool undo()
            {
                showCorrectTab();
                getComponent()->setCurrentTabIndex (oldValue);
                changed();
                return true;
            }

        private:
            int newValue, oldValue;
        };
    };

    //==============================================================================
    class TabDepthProperty  : public SliderPropertyComponent,
                              public ChangeListener
    {
    public:
        TabDepthProperty (TabbedComponent* comp, JucerDocument& document_)
            : SliderPropertyComponent ("tab depth", 10.0, 80.0, 1.0, 1.0),
              component (comp),
              document (document_)
        {
            document.addChangeListener (this);
        }

        ~TabDepthProperty()
        {
            document.removeChangeListener (this);
        }

        void setValue (double newValue)
        {
            document.getUndoManager().undoCurrentTransactionOnly();

            document.perform (new TabDepthChangeAction (component, *document.getComponentLayout(), roundToInt (newValue)),
                              "Change TabComponent tab depth");
        }

        double getValue() const
        {
            return component->getTabBarDepth();
        }

        void changeListenerCallback (ChangeBroadcaster*)
        {
            refresh();
        }

        TabbedComponent* const component;
        JucerDocument& document;

    private:
        class TabDepthChangeAction  : public ComponentUndoableAction <TabbedComponent>
        {
        public:
            TabDepthChangeAction (TabbedComponent* const comp, ComponentLayout& layout, const int newState_)
                : ComponentUndoableAction <TabbedComponent> (comp, layout),
                  newState (newState_)
            {
                oldState = comp->getTabBarDepth();
            }

            bool perform()
            {
                showCorrectTab();
                getComponent()->setTabBarDepth (newState);
                changed();
                return true;
            }

            bool undo()
            {
                showCorrectTab();
                getComponent()->setTabBarDepth (oldState);
                changed();
                return true;
            }

            int newState, oldState;
        };
    };

    //==============================================================================
    class TabAddTabProperty  : public ButtonPropertyComponent
    {
    public:
        TabAddTabProperty (TabbedComponent* comp, JucerDocument& document_)
            : ButtonPropertyComponent ("add tab", false),
              component (comp),
              document (document_)
        {
        }

        void buttonClicked()
        {
            document.perform (new AddTabAction (component, *document.getComponentLayout()),
                              "Add a new tab");
        }

        String getButtonText() const
        {
            return "Create a new tab";
        }

        TabbedComponent* const component;
        JucerDocument& document;

    private:
        class AddTabAction  : public ComponentUndoableAction <TabbedComponent>
        {
        public:
            AddTabAction (TabbedComponent* const comp, ComponentLayout& layout)
                : ComponentUndoableAction <TabbedComponent> (comp, layout)
            {
            }

            bool perform()
            {
                showCorrectTab();
                addNewTab (getComponent());
                layout.getDocument()->refreshAllPropertyComps();
                changed();
                return true;
            }

            bool undo()
            {
                showCorrectTab();
                getComponent()->removeTab (getComponent()->getNumTabs() - 1);
                layout.getDocument()->refreshAllPropertyComps();
                changed();
                return true;
            }
        };
    };

    //==============================================================================
    class TabRemoveTabProperty  : public ButtonPropertyComponent
    {
    public:
        TabRemoveTabProperty (TabbedComponent* comp, JucerDocument& document_)
            : ButtonPropertyComponent ("remove tab", true),
              component (comp),
              document (document_)
        {
        }

        void buttonClicked()
        {
            const StringArray names (component->getTabNames());

            PopupMenu m;
            for (int i = 0; i < component->getNumTabs(); ++i)
                m.addItem (i + 1, "Delete tab " + String (i)
                                    + ": \"" + names[i] + "\"");

            const int r = m.showAt (this);

            if (r > 0)
            {
                document.perform (new RemoveTabAction (component, *document.getComponentLayout(), r - 1),
                                  "Remove a tab");
            }
        }

        String getButtonText() const
        {
            return "Delete a tab...";
        }

        TabbedComponent* const component;
        JucerDocument& document;

    private:
        class RemoveTabAction  : public ComponentUndoableAction <TabbedComponent>
        {
        public:
            RemoveTabAction (TabbedComponent* const comp, ComponentLayout& layout, int indexToRemove_)
                : ComponentUndoableAction <TabbedComponent> (comp, layout),
                  indexToRemove (indexToRemove_)
            {
                previousState = getTabState (comp, indexToRemove);
            }

            bool perform()
            {
                showCorrectTab();

                getComponent()->removeTab (indexToRemove);
                layout.getDocument()->refreshAllPropertyComps();
                changed();
                return true;
            }

            bool undo()
            {
                showCorrectTab();
                addNewTab (getComponent(), indexToRemove);
                restoreTabState (getComponent(), indexToRemove, *previousState);
                layout.getDocument()->refreshAllPropertyComps();
                changed();
                return true;
            }

        private:
            int indexToRemove;
            ScopedPointer<XmlElement> previousState;
        };
    };

    //==============================================================================
    class TabNameProperty  : public ComponentTextProperty <TabbedComponent>
    {
    public:
        TabNameProperty (TabbedComponent* comp, JucerDocument& document, const int tabIndex_)
            : ComponentTextProperty <TabbedComponent> ("name", 200, false, comp, document),
              tabIndex (tabIndex_)
        {
        }

        void setText (const String& newText)
        {
            document.perform (new TabNameChangeAction (component, *document.getComponentLayout(), tabIndex, newText),
                              "Change tab name");
        }

        String getText() const
        {
            return component->getTabNames() [tabIndex];
        }

    private:
        int tabIndex;

        class TabNameChangeAction  : public ComponentUndoableAction <TabbedComponent>
        {
        public:
            TabNameChangeAction (TabbedComponent* const comp, ComponentLayout& layout, const int tabIndex_, const String& newValue_)
                : ComponentUndoableAction <TabbedComponent> (comp, layout),
                  tabIndex (tabIndex_),
                  newValue (newValue_)
            {
                oldValue = comp->getTabNames() [tabIndex];
            }

            bool perform()
            {
                showCorrectTab();
                getComponent()->setTabName (tabIndex, newValue);
                changed();
                return true;
            }

            bool undo()
            {
                showCorrectTab();
                getComponent()->setTabName (tabIndex, oldValue);
                changed();
                return true;
            }

        private:
            const int tabIndex;
            String newValue, oldValue;
        };
    };

    //==============================================================================
    class TabColourProperty  : public ColourPropertyComponent,
                               private ChangeListener
    {
    public:
        TabColourProperty (TabbedComponent* comp, JucerDocument& document_, const int tabIndex_)
            : ColourPropertyComponent ("colour", false),
              component (comp),
              document (document_),
              tabIndex (tabIndex_)
        {
            document.addChangeListener (this);
        }

        ~TabColourProperty()
        {
            document.removeChangeListener (this);
        }

        void setColour (const Colour& newColour)
        {
            document.getUndoManager().undoCurrentTransactionOnly();

            document.perform (new TabColourChangeAction (component, *document.getComponentLayout(), tabIndex, newColour),
                              "Change tab colour");
        }

        Colour getColour() const
        {
            return component->getTabBackgroundColour (tabIndex);
        }

        void resetToDefault()
        {
            jassertfalse // shouldn't get called
        }

        void changeListenerCallback (ChangeBroadcaster*)     { refresh(); }

    private:
        TabbedComponent* component;
        JucerDocument& document;
        int tabIndex;

        class TabColourChangeAction  : public ComponentUndoableAction <TabbedComponent>
        {
        public:
            TabColourChangeAction (TabbedComponent* const comp, ComponentLayout& layout, const int tabIndex_, const Colour& newValue_)
                : ComponentUndoableAction <TabbedComponent> (comp, layout),
                  tabIndex (tabIndex_),
                  newValue (newValue_)
            {
                oldValue = comp->getTabBackgroundColour (tabIndex);
            }

            bool perform()
            {
                showCorrectTab();
                getComponent()->setTabBackgroundColour (tabIndex, newValue);
                changed();
                return true;
            }

            bool undo()
            {
                showCorrectTab();
                getComponent()->setTabBackgroundColour (tabIndex, oldValue);
                changed();
                return true;
            }

        private:
            const int tabIndex;
            Colour newValue, oldValue;
        };
    };

    //==============================================================================
    class TabContentTypeProperty  : public ComponentChoiceProperty <TabbedComponent>
    {
    public:
        TabContentTypeProperty (TabbedComponent* comp, JucerDocument& document, const int tabIndex_)
            : ComponentChoiceProperty <TabbedComponent> ("content type", comp, document),
              tabIndex (tabIndex_)
        {
            choices.add ("Jucer content component");
            choices.add ("Named content component");
        }

        void setIndex (int newIndex)
        {
            document.perform (new TabContentTypeChangeAction (component, *document.getComponentLayout(), tabIndex, newIndex == 0),
                              "Change tab content type");
        }

        int getIndex() const
        {
            return isTabUsingJucerComp (component, tabIndex) ? 0 : 1;
        }

    private:
        int tabIndex;

        class TabContentTypeChangeAction  : public ComponentUndoableAction <TabbedComponent>
        {
        public:
            TabContentTypeChangeAction (TabbedComponent* const comp, ComponentLayout& layout, const int tabIndex_, const bool newValue_)
                : ComponentUndoableAction <TabbedComponent> (comp, layout),
                  tabIndex (tabIndex_),
                  newValue (newValue_)
            {
                oldValue = isTabUsingJucerComp (comp, tabIndex);
            }

            bool perform()
            {
                showCorrectTab();
                setTabUsingJucerComp (getComponent(), tabIndex, newValue);
                layout.getDocument()->refreshAllPropertyComps();
                changed();
                return true;
            }

            bool undo()
            {
                showCorrectTab();
                setTabUsingJucerComp (getComponent(), tabIndex, oldValue);
                layout.getDocument()->refreshAllPropertyComps();
                changed();
                return true;
            }

        private:
            int tabIndex;
            bool newValue, oldValue;
        };
    };

    //==============================================================================
    class TabJucerFileProperty   : public FilePropertyComponent,
                                   public ChangeListener
    {
    public:
        TabJucerFileProperty (TabbedComponent* const component_, JucerDocument& document_, const int tabIndex_)
            : FilePropertyComponent ("jucer file", false, true),
              component (component_),
              document (document_),
              tabIndex (tabIndex_)
        {
            document.addChangeListener (this);
        }

        ~TabJucerFileProperty()
        {
            document.removeChangeListener (this);
        }

        //==============================================================================
        void setFile (const File& newFile)
        {
            document.perform (new JucerCompFileChangeAction (component, *document.getComponentLayout(), tabIndex,
                                                             newFile.getRelativePathFrom (document.getFile().getParentDirectory())
                                                                    .replaceCharacter ('\\', '/')),
                              "Change tab component file");
        }

        const File getFile() const
        {
            return document.getFile().getSiblingFile (getTabJucerFile (component, tabIndex));
        }

        void changeListenerCallback (ChangeBroadcaster*)     { refresh(); }

    private:
        TabbedComponent* const component;
        JucerDocument& document;
        int tabIndex;

        class JucerCompFileChangeAction  : public ComponentUndoableAction <TabbedComponent>
        {
        public:
            JucerCompFileChangeAction (TabbedComponent* const comp, ComponentLayout& layout, const int tabIndex_, const String& newState_)
                : ComponentUndoableAction <TabbedComponent> (comp, layout),
                  tabIndex (tabIndex_),
                  newState (newState_)
            {
                oldState = getTabJucerFile (comp, tabIndex);
            }

            bool perform()
            {
                showCorrectTab();
                setTabJucerFile (getComponent(), tabIndex, newState);
                changed();
                return true;
            }

            bool undo()
            {
                showCorrectTab();
                setTabJucerFile (getComponent(), tabIndex, oldState);
                changed();
                return true;
            }

            int tabIndex;
            String newState, oldState;
        };
    };

    //==============================================================================
    class TabContentClassProperty   : public ComponentTextProperty <TabbedComponent>
    {
    public:
        TabContentClassProperty (TabbedComponent* comp, JucerDocument& document, const int tabIndex_)
            : ComponentTextProperty <TabbedComponent> ("content class", 256, false, comp, document),
              tabIndex (tabIndex_)
        {
        }

        void setText (const String& newText)
        {
            document.perform (new TabClassNameChangeAction (component, *document.getComponentLayout(), tabIndex, newText),
                              "Change TabbedComponent content class");
        }

        String getText() const
        {
            return getTabClassName (component, tabIndex);
        }

    private:
        int tabIndex;

        class TabClassNameChangeAction  : public ComponentUndoableAction <TabbedComponent>
        {
        public:
            TabClassNameChangeAction (TabbedComponent* const comp, ComponentLayout& layout, const int tabIndex_, const String& newValue_)
                : ComponentUndoableAction <TabbedComponent> (comp, layout),
                  tabIndex (tabIndex_),
                  newValue (newValue_)
            {
                oldValue = getTabClassName (comp, tabIndex);
            }

            bool perform()
            {
                showCorrectTab();
                setTabClassName (getComponent(), tabIndex, newValue);
                changed();
                layout.getDocument()->refreshAllPropertyComps();
                return true;
            }

            bool undo()
            {
                showCorrectTab();
                setTabClassName (getComponent(), tabIndex, oldValue);
                changed();
                layout.getDocument()->refreshAllPropertyComps();
                return true;
            }

            int tabIndex;
            String newValue, oldValue;
        };
    };

    //==============================================================================
    class TabContentConstructorParamsProperty   : public ComponentTextProperty <TabbedComponent>
    {
    public:
        TabContentConstructorParamsProperty (TabbedComponent* comp, JucerDocument& document, const int tabIndex_)
            : ComponentTextProperty <TabbedComponent> ("constructor params", 512, false, comp, document),
              tabIndex (tabIndex_)
        {
        }

        void setText (const String& newText)
        {
            document.perform (new TabConstructorParamChangeAction (component, *document.getComponentLayout(), tabIndex, newText),
                              "Change TabbedComponent content constructor param");
        }

        String getText() const
        {
            return getTabConstructorParams (component, tabIndex);
        }

    private:
        int tabIndex;

        class TabConstructorParamChangeAction  : public ComponentUndoableAction <TabbedComponent>
        {
        public:
            TabConstructorParamChangeAction (TabbedComponent* const comp, ComponentLayout& layout, const int tabIndex_, const String& newValue_)
                : ComponentUndoableAction <TabbedComponent> (comp, layout),
                  tabIndex (tabIndex_),
                  newValue (newValue_)
            {
                oldValue = getTabConstructorParams (comp, tabIndex);
            }

            bool perform()
            {
                showCorrectTab();
                setTabConstructorParams (getComponent(), tabIndex, newValue);
                changed();
                layout.getDocument()->refreshAllPropertyComps();
                return true;
            }

            bool undo()
            {
                showCorrectTab();
                setTabConstructorParams (getComponent(), tabIndex, oldValue);
                changed();
                layout.getDocument()->refreshAllPropertyComps();
                return true;
            }

            int tabIndex;
            String newValue, oldValue;
        };
    };

    //==============================================================================
    class TabMoveProperty   : public ButtonPropertyComponent
    {
    public:
        TabMoveProperty (TabbedComponent* comp, JucerDocument& document_,
                         const int tabIndex_, const int totalNumTabs_)
            : ButtonPropertyComponent ("move tab", false),
              component (comp),
              document (document_),
              tabIndex (tabIndex_),
              totalNumTabs (totalNumTabs_)
        {

        }

        void buttonClicked()
        {
            PopupMenu m;
            m.addItem (1, "Move this tab up", tabIndex > 0);
            m.addItem (2, "Move this tab down", tabIndex < totalNumTabs - 1);

            const int r = m.showAt (this);

            if (r != 0)
            {
                document.perform (new MoveTabAction (component, *document.getComponentLayout(), tabIndex, tabIndex + (r == 2 ? 1 : -1)),
                                  "Move a tab");
            }
        }

        String getButtonText() const
        {
            return "Move this tab...";
        }

        TabbedComponent* const component;
        JucerDocument& document;
        const int tabIndex, totalNumTabs;

    private:
        class MoveTabAction  : public ComponentUndoableAction <TabbedComponent>
        {
        public:
            MoveTabAction (TabbedComponent* const comp, ComponentLayout& layout,
                           const int oldIndex_, const int newIndex_)
                : ComponentUndoableAction <TabbedComponent> (comp, layout),
                  oldIndex (oldIndex_),
                  newIndex (newIndex_)
            {
            }

            void move (int from, int to)
            {
                showCorrectTab();

                ScopedPointer<XmlElement> state (getTabState (getComponent(), from));

                getComponent()->removeTab (from);
                addNewTab (getComponent(), to);

                restoreTabState (getComponent(), to, *state);

                layout.getDocument()->refreshAllPropertyComps();
                changed();
            }

            bool perform()
            {
                move (oldIndex, newIndex);
                return true;
            }

            bool undo()
            {
                move (newIndex, oldIndex);
                return true;
            }

        private:
            const int oldIndex, newIndex;
        };
    };
};


#endif   // __JUCER_TABBEDCOMPONENTHANDLER_JUCEHEADER__
