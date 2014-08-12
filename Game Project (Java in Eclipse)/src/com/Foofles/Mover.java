package com.Foofles;

public class Mover extends DynamicObject {
	public byte uFunctionType;
	public byte uMoverType;
	public float fSpeed;
	public float Width;
	public float Pause;
	public float OriginX;
	public float OriginY;
	public float Sync;  //Offset of Time
	public float Mod;
	
	public static final byte FunctionSin = 1;
	public static final byte FunctionLin = 2;
	public static final byte HorizontalMover = 0;
	public static final byte VerticalMover = 1;
	public static final byte Fader = 2;
	public static final byte Slammer = 3;
	

	@Override
	public boolean Update(){
		float DeltaX = 0.0f;
		float DeltaY = 0.0f;
		if(uMoverType == Slammer){
			Mod = FoofMath.SineWave(Globals.runtimeSeconds + Sync, fSpeed, Width, Pause, Pause);
			final float NewY = Mod + OriginY;
			DeltaY = NewY - Y;
			Y = Mod + OriginY;
			X = OriginX;
		}else{
			Mod = FoofMath.TriangleWave(Globals.runtimeSeconds + Sync, fSpeed, Width, Pause, 0);
			//final float Mod = (float) Math.sin(Globals.runtimeSeconds);
			switch(uMoverType){
				case HorizontalMover:{
					final float NewX = Mod + OriginX;
					DeltaX = NewX - X;
					X = NewX;
					Y = OriginY;
					break;
				}
				case VerticalMover:{
					final float NewY = Mod + OriginY;
					DeltaY = NewY - Y;
					Y = Mod + OriginY;
					X = OriginX;
					break;
				}
				default:
					break;
			}
		}
		//if(DeltaX != 0.0f || DeltaY != 0.0f){
		VelocityX = DeltaX;
		CD.Translate(DeltaX, DeltaY);
		return true;
	}
	
	@Override
	public void ComputeVisibleArea(){
		final float WidthRight = CD.Right - X;
		final float WidthLeft = X - CD.Left;

		if(uMoverType == Mover.VerticalMover || uMoverType == Mover.Fader){
			VisibilityMax = OriginX + WidthRight;
			VisibilityMin = OriginX - WidthLeft;
		}else{
			final float DoubleWidth = Width * 2;
			VisibilityMax = OriginX + DoubleWidth + WidthRight;
			VisibilityMin = OriginX - DoubleWidth - WidthLeft;
		}
		return;
	}
	@Override
	void Render(final float CamX, final float CamY, final SpriteBatch mSpriteBatch){
		final int SpriteID = (mAnimation.bIsPlaying) ? mAnimation.AdvanceFrame() : Info.nSpriteID;
		if(uMoverType == Mover.Fader){
			mAnimation.bIsPlaying = true;
			if(Mod > 0.0f){
				Globals.mGameGraph.SetColor(1.0f, 1.0f, 1.0f, (float)Math.sin(Mod)*2.0f);
				mSpriteBatch.AddSprite(X - CamX, Y - CamY, SpriteID, Info.fDefaultScale, bFlip, Globals.mGameGraph.Color);
			}
		}else{
			mSpriteBatch.AddSprite(X - CamX, Y - CamY, SpriteID, Info.fDefaultScale, bFlip, null);
		}
		return;
	}
}
