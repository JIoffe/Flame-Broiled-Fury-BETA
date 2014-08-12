package com.Foofles;

/////////////////////////////////
//This contains collision information for each sprite.
//Note - NOT per game object, but per unique texture.
//Includes a base bounding box. This bounding box is manipulated
//to refit the sprite if it rotates... hopefully it's accurate enough

//(C) Jason Ioffe
public class CollisionData {
	//AA bounding box.
	//Base values for non-rotated sprite
	float Left;
	float Right;
	float Top;
	float Bottom;
	CollisionData NextCD = null;
	byte bIgnore; // Use to sort but not for collision
	byte bIsWeakpoint;
	public CollisionData(){
		Left = -1.0f;
		Right = 1.0f;
		Top = 1.0f;
		Bottom = -1.0f;
		NextCD = null;
		bIsWeakpoint = 1;
		bIgnore = 0;
		return;
	}
	public CollisionData(final float L, final float R, final float T, final float B){
		Left = L;
		Right = R;
		Top = T;
		Bottom = B;
	}
	public void Copy(CollisionData CD){
		Left = CD.Left;
		Right = CD.Right;
		Top = CD.Top;
		Bottom = CD.Bottom;
		//SpotGrid = CD.SpotGrid; // COPY BY REFERENCE
	}
	public void Scale(float fScale){
		Left *= fScale;
		Right *= fScale;
		Top *= fScale;
		Bottom *= fScale;
		return;
	}
	public void Translate(float x, float y){
		Left += x;
		Right += x;
		Top += y;
		Bottom += y;
		return;
	}
}
