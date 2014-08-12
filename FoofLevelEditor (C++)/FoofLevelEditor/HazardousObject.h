#pragma once
#include <string>
#include <cstdio>
#include "ExtendedObject.h"

struct HazardousObject : public ExtendedObject{
	int Damage;   //How much damage is done to the player?
	int SpriteID;
};