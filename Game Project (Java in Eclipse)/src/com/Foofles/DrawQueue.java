package com.Foofles;


public abstract class DrawQueue {
	// 0 = BG,  1 = PlayField , 2 = HUD
	public static final int nSpriteBatches = 3;
	public static int bgIndex;
	public static final Object Lock = new Object();
	public static final SpriteBatch[] spriteBatches = new SpriteBatch[3];
	
	public static boolean bReadyForNewFrame;
	
	public static void Initialize(){
		spriteBatches[0] = new SpriteBatch();
		spriteBatches[1] = new SpriteBatch();
		spriteBatches[2] = new SpriteBatch();
	}
}
