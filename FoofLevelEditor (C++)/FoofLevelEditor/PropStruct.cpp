#include "PropStruct.h"
#include "Collision.h"
#include "Globals.h"
void PropStruct::Duplicate(PropStruct ** ppPropStruct){
	*ppPropStruct = new PropStruct;
	**ppPropStruct = *this;
}

void PropStruct::Update(){
	//THEY DON'T ACTUALLY STOP AT THE EDGES THAT's THE PROBLEM!!!!!!!!!!!!!!!!
	//y = TraceFloorHeight(x, y) + 0.1f;

	//float Delta = 1.0f * Globals::SecondsSinceLastFrame;
	//if(!PointIntersectsWallOrEdge(x, y, Delta)){
	//	x += Delta;
	//}
	return;
}