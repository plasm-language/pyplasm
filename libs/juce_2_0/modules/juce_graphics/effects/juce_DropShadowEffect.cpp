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

#if JUCE_MSVC && JUCE_DEBUG
 #pragma optimize ("t", on)
#endif

//==============================================================================
DropShadowEffect::DropShadowEffect()
  : offsetX (0),
    offsetY (0),
    radius (4),
    opacity (0.6f)
{
}

DropShadowEffect::~DropShadowEffect()
{
}

void DropShadowEffect::setShadowProperties (const float newRadius,
                                            const float newOpacity,
                                            const int newShadowOffsetX,
                                            const int newShadowOffsetY)
{
    radius = jmax (1.1f, newRadius);
    offsetX = newShadowOffsetX;
    offsetY = newShadowOffsetY;
    opacity = newOpacity;
}

void DropShadowEffect::applyEffect (Image& image, Graphics& g, float alpha)
{
    const int w = image.getWidth();
    const int h = image.getHeight();

    Image shadowImage (Image::SingleChannel, w, h, false);

    {
        const Image::BitmapData srcData (image, Image::BitmapData::readOnly);
        const Image::BitmapData destData (shadowImage, Image::BitmapData::readWrite);

        const int filter = roundToInt (63.0f / radius);
        const int radiusMinus1 = roundToInt ((radius - 1.0f) * 63.0f);

        for (int x = w; --x >= 0;)
        {
            int shadowAlpha = 0;

            const PixelARGB* src = ((const PixelARGB*) srcData.data) + x;
            uint8* shadowPix = destData.data + x;

            for (int y = h; --y >= 0;)
            {
                shadowAlpha = ((shadowAlpha * radiusMinus1 + (src->getAlpha() << 6)) * filter) >> 12;

                *shadowPix = (uint8) shadowAlpha;
                src = addBytesToPointer (src, srcData.lineStride);
                shadowPix += destData.lineStride;
            }
        }

        for (int y = h; --y >= 0;)
        {
            int shadowAlpha = 0;
            uint8* shadowPix = destData.getLinePointer (y);

            for (int x = w; --x >= 0;)
            {
                shadowAlpha = ((shadowAlpha * radiusMinus1 + (*shadowPix << 6)) * filter) >> 12;
                *shadowPix++ = (uint8) shadowAlpha;
            }
        }
    }

    g.setColour (Colours::black.withAlpha (opacity * alpha));
    g.drawImageAt (shadowImage, offsetX, offsetY, true);

    g.setOpacity (alpha);
    g.drawImageAt (image, 0, 0);
}

#if JUCE_MSVC && JUCE_DEBUG
  #pragma optimize ("", on)  // resets optimisations to the project defaults
#endif
