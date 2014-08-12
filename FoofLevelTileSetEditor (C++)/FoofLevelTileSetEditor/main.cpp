#include <Windows.h>
#include <commctrl.h>
#include <Commdlg.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "DestructibleObject.h"
#include "resource.h"
#include "Mover.h"
#include "Enemy.h"
#include "EnemyWalker.h"
#include "EnemyFlyer.h"
#include "Globals.h"
#include "TextureAtlasManager.h"
#include "AnimationTrack.h"
#include "DynamicObject.h"
#include "StaticDeco.h"
#include "LevelTile.h"
#include "Renderer.h"
#include "WICImageLoader.h"
#include "GDIImageLoader.h"
#include "HazardousObject.h"

#pragma comment (lib,"Comctl32.lib ")
#pragma comment (lib,"Comdlg32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


std::vector<std::string> Globals::TextureList;
std::vector<AnimationTrack> AnimationSet;
std::vector<StaticDeco> DecoSet;
std::vector<DynamicObject*> DynamicSet;
std::vector<DynamicObject*> ProjectileSet;
std::vector<HazardousObject> HazardousSet;
std::vector<LevelTile> LevelTileSet;

//TextureAtlasManager TextureAtlas("C:/Users/Malvina/workspace/FoofGame/res/raw/forestlevel.uv");
TextureAtlasManager TextureAtlas("C:/Users/Malvina/workspace/FoofGame/res/raw/mechlevel.uv");
HINSTANCE hAppInstance = NULL;
CollisionData Globals::PreviewCD;
static const int ChargingEnemyA = 0;
static const int ChargingEnemyB = 1;
static const int SidewayShooter = 2;
static const int VerticalShooter = 3;
static const int HorizontalMover = 4;
static const int VerticalMover = 5;
static const int AngryPlatform = 6; //piranha Plants, etc.
static const int FallingSpike = 7;  // what does it sound like?
std::string DynamicLabels[] = {"Enemy Walker", "Enemy Flyer", "Mover","Snapmouth", "Falling Hazard","Destructible", "FartPlant", "FallingPlatform", "SparkleParticle", "GiantMantis", "FatBug",};
const int nDynamicLabels = sizeof(DynamicLabels) / sizeof(std::string);

Renderer PreviewRenderer;
using namespace std;
int SpriteID = 0;
DynamicObject * pEditObject = NULL;

float GetDlgItemFloat(HWND hwnd, int nDlgItem){
	char strFloat[MAX_PATH];
	GetDlgItemText(hwnd, nDlgItem, strFloat, MAX_PATH);
	return (float)atof(strFloat);
}
BOOL CALLBACK AddDefault(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		case WM_INITDIALOG:{
			//GEt sprite list
			const HWND hSpriteCombo = GetDlgItem(hwnd, IDC_BASESPRITE);
			for(std::vector<std::string>::iterator it = TextureAtlas.TextureList.begin(); it < TextureAtlas.TextureList.end(); it++){
				const string SpriteName = *it;
				SendMessage(hSpriteCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) SpriteName.c_str()); 
			}
			SendMessage(hSpriteCombo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SetDlgItemText(hwnd, IDC_NAME, "New Object");
			SetDlgItemText(hwnd, IDC_SCALE, "1.0");
			const HWND DefaultAnim = GetDlgItem(hwnd, IDC_BASICANIM);
			SendMessage(DefaultAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"NONE"); 

			for(std::vector<AnimationTrack>::iterator it = AnimationSet.begin(); it < AnimationSet.end(); it++){
				SendMessage(DefaultAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)it->Name.c_str()); 
			}
			SendMessage(DefaultAnim, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			break;
		}
			case WM_COMMAND:{
			if(LOWORD(wParam) == IDOK){
				pEditObject = new DynamicObject;
				pEditObject->CD.Copy(Globals::PreviewCD);
				pEditObject->fDefaultScale = GetDlgItemFloat(hwnd, IDC_SCALE);
				pEditObject->iSpriteIndex = SendMessage(GetDlgItem(hwnd, IDC_BASESPRITE), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				pEditObject->iDefaultAnimationIndex = SendMessage(GetDlgItem(hwnd, IDC_BASICANIM), CB_GETCURSEL, (WPARAM)0, (LPARAM)0) - 1;
				char Name[MAX_PATH];
				GetDlgItemText(hwnd, IDC_NAME, Name, MAX_PATH);
				pEditObject->Name.assign(Name);
				DynamicSet.push_back(pEditObject);
				EndDialog(hwnd, IDOK);
			}else if(LOWORD(wParam) == IDCANCEL){
				EndDialog(hwnd, IDCANCEL);
			}
			break;
	}
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK AddDestructibleDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		case WM_INITDIALOG:{
						//GEt sprite list
			const HWND hSpriteCombo = GetDlgItem(hwnd, IDC_BASESPRITE);
			for(std::vector<std::string>::iterator it = TextureAtlas.TextureList.begin(); it < TextureAtlas.TextureList.end(); it++){
				const string SpriteName = *it;
				SendMessage(hSpriteCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) SpriteName.c_str()); 
			}
			SendMessage(hSpriteCombo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SetDlgItemText(hwnd, IDC_NAME, "New Object");
			SetDlgItemText(hwnd, IDC_SCALE, "1.0");
			SetDlgItemInt(hwnd, IDC_HP, 3, TRUE);
			const HWND DefaultAnim = GetDlgItem(hwnd, IDC_BASICANIM);
			SendMessage(DefaultAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"NONE"); 

			for(std::vector<AnimationTrack>::iterator it = AnimationSet.begin(); it < AnimationSet.end(); it++){
				SendMessage(DefaultAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)it->Name.c_str()); 
			}
			SendMessage(DefaultAnim, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

			const HWND SpawnType = GetDlgItem(hwnd, IDC_DESTRUCTYPE);
			SendMessage(SpawnType,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Explosive");
			SendMessage(SpawnType,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Points150");
			SendMessage(SpawnType,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Points300");
			SendMessage(SpawnType,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Dynamic");
			SendMessage(SpawnType,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Collectible");
			SendMessage(SpawnType, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			break;
		}
		case WM_COMMAND:{
			if(LOWORD(wParam) == IDOK){
				pEditObject = new DestructibleObject;
				pEditObject->CD.Copy(Globals::PreviewCD);
				pEditObject->fDefaultScale = GetDlgItemFloat(hwnd, IDC_SCALE);
				((DestructibleObject*)pEditObject)->HP = GetDlgItemInt(hwnd, IDC_HP, false, true);
				pEditObject->iSpriteIndex = SendMessage(GetDlgItem(hwnd, IDC_BASESPRITE), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				pEditObject->iDefaultAnimationIndex = SendMessage(GetDlgItem(hwnd, IDC_BASICANIM), CB_GETCURSEL, (WPARAM)0, (LPARAM)0) - 1;
				char Name[MAX_PATH];
				GetDlgItemText(hwnd, IDC_NAME, Name, MAX_PATH);
				pEditObject->Name.assign(Name);
				((DestructibleObject*)pEditObject)->SpawnID = SendMessage(GetDlgItem(hwnd, IDC_DESTID), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				((DestructibleObject*)pEditObject)->Type = SendMessage(GetDlgItem(hwnd, IDC_DESTRUCTYPE), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				DynamicSet.push_back(pEditObject);
				EndDialog(hwnd, IDOK);
			}else if(LOWORD(wParam) == IDCANCEL){
				EndDialog(hwnd, IDCANCEL);
			}else{
				if(HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == IDC_DESTRUCTYPE){
					const HWND SpawnID = GetDlgItem(hwnd, IDC_DESTID);
					const int Type = SendMessage(GetDlgItem(hwnd, IDC_DESTRUCTYPE), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					if(Type == 3){
						SendMessage(SpawnID, CB_RESETCONTENT, 0, 0);
						for(vector<DynamicObject*>::iterator it = DynamicSet.begin(); it < DynamicSet.end(); it++){
							SendMessage(SpawnID,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)(*it)->Name.c_str());
						}
					}else if(Type == 4){
						SendMessage(SpawnID, CB_RESETCONTENT, 0, 0);
						SendMessage(SpawnID,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Choco Pop");
						SendMessage(SpawnID,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Frosted Donut");
					}
				}
			}
			break;
		}
		default:
			return FALSE;
	}
	return true;
}
BOOL CALLBACK AddMoverDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		case WM_INITDIALOG:{
			//GEt sprite list
			const HWND hSpriteCombo = GetDlgItem(hwnd, IDC_BASESPRITE);
			for(std::vector<std::string>::iterator it = TextureAtlas.TextureList.begin(); it < TextureAtlas.TextureList.end(); it++){
				const string SpriteName = *it;
				SendMessage(hSpriteCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) SpriteName.c_str()); 
			}
			SendMessage(hSpriteCombo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SetDlgItemText(hwnd, IDC_NAME, "New Object");
			SetDlgItemText(hwnd, IDC_SCALE, "1.0");
			SetDlgItemInt(hwnd, IDC_HP, 3, TRUE);
			const HWND DefaultAnim = GetDlgItem(hwnd, IDC_BASICANIM);
			const HWND MoveAnim = GetDlgItem(hwnd, IDC_MOVEANIM);
			SendMessage(DefaultAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"NONE"); 
			SendMessage(MoveAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"NONE"); 

			for(std::vector<AnimationTrack>::iterator it = AnimationSet.begin(); it < AnimationSet.end(); it++){
				SendMessage(DefaultAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)it->Name.c_str()); 
				SendMessage(MoveAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)it->Name.c_str()); 
			}
			SendMessage(DefaultAnim, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SendMessage(MoveAnim, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

			break;
		}
		case WM_COMMAND:{
			if(LOWORD(wParam) == IDOK){
					pEditObject = new Mover;
					Mover * NewMover = (Mover*)pEditObject;
					NewMover->iSpriteIndex = SendMessage(GetDlgItem(hwnd, IDC_BASESPRITE), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					char Name[MAX_PATH];
					GetDlgItemText(hwnd, IDC_NAME, Name, MAX_PATH);
					NewMover->Name.assign(Name);
					NewMover->fDefaultScale = GetDlgItemFloat(hwnd, IDC_SCALE);
					NewMover->iDefaultAnimationIndex = SendMessage(GetDlgItem(hwnd, IDC_BASICANIM), CB_GETCURSEL, (WPARAM)0, (LPARAM)0) - 1;
					NewMover->iMovingAnimationIndex = SendMessage(GetDlgItem(hwnd, IDC_MOVEANIM), CB_GETCURSEL, (WPARAM)0, (LPARAM)0) - 1;
					NewMover->HP = GetDlgItemInt(hwnd, IDC_HP, FALSE, TRUE);
					NewMover->bIsHazard = IsDlgButtonChecked(hwnd, IDC_HAZCHECK);
					DynamicSet.push_back(pEditObject);
					EndDialog(hwnd, IDOK);
			}else if(LOWORD(wParam) == IDCANCEL){
				EndDialog(hwnd, IDCANCEL);
			}
			break;
		}
		default:
			return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK AddDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		case WM_INITDIALOG:{
			//GEt sprite list
			const HWND hSpriteCombo = GetDlgItem(hwnd, IDC_BASESPRITE);
			for(std::vector<std::string>::iterator it = TextureAtlas.TextureList.begin(); it < TextureAtlas.TextureList.end(); it++){
				const string SpriteName = *it;
				SendMessage(hSpriteCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) SpriteName.c_str()); 
			}
			SendMessage(hSpriteCombo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SetDlgItemText(hwnd, IDC_NAME, "New Object");
			SetDlgItemText(hwnd, IDC_SCALE, "1.0");
			SetDlgItemInt(hwnd, IDC_HP, 3, TRUE);
			const HWND DefaultAnim = GetDlgItem(hwnd, IDC_BASICANIM);
			const HWND AttackAnim = GetDlgItem(hwnd, IDC_ATTACKANIM);
			const HWND DeathAnim = GetDlgItem(hwnd, IDC_DEATHANIM);
			SendMessage(DefaultAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"NONE"); 
			SendMessage(AttackAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"NONE"); 
			SendMessage(DeathAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"NONE"); 

			for(std::vector<AnimationTrack>::iterator it = AnimationSet.begin(); it < AnimationSet.end(); it++){
				SendMessage(DefaultAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)it->Name.c_str()); 
				SendMessage(AttackAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)it->Name.c_str()); 
				SendMessage(DeathAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)it->Name.c_str()); 
			}
			SendMessage(DefaultAnim, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SendMessage(DeathAnim, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SendMessage(AttackAnim, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

			const HWND hShotType = GetDlgItem(hwnd, IDC_SHOTTYPE);
			SendMessage(hShotType,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"NONE"); 
			for(vector<DynamicObject*>::iterator it = ProjectileSet.begin(); it < ProjectileSet.end(); it++){
				SendMessage(hShotType,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)(*it)->Name.c_str()); 
			}

			const HWND hShotDir = GetDlgItem(hwnd, IDC_SHOTDIR);
			SendMessage(hShotDir,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"LEFT & RIGHT"); 
			SendMessage(hShotDir,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"UP & DOWN");
			SendMessage(hShotDir,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"UP ONLY");
			SendMessage(hShotDir,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"DOWN ONLY");
			SendMessage(hShotDir,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Diagonal Down");
			SendMessage(hShotDir,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Diagonal Up");
			SendMessage(hShotDir,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"TRACK PLAYER");

			const HWND hAgro = GetDlgItem(hwnd, IDC_AGRO);
			SendMessage(hAgro,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Passive"); 
			SendMessage(hAgro,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Fully Aggressive"); 
			SendMessage(hAgro,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Semi Aggressive"); 
			SendMessage(hAgro, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SetDlgItemText(hwnd, IDC_SPEED, "1.0");
			SetDlgItemText(hwnd, IDC_JUMPSTR, "1.0");

			SendMessage(hShotDir, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SendMessage(hShotType, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SetDlgItemText(hwnd, IDC_NSHOTS, "0");
			SetDlgItemText(hwnd, IDC_SHOTDELAY, "0.0f");
			break;
		}
		case WM_COMMAND:{
			switch(LOWORD(wParam)){
				case IDOK:{
					pEditObject = new EnemyWalker();
					EnemyWalker * NewEnemy = (EnemyWalker*)pEditObject;
					NewEnemy->HP = GetDlgItemInt(hwnd, IDC_HP, FALSE, TRUE);
					NewEnemy->iSpriteIndex = SendMessage(GetDlgItem(hwnd, IDC_BASESPRITE), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					NewEnemy->JumpStrength = GetDlgItemFloat(hwnd, IDC_JUMPSTR);
					NewEnemy->Speed = GetDlgItemFloat(hwnd, IDC_SPEED);
					char Name[MAX_PATH];
					GetDlgItemText(hwnd, IDC_NAME, Name, MAX_PATH);
					NewEnemy->Name.assign(Name);
					NewEnemy->fDefaultScale = GetDlgItemFloat(hwnd, IDC_SCALE);
					NewEnemy->iDefaultAnimationIndex = SendMessage(GetDlgItem(hwnd, IDC_BASICANIM), CB_GETCURSEL, (WPARAM)0, (LPARAM)0) - 1;
					NewEnemy->AttackAnimationIndex = SendMessage(GetDlgItem(hwnd, IDC_ATTACKANIM), CB_GETCURSEL, (WPARAM)0, (LPARAM)0) - 1;
					NewEnemy->DeathAnimationIndex = SendMessage(GetDlgItem(hwnd, IDC_DEATHANIM), CB_GETCURSEL, (WPARAM)0, (LPARAM)0) - 1;
					NewEnemy->Aggressiveness = SendMessage(GetDlgItem(hwnd, IDC_AGRO), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					NewEnemy->bIsDormant = (int)IsDlgButtonChecked(hwnd, IDC_CHKDORMANT);
					NewEnemy->ShotDirection = SendMessage(GetDlgItem(hwnd, IDC_SHOTDIR), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					NewEnemy->ShotType = SendMessage(GetDlgItem(hwnd, IDC_SHOTTYPE), CB_GETCURSEL, (WPARAM)0, (LPARAM)0) - 1;
					NewEnemy->nShotsPerBurst = GetDlgItemInt(hwnd, IDC_NSHOTS, false, false);
					NewEnemy->nBurstDelay = GetDlgItemFloat(hwnd, IDC_SHOTDELAY);
					DynamicSet.push_back(pEditObject);
					EndDialog(hwnd, IDOK);
					break;
				}
				case IDCANCEL:{
					EndDialog(hwnd, IDCANCEL);
					break;
				}
				default:
					break;
			}
		}
		default:
			return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK AddFlyerDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		case WM_INITDIALOG:{
			//GEt sprite list
			const HWND hSpriteCombo = GetDlgItem(hwnd, IDC_BASESPRITE);
			for(std::vector<std::string>::iterator it = TextureAtlas.TextureList.begin(); it < TextureAtlas.TextureList.end(); it++){
				const string SpriteName = *it;
				SendMessage(hSpriteCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) SpriteName.c_str()); 
			}
			SendMessage(hSpriteCombo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SetDlgItemText(hwnd, IDC_NAME, "New Object");
			SetDlgItemText(hwnd, IDC_SCALE, "1.0");
			SetDlgItemInt(hwnd, IDC_HP, 3, TRUE);
			const HWND DefaultAnim = GetDlgItem(hwnd, IDC_BASICANIM);
			const HWND AttackAnim = GetDlgItem(hwnd, IDC_ATTACKANIM);
			const HWND DeathAnim = GetDlgItem(hwnd, IDC_DEATHANIM);
			SendMessage(DefaultAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"NONE"); 
			SendMessage(AttackAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"NONE"); 
			SendMessage(DeathAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"NONE"); 

			for(std::vector<AnimationTrack>::iterator it = AnimationSet.begin(); it < AnimationSet.end(); it++){
				SendMessage(DefaultAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)it->Name.c_str()); 
				SendMessage(AttackAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)it->Name.c_str()); 
				SendMessage(DeathAnim,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)it->Name.c_str()); 
			}
			SendMessage(DefaultAnim, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SendMessage(DeathAnim, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SendMessage(AttackAnim, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

			const HWND hShotType = GetDlgItem(hwnd, IDC_SHOTTYPE);
			SendMessage(hShotType,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"NONE"); 
			for(vector<DynamicObject*>::iterator it = ProjectileSet.begin(); it < ProjectileSet.end(); it++){
				SendMessage(hShotType,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)(*it)->Name.c_str()); 
			}

			const HWND hShotDir = GetDlgItem(hwnd, IDC_SHOTDIR);
			SendMessage(hShotDir,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"LEFT & RIGHT"); 
			SendMessage(hShotDir,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"UP & DOWN");
			SendMessage(hShotDir,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"UP ONLY");
			SendMessage(hShotDir,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"DOWN ONLY");
			SendMessage(hShotDir,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Diagonal Down");
			SendMessage(hShotDir,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Diagonal Up");
			SendMessage(hShotDir,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"TRACK PLAYER");

			const HWND hAgro = GetDlgItem(hwnd, IDC_Flight);
			SendMessage(hAgro,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Passive"); 
			SendMessage(hAgro,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Fully Aggressive"); 
			SendMessage(hAgro,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Semi Aggressive"); 
			SendMessage(hAgro, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SetDlgItemText(hwnd, IDC_SPEED, "1.0");

			SendMessage(hShotDir, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SendMessage(hShotType, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SetDlgItemText(hwnd, IDC_NSHOTS, "0");
			SetDlgItemText(hwnd, IDC_SHOTDELAY, "0.0f");
			break;
		}
		case WM_COMMAND:{
			switch(LOWORD(wParam)){
				case IDOK:{
					pEditObject = new EnemyFlyer();
					EnemyFlyer * NewEnemy = (EnemyFlyer*)pEditObject;
					NewEnemy->HP = GetDlgItemInt(hwnd, IDC_HP, FALSE, TRUE);
					NewEnemy->iSpriteIndex = SendMessage(GetDlgItem(hwnd, IDC_BASESPRITE), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					NewEnemy->FlightType = 0;
					NewEnemy->Speed = GetDlgItemFloat(hwnd, IDC_SPEED);
					char Name[MAX_PATH];
					GetDlgItemText(hwnd, IDC_NAME, Name, MAX_PATH);
					NewEnemy->Name.assign(Name);
					NewEnemy->fDefaultScale = GetDlgItemFloat(hwnd, IDC_SCALE);
					NewEnemy->iDefaultAnimationIndex = SendMessage(GetDlgItem(hwnd, IDC_BASICANIM), CB_GETCURSEL, (WPARAM)0, (LPARAM)0) - 1;
					NewEnemy->AttackAnimationIndex = SendMessage(GetDlgItem(hwnd, IDC_ATTACKANIM), CB_GETCURSEL, (WPARAM)0, (LPARAM)0) - 1;
					NewEnemy->DeathAnimationIndex = SendMessage(GetDlgItem(hwnd, IDC_DEATHANIM), CB_GETCURSEL, (WPARAM)0, (LPARAM)0) - 1;
					NewEnemy->bIsDormant = (int)IsDlgButtonChecked(hwnd, IDC_CHKDORMANT);
					NewEnemy->ShotDirection = SendMessage(GetDlgItem(hwnd, IDC_SHOTDIR), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					NewEnemy->ShotType = SendMessage(GetDlgItem(hwnd, IDC_SHOTTYPE), CB_GETCURSEL, (WPARAM)0, (LPARAM)0) - 1;
					NewEnemy->nShotsPerBurst = GetDlgItemInt(hwnd, IDC_NSHOTS, false, false);
					NewEnemy->nBurstDelay = GetDlgItemFloat(hwnd, IDC_SHOTDELAY);
					DynamicSet.push_back(pEditObject);
					EndDialog(hwnd, IDOK);
					break;
				}
				case IDCANCEL:{
					EndDialog(hwnd, IDCANCEL);
					break;
				}
				default:
					break;
			}
		}
		default:
			return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch(msg){
		case WM_CLOSE:{
			DestroyWindow(hwnd);
			break;
		}
		case WM_DESTROY:{
			PostQuitMessage(0);
			break;
		}
		case WM_COMMAND:{
			if(HIWORD(wParam) == EN_CHANGE){
				const int ID = LOWORD(wParam);
				switch(ID){
					case IDC_CDLEFT:{
						Globals::PreviewCD.Left = GetDlgItemFloat(hwnd, ID);
						break;
					}
					case IDC_CDRIGHT:{
						Globals::PreviewCD.Right = GetDlgItemFloat(hwnd, ID);
						break;
					}
					case IDC_CDTOP:{
						Globals::PreviewCD.Top = GetDlgItemFloat(hwnd, ID);
						break;
					}
					case IDC_CDBOTTOM:{
						Globals::PreviewCD.Bottom = GetDlgItemFloat(hwnd, ID);
						break;
					}
					default:
						break;
				}
			}
			switch(LOWORD(wParam)){
				case IDM_EXIT1:{
					DestroyWindow(hwnd);
				}
				case IDM_OPEN1:{
					OPENFILENAME sfn;
					char sfPath[MAX_PATH];
					ZeroMemory( &sfn , sizeof( sfn));
					sfn.lStructSize = sizeof (OPENFILENAME);
					sfn.lpstrFile = sfPath;
					sfn.lpstrFile[0] = '\0';
					sfn.nMaxFile = MAX_PATH;
					sfn.lpstrFilter = "Foof Level Tile Set\0*.fts\0\0";
					sfn.nFilterIndex = 1;
					sfn.hwndOwner = hwnd;
					if(GetOpenFileName(&sfn) != 0){
						LevelTileSet.clear();
						AnimationSet.clear();
						DynamicSet.clear();
						DecoSet.clear();
						HazardousSet.clear();
						ifstream FileIn(sfPath);
						if(FileIn.is_open()){
							string Reader;
							FileIn >> Reader;
							if(Reader == "Foof72"){
								int Size = 0;
								FileIn >> Size;
								HWND hListBox = GetDlgItem(hwnd, IDC_COLLIST);
								SendMessage(hListBox, LB_RESETCONTENT, 0,0);
								for(int i = 0; i < Size; i++){
									LevelTile * pLevelTile = new LevelTile;
									FileIn >> pLevelTile->SpriteID;
									FileIn >> pLevelTile->IsCeiling;
									FileIn >> pLevelTile->IsNotWall;
									for(int i = 0; i < 64; i++){
										FileIn >> pLevelTile->HeightMask[i];
									}
									FileIn >> pLevelTile->Name;

									LevelTileSet.push_back(*pLevelTile);


									SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)pLevelTile->Name.c_str());
								}
								FileIn >> Size;
								hListBox = GetDlgItem(hwnd, IDC_ANIMLIST);
								SendMessage(hListBox, LB_RESETCONTENT, 0,0);
								for(int i = 0; i < Size; i++){
									AnimationTrack * pAnim = new AnimationTrack;
									FileIn >> pAnim->Name;
									FileIn >> pAnim->iStartingFrame;
									FileIn >> pAnim->nFrames;
									FileIn >> pAnim->fDefaultSpeed;
									char TruncName[7];
									size_t TruncNameLength = pAnim->Name.copy(TruncName, 6,0);
									TruncName[TruncNameLength] = '\0';
									string Name = TruncName;
									char strFrames[50];
									sprintf(strFrames, "%i", pAnim->nFrames);
									char strFPS[50];
									sprintf(strFPS, "%f", pAnim->fDefaultSpeed);
									string ListEntry = Name + "..." + " - " + strFrames + " Frames" + ", " + strFPS + " FPS";
									SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)ListEntry.c_str());
									AnimationSet.push_back(*pAnim);
								}
								FileIn >> Size;
								hListBox = GetDlgItem(hwnd, IDC_DYNLIST);
								SendMessage(hListBox, LB_RESETCONTENT, 0,0);
								for(int i = 0; i < Size; i++){
									DynamicObject * pDyn = NULL;
									int Type;
									FileIn >> Type;
									switch(Type){
										case 0:{  //Walker
											pDyn = new EnemyWalker;
											EnemyWalker * pWalker = (EnemyWalker*)pDyn;
											FileIn >> pWalker->AttackAnimationIndex
													>> pWalker->DeathAnimationIndex
													>> pWalker->JumpStrength
													>> pWalker->Speed
													>> pWalker->HP
													>> pWalker->Aggressiveness
													>> pWalker->bIsDormant
													>> pWalker->ShotType
													>> pWalker->ShotDirection
													>> pWalker->nBurstDelay
													>> pWalker->nShotsPerBurst;
											break;
										}
										case 1:{  //Flyer
											pDyn = new EnemyFlyer;
											EnemyFlyer * pWalker = (EnemyFlyer*)pDyn;
											FileIn >> pWalker->AttackAnimationIndex
													>> pWalker->DeathAnimationIndex
													>> pWalker->Speed
													>> pWalker->HP
													>> pWalker->FlightType
													>> pWalker->bIsDormant
													>> pWalker->ShotType
													>> pWalker->ShotDirection
													>> pWalker->nBurstDelay
													>> pWalker->nShotsPerBurst;
											break;
										}
										case 2: { //MOver
											pDyn = new Mover;
											Mover * pMover = (Mover*)pDyn;
											FileIn >> pMover->iMovingAnimationIndex
													>> pMover->HP
													>> pMover->bIsHazard;
											break;
										}
										case 5: { //Destructible
											pDyn = new DestructibleObject;
											DestructibleObject * pDest = (DestructibleObject*)pDyn;
											FileIn >> pDest->HP
												>> pDest->Type
												>> pDest->SpawnID;
											break;
										 }
										default:
											pDyn = new DynamicObject;
											break;
									}

									FileIn >> pDyn->Name
											>> pDyn->iDefaultAnimationIndex
											>> pDyn->iSpriteIndex
											>> pDyn->fDefaultScale
											>> pDyn->CD.Left
											>> pDyn->CD.Right
											>> pDyn->CD.Top
											>> pDyn->CD.Bottom;
									pDyn->Type = Type;
									DynamicSet.push_back(pDyn);
									SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)pDyn->Name.c_str()); 
								}

								FileIn >> Size;
								hListBox = GetDlgItem(hwnd, IDC_HAZLIST);
								SendMessage(hListBox, LB_RESETCONTENT, 0,0);
								for(int i = 0; i < Size; i++){
									HazardousObject * pHaz = new HazardousObject();
									FileIn >> pHaz->Name;
									FileIn >> pHaz->SpriteID;
									FileIn >> pHaz->Damage;
									FileIn >> pHaz->fDefaultScale
										>> pHaz->CD.Left
										>> pHaz->CD.Right
										>> pHaz->CD.Top
										>> pHaz->CD.Bottom;

									SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)pHaz->Name.c_str()); 

									HazardousSet.push_back(*pHaz);
								}
								FileIn >> Size;
								hListBox = GetDlgItem(hwnd, IDC_DECOLIST);
								SendMessage(hListBox, LB_RESETCONTENT, 0,0);
								for(int i = 0; i < Size; i++){
									StaticDeco * pDeco = new StaticDeco();
									FileIn >> pDeco->Name;
									FileIn >> pDeco->iSpriteID;
									FileIn >> pDeco->fDefaultScale
										>> pDeco->CD.Left
										>> pDeco->CD.Right
										>> pDeco->CD.Top
										>> pDeco->CD.Bottom;;

									SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)pDeco->Name.c_str()); 

									DecoSet.push_back(*pDeco);
								}
								FileIn >> Size;
								hListBox = GetDlgItem(hwnd, IDC_PROJECTILELIST);
								SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
								for(int i = 0; i < Size; i++){
									DynamicObject * pProj = new DynamicObject();
									FileIn >> pProj->Name 
										>> pProj->iDefaultAnimationIndex 
										>> pProj->fDefaultScale
										>> pProj->CD.Left
										>> pProj->CD.Right
										>> pProj->CD.Top
										>> pProj->CD.Bottom;

									SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)pProj->Name.c_str()); 
									ProjectileSet.push_back(pProj);
								}
							}
							FileIn.close();
						}
					}
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
					sfn.lpstrFilter = "Foof Level Tile Set\0*.fts\0\0";
					sfn.nFilterIndex = 1;
					sfn.hwndOwner = hwnd;

					if(GetSaveFileName(&sfn) != 0){
								ofstream FileOut(sfPath);
						if(FileOut.is_open()){
							FileOut << "Foof72" << endl
								<< LevelTileSet.size() << endl;
							for(vector<LevelTile>::iterator it = LevelTileSet.begin(); it < LevelTileSet.end(); it++){
								const LevelTile * pLevelTile = &(*it);
								FileOut << pLevelTile->SpriteID << endl;
								FileOut << pLevelTile->IsCeiling << endl;
								FileOut << pLevelTile->IsNotWall << endl;
								for(int i = 0; i < 64; i++){
									FileOut << pLevelTile->HeightMask[i] << endl;
								}
								FileOut	<< pLevelTile->Name << endl;
							}
							FileOut << AnimationSet.size() << endl;
							for(vector<AnimationTrack>::iterator it = AnimationSet.begin(); it < AnimationSet.end(); it++){
								const AnimationTrack * pAnim = &(*it);
								FileOut << pAnim->Name << endl
									<< pAnim->iStartingFrame << endl
									<< pAnim->nFrames << endl
									<< pAnim->fDefaultSpeed << endl;
							}
							FileOut << DynamicSet.size() << endl;
							for(vector<DynamicObject*>::iterator it = DynamicSet.begin(); it < DynamicSet.end(); it++){
								DynamicObject * pDyn = (*it);
								FileOut << pDyn->Type << endl;
								switch(pDyn->Type){
									case 0:{  //Walker
										EnemyWalker * pWalker = (EnemyWalker*)pDyn;
										FileOut << pWalker->AttackAnimationIndex << endl
												<< pWalker->DeathAnimationIndex << endl
												<< pWalker->JumpStrength << endl
												<< pWalker->Speed << endl
												<< pWalker->HP << endl
												<< pWalker->Aggressiveness << endl
												<< pWalker->bIsDormant << endl
												<< pWalker->ShotType << endl
												<< pWalker->ShotDirection << endl
												<< pWalker->nBurstDelay << endl
												<< pWalker->nShotsPerBurst << endl;
										break;
									}
									case 1:{  //Flyer
										EnemyFlyer * pWalker = (EnemyFlyer*)pDyn;
										FileOut << pWalker->AttackAnimationIndex << endl
												<< pWalker->DeathAnimationIndex << endl
												<< pWalker->Speed << endl
												<< pWalker->HP << endl
												<< pWalker->FlightType << endl
												<< pWalker->bIsDormant << endl
												<< pWalker->ShotType << endl
												<< pWalker->ShotDirection << endl
												<< pWalker->nBurstDelay << endl
												<< pWalker->nShotsPerBurst << endl;
										break;
									}
									case 2:{
											Mover * pMover = (Mover*)pDyn;
											FileOut << pMover->iMovingAnimationIndex << endl
													<< pMover->HP << endl
													<< pMover->bIsHazard << endl;
										break;
									}
									case 5:{
										DestructibleObject * pDest = (DestructibleObject*)pDyn;
										FileOut << pDest->HP << endl
											<< pDest->Type << endl
											<< pDest->SpawnID << endl;
									}
									default:
										break;
								}
								FileOut << pDyn->Name << endl
									<< pDyn->iDefaultAnimationIndex << endl
									<< pDyn->iSpriteIndex << endl
									<< pDyn->fDefaultScale << endl
									<< pDyn->CD.Left << endl
									<< pDyn->CD.Right << endl
									<< pDyn->CD.Top << endl
									<< pDyn->CD.Bottom << endl;
							}

							FileOut << HazardousSet.size() << endl;
							for(vector<HazardousObject>::iterator it = HazardousSet.begin(); it < HazardousSet.end(); it++){
								const HazardousObject * pHaz = &(*it);
								FileOut << pHaz->Name << endl
									<< pHaz->SpriteID << endl
									<< pHaz->Damage << endl
									<< pHaz->fDefaultScale << endl
									<< pHaz->CD.Left << endl
									<< pHaz->CD.Right << endl
									<< pHaz->CD.Top << endl
									<< pHaz->CD.Bottom << endl;
							}
							FileOut << DecoSet.size() << endl;
							for(vector<StaticDeco>::iterator it = DecoSet.begin(); it < DecoSet.end(); it++){
								const StaticDeco * pDeco = &(*it);
								FileOut << pDeco->Name << endl
									<< pDeco->iSpriteID << endl
									<< pDeco->fDefaultScale << endl
									<< pDeco->CD.Left << endl
									<< pDeco->CD.Right << endl
									<< pDeco->CD.Top << endl
									<< pDeco->CD.Bottom << endl;
							}
							FileOut << ProjectileSet.size() << endl;
							for(vector<DynamicObject*>::iterator it = ProjectileSet.begin(); it < ProjectileSet.end(); it++){
								const DynamicObject* pProj = *it;
								FileOut << pProj->Name << endl
									<< pProj->iDefaultAnimationIndex << endl
									<< pProj->fDefaultScale << endl
									<< pProj->CD.Left << endl
									<< pProj->CD.Right << endl
									<< pProj->CD.Top << endl
									<< pProj->CD.Bottom << endl;
							}
							FileOut.close();
						}
					}
					break;
				}
				case IDC_REMHAZ:{
					HWND hHazList = GetDlgItem(hwnd, IDC_HAZLIST);
					const int Selection = SendMessage(hHazList, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					if(Selection != LB_ERR){
						HazardousSet.erase(HazardousSet.begin() + Selection);
						SendMessage(hHazList, LB_DELETESTRING, (WPARAM)Selection, (LPARAM)0);
					}
					break;
				}
				case IDC_ADDHAZ:{
					HazardousObject * pHaz = new HazardousObject();
					pHaz->CD.Copy(Globals::PreviewCD);
					string LineEntry;
					pHaz->Damage = GetDlgItemInt(hwnd, IDC_HAZDMG, NULL, false);
					char strScale[8];
					GetDlgItemText(hwnd, IDC_HAZSCALE, strScale, 6);
					pHaz->fDefaultScale = (float)atof(strScale);

					pHaz->Name = "H:" + TextureAtlas.TextureList[SpriteID];
					pHaz->SpriteID = SpriteID;
					HazardousSet.push_back(*pHaz);

					LineEntry = pHaz->Name;
					HWND hHazList = GetDlgItem(hwnd, IDC_HAZLIST);
					SendMessage(hHazList, LB_ADDSTRING, 0, (LPARAM)LineEntry.c_str()); 
					break;
				}
				case IDC_ADDDYNAMIC:{
						HWND hDynamicList = GetDlgItem(hwnd, IDC_DYNLIST);
						const int Type = SendMessage(GetDlgItem(hwnd, IDC_DYNAMICCOMBO), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
						if(pEditObject){
							pEditObject = NULL;
						}
						int Result = IDCANCEL;
						switch(Type){
							case 0:
								Result = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADDWALKER), hwnd, AddDlgProc);
								break;
							case 1:
								Result = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADDFLYER), hwnd, AddFlyerDlgProc);
								break;
							case 2:
								Result = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADDMOVER), hwnd, AddMoverDlgProc);
								break;
							case 5:
								Result = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADDEST), hwnd, AddDestructibleDlgProc);
								break;
							default:
								Result = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADDDEFAULT), hwnd, AddDefault);
								break;
						}
						if(Result == IDOK){
							DynamicObject * pDyn = DynamicSet.back();
							pDyn->CD.Copy(Globals::PreviewCD);
							SendMessage(GetDlgItem(hwnd, IDC_DYNLIST), LB_ADDSTRING, 0, (LPARAM)pDyn->Name.c_str()); 
							pDyn->Type = Type;
						}
					break;
				}
				case IDC_EDITDYNAMIC:{
					break;
				}
				case IDC_DECOREMOVE:{
					HWND hDecoList = GetDlgItem(hwnd, IDC_DECOLIST);
					const int Selection = SendMessage(hDecoList, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					if(Selection != LB_ERR){
						DecoSet.erase(DecoSet.begin() + Selection);
						SendMessage(hDecoList, LB_DELETESTRING, (WPARAM)Selection, (LPARAM)0);
					}
				}
				case IDC_REMOVEDYNAMIC:{
					HWND hDynList = GetDlgItem(hwnd, IDC_DYNLIST);
					const int Selection = SendMessage(hDynList, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					if(Selection != LB_ERR){
						DynamicSet.erase(DynamicSet.begin() + Selection);
						SendMessage(hDynList, LB_DELETESTRING, (WPARAM)Selection, (LPARAM)0);
					}
				}
				case IDC_REMOVEANIM:{
					HWND hAnimList = GetDlgItem(hwnd, IDC_ANIMLIST);
					const int Selection = SendMessage(hAnimList, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					if(Selection != LB_ERR){
						AnimationSet.erase(AnimationSet.begin() + Selection);
						SendMessage(hAnimList, LB_DELETESTRING, (WPARAM)Selection, (LPARAM)0);
					}
					break;
				}
				case IDC_ADDPROJ:{
					HWND hAnimList = GetDlgItem(hwnd, IDC_ANIMLIST);
					const int Selection = SendMessage(hAnimList, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					if(Selection != LB_ERR){
						DynamicObject *pProj = new DynamicObject();
						pProj->fDefaultScale = GetDlgItemFloat(hwnd, IDC_PROJSCALE);
						pProj->Name = AnimationSet[Selection].Name;
						pProj->CD.Copy(Globals::PreviewCD);
						pProj->iDefaultAnimationIndex = Selection;
						ProjectileSet.push_back(pProj);

						HWND hProjectileList = GetDlgItem(hwnd, IDC_PROJECTILELIST);
						SendMessage(hProjectileList, LB_ADDSTRING, 0, (LPARAM)pProj->Name.c_str()); 
					}

					break;
				}
				case IDC_DECOADD:{
					StaticDeco *pDeco = new StaticDeco();
					char strScale[8];
					GetDlgItemText(hwnd, IDC_DECOSCALE, strScale, 6);
					pDeco->fDefaultScale = (float)atof(strScale);
					pDeco->iSpriteID = SpriteID;
					pDeco->Name = "S:" + TextureAtlas.TextureList[SpriteID];
					pDeco->CD.Copy(Globals::PreviewCD);
					HWND hListBox = GetDlgItem(hwnd, IDC_DECOLIST);
					string ListEntry = pDeco->Name;
					SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)ListEntry.c_str()); 
					DecoSet.push_back(*pDeco);
					break;
				}	
				case IDC_ANIMADD:{
					AnimationTrack * pAnim = new AnimationTrack();
					char strFrames[8];
					GetDlgItemText(hwnd, IDC_NFRAMES, strFrames, 6);
					char strFPS[8];
					GetDlgItemText(hwnd, IDC_SPEED, strFPS, 6);
					pAnim->nFrames =  atoi(strFrames);//(;//GetDlgItemInt(hwnd, IDC_NFRAMES, false, false);
					pAnim->fDefaultSpeed = (float)atof(strFPS);
					pAnim->iStartingFrame = SpriteID;
					string Name = TextureAtlas.TextureList[SpriteID];
					pAnim->Name = Name;
					char TruncName[7];
					size_t TruncNameLength = Name.copy(TruncName, 6,0);
					TruncName[TruncNameLength] = '\0';
					Name = TruncName;
					HWND hListBox = GetDlgItem(hwnd, IDC_ANIMLIST);
					string ListEntry = Name + "..." + " - " + strFrames + " Frames" + ", " + strFPS + " FPS";
					SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)ListEntry.c_str()); 

					AnimationSet.push_back(*pAnim);
					break;
				}
				case IDC_ADDTILE:{
					for(int i = SpriteID; i < SpriteID + 1; i++){
						const string Name = TextureAtlas.TextureList[i];

						string HeightMapName = Name;
						string::iterator it = HeightMapName.end() - 4;
						HeightMapName.replace(it, HeightMapName.end(), "_heightmask.png");
						HeightMapName.insert(0, "./");
													WCHAR wFileName[MAX_PATH];
								//Make sure we have .png at the end
								int sfLength = HeightMapName.length();
								// Convert to a wchar_t*
								size_t convertedChars = 0;
								mbstowcs_s(&convertedChars, wFileName, sfLength + 2, HeightMapName.c_str(), _TRUNCATE);

						LevelTile * pLevelTile = new LevelTile;
						bool bIsCeiling = IsDlgButtonChecked(hwnd, IDC_ISCEILING);

						GDIImageLoader::ConvertImageToHeightMask(wFileName, pLevelTile->HeightMask, bIsCeiling);
						pLevelTile->IsCeiling = (int)bIsCeiling;
						pLevelTile->IsNotWall = IsDlgButtonChecked(hwnd, IDC_ISWALL);
						pLevelTile->SpriteID = i;
						pLevelTile->Name = Name;

						LevelTileSet.push_back(*pLevelTile);
						SendMessage(GetDlgItem(hwnd, IDC_COLLIST), LB_ADDSTRING, 0, (LPARAM)pLevelTile->Name.c_str());
					}
					break;
			    }
				case IDC_IMAGECOMBO:{
					switch(HIWORD(wParam)){
						case CBN_SELCHANGE:{
							HWND hSpriteCombo = GetDlgItem(hwnd, IDC_IMAGECOMBO);
							SpriteID = SendMessage(hSpriteCombo, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
							break;
						}
						default:
							break;
					}
					break;
				}
				default:
					break;
			}
			
			break;
		}
		default:
			return FALSE;
	}
	return TRUE;
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	Globals::PreviewCD.Left = -1.0f;
	Globals::PreviewCD.Right = 1.0f;
	Globals::PreviewCD.Top = 1.0f;
	Globals::PreviewCD.Bottom = -1.0f;
	InitCommonControls();
	hAppInstance = hInstance;
	PreviewRenderer.AssociateTextureAtlas(&TextureAtlas);
	HWND hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	ShowWindow(hwnd, SW_SHOW);
	SetDlgItemText(hwnd, IDC_CDRIGHT, "1.0");
	SetDlgItemText(hwnd, IDC_CDTOP, "1.0");
	SetDlgItemText(hwnd, IDC_CDLEFT, "-1.0");
	SetDlgItemText(hwnd, IDC_CDBOTTOM, "-1.0");
	HWND hSpriteCombo = GetDlgItem(hwnd, IDC_IMAGECOMBO);

	for(std::vector<std::string>::iterator it = TextureAtlas.TextureList.begin(); it < TextureAtlas.TextureList.end(); it++){
		const string SpriteName = *it;
		SendMessage(hSpriteCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) SpriteName.c_str()); 
	}
	HWND hDynCombo = GetDlgItem(hwnd, IDC_DYNAMICCOMBO);
	for(int i = 0; i < nDynamicLabels; i++){
		SendMessage(hDynCombo,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) DynamicLabels[i].c_str()); 
	}
	SendMessage(hDynCombo,(UINT) CB_SETCURSEL,(WPARAM) 0,(LPARAM)0); 
	HWND PreviewPanel = GetDlgItem(hwnd, IDC_PREVIEW);
	RECT PreviewRect;
	GetWindowRect(PreviewPanel, &PreviewRect);
	const int Width = PreviewRect.right - PreviewRect.left;
	const int Height = PreviewRect.bottom - PreviewRect.top;
	PreviewRenderer.Initialize(PreviewPanel, Width, Height);
	SendMessage(hSpriteCombo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0) > 0){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		PreviewRenderer.Draw(SpriteID);
	}
	return 0;
}