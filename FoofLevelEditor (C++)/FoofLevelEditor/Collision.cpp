#include "Collision.h"
#include "FoofMath.h"
#include "LevelScene.h"

extern LevelScene Scene;

bool CollisionAgainstLevelHorizontally(float Delta, CollisionData  * pCD, float WallSenseHeight){
	return GetTileHeight(Delta > 0.0f ? (pCD->Right + Delta) : (pCD->Left + Delta), WallSenseHeight) > WallSenseHeight;
}
bool CollisionAgainstWallsOrEdges(float Delta, CollisionData * pCD, float WallSenseHeight){
	if(CollisionAgainstLevelHorizontally(Delta, pCD, WallSenseHeight)){
		return true;
	}else{
		int X = (Delta > 0.0f ? (pCD->Right + Delta) : (pCD->Left + Delta)) / LevelScene::LevelTileOffset;
		int Y = (pCD->Bottom / LevelScene::LevelTileOffset) - 1;
		return (Scene.GetGridUnit(X, Y) == -1);
	}
}

float GetTileHeight(float X,  float Y){
	float Height = 0.0f;
	const int LookupX = X / LevelScene::LevelTileOffset;
	const int LookupY = Y / LevelScene::LevelTileOffset;
	const int TileID = Scene.GetGridUnit(LookupX, LookupY);
	if(TileID > -1 && TileID != 127){
		const float HeightmapStep = LevelScene::LevelTileOffset / 64.0f;

		const float TileStart = LookupX * LevelScene::LevelTileOffset;
		int HeightmapLookup = (X - TileStart) / HeightmapStep;
		const float LocalHeight = Globals::TileSet[TileID].HeightMask[HeightmapLookup] * HeightmapStep;
		if(LocalHeight > 0.01f){
			Height = LookupY * LevelScene::LevelTileOffset + LocalHeight;
		}
	}
	//TODO - DO!
	return Height;
}
float FindCeilingIntersectionHeight(float XLeft, float XRight, float Y){
	const float HeightLeft = GetTileHeight(XLeft, Y);
	const float HeightRight = GetTileHeight(XRight, Y);
	float ReturnHeight = -1.0f;
	if(HeightLeft < HeightRight && HeightLeft > 0.01f){
		ReturnHeight = HeightLeft;
	}else{
		if(HeightRight > 0.01f)
			ReturnHeight = HeightRight;
	}
	return ReturnHeight;
}
float FindFloorIntersectionHeight(float XLeft, float XRight, float Y){
	const float HeightLeft = GetTileHeight(XLeft, Y);
	const float HeightRight = GetTileHeight(XRight, Y);
	if(HeightLeft > HeightRight){
		return HeightLeft;
	}else{
		return HeightRight;
	}
}

CamZone * GetCurrentCamZone(float X, float Y){
	CamZone * pCamZone = NULL;
	for(std::vector<CamZone*>::iterator it = Scene.CamList.begin(); it < Scene.CamList.end(); it++){
		CamZone * pCam = *it;
		if(X < pCam->Right && X > pCam->Left
			&& Y > pCam->Down && Y < pCam->Up){
				pCamZone = pCam;
				break;
		}
	}
	return pCamZone;
}

bool PointIntersectsWallOrEdge(float X, float Y, float Delta){
		//Are we going right? Check right side. Else, check left
		const int CurrentX = (int)(X / LevelScene::LevelTileOffset);
		const float FutureX = X + Delta;
		const int YIndex = (int)(Y / LevelScene::LevelTileOffset);
		int GridID = Scene.GetGridUnit(FutureX, YIndex);
		const float CurrentHeight = GetTileHeight(X, Y);
		if(GetTileHeight(FutureX, Y) > CurrentHeight + 0.15f)
			return true;
		else{
			const float BelowY = Y - LevelScene::LevelTileOffset;
			return (GetTileHeight(FutureX, BelowY) < 0.01f);
		}
}
float TraceFloorHeight(float X, float Y){
	//TODO - IMPROVE
	float Height = 0.0f;
	while(Height < 0.01f && Y > 0.0f){
		Height = GetTileHeight(X, Y);
		Y -= LevelScene::LevelTileScale;
	}

	return Height;
}