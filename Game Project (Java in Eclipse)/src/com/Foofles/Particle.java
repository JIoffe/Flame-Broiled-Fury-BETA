package com.Foofles;

public class Particle implements Copyable, UpdateRender {
	int ImageID = 0;
	public float VelocityX;
	public float VelocityY;
	public float ScaleShift = 0.0f;
	public float X;
	public float Y;
	public float Scale;
	public float Lifetime;
	public float HalfLifespan;
	final public AnimationComponent mAnimation = new AnimationComponent();
	
	public Particle(){
		mAnimation.Behaviour = AnimationType.Behaviour_OneShot;
		mAnimation.Animations = new AnimationTrack[1];
	}
	@Override
	public boolean UpdateAndDraw(SpriteBatch spriteBatch) {
		final GameGraph mGameGraph = Globals.mGameGraph;
		if(Lifetime > 0.0f && X < mGameGraph.DRight &&
				X  > mGameGraph.DLeft){
			final int SpriteID = (mAnimation.bIsPlaying) ? mAnimation.AdvanceFrame() : ImageID;
			if(Lifetime < HalfLifespan){
				mGameGraph.SetColor(1.0f, 1.0f, 1.0f, Lifetime/HalfLifespan);
				spriteBatch.AddSprite(X - mGameGraph.CamX, Y - mGameGraph.CamY, SpriteID, Scale, false, mGameGraph.Color);
			}else{
				spriteBatch.AddSprite(X - mGameGraph.CamX, Y - mGameGraph.CamY, SpriteID, Scale, false, null);
			}
			X += VelocityX * Globals.secondsSinceLastFrame;
			Y += VelocityY * Globals.secondsSinceLastFrame;
			Scale += ScaleShift * Globals.secondsSinceLastFrame;
			Lifetime -= Globals.secondsSinceLastFrame;
			return true;
		}else{	
			return false;
		}
	}

	@Override
	public <T> void Copy(T Src) {
		final Particle PSrc = (Particle)Src;
		ImageID = PSrc.ImageID;
		X = PSrc.X;
		Y = PSrc.Y;
		Scale = PSrc.Scale;
		VelocityX = PSrc.VelocityX;
		VelocityY = PSrc.VelocityY;
		Lifetime = PSrc.Lifetime;
		HalfLifespan = PSrc.HalfLifespan;
		mAnimation.fCurrentFrame = PSrc.mAnimation.fCurrentFrame;
		mAnimation.bIsPlaying = PSrc.mAnimation.bIsPlaying;
	}
}
