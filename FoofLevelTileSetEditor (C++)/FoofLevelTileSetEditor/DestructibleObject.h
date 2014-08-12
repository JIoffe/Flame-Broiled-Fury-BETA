#pragma once
#include "DynamicObject.h"

struct DestructibleObject : public DynamicObject{
	int Type;
	int SpawnID;
	int HP;
};