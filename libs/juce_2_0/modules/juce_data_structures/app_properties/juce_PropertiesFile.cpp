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

namespace PropertyFileConstants
{
    static const int magicNumber            = (int) ByteOrder::littleEndianInt ("PROP");
    static const int magicNumberCompressed  = (int) ByteOrder::littleEndianInt ("CPRP");

    static const char* const fileTag        = "PROPERTIES";
    static const char* const valueTag       = "VALUE";
    static const char* const nameAttribute  = "name";
    static const char* const valueAttribute = "val";
}

//==============================================================================
PropertiesFile::Options::Options()
    : commonToAllUsers (false),
      ignoreCaseOfKeyNames (false),
      millisecondsBeforeSaving (3000),
      storageFormat (PropertiesFile::storeAsXML),
      processLock (nullptr)
{
}

File PropertiesFile::Options::getDefaultFile() const
{
    // mustn't have illegal characters in this name..
    jassert (applicationName == File::createLegalFileName (applicationName));

  #if JUCE_MAC || JUCE_IOS
    File dir (commonToAllUsers ?  "/Library/"
                               : "~/Library/");

    if (osxLibrarySubFolder != "Preferences" && osxLibrarySubFolder != "Application Support")
    {
        /* The PropertiesFile class always used to put its settings files in "Library/Preferences", but Apple
           have changed their advice, and now stipulate that settings should go in "Library/Application Support".

           Because older apps would be broken by a silent change in this class's behaviour, you must now
           explicitly set the osxLibrarySubFolder value to indicate which path you want to use.

           In newer apps, you should always set this to "Application Support".

           If your app needs to load settings files that were created by older versions of juce and
           you want to maintain backwards-compatibility, then you can set this to "Preferences".
           But.. for better Apple-compliance, the recommended approach would be to write some code that
           finds your old settings files in ~/Library/Preferences, moves them to ~/Library/Application Support,
           and then uses the new path.
        */
        jassertfalse;

        dir = dir.getChildFile ("Application Support");
    }
    else
    {
        dir = dir.getChildFile (osxLibrarySubFolder);
    }

    if (folderName.isNotEmpty())
        dir = dir.getChildFile (folderName);

  #elif JUCE_LINUX || JUCE_ANDROID
    const File dir ((commonToAllUsers ? "/var/" : "~/")
                        + (folderName.isNotEmpty() ? folderName
                                                   : ("." + applicationName)));

  #elif JUCE_WINDOWS
    File dir (File::getSpecialLocation (commonToAllUsers ? File::commonApplicationDataDirectory
                                                         : File::userApplicationDataDirectory));

    if (dir == File::nonexistent)
        return File::nonexistent;

    dir = dir.getChildFile (folderName.isNotEmpty() ? folderName
                                                    : applicationName);
  #endif

    return dir.getChildFile (applicationName)
              .withFileExtension (filenameSuffix);
}


//==============================================================================
PropertiesFile::PropertiesFile (const File& file_, const Options& options_)
    : PropertySet (options_.ignoreCaseOfKeyNames),
      file (file_), options (options_),
      loadedOk (false), needsWriting (false)
{
    initialise();
}

PropertiesFile::PropertiesFile (const Options& options_)
    : PropertySet (options_.ignoreCaseOfKeyNames),
      file (options_.getDefaultFile()), options (options_),
      loadedOk (false), needsWriting (false)
{
    initialise();
}

void PropertiesFile::initialise()
{
    // You need to correctly specify just one storage format for the file
    ProcessScopedLock pl (createProcessLock());

    if (pl != nullptr && ! pl->isLocked())
        return; // locking failure..

    ScopedPointer<InputStream> fileStream (file.createInputStream());

    if (fileStream != nullptr)
    {
        int magicNumber = fileStream->readInt();

        if (magicNumber == PropertyFileConstants::magicNumberCompressed)
        {
            fileStream = new GZIPDecompressorInputStream (new SubregionStream (fileStream.release(), 4, -1, true), true);
            magicNumber = PropertyFileConstants::magicNumber;
        }

        if (magicNumber == PropertyFileConstants::magicNumber)
        {
            loadedOk = true;
            BufferedInputStream in (fileStream.release(), 2048, true);

            int numValues = in.readInt();

            while (--numValues >= 0 && ! in.isExhausted())
            {
                const String key (in.readString());
                const String value (in.readString());

                jassert (key.isNotEmpty());
                if (key.isNotEmpty())
                    getAllProperties().set (key, value);
            }
        }
        else
        {
            // Not a binary props file - let's see if it's XML..
            fileStream = nullptr;

            XmlDocument parser (file);
            ScopedPointer<XmlElement> doc (parser.getDocumentElement (true));

            if (doc != nullptr && doc->hasTagName (PropertyFileConstants::fileTag))
            {
                doc = parser.getDocumentElement();

                if (doc != nullptr)
                {
                    loadedOk = true;

                    forEachXmlChildElementWithTagName (*doc, e, PropertyFileConstants::valueTag)
                    {
                        const String name (e->getStringAttribute (PropertyFileConstants::nameAttribute));

                        if (name.isNotEmpty())
                        {
                            getAllProperties().set (name,
                                                    e->getFirstChildElement() != nullptr
                                                        ? e->getFirstChildElement()->createDocument (String::empty, true)
                                                        : e->getStringAttribute (PropertyFileConstants::valueAttribute));
                        }
                    }
                }
                else
                {
                    // must be a pretty broken XML file we're trying to parse here,
                    // or a sign that this object needs an InterProcessLock,
                    // or just a failure reading the file.  This last reason is why
                    // we don't jassertfalse here.
                }
            }
        }
    }
    else
    {
        loadedOk = ! file.exists();
    }
}

PropertiesFile::~PropertiesFile()
{
    if (! saveIfNeeded())
        jassertfalse;
}

InterProcessLock::ScopedLockType* PropertiesFile::createProcessLock() const
{
    return options.processLock != nullptr ? new InterProcessLock::ScopedLockType (*options.processLock) : nullptr;
}

bool PropertiesFile::saveIfNeeded()
{
    const ScopedLock sl (getLock());
    return (! needsWriting) || save();
}

bool PropertiesFile::needsToBeSaved() const
{
    const ScopedLock sl (getLock());
    return needsWriting;
}

void PropertiesFile::setNeedsToBeSaved (const bool needsToBeSaved_)
{
    const ScopedLock sl (getLock());
    needsWriting = needsToBeSaved_;
}

bool PropertiesFile::save()
{
    const ScopedLock sl (getLock());

    stopTimer();

    if (file == File::nonexistent
         || file.isDirectory()
         || ! file.getParentDirectory().createDirectory())
        return false;

    if (options.storageFormat == storeAsXML)
    {
        XmlElement doc (PropertyFileConstants::fileTag);

        for (int i = 0; i < getAllProperties().size(); ++i)
        {
            XmlElement* const e = doc.createNewChildElement (PropertyFileConstants::valueTag);
            e->setAttribute (PropertyFileConstants::nameAttribute, getAllProperties().getAllKeys() [i]);

            // if the value seems to contain xml, store it as such..
            XmlElement* const childElement = XmlDocument::parse (getAllProperties().getAllValues() [i]);

            if (childElement != nullptr)
                e->addChildElement (childElement);
            else
                e->setAttribute (PropertyFileConstants::valueAttribute,
                                 getAllProperties().getAllValues() [i]);
        }

        ProcessScopedLock pl (createProcessLock());

        if (pl != nullptr && ! pl->isLocked())
            return false; // locking failure..

        if (doc.writeToFile (file, String::empty))
        {
            needsWriting = false;
            return true;
        }
    }
    else
    {
        ProcessScopedLock pl (createProcessLock());

        if (pl != nullptr && ! pl->isLocked())
            return false; // locking failure..

        TemporaryFile tempFile (file);
        ScopedPointer <OutputStream> out (tempFile.getFile().createOutputStream());

        if (out != nullptr)
        {
            if (options.storageFormat == storeAsCompressedBinary)
            {
                out->writeInt (PropertyFileConstants::magicNumberCompressed);
                out->flush();

                out = new GZIPCompressorOutputStream (out.release(), 9, true);
            }
            else
            {
                // have you set up the storage option flags correctly?
                jassert (options.storageFormat == storeAsBinary);

                out->writeInt (PropertyFileConstants::magicNumber);
            }

            const int numProperties = getAllProperties().size();

            out->writeInt (numProperties);

            for (int i = 0; i < numProperties; ++i)
            {
                out->writeString (getAllProperties().getAllKeys() [i]);
                out->writeString (getAllProperties().getAllValues() [i]);
            }

            out = nullptr;

            if (tempFile.overwriteTargetFileWithTemporary())
            {
                needsWriting = false;
                return true;
            }
        }
    }

    return false;
}

void PropertiesFile::timerCallback()
{
    saveIfNeeded();
}

void PropertiesFile::propertyChanged()
{
    sendChangeMessage();

    needsWriting = true;

    if (options.millisecondsBeforeSaving > 0)
        startTimer (options.millisecondsBeforeSaving);
    else if (options.millisecondsBeforeSaving == 0)
        saveIfNeeded();
}
