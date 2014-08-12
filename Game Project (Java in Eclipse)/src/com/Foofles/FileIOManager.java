package com.Foofles;

import java.io.InputStream;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

/**
 * This is a layer between the rest of the game and File / Res IO
 * Just to stream line everything instead of having
 * a ton of IO code in every other class.
 * 
 * @author Jason Ioffe
 *
 */
abstract public class FileIOManager {
	private static Resources mResources = null;
	public static void InitResources(Context mContext){
		mResources = mContext.getResources();
		return;
	}
	public static Bitmap GetBitmapResource(final int ResourceID){
		final Bitmap bmp = BitmapFactory.decodeResource(mResources, ResourceID);
		if(bmp == null){
			Log.e("File IO", "Failed to Load bitmap ID " + ResourceID);
		}
		return bmp;
	}
	public static InputStream GetInputStreamToResource(final int ResourceID){
		return mResources.openRawResource(ResourceID);
	}
}
