#include "LevelFileIO.h"
#include "Globals.h"
#include "Collision.h"

using namespace std;
bool LevelFileIO::SaveLevelToDisk(const char * cFilePath, LevelScene * pScene){
	if(pScene && pScene->GridOk()){
		const int GridWidth = pScene->GetWidth();
		const int GridHeight = pScene->GetHeight();

		ofstream FileOut(cFilePath);
		if(FileOut.is_open()){
			FileOut << "Foof72" << endl;

			FileOut << GridWidth << endl;
			FileOut << GridHeight << endl;
			const int Total = GridWidth * GridHeight;
			for(int y = 0; y < GridHeight; y++){
				//const int YSkip = y * GridWidth;
				for(int x = 0; x < GridWidth; x++){
					//Output the level grid one tile at a time
					const int TileID = pScene->GetGridUnit(x,y);
					FileOut << TileID << endl;
				}
			}
			const float PlayerStartX = pScene->GetPlayerStartX();
			const float PlayerStartY = pScene->GetPlayerStartY();
			//Then player start location
			if(PlayerStartX > -1 && PlayerStartY > -1){
				FileOut << PlayerStartX << endl;
				FileOut << PlayerStartY << endl;
			}else{
				FileOut << 0 << endl;
				FileOut << 0 << endl;
			}
			//Exit Location
			FileOut << pScene->ExitPos_X << endl
				<< pScene->ExitPos_Y << endl;

			//Output total number of game objects in level
			FileOut << pScene->PropList.size() + pScene->EnemyList.size() + pScene->DecoList.size() + pScene->HazardList.size() + pScene->TokenList.size() << endl;

			//Output props (coins , etc) if they exist
			const int nProps = pScene->PropList.size();
			FileOut << nProps << endl;
			if(nProps > 0){
				for(vector<PropStruct*>::iterator it = pScene->PropList.begin(); it < pScene->PropList.end(); it++){
					PropStruct * pProp = (*it);
					FileOut << ((int)pProp->uType - 1) << endl
							<< pProp->x << endl
							<< pProp->y << endl;
				}
			}
			//Output ENEMIES
			const int nEnemies = pScene->EnemyList.size();
			FileOut << nEnemies << endl;
			if(nEnemies > 0){
				for(vector<PropStruct*>::iterator it = pScene->EnemyList.begin(); it < pScene->EnemyList.end(); it++){
					PropStruct * pProp = (*it);
					FileOut << pProp->ExtendedInfo->ArrayID << endl
							<< pProp->x << endl
							<< pProp->y << endl;

					switch(pProp->ExtendedInfo->SubType){
						case 2:{
							Mover * pMover = (Mover*)pProp;
							FileOut << (int)pMover->GetFunctionType() << endl
								<< (int)pMover->GetMoverType() << endl 
								<< pMover->OriginX << endl
								<< pMover->OriginY << endl
								<< pMover->GetSpeed() << endl
								<< pMover->GetWidth() << endl
								<< pMover->GetPause() << endl
								<< pMover->Sync << endl;
							break;
						}
						case 3:{ //Snapmouth

							break;
						}
						case 5:{ // Destructible
							FileOut << (pProp->x - pProp->ExtendedInfo->fDefaultScale) << endl
								     << (pProp->x + pProp->ExtendedInfo->fDefaultScale) << endl;
							break;
						}
						case 0:{
							//Compute Potential Visibility Bounds
							float VisibilityMax, VisibilityMin;
							float XDelta = 0.1f;

							float X = pProp->x;
							float Y = TraceFloorHeight(X, pProp->y) + 0.1f;
							float PrevY = Y;
							while(abs(Y - PrevY) < 0.15f){
								PrevY = Y;
								X += XDelta;
								Y = TraceFloorHeight(X, Y) + 0.1f;
							}
							VisibilityMax = X - XDelta*2.0f;
							
							X = pProp->x;
							Y = TraceFloorHeight(X, pProp->y) + 0.1f;
							PrevY = Y;
							while(abs(Y - PrevY) < 0.15f){
								PrevY = Y;
								X -= XDelta;
								Y = TraceFloorHeight(X, Y) + 0.01f;
							}
							VisibilityMin = X + XDelta*2.0f;

							FileOut << VisibilityMin << endl
								     << VisibilityMax << endl;
							break;
						}
						default:
							break;
					}
				}
			}
			FileOut << pScene->HazardList.size() << endl;
			for(vector<PropStruct*>::iterator it = pScene->HazardList.begin(); it < pScene->HazardList.end(); it++){
					PropStruct * pProp = (*it);
					FileOut << (int)pProp->ExtendedInfo->ArrayID << endl
							<< pProp->x << endl
							<< pProp->y << endl;
			}
			FileOut << pScene->DecoList.size() << endl;
			for(vector<PropStruct*>::iterator it = pScene->DecoList.begin(); it < pScene->DecoList.end(); it++){
					PropStruct * pProp = (*it);
					FileOut << (int)pProp->ExtendedInfo->ArrayID << endl
							<< pProp->x << endl
							<< pProp->y << endl;
			}
			FileOut << pScene->TokenList.size() << endl;
			for(vector<PropStruct*>::iterator it = pScene->TokenList.begin(); it < pScene->TokenList.end(); it++){
					PropStruct * pProp = (*it);
					FileOut << pProp->x << endl
							<< pProp->y << endl;
			}
			FileOut << pScene->BunnyList.size() << endl;
			for(vector<PropStruct*>::iterator it = pScene->BunnyList.begin(); it < pScene->BunnyList.end(); it++){
					PropStruct * pProp = (*it);
					FileOut << pProp->x << endl
							<< pProp->y << endl;
			}
			const int nCamZones = pScene->CamList.size();
			FileOut << nCamZones << endl;
			if(nCamZones > 0){
				for(vector<CamZone*>::iterator it = pScene->CamList.begin(); it < pScene->CamList.end(); it++){
					CamZone * pCam = *it;
					FileOut << pCam->x << endl
							<< pCam->y << endl
							<< pCam->Left << endl
							<< pCam->Right << endl
							<< pCam->Up << endl
							<< pCam->Down << endl
							<< pCam->nAxis << endl;
				}
			}
			FileOut.close();
			return true;
		}
	}
	return false;
}
bool LevelFileIO::OpenLevelFromFile(const char * cFilePath, LevelScene * pScene){
	if(pScene && pScene->GridOk()){
		ifstream FileIn(cFilePath);
		if(FileIn.is_open()){
			string LineReader;
			FileIn >> LineReader;
			if(LineReader == "Foof72"){
				int GridWidth = 0, GridHeight = 0;
				FileIn >> GridWidth;
				FileIn >> GridHeight;
				pScene->InitializeGrid(GridWidth, GridHeight);
				for(int y = 0; y < GridHeight; y++){
					//const int YSkip = y * GridWidth;
					for(int x = 0; x < GridWidth; x++){
						//const int Index = YSkip + x;
						int GridValue = 0;
						FileIn >> GridValue;
						pScene->SetGridUnit(x,y,GridValue);
					}
				}
				//Then Player
				float PlayerX, PlayerY;
				FileIn >> PlayerX;
				FileIn >> PlayerY;
				pScene->SetPlayerStart(PlayerX, PlayerY);
				pScene->ResetPlayerPosition();
				FileIn >> pScene->ExitPos_X >> pScene->ExitPos_Y;


				//Then props if any
				//Ignore grand object total
				int nProps = 0;
				FileIn >> nProps;
				FileIn >> nProps;
				if(nProps > 0){
					for(int i = 0; i < nProps; i++){
						PropStruct * pProp = new PropStruct;
						pProp->bIsSelected = false;
						FileIn >> pProp->uType;
						pProp->uType++;
						FileIn >> pProp->x;
						FileIn >> pProp->y;

						pProp->nID = pScene->PropList.size();
						pProp->uObjectType = PropStruct::Collectible;
						pScene->PropList.push_back(pProp);
					}
				}
				int nEnemies = 0;
				FileIn >> nEnemies;
				if(nEnemies > 0){
					for(int i = 0; i < nEnemies; i++){
						PropStruct * pProp = NULL;
	
						unsigned int Type = 0;
						float X = 0.0f, Y = 0.0f;
						FileIn >> Type;
						FileIn >> X;
						FileIn >> Y;
						
						
						
						ExtendedObject * ExtendedInfo = Globals::DynamicSet[Type];
						switch(ExtendedInfo->SubType){
							case 2:{
								pProp = new Mover;
								Mover * pMover = (Mover*)pProp;
								float Speed, Width, Pause;
								unsigned int FunctionType, MoverType;
								FileIn >> FunctionType
									>> MoverType 
									>> pMover->OriginX
									>> pMover->OriginY
									>> Speed
									>> Width
									>> Pause
									>> pMover->Sync;
								pMover->SetSpeed(Speed);
								pMover->SetPause(Pause);
								pMover->SetWidth(Width);
								pMover->SetMoverType(MoverType);
								pMover->SetFunctionType(FunctionType);
								break;
							}
							case 3:{
								pProp = new PropStruct;
								break;
							}
							case 0:{
								pProp = new PropStruct;
								float Eater;
								FileIn >> Eater >> Eater;
								break;
							}
							case 5:{
								pProp = new PropStruct;
								float Eater;
								FileIn >> Eater >> Eater;
								break;
							}
							default:
								pProp = new PropStruct;
								break;
						}
						pProp->uType = Type;
						pProp->bIsSelected = false;
						pProp->ExtendedInfo = ExtendedInfo;
						pProp->uObjectType = PropStruct::Enemy;
						pProp->x = X;
						pProp->y = Y;
						pProp->nID = pScene->EnemyList.size();
						pScene->EnemyList.push_back(pProp);
					}
				}
				int nHazards = 0;
				FileIn >> nHazards;
				if(nHazards > 0){
					for(int i = 0; i < nHazards; i++){
						PropStruct * pProp = new PropStruct;
						pProp->bIsSelected = false;
						FileIn >> pProp->uType;
						FileIn >> pProp->x;
						FileIn >> pProp->y;
						//pProp->uObjectType = PropStruct::Enemy;
						pProp->nID = pScene->HazardList.size();
						pProp->ExtendedInfo = &Globals::HazardousSet[pProp->uType];
						pScene->HazardList.push_back(pProp);
					}
				}
				int nDecos = 0;
				FileIn >> nDecos;
				if(nDecos > 0){
					for(int i = 0; i < nDecos; i++){
						PropStruct * pProp = new PropStruct;
						pProp->bIsSelected = false;
						FileIn >> pProp->uType;
						FileIn >> pProp->x;
						FileIn >> pProp->y;
						//pProp->uObjectType = PropStruct::Enemy;
						pProp->nID = pScene->DecoList.size();
						pProp->ExtendedInfo = &Globals::DecoSet[pProp->uType];
						pScene->DecoList.push_back(pProp);
					}
				}
				int nTokens = 0;
				FileIn >> nTokens;
				if(nTokens > 0){
					for(int i = 0; i < nTokens; i++){
						PropStruct * pProp = new PropStruct;
						pProp->bIsSelected = false;
						pProp->uType = PropStruct::Token;
						FileIn >> pProp->x;
						FileIn >> pProp->y;
						pProp->nID = pScene->TokenList.size();
						pProp->ExtendedInfo = NULL;;
						pScene->TokenList.push_back(pProp);
					}
				}
				int nBunnies = 0;
				FileIn >> nBunnies;
				if(nBunnies > 0){
					for(int i = 0; i < nBunnies; i++){
						PropStruct * pProp = new PropStruct;
						pProp->bIsSelected = false;
						pProp->uType = PropStruct::Bunny;
						FileIn >> pProp->x;
						FileIn >> pProp->y;
						pProp->nID = pScene->BunnyList.size();
						pProp->ExtendedInfo = NULL;;
						pScene->BunnyList.push_back(pProp);
					}
				}
			}
			int nCamZones = 0;
			FileIn >> nCamZones;
			if(nCamZones > 0){
				for(int i = 0; i < nCamZones; i++){
					CamZone * pCam = new CamZone;

					FileIn  >> pCam->x 
							>> pCam->y 
							>> pCam->Left
							>> pCam->Right
							>> pCam->Up 
							>> pCam->Down
							>> pCam->nAxis;
					pCam->nID = pScene->CamList.size();
					pCam->uObjectType = PropStruct::CameraZone;
					pCam->bIsSelected = false;

					pScene->CamList.push_back(pCam);
				}
			}
			FileIn.close();
		}
	}
	return true;
}
bool LevelFileIO::OpenLevelTileSet(const char * cFilePath){
	ifstream FileIn(cFilePath);
	if(FileIn.is_open()){
		string LineReader;
		FileIn >> LineReader;
		if(LineReader == "Foof72"){
			int Size = 0;
			FileIn >> Size;
			Globals::nLevelTileTypes = Size;
			if(Size > 0){
				Globals::TileSet = new LevelTile[Size];
				for(int i = 0; i < Size; i++){
					LevelTile * pLevelTile = &Globals::TileSet[i];
					FileIn >> pLevelTile->SpriteID;
					FileIn >> pLevelTile->IsCeiling;
					FileIn >> pLevelTile->IsNotWAll;
					for(int i = 0; i < 64; i++){
						FileIn >> pLevelTile->HeightMask[i];
					}
					FileIn >> pLevelTile->Name;
				}
				FileIn >> Size;
				Globals::pAnimationTrackSet = new AnimationTrack[Size];
				for(int i = 0; i < Size; i++){
					AnimationTrack * pAnim = &Globals::pAnimationTrackSet[i];
					FileIn >> pAnim->Name;
					FileIn >> pAnim->iStartingFrame;
					FileIn >> pAnim->nFrames;
					FileIn >> pAnim->fDefaultSpeed;
				}
				FileIn >> Size;
				Globals::DynamicSet = new ExtendedObject*[Size];
				Globals::nDynamics = Size;
				for(int i = 0; i < Size; i++){
					
					int Type;
					FileIn >> Type;
					switch(Type){
					case 0:{  //Walker
						Globals::DynamicSet[i] = new EnemyWalker;
						EnemyWalker * pWalker = (EnemyWalker*)Globals::DynamicSet[i];
						FileIn >> pWalker->AttackAnimationIndex
							>> pWalker->DeathAnimationIndex
							>> pWalker->JumpStrength
							>> pWalker->Speed
							>> pWalker->HP
							>> pWalker->bChargesPlayer
							>> pWalker->bIsDormant
							>> pWalker->ShotType
							>> pWalker->ShotDirection
							>> pWalker->ShotDelay
							>> pWalker->nShots;
						break;
					}
					case 1:{  //This is really a flyer...
						Globals::DynamicSet[i] = new EnemyWalker;
						EnemyWalker * pWalker = (EnemyWalker*)Globals::DynamicSet[i];
						FileIn >> pWalker->AttackAnimationIndex
							>> pWalker->DeathAnimationIndex
							>> pWalker->Speed
							>> pWalker->HP
							>> pWalker->bChargesPlayer
							>> pWalker->bIsDormant
							>> pWalker->ShotType
							>> pWalker->ShotDirection
							>> pWalker->ShotDelay
							>> pWalker->nShots;
						break;
					}
					case 2:{ // MOVER
						Globals::DynamicSet[i] = new MoverProp;
						MoverProp * pMover = (MoverProp*)Globals::DynamicSet[i];
						FileIn >> pMover->iMovingAnimationIndex
								>> pMover->HP
								>> pMover->bIsHazard;
						break;
					}
					case 5:{
						Globals::DynamicSet[i] = new DestructibleObject;
						DestructibleObject * pDest = (DestructibleObject*)Globals::DynamicSet[i];
						FileIn >> pDest->HP
							>> pDest->Type
							>> pDest->SpawnID;
						break;
					}
					default:
						Globals::DynamicSet[i] = new ExtendedObject;
						break;
					}
					ExtendedObject * pDyn = Globals::DynamicSet[i];
					pDyn->SubType = Type;
					FileIn >> pDyn->Name
						>> pDyn->iDefaultAnimationIndex
						>> pDyn->iBaseSpriteIndex
						>> pDyn->fDefaultScale
						>> pDyn->CD.Left
						>> pDyn->CD.Right
						>> pDyn->CD.Top
						>> pDyn->CD.Bottom;
					pDyn->iSpriteIndex = pDyn->iBaseSpriteIndex;
					if(pDyn->iDefaultAnimationIndex > -1){
						pDyn->mAnimation.pTrack = &Globals::pAnimationTrackSet[pDyn->iDefaultAnimationIndex];
					}
					pDyn->Type = 1; // dynamic
					pDyn->ArrayID = i;
				}
				FileIn >> Size;
				Globals::nHazards = Size;
				Globals::HazardousSet = new HazardousObject[Size];
				for(int i = 0; i < Size; i++){
					HazardousObject * pHaz = &Globals::HazardousSet[i];
					pHaz->ArrayID = i;
					FileIn >> pHaz->Name;
					FileIn >> pHaz->SpriteID;
					FileIn >> pHaz->Damage;
					FileIn >> pHaz->fDefaultScale
						>> pHaz->CD.Left
						>> pHaz->CD.Right
						>> pHaz->CD.Top
						>> pHaz->CD.Bottom;
				}
				FileIn >> Size;
				Globals::nDecos = Size;
				Globals::DecoSet = new StaticDeco[Size];
				for(int i = 0; i < Size; i++){
					StaticDeco * pDeco = &Globals::DecoSet[i];
					pDeco->ArrayID = i;
					FileIn >> pDeco->Name;
					FileIn >> pDeco->iSpriteID;
					FileIn >> pDeco->fDefaultScale
						>> pDeco->CD.Left
						>> pDeco->CD.Right
						>> pDeco->CD.Top
						>> pDeco->CD.Bottom;
				}
				return true;
			}
		}
	}
	return false;
}