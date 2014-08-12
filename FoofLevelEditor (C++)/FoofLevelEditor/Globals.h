#pragma once
#include <cstdio>
#include <vector>
#include <string>
#include "Player.h"
#include "DestructibleObject.h"
#include "LevelTile.h"
#include "PropStruct.h"
#include "Animation.h"
#include "Mover.h"
#include "HazardousObject.h"
#include "Collision.h"
#include "StaticDeco.h"
#include "MoverProp.h"
#include "Enemy.h"
#include "EnemyWalker.h"
#include "ExtendedObject.h"

class Globals{
	public:
		static float fGravity;
		static bool bPreviewPlay;
		static int nLevelTileTypes;
		static LevelTile * TileSet;
		static ExtendedObject ** DynamicSet;
		static StaticDeco * DecoSet;
		static int nDecos;
		static HazardousObject * HazardousSet;
		static int nHazards;
		static int nDynamics;
		static bool bDrawGrid;
		static int EnemyA_ID;
		static int Coin_ID;
		static int Gem_ID;
		static int CheckPointIndex;
		static int BabyBunnyIndex;
		static int CageIndex;
		static int TokenIndex;
		static int ExitIndex;
		static int CollectibleID[10];
		static std::vector<PropStruct*> SelectedObjects;
		static PropStruct * pObjectBrush;
		static unsigned char uSnapGridType;
		static float MouseX_World;
		static float MouseX_WorldPrev;
		static float MouseY_World;
		static float MouseY_WorldPrev;
		static float CamX;
		static float CamY;
		static AnimationTrack * pAnimationTrackSet;

		static unsigned long MSSinceLastFrame;
		static float SecondsSinceLastFrame;
		static float uRunTimeSeconds; // I guess this wraps around eventually?
		static unsigned long uRunTime; //Just wraps around
};