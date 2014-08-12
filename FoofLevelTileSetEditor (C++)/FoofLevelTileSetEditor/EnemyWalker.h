#pragma once
#include "Enemy.h"
struct EnemyWalker :
	public Enemy{
		int Aggressiveness;
		float JumpStrength;

	EnemyWalker(){
		this->Aggressiveness = 0;
		this->AttackAnimationIndex = -1;
		this->bIsDormant = 0;
		this->DeathAnimationIndex = -1;
		this->fDefaultScale = 0.0f;
		this->HP = -1;
		this->iDefaultAnimationIndex = -1;
		this->iSpriteIndex = -1;
		this->JumpStrength = 0.0f;
		this->Name = "Untitled";
		this->nBurstDelay = 0.0f;
		this->nShotsPerBurst = 0;
		this->ShotDirection = 0;
		this->ShotType = -1;
		this->Speed = 0.0f;
		this->Type = 0;
	}
};
