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

namespace CoreMidiHelpers
{
    static bool logError (const OSStatus err, const int lineNum)
    {
        if (err == noErr)
            return true;

        Logger::writeToLog ("CoreMidi error: " + String (lineNum) + " - " + String::toHexString ((int) err));
        jassertfalse;
        return false;
    }

    #undef CHECK_ERROR
    #define CHECK_ERROR(a) CoreMidiHelpers::logError (a, __LINE__)

    //==============================================================================
    static String getEndpointName (MIDIEndpointRef endpoint, bool isExternal)
    {
        String result;
        CFStringRef str = 0;

        MIDIObjectGetStringProperty (endpoint, kMIDIPropertyName, &str);

        if (str != 0)
        {
            result = String::fromCFString (str);
            CFRelease (str);
            str = 0;
        }

        MIDIEntityRef entity = 0;
        MIDIEndpointGetEntity (endpoint, &entity);

        if (entity == 0)
            return result; // probably virtual

        if (result.isEmpty())
        {
            // endpoint name has zero length - try the entity
            MIDIObjectGetStringProperty (entity, kMIDIPropertyName, &str);

            if (str != 0)
            {
                result += String::fromCFString (str);
                CFRelease (str);
                str = 0;
            }
        }

        // now consider the device's name
        MIDIDeviceRef device = 0;
        MIDIEntityGetDevice (entity, &device);
        if (device == 0)
            return result;

        MIDIObjectGetStringProperty (device, kMIDIPropertyName, &str);

        if (str != 0)
        {
            const String s (String::fromCFString (str));
            CFRelease (str);

            // if an external device has only one entity, throw away
            // the endpoint name and just use the device name
            if (isExternal && MIDIDeviceGetNumberOfEntities (device) < 2)
            {
                result = s;
            }
            else if (! result.startsWithIgnoreCase (s))
            {
                // prepend the device name to the entity name
                result = (s + " " + result).trimEnd();
            }
        }

        return result;
    }

    static String getConnectedEndpointName (MIDIEndpointRef endpoint)
    {
        String result;

        // Does the endpoint have connections?
        CFDataRef connections = 0;
        int numConnections = 0;

        MIDIObjectGetDataProperty (endpoint, kMIDIPropertyConnectionUniqueID, &connections);

        if (connections != 0)
        {
            numConnections = ((int) CFDataGetLength (connections)) / (int) sizeof (MIDIUniqueID);

            if (numConnections > 0)
            {
                const SInt32* pid = reinterpret_cast <const SInt32*> (CFDataGetBytePtr (connections));

                for (int i = 0; i < numConnections; ++i, ++pid)
                {
                    MIDIUniqueID uid = (MIDIUniqueID) ByteOrder::swapIfLittleEndian ((uint32) *pid);
                    MIDIObjectRef connObject;
                    MIDIObjectType connObjectType;
                    OSStatus err = MIDIObjectFindByUniqueID (uid, &connObject, &connObjectType);

                    if (err == noErr)
                    {
                        String s;

                        if (connObjectType == kMIDIObjectType_ExternalSource
                             || connObjectType == kMIDIObjectType_ExternalDestination)
                        {
                            // Connected to an external device's endpoint (10.3 and later).
                            s = getEndpointName (static_cast <MIDIEndpointRef> (connObject), true);
                        }
                        else
                        {
                            // Connected to an external device (10.2) (or something else, catch-all)
                            CFStringRef str = 0;
                            MIDIObjectGetStringProperty (connObject, kMIDIPropertyName, &str);

                            if (str != 0)
                            {
                                s = String::fromCFString (str);
                                CFRelease (str);
                            }
                        }

                        if (s.isNotEmpty())
                        {
                            if (result.isNotEmpty())
                                result += ", ";

                            result += s;
                        }
                    }
                }
            }

            CFRelease (connections);
        }

        if (result.isNotEmpty())
            return result;

        // Here, either the endpoint had no connections, or we failed to obtain names for any of them.
        return getEndpointName (endpoint, false);
    }

    static MIDIClientRef getGlobalMidiClient()
    {
        static MIDIClientRef globalMidiClient = 0;

        if (globalMidiClient == 0)
        {
            String name ("JUCE");

            if (JUCEApplicationBase::getInstance() != nullptr)
                name = JUCEApplicationBase::getInstance()->getApplicationName();

            CFStringRef appName = name.toCFString();
            CHECK_ERROR (MIDIClientCreate (appName, 0, 0, &globalMidiClient));
            CFRelease (appName);
        }

        return globalMidiClient;
    }

    //==============================================================================
    class MidiPortAndEndpoint
    {
    public:
        MidiPortAndEndpoint (MIDIPortRef port_, MIDIEndpointRef endPoint_)
            : port (port_), endPoint (endPoint_)
        {
        }

        ~MidiPortAndEndpoint()
        {
            if (port != 0)
                MIDIPortDispose (port);

            if (port == 0 && endPoint != 0) // if port == 0, it means we created the endpoint, so it's safe to delete it
                MIDIEndpointDispose (endPoint);
        }

        void send (const MIDIPacketList* const packets)
        {
            if (port != 0)
                MIDISend (port, endPoint, packets);
            else
                MIDIReceived (endPoint, packets);
        }

        MIDIPortRef port;
        MIDIEndpointRef endPoint;
    };

    //==============================================================================
    class MidiPortAndCallback;
    CriticalSection callbackLock;
    Array<MidiPortAndCallback*> activeCallbacks;

    class MidiPortAndCallback
    {
    public:
        MidiPortAndCallback (MidiInputCallback& callback_)
            : input (nullptr), active (false), callback (callback_), concatenator (2048)
        {
        }

        ~MidiPortAndCallback()
        {
            active = false;

            {
                const ScopedLock sl (callbackLock);
                activeCallbacks.removeValue (this);
            }

            if (portAndEndpoint != nullptr && portAndEndpoint->port != 0)
                CHECK_ERROR (MIDIPortDisconnectSource (portAndEndpoint->port, portAndEndpoint->endPoint));
        }

        void handlePackets (const MIDIPacketList* const pktlist)
        {
            const double time = Time::getMillisecondCounterHiRes() * 0.001;

            const ScopedLock sl (callbackLock);
            if (activeCallbacks.contains (this) && active)
            {
                const MIDIPacket* packet = &pktlist->packet[0];

                for (unsigned int i = 0; i < pktlist->numPackets; ++i)
                {
                    concatenator.pushMidiData (packet->data, (int) packet->length, time,
                                               input, callback);

                    packet = MIDIPacketNext (packet);
                }
            }
        }

        MidiInput* input;
        ScopedPointer<MidiPortAndEndpoint> portAndEndpoint;
        volatile bool active;

    private:
        MidiInputCallback& callback;
        MidiDataConcatenator concatenator;
    };

    static void midiInputProc (const MIDIPacketList* pktlist, void* readProcRefCon, void* /*srcConnRefCon*/)
    {
        static_cast <MidiPortAndCallback*> (readProcRefCon)->handlePackets (pktlist);
    }
}

//==============================================================================
StringArray MidiOutput::getDevices()
{
    StringArray s;

    const ItemCount num = MIDIGetNumberOfDestinations();
    for (ItemCount i = 0; i < num; ++i)
    {
        MIDIEndpointRef dest = MIDIGetDestination (i);
        String name;

        if (dest != 0)
            name = CoreMidiHelpers::getConnectedEndpointName (dest);

        if (name.isEmpty())
            name = "<error>";

        s.add (name);
    }

    return s;
}

int MidiOutput::getDefaultDeviceIndex()
{
    return 0;
}

MidiOutput* MidiOutput::openDevice (int index)
{
    MidiOutput* mo = nullptr;

    if (isPositiveAndBelow (index, (int) MIDIGetNumberOfDestinations()))
    {
        MIDIEndpointRef endPoint = MIDIGetDestination ((ItemCount) index);

        CFStringRef pname;
        if (CHECK_ERROR (MIDIObjectGetStringProperty (endPoint, kMIDIPropertyName, &pname)))
        {
            MIDIClientRef client = CoreMidiHelpers::getGlobalMidiClient();
            MIDIPortRef port;

            if (client != 0 && CHECK_ERROR (MIDIOutputPortCreate (client, pname, &port)))
            {
                mo = new MidiOutput();
                mo->internal = new CoreMidiHelpers::MidiPortAndEndpoint (port, endPoint);
            }

            CFRelease (pname);
        }
    }

    return mo;
}

MidiOutput* MidiOutput::createNewDevice (const String& deviceName)
{
    MidiOutput* mo = nullptr;
    MIDIClientRef client = CoreMidiHelpers::getGlobalMidiClient();

    MIDIEndpointRef endPoint;
    CFStringRef name = deviceName.toCFString();

    if (client != 0 && CHECK_ERROR (MIDISourceCreate (client, name, &endPoint)))
    {
        mo = new MidiOutput();
        mo->internal = new CoreMidiHelpers::MidiPortAndEndpoint (0, endPoint);
    }

    CFRelease (name);
    return mo;
}

MidiOutput::~MidiOutput()
{
    delete static_cast<CoreMidiHelpers::MidiPortAndEndpoint*> (internal);
}

void MidiOutput::sendMessageNow (const MidiMessage& message)
{
    CoreMidiHelpers::MidiPortAndEndpoint* const mpe = static_cast<CoreMidiHelpers::MidiPortAndEndpoint*> (internal);

   #if JUCE_IOS
    const MIDITimeStamp timeStamp = mach_absolute_time();
   #else
    const MIDITimeStamp timeStamp = AudioGetCurrentHostTime();
   #endif

    if (message.isSysEx())
    {
        const int maxPacketSize = 256;
        int pos = 0, bytesLeft = message.getRawDataSize();
        const int numPackets = (bytesLeft + maxPacketSize - 1) / maxPacketSize;
        HeapBlock <MIDIPacketList> packets;
        packets.malloc ((size_t) (32 * numPackets + message.getRawDataSize()), 1);
        packets->numPackets = (UInt32) numPackets;

        MIDIPacket* p = packets->packet;

        for (int i = 0; i < numPackets; ++i)
        {
            p->timeStamp = timeStamp;
            p->length = (UInt16) jmin (maxPacketSize, bytesLeft);
            memcpy (p->data, message.getRawData() + pos, p->length);
            pos += p->length;
            bytesLeft -= p->length;
            p = MIDIPacketNext (p);
        }

        mpe->send (packets);
    }
    else
    {
        MIDIPacketList packets;
        packets.numPackets = 1;
        packets.packet[0].timeStamp = timeStamp;
        packets.packet[0].length = (UInt16) message.getRawDataSize();
        *(int*) (packets.packet[0].data) = *(const int*) message.getRawData();

        mpe->send (&packets);
    }
}

//==============================================================================
StringArray MidiInput::getDevices()
{
    StringArray s;

    const ItemCount num = MIDIGetNumberOfSources();
    for (ItemCount i = 0; i < num; ++i)
    {
        MIDIEndpointRef source = MIDIGetSource (i);
        String name;

        if (source != 0)
            name = CoreMidiHelpers::getConnectedEndpointName (source);

        if (name.isEmpty())
            name = "<error>";

        s.add (name);
    }

    return s;
}

int MidiInput::getDefaultDeviceIndex()
{
    return 0;
}

MidiInput* MidiInput::openDevice (int index, MidiInputCallback* callback)
{
    jassert (callback != 0);

    using namespace CoreMidiHelpers;
    MidiInput* newInput = nullptr;

    if (isPositiveAndBelow (index, (int) MIDIGetNumberOfSources()))
    {
        MIDIEndpointRef endPoint = MIDIGetSource ((ItemCount) index);

        if (endPoint != 0)
        {
            CFStringRef name;

            if (CHECK_ERROR (MIDIObjectGetStringProperty (endPoint, kMIDIPropertyName, &name)))
            {
                MIDIClientRef client = getGlobalMidiClient();

                if (client != 0)
                {
                    MIDIPortRef port;
                    ScopedPointer <MidiPortAndCallback> mpc (new MidiPortAndCallback (*callback));

                    if (CHECK_ERROR (MIDIInputPortCreate (client, name, midiInputProc, mpc, &port)))
                    {
                        if (CHECK_ERROR (MIDIPortConnectSource (port, endPoint, 0)))
                        {
                            mpc->portAndEndpoint = new MidiPortAndEndpoint (port, endPoint);

                            newInput = new MidiInput (getDevices() [index]);
                            mpc->input = newInput;
                            newInput->internal = mpc;

                            const ScopedLock sl (callbackLock);
                            activeCallbacks.add (mpc.release());
                        }
                        else
                        {
                            CHECK_ERROR (MIDIPortDispose (port));
                        }
                    }
                }
            }

            CFRelease (name);
        }
    }

    return newInput;
}

MidiInput* MidiInput::createNewDevice (const String& deviceName, MidiInputCallback* callback)
{
    jassert (callback != nullptr);

    using namespace CoreMidiHelpers;
    MidiInput* mi = nullptr;
    MIDIClientRef client = getGlobalMidiClient();

    if (client != 0)
    {
        ScopedPointer <MidiPortAndCallback> mpc (new MidiPortAndCallback (*callback));
        mpc->active = false;

        MIDIEndpointRef endPoint;
        CFStringRef name = deviceName.toCFString();

        if (CHECK_ERROR (MIDIDestinationCreate (client, name, midiInputProc, mpc, &endPoint)))
        {
            mpc->portAndEndpoint = new MidiPortAndEndpoint (0, endPoint);

            mi = new MidiInput (deviceName);
            mpc->input = mi;
            mi->internal = mpc;

            const ScopedLock sl (callbackLock);
            activeCallbacks.add (mpc.release());
        }

        CFRelease (name);
    }

    return mi;
}

MidiInput::MidiInput (const String& name_)
    : name (name_)
{
}

MidiInput::~MidiInput()
{
    delete static_cast<CoreMidiHelpers::MidiPortAndCallback*> (internal);
}

void MidiInput::start()
{
    const ScopedLock sl (CoreMidiHelpers::callbackLock);
    static_cast<CoreMidiHelpers::MidiPortAndCallback*> (internal)->active = true;
}

void MidiInput::stop()
{
    const ScopedLock sl (CoreMidiHelpers::callbackLock);
    static_cast<CoreMidiHelpers::MidiPortAndCallback*> (internal)->active = false;
}

#undef CHECK_ERROR
