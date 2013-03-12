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

#ifndef __JUCER_ELEMENTSIBLINGCOMPONENT_JUCEHEADER__
#define __JUCER_ELEMENTSIBLINGCOMPONENT_JUCEHEADER__


//==============================================================================
/**
*/
class ElementSiblingComponent  : public Component,
                                 public ChangeListener
{
public:
    //==============================================================================
    ElementSiblingComponent (PaintElement* const owner_)
        : owner (owner_)
    {
        setAlwaysOnTop (true);

        owner->getDocument()->addChangeListener (this);
    }

    ~ElementSiblingComponent()
    {
        owner->getDocument()->removeChangeListener (this);
    }

    virtual void updatePosition() = 0;


    //==============================================================================
    void changeListenerCallback (ChangeBroadcaster*)
    {
        updatePosition();
    }

protected:
    PaintElement* const owner;
};

#endif   // __JUCER_ELEMENTSIBLINGCOMPONENT_JUCEHEADER__
