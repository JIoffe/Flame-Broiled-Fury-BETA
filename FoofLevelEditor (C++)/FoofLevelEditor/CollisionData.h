#pragma once
struct CollisionData{
	float Left;
	float Right;
	float Top;
	float Bottom;

	void Scale(float fScale){
		Left *= fScale;
		Right *= fScale;
		Top *= fScale;
		Bottom *= fScale;
		return;
	}
	void Translate(float x, float y){
		Left += x;
		Right += x;
		Top += y;
		Bottom += y;
		return;
	}
};