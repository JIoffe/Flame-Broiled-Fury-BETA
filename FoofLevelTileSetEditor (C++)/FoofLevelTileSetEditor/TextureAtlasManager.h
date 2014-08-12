#pragma once
//Will hold UV Coord data for the texture atlas
#include <cstdio>
#include <vector>
#include <string>
class TextureAtlasManager
{

	float * pAtlasUVBuffer;
	std::string * pFileNameBuffer;
	int nTextures;
public:
	std::vector<std::string> TextureList;
	void PushUVsToBuffer(float * pBuffer, int CurrentIndex, int UVIndex);
	int GetIndexByTitle(std::string Title);
	TextureAtlasManager(const char * pPath);
	~TextureAtlasManager(void);
};

