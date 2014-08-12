#include "FoofWin32Util.h"
#include <stdio.h>

HWND FoofWin32Util::CreateScaledWindow(HINSTANCE hInstance, LPCSTR cClassName, LPCSTR cWindowTitle, DWORD dwStyle, BOOL bMenu, int x, int y, int Width, int Height){
	//Creates a window with a client area with the exact dimensions of Width and Height

	RECT WinRect;
	WinRect.left = 0;
	WinRect.top = 0;
	WinRect.bottom = Height;
	WinRect.right = Width;

	AdjustWindowRectEx(&WinRect, dwStyle, bMenu, WS_EX_CLIENTEDGE);
	//AdjustWindowRect doesn't take scroll bars into consideration
	if(dwStyle & WS_VSCROLL){
		WinRect.right += GetSystemMetrics(SM_CXVSCROLL);
	}
	if(dwStyle & WS_HSCROLL){
		WinRect.bottom += GetSystemMetrics(SM_CXHSCROLL);
	}
	return CreateWindowEx(
        WS_EX_CLIENTEDGE,
        cClassName,
        cWindowTitle,
        dwStyle,
		x, y, WinRect.right - WinRect.left, WinRect.bottom - WinRect.top,
        NULL, NULL, hInstance, NULL);
}
float FoofWin32Util::GetDlgItemFloat(HWND hwnd, int nDlgItem){
	char strFloat[MAX_PATH];
	GetDlgItemText(hwnd, nDlgItem, strFloat, MAX_PATH);
	return (float)atof(strFloat);
}
void FoofWin32Util::SetDlgItemFloat(HWND hwnd, int nDlgItem, float Value){
	char strFloat[MAX_PATH];
	sprintf(strFloat, "%f", Value);
	SetDlgItemText(hwnd, nDlgItem, strFloat);
	return;
}