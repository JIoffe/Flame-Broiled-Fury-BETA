package com.Foofles;

//A little blip that flies up to show that the player scored X amount of points.
//This should probably only apply to enemies, rather than collectibles
//so we don't spam the screen.
//(C) Jason Ioffe
public class ScoreToast implements Copyable, UpdateRender{
	public static final byte pts150 = 0;
	final public static byte TotalAmounts = 1;
	public static final ScoreToastDisplay[] DisplayAmounts = new ScoreToastDisplay[TotalAmounts];
	public float X;
	public float Y;
	public float Lifespan;
	public byte Index = 0;
	
	public static void InitializeScoreToasts(){
		DisplayAmounts[pts150] = new ScoreToastDisplay(150);
	}
	public boolean UpdateAndDraw(final SpriteBatch spriteBatch){
		Lifespan -= Globals.secondsSinceLastFrame;
		if(Lifespan > 0.0f){
			Y += 0.24f * Globals.secondsSinceLastFrame;
			final ScoreToastDisplay Display = ScoreToast.DisplayAmounts[Index];
			final float iY = Y - Globals.mGameGraph.CamY;
			float iX = X + ScoreToastComponent.NumberOffset - Globals.mGameGraph.CamX;
			for(byte d = 0; d < 3; d++){
				spriteBatch.AddSprite(iX, iY, Display.Digits[d], ScoreToastComponent.NumberScale, false, null);
				iX -= ScoreToastComponent.NumberOffset;
			}
			return true;
		}else{
			return false;
		}
	}
	public <T> void Copy(final T Src){
		ScoreToast src = (ScoreToast)Src;
		X = src.X;
		Y = src.Y;
		Lifespan = src.Lifespan;
		Index = src.Index;
		return;
	}
}
