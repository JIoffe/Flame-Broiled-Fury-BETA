#pragma once
#include <string>
#include <cstdio>
#include "CollisionData.h"

struct HazardousObject{
	std::string Name;
	int Damage;   //How much damage is done to the player?
	int SpriteID;
	float fDefaultScale;
	CollisionData CD;
};