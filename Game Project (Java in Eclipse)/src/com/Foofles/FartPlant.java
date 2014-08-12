package com.Foofles;

public class FartPlant extends DynamicObject {
	final FoofTimer FartTimer = new FoofTimer(8.0f, true);
	final ParticleSystem FartParticles = new ParticleSystem(10, 0.1f, 34);
	public FartPlant(){
		mAnimation.Behaviour = AnimationType.Behaviour_PingPongOneShot;
		mAnimation.bIsPlaying = false;
		Type = GameObjectTypes.Enemy;
	}
	@Override
	public boolean Update(){
		if(FartTimer.Update()){
			mAnimation.bIsPlaying = true;
			FartParticles.Start(X, Y, 5.0f);
		}
		return true;
	}	
	@Override
	void Render(final float CamX, final float CamY, final SpriteBatch mSpriteBatch){
		FartParticles.UpdateAndDraw(mSpriteBatch);		
		super.Render(CamX, CamY, mSpriteBatch);		
		return;
	}	
}
