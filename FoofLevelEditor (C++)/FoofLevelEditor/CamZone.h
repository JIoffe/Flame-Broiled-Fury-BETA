#pragma once
#include "PropStruct.h"

struct CamZone : public PropStruct{
	static float Scale;
	static const int AffectY = 0;
	static const int AffectX = 1;
	static const int AffectXY = 2;
	float Left;
	float Up;
	float Down;
	float Right;

	int nAxis;
	void ScaleZone(float s){
		Left *= s;
		Right *= s;
		Up *= s;
		Down *= s;
		return;
	}
	void TranslateZone(float dX, float dY){
		Left += dX;
		Right += dX;
		Up += dY;
		Down += dY;
		return;
	}

	void Duplicate(PropStruct ** ppPropStruct){
		*ppPropStruct = new CamZone;
		memcpy(*ppPropStruct, this, sizeof(CamZone));
		return;
	}
};