#include "LevelScene.h"
#include "Globals.h"
#include <Windows.h>
const float LevelScene::LevelTileScale = 0.15f;
const float LevelScene::LevelTileOffset = LevelTileScale * 2;
const float LevelScene::CoinWidth = 0.082f;
const float LevelScene::EnemyAWidth = 0.22f;
const float LevelScene::PlayerWidth = 0.2f;
const float LevelScene::PropBWidth = 0.095f;

void LevelScene::InitializeGrid(int Width, int Height){
	GridHeight = Height;
	GridWidth = Width;
	const int Area = Width * Height;
	if(pLevelGrid){
		delete[] pLevelGrid;
	}
	pLevelGrid = new int[Area];
	//Init to -1
	for(int i = 0; i < Area; i++){
		pLevelGrid[i] = -1; 
	}
	//We might as well also re-init the prop tree
	if(this->PropList.size() > 0){
		for(std::vector<PropStruct*>::iterator it = PropList.begin(); it < PropList.end(); it++){
			delete (*it);
		}
		PropList.clear();
	}
	if(this->EnemyList.size() > 0){
		for(std::vector<PropStruct*>::iterator it = EnemyList.begin(); it < EnemyList.end(); it++){
			delete (*it);
		}
		EnemyList.clear();
	}
}
int LevelScene::GetHeight(){
	return GridHeight;
}
int LevelScene::GetWidth(){
	return GridWidth;
}
int LevelScene::GetGridUnit(int x, int y){
	if( x < 0 || x >= GridWidth || y < 0 || y >= GridHeight){
		return -1;
	}else{
		return pLevelGrid[y * GridWidth + x];
	}
}
void LevelScene::SetGridUnit(int x, int y, int value){
	pLevelGrid[y * GridWidth + x] = value;
	return;
}
bool LevelScene::GridOk(){
	return (pLevelGrid);
}
void LevelScene::SetPlayerStart(float x, float y){
	PlayerStartX = x;
	PlayerStartY = y;
	return;
}
float LevelScene::GetPlayerStartX(){
	return PlayerStartX;
}
float LevelScene::GetPlayerStartY(){
	return PlayerStartY;
}
void LevelScene::Update(){
	mPlayer.Update();
	for(int i = 0; i < Globals::nDynamics; i++){
		ExtendedObject * pDyn = Globals::DynamicSet[i];
		if(pDyn->mAnimation.pTrack){
			pDyn->iSpriteIndex = pDyn->mAnimation.AdvanceFrame();
		}
	}
		for(std::vector<PropStruct*>::iterator it = EnemyList.begin(); it < EnemyList.end(); it++){
			(*it)->Update();
		}
	return;
}

void LevelScene::ResetPlayerPosition(){
	float DeltaX = PlayerStartX - mPlayer.X;
	float DeltaY = PlayerStartY - mPlayer.Y;
	mPlayer.X = PlayerStartX;
	mPlayer.Y = PlayerStartY;
	mPlayer.CD.Translate(DeltaX, DeltaY);
	return;
}
void LevelScene::ResetScene(){
	int nObjects = HazardList.size();
	//we're trying to prevent memory leaks so... go through deleting
	for(int i = 0; i < nObjects; i++){
		if(HazardList[i]){
			delete HazardList[i];
			HazardList[i] = NULL;
		}
	}
	HazardList.clear();

	nObjects = PropList.size();
	//we're trying to prevent memory leaks so... go through deleting
	for(int i = 0; i < nObjects; i++){
		if(PropList[i]){
			delete PropList[i];
			PropList[i] = NULL;
		}
	}
	PropList.clear();

	nObjects = EnemyList.size();
	//we're trying to prevent memory leaks so... go through deleting
	for(int i = 0; i < nObjects; i++){
		if(EnemyList[i]){
			delete EnemyList[i];
			EnemyList[i] = NULL;
		}
	}
	EnemyList.clear();

	nObjects = DecoList.size();
	for(int i = 0; i < nObjects; i++){
		if(DecoList[i]){
			delete DecoList[i];
			DecoList[i] = NULL;
		}
	}
	DecoList.clear();

	nObjects = CamList.size();
	//we're trying to prevent memory leaks so... go through deleting
	for(int i = 0; i < nObjects; i++){
		if(CamList[i]){
			delete CamList[i];
			CamList[i] = NULL;
		}
	}
	CamList.clear();
	ResetPlayerPosition();
	return;
}
LevelScene::LevelScene(){
	GridHeight = 0;
	GridWidth = 0;
	pLevelGrid = NULL;
	SetPlayerStart(0,0);
}
LevelScene::~LevelScene(){
}
