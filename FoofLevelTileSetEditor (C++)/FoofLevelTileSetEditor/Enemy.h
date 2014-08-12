#pragma once
#include "dynamicobject.h"
struct Enemy :
	public DynamicObject{

	int HP;
	int AttackAnimationIndex;
	int DeathAnimationIndex;
	float Speed;
	int ShotType;
	int ShotDirection;
	int nShotsPerBurst;
	float nBurstDelay; //Time between shot groupings
	int bIsDormant; //Dormant means activated by player proximity
};

