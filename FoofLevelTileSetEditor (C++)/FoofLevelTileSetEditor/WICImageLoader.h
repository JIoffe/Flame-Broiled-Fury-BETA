#pragma once
#include<Wincodec.h>
#pragma comment (lib,"Windowscodecs.lib")
namespace WICImageLoader{
	void Initialize();
	void ConvertImageToHeightMask(const WCHAR * wFilePath, int HeightMask[], bool bIsCeiling = false);
};