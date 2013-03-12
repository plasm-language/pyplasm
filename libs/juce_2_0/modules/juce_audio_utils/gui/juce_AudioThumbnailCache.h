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

#ifndef __JUCE_AUDIOTHUMBNAILCACHE_JUCEHEADER__
#define __JUCE_AUDIOTHUMBNAILCACHE_JUCEHEADER__

#include "juce_AudioThumbnail.h"


//==============================================================================
/**
    An instance of this class is used to manage multiple AudioThumbnail objects.

    The cache runs a single background thread that is shared by all the thumbnails
    that need it, and it maintains a set of low-res previews in memory, to avoid
    having to re-scan audio files too often.

    @see AudioThumbnail
*/
class JUCE_API  AudioThumbnailCache   : public TimeSliceThread
{
public:
    //==============================================================================
    /** Creates a cache object.

        The maxNumThumbsToStore parameter lets you specify how many previews should
        be kept in memory at once.
    */
    explicit AudioThumbnailCache (int maxNumThumbsToStore);

    /** Destructor. */
    ~AudioThumbnailCache();

    //==============================================================================
    /** Clears out any stored thumbnails.
    */
    void clear();

    /** Reloads the specified thumb if this cache contains the appropriate stored
        data.

        This is called automatically by the AudioThumbnail class, so you shouldn't
        normally need to call it directly.
    */
    bool loadThumb (AudioThumbnail& thumb, int64 hashCode);

    /** Stores the cachable data from the specified thumb in this cache.

        This is called automatically by the AudioThumbnail class, so you shouldn't
        normally need to call it directly.
    */
    void storeThumb (const AudioThumbnail& thumb, int64 hashCode);

    //==============================================================================
    /** Attempts to re-load a saved cache of thumbnails from a stream.
        The cache data must have been written by the writeToStream() method.
        This will replace all currently-loaded thumbnails with the new data.
    */
    bool readFromStream (InputStream& source);

    /** Writes all currently-loaded cache data to a stream.
        The resulting data can be re-loaded with readFromStream().
    */
    void writeToStream (OutputStream& stream);

private:
    //==============================================================================
    class ThumbnailCacheEntry;
    friend class OwnedArray<ThumbnailCacheEntry>;
    OwnedArray<ThumbnailCacheEntry> thumbs;
    CriticalSection lock;
    int maxNumThumbsToStore;

    ThumbnailCacheEntry* findThumbFor (int64 hash) const;
    int findOldestThumb() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioThumbnailCache);
};


#endif   // __JUCE_AUDIOTHUMBNAILCACHE_JUCEHEADER__
