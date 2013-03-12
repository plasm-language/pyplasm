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

//==============================================================================
#define JNI_CLASS_MEMBERS(METHOD, STATICMETHOD, FIELD, STATICFIELD) \
 STATICMETHOD (create,          "create",           "(Ljava/lang/String;I)Landroid/graphics/Typeface;") \
 STATICMETHOD (createFromFile,  "createFromFile",   "(Ljava/lang/String;)Landroid/graphics/Typeface;") \

DECLARE_JNI_CLASS (TypefaceClass, "android/graphics/Typeface");
#undef JNI_CLASS_MEMBERS


//==============================================================================
StringArray Font::findAllTypefaceNames()
{
    StringArray results;

    Array<File> fonts;
    File ("/system/fonts").findChildFiles (fonts, File::findFiles, false, "*.ttf");

    for (int i = 0; i < fonts.size(); ++i)
        results.add (fonts.getReference(i).getFileNameWithoutExtension());

    return results;
}

struct DefaultFontNames
{
    DefaultFontNames()
        : defaultSans  ("sans"),
          defaultSerif ("serif"),
          defaultFixed ("monospace"),
          defaultFallback ("sans")
    {
    }

    String defaultSans, defaultSerif, defaultFixed, defaultFallback;
};

Typeface::Ptr Font::getDefaultTypefaceForFont (const Font& font)
{
    static DefaultFontNames defaultNames;

    String faceName (font.getTypefaceName());

    if (faceName == Font::getDefaultSansSerifFontName())       faceName = defaultNames.defaultSans;
    else if (faceName == Font::getDefaultSerifFontName())      faceName = defaultNames.defaultSerif;
    else if (faceName == Font::getDefaultMonospacedFontName()) faceName = defaultNames.defaultFixed;

    Font f (font);
    f.setTypefaceName (faceName);
    return Typeface::createSystemTypefaceFor (f);
}

//==============================================================================
class AndroidTypeface   : public Typeface
{
public:
    AndroidTypeface (const Font& font)
        : Typeface (font.getTypefaceName()),
          ascent (0),
          descent (0)
    {
        jint flags = 0;
        if (font.isBold()) flags = 1;
        if (font.isItalic()) flags += 2;

        JNIEnv* env = getEnv();

        File fontFile (File ("/system/fonts").getChildFile (name).withFileExtension (".ttf"));

        if (fontFile.exists())
            typeface = GlobalRef (env->CallStaticObjectMethod (TypefaceClass, TypefaceClass.createFromFile,
                                                               javaString (fontFile.getFullPathName()).get()));
        else
            typeface = GlobalRef (env->CallStaticObjectMethod (TypefaceClass, TypefaceClass.create,
                                                               javaString (getName()).get(), flags));

        rect = GlobalRef (env->NewObject (RectClass, RectClass.constructor, 0, 0, 0, 0));

        paint = GlobalRef (GraphicsHelpers::createPaint (Graphics::highResamplingQuality));
        const LocalRef<jobject> ignored (paint.callObjectMethod (Paint.setTypeface, typeface.get()));

        const float standardSize = 256.0f;
        paint.callVoidMethod (Paint.setTextSize, standardSize);
        ascent = std::abs (paint.callFloatMethod (Paint.ascent)) / standardSize;
        descent = paint.callFloatMethod (Paint.descent) / standardSize;

        const float height = ascent + descent;
        unitsToHeightScaleFactor = 1.0f / 256.0f;
    }

    float getAscent() const    { return ascent; }
    float getDescent() const   { return descent; }

    float getStringWidth (const String& text)
    {
        JNIEnv* env = getEnv();
        const int numChars = text.length();
        jfloatArray widths = env->NewFloatArray (numChars);

        const int numDone = paint.callIntMethod (Paint.getTextWidths, javaString (text).get(), widths);

        HeapBlock<jfloat> localWidths (numDone);
        env->GetFloatArrayRegion (widths, 0, numDone, localWidths);
        env->DeleteLocalRef (widths);

        float x = 0;
        for (int i = 0; i < numDone; ++i)
            x += localWidths[i];

        return x * unitsToHeightScaleFactor;
    }

    void getGlyphPositions (const String& text, Array<int>& glyphs, Array<float>& xOffsets)
    {
        JNIEnv* env = getEnv();
        const int numChars = text.length();
        jfloatArray widths = env->NewFloatArray (numChars);

        const int numDone = paint.callIntMethod (Paint.getTextWidths, javaString (text).get(), widths);

        HeapBlock<jfloat> localWidths (numDone);
        env->GetFloatArrayRegion (widths, 0, numDone, localWidths);
        env->DeleteLocalRef (widths);

        String::CharPointerType s (text.getCharPointer());

        xOffsets.add (0);

        float x = 0;
        for (int i = 0; i < numDone; ++i)
        {
            glyphs.add ((int) s.getAndAdvance());
            x += localWidths[i];
            xOffsets.add (x * unitsToHeightScaleFactor);
        }
    }

    bool getOutlineForGlyph (int /*glyphNumber*/, Path& /*destPath*/)
    {
        return false;
    }

    EdgeTable* getEdgeTableForGlyph (int glyphNumber, const AffineTransform& t)
    {
        JNIEnv* env = getEnv();

        jobject matrix = GraphicsHelpers::createMatrix (env, AffineTransform::scale (unitsToHeightScaleFactor, unitsToHeightScaleFactor).followedBy (t));
        jintArray maskData = (jintArray) android.activity.callObjectMethod (JuceAppActivity.renderGlyph, (jchar) glyphNumber, paint.get(), matrix, rect.get());

        env->DeleteLocalRef (matrix);

        const int left   = env->GetIntField (rect.get(), RectClass.left);
        const int top    = env->GetIntField (rect.get(), RectClass.top);
        const int right  = env->GetIntField (rect.get(), RectClass.right);
        const int bottom = env->GetIntField (rect.get(), RectClass.bottom);

        const Rectangle<int> bounds (left, top, right - left, bottom - top);

        EdgeTable* et = nullptr;

        if (! bounds.isEmpty())
        {
            et = new EdgeTable (bounds);

            jint* const maskDataElements = env->GetIntArrayElements (maskData, 0);
            const jint* mask = maskDataElements;

            for (int y = top; y < bottom; ++y)
            {
               #if JUCE_LITTLE_ENDIAN
                const uint8* const lineBytes = ((const uint8*) mask) + 3;
               #else
                const uint8* const lineBytes = (const uint8*) mask;
               #endif

                et->clipLineToMask (left, y, lineBytes, 4, bounds.getWidth());
                mask += bounds.getWidth();
            }

            env->ReleaseIntArrayElements (maskData, maskDataElements, 0);
        }

        env->DeleteLocalRef (maskData);
        return et;
    }

    GlobalRef typeface, paint, rect;
    float ascent, descent, unitsToHeightScaleFactor;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AndroidTypeface);
};

//==============================================================================
Typeface::Ptr Typeface::createSystemTypefaceFor (const Font& font)
{
    return new AndroidTypeface (font);
}

bool TextLayout::createNativeLayout (const AttributedString&)
{
    return false;
}
