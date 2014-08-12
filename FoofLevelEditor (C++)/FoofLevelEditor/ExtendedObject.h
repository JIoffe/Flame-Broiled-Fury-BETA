#pragma once
#include <cstdio>
#include <string>
#include "Animation.h"
#include "CollisionData.h"
struct ExtendedObject{
	int Type;
	int SubType;
	std::string Name;
	float fDefaultScale;
	int ArrayID;
	int iDefaultAnimationIndex;
	int iSpriteIndex; 
	int iBaseSpriteIndex;
	AnimationComponent mAnimation;
	CollisionData CD;
};