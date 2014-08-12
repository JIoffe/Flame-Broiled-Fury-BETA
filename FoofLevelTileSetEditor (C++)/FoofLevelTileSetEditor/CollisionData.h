#pragma once
struct CollisionData{
	float Left;
	float Right;
	float Top;
	float Bottom;

	void Copy(const CollisionData &Src);
};