package com.Foofles;

public class DynamicObject extends GameObject{
	public byte HP = -1;
	
	//For Potential Visibility Set List
	public DynamicObject PVSNext;
	public DynamicObject PVSPrev;
	public DynamicObjectPVSList PVSOwner;
	
	//Potential Visibility extents so we can update in a less
	//herp derp kind of way.
	public float VisibilityMin = 0.0f;
	public float VisibilityMax = 0.0f;
	
	public float VelocityX = 0.0f;
	public float VelocityY = 0.0f;
	
	public float DamageFlash = 0.0f;
	public boolean Update(){
		//TRUE = ALIVE , FALSE = DEAD
		final float DeltaX = VelocityX * Globals.secondsSinceLastFrame;
		final float DeltaY = VelocityY * Globals.secondsSinceLastFrame;
		X += DeltaX;
		Y += DeltaY;
		CD.Translate(DeltaX, DeltaY);
		
		return true;
	}
	public void ComputeVisibleArea(){
		//final float Offset = Info.fDefaultScale * 3.0f;
		VisibilityMin = CD.Left;
		VisibilityMax = CD.Right;
		return;
	}
	@Override
	public void Remove(){
		super.Remove();
		PVSOwner.Remove(this);
	}
	final public void Move(final float DeltaX, final float DeltaY){
		X += DeltaX;
		Y += DeltaY;
		CD.Translate(DeltaX, DeltaY);
		return;
	}
	//Override this to implement actions on player step
	//eg. Small enemies getting squished,fruit falling,
	//    traps triggering
	//TODO - consider making this in another class
	public void PlayerSteppedOn(){
		return;
	}
	
	@Override
	void Render(final float CamX, final float CamY, final SpriteBatch mSpriteBatch){
		final int SpriteID = (mAnimation.bIsPlaying) ? mAnimation.AdvanceFrame() : Info.nSpriteID;
		mSpriteBatch.AddSprite(X - CamX, Y - CamY, SpriteID, Info.fDefaultScale, bFlip, null);
		return;
	}
	void TakeDamage(){
		DamageFlash = 0.3f;
		HP--;
		return;
	}
	final void RenderDamageFlash(final float X, final float Y, final int SpriteID, final float fScale, final SpriteBatch mSpriteBatch){
		final float DamageColor = DamageFlash / 0.3f;
		Globals.mGameGraph.SetColor(1.0f, DamageColor, DamageColor, 1.0f);
		mSpriteBatch.AddWigglingSprite(X, Y, SpriteID, fScale, bFlip, Globals.mGameGraph.Color);
		return;
	}
}
