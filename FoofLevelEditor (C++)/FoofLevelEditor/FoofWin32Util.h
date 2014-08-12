#pragma once
#include <Windows.h>
#include "ScrollingComponent.h"
//--------------------------------------------
// Basic utilities to start win32 apps faster
//============================================
namespace FoofWin32Util{
	HWND CreateScaledWindow(HINSTANCE hInstance, LPCSTR cClassName, LPCSTR cWindowTitle, DWORD dwStyle, BOOL bMenu, int x, int y, int Width, int Height);
	float GetDlgItemFloat(HWND hwnd, int nDlgItem);
	void SetDlgItemFloat(HWND hwnd, int nDlgItem, float Value);
};