/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-9 by Raw Material Software Ltd.

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

#include "../jucedemo_headers.h"


//==============================================================================
class FontsAndTextDemo  : public Component,
                          public ListBoxModel,
                          public ButtonListener,
                          public SliderListener
{
public:
    //==============================================================================
    FontsAndTextDemo()
        : boldButton ("Bold"),
          italicButton ("Italic"),
          sizeLabel (String::empty, "Size"),
          kerningLabel (String::empty, "Kerning"),
          horizontalScaleLabel (String::empty, "Scale")
    {
        setName ("Fonts");

        Font::findFonts (fonts);

        addAndMakeVisible (listBox = new ListBox ("fonts", this));
        listBox->setRowHeight (28);

        addAndMakeVisible (&textBox);

        textBox.setColour (TextEditor::backgroundColourId, Colours::white);
        textBox.setColour (TextEditor::outlineColourId, Colours::black.withAlpha (0.5f));

        textBox.setMultiLine (true, true);
        textBox.setReturnKeyStartsNewLine (true);
        textBox.setText ("The Quick Brown Fox Jumps Over The Lazy Dog\n\nAa Bb Cc Dd Ee Ff Gg Hh Ii Jj Kk Ll Mm Nn Oo Pp Qq Rr Ss Tt Uu Vv Ww Xx Yy Zz 0123456789");

        addAndMakeVisible (&boldButton);
        boldButton.addListener (this);

        addAndMakeVisible (&italicButton);
        italicButton.addListener (this);

        addAndMakeVisible (&sizeSlider);
        sizeSlider.setRange (3.0, 150.0, 0.1);
        sizeSlider.setValue (20.0);
        sizeSlider.addListener (this);
        sizeLabel.attachToComponent (&sizeSlider, true);

        addAndMakeVisible (&kerningSlider);
        kerningSlider.setRange (-1.0, 1.0, 0.01);
        kerningSlider.setValue (0.0);
        kerningSlider.addListener (this);
        kerningLabel.attachToComponent (&kerningSlider, true);

        addAndMakeVisible (&horizontalScaleSlider);
        horizontalScaleSlider.setRange (0.1, 4.0, 0.01);
        horizontalScaleSlider.setValue (1.0);
        horizontalScaleSlider.addListener (this);
        horizontalScaleLabel.attachToComponent (&horizontalScaleSlider, true);

        listBox->setColour (ListBox::outlineColourId, Colours::black.withAlpha (0.5f));
        listBox->setOutlineThickness (1);
        listBox->selectRow (Random::getSystemRandom().nextInt (fonts.size()));

        // set up the layout and resizer bars..

        verticalLayout.setItemLayout (0, -0.2, -0.8, -0.5);  // width of the font list must be
                                                             // between 20% and 80%, preferably 50%
        verticalLayout.setItemLayout (1, 8, 8, 8);           // the vertical divider drag-bar thing is always 8 pixels wide
        verticalLayout.setItemLayout (2, 150, -1.0, -0.5);   // the components on the right must be
                                                             // at least 150 pixels wide, preferably 50% of the total width

        verticalDividerBar = new StretchableLayoutResizerBar (&verticalLayout, 1, true);
        addAndMakeVisible (verticalDividerBar);
    }

    ~FontsAndTextDemo()
    {
    }

    void resized()
    {
        // lay out the list box and vertical divider..
        Component* vcomps[] = { listBox, verticalDividerBar, 0 };

        verticalLayout.layOutComponents (vcomps, 3,
                                         4, 4, getWidth() - 8, getHeight() - 8,
                                         false,     // lay out side-by-side
                                         true);     // resize the components' heights as well as widths

        // now lay out the text box and the controls below it..
        int x = verticalLayout.getItemCurrentPosition (2) + 4;
        textBox.setBounds (x, 0, getWidth() - x, getHeight() - 110);
        x += 70;
        sizeSlider.setBounds (x, getHeight() - 106, getWidth() - x, 22);
        kerningSlider.setBounds (x, getHeight() - 82, getWidth() - x, 22);
        horizontalScaleSlider.setBounds (x, getHeight() - 58, getWidth() - x, 22);
        boldButton.setBounds (x, getHeight() - 34, (getWidth() - x) / 2, 22);
        italicButton.setBounds (x + (getWidth() - x) / 2, getHeight() - 34, (getWidth() - x) / 2, 22);
    }

    // implements the ListBoxModel method
    int getNumRows()
    {
        return fonts.size();
    }

    // implements the ListBoxModel method
    void paintListBoxItem (int rowNumber,
                           Graphics& g,
                           int width, int height,
                           bool rowIsSelected)
    {
        if (rowIsSelected)
            g.fillAll (Colours::lightblue);

        Font font (fonts [rowNumber]);
        font.setHeight (height * 0.7f);

        g.setFont (font);
        g.setColour (Colours::black);
        g.drawText (font.getTypefaceName(),
                    4, 0, width - 4, height,
                    Justification::centredLeft, true);

        int x = jmax (0, font.getStringWidth (font.getTypefaceName())) + 12;
        g.setFont (Font (11.0f, Font::italic));
        g.setColour (Colours::grey);
        g.drawText (font.getTypefaceName(),
                    x, 0, width - x - 2, height,
                    Justification::centredLeft, true);
    }

    void updatePreviewBoxText()
    {
        Font font (fonts [listBox->getSelectedRow()]);

        font.setHeight ((float) sizeSlider.getValue());
        font.setBold (boldButton.getToggleState());
        font.setItalic (italicButton.getToggleState());
        font.setExtraKerningFactor ((float) kerningSlider.getValue());
        font.setHorizontalScale ((float) horizontalScaleSlider.getValue());

        textBox.applyFontToAllText (font);
    }

    void selectedRowsChanged (int /*lastRowselected*/)
    {
        updatePreviewBoxText();
    }

    void buttonClicked (Button*)
    {
        updatePreviewBoxText();
    }

    void sliderValueChanged (Slider*)
    {
        // (this is called when the size slider is moved)
        updatePreviewBoxText();
    }

private:
    Array<Font> fonts;

    ScopedPointer<ListBox> listBox;
    TextEditor textBox;
    ToggleButton boldButton, italicButton;
    Slider sizeSlider, kerningSlider, horizontalScaleSlider;
    Label sizeLabel, kerningLabel, horizontalScaleLabel;

    StretchableLayoutManager verticalLayout;
    ScopedPointer<StretchableLayoutResizerBar> verticalDividerBar;
};


//==============================================================================
Component* createFontsAndTextDemo()
{
    return new FontsAndTextDemo();
}
