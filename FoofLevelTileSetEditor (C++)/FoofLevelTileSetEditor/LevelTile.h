#pragma once
#include <string>
struct LevelTile{
	int HeightMask[64];
	int SpriteID;
	int IsCeiling;
	int IsNotWall;
	std::string Name;
	LevelTile(){
		for(int i = 0; i < 64; i++){
			HeightMask[i] = 0;
		}
	}
};