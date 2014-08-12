#pragma once
#include <Windows.h>
#include <GdiPlus.h>  // once again, use the powers of GDI+ to do most of the work
#pragma comment (lib,"Gdiplus.lib")

//Using the powers of GDI+ to load / save images.
namespace GDIImageLoader
{
	void Initialize();
	void * LoadImage(const WCHAR * wFilePath, int * pWidth, int * pHeight);
	void ConvertImageToHeightMask(const WCHAR * wFilePath, int HeightMask[], bool bIsCeiling = false);
};

