#include "TextureAtlasManager.h"
#include "Globals.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cstdio>

using namespace std;

TextureAtlasManager::TextureAtlasManager(const char * pPath)
{
	nTextures = 0;
	pFileNameBuffer = NULL;
	//Just load the default in the constructor.
	ifstream FileIn(pPath);
	string LineReader;
	FileIn >> LineReader;
	if(LineReader == "Foof72"){
		FileIn >> nTextures;
		pAtlasUVBuffer = new float[nTextures * 4];
		for(int i = 0; i < nTextures; i++){
			FileIn >> LineReader;
			TextureList.push_back(LineReader);
			//SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)LineReader.c_str());
			const int ifour = i*4; 
			FileIn >> pAtlasUVBuffer[ifour];
			FileIn >> pAtlasUVBuffer[ifour+1];
			FileIn >> pAtlasUVBuffer[ifour+2];
			FileIn >> pAtlasUVBuffer[ifour+3];
		}
	}
	FileIn.close();
}

void TextureAtlasManager::PushUVsToBuffer(float * pBuffer, int CurrentIndex, int UVIndex){
	if(pAtlasUVBuffer && UVIndex < nTextures){
		const int iFour = UVIndex*4;
		const float Left = pAtlasUVBuffer[iFour];
		const float Top = pAtlasUVBuffer[iFour+1];
		const float Right = pAtlasUVBuffer[iFour+2];
		const float Bottom = pAtlasUVBuffer[iFour+3];
		const float pUVs[] = {
						Left, Top,
						Left, Bottom,
						Right, Bottom,
						Right, Top,
				}; 
		memcpy(pBuffer+CurrentIndex, pUVs, sizeof(float) * 8);
	}
	return;
}
TextureAtlasManager::~TextureAtlasManager(void)
{
}
int TextureAtlasManager::GetIndexByTitle(std::string Title){
	const int Size = TextureList.size();
	for(int i = 0; i < Size; i++){
		if(TextureList[i] == Title){
			return i;
		}
	}
	return -1; //nout found
}