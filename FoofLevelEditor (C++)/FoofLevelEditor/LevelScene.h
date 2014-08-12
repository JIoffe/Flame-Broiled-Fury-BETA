#pragma once
#ifndef LEVEL_SCENE
#define LEVEL_SCENE
#include "PropStruct.h"
#include "CamZone.h"
#include <cstdio>
#include <vector>
#include "Player.h"
class LevelScene
{
	int GridWidth;
	int GridHeight;
	int GridSpacing;
	int * pLevelGrid;

	float PlayerStartX;
	float PlayerStartY;

public:
	Player mPlayer;
	float ExitPos_X;
	float ExitPos_Y;
	int SceneWidthPixels;
	int SceneHeightPixels;
	static const float LevelTileScale;
	static const float LevelTileOffset;
	static const float CoinWidth;
	static const float EnemyAWidth;
	static const float PlayerWidth;
	static const float PropBWidth;
	std::vector<PropStruct*> HazardList;
	std::vector<PropStruct*> DecoList;
	std::vector<PropStruct*> PropList;
	std::vector<PropStruct*> EnemyList;
	std::vector<PropStruct*> TokenList;
	std::vector<PropStruct*> BunnyList;
	std::vector<CamZone*> CamList;
	void InitializeGrid(int Width, int Height);

	int GetWidth();
	int GetHeight();
	int GetGridSpacing();

	int GetGridUnit(int x, int y);
	void SetGridUnit(int x, int y, int value);
	bool GridOk();

	void SetPlayerStart(float x, float y);
	float GetPlayerStartX();
	float GetPlayerStartY();

	void Update();

	void ResetPlayerPosition();
	void ResetScene();

	LevelScene();
	~LevelScene();
};
#endif