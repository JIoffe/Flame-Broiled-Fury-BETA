#pragma once
#include "Enemy.h"
struct EnemyWalker :
	public Enemy{
		int bChargesPlayer;
		float JumpStrength;
};
