#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include "resource.h"
#include "ExtendedObject.h"

class ToolBoxWindow
{
	HWND hwnd;
	HWND hPanel;
	HINSTANCE hCurrentInstance;
public:
	void Kill();
	ToolBoxWindow(HINSTANCE hInstance, HWND hParent);
	void NewPanel(int ID);
	int GetCurrentBrushBehavior();
	ExtendedObject * GetObjectInfo(int ListID);
	int GetBrushImageID();
	~ToolBoxWindow(void);
};

