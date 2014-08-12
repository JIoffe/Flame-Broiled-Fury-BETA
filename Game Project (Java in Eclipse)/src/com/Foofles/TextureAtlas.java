package com.Foofles;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import android.util.Log;

/**
 *  This class will take a group of art assets 
 *  and assemble them into one giant sheet of textures
 *  to prevent multiple bindtexture calls.
 *  
 *  It will also store the texture coordinates for each texture by ID.
 * @author Jason Ioffe
 *
 */
public class TextureAtlas { //purely static 
	public float[][] pTexUVs = null; //2D array
	final private float[] pFlippedUVs = new float[8];
	public int nTextures = 0;
	private String[] TextureTitles = null; //Perform initial lookups by title...
	public void GetUVsByIndex(final int Index){
		return;
	}
	public Boolean ParseUVFromFile(final InputStream is, final boolean bFlipVertical){
//TODO - MAke this fast
		
		/*
		 *  This will parse a uv file
		 *  and extract the UV texture coordinates
		 *  for each independent texture in the 
		 *  texture atlas.
		 *  
		 *  File contents:
		 *  Foof72    (token)
		 *  # of Individual Textures
		 *  UV Data.
		 *  
		 */
		//We can't actually "delete" in java so make a note to the
		//GC to remove previous entries
		TextureTitles = null;
		pTexUVs = null; 
		nTextures = 0;
		final BufferedReader reader = new BufferedReader(new InputStreamReader(is));
		String line = null;
		try {
			line = reader.readLine();
		} catch (IOException e) {
			Log.e("Tex Atlas", "Could not read file token!");
			try {
				reader.close();
				is.close();
			} catch (IOException e0) {
				//This shouldn't matter, we're closing it, right?
			}
			return false;
		}
		if(line.equals("Foof72")){
			try {
				line = reader.readLine();
			} catch (IOException e1) {
				Log.e("Tex Atlas", "Could not parse file header!");
				try {
					reader.close();
					is.close();
				} catch (IOException e2) {
					//This shouldn't matter, we're closing it, right?
				}
				return false;
			}
			nTextures = Integer.parseInt(line);
			TextureTitles = new String[nTextures];
			pTexUVs = new float[nTextures][8];
			try{
				for(int x = 0; x < nTextures; x++){
					TextureTitles[x] = reader.readLine();
					//Log.d("Initialization", "Texture - " + TextureTitles[x]);
					
					line = reader.readLine();
					final float Left = Float.parseFloat(line);
					line = reader.readLine();
					float Top = Float.parseFloat(line);
					line = reader.readLine();
					final float Right = Float.parseFloat(line);
					line = reader.readLine();
					float Bottom = Float.parseFloat(line);
					//TODO - Remove this for release
					if(bFlipVertical){
						Top = 1.0f - Top;
						Bottom = 1.0f - Bottom;
					}
					pTexUVs[x][0] = Left;
					pTexUVs[x][1] = Top;
					pTexUVs[x][2] = Left;
					pTexUVs[x][3] = Bottom;
					pTexUVs[x][4] = Right;
					pTexUVs[x][5] = Bottom;
					pTexUVs[x][6] = Right;
					pTexUVs[x][7] = Top;

				}
			}catch(IOException e){
				//Something went wrong
				Log.e("Tex Atlas", "Error reading file!");
				try {
					reader.close();
					is.close();
				} catch (IOException e3) {
					//This shouldn't matter, we're closing it, right?
				}
				return false;
			} 

		}
		else{
			//Bad File!
			Log.e("Tex Atlas", "Bad token on file - " + line);
			try {
				reader.close();
				is.close();
			} catch (IOException e) {
				//This shouldn't matter, we're closing it, right?
			}
			return false;
		}
		try { 
			reader.close();
			is.close();
		} catch (IOException e) {
			//This shouldn't matter, we're closing it, right?
		}
		//All Sytems green

		return true;
	}
	public final float[] GetUVsByIndex(final int index, final boolean bFlip){
		//TODO - One of the main bottlenecks, find way to make this faster
		if(!bFlip){
			return pTexUVs[index];
		}else{
			//Flipping requires slightly more work.
			final float[] pUVs = pTexUVs[index];
			//pFlippedUVs
			pFlippedUVs[0] = pUVs[4];
			pFlippedUVs[1] = pUVs[1];
			pFlippedUVs[2] = pUVs[6];
			pFlippedUVs[3] = pUVs[3];
			pFlippedUVs[4] = pUVs[0];
			pFlippedUVs[5] = pUVs[5];
			pFlippedUVs[6] = pUVs[2];
			pFlippedUVs[7] = pUVs[7];
			
			return pFlippedUVs;
		}
	}
	public int GetIndexByTitle(final String Title){
		//TODO - Find less derp way of doing this.
		for(int i = 0; i < nTextures; i++){	
			if(TextureTitles[i].equalsIgnoreCase(Title)){
				return i;
			}
		}
		Log.e("TEX ATLAS", "Couldn't find texture " + Title);
		return -1; // If we don't find anything, return - 1 as an error
	}
}
