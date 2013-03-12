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

AudioProcessor::AudioProcessor()
    : playHead (nullptr),
      sampleRate (0),
      blockSize (0),
      numInputChannels (0),
      numOutputChannels (0),
      latencySamples (0),
      suspended (false),
      nonRealtime (false)
{
}

AudioProcessor::~AudioProcessor()
{
    // ooh, nasty - the editor should have been deleted before the filter
    // that it refers to is deleted..
    jassert (activeEditor == nullptr);

   #if JUCE_DEBUG
    // This will fail if you've called beginParameterChangeGesture() for one
    // or more parameters without having made a corresponding call to endParameterChangeGesture...
    jassert (changingParams.countNumberOfSetBits() == 0);
   #endif
}

void AudioProcessor::setPlayHead (AudioPlayHead* const newPlayHead) noexcept
{
    playHead = newPlayHead;
}

void AudioProcessor::addListener (AudioProcessorListener* const newListener)
{
    const ScopedLock sl (listenerLock);
    listeners.addIfNotAlreadyThere (newListener);
}

void AudioProcessor::removeListener (AudioProcessorListener* const listenerToRemove)
{
    const ScopedLock sl (listenerLock);
    listeners.removeValue (listenerToRemove);
}

void AudioProcessor::setPlayConfigDetails (const int numIns,
                                           const int numOuts,
                                           const double sampleRate_,
                                           const int blockSize_) noexcept
{
    numInputChannels = numIns;
    numOutputChannels = numOuts;
    sampleRate = sampleRate_;
    blockSize = blockSize_;
}

void AudioProcessor::setNonRealtime (const bool nonRealtime_) noexcept
{
    nonRealtime = nonRealtime_;
}

void AudioProcessor::setLatencySamples (const int newLatency)
{
    if (latencySamples != newLatency)
    {
        latencySamples = newLatency;
        updateHostDisplay();
    }
}

void AudioProcessor::setParameterNotifyingHost (const int parameterIndex,
                                                const float newValue)
{
    setParameter (parameterIndex, newValue);
    sendParamChangeMessageToListeners (parameterIndex, newValue);
}

void AudioProcessor::sendParamChangeMessageToListeners (const int parameterIndex, const float newValue)
{
    jassert (isPositiveAndBelow (parameterIndex, getNumParameters()));

    for (int i = listeners.size(); --i >= 0;)
    {
        AudioProcessorListener* l;

        {
            const ScopedLock sl (listenerLock);
            l = listeners [i];
        }

        if (l != nullptr)
            l->audioProcessorParameterChanged (this, parameterIndex, newValue);
    }
}

void AudioProcessor::beginParameterChangeGesture (int parameterIndex)
{
    jassert (isPositiveAndBelow (parameterIndex, getNumParameters()));

   #if JUCE_DEBUG
    // This means you've called beginParameterChangeGesture twice in succession without a matching
    // call to endParameterChangeGesture. That might be fine in most hosts, but better to avoid doing it.
    jassert (! changingParams [parameterIndex]);
    changingParams.setBit (parameterIndex);
   #endif

    for (int i = listeners.size(); --i >= 0;)
    {
        AudioProcessorListener* l;

        {
            const ScopedLock sl (listenerLock);
            l = listeners [i];
        }

        if (l != nullptr)
            l->audioProcessorParameterChangeGestureBegin (this, parameterIndex);
    }
}

void AudioProcessor::endParameterChangeGesture (int parameterIndex)
{
    jassert (isPositiveAndBelow (parameterIndex, getNumParameters()));

   #if JUCE_DEBUG
    // This means you've called endParameterChangeGesture without having previously called
    // endParameterChangeGesture. That might be fine in most hosts, but better to keep the
    // calls matched correctly.
    jassert (changingParams [parameterIndex]);
    changingParams.clearBit (parameterIndex);
   #endif

    for (int i = listeners.size(); --i >= 0;)
    {
        AudioProcessorListener* l;

        {
            const ScopedLock sl (listenerLock);
            l = listeners [i];
        }

        if (l != nullptr)
            l->audioProcessorParameterChangeGestureEnd (this, parameterIndex);
    }
}

void AudioProcessor::updateHostDisplay()
{
    for (int i = listeners.size(); --i >= 0;)
    {
        AudioProcessorListener* l;

        {
            const ScopedLock sl (listenerLock);
            l = listeners [i];
        }

        if (l != nullptr)
            l->audioProcessorChanged (this);
    }
}

bool AudioProcessor::isParameterAutomatable (int /*parameterIndex*/) const
{
    return true;
}

bool AudioProcessor::isMetaParameter (int /*parameterIndex*/) const
{
    return false;
}

void AudioProcessor::suspendProcessing (const bool shouldBeSuspended)
{
    const ScopedLock sl (callbackLock);
    suspended = shouldBeSuspended;
}

void AudioProcessor::reset()
{
}

//==============================================================================
void AudioProcessor::editorBeingDeleted (AudioProcessorEditor* const editor) noexcept
{
    const ScopedLock sl (callbackLock);

    if (activeEditor == editor)
        activeEditor = nullptr;
}

AudioProcessorEditor* AudioProcessor::createEditorIfNeeded()
{
    if (activeEditor != nullptr)
        return activeEditor;

    AudioProcessorEditor* const ed = createEditor();

    // You must make your hasEditor() method return a consistent result!
    jassert (hasEditor() == (ed != nullptr));

    if (ed != nullptr)
    {
        // you must give your editor comp a size before returning it..
        jassert (ed->getWidth() > 0 && ed->getHeight() > 0);

        const ScopedLock sl (callbackLock);
        activeEditor = ed;
    }

    return ed;
}

//==============================================================================
void AudioProcessor::getCurrentProgramStateInformation (juce::MemoryBlock& destData)
{
    getStateInformation (destData);
}

void AudioProcessor::setCurrentProgramStateInformation (const void* data, int sizeInBytes)
{
    setStateInformation (data, sizeInBytes);
}

//==============================================================================
// magic number to identify memory blocks that we've stored as XML
const uint32 magicXmlNumber = 0x21324356;

void AudioProcessor::copyXmlToBinary (const XmlElement& xml, juce::MemoryBlock& destData)
{
    const String xmlString (xml.createDocument (String::empty, true, false));
    const int stringLength = xmlString.getNumBytesAsUTF8();

    destData.setSize ((size_t) stringLength + 10);

    char* const d = static_cast<char*> (destData.getData());
    *(uint32*) d = ByteOrder::swapIfBigEndian ((const uint32) magicXmlNumber);
    *(uint32*) (d + 4) = ByteOrder::swapIfBigEndian ((const uint32) stringLength);

    xmlString.copyToUTF8 (d + 8, stringLength + 1);
}

XmlElement* AudioProcessor::getXmlFromBinary (const void* data,
                                              const int sizeInBytes)
{
    if (sizeInBytes > 8
         && ByteOrder::littleEndianInt (data) == magicXmlNumber)
    {
        const int stringLength = (int) ByteOrder::littleEndianInt (addBytesToPointer (data, 4));

        if (stringLength > 0)
            return XmlDocument::parse (String::fromUTF8 (static_cast<const char*> (data) + 8,
                                                         jmin ((sizeInBytes - 8), stringLength)));
    }

    return nullptr;
}

//==============================================================================
void AudioProcessorListener::audioProcessorParameterChangeGestureBegin (AudioProcessor*, int) {}
void AudioProcessorListener::audioProcessorParameterChangeGestureEnd (AudioProcessor*, int) {}

//==============================================================================
bool AudioPlayHead::CurrentPositionInfo::operator== (const CurrentPositionInfo& other) const noexcept
{
    return timeInSeconds == other.timeInSeconds
        && ppqPosition == other.ppqPosition
        && editOriginTime == other.editOriginTime
        && ppqPositionOfLastBarStart == other.ppqPositionOfLastBarStart
        && frameRate == other.frameRate
        && isPlaying == other.isPlaying
        && isRecording == other.isRecording
        && bpm == other.bpm
        && timeSigNumerator == other.timeSigNumerator
        && timeSigDenominator == other.timeSigDenominator;
}

bool AudioPlayHead::CurrentPositionInfo::operator!= (const CurrentPositionInfo& other) const noexcept
{
    return ! operator== (other);
}

void AudioPlayHead::CurrentPositionInfo::resetToDefault()
{
    zerostruct (*this);
    timeSigNumerator = 4;
    timeSigDenominator = 4;
    bpm = 120;
}
