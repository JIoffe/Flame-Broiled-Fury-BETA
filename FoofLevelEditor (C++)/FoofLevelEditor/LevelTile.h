#pragma once
struct LevelTile{
	int HeightMask[64];
	int SpriteID;
	int IsCeiling;
	int IsNotWAll;
	std::string Name;
	LevelTile(){
		for(int i = 0; i < 64; i++){
			HeightMask[i] = 0;
		}
	}
};