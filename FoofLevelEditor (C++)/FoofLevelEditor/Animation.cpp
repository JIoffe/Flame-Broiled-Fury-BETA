#include "Animation.h"
#include "Globals.h"
AnimationComponent::AnimationComponent(){
		pTrack = NULL;
		bIsPlaying = false;
		Behaviour = AnimationType::Behaviour_Loop;
		fCurrentFrame = 0.0f;
		PingPongDirection = 0;
	}
int AnimationComponent::AdvanceFrame(){
		float fAnimationSpeed = pTrack->fDefaultSpeed;
		int nFrames = pTrack->nFrames;
		int iIndexStart = pTrack->iStartingFrame;
		switch(Behaviour){
			case AnimationType::Behaviour_Loop:{
				fCurrentFrame += fAnimationSpeed * Globals::SecondsSinceLastFrame;
				if((int)fCurrentFrame > (nFrames - 1)){
					fCurrentFrame -= (int)fCurrentFrame; //Let's keep the decimal and go to 0
				}
				break;
			}
			case AnimationType::Behaviour_PingPong:{
				if(PingPongDirection == 0){
					fCurrentFrame += fAnimationSpeed * Globals::SecondsSinceLastFrame;
					if((int)fCurrentFrame >= nFrames){
						fCurrentFrame = (nFrames - 1) - (fCurrentFrame - (int)fCurrentFrame);
						PingPongDirection = 1;
					}
				}else{
					fCurrentFrame -= fAnimationSpeed * Globals::SecondsSinceLastFrame;
					if(fCurrentFrame <= 0){
						fCurrentFrame = 1.0f + abs(fCurrentFrame);
						PingPongDirection = 0;
					}
				}
				break;
			}
			case AnimationType::Behaviour_OneShot:{
				fCurrentFrame += fAnimationSpeed * Globals::SecondsSinceLastFrame;
				if((int)fCurrentFrame > (nFrames - 1)){
					fCurrentFrame = nFrames - 1;
					bIsPlaying = false;
				}
				break;
			}
		}
		int texID = iIndexStart + (int)fCurrentFrame;
		const int LastFrame = iIndexStart + nFrames - 1;
		if(texID > LastFrame){
			texID = LastFrame;
		}else if(texID < 0){
			texID = 0;
		}
		return texID;
}