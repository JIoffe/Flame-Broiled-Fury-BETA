#include "ToolBoxWindow.h"
#include "Globals.h"
#include <cstdio>
#include <string>
#include <vector>

const int BEHAVIOR_VERTICALLINE = -2;
const int BEHAVIOR_HORIZONTALLINE = -1;
const int BEHAVIOR_BUCKETFILL = 0;
const int BEHAVIOR_DRAWLEVELTILE = 1;
const int BEHAVIOR_DRAWNOOVERRIDE = 2;
const int BEHAVIOR_ERASELEVELTILE = 3;
const int BEHAVIOR_LAYPROP = 4;
const int BEHAVIOR_SELMOVEPROP = 5;
int CurrentBrushBehavior;
int CurrentPanelID = 0;
ToolBoxWindow * pActiveToolbox = NULL;

BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		case WM_NOTIFY:{
			UINT code = ((LPNMHDR)lParam)->code;
			switch(code){
				case TCN_SELCHANGE:{
					const int TabSelection = SendMessage(GetDlgItem(hwnd, IDC_TOOLTABS), TCM_GETCURSEL, 0 , 0 );
					if(TabSelection > -1){
						if(Globals::pObjectBrush){
							delete Globals::pObjectBrush;
							Globals::pObjectBrush = NULL;
						}
						switch(TabSelection){
							case 0:{
								pActiveToolbox->NewPanel(IDD_LEVELGRID);
								break;
							}
							case 1:{
								pActiveToolbox->NewPanel(IDD_PROPS);
								break;
							}
							default:
								break;
						}
					}
					break;
				}
				default:
					break;
			}
			break;
		}
		case WM_COMMAND:{
			switch(HIWORD(wParam)){
				case LBN_SELCHANGE:{
					switch(CurrentPanelID){
						case IDD_PROPS:{
							if(Globals::pObjectBrush){
								delete Globals::pObjectBrush;
							}
							Globals::pObjectBrush = new PropStruct();
							Globals::pObjectBrush->uType = (int)SendMessage(GetDlgItem(hwnd, IDC_LIST), LB_GETCURSEL, 0, 0) + 1;
							if(Globals::pObjectBrush->uType > 9){
								Globals::pObjectBrush->ExtendedInfo = (ExtendedObject*)SendMessage(GetDlgItem(hwnd, IDC_LIST), LB_GETITEMDATA, Globals::pObjectBrush->uType -1, 0);
							}
							break;
						}
						default:
							break;
					}
					break;
				}
				case BN_CLICKED:{
					const DWORD ID = LOWORD(wParam);
					CheckDlgButton(hwnd, ID, BST_CHECKED);
					switch(ID){
						case IDC_DRAWLEVELTILE:{
							CurrentBrushBehavior = BEHAVIOR_DRAWLEVELTILE;
							CheckDlgButton(hwnd, IDC_ERASELEVELTILE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_DRAWNOOVERRIDE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_BUCKETFILL, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_HORLINE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_VERLINE, BST_UNCHECKED);
							break;
						}
						case IDC_DRAWNOOVERRIDE:{
							CurrentBrushBehavior = BEHAVIOR_DRAWNOOVERRIDE;
							CheckDlgButton(hwnd, IDC_ERASELEVELTILE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_DRAWLEVELTILE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_BUCKETFILL, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_HORLINE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_VERLINE, BST_UNCHECKED);
							break;
						}
						case IDC_ERASELEVELTILE:{
							CurrentBrushBehavior = BEHAVIOR_ERASELEVELTILE;
							CheckDlgButton(hwnd, IDC_DRAWLEVELTILE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_DRAWNOOVERRIDE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_BUCKETFILL, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_HORLINE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_VERLINE, BST_UNCHECKED);
							break;
						}
						case IDC_BUCKETFILL:{
							CurrentBrushBehavior = BEHAVIOR_BUCKETFILL;
							CheckDlgButton(hwnd, IDC_DRAWLEVELTILE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_DRAWNOOVERRIDE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_ERASELEVELTILE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_HORLINE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_VERLINE, BST_UNCHECKED);
							break;
						}
						case IDC_VERLINE:{
							CurrentBrushBehavior = BEHAVIOR_VERTICALLINE;
							CheckDlgButton(hwnd, IDC_DRAWLEVELTILE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_DRAWNOOVERRIDE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_ERASELEVELTILE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_HORLINE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_BUCKETFILL, BST_UNCHECKED);
							break;
						}
						case IDC_HORLINE:{
							CurrentBrushBehavior = BEHAVIOR_HORIZONTALLINE;
							CheckDlgButton(hwnd, IDC_DRAWLEVELTILE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_DRAWNOOVERRIDE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_ERASELEVELTILE, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_BUCKETFILL, BST_UNCHECKED);
							CheckDlgButton(hwnd, IDC_VERLINE, BST_UNCHECKED);
							break;
						}
						case IDC_PLACEPROP:{
							CurrentBrushBehavior = BEHAVIOR_LAYPROP;
							CheckDlgButton(hwnd, IDC_SELMOVE, BST_UNCHECKED);

							if(Globals::pObjectBrush){
								delete Globals::pObjectBrush;
							}
							Globals::pObjectBrush = new PropStruct();
							Globals::pObjectBrush->uType = (int)SendMessage(GetDlgItem(hwnd, IDC_LIST), LB_GETCURSEL, 0, 0) + 1;
							if(Globals::pObjectBrush->uType > 9){
								Globals::pObjectBrush->ExtendedInfo = (ExtendedObject*)SendMessage(GetDlgItem(hwnd, IDC_LIST), LB_GETITEMDATA, (int)SendMessage(GetDlgItem(hwnd, IDC_LIST), LB_GETCURSEL, 0, 0), 0);
							}
							break;
						}
						case IDC_SELMOVE:{
							if(Globals::pObjectBrush){
								delete Globals::pObjectBrush;
								Globals::pObjectBrush = NULL;
							}
							CurrentBrushBehavior = BEHAVIOR_SELMOVEPROP;
							CheckDlgButton(hwnd, IDC_PLACEPROP, BST_UNCHECKED);
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
void ToolBoxWindow::NewPanel(int ID){
	if(hPanel){
		EndDialog(hPanel, 0);
	}
	hPanel = CreateDialog(hCurrentInstance, MAKEINTRESOURCE(ID), hwnd, DlgProc);
	CurrentPanelID = ID;
	switch(ID){
		case IDD_LEVELGRID:{
			CheckDlgButton(hPanel, IDC_DRAWLEVELTILE, BST_CHECKED);
			CurrentBrushBehavior = BEHAVIOR_DRAWLEVELTILE;
			HWND hListBox = GetDlgItem(hPanel, IDC_LIST);
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)"Bottomless Pit");
			for(int i = 0; i < Globals::nLevelTileTypes; i++){
				SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)Globals::TileSet[i].Name.c_str());
			}
			//for(std::vector<std::string>::iterator it = Globals::TextureList.begin(); it < Globals::TextureList.end(); it++){
			//	SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(*it).c_str());
			//}
			SendMessage(GetDlgItem(hPanel, IDC_LIST), LB_SETCURSEL, 0, 0);
			break;
		}
		case IDD_PROPS:{
			HWND hListBox = GetDlgItem(hPanel, IDC_LIST);
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)"Ice Cream Popsicle");
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)"Frosted Donut");
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)"Cake");
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)"Cherries");
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)"Brownie");
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)"Cookie");
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)"Jawbreaker");
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)"Double Cheeseburger");
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)"Energy Drink");

			//Fill with TileSetSpecifics.
			int EntryCount = 9;
			for(int i = 0; i < Globals::nDynamics; i++){
				ExtendedObject * pExtended = Globals::DynamicSet[i];
				pExtended->Type = 1;
				SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)pExtended->Name.c_str());
				SendMessage(hListBox, LB_SETITEMDATA, EntryCount++, (LPARAM)pExtended);
				
			}

			for(int i = 0; i < Globals::nHazards; i++){
				ExtendedObject * pExtended = &Globals::HazardousSet[i];
				pExtended->Type = 2;
				SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)pExtended->Name.c_str());
				SendMessage(hListBox, LB_SETITEMDATA, EntryCount++, (LPARAM)pExtended);
				
			}
			for(int i = 0; i < Globals::nDecos; i++){

				ExtendedObject * pExtended = &Globals::DecoSet[i];
				pExtended->Type = 3; //static rop
				SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)pExtended->Name.c_str());
				SendMessage(hListBox, LB_SETITEMDATA, EntryCount++, (LPARAM)pExtended);
				
			}
			SendMessage(GetDlgItem(hPanel, IDC_LIST), LB_SETCURSEL, 0, 0);
			CheckDlgButton(hPanel, IDC_PLACEPROP, BST_CHECKED);
			CurrentBrushBehavior = BEHAVIOR_LAYPROP;
			if(Globals::pObjectBrush){
				delete Globals::pObjectBrush;
			}
			Globals::pObjectBrush = new PropStruct();
			Globals::pObjectBrush->uType = (int)SendMessage(hListBox, LB_GETCURSEL, 0, 0) + 1;
			if(Globals::pObjectBrush->uType > 9){
				Globals::pObjectBrush->ExtendedInfo = (ExtendedObject*)SendMessage(GetDlgItem(hwnd, IDC_LIST), LB_GETITEMDATA, Globals::pObjectBrush->uType-1, 0);
			}
			break;
		}
		default:
			break;
	}
	return;
}
void ToolBoxWindow::Kill(){
	//Kill this window and all its children
	if(hPanel){
		EndDialog(hPanel, 0);
	}
	EndDialog(hwnd, 0);
	return;
}
int ToolBoxWindow::GetCurrentBrushBehavior(){
	return CurrentBrushBehavior;
}
int ToolBoxWindow::GetBrushImageID(){
	return (int)SendMessage(GetDlgItem(hPanel, IDC_LIST), LB_GETCURSEL, 0, 0);
}
ExtendedObject * ToolBoxWindow::GetObjectInfo(int ListID){
	return (ExtendedObject*)SendMessage(GetDlgItem(hPanel, IDC_LIST), LB_GETITEMDATA, ListID, 0);
}
ToolBoxWindow::ToolBoxWindow(HINSTANCE hInstance, HWND hParent){
	pActiveToolbox = this;
	//Might as well create it in the constructor
	hCurrentInstance = hInstance;
	hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_TOOLSPANEL), hParent, DlgProc);
	//Set up Tabs
	HWND hTabs = GetDlgItem(hwnd, IDC_TOOLTABS);
	TCITEM tabitem;
	tabitem.mask = TCIF_TEXT | TCIF_IMAGE; 
    tabitem.iImage = -1; 
	tabitem.pszText = "Level Grid";
	TabCtrl_InsertItem(hTabs, 0, &tabitem);
	
	tabitem.pszText = "Actors & Props";
	TabCtrl_InsertItem(hTabs, 1, &tabitem);

	tabitem.pszText = "Static Deco";
	TabCtrl_InsertItem(hTabs, 2, &tabitem);

	//SetDefault LEVEL GRID
	NewPanel(IDD_LEVELGRID);

}


ToolBoxWindow::~ToolBoxWindow(void){
}
