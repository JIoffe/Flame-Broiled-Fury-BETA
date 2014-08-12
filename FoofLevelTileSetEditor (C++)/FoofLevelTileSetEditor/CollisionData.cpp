#include "CollisionData.h"

void CollisionData::Copy(const CollisionData &Src){
	Left = Src.Left;
	Right = Src.Right;
	Top = Src.Top;
	Bottom = Src.Bottom;
}