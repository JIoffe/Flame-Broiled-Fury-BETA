package com.Foofles;

/**
 * Misc. Global vars and time.
 * 
 * @author Jason Ioffe
 *
 */
public abstract class Globals {
	public static float runtimeSeconds = 0.0f;
	public static float secondsSinceLastFrame = 0.0f;
	public static long msSinceLastFrame = 0;
	public static float fGravity = 0.0f;
	public static GameGraph mGameGraph = null;
	public static BaseGame mGame = null;
	static public FoofGameActivity GameActivity = null;
	public static int CurrentTheme = 0;
	public static float ratio = 0.0f; //Ratio of Screen width to height
	public static float screenspaceWidth;
	
	final public static float[] FSQuadVertices = new float[12];
}
