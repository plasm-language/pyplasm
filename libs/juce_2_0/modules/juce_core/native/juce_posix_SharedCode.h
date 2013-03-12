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

/*
    This file contains posix routines that are common to both the Linux and Mac builds.

    It gets included directly in the cpp files for these platforms.
*/


//==============================================================================
CriticalSection::CriticalSection() noexcept
{
    pthread_mutexattr_t atts;
    pthread_mutexattr_init (&atts);
    pthread_mutexattr_settype (&atts, PTHREAD_MUTEX_RECURSIVE);
   #if ! JUCE_ANDROID
    pthread_mutexattr_setprotocol (&atts, PTHREAD_PRIO_INHERIT);
   #endif
    pthread_mutex_init (&internal, &atts);
}

CriticalSection::~CriticalSection() noexcept
{
    pthread_mutex_destroy (&internal);
}

void CriticalSection::enter() const noexcept
{
    pthread_mutex_lock (&internal);
}

bool CriticalSection::tryEnter() const noexcept
{
    return pthread_mutex_trylock (&internal) == 0;
}

void CriticalSection::exit() const noexcept
{
    pthread_mutex_unlock (&internal);
}


//==============================================================================
class WaitableEventImpl
{
public:
    WaitableEventImpl (const bool manualReset_)
        : triggered (false),
          manualReset (manualReset_)
    {
        pthread_cond_init (&condition, 0);

        pthread_mutexattr_t atts;
        pthread_mutexattr_init (&atts);
       #if ! JUCE_ANDROID
        pthread_mutexattr_setprotocol (&atts, PTHREAD_PRIO_INHERIT);
       #endif
        pthread_mutex_init (&mutex, &atts);
    }

    ~WaitableEventImpl()
    {
        pthread_cond_destroy (&condition);
        pthread_mutex_destroy (&mutex);
    }

    bool wait (const int timeOutMillisecs) noexcept
    {
        pthread_mutex_lock (&mutex);

        if (! triggered)
        {
            if (timeOutMillisecs < 0)
            {
                do
                {
                    pthread_cond_wait (&condition, &mutex);
                }
                while (! triggered);
            }
            else
            {
                struct timeval now;
                gettimeofday (&now, 0);

                struct timespec time;
                time.tv_sec  = now.tv_sec  + (timeOutMillisecs / 1000);
                time.tv_nsec = (now.tv_usec + ((timeOutMillisecs % 1000) * 1000)) * 1000;

                if (time.tv_nsec >= 1000000000)
                {
                    time.tv_nsec -= 1000000000;
                    time.tv_sec++;
                }

                do
                {
                    if (pthread_cond_timedwait (&condition, &mutex, &time) == ETIMEDOUT)
                    {
                        pthread_mutex_unlock (&mutex);
                        return false;
                    }
                }
                while (! triggered);
            }
        }

        if (! manualReset)
            triggered = false;

        pthread_mutex_unlock (&mutex);
        return true;
    }

    void signal() noexcept
    {
        pthread_mutex_lock (&mutex);
        triggered = true;
        pthread_cond_broadcast (&condition);
        pthread_mutex_unlock (&mutex);
    }

    void reset() noexcept
    {
        pthread_mutex_lock (&mutex);
        triggered = false;
        pthread_mutex_unlock (&mutex);
    }

private:
    pthread_cond_t condition;
    pthread_mutex_t mutex;
    bool triggered;
    const bool manualReset;

    JUCE_DECLARE_NON_COPYABLE (WaitableEventImpl);
};

WaitableEvent::WaitableEvent (const bool manualReset) noexcept
    : internal (new WaitableEventImpl (manualReset))
{
}

WaitableEvent::~WaitableEvent() noexcept
{
    delete static_cast <WaitableEventImpl*> (internal);
}

bool WaitableEvent::wait (const int timeOutMillisecs) const noexcept
{
    return static_cast <WaitableEventImpl*> (internal)->wait (timeOutMillisecs);
}

void WaitableEvent::signal() const noexcept
{
    static_cast <WaitableEventImpl*> (internal)->signal();
}

void WaitableEvent::reset() const noexcept
{
    static_cast <WaitableEventImpl*> (internal)->reset();
}

//==============================================================================
void JUCE_CALLTYPE Thread::sleep (int millisecs)
{
    struct timespec time;
    time.tv_sec = millisecs / 1000;
    time.tv_nsec = (millisecs % 1000) * 1000000;
    nanosleep (&time, 0);
}


//==============================================================================
const juce_wchar File::separator = '/';
const String File::separatorString ("/");

//==============================================================================
File File::getCurrentWorkingDirectory()
{
    HeapBlock<char> heapBuffer;

    char localBuffer [1024];
    char* cwd = getcwd (localBuffer, sizeof (localBuffer) - 1);
    int bufferSize = 4096;

    while (cwd == nullptr && errno == ERANGE)
    {
        heapBuffer.malloc (bufferSize);
        cwd = getcwd (heapBuffer, bufferSize - 1);
        bufferSize += 1024;
    }

    return File (CharPointer_UTF8 (cwd));
}

bool File::setAsCurrentWorkingDirectory() const
{
    return chdir (getFullPathName().toUTF8()) == 0;
}

//==============================================================================
namespace
{
   #if JUCE_LINUX || (JUCE_IOS && ! __DARWIN_ONLY_64_BIT_INO_T) // (this iOS stuff is to avoid a simulator bug)
    typedef struct stat64 juce_statStruct;
    #define JUCE_STAT     stat64
   #else
    typedef struct stat   juce_statStruct;
    #define JUCE_STAT     stat
   #endif

    bool juce_stat (const String& fileName, juce_statStruct& info)
    {
        return fileName.isNotEmpty()
                 && JUCE_STAT (fileName.toUTF8(), &info) == 0;
    }

    // if this file doesn't exist, find a parent of it that does..
    bool juce_doStatFS (File f, struct statfs& result)
    {
        for (int i = 5; --i >= 0;)
        {
            if (f.exists())
                break;

            f = f.getParentDirectory();
        }

        return statfs (f.getFullPathName().toUTF8(), &result) == 0;
    }

    void updateStatInfoForFile (const String& path, bool* const isDir, int64* const fileSize,
                                Time* const modTime, Time* const creationTime, bool* const isReadOnly)
    {
        if (isDir != nullptr || fileSize != nullptr || modTime != nullptr || creationTime != nullptr)
        {
            juce_statStruct info;
            const bool statOk = juce_stat (path, info);

            if (isDir != nullptr)         *isDir        = statOk && ((info.st_mode & S_IFDIR) != 0);
            if (fileSize != nullptr)      *fileSize     = statOk ? info.st_size : 0;
            if (modTime != nullptr)       *modTime      = Time (statOk ? (int64) info.st_mtime * 1000 : 0);
            if (creationTime != nullptr)  *creationTime = Time (statOk ? (int64) info.st_ctime * 1000 : 0);
        }

        if (isReadOnly != nullptr)
            *isReadOnly = access (path.toUTF8(), W_OK) != 0;
    }

    Result getResultForErrno()
    {
        return Result::fail (String (strerror (errno)));
    }

    Result getResultForReturnValue (int value)
    {
        return value == -1 ? getResultForErrno() : Result::ok();
    }

    int getFD (void* handle) noexcept
    {
        return (int) (pointer_sized_int) handle;
    }
}

bool File::isDirectory() const
{
    juce_statStruct info;

    return fullPath.isEmpty()
            || (juce_stat (fullPath, info) && ((info.st_mode & S_IFDIR) != 0));
}

bool File::exists() const
{
    return fullPath.isNotEmpty()
             && access (fullPath.toUTF8(), F_OK) == 0;
}

bool File::existsAsFile() const
{
    return exists() && ! isDirectory();
}

int64 File::getSize() const
{
    juce_statStruct info;
    return juce_stat (fullPath, info) ? info.st_size : 0;
}

//==============================================================================
bool File::hasWriteAccess() const
{
    if (exists())
        return access (fullPath.toUTF8(), W_OK) == 0;

    if ((! isDirectory()) && fullPath.containsChar (separator))
        return getParentDirectory().hasWriteAccess();

    return false;
}

bool File::setFileReadOnlyInternal (const bool shouldBeReadOnly) const
{
    juce_statStruct info;
    if (! juce_stat (fullPath, info))
        return false;

    info.st_mode &= 0777;   // Just permissions

    if (shouldBeReadOnly)
        info.st_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
    else
        // Give everybody write permission?
        info.st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;

    return chmod (fullPath.toUTF8(), info.st_mode) == 0;
}

void File::getFileTimesInternal (int64& modificationTime, int64& accessTime, int64& creationTime) const
{
    modificationTime = 0;
    accessTime = 0;
    creationTime = 0;

    juce_statStruct info;
    if (juce_stat (fullPath, info))
    {
        modificationTime = (int64) info.st_mtime * 1000;
        accessTime = (int64) info.st_atime * 1000;
        creationTime = (int64) info.st_ctime * 1000;
    }
}

bool File::setFileTimesInternal (int64 modificationTime, int64 accessTime, int64 /*creationTime*/) const
{
    juce_statStruct info;

    if ((modificationTime != 0 || accessTime != 0) && juce_stat (fullPath, info))
    {
        struct utimbuf times;
        times.actime  = accessTime != 0       ? (time_t) (accessTime / 1000)       : info.st_atime;
        times.modtime = modificationTime != 0 ? (time_t) (modificationTime / 1000) : info.st_mtime;

        return utime (fullPath.toUTF8(), &times) == 0;
    }

    return false;
}

bool File::deleteFile() const
{
    if (! exists())
        return true;

    if (isDirectory())
        return rmdir (fullPath.toUTF8()) == 0;

    return remove (fullPath.toUTF8()) == 0;
}

bool File::moveInternal (const File& dest) const
{
    if (rename (fullPath.toUTF8(), dest.getFullPathName().toUTF8()) == 0)
        return true;

    if (hasWriteAccess() && copyInternal (dest))
    {
        if (deleteFile())
            return true;

        dest.deleteFile();
    }

    return false;
}

Result File::createDirectoryInternal (const String& fileName) const
{
    return getResultForReturnValue (mkdir (fileName.toUTF8(), 0777));
}

//=====================================================================
int64 juce_fileSetPosition (void* handle, int64 pos)
{
    if (handle != 0 && lseek (getFD (handle), pos, SEEK_SET) == pos)
        return pos;

    return -1;
}

void FileInputStream::openHandle()
{
    totalSize = file.getSize();

    const int f = open (file.getFullPathName().toUTF8(), O_RDONLY, 00644);

    if (f != -1)
        fileHandle = (void*) f;
    else
        status = getResultForErrno();
}

void FileInputStream::closeHandle()
{
    if (fileHandle != 0)
    {
        close (getFD (fileHandle));
        fileHandle = 0;
    }
}

size_t FileInputStream::readInternal (void* const buffer, const size_t numBytes)
{
    ssize_t result = 0;

    if (fileHandle != 0)
    {
        result = ::read (getFD (fileHandle), buffer, numBytes);

        if (result < 0)
        {
            status = getResultForErrno();
            result = 0;
        }
    }

    return (size_t) result;
}

//==============================================================================
void FileOutputStream::openHandle()
{
    if (file.exists())
    {
        const int f = open (file.getFullPathName().toUTF8(), O_RDWR, 00644);

        if (f != -1)
        {
            currentPosition = lseek (f, 0, SEEK_END);

            if (currentPosition >= 0)
            {
                fileHandle = (void*) f;
            }
            else
            {
                status = getResultForErrno();
                close (f);
            }
        }
        else
        {
            status = getResultForErrno();
        }
    }
    else
    {
        const int f = open (file.getFullPathName().toUTF8(), O_RDWR + O_CREAT, 00644);

        if (f != -1)
            fileHandle = (void*) f;
        else
            status = getResultForErrno();
    }
}

void FileOutputStream::closeHandle()
{
    if (fileHandle != 0)
    {
        close (getFD (fileHandle));
        fileHandle = 0;
    }
}

int FileOutputStream::writeInternal (const void* const data, const int numBytes)
{
    ssize_t result = 0;

    if (fileHandle != 0)
    {
        result = ::write (getFD (fileHandle), data, numBytes);

        if (result == -1)
            status = getResultForErrno();
    }

    return (int) result;
}

void FileOutputStream::flushInternal()
{
    if (fileHandle != 0)
        if (fsync (getFD (fileHandle)) == -1)
            status = getResultForErrno();
}

Result FileOutputStream::truncate()
{
    if (fileHandle == 0)
        return status;

    flush();
    return getResultForReturnValue (ftruncate (getFD (fileHandle), (off_t) currentPosition));
}

//==============================================================================
MemoryMappedFile::MemoryMappedFile (const File& file, MemoryMappedFile::AccessMode mode)
    : address (nullptr),
      length (0),
      fileHandle (0)
{
    jassert (mode == readOnly || mode == readWrite);

    fileHandle = open (file.getFullPathName().toUTF8(),
                       mode == readWrite ? (O_CREAT + O_RDWR) : O_RDONLY, 00644);

    if (fileHandle != -1)
    {
        const int64 fileSize = file.getSize();

        void* m = mmap (0, (size_t) fileSize,
                        mode == readWrite ? (PROT_READ | PROT_WRITE) : PROT_READ,
                        MAP_SHARED, fileHandle, 0);

        if (m != MAP_FAILED)
        {
            address = m;
            length = (size_t) fileSize;
        }
    }
}

MemoryMappedFile::~MemoryMappedFile()
{
    if (address != nullptr)
        munmap (address, length);

    if (fileHandle != 0)
        close (fileHandle);
}

//==============================================================================
File juce_getExecutableFile();
File juce_getExecutableFile()
{
   #if JUCE_ANDROID
    return File (android.appFile);
   #else
    struct DLAddrReader
    {
        static String getFilename()
        {
            Dl_info exeInfo;
            dladdr ((void*) juce_getExecutableFile, &exeInfo);
            return CharPointer_UTF8 (exeInfo.dli_fname);
        }
    };

    static String filename (DLAddrReader::getFilename());
    return File::getCurrentWorkingDirectory().getChildFile (filename);
   #endif
}

//==============================================================================
int64 File::getBytesFreeOnVolume() const
{
    struct statfs buf;
    if (juce_doStatFS (*this, buf))
        return (int64) buf.f_bsize * (int64) buf.f_bavail; // Note: this returns space available to non-super user

    return 0;
}

int64 File::getVolumeTotalSize() const
{
    struct statfs buf;
    if (juce_doStatFS (*this, buf))
        return (int64) buf.f_bsize * (int64) buf.f_blocks;

    return 0;
}

String File::getVolumeLabel() const
{
   #if JUCE_MAC
    struct VolAttrBuf
    {
        u_int32_t       length;
        attrreference_t mountPointRef;
        char            mountPointSpace [MAXPATHLEN];
    } attrBuf;

    struct attrlist attrList = { 0 };
    attrList.bitmapcount = ATTR_BIT_MAP_COUNT;
    attrList.volattr = ATTR_VOL_INFO | ATTR_VOL_NAME;

    File f (*this);

    for (;;)
    {
        if (getattrlist (f.getFullPathName().toUTF8(), &attrList, &attrBuf, sizeof (attrBuf), 0) == 0)
            return String::fromUTF8 (((const char*) &attrBuf.mountPointRef) + attrBuf.mountPointRef.attr_dataoffset,
                                     (int) attrBuf.mountPointRef.attr_length);

        const File parent (f.getParentDirectory());

        if (f == parent)
            break;

        f = parent;
    }
   #endif

    return String::empty;
}

int File::getVolumeSerialNumber() const
{
    int result = 0;
/*    int fd = open (getFullPathName().toUTF8(), O_RDONLY | O_NONBLOCK);

    char info [512];

    #ifndef HDIO_GET_IDENTITY
     #define HDIO_GET_IDENTITY 0x030d
    #endif

    if (ioctl (fd, HDIO_GET_IDENTITY, info) == 0)
    {
        DBG (String (info + 20, 20));
        result = String (info + 20, 20).trim().getIntValue();
    }

    close (fd);*/
    return result;
}

//==============================================================================
void juce_runSystemCommand (const String&);
void juce_runSystemCommand (const String& command)
{
    int result = system (command.toUTF8());
    (void) result;
}

String juce_getOutputFromCommand (const String&);
String juce_getOutputFromCommand (const String& command)
{
    // slight bodge here, as we just pipe the output into a temp file and read it...
    const File tempFile (File::getSpecialLocation (File::tempDirectory)
                           .getNonexistentChildFile (String::toHexString (Random::getSystemRandom().nextInt()), ".tmp", false));

    juce_runSystemCommand (command + " > " + tempFile.getFullPathName());

    String result (tempFile.loadFileAsString());
    tempFile.deleteFile();
    return result;
}


//==============================================================================
class InterProcessLock::Pimpl
{
public:
    Pimpl (const String& name, const int timeOutMillisecs)
        : handle (0), refCount (1)
    {
       #if JUCE_IOS
        handle = 1; // On iOS we can't run multiple apps, so just assume success.
       #else

         // Note that we can't get the normal temp folder here, as it might be different for each app.
        #if JUCE_MAC
         File tempFolder ("~/Library/Caches/com.juce.locks");
        #else
         File tempFolder ("/var/tmp");

         if (! tempFolder.isDirectory())
             tempFolder = "/tmp";
        #endif

        const File temp (tempFolder.getChildFile (name));

        temp.create();
        handle = open (temp.getFullPathName().toUTF8(), O_RDWR);

        if (handle != 0)
        {
            struct flock fl = { 0 };
            fl.l_whence = SEEK_SET;
            fl.l_type = F_WRLCK;

            const int64 endTime = Time::currentTimeMillis() + timeOutMillisecs;

            for (;;)
            {
                const int result = fcntl (handle, F_SETLK, &fl);

                if (result >= 0)
                    return;

                if (errno != EINTR)
                {
                    if (timeOutMillisecs == 0
                         || (timeOutMillisecs > 0 && Time::currentTimeMillis() >= endTime))
                        break;

                    Thread::sleep (10);
                }
            }
        }

        closeFile();
       #endif
    }

    ~Pimpl()
    {
        closeFile();
    }

    void closeFile()
    {
       #if ! JUCE_IOS
        if (handle != 0)
        {
            struct flock fl = { 0 };
            fl.l_whence = SEEK_SET;
            fl.l_type = F_UNLCK;

            while (! (fcntl (handle, F_SETLKW, &fl) >= 0 || errno != EINTR))
            {}

            close (handle);
            handle = 0;
        }
       #endif
    }

    int handle, refCount;
};

InterProcessLock::InterProcessLock (const String& name_)
    : name (name_)
{
}

InterProcessLock::~InterProcessLock()
{
}

bool InterProcessLock::enter (const int timeOutMillisecs)
{
    const ScopedLock sl (lock);

    if (pimpl == nullptr)
    {
        pimpl = new Pimpl (name, timeOutMillisecs);

        if (pimpl->handle == 0)
            pimpl = nullptr;
    }
    else
    {
        pimpl->refCount++;
    }

    return pimpl != nullptr;
}

void InterProcessLock::exit()
{
    const ScopedLock sl (lock);

    // Trying to release the lock too many times!
    jassert (pimpl != nullptr);

    if (pimpl != nullptr && --(pimpl->refCount) == 0)
        pimpl = nullptr;
}

//==============================================================================
void JUCE_API juce_threadEntryPoint (void*);

extern "C" void* threadEntryProc (void*);
extern "C" void* threadEntryProc (void* userData)
{
    JUCE_AUTORELEASEPOOL

   #if JUCE_ANDROID
    struct AndroidThreadScope
    {
        AndroidThreadScope()   { threadLocalJNIEnvHolder.attach(); }
        ~AndroidThreadScope()  { threadLocalJNIEnvHolder.detach(); }
    };

    const AndroidThreadScope androidEnv;
   #endif

    juce_threadEntryPoint (userData);
    return nullptr;
}

void Thread::launchThread()
{
    threadHandle = 0;
    pthread_t handle = 0;

    if (pthread_create (&handle, 0, threadEntryProc, this) == 0)
    {
        pthread_detach (handle);
        threadHandle = (void*) handle;
        threadId = (ThreadID) threadHandle;
    }
}

void Thread::closeThreadHandle()
{
    threadId = 0;
    threadHandle = 0;
}

void Thread::killThread()
{
    if (threadHandle != 0)
    {
       #if JUCE_ANDROID
        jassertfalse; // pthread_cancel not available!
       #else
        pthread_cancel ((pthread_t) threadHandle);
       #endif
    }
}

void Thread::setCurrentThreadName (const String& name)
{
   #if JUCE_IOS || (JUCE_MAC && defined (MAC_OS_X_VERSION_10_5) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5)
    JUCE_AUTORELEASEPOOL
    [[NSThread currentThread] setName: juceStringToNS (name)];
   #elif JUCE_LINUX
    prctl (PR_SET_NAME, name.toUTF8().getAddress(), 0, 0, 0);
   #endif
}

bool Thread::setThreadPriority (void* handle, int priority)
{
    struct sched_param param;
    int policy;
    priority = jlimit (0, 10, priority);

    if (handle == 0)
        handle = (void*) pthread_self();

    if (pthread_getschedparam ((pthread_t) handle, &policy, &param) != 0)
        return false;

    policy = priority == 0 ? SCHED_OTHER : SCHED_RR;

    const int minPriority = sched_get_priority_min (policy);
    const int maxPriority = sched_get_priority_max (policy);

    param.sched_priority = ((maxPriority - minPriority) * priority) / 10 + minPriority;
    return pthread_setschedparam ((pthread_t) handle, policy, &param) == 0;
}

Thread::ThreadID Thread::getCurrentThreadId()
{
    return (ThreadID) pthread_self();
}

void Thread::yield()
{
    sched_yield();
}

//==============================================================================
/* Remove this macro if you're having problems compiling the cpu affinity
   calls (the API for these has changed about quite a bit in various Linux
   versions, and a lot of distros seem to ship with obsolete versions)
*/
#if defined (CPU_ISSET) && ! defined (SUPPORT_AFFINITIES)
 #define SUPPORT_AFFINITIES 1
#endif

void Thread::setCurrentThreadAffinityMask (const uint32 affinityMask)
{
   #if SUPPORT_AFFINITIES
    cpu_set_t affinity;
    CPU_ZERO (&affinity);

    for (int i = 0; i < 32; ++i)
        if ((affinityMask & (1 << i)) != 0)
            CPU_SET (i, &affinity);

    /*
       N.B. If this line causes a compile error, then you've probably not got the latest
       version of glibc installed.

       If you don't want to update your copy of glibc and don't care about cpu affinities,
       then you can just disable all this stuff by setting the SUPPORT_AFFINITIES macro to 0.
    */
    sched_setaffinity (getpid(), sizeof (cpu_set_t), &affinity);
    sched_yield();

   #else
    /* affinities aren't supported because either the appropriate header files weren't found,
       or the SUPPORT_AFFINITIES macro was turned off
    */
    jassertfalse;
    (void) affinityMask;
   #endif
}

//==============================================================================
bool DynamicLibrary::open (const String& name)
{
    close();
    handle = dlopen (name.toUTF8(), RTLD_LOCAL | RTLD_NOW);
    return handle != 0;
}

void DynamicLibrary::close()
{
    if (handle != nullptr)
    {
        dlclose (handle);
        handle = nullptr;
    }
}

void* DynamicLibrary::getFunction (const String& functionName) noexcept
{
    return handle != nullptr ? dlsym (handle, functionName.toUTF8()) : nullptr;
}



//==============================================================================
class ChildProcess::ActiveProcess
{
public:
    ActiveProcess (const StringArray& arguments)
        : childPID (0), pipeHandle (0), readHandle (0)
    {
        int pipeHandles[2] = { 0 };

        if (pipe (pipeHandles) == 0)
        {
            const pid_t result = fork();

            if (result < 0)
            {
                close (pipeHandles[0]);
                close (pipeHandles[1]);
            }
            else if (result == 0)
            {
                // we're the child process..
                close (pipeHandles[0]);   // close the read handle
                dup2 (pipeHandles[1], 1); // turns the pipe into stdout
                close (pipeHandles[1]);

                Array<char*> argv;
                for (int i = 0; i < arguments.size(); ++i)
                    argv.add (arguments[i].toUTF8().getAddress());

                argv.add (nullptr);

                execvp (argv[0], argv.getRawDataPointer());
                exit (-1);
            }
            else
            {
                // we're the parent process..
                childPID = result;
                pipeHandle = pipeHandles[0];
                close (pipeHandles[1]); // close the write handle
            }
        }
    }

    ~ActiveProcess()
    {
        if (readHandle != 0)
            fclose (readHandle);

        if (pipeHandle != 0)
            close (pipeHandle);
    }

    bool isRunning() const
    {
        if (childPID != 0)
        {
            int childState;
            const int pid = waitpid (childPID, &childState, WNOHANG);
            return pid > 0 && ! (WIFEXITED (childState) || WIFSIGNALED (childState));
        }

        return false;
    }

    int read (void* const dest, const int numBytes)
    {
        jassert (dest != nullptr);

        if (readHandle == 0 && childPID != 0)
            readHandle = fdopen (pipeHandle, "r");

        if (readHandle != 0)
            return fread (dest, 1, numBytes, readHandle);

        return 0;
    }

    bool killProcess() const
    {
        return ::kill (childPID, SIGKILL) == 0;
    }

    int childPID;

private:
    int pipeHandle;
    FILE* readHandle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ActiveProcess);
};

bool ChildProcess::start (const String& command)
{
    StringArray tokens;
    tokens.addTokens (command, true);
    tokens.removeEmptyStrings (true);

    if (tokens.size() == 0)
        return false;

    activeProcess = new ActiveProcess (tokens);

    if (activeProcess->childPID == 0)
        activeProcess = nullptr;

    return activeProcess != nullptr;
}

bool ChildProcess::isRunning() const
{
    return activeProcess != nullptr && activeProcess->isRunning();
}

int ChildProcess::readProcessOutput (void* dest, int numBytes)
{
    return activeProcess != nullptr ? activeProcess->read (dest, numBytes) : 0;
}

bool ChildProcess::kill()
{
    return activeProcess == nullptr || activeProcess->killProcess();
}
