package com.Foofles;

public class AnimationComponent {
	//TODO - Consider removing the Animations array here altogether
	public char Behaviour;
	public AnimationTrack[] Animations;
	public int CurrentTrack = 0;
	public float fCurrentFrame = 0;
	public boolean bIsPlaying = true;
	
	//public float fAnimationSpeed = 5.0f; // just a random number
	//public int nFrames = 1;
	//public int iStartingFrame = 
	
	protected char PingPongDirection; // Not really meant for outside tampering.
	
	public AnimationComponent(){
		Animations = null;
		bIsPlaying = false;
		Behaviour = AnimationType.Behaviour_Loop;
		fCurrentFrame = 0.0f;
		PingPongDirection = 0;
	}
	public int AdvanceFrame(){
		final AnimationTrack Anim = Animations[CurrentTrack];
		final int iIndexStart = Anim.iStartingFrame;
		if(bIsPlaying){
			final float fAnimationSpeed = Anim.fDefaultFPS;
			final int nFrames = Anim.nFrames;
			switch(Behaviour){
				case AnimationType.Behaviour_Loop:{
					fCurrentFrame += fAnimationSpeed*Globals.secondsSinceLastFrame;
					if((int)fCurrentFrame > (nFrames - 1)){
						fCurrentFrame -= (int)fCurrentFrame; //Let's keep the decimal and go to 0
					}
					break;
				}
				case AnimationType.Behaviour_PingPong:{
					if(PingPongDirection == 0){
						fCurrentFrame += fAnimationSpeed*Globals.secondsSinceLastFrame;
						if((int)fCurrentFrame >= nFrames){
							fCurrentFrame = (nFrames - 1) - (fCurrentFrame - (int)fCurrentFrame);
							PingPongDirection = 1;
						}
					}else{
						fCurrentFrame -= fAnimationSpeed*Globals.secondsSinceLastFrame;
						if(fCurrentFrame <= 0){
							fCurrentFrame = 1.0f + Math.abs(fCurrentFrame);
							PingPongDirection = 0;
						}
					}
					break;
				}
				case AnimationType.Behaviour_OneShot:{
					fCurrentFrame += fAnimationSpeed*Globals.secondsSinceLastFrame;
					if((int)fCurrentFrame > (nFrames - 1)){
						fCurrentFrame = nFrames - 1;
						bIsPlaying = false;
					}
					break;
				}
				case AnimationType.Behaviour_PingPongOneShot:{
					if(PingPongDirection == 0){
						fCurrentFrame += fAnimationSpeed*Globals.secondsSinceLastFrame;
						if((int)fCurrentFrame >= nFrames){
							fCurrentFrame = (nFrames - 1) - (fCurrentFrame - (int)fCurrentFrame);
							PingPongDirection = 1;
						}
					}else{
						fCurrentFrame -= fAnimationSpeed*Globals.secondsSinceLastFrame;
						if(fCurrentFrame <= 0){
							fCurrentFrame = 0.0f;
							bIsPlaying = false;
							PingPongDirection = 0;
						}
					}					
					break;
				}
				default:  //ie on pause, don't do shit.
					break;
			}
		}
		int texID = iIndexStart + (int)fCurrentFrame;
		//final int LastFrame = iIndexStart + nFrames - 1;
		//if(texID > LastFrame){
			//texID = LastFrame;
	//	}else if(texID < 0){
		//	texID = 0;
	//	}
		return texID;
	}
	void SetTrack(final int nTrackID){
		if(Animations[nTrackID] != null){
			CurrentTrack = nTrackID;
			fCurrentFrame = 0.0f;
			bIsPlaying = true;
		}else{
			bIsPlaying = false;
		}
	}
}
