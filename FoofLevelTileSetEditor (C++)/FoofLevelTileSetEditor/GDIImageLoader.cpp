#include "GDIImageLoader.h"

using namespace Gdiplus;

void GDIImageLoader::Initialize(){
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
  
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}
void * GDIImageLoader::LoadImage(const WCHAR * wFilePath, int * pWidth, int * pHeight){
	Bitmap bmp(wFilePath);

	const int Width = bmp.GetWidth();
	const int Height = bmp.GetHeight();

	const Rect LockRect(0, 0, Width, Height); // Lock down the entire image.
	BitmapData bmpdata;
	bmp.LockBits(&LockRect, ImageLockModeRead, PixelFormat32bppARGB, &bmpdata);
	//const size_t DataArraySize = bmpdata.Stride * Width * Height;
	BYTE * pRawImageData = new BYTE[4 * Width * Height];
	UINT * pTexels = (UINT*)bmpdata.Scan0;
	//Have to loop since memcpy just does an access violation.
	for(int y = 0; y < Height; y++){
		const int YSkip = y * bmpdata.Stride / 4;
		for(int x = 0; x < Width; x++){
			const UINT TexelColor = pTexels[YSkip + x];
			//Stored in ARGB, extract and put into RGBA!
			BYTE Alpha = (TexelColor >> 24) & 0xFF;
			BYTE Red = (TexelColor >> 16) & 0xFF;
			BYTE Green = (TexelColor >> 8 ) & 0xFF;
			BYTE Blue = TexelColor & 0xFF;
			const int RawImageIndex = y * Width * 4 + x * 4;
			pRawImageData[RawImageIndex] = Red;
			pRawImageData[RawImageIndex+1] = Green;
			pRawImageData[RawImageIndex+2] = Blue;
			pRawImageData[RawImageIndex+3] = Alpha;
		}
	}
	bmp.UnlockBits(&bmpdata);

	*pWidth = Width;
	*pHeight = Height;
	return (void*)pRawImageData;
}
void GDIImageLoader::ConvertImageToHeightMask(const WCHAR * wFilePath, int HeightMask[], bool bIsCeiling){
	Bitmap bmp(wFilePath);
	Status loadStatus = bmp.GetLastStatus();
	if(loadStatus != Ok){
		//We didn't load it properly, so we're just gonna do all 0s
		return;
	}
	const int Width = bmp.GetWidth();
	const int Height = bmp.GetHeight();

	const Rect LockRect(0, 0, Width, Height); // Lock down the entire image.
	BitmapData bmpdata;
	bmp.LockBits(&LockRect, ImageLockModeRead, PixelFormat32bppARGB, &bmpdata);
	//const size_t DataArraySize = bmpdata.Stride * Width * Height;
	UINT * pTexels = (UINT*)bmpdata.Scan0;
	for(int x = 0; x < Width; x++){
		if(bIsCeiling){
			for(int y = 0 ; y < Height; y++){ // Go in opposite direction
				const int YSkip = y * bmpdata.Stride / 4;
				const UINT TexelColor = pTexels[YSkip + x];
				//Check alpha 
				BYTE Alpha = (TexelColor >> 24) & 0xFF;
				if(Alpha == 0){
					HeightMask[x] = 64 - y - 1;
					break;
				}
			}
		}else{
			for(int y = 0; y < Height; y++){
				const int YSkip = y * bmpdata.Stride / 4;
				const UINT TexelColor = pTexels[YSkip + x];
				//Check alpha 
				BYTE Alpha = (TexelColor >> 24) & 0xFF;
				if(Alpha > 0){
					HeightMask[x] = 64 - y;
					break;
				}
			}
		}
	}
	bmp.UnlockBits(&bmpdata);
	return;
}