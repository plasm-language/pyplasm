/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-10 by Raw Material Software Ltd.

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

package com.juce;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.*;
import android.graphics.*;
import android.opengl.*;
import android.text.ClipboardManager;
import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.net.HttpURLConnection;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

//==============================================================================
public final class JuceAppActivity   extends Activity
{
    //==============================================================================
    static
    {
        System.loadLibrary ("juce_jni");
    }

    @Override
    public final void onCreate (Bundle savedInstanceState)
    {
        super.onCreate (savedInstanceState);

        viewHolder = new ViewHolder (this);
        setContentView (viewHolder);
    }

    @Override
    protected final void onDestroy()
    {
        quitApp();
        super.onDestroy();
    }

    private void callAppLauncher()
    {
        launchApp (getApplicationInfo().publicSourceDir,
                   getApplicationInfo().dataDir);
    }

    //==============================================================================
    private native void launchApp (String appFile, String appDataDir);
    private native void quitApp();
    private native void setScreenSize (int screenWidth, int screenHeight);

    //==============================================================================
    public native void deliverMessage (long value);
    private android.os.Handler messageHandler = new android.os.Handler();

    public final void postMessage (long value)
    {
        messageHandler.post (new MessageCallback (value));
    }

    private final class MessageCallback  implements Runnable
    {
        public MessageCallback (long value_)        { value = value_; }
        public final void run()                     { deliverMessage (value); }

        private long value;
    }

    //==============================================================================
    private ViewHolder viewHolder;

    public final ComponentPeerView createNewView (boolean opaque)
    {
        ComponentPeerView v = new ComponentPeerView (this, opaque);
        viewHolder.addView (v);
        return v;
    }

    public final void deleteView (ComponentPeerView view)
    {
        ViewGroup group = (ViewGroup) (view.getParent());

        if (group != null)
            group.removeView (view);
    }

    final class ViewHolder  extends ViewGroup
    {
        public ViewHolder (Context context)
        {
            super (context);
            setDescendantFocusability (ViewGroup.FOCUS_AFTER_DESCENDANTS);
            setFocusable (false);
        }

        protected final void onLayout (boolean changed, int left, int top, int right, int bottom)
        {
            setScreenSize (getWidth(), getHeight());

            if (isFirstResize)
            {
                isFirstResize = false;
                callAppLauncher();
            }
        }

        private boolean isFirstResize = true;
    }

    public final void excludeClipRegion (android.graphics.Canvas canvas, float left, float top, float right, float bottom)
    {
        canvas.clipRect (left, top, right, bottom, android.graphics.Region.Op.DIFFERENCE);
    }

    //==============================================================================
    public final String getClipboardContent()
    {
        ClipboardManager clipboard = (ClipboardManager) getSystemService (CLIPBOARD_SERVICE);
        return clipboard.getText().toString();
    }

    public final void setClipboardContent (String newText)
    {
        ClipboardManager clipboard = (ClipboardManager) getSystemService (CLIPBOARD_SERVICE);
        clipboard.setText (newText);
    }

    //==============================================================================
    public final void showMessageBox (String title, String message, final long callback)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder (this);
        builder.setTitle (title)
               .setMessage (message)
               .setCancelable (true)
               .setPositiveButton ("OK", new DialogInterface.OnClickListener()
                    {
                        public void onClick (DialogInterface dialog, int id)
                        {
                            dialog.cancel();
                            JuceAppActivity.this.alertDismissed (callback, 0);
                        }
                    });

        builder.create().show();
    }

    public final void showOkCancelBox (String title, String message, final long callback)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder (this);
        builder.setTitle (title)
               .setMessage (message)
               .setCancelable (true)
               .setPositiveButton ("OK", new DialogInterface.OnClickListener()
                    {
                        public void onClick (DialogInterface dialog, int id)
                        {
                            dialog.cancel();
                            JuceAppActivity.this.alertDismissed (callback, 1);
                        }
                    })
               .setNegativeButton ("Cancel", new DialogInterface.OnClickListener()
                    {
                        public void onClick (DialogInterface dialog, int id)
                        {
                            dialog.cancel();
                            JuceAppActivity.this.alertDismissed (callback, 0);
                        }
                    });

        builder.create().show();
    }

    public final void showYesNoCancelBox (String title, String message, final long callback)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder (this);
        builder.setTitle (title)
               .setMessage (message)
               .setCancelable (true)
               .setPositiveButton ("Yes", new DialogInterface.OnClickListener()
                    {
                        public void onClick (DialogInterface dialog, int id)
                        {
                            dialog.cancel();
                            JuceAppActivity.this.alertDismissed (callback, 1);
                        }
                    })
               .setNegativeButton ("No", new DialogInterface.OnClickListener()
                    {
                        public void onClick (DialogInterface dialog, int id)
                        {
                            dialog.cancel();
                            JuceAppActivity.this.alertDismissed (callback, 2);
                        }
                    })
               .setNeutralButton ("Cancel", new DialogInterface.OnClickListener()
                    {
                        public void onClick (DialogInterface dialog, int id)
                        {
                            dialog.cancel();
                            JuceAppActivity.this.alertDismissed (callback, 0);
                        }
                    });

        builder.create().show();
    }

    public native void alertDismissed (long callback, int id);

    //==============================================================================
    public final class ComponentPeerView extends ViewGroup
                                         implements View.OnFocusChangeListener
    {
        public ComponentPeerView (Context context, boolean opaque_)
        {
            super (context);
            setWillNotDraw (false);
            opaque = opaque_;

            setFocusable (true);
            setFocusableInTouchMode (true);
            setOnFocusChangeListener (this);
            requestFocus();
        }

        //==============================================================================
        private native void handlePaint (Canvas canvas);

        @Override
        public void draw (Canvas canvas)
        {
            super.draw (canvas);
            handlePaint (canvas);
        }

        @Override
        public boolean isOpaque()
        {
            return opaque;
        }

        private boolean opaque;

        //==============================================================================
        private native void handleMouseDown (float x, float y, long time);
        private native void handleMouseDrag (float x, float y, long time);
        private native void handleMouseUp (float x, float y, long time);

        @Override
        public boolean onTouchEvent (MotionEvent event)
        {
            switch (event.getAction())
            {
                case MotionEvent.ACTION_DOWN:  handleMouseDown (event.getX(), event.getY(), event.getEventTime()); return true;
                case MotionEvent.ACTION_MOVE:  handleMouseDrag (event.getX(), event.getY(), event.getEventTime()); return true;
                case MotionEvent.ACTION_CANCEL:
                case MotionEvent.ACTION_UP:    handleMouseUp (event.getX(), event.getY(), event.getEventTime()); return true;
                default: break;
            }

            return false;
        }

        //==============================================================================
        @Override
        protected void onSizeChanged (int w, int h, int oldw, int oldh)
        {
            super.onSizeChanged (w, h, oldw, oldh);
            viewSizeChanged();
        }

        @Override
        protected void onLayout (boolean changed, int left, int top, int right, int bottom)
        {
            for (int i = getChildCount(); --i >= 0;)
                requestTransparentRegion (getChildAt (i));
        }

        private native void viewSizeChanged();

        @Override
        public void onFocusChange (View v, boolean hasFocus)
        {
            if (v == this)
                focusChanged (hasFocus);
        }

        private native void focusChanged (boolean hasFocus);

        public void setViewName (String newName)    {}

        public boolean isVisible()                  { return getVisibility() == VISIBLE; }
        public void setVisible (boolean b)          { setVisibility (b ? VISIBLE : INVISIBLE); }

        public boolean containsPoint (int x, int y)
        {
            return true; //xxx needs to check overlapping views
        }

        public OpenGLView createGLView()
        {
            OpenGLView glView = new OpenGLView (getContext());
            addView (glView);
            return glView;
        }
    }

    //==============================================================================
    public final class OpenGLView   extends GLSurfaceView
                                    implements GLSurfaceView.Renderer
    {
        OpenGLView (Context context)
        {
            super (context);
            setEGLContextClientVersion (2);
            setRenderer (this);
            setRenderMode (RENDERMODE_WHEN_DIRTY);
        }

        @Override
        public void onSurfaceCreated (GL10 unused, EGLConfig config)
        {
            contextCreated();
        }

        @Override
        public void onSurfaceChanged (GL10 unused, int width, int height)
        {
            contextChangedSize();
        }

        @Override
        public void onDrawFrame (GL10 unused)
        {
            render();
        }

        private native void contextCreated();
        private native void contextChangedSize();
        private native void render();
    }

    //==============================================================================
    public final int[] renderGlyph (char glyph, Paint paint, android.graphics.Matrix matrix, Rect bounds)
    {
        Path p = new Path();
        paint.getTextPath (String.valueOf (glyph), 0, 1, 0.0f, 0.0f, p);

        RectF boundsF = new RectF();
        p.computeBounds (boundsF, true);
        matrix.mapRect (boundsF);

        boundsF.roundOut (bounds);
        bounds.left--;
        bounds.right++;

        final int w = bounds.width();
        final int h = bounds.height();

        Bitmap bm = Bitmap.createBitmap (w, h, Bitmap.Config.ARGB_8888);

        Canvas c = new Canvas (bm);
        matrix.postTranslate (-bounds.left, -bounds.top);
        c.setMatrix (matrix);
        c.drawPath (p, paint);

        final int sizeNeeded = w * h;
        if (cachedRenderArray.length < sizeNeeded)
            cachedRenderArray = new int [sizeNeeded];

        bm.getPixels (cachedRenderArray, 0, w, 0, 0, w, h);
        bm.recycle();
        return cachedRenderArray;
    }

    private int[] cachedRenderArray = new int [256];

    //==============================================================================
    public static class HTTPStream
    {
        public HTTPStream (HttpURLConnection connection_) throws IOException
        {
            connection = connection_;
            inputStream = new BufferedInputStream (connection.getInputStream());
        }

        public final void release()
        {
            try
            {
                inputStream.close();
            }
            catch (IOException e)
            {}

            connection.disconnect();
        }

        public final int read (byte[] buffer, int numBytes)
        {
            int num = 0;

            try
            {
                num = inputStream.read (buffer, 0, numBytes);
            }
            catch (IOException e)
            {}

            if (num > 0)
                position += num;

            return num;
        }

        public final long getPosition()                 { return position; }
        public final long getTotalLength()              { return -1; }
        public final boolean isExhausted()              { return false; }
        public final boolean setPosition (long newPos)  { return false; }

        private HttpURLConnection connection;
        private InputStream inputStream;
        private long position;
    }

    public static final HTTPStream createHTTPStream (String address, boolean isPost, byte[] postData,
                                                     String headers, int timeOutMs,
                                                     java.lang.StringBuffer responseHeaders)
    {
        try
        {
            HttpURLConnection connection = (HttpURLConnection) (new URL (address).openConnection());

            if (connection != null)
            {
                try
                {
                    if (isPost)
                    {
                        connection.setConnectTimeout (timeOutMs);
                        connection.setDoOutput (true);
                        connection.setChunkedStreamingMode (0);

                        OutputStream out = connection.getOutputStream();
                        out.write (postData);
                        out.flush();
                    }

                    return new HTTPStream (connection);
                }
                catch (Throwable e)
                {
                    connection.disconnect();
                }
            }
        }
        catch (Throwable e)
        {}

        return null;
    }

    public final void launchURL (String url)
    {
        startActivity (new Intent (Intent.ACTION_VIEW, Uri.parse (url)));
    }
}
