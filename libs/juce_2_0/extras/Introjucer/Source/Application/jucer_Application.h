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

#ifndef __JUCER_APPLICATION_JUCEHEADER__
#define __JUCER_APPLICATION_JUCEHEADER__

#include "../jucer_Headers.h"
#include "jucer_MainWindow.h"
#include "jucer_JuceUpdater.h"
#include "jucer_CommandLine.h"


//==============================================================================
class JucerApplication   : public JUCEApplication
{
public:
    //==============================================================================
    JucerApplication() {}
    ~JucerApplication() {}

    //==============================================================================
    void initialise (const String& commandLine)
    {
        if (commandLine.isNotEmpty())
        {
            const int appReturnCode = performCommandLine (commandLine);

            if (appReturnCode != commandLineNotPerformed)
            {
                setApplicationReturnValue (appReturnCode);
                quit();
                return;
            }
        }

        commandManager = new ApplicationCommandManager();
        commandManager->registerAllCommandsForTarget (this);

        menuModel = new MainMenuModel();

        doExtraInitialisation();

        ImageCache::setCacheTimeout (30 * 1000);

        if (commandLine.trim().isNotEmpty() && ! commandLine.trim().startsWithChar ('-'))
        {
            anotherInstanceStarted (commandLine);
        }
        else
        {
            Array<File> projects (StoredSettings::getInstance()->getLastProjects());

            for (int i = 0; i < projects.size(); ++ i)
                openFile (projects.getReference(i));
        }

        makeSureUserHasSelectedModuleFolder();

        if (mainWindows.size() == 0)
            createNewMainWindow()->makeVisible();

       #if JUCE_MAC
        MenuBarModel::setMacMainMenu (menuModel);
       #endif
    }

    void shutdown()
    {
       #if JUCE_MAC
        MenuBarModel::setMacMainMenu (nullptr);
       #endif
        menuModel = nullptr;

        StoredSettings::deleteInstance();
        mainWindows.clear();

        OpenDocumentManager::deleteInstance();
        commandManager = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit()
    {
        if (cancelAnyModalComponents())
        {
            new AsyncQuitRetrier();
            return;
        }

        while (mainWindows.size() > 0)
        {
            if (! mainWindows[0]->closeCurrentProject())
                return;

            mainWindows.remove (0);
        }

        quit();
    }

    void closeWindow (MainWindow* w)
    {
        jassert (mainWindows.contains (w));
        mainWindows.removeObject (w);

       #if ! JUCE_MAC
        if (mainWindows.size() == 0)
            systemRequestedQuit();
       #endif

        updateRecentProjectList();
    }

    //==============================================================================
    const String getApplicationName()
    {
        return String (ProjectInfo::projectName) + " " + getApplicationVersion();
    }

    const String getApplicationVersion()
    {
        return ProjectInfo::versionString;
    }

    bool moreThanOneInstanceAllowed()
    {
       #ifndef JUCE_LINUX
        return false;
       #else
        return true; //xxx should be false but doesn't work on linux..
       #endif
    }

    void anotherInstanceStarted (const String& commandLine)
    {
        openFile (commandLine.unquoted());
    }

    virtual void doExtraInitialisation() {}

    static JucerApplication* getApp()
    {
        return dynamic_cast<JucerApplication*> (JUCEApplication::getInstance());
    }

    //==============================================================================
    class MainMenuModel  : public MenuBarModel
    {
    public:
        MainMenuModel()
        {
            setApplicationCommandManagerToWatch (commandManager);
        }

        const StringArray getMenuBarNames()
        {
            const char* const names[] = { "File", "Edit", "View", "Window", "Tools", 0 };
            return StringArray ((const char**) names);
        }

        const PopupMenu getMenuForIndex (int topLevelMenuIndex, const String& /*menuName*/)
        {
            PopupMenu menu;

            if (topLevelMenuIndex == 0)    // "File" menu
            {
                menu.addCommandItem (commandManager, CommandIDs::newProject);
                menu.addSeparator();
                menu.addCommandItem (commandManager, CommandIDs::open);

                PopupMenu recentFiles;
                StoredSettings::getInstance()->recentFiles.createPopupMenuItems (recentFiles, 100, true, true);
                menu.addSubMenu ("Open recent file", recentFiles);

                menu.addSeparator();
                menu.addCommandItem (commandManager, CommandIDs::closeDocument);
                menu.addCommandItem (commandManager, CommandIDs::saveDocument);
                menu.addCommandItem (commandManager, CommandIDs::saveDocumentAs);
                menu.addSeparator();
                menu.addCommandItem (commandManager, CommandIDs::closeProject);
                menu.addCommandItem (commandManager, CommandIDs::saveProject);
                menu.addCommandItem (commandManager, CommandIDs::saveProjectAs);
                menu.addSeparator();
                menu.addCommandItem (commandManager, CommandIDs::openInIDE);
                menu.addCommandItem (commandManager, CommandIDs::saveAndOpenInIDE);

                #if ! JUCE_MAC
                  menu.addSeparator();
                  menu.addCommandItem (commandManager, StandardApplicationCommandIDs::quit);
                #endif
            }
            else if (topLevelMenuIndex == 1)    // "Edit" menu
            {
                menu.addCommandItem (commandManager, CommandIDs::undo);
                menu.addCommandItem (commandManager, CommandIDs::redo);
                menu.addSeparator();
                menu.addCommandItem (commandManager, StandardApplicationCommandIDs::cut);
                menu.addCommandItem (commandManager, StandardApplicationCommandIDs::copy);
                menu.addCommandItem (commandManager, StandardApplicationCommandIDs::paste);
                menu.addCommandItem (commandManager, StandardApplicationCommandIDs::del);
                menu.addCommandItem (commandManager, StandardApplicationCommandIDs::selectAll);
                menu.addCommandItem (commandManager, StandardApplicationCommandIDs::deselectAll);
                menu.addSeparator();
                menu.addCommandItem (commandManager, CommandIDs::toFront);
                menu.addCommandItem (commandManager, CommandIDs::toBack);
                menu.addSeparator();
                menu.addCommandItem (commandManager, CommandIDs::group);
                menu.addCommandItem (commandManager, CommandIDs::ungroup);
                menu.addSeparator();
                menu.addCommandItem (commandManager, CommandIDs::bringBackLostItems);
            }
            else if (topLevelMenuIndex == 2)    // "View" menu
            {
                menu.addCommandItem (commandManager, CommandIDs::showProjectSettings);
                menu.addSeparator();

                menu.addCommandItem (commandManager, CommandIDs::test);
                menu.addSeparator();

                menu.addCommandItem (commandManager, CommandIDs::showGrid);
                menu.addCommandItem (commandManager, CommandIDs::enableSnapToGrid);

                menu.addSeparator();
                menu.addCommandItem (commandManager, CommandIDs::zoomIn);
                menu.addCommandItem (commandManager, CommandIDs::zoomOut);
                menu.addCommandItem (commandManager, CommandIDs::zoomNormal);

                menu.addSeparator();
                menu.addCommandItem (commandManager, CommandIDs::useTabbedWindows);
            }
            else if (topLevelMenuIndex == 3)   // "Window" menu
            {
                menu.addCommandItem (commandManager, CommandIDs::closeWindow);
                menu.addSeparator();

                const int numDocs = jmin (50, OpenDocumentManager::getInstance()->getNumOpenDocuments());

                for (int i = 0; i < numDocs; ++i)
                {
                    OpenDocumentManager::Document* doc = OpenDocumentManager::getInstance()->getOpenDocument(i);

                    menu.addItem (300 + i, doc->getName());
                }

                menu.addSeparator();
                menu.addCommandItem (commandManager, CommandIDs::closeAllDocuments);
            }
            else if (topLevelMenuIndex == 4)  // "Tools" menu
            {
                menu.addCommandItem (commandManager, CommandIDs::updateModules);
                menu.addCommandItem (commandManager, CommandIDs::showUTF8Tool);
            }

            return menu;
        }

        void menuItemSelected (int menuItemID, int /*topLevelMenuIndex*/)
        {
            if (menuItemID >= 100 && menuItemID < 200)
            {
                // open a file from the "recent files" menu
                const File file (StoredSettings::getInstance()->recentFiles.getFile (menuItemID - 100));

                getApp()->openFile (file);
            }
            else if (menuItemID >= 300 && menuItemID < 400)
            {
                OpenDocumentManager::Document* doc = OpenDocumentManager::getInstance()->getOpenDocument (menuItemID - 300);

                MainWindow* w = getApp()->getOrCreateFrontmostWindow();
                w->makeVisible();
                w->getProjectContentComponent()->showDocument (doc);
                getApp()->avoidSuperimposedWindows (w);
            }
        }
    };

    //==============================================================================
    void getAllCommands (Array <CommandID>& commands)
    {
        JUCEApplication::getAllCommands (commands);

        const CommandID ids[] = { CommandIDs::newProject,
                                  CommandIDs::open,
                                  CommandIDs::showPrefs,
                                  CommandIDs::closeAllDocuments,
                                  CommandIDs::saveAll,
                                  CommandIDs::updateModules,
                                  CommandIDs::showUTF8Tool };

        commands.addArray (ids, numElementsInArray (ids));
    }

    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
    {
        switch (commandID)
        {
        case CommandIDs::newProject:
            result.setInfo ("New Project...", "Creates a new Jucer project", CommandCategories::general, 0);
            result.defaultKeypresses.add (KeyPress ('n', ModifierKeys::commandModifier, 0));
            break;

        case CommandIDs::open:
            result.setInfo ("Open...", "Opens a Jucer project", CommandCategories::general, 0);
            result.defaultKeypresses.add (KeyPress ('o', ModifierKeys::commandModifier, 0));
            break;

        case CommandIDs::showPrefs:
            result.setInfo ("Preferences...", "Shows the preferences panel.", CommandCategories::general, 0);
            result.defaultKeypresses.add (KeyPress (',', ModifierKeys::commandModifier, 0));
            break;

        case CommandIDs::closeAllDocuments:
            result.setInfo ("Close All Documents", "Closes all open documents", CommandCategories::general, 0);
            result.setActive (OpenDocumentManager::getInstance()->getNumOpenDocuments() > 0);
            break;

        case CommandIDs::saveAll:
            result.setInfo ("Save All", "Saves all open documents", CommandCategories::general, 0);
            result.setActive (OpenDocumentManager::getInstance()->anyFilesNeedSaving());
            break;

        case CommandIDs::updateModules:
            result.setInfo ("Download the latest JUCE modules", "Checks online for any JUCE modules updates and installs them", CommandCategories::general, 0);
            break;

        case CommandIDs::showUTF8Tool:
            result.setInfo ("UTF-8 String-Literal Helper", "Shows the UTF-8 string literal utility", CommandCategories::general, 0);
            break;

        default:
            JUCEApplication::getCommandInfo (commandID, result);
            break;
        }
    }

    bool perform (const InvocationInfo& info)
    {
        switch (info.commandID)
        {
            case CommandIDs::newProject:        createNewProject(); break;
            case CommandIDs::open:              askUserToOpenFile(); break;
            case CommandIDs::showPrefs:         showPrefsPanel(); break;
            case CommandIDs::saveAll:           OpenDocumentManager::getInstance()->saveAll(); break;
            case CommandIDs::closeAllDocuments: closeAllDocuments (true); break;
            case CommandIDs::showUTF8Tool:      showUTF8ToolWindow(); break;
            case CommandIDs::updateModules:     runModuleUpdate (String::empty); break;

            default:                            return JUCEApplication::perform (info);
        }

        return true;
    }

    //==============================================================================
    void showPrefsPanel()
    {
        jassertfalse;
    }

    void createNewProject()
    {
        if (makeSureUserHasSelectedModuleFolder())
        {
            MainWindow* mw = getOrCreateEmptyWindow();
            mw->showNewProjectWizard();
            avoidSuperimposedWindows (mw);
        }
    }

    void askUserToOpenFile()
    {
        FileChooser fc ("Open File");

        if (fc.browseForFileToOpen())
            openFile (fc.getResult());
    }

    bool openFile (const File& file)
    {
        for (int j = mainWindows.size(); --j >= 0;)
        {
            if (mainWindows.getUnchecked(j)->getProject() != nullptr
                 && mainWindows.getUnchecked(j)->getProject()->getFile() == file)
            {
                mainWindows.getUnchecked(j)->toFront (true);
                return true;
            }
        }

        if (file.hasFileExtension (Project::projectFileExtension))
        {
            ScopedPointer <Project> newDoc (new Project (file));

            if (newDoc->loadFrom (file, true))
            {
                MainWindow* w = getOrCreateEmptyWindow();
                w->setProject (newDoc.release());
                w->makeVisible();
                avoidSuperimposedWindows (w);
                return true;
            }
        }
        else if (file.exists())
        {
            MainWindow* w = getOrCreateFrontmostWindow();

            const bool ok = w->openFile (file);
            w->makeVisible();
            avoidSuperimposedWindows (w);
            return ok;
        }

        return false;
    }

    bool closeAllDocuments (bool askUserToSave)
    {
        return OpenDocumentManager::getInstance()->closeAll (askUserToSave);
    }

    void updateRecentProjectList()
    {
        Array<File> projects;

        for (int i = 0; i < mainWindows.size(); ++i)
        {
            MainWindow* mw = mainWindows[i];

            if (mw != nullptr && mw->getProject() != nullptr)
                projects.add (mw->getProject()->getFile());
        }

        StoredSettings::getInstance()->setLastProjects (projects);
    }

    bool makeSureUserHasSelectedModuleFolder()
    {
        if (! ModuleList::isLocalModulesFolderValid())
        {
            if (! runModuleUpdate ("Please select a location to store your local set of JUCE modules,\n"
                                   "and download the ones that you'd like to use!"))
            {
                AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                             "Introjucer",
                                             "Unless you create a local JUCE folder containing some modules, you'll be unable to save any projects correctly!\n\n"
                                             "Use the option on the 'Tools' menu to set this up!");

                return false;
            }
        }

        return true;
    }

    bool runModuleUpdate (const String& message)
    {
        ModuleList list;
        list.rescan (ModuleList::getDefaultModulesFolder (nullptr));
        JuceUpdater::show (list, mainWindows[0], message);

        ModuleList::setLocalModulesFolder (list.getModulesFolder());
        return ModuleList::isJuceOrModulesFolder (list.getModulesFolder());
    }

    ScopedPointer<MainMenuModel> menuModel;

private:
    OwnedArray <MainWindow> mainWindows;

    MainWindow* createNewMainWindow()
    {
        MainWindow* mw = new MainWindow();
        mainWindows.add (mw);
        mw->restoreWindowPosition();
        avoidSuperimposedWindows (mw);
        return mw;
    }

    MainWindow* getOrCreateFrontmostWindow()
    {
        if (mainWindows.size() == 0)
            return createNewMainWindow();

        for (int i = Desktop::getInstance().getNumComponents(); --i >= 0;)
        {
            MainWindow* mw = dynamic_cast <MainWindow*> (Desktop::getInstance().getComponent (i));
            if (mainWindows.contains (mw))
                return mw;
        }

        return mainWindows.getLast();
    }

    MainWindow* getOrCreateEmptyWindow()
    {
        if (mainWindows.size() == 0)
            return createNewMainWindow();

        for (int i = Desktop::getInstance().getNumComponents(); --i >= 0;)
        {
            MainWindow* mw = dynamic_cast <MainWindow*> (Desktop::getInstance().getComponent (i));
            if (mainWindows.contains (mw) && mw->getProject() == nullptr)
                return mw;
        }

        return createNewMainWindow();
    }

    void avoidSuperimposedWindows (MainWindow* const mw)
    {
        for (int i = mainWindows.size(); --i >= 0;)
        {
            MainWindow* const other = mainWindows.getUnchecked(i);

            const Rectangle<int> b1 (mw->getBounds());
            const Rectangle<int> b2 (other->getBounds());

            if (mw != other
                 && std::abs (b1.getX() - b2.getX()) < 3
                 && std::abs (b1.getY() - b2.getY()) < 3
                 && std::abs (b1.getRight() - b2.getRight()) < 3
                 && std::abs (b1.getBottom() - b2.getBottom()) < 3)
            {
                int dx = 40, dy = 30;

                if (b1.getCentreX() >= mw->getScreenBounds().getCentreX())   dx = -dx;
                if (b1.getCentreY() >= mw->getScreenBounds().getCentreY())   dy = -dy;

                mw->setBounds (b1.translated (dx, dy));
            }
        }
    }

    //==============================================================================
    class AsyncQuitRetrier  : public Timer
    {
    public:
        AsyncQuitRetrier()   { startTimer (500); }

        void timerCallback()
        {
            stopTimer();
            delete this;

            if (getApp() != nullptr)
                getApp()->systemRequestedQuit();
        }

        JUCE_DECLARE_NON_COPYABLE (AsyncQuitRetrier);
    };
};


#endif   // __JUCER_APPLICATION_JUCEHEADER__
