package com.Foofles;

//System for managing and rendering the toasts that fly up to alert 
//the player when he/she scores x amount of points.
//The score toasts themselves reference precomputed arrays of numerical sprites

// (c) Jason Ioffe
public class ScoreToastComponent {
	final static float NumberOffset = 0.065f;
	final static float NumberScale = 0.05f;
	public static final byte MaxScoreToasts = 10; //TODO - Tweak this value
	public SmartArray<ScoreToast> mToasts;
	public static byte nToasts = 0;
	public ScoreToastComponent(){
		
	}
	
	public void Update(final SpriteBatch spriteBatch){
		mToasts.UpdateAndDraw(spriteBatch);
		return;
	}
	public void Draw(final SpriteBatch mSpriteBatch, final float CamX, final float CamY){
		if(nToasts == 0){
			return;
		}else{
			for(byte i = 0; i < nToasts; i++){

			}
		}
	}
	public void AddToast(final float X, final float Y, final byte Index){
		final ScoreToast mToast = mToasts.Add();
		if(mToast != null){
			mToast.X = X;
			mToast.Y = Y;
			mToast.Index = Index;
			mToast.Lifespan = 2.0f;
		}
		return;
	}
}
