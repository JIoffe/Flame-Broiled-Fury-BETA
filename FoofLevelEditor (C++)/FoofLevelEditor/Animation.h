#pragma once
#include <cstdlib>
#include <string>

struct AnimationTrack{
	std::string Name;
	int nFrames;
	int iStartingFrame;
	float fDefaultSpeed;
};

class AnimationComponent {
public:
	char Behaviour;
	AnimationTrack * pTrack;
	float fCurrentFrame;
	bool bIsPlaying;
	
	AnimationComponent();
	int AdvanceFrame();

	protected:
		char PingPongDirection; // Not really meant for outside tampering.
	

};




struct AnimationType {
	const static char Behaviour_OneShot = 0;
	const static char Behaviour_Loop = 1;
	const static char Behaviour_PingPong = 2;
};