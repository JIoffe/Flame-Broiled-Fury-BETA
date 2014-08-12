#pragma once
#include "Globals.h"
#include "Animation.h"
#include "Collision.h"

class Player{
public:
	static const int nPlayerAnimations = 1;
	static int ANIMATION_RUNBASIC;
	static int BASICRUN_SPRITEINDEX;

	static bool MOVE_RIGHT;
	static bool MOVE_LEFT;
	
	static float Speed;
	static float Scale;
	static float JumpVelocity;
	static float YVelocity;
	static unsigned char JumpCount;

	int CurrentSprite;

	CollisionData CD;
	AnimationComponent mAnimation;

	bool bFlip;

	float X;
	float Y;

	Player(void);
	~Player(void);

	void Initialize();
	void PrepareAnimations();
	void Update();
private:
		AnimationTrack BasicRunTrack;
};

