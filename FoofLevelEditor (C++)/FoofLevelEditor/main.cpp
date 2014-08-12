/*
/    Level Editor for foofles Android OS Game Engine
/    Supports level tiles as well  as parallax layers, player start, props, etc.
/
/*/
#include <Windows.h>
#include <commctrl.h>
#include <Commdlg.h>
#include "resource.h"
#include "Renderer.h"
#include "LevelScene.h"
#include "TextureAtlasManager.h"
#include "FoofWin32Util.h"
#include "ToolBoxWindow.h"
#include "StaticDeco.h"

#include "Globals.h"
#include "PropStruct.h"
#include "CamZone.h"
#include "LevelFileIO.h"
#include "Player.h"

#define PLACE_TOKEN 20355
#define PLACE_BUNNY 20466
#define PLACE_CHECKPOINT 20200
#define PLACE_EXIT 20345
#define PLACE_CAMERA 20111
#define PLACE_PLAYERSTART 4004
#define OBJ_PROPERTIES 20112

#pragma comment (lib,"Comctl32.lib ")
#pragma comment (lib,"Comdlg32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

static const int ChargingEnemyA = 0;
static const int ChargingEnemyB = 1;
static const int SidewayShooter = 2;
static const int VerticalShooter = 3;
static const int HorizontalMover = 4;
static const int VerticalMover = 5;

unsigned long Globals::uRunTime = 0;
float Globals::uRunTimeSeconds = 0.0f;
unsigned long FrameTime = 0;
unsigned long PrevFrameTime = 0;

bool bRun = true;
unsigned long Globals::MSSinceLastFrame = 0;
float Globals::SecondsSinceLastFrame = 0;
static const char uSnapLargeGrid = 1;
static const char uSnapMediumGrid = 2;
static const char uSnapSmallGrid = 3;
HINSTANCE hAppInstance = NULL;
LevelTile * Globals::TileSet = NULL;
StaticDeco * Globals::DecoSet = NULL;
int Globals::nDecos = 0;
ExtendedObject ** Globals::DynamicSet = NULL;
int Globals::nHazards = 0;
HazardousObject * Globals::HazardousSet = NULL;
bool Globals::bDrawGrid = true; 
float Globals::CamX = 0.0f;
float Globals::CamY = 0.0f;
AnimationTrack * Globals::pAnimationTrackSet = NULL;
bool Globals::bPreviewPlay = false;
int Globals::nDynamics = 0;
int Globals::nLevelTileTypes = 0;
int Globals::EnemyA_ID = 0;
int Globals::Coin_ID = 0;
int Globals::Gem_ID = 0;
int Globals::CollectibleID[10];
int Globals::CheckPointIndex = 0;
int Globals::BabyBunnyIndex = 0;
int Globals::CageIndex = 0;
int Globals::TokenIndex = 0;
int Globals::ExitIndex = 0;
PropStruct * Globals::pObjectBrush = NULL;
std::vector<PropStruct*> Globals::SelectedObjects;
int Width = 640;
int Height = 480;
unsigned char Globals::uSnapGridType = uSnapMediumGrid;
float Globals::MouseX_World = 0.0f;
float Globals::MouseY_World = 0.0f;
float Globals::MouseX_WorldPrev = 0.0f;
float Globals::MouseY_WorldPrev = 0.0f;
float Globals::fGravity = 0.0f;
float CamZone::Scale = 0.09f;

Renderer hRenderer;
ScrollingComponent Scroller;
LevelScene Scene;

//TextureAtlasManager TextureAtlas("C:/Users/Malvina/workspace/FoofGame/res/raw/forestlevel.uv");
TextureAtlasManager TextureAtlas("C:/Users/Malvina/workspace/FoofGame/res/raw/mechlevel.uv");
TextureAtlasManager GlobalTextureAtlas("C:/Users/Malvina/workspace/FoofGame/res/raw/gameglobal.uv");
ToolBoxWindow * pToolBox = NULL;

int RClickX = 0;
int RClickY = 0;
const float CollectibleWidths[] = { 0, 0.082f,0.095f, 0.082f};



using namespace FoofWin32Util;
using namespace std;

HWND hPropertiesWindow = NULL;

vector<PropStruct*> Clipboard;

void DeselectAll(){
	if(Globals::SelectedObjects.size() > 0){
		for(std::vector<PropStruct*>::iterator it = Globals::SelectedObjects.begin(); it < Globals::SelectedObjects.end(); it++){
			PropStruct * pObject = *it;
			pObject->bIsSelected = false;
		}
		Globals::SelectedObjects.clear();
	}
}
void DeleteObject(PropStruct * pProp){
	if(pProp->uObjectType == PropStruct::Collectible || pProp->uObjectType == PropStruct::CheckPoint ){
		const int LastProp = Scene.PropList.size() - 1;
		if(pProp->nID != LastProp){
			Scene.PropList[pProp->nID] = Scene.PropList[LastProp];
			Scene.PropList[pProp->nID]->nID = pProp->nID;
		}
		Scene.PropList.pop_back();
	}else if(pProp->uObjectType == PropStruct::CameraZone){
		const int LastProp = Scene.CamList.size() - 1;
		if(pProp->nID != LastProp){
			Scene.CamList[pProp->nID] = Scene.CamList[LastProp];
			Scene.CamList[pProp->nID]->nID = pProp->nID;
		}
		Scene.CamList.pop_back();
	}else if(pProp->uType == PropStruct::Token){
		const int LastProp = Scene.TokenList.size() - 1;
		if(pProp->nID != LastProp){
			Scene.TokenList[pProp->nID] = Scene.TokenList[LastProp];
			Scene.TokenList[pProp->nID]->nID = pProp->nID;
		}
		Scene.TokenList.pop_back();
	}else if(pProp->uType == PropStruct::Bunny){
		const int LastProp = Scene.BunnyList.size() - 1;
		if(pProp->nID != LastProp){
			Scene.BunnyList[pProp->nID] = Scene.BunnyList[LastProp];
			Scene.BunnyList[pProp->nID]->nID = pProp->nID;
		}
		Scene.BunnyList.pop_back();
	}else{
		if(pProp->ExtendedInfo->Type == 1){
			const int LastProp = Scene.EnemyList.size() - 1;
			if(pProp->nID != LastProp){
				Scene.EnemyList[pProp->nID] = Scene.EnemyList[LastProp];
				Scene.EnemyList[pProp->nID]->nID = pProp->nID;
			}
			Scene.EnemyList.pop_back();
		}else if(pProp->ExtendedInfo->Type == 2){
			const int LastProp = Scene.HazardList.size() - 1;
			if(pProp->nID != LastProp){
				Scene.HazardList[pProp->nID] = Scene.HazardList[LastProp];
				Scene.HazardList[pProp->nID]->nID = pProp->nID;
			}
			Scene.HazardList.pop_back();
		}
		
	}
	delete pProp;
	return;
}
bool CheckMouseSelection(std::vector<CamZone*> * pList, WPARAM wParam){
	bool Hit = false;
	for(vector<CamZone*>::iterator it = pList->begin(); it < pList->end(); it++){
		PropStruct * pObject = (*it);
		const int PropID = pObject->uType;
		float SpriteWidth = CamZone::Scale;
		if(Globals::MouseX_World > (pObject->x - SpriteWidth) && Globals::MouseX_World < (pObject->x + SpriteWidth)
			&& Globals::MouseY_World > (pObject->y - SpriteWidth) && Globals::MouseY_World < (pObject->y + SpriteWidth)){
				Hit = true;
				if(pObject->bIsSelected){
					break;
				}else{
					if(!(wParam & MK_SHIFT)){
						DeselectAll();
					}
					pObject->bIsSelected = true;
					Globals::SelectedObjects.push_back(pObject);
					break;
				}
		}
	}
	return Hit;
}
bool CheckMouseSelection(std::vector<PropStruct*> * pList, WPARAM wParam){
	bool Hit = false;
	for(vector<PropStruct*>::iterator it = pList->begin(); it < pList->end(); it++){
		PropStruct * pObject = (*it);
		const int PropID = pObject->uObjectType;
		float SpriteWidth = 0.0f;
		if(PropID == PropStruct::Collectible){
			SpriteWidth = CollectibleWidths[PropID];
		}else if(PropID == PropStruct::CheckPoint){
			SpriteWidth = 0.20f;
		}else if(PropID == PropStruct::Bunny || PropID == PropStruct::Token){
			SpriteWidth = 0.10f;
		}else{
			SpriteWidth = pObject->ExtendedInfo->fDefaultScale;
		}
		if(Globals::MouseX_World > (pObject->x - SpriteWidth) && Globals::MouseX_World < (pObject->x + SpriteWidth)
			&& Globals::MouseY_World > (pObject->y - SpriteWidth) && Globals::MouseY_World < (pObject->y + SpriteWidth)){
				Hit = true;
				if(pObject->bIsSelected){
					break;
				}else{
					if(!(wParam & MK_SHIFT)){
						DeselectAll();
					}
					pObject->bIsSelected = true;
					Globals::SelectedObjects.push_back(pObject);
					break;
				}
		}
	}
	return Hit;
}
void UpdateGrid(int GridWidth, int GridHeight){
	hRenderer.RebuildGrid(GridWidth,GridHeight);
	//Scene.ResetScene();
	const float DoubleRatio = hRenderer.GetRatio() * 2;
	const float SceneWidth = Scene.GetWidth() * LevelScene::LevelTileOffset;
	Scene.SceneWidthPixels = (SceneWidth/DoubleRatio) * Width;
	int ScrollMax = Scene.SceneWidthPixels;
	UINT nPage = Width;
	Scroller.UpdateScrollbar(SB_HORZ, 0, ScrollMax, nPage);
	const float SceneHeight = Scene.GetHeight() * LevelScene::LevelTileOffset;
	Scene.SceneHeightPixels = (SceneHeight/2.0f) * Height;
	ScrollMax = Scene.SceneHeightPixels;
	nPage = Height;
	Scroller.UpdateScrollbar(SB_VERT, 0, ScrollMax, nPage);
	return;
}
void NewGrid(int GridWidth, int GridHeight){
	//New Grid!
	Scene.InitializeGrid(GridWidth,GridHeight);
	UpdateGrid(GridWidth, GridHeight);
	Scene.ResetScene();
	return;
}
void ResetTimestep(){
	PrevFrameTime = GetTickCount();
	Globals::uRunTime = 0;
}
void UpdateFrametime(){
	FrameTime = GetTickCount();
	const DWORD DeltaTime = FrameTime - PrevFrameTime;
	Globals::MSSinceLastFrame = DeltaTime;
	Globals::uRunTime += DeltaTime;
	Globals::SecondsSinceLastFrame = (float)DeltaTime / 1000.0f;
	Globals::uRunTimeSeconds += Globals::SecondsSinceLastFrame;
	Globals::fGravity = -9.4f * Globals::SecondsSinceLastFrame;
}
void DrawBlock(int LookupX, int LookupY, int CurrentMouseBehavior){
	if(LookupX > -1 && LookupX < Scene.GetWidth()
		&& LookupY > -1 && LookupY < Scene.GetHeight()
		&& Scene.GridOk()){

			int CurrentBrushImage = pToolBox->GetBrushImageID() - 1;
			if(CurrentBrushImage == -1){
				CurrentBrushImage = 127;
			}
			if(CurrentBrushImage > -1){
				if(CurrentMouseBehavior == 1){
					Scene.SetGridUnit(LookupX, LookupY, CurrentBrushImage);
				}else if(CurrentMouseBehavior == 2){
					if(Scene.GetGridUnit(LookupX, LookupY) < 0){
						Scene.SetGridUnit(LookupX, LookupY, CurrentBrushImage);
					}
				}else{
					Scene.SetGridUnit(LookupX, LookupY, -1);
				}
			}
	}
}
void BucketFill(int LookupX, int LookupY, int TileID){
	if(	LookupX > -1 && LookupX < Scene.GetWidth()
		&& LookupY > -1 && LookupY < Scene.GetHeight()
		&& Scene.GridOk() && Scene.GetGridUnit(LookupX, LookupY) == TileID){
			DrawBlock(LookupX, LookupY, 1);
			const int Left = LookupX - 1;
			const int Right = LookupX + 1;
			const int Up = LookupY + 1;
			const int Down = LookupY - 1;

			BucketFill(Left, LookupY, TileID);
			BucketFill(Right, LookupY, TileID);
			BucketFill(LookupX, Up, TileID);
			BucketFill(LookupX, Down, TileID);
	}
	return;
}
void HorizontalLine(int LookupX, int LookupY){
	if(	LookupX > -1 && LookupX < Scene.GetWidth()
		&& LookupY > -1 && LookupY < Scene.GetHeight()
		&& Scene.GridOk() && Scene.GetGridUnit(LookupX, LookupY) < 0){
			DrawBlock(LookupX, LookupY, 1);
			const int Left = LookupX - 1;
			const int Right = LookupX + 1;
			HorizontalLine(Left, LookupY);
			HorizontalLine(Right,LookupY);
	}
}
void VerticalLine(int LookupX, int LookupY){
	if(	LookupX > -1 && LookupX < Scene.GetWidth()
		&& LookupY > -1 && LookupY < Scene.GetHeight()
		&& Scene.GridOk() && Scene.GetGridUnit(LookupX, LookupY) < 0){
			DrawBlock(LookupX, LookupY, 1);
			const int Up = LookupY + 1;
			const int Down = LookupY - 1;
			VerticalLine(LookupX, Up);
			VerticalLine(LookupX, Down);
	}
}
BOOL CALLBACK PropertiesDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam){
	switch(Message){
		case WM_CLOSE:{
			DestroyWindow(hwnd);
			break;
		}
		case WM_INITDIALOG:{
			if(Globals::SelectedObjects.size() == 1){
				switch(Globals::SelectedObjects[0]->ExtendedInfo->SubType){
					case 2:{ //Move{
						Mover * pMover = (Mover*)Globals::SelectedObjects[0];
						HWND hDirectionCombo = GetDlgItem(hwnd, IDC_MOVERDIR);
						SendMessage(hDirectionCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Horizontal"); 
						SendMessage(hDirectionCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Vertical"); 
						SendMessage(hDirectionCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Fader"); 
						SendMessage(hDirectionCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Slammer"); 
						SendMessage(hDirectionCombo, (UINT) CB_SETCURSEL, 0, pMover->GetMoverType());

						SetDlgItemFloat(hwnd, IDC_SPEED, pMover->GetSpeed());
						SetDlgItemFloat(hwnd, IDC_SYNC, pMover->Sync);
						SetDlgItemFloat(hwnd, IDC_WIDTH, pMover->GetWidth());
						SetDlgItemFloat(hwnd, IDC_PAUSE, pMover->GetPause());
						break;
					}
					default:
						break;
				}
			}else{
				DestroyWindow(hwnd);
			}
			break;
		}
        case WM_COMMAND:{
			if(LOWORD(wParam) == IDOK){
				switch(Globals::SelectedObjects[0]->ExtendedInfo->SubType){
					case 2:{ //Move{
						Mover * pMover = (Mover*)Globals::SelectedObjects[0];
						HWND hDirectionCombo = GetDlgItem(hwnd, IDC_MOVERDIR);
						pMover->SetMoverType(SendMessage(hDirectionCombo,(UINT) CB_GETCURSEL,(WPARAM) 0,(LPARAM)0)); 

						pMover->SetSpeed(GetDlgItemFloat(hwnd, IDC_SPEED));
						pMover->Sync = GetDlgItemFloat(hwnd, IDC_SYNC);
						pMover->SetWidth(GetDlgItemFloat(hwnd, IDC_WIDTH));
						pMover->SetPause(GetDlgItemFloat(hwnd, IDC_PAUSE));
						break;
					}
					default:
						break;
				}
			}
			break;
		}
		default:
			return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK NewDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam){
    switch(Message)    {

        case WM_COMMAND:{
            switch(LOWORD(wParam)){
                case IDOK:{
					const int Width = GetDlgItemInt(hwnd, IDC_LVLWIDTH, NULL, FALSE);
					const int Height = GetDlgItemInt(hwnd, IDC_LVLHEIGHT, NULL, FALSE);
					NewGrid(Width, Height);
                    EndDialog(hwnd, IDOK);
					break;
				}
                case IDCANCEL:{
                    EndDialog(hwnd, IDCANCEL);
					break;
				}
            }
			break;
		}
        default:
            return FALSE;
    }
    return TRUE;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE:{
            DestroyWindow(hwnd);
			break;
		}
        case WM_DESTROY:{
			bRun = false;
            PostQuitMessage(0);
			break;
		}
		case WM_KEYUP:{
			if(wParam == VK_LEFT){
				Player::MOVE_LEFT = false;
			}else if(wParam == VK_RIGHT){
				Player::MOVE_RIGHT = false;
			}
			break;
		}
		case WM_KEYDOWN:{
			if(wParam == VK_LEFT){
				Player::MOVE_LEFT = true;
				Player::MOVE_RIGHT = false;
			}else if(wParam == VK_RIGHT){
				Player::MOVE_RIGHT = true;
				Player::MOVE_LEFT = false;
			}else if(wParam == VK_SPACE){
				if(Player::JumpCount < 2){
					Player::YVelocity = Player::JumpVelocity;
					Player::JumpCount++;
				}
			}
			break;
		}
		case WM_COMMAND:{
			switch(LOWORD(wParam)){
				case IDM_QUIT1:{
					DestroyWindow(hwnd);
					break;
				}
				case IDM_COPY1:{
					//TODO ALLOCATE NEW ONES! REFERENCING OLD JUST MEANS WE FUCKING HAVE A ROBLEM
					if(Globals::SelectedObjects.size() > 0){
						if(Clipboard.size() > 0){
							for(vector<PropStruct*>::iterator it = Clipboard.begin(); it < Clipboard.end(); it++){
								delete *it;
							}
							Clipboard.clear();
						}
						for(vector<PropStruct*>::iterator it = Globals::SelectedObjects.begin(); it < Globals::SelectedObjects.end(); it++){
							const size_t PropSize = sizeof(*(*it));
							PropStruct * pProp = NULL;
							(*it)->Duplicate(&pProp);
							if(pProp)
								Clipboard.push_back(pProp);
						}
					}
					break;
				}
				case IDM_PASTE1:{
					if(Clipboard.size() > 0){
						int GridWidth = Scene.GetWidth();
						int GridHeight = Scene.GetHeight();
						float MouseXWorld = ((float)RClickX / Scene.SceneWidthPixels) * (GridWidth * LevelScene::LevelTileOffset) + Globals::CamX - hRenderer.GetRatio();
						float MouseYWorld = ((float)(Height - RClickY)  / Scene.SceneHeightPixels) * (GridHeight * LevelScene::LevelTileOffset) + Globals::CamY - 1.0f;

						if(Globals::uSnapGridType == uSnapMediumGrid){
							const float MedOffset = LevelScene::LevelTileOffset / 2;
							const float SmallOffset = MedOffset / 2;
							MouseXWorld = (int)((MouseXWorld + SmallOffset) / MedOffset) * MedOffset;
							MouseYWorld = (int)((MouseYWorld + SmallOffset) / MedOffset) * MedOffset;
						}else if(Globals::uSnapGridType == uSnapSmallGrid){
							const float SmallOffset = LevelScene::LevelTileOffset / 4;
							const float MicroOffset = SmallOffset / 2;
							MouseXWorld = (int)((MouseXWorld + MicroOffset) / SmallOffset) * SmallOffset;
							MouseYWorld = (int)((MouseYWorld + MicroOffset) / SmallOffset) * SmallOffset;
						}
						float DeltaX = MouseXWorld - Clipboard[0]->x;
						float DeltaY = MouseYWorld - Clipboard[0]->y;
						for(vector<PropStruct*>::iterator it = Clipboard.begin(); it < Clipboard.end(); it++){
							PropStruct * pSrc = *it;
							PropStruct * pProp = NULL;

							pSrc->Duplicate(&pProp);
							pProp->x += DeltaX;
							pProp->y += DeltaY;
							pProp->bIsSelected = false;
							const int CurrentProp = pProp->ExtendedInfo->Type;//pToolBox->GetBrushImageID() + 1;

							pProp->nID = Scene.EnemyList.size();
							pProp->uObjectType = PropStruct::Enemy;
							if(pProp->ExtendedInfo->SubType == 2){
								//If we're a mover, we need to log hte original X and Y values.
								Mover * pMover = (Mover*)pProp;
								pMover->OriginX = pMover->x;
								pMover->OriginY = pMover->y;
							}
							Scene.EnemyList.push_back(pProp);
						}
					}
					break;
				}
				case IDM_NEW___1:{
					DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_NEWLEVEL), hwnd, NewDlgProc);
					break;
				}
				case IDM_TOGGLE_GRID1:{
					Globals::bDrawGrid = !Globals::bDrawGrid;
					break;
				}
				case IDM_PROPERTIES1:{
					if(Globals::SelectedObjects.size() == 1){
						const PropStruct * pSelected = Globals::SelectedObjects[0];
						if(pSelected->ExtendedInfo->Type == 1){
							if(hPropertiesWindow){
								DestroyWindow(hPropertiesWindow);
							}
							switch(Globals::SelectedObjects[0]->ExtendedInfo->SubType){
								case 2:{
									hPropertiesWindow = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MOVEREDIT), hwnd, PropertiesDlgProc);
									break;
								}
								default:
									break;
							}
							if(hPropertiesWindow){
								ShowWindow(hPropertiesWindow, SW_SHOW);
							}
						}
					}
					break;
				}
				case IDM_DELETE1:{
					if(Globals::SelectedObjects.size() > 0){
						for(std::vector<PropStruct*>::iterator it = Globals::SelectedObjects.begin(); it < Globals::SelectedObjects.end(); it++){
							DeleteObject(*it);
						}
						Globals::SelectedObjects.clear();
					}
					break;
				}
				case IDM_DESELECT_ALL1:{
					DeselectAll();
					break;
				}
				case IDM_SMALL_GRID1:{
					Globals::uSnapGridType = uSnapSmallGrid;
					CheckMenuItem(GetMenu(hwnd), IDM_SMALL_GRID1, MF_CHECKED);
					CheckMenuItem(GetMenu(hwnd), IDM_MEDIUM_GRID1, MF_UNCHECKED);
					CheckMenuItem(GetMenu(hwnd), IDM_LARGE_GRID1, MF_UNCHECKED);
					CheckMenuItem(GetMenu(hwnd), IDM_DON_T_SNAP1, MF_UNCHECKED);
					break;
				}
				case IDM_MEDIUM_GRID1:{
					Globals::uSnapGridType = uSnapMediumGrid;
					CheckMenuItem(GetMenu(hwnd), IDM_SMALL_GRID1, MF_UNCHECKED);
					CheckMenuItem(GetMenu(hwnd), IDM_MEDIUM_GRID1, MF_CHECKED);
					CheckMenuItem(GetMenu(hwnd), IDM_LARGE_GRID1, MF_UNCHECKED);
					CheckMenuItem(GetMenu(hwnd), IDM_DON_T_SNAP1, MF_UNCHECKED);
					break;
				}
				case IDM_LARGE_GRID1:{
					Globals::uSnapGridType = uSnapLargeGrid;
					CheckMenuItem(GetMenu(hwnd), IDM_SMALL_GRID1, MF_UNCHECKED);
					CheckMenuItem(GetMenu(hwnd), IDM_MEDIUM_GRID1, MF_UNCHECKED);
					CheckMenuItem(GetMenu(hwnd), IDM_LARGE_GRID1, MF_CHECKED);
					CheckMenuItem(GetMenu(hwnd), IDM_DON_T_SNAP1, MF_UNCHECKED);
					break;
				}
				case IDM_DON_T_SNAP1:{
					Globals::uSnapGridType = 0;
					CheckMenuItem(GetMenu(hwnd), IDM_SMALL_GRID1, MF_UNCHECKED);
					CheckMenuItem(GetMenu(hwnd), IDM_MEDIUM_GRID1, MF_UNCHECKED);
					CheckMenuItem(GetMenu(hwnd), IDM_LARGE_GRID1, MF_UNCHECKED);
					CheckMenuItem(GetMenu(hwnd), IDM_DON_T_SNAP1, MF_CHECKED);
					break;
				}

				case IDM_SAVE1:{
					OPENFILENAME sfn;
					char sfPath[MAX_PATH];
					ZeroMemory( &sfn , sizeof( sfn));
					sfn.lStructSize = sizeof (OPENFILENAME);
					sfn.lpstrFile = sfPath;
					sfn.lpstrFile[0] = '\0';
					sfn.nMaxFile = MAX_PATH;
					sfn.lpstrFilter = "Foof Level Data\0*.flt\0\0";
					sfn.nFilterIndex = 1;
					sfn.hwndOwner = hwnd;

					if(GetSaveFileName(&sfn) != 0){
						LevelFileIO::SaveLevelToDisk(sfPath, &Scene);
					}
					break;
				}
				case IDM_OPEN___1:{
					OPENFILENAME sfn;
					char sfPath[MAX_PATH];
					ZeroMemory( &sfn , sizeof( sfn));
					sfn.lStructSize = sizeof (OPENFILENAME);
					sfn.lpstrFile = sfPath;
					sfn.lpstrFile[0] = '\0';
					sfn.nMaxFile = MAX_PATH;
					sfn.lpstrFilter = "Foof Level Data\0*.flt\0\0";
					sfn.nFilterIndex = 1;
					sfn.hwndOwner = hwnd;
					sfn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
					if(GetOpenFileName(&sfn) != 0){
						Scene.ResetScene();
						if(LevelFileIO::OpenLevelFromFile(sfPath, &Scene)){
							UpdateGrid(Scene.GetWidth(), Scene.GetHeight());
						}
					}
					break;
				}
				case IDC_PREVPLAY:{
					Globals::bPreviewPlay = true;
					Scene.ResetPlayerPosition();
					break;
				}
				case IDC_PREVSTOP:{
					Globals::bPreviewPlay = false;
					Scene.ResetPlayerPosition();
					break;
				}
				case PLACE_EXIT:{
					const int GridWidth = Scene.GetWidth();
					const int GridHeight = Scene.GetHeight();
					Scene.ExitPos_X = ((float)RClickX / Scene.SceneWidthPixels) * (GridWidth * LevelScene::LevelTileOffset) + Globals::CamX - hRenderer.GetRatio();
					Scene.ExitPos_Y = ((float)(Height - RClickY) / Scene.SceneHeightPixels) * (GridHeight * LevelScene::LevelTileOffset) + Globals::CamY - 1.0f;
					break;
				}
				case PLACE_BUNNY:{
					if(Scene.BunnyList.size() < 7){
						const int GridWidth = Scene.GetWidth();
						const int GridHeight = Scene.GetHeight();
						PropStruct * pProp = new PropStruct();
						pProp->nID = Scene.BunnyList.size();
						pProp->ExtendedInfo = NULL;
						pProp->x = ((float)RClickX / Scene.SceneWidthPixels) * (GridWidth * LevelScene::LevelTileOffset) + Globals::CamX - hRenderer.GetRatio();
						pProp->y = ((float)(Height - RClickY) / Scene.SceneHeightPixels) * (GridHeight * LevelScene::LevelTileOffset) + Globals::CamY - 1.0f;
						pProp->bIsSelected = false;
						pProp->uType = PropStruct::Bunny;
						pProp->uObjectType = PropStruct::Bunny;
						Scene.BunnyList.push_back(pProp);
					}
					break;
				}
				case PLACE_TOKEN:{
					const int GridWidth = Scene.GetWidth();
					const int GridHeight = Scene.GetHeight();
					PropStruct * pProp = new PropStruct();
					pProp->uObjectType = PropStruct::Token;
					pProp->uType = PropStruct::Token;
					pProp->nID = Scene.TokenList.size();
					pProp->x = ((float)RClickX / Scene.SceneWidthPixels) * (GridWidth * LevelScene::LevelTileOffset) + Globals::CamX - hRenderer.GetRatio();
					pProp->y = ((float)(Height - RClickY) / Scene.SceneHeightPixels) * (GridHeight * LevelScene::LevelTileOffset) + Globals::CamY - 1.0f;
					pProp->bIsSelected = false;
					pProp->ExtendedInfo = NULL;
					Scene.TokenList.push_back(pProp);
					break;
				}
				case PLACE_CHECKPOINT:{
					const int GridWidth = Scene.GetWidth();
					const int GridHeight = Scene.GetHeight();
					PropStruct * pProp = new PropStruct();
		
					pProp->bIsSelected = false;
					pProp->uType = 88;
					pProp->uObjectType = PropStruct::CheckPoint;
					pProp->ExtendedInfo = NULL;
					pProp->x = ((float)RClickX / Scene.SceneWidthPixels) * (GridWidth * LevelScene::LevelTileOffset) + Globals::CamX - hRenderer.GetRatio();
					pProp->y = ((float)(Height - RClickY) / Scene.SceneHeightPixels) * (GridHeight * LevelScene::LevelTileOffset) + Globals::CamY - 1.0f;

					pProp->nID = Scene.PropList.size();
					Scene.PropList.push_back(pProp);
					break;
				}
				case PLACE_PLAYERSTART:{
					const int GridWidth = Scene.GetWidth();
					const int GridHeight = Scene.GetHeight();


					const float PlayerX = ((float)RClickX / Scene.SceneWidthPixels) * (GridWidth * LevelScene::LevelTileOffset) + Globals::CamX - hRenderer.GetRatio();
					const float PlayerY = ((float)(Height - RClickY) / Scene.SceneHeightPixels) * (GridHeight * LevelScene::LevelTileOffset) + Globals::CamY - 1.0f;
					Scene.SetPlayerStart(PlayerX, PlayerY);
					Scene.ResetPlayerPosition();
					break;
				}
				case PLACE_CAMERA:{
					const int GridWidth = Scene.GetWidth();
					const int GridHeight = Scene.GetHeight();
					const float CamX = ((float)RClickX / Scene.SceneWidthPixels) * (GridWidth * LevelScene::LevelTileOffset) + Globals::CamX - hRenderer.GetRatio();;
					const float CamY = ((float)(Height- RClickY)  / Scene.SceneHeightPixels) * (GridHeight * LevelScene::LevelTileOffset) + Globals::CamY - 1.0f;;
					CamZone * pCam = new CamZone;
					pCam->nAxis = 0;
					pCam->bIsSelected = false;
					
					pCam->x = CamX;
					pCam->y = CamY;
					pCam->Left = pCam->x - CamZone::Scale * 3;
					pCam->Right = pCam->x + CamZone::Scale * 3;
					pCam->Up = pCam->y + CamZone::Scale * 3;
					pCam->Down = pCam->y - CamZone::Scale * 3;
					pCam->uObjectType = PropStruct::CameraZone;
					pCam->nID = Scene.CamList.size();
					Scene.CamList.push_back(pCam);
					break;
				}
				default:
					break;
			}
			ResetTimestep();
			break;
		}
		case WM_SIZE:{
			//Height = LOWORD(lParam);
			//Width = HIWORD(lParam);
			//hRenderer.ResizeFrame(Height,Width);
			//const float DoubleRatio = hRenderer.GetRatio() * 2;
			//const float SceneWidth = Scene.GetWidth() * LevelScene::LevelTileOffset;
			//SceneWidthPixels = (SceneWidth/DoubleRatio) * Width;
			//int ScrollMax = SceneWidthPixels;
			//UINT nPage = Width;
			//Scroller.UpdateScrollbar(SB_HORZ, 0, ScrollMax, nPage);
			//const float SceneHeight = Scene.GetHeight() * LevelScene::LevelTileOffset;
			//Scene.SceneHeightPixels = (SceneHeight/2.0f) * Height;
			//ScrollMax = Scene.SceneHeightPixels;
			//nPage = Height;
			//Scroller.UpdateScrollbar(SB_VERT, 0, ScrollMax, nPage);
			break;
		}
		case WM_RBUTTONDOWN:{
			RClickX = LOWORD(lParam); 
			RClickY = HIWORD(lParam); 
			HMENU hPopupMenu = CreatePopupMenu();
			int Count = 0;
			InsertMenu(hPopupMenu, Count++, MF_BYPOSITION | MF_STRING, PLACE_PLAYERSTART, "Place Player Start");
			InsertMenu(hPopupMenu, Count++, MF_BYPOSITION | MF_STRING, PLACE_CAMERA, "Place Camera Zone Here");
			InsertMenu(hPopupMenu, Count++, MF_BYPOSITION | MF_STRING, PLACE_CHECKPOINT, "Place CheckPoint Here");
			InsertMenu(hPopupMenu, Count++, MF_BYPOSITION | MF_STRING, PLACE_EXIT, "Place Exit Here");
			InsertMenu(hPopupMenu, Count++, MF_BYPOSITION | MF_STRING, PLACE_BUNNY, "Place Bunny Hostage Here");
			InsertMenu(hPopupMenu, Count++, MF_BYPOSITION | MF_STRING, PLACE_TOKEN, "Place Token Here");
			if(Globals::SelectedObjects.size() > 0){
				InsertMenu(hPopupMenu, Count++, MF_SEPARATOR, NULL, NULL);
				InsertMenu(hPopupMenu, Count++, MF_BYPOSITION | MF_STRING, IDM_CUT1, "Cut");
				InsertMenu(hPopupMenu, Count++, MF_BYPOSITION | MF_STRING, IDM_COPY1, "Copy");
				InsertMenu(hPopupMenu, Count++, MF_BYPOSITION | MF_STRING, IDM_PASTE1, "Paste");
				InsertMenu(hPopupMenu, Count++, MF_BYPOSITION | MF_STRING, IDM_DELETE1, "Delete");
				InsertMenu(hPopupMenu, Count++, MF_SEPARATOR, NULL, NULL);
				if(Globals::SelectedObjects.size() > 1){
					InsertMenu(hPopupMenu, Count++, MF_BYPOSITION | MF_STRING | MF_GRAYED, NULL, "Properties N/A: Multiple Selected");
				}else{
					InsertMenu(hPopupMenu, Count++, MF_BYPOSITION | MF_STRING , IDM_PROPERTIES1, "Properties");
				}
			}
			InsertMenu(hPopupMenu, Count++, MF_BYPOSITION | MF_SEPARATOR, NULL, NULL);
			if(Globals::bPreviewPlay)
				InsertMenu(hPopupMenu, Count++, MF_BYPOSITION | MF_STRING, IDC_PREVSTOP, "Stop Gameplay Preview");
			else
				InsertMenu(hPopupMenu, Count++, MF_BYPOSITION | MF_STRING, IDC_PREVPLAY, "Start Gameplay Preview");
			SetForegroundWindow(hwnd);
			TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, RClickX, RClickY, 0, hwnd, NULL);

			break;
		}
		case WM_LBUTTONDOWN:{
			const int CurrentMouseBehavior = pToolBox->GetCurrentBrushBehavior();
			const int MouseX = LOWORD(lParam); 
			const int MouseY = HIWORD(lParam); 

			const int GridWidth = Scene.GetWidth();
			const int GridHeight = Scene.GetHeight();
			Globals::MouseX_World = ((float)MouseX / Scene.SceneWidthPixels) * (GridWidth * LevelScene::LevelTileOffset) + Globals::CamX - hRenderer.GetRatio();
			Globals::MouseY_World = ((float)(Height - MouseY)  / Scene.SceneHeightPixels) * (GridHeight * LevelScene::LevelTileOffset) + Globals::CamY - 1.0f;
			Globals::MouseX_WorldPrev = Globals::MouseX_World;
			Globals::MouseY_WorldPrev = Globals::MouseY_World;
			if(CurrentMouseBehavior < 4){
				const int LookupX = (int)(Globals::MouseX_World / LevelScene::LevelTileOffset);
				const int LookupY = (int)(Globals::MouseY_World / LevelScene::LevelTileOffset);
				if(CurrentMouseBehavior > 0){
					DrawBlock(LookupX, LookupY, CurrentMouseBehavior);
				}else{
					switch(CurrentMouseBehavior){
						case 0:
							BucketFill(LookupX, LookupY, Scene.GetGridUnit(LookupX, LookupY));
							break;
						case -1:
							HorizontalLine(LookupX, LookupY);
							break;
						case -2:
							VerticalLine(LookupX, LookupY);
							break;
					}
				}
			}else{
				switch(CurrentMouseBehavior){
					case 4:{
						if(Globals::pObjectBrush){
							const int CurrentProp = Globals::pObjectBrush->uType;//pToolBox->GetBrushImageID() + 1;
							if(CurrentProp > 0){
								PropStruct * pProp = NULL;
								if(CurrentProp > 9){
									switch(Globals::pObjectBrush->ExtendedInfo->SubType){
										case 2:{
											pProp = new Mover;
											Mover * pMover = (Mover*)pProp;
											pMover->OriginX = Globals::pObjectBrush->x;
											pMover->OriginY = Globals::pObjectBrush->y;
											break;
										}
										default:
											pProp = new PropStruct;
									}
								}else{
									pProp = new PropStruct;
								}
								pProp->bIsSelected = false;
								pProp->uType = CurrentProp;
								pProp->x = Globals::pObjectBrush->x;
								pProp->y = Globals::pObjectBrush->y;
								if(CurrentProp < 10){
									pProp->nID = Scene.PropList.size();
									pProp->uObjectType = PropStruct::Collectible;
									pProp->ExtendedInfo = NULL;
									Scene.PropList.push_back(pProp);
								}else{
									pProp->uType -= 10;
									ExtendedObject * ObjectInfo = Globals::pObjectBrush->ExtendedInfo;
									pProp->ExtendedInfo = ObjectInfo;
									if(ObjectInfo->Type == 1){
										//if(Globals::DynamicSet[pProp->uType].Type < 4){
											pProp->nID = Scene.EnemyList.size();
											pProp->uObjectType = PropStruct::Enemy;
											//if(Globals::DynamicSet[CurrentProp]->SubType == 2){
											//	//If we're a mover, we need to log hte original X and Y values.
											//	Mover * pMover = (Mover*)pProp;
											//	pMover->OriginX = pMover->x;
											//	pMover->OriginY = pMover->y;
											//}
											Scene.EnemyList.push_back(pProp);
										//}
									}else if (ObjectInfo->Type == 2){
										pProp->nID = Scene.HazardList.size();
										pProp->uObjectType = PropStruct::Hazard;
										Scene.HazardList.push_back(pProp);
									}else if (ObjectInfo->Type == 3){
										pProp->nID = Scene.DecoList.size();
										pProp->uObjectType = PropStruct::Deco;
										Scene.DecoList.push_back(pProp);
									}
								}
							}
						}
						break;
					}
					case 5:{  //SELECT / MOVE PROP
						if(!(wParam & MK_CONTROL)){
							bool Hit = CheckMouseSelection(&Scene.PropList, wParam);
							if(!Hit){
								Hit = CheckMouseSelection(&Scene.EnemyList, wParam);
							}
							if(!Hit){
								Hit = CheckMouseSelection(&Scene.CamList, wParam);
							}
							if(!Hit){
								Hit = CheckMouseSelection(&Scene.HazardList, wParam);
							}
							if(!Hit){
								Hit = CheckMouseSelection(&Scene.DecoList, wParam);
							}
							if(!Hit){
								Hit = CheckMouseSelection(&Scene.TokenList, wParam);
							}
							if(!Hit){
								Hit = CheckMouseSelection(&Scene.BunnyList, wParam);
							}
							if(!(wParam & MK_SHIFT) && !Hit){
								DeselectAll();
							}
						}
						break;
					} //END SELECT / MOVE PROP CLICK
					default:
						break;
				}
			}
			break;
		}
		case WM_SETCURSOR:{
			break;
		}
		case WM_MOUSEMOVE:{
			const int MouseX = LOWORD(lParam); 
			const int MouseY = HIWORD(lParam); 

			const int GridWidth = Scene.GetWidth();
			const int GridHeight = Scene.GetHeight();
			Globals::MouseX_World = ((float)MouseX / Scene.SceneWidthPixels) * (GridWidth * LevelScene::LevelTileOffset) + Globals::CamX - hRenderer.GetRatio();
			Globals::MouseY_World = ((float)(Height - MouseY)  / Scene.SceneHeightPixels) * (GridHeight * LevelScene::LevelTileOffset) + Globals::CamY - 1.0f;

			if(Globals::pObjectBrush){
				if(Globals::uSnapGridType == uSnapMediumGrid){
					const float MedOffset = LevelScene::LevelTileOffset / 2;
					const float SmallOffset = MedOffset / 2;
					Globals::pObjectBrush->x = (int)((Globals::MouseX_World + SmallOffset) / MedOffset) * MedOffset;
					Globals::pObjectBrush->y = (int)((Globals::MouseY_World + SmallOffset) / MedOffset) * MedOffset;
				}else if(Globals::uSnapGridType == uSnapSmallGrid){
					const float SmallOffset = LevelScene::LevelTileOffset / 4;
					const float MicroOffset = SmallOffset / 2;
					Globals::pObjectBrush->x = (int)((Globals::MouseX_World + MicroOffset) / SmallOffset) * SmallOffset;
					Globals::pObjectBrush->y = (int)((Globals::MouseY_World + MicroOffset) / SmallOffset) * SmallOffset;
				}else{
					Globals::pObjectBrush->x = Globals::MouseX_World;
					Globals::pObjectBrush->y = Globals::MouseY_World;
				}
			}
			if(wParam & MK_CONTROL){
				bool OverZone = false;
				if(Globals::SelectedObjects.size() > 0 && Scene.CamList.size() > 0){
					for(std::vector<CamZone*>::iterator it = Scene.CamList.begin(); it < Scene.CamList.end(); it++){
						CamZone * pCam = *it;
						if(pCam->bIsSelected){
							if(Globals::MouseY_World < pCam->Up && Globals::MouseY_World > pCam->Down &&
								Globals::MouseX_World < pCam->Right && Globals::MouseX_World > pCam->Left){
								float DeltaX = Globals::MouseX_World - Globals::MouseX_WorldPrev;
								float DeltaY = Globals::MouseY_World - Globals::MouseY_WorldPrev;
								if(abs(Globals::MouseX_World - pCam->Right) < 0.15f){
									SetCursor(LoadCursor(NULL, IDC_SIZEWE));
									if(wParam & MK_LBUTTON){
										pCam->Right += DeltaX;
									}
								}else if(abs(Globals::MouseX_World - pCam->Left) < 0.15f){
									SetCursor(LoadCursor(NULL, IDC_SIZEWE));
									if(wParam & MK_LBUTTON){
										pCam->Left += DeltaX;
									}
								}else if(abs(Globals::MouseY_World - pCam->Up) < 0.15f){
									SetCursor(LoadCursor(NULL, IDC_SIZENS));
									if(wParam & MK_LBUTTON){
										pCam->Up += DeltaY;
									}
								}else if(abs(Globals::MouseY_World - pCam->Down) < 0.15f){
									SetCursor(LoadCursor(NULL, IDC_SIZENS));
									if(wParam & MK_LBUTTON){
										pCam->Down += DeltaY;
									}
								}else{
									SetCursor(LoadCursor(NULL, IDC_SIZEALL));
									if(wParam & MK_LBUTTON){
										pCam->TranslateZone(DeltaX, DeltaY);
									}
								}
								OverZone = true;
							}
						}
					}
				}
				if(!OverZone){
					SetCursor(LoadCursor(NULL, IDC_ARROW));
				}
			}else{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				if(wParam & MK_LBUTTON){
					const int CurrentMouseBehavior = pToolBox->GetCurrentBrushBehavior();
					if(CurrentMouseBehavior < 4){
						const int LookupX = (int)(Globals::MouseX_World / LevelScene::LevelTileOffset);
						const int LookupY = (int)(Globals::MouseY_World / LevelScene::LevelTileOffset);
						if(CurrentMouseBehavior > 0){
							DrawBlock(LookupX,LookupY, CurrentMouseBehavior);
						}
					}else if(CurrentMouseBehavior == 5){
						if(Globals::SelectedObjects.size() > 0){
							float DeltaX = 0.0f;
							float DeltaY = 0.0f;
							if(Globals::uSnapGridType > 0){
								const float MedOffset = LevelScene::LevelTileOffset / 2;
								const float SmallOffset = MedOffset / 2;
								const float MicroOffset = SmallOffset / 2;

								switch(Globals::uSnapGridType){
									case 2:{
										//Scale Previous to grid
										Globals::MouseX_WorldPrev = ((int)((Globals::MouseX_WorldPrev + SmallOffset) / MedOffset) * MedOffset);
										Globals::MouseY_WorldPrev = ((int)((Globals::MouseY_WorldPrev + SmallOffset) / MedOffset) * MedOffset);
										Globals::MouseX_World = ((int)((Globals::MouseX_World + SmallOffset) / MedOffset) * MedOffset);
										Globals::MouseY_World = ((int)((Globals::MouseY_World + SmallOffset) / MedOffset) * MedOffset);
										break;
									}
									case uSnapSmallGrid:{
								//Scale Previous to grid
									Globals::MouseX_WorldPrev = ((int)((Globals::MouseX_WorldPrev + MicroOffset) / SmallOffset) * SmallOffset);
									Globals::MouseY_WorldPrev = ((int)((Globals::MouseY_WorldPrev + MicroOffset) / SmallOffset) * SmallOffset);
										Globals::MouseX_World = ((int)((Globals::MouseX_World + MicroOffset) / SmallOffset) * SmallOffset);
										Globals::MouseY_World = ((int)((Globals::MouseY_World + MicroOffset) / SmallOffset) * SmallOffset);
										break;
									}
									default:
										break;
								}
							}
							DeltaX = Globals::MouseX_World - Globals::MouseX_WorldPrev;
							DeltaY = Globals::MouseY_World - Globals::MouseY_WorldPrev;
							for(vector<PropStruct*>::iterator it = Globals::SelectedObjects.begin(); it < Globals::SelectedObjects.end(); it++){
								PropStruct * pProp = *it;
								pProp->x += DeltaX;
								pProp->y += DeltaY;

								if(pProp->uObjectType == PropStruct::CameraZone){
									((CamZone*)pProp)->TranslateZone(DeltaX, DeltaY);
								}else if(pProp->ExtendedInfo){
									if(pProp->ExtendedInfo->SubType == 2){
										Mover * pMover = (Mover*)pProp;
										pMover->OriginX += DeltaX;
										pMover->OriginY += DeltaY;
									}
								}

							}
						} //Endif, Selectedobjects > 0
					}
				}
			}
			Globals::MouseX_WorldPrev = Globals::MouseX_World;
			Globals::MouseY_WorldPrev = Globals::MouseY_World;
			break;
		}
		case WM_VSCROLL:{
			Scroller.ScrollVertically(wParam);
			Scroller.UpdateCamY(Scene.GetHeight(), Scene.SceneHeightPixels);
			hRenderer.Draw(&Scene);
			break;
		}
		case WM_HSCROLL:{
			Scroller.ScrollHorizontally(wParam);
			Scroller.UpdateCamX(Scene.GetWidth(), Scene.SceneWidthPixels, hRenderer.GetRatio());
			hRenderer.Draw(&Scene);
			break;
		}
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	InitCommonControls();
	hAppInstance = hInstance;
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1);
	wc.style         = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    wc.lpszClassName = "FoofLevelEditor11211";
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
	if(!RegisterClassEx(&wc)){
        return 0;
    }
	HWND hAppWnd = CreateScaledWindow(hInstance, "FoofLevelEditor11211", "Foof Level Editor",
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, TRUE, 
			CW_USEDEFAULT, CW_USEDEFAULT, Width, Height);
	if(! hAppWnd || !hRenderer.Initialize(hAppWnd, Width, Height)){
		return 0;
	}
	CheckMenuItem(GetMenu(hAppWnd), IDM_MEDIUM_GRID1, MF_CHECKED);
	hRenderer.AssociateTextureAtlas(&TextureAtlas);
	hRenderer.AssociateGlobalTextureAtlas(&GlobalTextureAtlas);
	Scroller.AssociateWindow(hAppWnd);
	hRenderer.AssociateScroller(&Scroller);
	//LevelFileIO::OpenLevelTileSet("C:/Users/Malvina/workspace/FoofGame/res/raw/forest.fts");
	LevelFileIO::OpenLevelTileSet("C:/Users/Malvina/workspace/FoofGame/res/raw/mech.fts");
	//Load TileSet Definition... TODO - find less retarded workaround

	//Globals::EnemyA_ID = GlobalTextureAtlas.GetIndexByTitle("robot1.png");
	Globals::CollectibleID[1] = GlobalTextureAtlas.GetIndexByTitle("choco_pop.png");
	Globals::CollectibleID[2] = GlobalTextureAtlas.GetIndexByTitle("frosted_donut.png");
	Globals::CollectibleID[3] = GlobalTextureAtlas.GetIndexByTitle("cake.png");
	Globals::CollectibleID[4] = GlobalTextureAtlas.GetIndexByTitle("cherries.png");
	Globals::CollectibleID[5] = GlobalTextureAtlas.GetIndexByTitle("brownie.png");
	Globals::CollectibleID[6] = GlobalTextureAtlas.GetIndexByTitle("cookie.png");
	Globals::CollectibleID[7] = GlobalTextureAtlas.GetIndexByTitle("jawbreaker.png");
	Globals::CollectibleID[8] = GlobalTextureAtlas.GetIndexByTitle("doubleburger.png");
	Globals::CollectibleID[9] = GlobalTextureAtlas.GetIndexByTitle("energy.png");

	Globals::CheckPointIndex = GlobalTextureAtlas.GetIndexByTitle("CHECKPOINT.png");
	Globals::CageIndex = GlobalTextureAtlas.GetIndexByTitle("cageball.png");
	Globals::BabyBunnyIndex = GlobalTextureAtlas.GetIndexByTitle("babybunny1.png");
	Globals::TokenIndex = GlobalTextureAtlas.GetIndexByTitle("bunnymoney.png");
	Globals::ExitIndex = GlobalTextureAtlas.GetIndexByTitle("exit.png");
	Player::BASICRUN_SPRITEINDEX = GlobalTextureAtlas.GetIndexByTitle("bunny1.png");

	Scene.mPlayer.Initialize();
	Scene.mPlayer.PrepareAnimations();

	NewGrid(64, 32);

	Scroller.UpdateCamX(Scene.GetWidth(), Scene.SceneWidthPixels, hRenderer.GetRatio());
	Scroller.UpdateCamY(Scene.GetHeight(), Scene.SceneHeightPixels);

	ShowWindow(hAppWnd, SW_SHOW);
	pToolBox = new ToolBoxWindow(hInstance, hAppWnd);
	MSG msg;

	PrevFrameTime = GetTickCount();
	while(bRun){
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			if(msg.message == WM_QUIT){
				break;
			}else{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}else{
			//On win idle time, update + draw
			UpdateFrametime();
			Scene.Update();
			hRenderer.Draw(&Scene);
			PrevFrameTime = FrameTime;
		}
	}
	pToolBox->Kill();
	delete pToolBox;
	return 0;
}