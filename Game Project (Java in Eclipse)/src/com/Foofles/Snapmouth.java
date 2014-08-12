package com.Foofles;

public class Snapmouth extends DynamicObject {
	public static final float BiteTime = 0.7f;
	public static final float ShakeAmt = 0.034f;
	public static final float ShakeSpeed = 1.2f;
	public boolean ShakeDir = false; //F = left, T = right
	public boolean bGotSteppedOn = false;
	private boolean bIsBiting = false;
	public float fAngerTimer = 0.0f;
	public float XOrigin = 0.0f; //We will spin around this
	public Snapmouth(){
		mAnimation.Behaviour = AnimationType.Behaviour_PingPong;
		mAnimation.bIsPlaying = false;
		Type = GameObjectTypes.Enemy;
		VelocityX = 0.0f;
		VelocityY = 0.0f;
	}
	@Override
	public boolean Update(){
		if(bGotSteppedOn){
			fAngerTimer += Globals.secondsSinceLastFrame;
			if(fAngerTimer > BiteTime){
				mAnimation.bIsPlaying = true;
				bGotSteppedOn = false;
				bIsPlatform = 0;
				if(Globals.mGameGraph.mPlayer.mMountedPlatform == this){
					Globals.mGameGraph.mPlayer.mMountedPlatform = null;
				}
				bIsBiting = true;
			}else{
				if(ShakeDir){
					X -= ShakeSpeed * Globals.secondsSinceLastFrame;
					if(X < (XOrigin - ShakeAmt)){
						ShakeDir = false;
					}
				}else{
					X += ShakeSpeed * Globals.secondsSinceLastFrame;
					if(X > (XOrigin + ShakeAmt)){
						ShakeDir = true;
					}
				}
			}
		}else if(bIsBiting){
			if(fAngerTimer > 4.0f){
				mAnimation.Behaviour = AnimationType.Behaviour_PingPong;
				if((int)mAnimation.fCurrentFrame == 0){
					mAnimation.bIsPlaying = false;
					bIsBiting = false;
					fAngerTimer = 0.0f;
					bIsPlatform = 1;
				}
			}else{
				fAngerTimer += Globals.secondsSinceLastFrame;
				if((int)mAnimation.fCurrentFrame == 3){
					mAnimation.Behaviour = AnimationType.Behaviour_Paused;
				}
			}
		}
		return true;
	}
	@Override
	public void PlayerSteppedOn(){
		bGotSteppedOn = true;
		return;
	}
}
