#include "WICImageLoader.h"
#include "GDIImageLoader.h"

void WICImageLoader::ConvertImageToHeightMask(const WCHAR * wFilePath, int HeightMask[], bool bIsCeiling){
	IWICBitmapDecoder *pDecoder = NULL;
	IWICImagingFactory *pFactory = NULL;
	if(SUCCEEDED(CoCreateInstance(  CLSID_WICImagingFactory,  NULL,  CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,  (LPVOID*)&pFactory ))){
		if(SUCCEEDED(pFactory->CreateDecoderFromFilename(
			wFilePath,                      // Image to be decoded
			NULL,                            // Do not prefer a particular vendor
			GENERIC_READ,                    // Desired read access to the file
			WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
			&pDecoder                        // Pointer to the decoder
			))){
				IWICBitmapFrameDecode *pFrame = NULL;
				if(SUCCEEDED(pDecoder->GetFrame(0, &pFrame))){
					//We only care about the first frame since it's a PNG not a freaking GIF
					IWICBitmapSource *pConverter = NULL;
					WICConvertBitmapSource(GUID_WICPixelFormat32bppRGBA, pFrame, &pConverter);
					pFrame->Release();
					IWICBitmap *pIBitmap = NULL;
					if(SUCCEEDED(pFactory->CreateBitmapFromSource(
						pConverter,          // Create a bitmap from the image frame
						WICBitmapCacheOnDemand,  // Cache metadata when needed
						&pIBitmap))){             // Pointer to the bitmap

							//Ok so we made the freaking bitmap
							UINT width = 0;
							UINT cbStride = 0;
							UINT height = 0;
							UINT cbBufferSize = 0;
							pConverter->GetSize(&width, &height); 
							UIntMult(width, sizeof(Gdiplus::ARGB), &cbStride);
							UIntMult(cbStride, height, &cbBufferSize);

							BYTE * pBuffer  = new BYTE[cbBufferSize];
							if(SUCCEEDED(pConverter->CopyPixels(NULL,cbStride,cbBufferSize,pBuffer))){
								Gdiplus::Bitmap bmp(
										width,
										height,
										cbStride,
										PixelFormat32bppARGB,
										pBuffer
										);
									const Gdiplus::Rect LockRect(0, 0, width, height); // Lock down the entire image.
									Gdiplus::BitmapData bmpdata;
									bmp.LockBits(&LockRect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpdata);
									//const size_t DataArraySize = bmpdata.Stride * Width * Height;
									UINT * pTexels = (UINT*)bmpdata.Scan0;
									for(int x = 0; x < width; x++){
										if(bIsCeiling){
											for(int y = 0 ; y < height; y++){ // Go in opposite direction
												const int YSkip = y * bmpdata.Stride / 4;
												const UINT TexelColor = pTexels[YSkip + x];
												//Check alpha 
												BYTE Alpha = TexelColor & 0xFF;
												if(Alpha == 0){
													HeightMask[x] = 64 - y - 1;
													break;
												}
											}
										}else{
											for(int y = 0; y < height; y++){
												const int YSkip = y * bmpdata.Stride / 4;
												const UINT TexelColor = pTexels[YSkip + x];
												//Check alpha 
												BYTE Alpha = TexelColor  & 0xFF;;
												if(Alpha > 0){
													HeightMask[x] = 64 - y;
													break;
												}
											}
										}
									}
									bmp.UnlockBits(&bmpdata);
							}

							pIBitmap->Release();
					}
					pConverter->Release();
				}
				pDecoder->Release();
		}
	}

	return;
}