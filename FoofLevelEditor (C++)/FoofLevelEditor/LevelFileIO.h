#pragma once
#include "LevelScene.h"
#include "PropStruct.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>

// This will deal with saving / opening level files
namespace LevelFileIO{
	bool SaveLevelToDisk(const char * cFilePath, LevelScene * pScene);
	bool OpenLevelFromFile(const char * cFilePath, LevelScene * pScene);
	bool OpenLevelTileSet(const char * cFilePath);
};