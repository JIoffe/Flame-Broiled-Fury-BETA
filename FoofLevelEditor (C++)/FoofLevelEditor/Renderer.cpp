#include "Renderer.h"
#include "GDIImageLoader.h"
#include "PropStruct.h"
#include "Globals.h"
#include "StaticDeco.h"

const float FullTexUV[] = {
						0.0f, 0.0f,
						0.0f, 1.0f,
						1.0f, 1.0f,
						1.0f, 0.0f,
				}; 
const float CollectibleWidths[] = { 0, 0.082f,0.095f,0.095f, 0.08f, 0.08f, 0.08f, 0.08f, 0.1f, 0.082f};
Renderer::Renderer(void)
{
	hDC = NULL;
	hRC = NULL;
	pGridLinesVB = NULL;
	GridPoints = 0;
	SmallPoints = 0;
	MediumPoints = 0;
	pSmallSpotGridVB = NULL;
	pMediumSpotGridVB = NULL;
}


Renderer::~Renderer(void)
{
}
bool Renderer::Initialize(const HWND hwnd, int w, int h){
	GDIImageLoader::Initialize();
	if(!(hDC = GetDC(hwnd))){
		return false;
	}
	PIXELFORMATDESCRIPTOR pfd;
	WORD uSize = sizeof(PIXELFORMATDESCRIPTOR);
	ZeroMemory(&pfd, uSize);
	pfd.nSize = uSize;
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL  | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.cColorBits = 32;

	const int PixelFormat = ChoosePixelFormat(hDC, &pfd);
	if(PixelFormat < 1 || !SetPixelFormat(hDC, PixelFormat, &pfd)){
		return false;
	}
	if (!(hRC=wglCreateContext(hDC)) || !wglMakeCurrent(hDC,hRC)){
		return false;
	}
	Width = w;
	Height = h;
	ResizeFrame(w, h);
	PixelWidth = (Ratio * 2) / Width;
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 


	//Set up some things that are good for a 2D sprite engine...
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glShadeModel(GL_FLAT);
    glDisable(GL_DITHER);
    glDisable(GL_LIGHTING);  // We definitely don't need lighting
    glDisable(GL_DEPTH_TEST);  // or Z-Buffer in 2D
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	glEnable(GL_TEXTURE_2D);

	SetTexture("lala");
	//Use Linear filtering

		
	//Use arrays / VBOs
	glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	return true;
}
void Renderer::PushColor(float R, float G, float B, float A, GLfloat * pColorBuffer, int RenderCount){
	int CBufferIndex = RenderCount * 16;
	for(int i = 0; i < 4; i++){
		pColorBuffer[CBufferIndex++] = R;
		pColorBuffer[CBufferIndex++] = G;
		pColorBuffer[CBufferIndex++] = B;
		pColorBuffer[CBufferIndex++] = A;
	}
	return;
}
void Renderer::PushQuadToVertexBuffer(float Left, float Right, float Top, float Bottom, GLfloat * pVertexBuffer, int RenderCount){
	int VBufferIndex = RenderCount *  12;
	pVertexBuffer[VBufferIndex++] = Left;
	pVertexBuffer[VBufferIndex++] = Top;
	pVertexBuffer[VBufferIndex++] = 1.0f;

	pVertexBuffer[VBufferIndex++] = Left;
	pVertexBuffer[VBufferIndex++] = Bottom;
	pVertexBuffer[VBufferIndex++] = 1.0f;

	pVertexBuffer[VBufferIndex++] = Right;
	pVertexBuffer[VBufferIndex++] = Bottom;
	pVertexBuffer[VBufferIndex++] = 1.0f;

	pVertexBuffer[VBufferIndex++] = Right;
	pVertexBuffer[VBufferIndex++] = Top;
	pVertexBuffer[VBufferIndex] = 1.0f;
	return;
}
void Renderer::Draw(LevelScene * pScene){

	const int QuadStride = 12;
	const int QuadTexStride = 8;

	const int GridWidth = pScene->GetWidth();
	const int GridHeight = pScene->GetHeight();
	const int Area = GridWidth * GridHeight;
	const int nProps = pScene->PropList.size();

	//const float DLeft = pScene->CamX - Ratio;
	//const int StartX = (DLeft <= 0) ? 0 : (int)((DLeft) / LevelScene::LevelTileOffset);
	//const float DRight = CamX + FoofRenderer.ratio;  pScene->
	//const int EndX = (DRight >= pScene->) ? GridWidth : (int)((DRight) / LevelScene::LevelTileOffset) + 1;
	//		
	//const float DDown = CamY - 1.0f;
	//const int StartY = (DDown <= 0) ? 0 : (int)((DDown) / LevelScene::LevelTileOffset);
	//const float DUp = CamY + 1.0f;
	//const int EndY = (DUp >= LevelGridMeasuredHeight) ? LevelGridHeight : (int)((DUp) / LevelScene::LevelTileOffset) + 1;

	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity(); 
	glTranslatef(-Globals::CamX , - Globals::CamY, 0.0f);
	const int nEnemies = pScene->EnemyList.size();
	GLfloat * pVertexBuffer = new GLfloat[(Area + nEnemies) * QuadStride];
	float * pTexBuffer = new float[(Area + nEnemies) * QuadTexStride];
	
	int RenderCount = 0;

	const int StartX = 0; //pScroller->GetXScrollFraction() * GridWidth;
	const int EndX = GridWidth;//StartX + 10;
	const int EndY = GridHeight;//(1.0f - pScroller->GetInvScrollYFraction()) * GridHeight + 1;
	const int StartY = 0;//EndY - 8;
	for(int y = StartY; y < EndY; y++){
		const float Top = (y+1) * LevelScene::LevelTileOffset;
		const float Bottom = y * LevelScene::LevelTileOffset;
		for(int x = StartX; x < EndX; x++){

			const int TileValue = pScene->GetGridUnit(x,y);
			if(TileValue > -1){
				const int SpriteID = Globals::TileSet[TileValue].SpriteID;
				// ORDER - TOP LEFT, BOTTOM LEFT, BOTTOM RIGHT, TOP RIGHT
				const float Left = x * LevelScene::LevelTileOffset;
				const float Right = (x+1) * LevelScene::LevelTileOffset;
				PushQuadToVertexBuffer(Left, Right, Top, Bottom, pVertexBuffer, RenderCount);
				int TexBufferIndex = RenderCount * QuadTexStride;
				if(pTexAtlas){
					if(TileValue != 127){
						pTexAtlas->PushUVsToBuffer(pTexBuffer, TexBufferIndex, SpriteID);
					}else{
							const float pUVs[] = {
										0.0f, 0.0f,
										0.0f, 1.0f,
										1.0f, 1.0f,
										1.0f, 0.0f,
								}; 
						memcpy(pTexBuffer+TexBufferIndex, pUVs, sizeof(float) * 8);
					}
				}

				RenderCount++;
			}
		}
	}
	for(std::vector<PropStruct*>::iterator it = pScene->DecoList.begin(); it < pScene->DecoList.end(); it++){
			PropStruct * pProp = (*it);
			StaticDeco * pDeco = (StaticDeco*)pProp->ExtendedInfo;
			const unsigned int PropID = pProp->uType;
			const float SpriteWidth = pDeco->fDefaultScale;
			const int SpriteID = pDeco->iSpriteID;
			const float Left = pProp->x - SpriteWidth;
			const float Right = pProp->x + SpriteWidth;
			const float Top = pProp->y + SpriteWidth;
			const float Bottom = pProp->y - SpriteWidth;
			PushQuadToVertexBuffer(Left,Right,Top,Bottom,pVertexBuffer,RenderCount);
			pTexAtlas->PushUVsToBuffer(pTexBuffer, RenderCount * QuadTexStride, SpriteID);

			RenderCount++;
	}
	if(nEnemies > 0){
		for(std::vector<PropStruct*>::iterator it = pScene->EnemyList.begin(); it < pScene->EnemyList.end(); it++){
			PropStruct * pProp = (*it);
			const unsigned int PropID = pProp->uType;
			ExtendedObject * pDyn = pProp->ExtendedInfo;
			if(!(pDyn->SubType == 2 && ((Mover*)pProp)->GetFunctionType() == Mover::Fader && ((Mover*)pProp)->Value < 0)){
				float SpriteWidth = pDyn->fDefaultScale;
				int SpriteID = pDyn->iSpriteIndex;
				if(pDyn->SubType == 9){
					SpriteWidth = 0.4f;
					SpriteID+= 1;
				}
				const float Left = pProp->x - SpriteWidth;
				const float Right = pProp->x + SpriteWidth;
				const float Top = pProp->y + SpriteWidth;
				const float Bottom = pProp->y - SpriteWidth;
				PushQuadToVertexBuffer(Left,Right,Top,Bottom,pVertexBuffer,RenderCount);
				pTexAtlas->PushUVsToBuffer(pTexBuffer, RenderCount * QuadTexStride, SpriteID);

				RenderCount++;
			}
		}
	}
	for(std::vector<PropStruct*>::iterator it = pScene->HazardList.begin(); it < pScene->HazardList.end(); it++){
			PropStruct * pProp = (*it);
			HazardousObject * pHaz = (HazardousObject*)pProp->ExtendedInfo;
			const unsigned int PropID = pProp->uType;
			const float SpriteWidth = pHaz->fDefaultScale;
			const int SpriteID = pHaz->SpriteID;
			const float Left = pProp->x - SpriteWidth;
			const float Right = pProp->x + SpriteWidth;
			const float Top = pProp->y + SpriteWidth;
			const float Bottom = pProp->y - SpriteWidth;
			PushQuadToVertexBuffer(Left,Right,Top,Bottom,pVertexBuffer,RenderCount);
			pTexAtlas->PushUVsToBuffer(pTexBuffer, RenderCount * QuadTexStride, SpriteID);

			RenderCount++;
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Texture[0]);
	glVertexPointer(3, GL_FLOAT, 0, pVertexBuffer);
	glTexCoordPointer(2, GL_FLOAT, 0, pTexBuffer);
	glDrawArrays(GL_QUADS, 0, RenderCount * 4);

	delete[] pVertexBuffer;
	delete[] pTexBuffer;
	const unsigned long nTotal = nProps + pScene->BunnyList.size() + pScene->TokenList.size();
	pVertexBuffer = new GLfloat[(nTotal+2) * QuadStride];
	pTexBuffer = new float[(nTotal+2) * QuadTexStride];
	GLfloat * pColorBuffer = new GLfloat[(nTotal+2)*16];
	RenderCount = 0;
	if(nProps > 0){
		for(std::vector<PropStruct*>::iterator it = pScene->PropList.begin(); it < pScene->PropList.end(); it++){
			PropStruct * pProp = (*it);
			const unsigned int PropID = pProp->uType;
			float SpriteWidth = 0.0f;
			int SpriteIndex = 0;
			if(PropID > 0){
				if(PropID == 88){
								SpriteWidth = 0.20f;
				SpriteIndex = Globals::CheckPointIndex;
				}else{
					SpriteWidth = CollectibleWidths[PropID];
					SpriteIndex = Globals::CollectibleID[PropID];
				}
			}
			
				const float Left = pProp->x - SpriteWidth;
				const float Right = pProp->x + SpriteWidth;
				const float Top = pProp->y + SpriteWidth;
				const float Bottom = pProp->y - SpriteWidth;
				PushQuadToVertexBuffer(Left,Right,Top,Bottom,pVertexBuffer,RenderCount);
				pGlobalAtlas->PushUVsToBuffer(pTexBuffer, RenderCount * QuadTexStride, SpriteIndex);
				PushColor(1.0f, 1.0f, 1.0f, 1.0f, pColorBuffer, RenderCount);
				RenderCount++;
		}
	} // End Props
	int Add = 0;
	for(std::vector<PropStruct*>::iterator it = pScene->BunnyList.begin(); it < pScene->BunnyList.end(); it++){
			PropStruct * pProp = (*it);
			const float SpriteWidth = 0.15f;
			const float Left = pProp->x - SpriteWidth;
			const float Right = pProp->x + SpriteWidth;
			const float Top = pProp->y + SpriteWidth;
			const float Bottom = pProp->y - SpriteWidth;
			PushQuadToVertexBuffer(Left,Right,Top,Bottom,pVertexBuffer,RenderCount);
			pGlobalAtlas->PushUVsToBuffer(pTexBuffer, RenderCount * QuadTexStride, Globals::BabyBunnyIndex + Add++);
			PushColor(1.0f, 1.0f, 1.0f, 1.0f, pColorBuffer, RenderCount);
			RenderCount++;
	} //end bunnies
	for(std::vector<PropStruct*>::iterator it = pScene->TokenList.begin(); it < pScene->TokenList.end(); it++){
			PropStruct * pProp = (*it);
			const float SpriteWidth = 0.12f;
			const float Left = pProp->x - SpriteWidth;
			const float Right = pProp->x + SpriteWidth;
			const float Top = pProp->y + SpriteWidth;
			const float Bottom = pProp->y - SpriteWidth;
			PushQuadToVertexBuffer(Left,Right,Top,Bottom,pVertexBuffer,RenderCount);
			pGlobalAtlas->PushUVsToBuffer(pTexBuffer, RenderCount * QuadTexStride, Globals::TokenIndex);
			PushColor(1.0f, 1.0f, 1.0f, 1.0f, pColorBuffer, RenderCount);
			RenderCount++;
	} //end tokens
	//Add exit
	{
		const float ExitScale = 0.3f;
		const float PlayerX = pScene->ExitPos_X; //GetPlayerStartX();
		const float PlayerY = pScene->ExitPos_Y;
		const float Left = PlayerX - ExitScale;
		const float Right = PlayerX + ExitScale;
		const float Top = PlayerY + ExitScale;
		const float Bottom = PlayerY - ExitScale;
		PushQuadToVertexBuffer(Left,Right,Top,Bottom,pVertexBuffer,RenderCount);
		pGlobalAtlas->PushUVsToBuffer(pTexBuffer, RenderCount * QuadTexStride, Globals::ExitIndex, false);
		PushColor(1.0f, 1.0f, 1.0f, 1.0f, pColorBuffer, RenderCount);
		RenderCount++;
	}
	//Add player to top of render queue
	{
		const float PlayerX = pScene->mPlayer.X; //GetPlayerStartX();
		const float PlayerY = pScene->mPlayer.Y;
		const float Left = PlayerX - LevelScene::PlayerWidth;
		const float Right = PlayerX + LevelScene::PlayerWidth;
		const float Top = PlayerY + LevelScene::PlayerWidth;
		const float Bottom = PlayerY - LevelScene::PlayerWidth;
		PushQuadToVertexBuffer(Left,Right,Top,Bottom,pVertexBuffer,RenderCount);
		pGlobalAtlas->PushUVsToBuffer(pTexBuffer, RenderCount * QuadTexStride, pScene->mPlayer.CurrentSprite, pScene->mPlayer.bFlip);
		PushColor(1.0f, 1.0f, 1.0f, 1.0f, pColorBuffer, RenderCount);
		RenderCount++;
	}


	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_COLOR_ARRAY);
	glBindTexture(GL_TEXTURE_2D, Texture[1]);
	glColorPointer(4, GL_FLOAT, 0, pColorBuffer);
	glVertexPointer(3, GL_FLOAT, 0, pVertexBuffer);
	glTexCoordPointer(2, GL_FLOAT, 0, pTexBuffer);
	glDrawArrays(GL_QUADS, 0, RenderCount * 4);


	delete[] pVertexBuffer;
	delete[] pTexBuffer;
	delete[] pColorBuffer;
		//Hover our current prop brush if it exists
	if(Globals::pObjectBrush){
		pVertexBuffer = new GLfloat[QuadStride];
		pTexBuffer = new float[QuadTexStride];
		pColorBuffer = new GLfloat[16];
		const unsigned int PropID = Globals::pObjectBrush->uType;
		if(PropID > 0 && pGlobalAtlas){
			float SpriteWidth = 1.0f;
			int SpriteID = 0;
			TextureAtlasManager * pAtlas = NULL;
			if(PropID < 10){
				SpriteWidth = CollectibleWidths[PropID];
				SpriteID = Globals::CollectibleID[PropID];
				pAtlas = pGlobalAtlas;
			}else{
				const int DynID = 0;
				if(Globals::pObjectBrush->ExtendedInfo->Type == 1){
					ExtendedObject * pDyn = Globals::pObjectBrush->ExtendedInfo;
					SpriteWidth = pDyn->fDefaultScale;
					SpriteID = pDyn->iSpriteIndex;
				}else if(Globals::pObjectBrush->ExtendedInfo->Type == 2){
					HazardousObject * pHaz = (HazardousObject*)Globals::pObjectBrush->ExtendedInfo;
					SpriteWidth = pHaz->fDefaultScale;
					SpriteID = pHaz->SpriteID;
				}else if(Globals::pObjectBrush->ExtendedInfo->Type == 3){
					StaticDeco * pDeco = (StaticDeco*)Globals::pObjectBrush->ExtendedInfo;
					SpriteWidth = pDeco->fDefaultScale;
					SpriteID = pDeco->iSpriteID;
				}
					glBindTexture(GL_TEXTURE_2D, Texture[0]);
					pAtlas = pTexAtlas;
				
			}
			const float Left = Globals::pObjectBrush->x - SpriteWidth;
			const float Right = Globals::pObjectBrush->x + SpriteWidth;
			const float Top = Globals::pObjectBrush->y + SpriteWidth;
			const float Bottom = Globals::pObjectBrush->y - SpriteWidth;
			PushQuadToVertexBuffer(Left,Right,Top,Bottom,pVertexBuffer,0);
			pAtlas->PushUVsToBuffer(pTexBuffer, 0, SpriteID);
			PushColor(1.0f, 1.0f, 1.0f, 0.5f, pColorBuffer, 0);
			
			glColorPointer(4, GL_FLOAT, 0, pColorBuffer);
			glVertexPointer(3, GL_FLOAT, 0, pVertexBuffer);
			glTexCoordPointer(2, GL_FLOAT, 0, pTexBuffer);
			glDrawArrays(GL_QUADS, 0, 4);

			delete[] pVertexBuffer;
			delete[] pTexBuffer;
			delete[] pColorBuffer;
		}
	}
	glDisableClientState(GL_COLOR_ARRAY);
	//Draw camera icons if they exist
	const int nCameras = pScene->CamList.size();
	if(nCameras > 0){
		glBindTexture(GL_TEXTURE_2D, Texture[2]);
		RenderCount = 0;
		pVertexBuffer = new GLfloat[nCameras * QuadStride];
		pTexBuffer = new float[QuadTexStride * nCameras];
		for(std::vector<CamZone*>::iterator it = pScene->CamList.begin(); it < pScene->CamList.end(); it++){
			CamZone * pCam = *it;
			const float Left = pCam->x - CamZone::Scale;
			const float Right = pCam->x + CamZone::Scale;
			const float Top = pCam->y + CamZone::Scale;
			const float Bottom = pCam->y - CamZone::Scale;
			PushQuadToVertexBuffer(Left,Right,Top,Bottom,pVertexBuffer,RenderCount);
			memcpy(pTexBuffer + (RenderCount * QuadTexStride), FullTexUV, sizeof(float) * 8);
			RenderCount++;
		}
		glVertexPointer(3, GL_FLOAT, 0, pVertexBuffer);
		glTexCoordPointer(2, GL_FLOAT, 0, pTexBuffer);
		glDrawArrays(GL_QUADS, 0, RenderCount*4);


		delete[] pVertexBuffer;
		delete[] pTexBuffer;
	}
	glDisable(GL_TEXTURE_2D);

	//Draw borders around selected objects if they exist

	const int nSelectedObjects = Globals::SelectedObjects.size();
	if(nSelectedObjects > 0){
		RenderCount = 0;
		pVertexBuffer = new GLfloat[QuadStride * nSelectedObjects];
		for(std::vector<PropStruct*>::iterator it = Globals::SelectedObjects.begin(); it < Globals::SelectedObjects.end(); it++){
			const PropStruct * pObject = *it;
			const int PropID = pObject->uType;
			float SpriteWidth = 0.0f;
			if(pObject->uObjectType == PropStruct::Collectible){
				SpriteWidth = CollectibleWidths[PropID];
			}else if(pObject->uObjectType == PropStruct::CameraZone){
				SpriteWidth = CamZone::Scale;

				//draw the area
				CamZone * pCam = (CamZone*)pObject;
				glBegin(GL_QUADS);
				glColor4f(0.3f, 1.0f, 0.0f, 0.5f);
				glVertex3f(pCam->Left, pCam->Up, 0.0f);
				glVertex3f(pCam->Left, pCam->Down, 0.0f);
				glVertex3f(pCam->Right, pCam->Down, 0.0f);
				glVertex3f(pCam->Right, pCam->Up, 0.0f);
				glEnd();
			} else if(PropID == PropStruct::Bunny || PropID == PropStruct::Token){
			SpriteWidth = 0.10f;
			} else{
				SpriteWidth = pObject->ExtendedInfo->fDefaultScale;
			}
			const float Left = pObject->x - SpriteWidth;
			const float Right = pObject->x + SpriteWidth;
			const float Top = pObject->y + SpriteWidth;
			const float Bottom = pObject->y - SpriteWidth;
			PushQuadToVertexBuffer(Left,Right,Top,Bottom,pVertexBuffer,RenderCount++);
		}
		glVertexPointer(3,GL_FLOAT,0,pVertexBuffer);
		glColor4f(0.7f, 0.4f, 1.0f, 0.5f);
		glDrawArrays(GL_QUADS, 0, RenderCount * 4);
		delete[] pVertexBuffer;
	}
	//Draw Lines
	glColor3f(1.0f, 1.0f, 1.0f);
	if(Globals::bDrawGrid){
		if(pGridLinesVB){
			glVertexPointer(3, GL_FLOAT, 0, pGridLinesVB);
			glDrawArrays(GL_LINES, 0, GridPoints);
		}
		if(pMediumSpotGridVB){
			glVertexPointer(3, GL_FLOAT, 0, pMediumSpotGridVB);
			glDrawArrays(GL_POINTS, 0, MediumPoints);
		}
	}

	SwapBuffers(hDC);

	return;
}
void Renderer::ResizeFrame(const GLsizei w, const GLsizei h){
	glViewport(0,0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	Ratio = (float)w/h;
	gluOrtho2D(-Ratio, Ratio, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return;
}
void Renderer::SetTexture(const char * cTexPath){
	int Width, Height;
	//void * ImageData = GDIImageLoader::LoadImage(L"C:/Users/Malvina/workspace/FoofGame/res/drawable-hdpi/forestlevel.png", &Width, &Height);
	void * ImageData = GDIImageLoader::LoadImage(L"C:/Users/Malvina/workspace/FoofGame/res/drawable-hdpi/mechlevel.png", &Width, &Height);
	glGenTextures(3, Texture); //Generate room for 1 texture. (We only need one...)
	if(ImageData){
		
		glBindTexture(GL_TEXTURE_2D, Texture[0]); //Bind it to the pipeline. Once and for all.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ImageData);
		delete[] ImageData;
		ImageData = NULL;
	}
	ImageData = GDIImageLoader::LoadImage(L"C:/Users/Malvina/workspace/FoofGame/res/drawable-hdpi/gameglobal.png", &Width, &Height);
	if(ImageData){
		glBindTexture(GL_TEXTURE_2D, Texture[1]); //Bind it to the pipeline. Once and for all.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, 4, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ImageData);
		delete[] ImageData;
	}
		ImageData = GDIImageLoader::LoadImage(L"./CameraIcon1.png", &Width, &Height);
	if(ImageData){
		glBindTexture(GL_TEXTURE_2D, Texture[2]); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, 4, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ImageData);
		delete[] ImageData;
	}
	return;
}
void Renderer::RebuildGrid(int Width, int Height){
	if(pGridLinesVB != NULL){
		delete[] pGridLinesVB;
	}
	GridPoints = (Width * 2 + Height * 2);
	pGridLinesVB = new GLfloat[GridPoints * 3];
	int Index = 0;
	const float MaxY = Height * LevelScene::LevelTileOffset;
	const float MaxX = Width * LevelScene::LevelTileOffset;
	for(int y = 0; y < Height; y++){
		float YCoord = LevelScene::LevelTileOffset * y;
		pGridLinesVB[Index++] = 0.0f;
		pGridLinesVB[Index++] = YCoord;
		pGridLinesVB[Index++] = 0.0f;

		pGridLinesVB[Index++] = MaxX;
		pGridLinesVB[Index++] = YCoord;
		pGridLinesVB[Index++] = 0.0f;
	}
	for(int x = 0; x < Width; x++){
		float XCoord = LevelScene::LevelTileOffset * x;
		pGridLinesVB[Index++] = XCoord;
		pGridLinesVB[Index++] = 0.0f;
		pGridLinesVB[Index++] = 0.0f;

		pGridLinesVB[Index++] = XCoord;
		pGridLinesVB[Index++] = MaxY;
		pGridLinesVB[Index++] = 0.0f;
	}

	// Make Medium Grid
	const float MediumWidth = Width * 2 + 1;
	const float MediumHeight = Height * 2 + 1;
	MediumPoints = MediumWidth * MediumHeight;
	pMediumSpotGridVB = new GLfloat[MediumPoints * 3];
	Index = 0;
	const float MediumOffset = LevelScene::LevelTileOffset / 2.0f;
	for(int x = 0; x < MediumWidth; x++){
		const float XCoord = x * MediumOffset;
		for(int y = 0; y < MediumHeight; y++){
			pMediumSpotGridVB[Index++] = XCoord;
			pMediumSpotGridVB[Index++] = y * MediumOffset;
			pMediumSpotGridVB[Index++] = 0.0f;
		}
	}
	return;
}
float Renderer::GetRatio(){
	return Ratio;
}
void Renderer::AssociateScroller(ScrollingComponent * pNewScroller){
	pScroller = pNewScroller;
	return;
}
void Renderer::AssociateTextureAtlas(TextureAtlasManager * pNewAtlas){
	pTexAtlas = pNewAtlas;
	return;
}
void Renderer::AssociateGlobalTextureAtlas(TextureAtlasManager * pNewAtlas){
	pGlobalAtlas = pNewAtlas;
	return;
}
float Renderer::GetPixelWidth(){
	return PixelWidth;
}