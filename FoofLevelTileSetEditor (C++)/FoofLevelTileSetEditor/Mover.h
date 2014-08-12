#pragma once
#include "dynamicobject.h"
struct Mover :
	public DynamicObject{
		int iMovingAnimationIndex;
		int HP;
		int bIsHazard; // Either a hazard or a moving platform
};

