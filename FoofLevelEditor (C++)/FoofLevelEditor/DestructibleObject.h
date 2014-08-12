#pragma once
#include "ExtendedObject.h"

struct DestructibleObject : public ExtendedObject{
	int Type;
	int SpawnID;
	int HP;
};