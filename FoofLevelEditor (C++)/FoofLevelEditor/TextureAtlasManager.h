#pragma once
//Will hold UV Coord data for the texture atlas
#include <cstdio>
#include <vector>
#include <string>
class TextureAtlasManager
{
	std::vector<std::string> TextureList;
	float * pAtlasUVBuffer;
	std::string * pFileNameBuffer;
	int nTextures;
public:
	void PushUVsToBuffer(float * pBuffer, int CurrentIndex, int UVIndex, bool bFlip = false);
	int GetIndexByTitle(std::string Title);
	TextureAtlasManager(const char * pPath);
	~TextureAtlasManager(void);
};

