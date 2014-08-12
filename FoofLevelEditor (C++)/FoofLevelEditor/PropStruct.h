#pragma once
#include "ExtendedObject.h"

struct PropStruct{
	static const unsigned int Collectible = 1;
	static const unsigned int Enemy = 2;
	static const unsigned int CameraZone = 66;
	static const unsigned int Coin = 1;
	static const unsigned int Hazard = 4;
	static const unsigned int Deco = 50;
	static const unsigned int CheckPoint = 88;
	static const unsigned int Bunny = 95;
	static const unsigned int Token = 105;
	float x;
	float y;
	unsigned int uType;
	ExtendedObject * ExtendedInfo;
	unsigned char uObjectType;
	bool bIsSelected;
	unsigned int nID; 
	
	float Speed;

	virtual void inline Update();
	virtual void Duplicate(PropStruct ** ppPropStruct);
};