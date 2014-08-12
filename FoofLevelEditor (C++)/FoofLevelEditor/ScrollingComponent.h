#pragma once
#include <Windows.h>
class ScrollingComponent
{
	HWND hwnd;
	int ScrollX;
	int ScrollY;

	int MaxX;
	int MaxY;
	int MinX;
	int MinY;
public:
	void UpdateScrollbar(int nBar, int ScrollMin, int ScrollMax, int PageSize);
	void AssociateWindow(HWND hNewWin);

	void ScrollHorizontally(WPARAM wParam);
	void ScrollVertically(WPARAM wParam);
	int GetScrollX();
	int GetScrollY();
	void UpdateCamY(int GridHeight, int ScreenHeightPixels);
	void UpdateCamX(int GridWidth, int ScreenWidthPixels, float Ratio);
	float GetXScrollFraction();
	float GetInvScrollYFraction();
	int GetInvertedScrollY();  //To remedy the problem of reverse scrolling 
	ScrollingComponent(void);
	~ScrollingComponent(void);
};

