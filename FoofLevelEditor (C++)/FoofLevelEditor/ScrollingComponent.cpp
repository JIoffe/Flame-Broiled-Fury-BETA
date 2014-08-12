#include "ScrollingComponent.h"
#include "LevelScene.h"
#include "Globals.h"

ScrollingComponent::ScrollingComponent(void)
{
}


ScrollingComponent::~ScrollingComponent(void)
{
}
void ScrollingComponent::AssociateWindow(HWND hNewWin){
	hwnd = hNewWin;
	return;
}

void ScrollingComponent::UpdateScrollbar(int nBar, int ScrollMin, int ScrollMax, int PageSize){
	if(hwnd){
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
		si.nMax = ScrollMax;
		si.nMin = ScrollMin;
		if(nBar == SB_VERT){
			si.nPos = max(0,min(ScrollY, ScrollMax));
			ScrollY = si.nPos;
			MaxY = ScrollMax;
			MinY = ScrollMin;
		}else{
			si.nPos = max(0,min(ScrollX, ScrollMax));
			ScrollX = si.nPos;
			MaxX = ScrollMax;
			MinX = ScrollMin;
		}
		si.nPage = PageSize;

		SetScrollInfo(hwnd, nBar, &si, TRUE);
	}
	return;
}
void ScrollingComponent::ScrollHorizontally(WPARAM wParam){
	int xNewPos = 0;    
	switch (LOWORD(wParam)){ 
		case SB_PAGEUP: 
			xNewPos = ScrollX - 50; 
			break; 
        case SB_PAGEDOWN: 
			xNewPos = ScrollX + 50; 
			break; 
        case SB_LINEUP: 
			xNewPos = ScrollX - 5; 
			break; 
        case SB_LINEDOWN: 
			xNewPos = ScrollX + 5; 
			break; 
        case SB_THUMBPOSITION: 
			xNewPos = HIWORD(wParam); 
			break; 
		case SB_THUMBTRACK:   //Constantly update as we drag
			xNewPos = HIWORD(wParam);
			break;
        default: 
			xNewPos = ScrollX; 
			break;
	} 
		xNewPos = min(MaxX, max(0, xNewPos));
		if(xNewPos == ScrollX){
			return;
		}else{
			ScrollX = xNewPos;
			SCROLLINFO si;
			si.cbSize = sizeof(si); 
            si.fMask  = SIF_POS; 
            si.nPos   = xNewPos; 
            SetScrollInfo(hwnd, SB_HORZ, &si, TRUE); 
		}
		return;
}
void ScrollingComponent::ScrollVertically(WPARAM wParam){
	int xNewPos = 0;    
	switch (LOWORD(wParam)){ 
		case SB_PAGEUP: 
			xNewPos = ScrollY - 50; 
			break; 
        case SB_PAGEDOWN: 
			xNewPos = ScrollY + 50; 
			break; 
        case SB_LINEUP: 
			xNewPos = ScrollY - 5; 
			break; 
        case SB_LINEDOWN: 
			xNewPos = ScrollY + 5; 
			break; 
        case SB_THUMBPOSITION: 
			xNewPos = HIWORD(wParam); 
			break; 
		case SB_THUMBTRACK:   //Constantly update as we drag
			xNewPos = HIWORD(wParam);
			break;
        default: 
			xNewPos = ScrollY; 
			break;
	} 
		xNewPos = min(MaxY, max(0, xNewPos));
		if(xNewPos == ScrollY){
			return;
		}else{
			ScrollY = xNewPos;
			SCROLLINFO si;
			si.cbSize = sizeof(si); 
            si.fMask  = SIF_POS; 
            si.nPos   = xNewPos; 
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE); 
		}
		return;
}
int ScrollingComponent::GetScrollX(){
	return ScrollX;
}
float ScrollingComponent::GetXScrollFraction(){
	return (float)ScrollX/MaxX;
}
int ScrollingComponent::GetScrollY(){
	return ScrollY;
}
float ScrollingComponent::GetInvScrollYFraction(){
	return (float)(ScrollY) / MaxY;
}
int ScrollingComponent::GetInvertedScrollY(){
	return MaxY-ScrollY; 
}
void ScrollingComponent::UpdateCamX(int GridWidth, int ScreenWidthPixels, float Ratio){
	Globals::CamX = Ratio + ((float)ScrollX / ScreenWidthPixels) * (GridWidth * LevelScene::LevelTileOffset);
	return;
}
void ScrollingComponent::UpdateCamY(int GridHeight, int ScreenHeightPixels){
	Globals::CamY = ((float)(ScreenHeightPixels - ScrollY) / ScreenHeightPixels) * (GridHeight * LevelScene::LevelTileOffset) -1.0f;
	return;
}