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

ReadWriteLock::ReadWriteLock() noexcept
    : numWaitingWriters (0),
      numWriters (0),
      writerThreadId (0)
{
    readerThreads.ensureStorageAllocated (16);
}

ReadWriteLock::~ReadWriteLock() noexcept
{
    jassert (readerThreads.size() == 0);
    jassert (numWriters == 0);
}

//==============================================================================
void ReadWriteLock::enterRead() const noexcept
{
    const Thread::ThreadID threadId = Thread::getCurrentThreadId();
    const SpinLock::ScopedLockType sl (accessLock);

    for (;;)
    {
        jassert (readerThreads.size() % 2 == 0);

        int i;
        for (i = 0; i < readerThreads.size(); i += 2)
            if (readerThreads.getUnchecked(i) == threadId)
                break;

        if (i < readerThreads.size()
              || numWriters + numWaitingWriters == 0
              || (threadId == writerThreadId && numWriters > 0))
        {
            if (i < readerThreads.size())
            {
                readerThreads.set (i + 1, (Thread::ThreadID) (1 + (pointer_sized_int) readerThreads.getUnchecked (i + 1)));
            }
            else
            {
                readerThreads.add (threadId);
                readerThreads.add ((Thread::ThreadID) 1);
            }

            return;
        }

        const SpinLock::ScopedUnlockType ul (accessLock);
        waitEvent.wait (100);
    }
}

void ReadWriteLock::exitRead() const noexcept
{
    const Thread::ThreadID threadId = Thread::getCurrentThreadId();
    const SpinLock::ScopedLockType sl (accessLock);

    for (int i = 0; i < readerThreads.size(); i += 2)
    {
        if (readerThreads.getUnchecked(i) == threadId)
        {
            const pointer_sized_int newCount = ((pointer_sized_int) readerThreads.getUnchecked (i + 1)) - 1;

            if (newCount == 0)
            {
                readerThreads.removeRange (i, 2);
                waitEvent.signal();
            }
            else
            {
                readerThreads.set (i + 1, (Thread::ThreadID) newCount);
            }

            return;
        }
    }

    jassertfalse; // unlocking a lock that wasn't locked..
}

//==============================================================================
void ReadWriteLock::enterWrite() const noexcept
{
    const Thread::ThreadID threadId = Thread::getCurrentThreadId();
    const SpinLock::ScopedLockType sl (accessLock);

    for (;;)
    {
        if (readerThreads.size() + numWriters == 0
             || threadId == writerThreadId
             || (readerThreads.size() == 2
                  && readerThreads.getUnchecked(0) == threadId))
        {
            writerThreadId = threadId;
            ++numWriters;
            break;
        }

        ++numWaitingWriters;
        accessLock.exit();
        waitEvent.wait (100);
        accessLock.enter();
        --numWaitingWriters;
    }
}

bool ReadWriteLock::tryEnterWrite() const noexcept
{
    const Thread::ThreadID threadId = Thread::getCurrentThreadId();
    const SpinLock::ScopedLockType sl (accessLock);

    if (readerThreads.size() + numWriters == 0
         || threadId == writerThreadId
         || (readerThreads.size() == 2
              && readerThreads.getUnchecked(0) == threadId))
    {
        writerThreadId = threadId;
        ++numWriters;
        return true;
    }

    return false;
}

void ReadWriteLock::exitWrite() const noexcept
{
    const SpinLock::ScopedLockType sl (accessLock);

    // check this thread actually had the lock..
    jassert (numWriters > 0 && writerThreadId == Thread::getCurrentThreadId());

    if (--numWriters == 0)
    {
        writerThreadId = 0;
        waitEvent.signal();
    }
}
