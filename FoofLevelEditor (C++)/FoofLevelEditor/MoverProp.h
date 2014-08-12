#pragma once
#include "extendedObject.h"
struct MoverProp :
	public ExtendedObject{
		int iMovingAnimationIndex;
		int HP;
		bool bIsHazard; // Either a hazard or a moving platform

};

