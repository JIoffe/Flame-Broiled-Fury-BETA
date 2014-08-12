package com.Foofles;

/**
 * This class represents a slice of scrolling background.
 * The idea here is that each layer repeats a pattern of images endlessly
 * and each scrolls at a different speed. 
 * @author Jason Ioffe
 *
 */
//TODO - Improve efficiency.
public class ParallaxLayer {
	public float fSpriteScale;
	public float fSpriteWidth;
	public int nImages; // How many unique images do we cycle through
	//public int nSprites;
	public float X;
	public float Y; 
	public float SpeedMultiplier;
	//public float MinX;
	//public float MaxX;
	public int[] mSpriteIDs;
	public float fSpritePadding;
	public ParallaxLayer(final int nIndex){
		mSpriteIDs = new int[1];
		nImages = 1;
		mSpriteIDs[0] = nIndex;
	}
	public ParallaxLayer(final int iA, final int iB){
		mSpriteIDs = new int[2];
		nImages = 2;
		mSpriteIDs[0] = iA;
		mSpriteIDs[1] = iB;
	}
	public ParallaxLayer(final int iA, final int iB, final int iC){
		mSpriteIDs = new int[3];
		nImages = 3;
		mSpriteIDs[0] = iA;
		mSpriteIDs[1] = iB;
		mSpriteIDs[2] = iC;
	}
	public ParallaxLayer(final int iA, final int iB, final int iC, final int iD){
		mSpriteIDs = new int[4];
		nImages = 4;
		mSpriteIDs[0] = iA;
		mSpriteIDs[1] = iB;
		mSpriteIDs[2] = iC;
		mSpriteIDs[3] = iD;
	}
	public void UpdateAndRender(final float CamY, final float CamX, final SpriteBatch spriteBatch){
		if(spriteBatch != null){
			final float MinX = -Globals.ratio - fSpriteScale - fSpritePadding;
			final float MaxX = -Globals.ratio + fSpriteScale;
			final int nSprites = (int)(Globals.screenspaceWidth / fSpriteWidth) + 3; 
			
			X = CamX * SpeedMultiplier;
			//Y += CamY * SpeedMultiplier*0.7f;
			final int Shift = (int)(X / nImages);
			int Count = (int)-X + (Shift * nImages);

			if(X < 0.0f || X > 1.0f){
				X = X - (int)X;
			}
			float XPos = FoofMath.Lerp(MinX, MaxX, X);
			for(int i = 0; i < nSprites; i++){
				spriteBatch.AddSprite(XPos, Y, mSpriteIDs[Count], fSpriteScale, false, null);
				XPos += fSpriteWidth;
				Count++;
				if(Count >= nImages){
					Count = 0;
				}
			}
		}
		return;
	}
	public void SetLayerSpacing(final float fScale, final float fPadding, final float fInitialY, final float fInitialSpeedX){
		fSpriteScale = fScale;
		fSpriteWidth = fSpriteScale * 2.0f + fPadding;
		fSpritePadding = fPadding;
		SpeedMultiplier = fInitialSpeedX;
		X = 0.0f;
		Y = fInitialY;
		return;
	}
}
