#pragma once
#include "CamZone.h"
#include "CollisionData.h"

bool CollisionAgainstLevelHorizontally(float Delta, CollisionData  * pCD, float WallSenseHeight);
float FindCeilingIntersectionHeight(float XLeft, float XRight, float Y);
float FindFloorIntersectionHeight(float XLeft, float XRight, float Y);
float GetTileHeight(float X,  float Y);
CamZone * GetCurrentCamZone(float X, float Y);
bool CollisionAgainstWallsOrEdges(float Delta, CollisionData * pCD, float X);
bool PointIntersectsWallOrEdge(float X, float Y, float Delta);
float TraceFloorHeight(float X, float Y);