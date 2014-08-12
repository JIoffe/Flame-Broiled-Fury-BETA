package com.Foofles;

public class FallingPlatform extends DynamicObject{

	public FallingPlatform(){
		mAnimation.Behaviour = AnimationType.Behaviour_PingPong;
		mAnimation.bIsPlaying = true;
		Type = GameObjectTypes.Enemy;
	}
	@Override
	public boolean Update(){
		final float DeltaY = VelocityY * Globals.secondsSinceLastFrame;
		Y += DeltaY;
		CD.Translate(0.0f, DeltaY);
		return true;
	}	
	@Override
	public void PlayerSteppedOn(){
		VelocityY = -0.6f;
		return;
	}
}
