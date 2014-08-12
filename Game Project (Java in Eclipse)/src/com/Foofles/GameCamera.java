package com.Foofles;

public class GameCamera {
	public float X;
	public float Y;
	
	public float VerticalBias;
	public float HorizontalBias;
	public static final float DefaultVerticalOffset = 0.33f;
	public static final float DefaultHorizontalOffset = 0.25f;
	public GameCamera(){
		X = 0.0f;
		Y = 0.0f;
		VerticalBias = 0.0f;
		HorizontalBias = 0.0f;
	}
	public void Update(final GameObject go){
		X = go.X;
		
		//TODO - FIX CAMERA SYSTEM
		Y = FoofMath.Lerp(Y, go.Y + 0.33f, 0.01f);
		return;
	}
}
