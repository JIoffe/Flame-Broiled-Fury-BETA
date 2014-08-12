#pragma once
#include "extendedobject.h"
struct Enemy :
	public ExtendedObject{

	int HP;
	int AttackAnimationIndex;
	int DeathAnimationIndex;
	float Speed;
	int ShotType;
	int ShotDirection;
	int nShots;
	float ShotDelay;
	int EnemyType;

	int bIsDormant; //Dormant means activated by player proximity
};

