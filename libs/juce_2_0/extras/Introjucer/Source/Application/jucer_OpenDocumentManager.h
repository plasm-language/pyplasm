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

#ifndef __JUCER_OPENDOCUMENTMANAGER_JUCEHEADER__
#define __JUCER_OPENDOCUMENTMANAGER_JUCEHEADER__

#include "../Project/jucer_Project.h"


//==============================================================================
/**
*/
class OpenDocumentManager
{
public:
    //==============================================================================
    OpenDocumentManager();
    ~OpenDocumentManager();

    juce_DeclareSingleton_SingleThreaded_Minimal (OpenDocumentManager);

    //==============================================================================
    class Document
    {
    public:
        Document() {}
        virtual ~Document() {}

        virtual bool loadedOk() const = 0;
        virtual bool isForFile (const File& file) const = 0;
        virtual bool isForNode (const ValueTree& node) const = 0;
        virtual bool refersToProject (Project& project) const = 0;
        virtual String getName() const = 0;
        virtual String getType() const = 0;
        virtual bool needsSaving() const = 0;
        virtual bool save() = 0;
        virtual bool canSaveAs() const = 0;
        virtual bool saveAs() = 0;
        virtual bool hasFileBeenModifiedExternally() = 0;
        virtual void reloadFromFile() = 0;
        virtual Component* createEditor() = 0;
        virtual Component* createViewer() = 0;
        virtual void fileHasBeenRenamed (const File& newFile) = 0;
    };

    //==============================================================================
    int getNumOpenDocuments() const;
    Document* getOpenDocument (int index) const;
    void moveDocumentToTopOfStack (Document* doc);

    bool canOpenFile (const File& file);
    Document* openFile (Project* project, const File& file);
    bool closeDocument (int index, bool saveIfNeeded);
    bool closeDocument (Document* document, bool saveIfNeeded);
    bool closeAll (bool askUserToSave);
    bool closeAllDocumentsUsingProject (Project& project, bool saveIfNeeded);
    void closeFile (const File& f, bool saveIfNeeded);
    bool anyFilesNeedSaving() const;
    bool saveAll();
    FileBasedDocument::SaveResult saveIfNeededAndUserAgrees (Document* doc);
    void reloadModifiedFiles();
    void fileHasBeenRenamed (const File& oldFile, const File& newFile);

    //==============================================================================
    class DocumentCloseListener
    {
    public:
        DocumentCloseListener() {}
        virtual ~DocumentCloseListener() {}

        virtual void documentAboutToClose (Document* document) = 0;
    };

    void addListener (DocumentCloseListener* listener);
    void removeListener (DocumentCloseListener* listener);

    //==============================================================================
    class DocumentType
    {
    public:
        DocumentType() {}
        virtual ~DocumentType() {}

        virtual bool canOpenFile (const File& file) = 0;
        virtual Document* openFile (Project* project, const File& file) = 0;
    };

    void registerType (DocumentType* type);


private:
    OwnedArray <DocumentType> types;
    OwnedArray <Document> documents;
    Array <DocumentCloseListener*> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenDocumentManager);
};


#endif   // __JUCER_OPENDOCUMENTMANAGER_JUCEHEADER__
