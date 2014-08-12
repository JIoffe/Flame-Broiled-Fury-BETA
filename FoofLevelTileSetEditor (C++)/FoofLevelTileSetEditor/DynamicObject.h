#pragma once 
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "CollisionData.h"

struct DynamicObject{
	std::string Name;
	int Type;
	int iDefaultAnimationIndex;
	int iSpriteIndex; 
	float fDefaultScale;
	CollisionData CD;
};