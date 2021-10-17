// ----------------------------------------------------------------------------
// Filename:	WinMain.cpp
// Class:		none. Creates MainFrame object
// Purpose:		Message loop for Vat application
// -----------------------------------------------------------------------------


#include "Resource.h"
#include <windows.h>

#include "../../common/Catalogue.h"
#include "../../common/GameInterface.h"
#include "../../common/WhichEngine.h"
#include "../../engine/Creature/Brain/BrainScriptFunctions.h"

#include "MainFrame.h"

extern WhichEngine theWhichEngine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
 	MainFrame *mainframeWnd = new MainFrame(hInstance, NULL);
	HWND hwnd;
	MSG msg;

	try
	{
		std::string path = theWhichEngine.CatalogueDir().begin();
		theCatalogue.AddDir(path, "en");
	}
	catch(Catalogue::Err e)
	{
		MessageBox(NULL, e.what(), "Vat Kit", MB_OK | MB_ICONERROR | MB_APPLMODAL);
	}

	InitBrainMappingsFromCatalogues();

	if(!(hwnd = mainframeWnd->Create()))
		return -1;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);			// posts WM_PAINT

	while(GetMessage(&msg, NULL, 0, 0))
	{
		if(!IsDialogMessage(NULL, &msg))
		{
			TranslateMessage(&msg);	// translate keyboard input (key up/down) to wm_char
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

