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

class AudioThumbnailCache::ThumbnailCacheEntry
{
public:
    ThumbnailCacheEntry (const int64 hash_)
        : hash (hash_),
          lastUsed (Time::getMillisecondCounter())
    {
    }

    ThumbnailCacheEntry (InputStream& in)
        : lastUsed (0)
    {
        hash = in.readInt64();
        const int64 len = in.readInt64();
        in.readIntoMemoryBlock (data, (ssize_t) len);
    }

    void write (OutputStream& out)
    {
        out.writeInt64 (hash);
        out.writeInt64 ((int64) data.getSize());
        out << data;
    }

    int64 hash;
    uint32 lastUsed;
    MemoryBlock data;

private:
    JUCE_LEAK_DETECTOR (ThumbnailCacheEntry);
};

//==============================================================================
AudioThumbnailCache::AudioThumbnailCache (const int maxNumThumbsToStore_)
    : TimeSliceThread ("thumb cache"),
      maxNumThumbsToStore (maxNumThumbsToStore_)
{
    jassert (maxNumThumbsToStore > 0);
    startThread (2);
}

AudioThumbnailCache::~AudioThumbnailCache()
{
}

AudioThumbnailCache::ThumbnailCacheEntry* AudioThumbnailCache::findThumbFor (const int64 hash) const
{
    for (int i = thumbs.size(); --i >= 0;)
        if (thumbs.getUnchecked(i)->hash == hash)
            return thumbs.getUnchecked(i);

    return nullptr;
}

int AudioThumbnailCache::findOldestThumb() const
{
    int oldest = 0;
    uint32 oldestTime = Time::getMillisecondCounter() + 1;

    for (int i = thumbs.size(); --i >= 0;)
    {
        const ThumbnailCacheEntry* const te = thumbs.getUnchecked(i);

        if (te->lastUsed < oldestTime)
        {
            oldest = i;
            oldestTime = te->lastUsed;
        }
    }

    return oldest;
}

bool AudioThumbnailCache::loadThumb (AudioThumbnail& thumb, const int64 hashCode)
{
    const ScopedLock sl (lock);
    ThumbnailCacheEntry* te = findThumbFor (hashCode);

    if (te != nullptr)
    {
        te->lastUsed = Time::getMillisecondCounter();

        MemoryInputStream in (te->data, false);
        thumb.loadFrom (in);
        return true;
    }

    return false;
}

void AudioThumbnailCache::storeThumb (const AudioThumbnail& thumb,
                                      const int64 hashCode)
{
    const ScopedLock sl (lock);
    ThumbnailCacheEntry* te = findThumbFor (hashCode);

    if (te == nullptr)
    {
        te = new ThumbnailCacheEntry (hashCode);

        if (thumbs.size() < maxNumThumbsToStore)
            thumbs.add (te);
        else
            thumbs.set (findOldestThumb(), te);
    }

    MemoryOutputStream out (te->data, false);
    thumb.saveTo (out);
}

void AudioThumbnailCache::clear()
{
    const ScopedLock sl (lock);
    thumbs.clear();
}

static inline int getThumbnailCacheFileMagicHeader() noexcept
{
    return (int) ByteOrder::littleEndianInt ("ThmC");
}

bool AudioThumbnailCache::readFromStream (InputStream& source)
{
    if (source.readInt() != getThumbnailCacheFileMagicHeader())
        return false;

    const ScopedLock sl (lock);
    clear();
    int numThumbnails = jmin (maxNumThumbsToStore, source.readInt());

    while (--numThumbnails >= 0 && ! source.isExhausted())
        thumbs.add (new ThumbnailCacheEntry (source));

    return true;
}

void AudioThumbnailCache::writeToStream (OutputStream& out)
{
    const ScopedLock sl (lock);

    out.writeInt (getThumbnailCacheFileMagicHeader());
    out.writeInt (thumbs.size());

    for (int i = 0; i < thumbs.size(); ++i)
        thumbs.getUnchecked(i)->write (out);
}
