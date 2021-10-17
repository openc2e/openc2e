// BrainViewport.cpp
#pragma warning (disable : 4786)

#include "Resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <wingdi.h>
#include <strstream>

#include "MainFrame.h"
#include "BrainViewport.h"
#include "Vat.h"		//general project definitions
#include "LobeDefinition.h"
#include "../../common/GameInterface.h"
#include "../../common/WhichEngine.h"
#include "ThreshHoldDlg.h"
#include "GeneAgeDlg.h"

extern CGameInterface theGameInterface;
extern WhichEngine theWhichEngine;




BrainViewport* theBrainViewport;



bool BrainViewport::ourIsRegistered = false;

// -------------------------------------------------------------------------
// Method:		BrainViewport
// Arguments:	hInstance - application instance                       
//				hWndOwner - handle to owner window
//				genome file to load - if none attempts to connect to game
//				using id.
//				id - online id of creature
//
// Returns:		none
// Description:	Constructor - uses hInstance and hWndOwner to register the 
//				and create the Window
// -------------------------------------------------------------------------
BrainViewport::BrainViewport(const HINSTANCE hInstance, const HWND hWndOwner, char filename[], int id) 
	: Viewport(hInstance, hWndOwner),
	myBrain(NULL), myGenome(NULL), mySVCaptions(NULL),	
	myLobePath(NULL), myNeuronPath(NULL), myTractPath(NULL), myDendritePath(NULL),
	myMigrationLog(NULL), 
	myLobePens(NULL), myLobeBrushes(NULL),
	myMouseMoveLobe(false), myTrackMouseLeave (false), myLeftButtonDown(false),	
	myNeuronViewVar(0), myDendriteViewVar(0),
	myBrainDlgContainer(NULL), myLobeNames(NULL), myTips(NULL),
	myOnlineId(id),
	myHOnlineMainMenu(NULL), myHOfflineMainMenu(NULL), myHOfflineReconnectMainMenu(NULL),
	myFilename(filename ? filename : "")
{
	BrainAccess::ResetDumpVersion();

	LoadString(hInstance, IDR_BRAINVIEWPORT, ourClassName, MAXRESOURCESTRING);

	if(!ourIsRegistered)
	{
		// Register Class
		myWCEX.lpszClassName = ourClassName;
		myWCEX.hbrBackground = NULL;	// no erase
		myWCEX.hCursor = NULL;	
		RegisterWndClass();
		ourIsRegistered =true;
	}
	myWCS.lpClassName = ourClassName;


	myViewThreshHoldStyle = NO_THRESHHOLD;
	myViewThreshHold = 0;

	myErase = RDW_NOERASE;


	myMouseHover.over = isNothing;
	myMouseHover.isLobe = NULL;
	myMouseHover.isTract = NULL;
	myMouseHover.isNeuron = NULL;
	myMouseHover.isDendrite = NULL; 

	myPopupHover.over = isNothing,
	myPopupHover.isLobe = NULL;
	myPopupHover.isTract = NULL;
	myPopupHover.isNeuron = NULL;
	myPopupHover.isDendrite = NULL; 

	myNeuronViewVar = 0;
	myDendriteViewVar = 0;


	// create highlighting colours
	for(int c=0; c != totalColours; c++)
	{ 
		int colour = 192+(c*(64/totalColours));;
		myHighlightPens[c] = CreatePen(PS_SOLID, GetSystemMetrics(SM_CXBORDER), RGB(colour, colour, colour) );
		myHighlightBrushes[c] = CreateSolidBrush(RGB(colour, colour, colour));
	}

	// Set all chemical values to zero
	for(int i=0; i<256; i++) {
		myChemicals[i] = 0;
	}

	// create font;
	LOGFONT logfont;
	memset(&logfont, 0, sizeof(logfont));
	logfont.lfHeight = 16;
	logfont.lfWeight = 14;
	strcpy(logfont.lfFaceName, "MS Sans Serif");
	myFont = CreateFontIndirect(&logfont);




}






// -------------------------------------------------------------------------
// Method:		~BrainViewport
// Arguments:	none
// Returns:		none
// Description:	Destructor
// -------------------------------------------------------------------------
BrainViewport::~BrainViewport()
{
	// note: all child windows automatically deleted

	// clean up brain specific stuff
	DeleteBrainSupport();
	if(myBrain) delete myBrain; 
	if(myGenome) delete myGenome;


	// clean up standard pens, brushes and fonts
	for(int c=0; c != totalColours; c++)
	{
		 DeletePen(myHighlightPens[c]);
		 DeleteObject(myHighlightBrushes[c]);
	}

	DeleteObject(myFont);
	if(myHOnlineMainMenu) DestroyMenu(myHOnlineMainMenu);
	if(myHOfflineMainMenu) DestroyMenu(myHOfflineMainMenu);
	if(myHOfflineReconnectMainMenu) DestroyMenu(myHOfflineReconnectMainMenu);
	if(myHPopupMenus) DestroyMenu(myHPopupMenus);

}


// -------------------------------------------------------------------------
// Method:		LoadFromGenome
// Arguments:	
// Returns:		success or failure
// Description:	Uses string as genome myFilename to load a genome and create 
//				a brain from it
// -------------------------------------------------------------------------
bool BrainViewport::LoadFromGenome()
{
	// clear 
	DeleteBrainSupport();
	if(myBrain) delete myBrain; 
	if(myGenome) delete myGenome;
	myGenome = NULL;

	myFinalGeneAge = GetGeneAge(0);

	// load genome

	try
	{
		myGenome = new Genome();
		myGenome->ReadFromFile(myFilename.begin(), 1, 0, 1, "");		
	}
	catch(Genome::GenomeException)
	{
		if(myGenome != NULL)
			delete myGenome;
		throw(InitFailedException(GetHwnd(myOwnerWnd), "Could not load genome"));
	}

	// create brain
	if(!(myBrain = new Brain()))
		throw(InitFailedException(GetHwnd(myOwnerWnd), "Could not create brain"));
	
	
	try {
		myBrain->RegisterBiochemistry(myChemicals);
		for(int geneAge = 0; geneAge <= myFinalGeneAge; geneAge++)
		{
			myGenome->SetAge(geneAge);
			myBrain->ReadFromGenome(*myGenome);
		}
	}
	catch(GenomeInitFailedException)
	{
		throw(InitFailedException(GetHwnd(myOwnerWnd), "Could notnitialze brain"));
	}

	if(GetNoLobes(*myBrain) == 0)
		throw(InitFailedException(GetHwnd(myOwnerWnd), "There are no lobes"));

	if(!CreateBrainSupport(myFilename.begin()))
		throw(InitFailedException(GetHwnd(myOwnerWnd), "Could not create brain support"));

	return true;
}



// -------------------------------------------------------------------------
// Method:		LoadFromCreatures
// Arguments:	none
// Returns:		success or failure
// Description:	Initialises the viewport, loading creature using myOnlineId 
// -------------------------------------------------------------------------
bool BrainViewport::LoadFromCreatures()
{
	char *reply = NULL;

	// clear 
	DeleteBrainSupport();
	if(myBrain) delete myBrain; 
	if(myGenome) delete myGenome;

	std::string filename = theWhichEngine.GetStringKey("Worlds Directory");
	char macro[1000];
	sprintf(macro, "execute\nOUTS WNAM", myOnlineId);
	if(!theGameInterface.Inject(macro, &reply))
		throw(InitFailedException(GetHwnd(myOwnerWnd), "Could not get Worlds Directory"));
	filename += "\\";
	filename += reply;
	filename += "\\genetics\\";


	sprintf(macro, "execute\nTARG AGNT %d OUTS GTOS 0", myOnlineId);
	if(!theGameInterface.Inject(macro, &reply))
		throw(InitFailedException(GetHwnd(myOwnerWnd), "Could not find norn"));
	filename += reply;
	filename += ".gen";

	sprintf(macro, "execute\nTARG AGNT %d OUTV CAGE", myOnlineId);
	if(!theGameInterface.Inject(macro, &reply))
		throw(InitFailedException(GetHwnd(myOwnerWnd), "Could not find norn"));
	myFinalGeneAge = ReadIntDesc(&reply);

	// load genome
	myGenome = new Genome();
	myGenome->ReadFromFile(filename.begin(), 1, 0, 0, "");

	sprintf(macro, "execute\nTARG AGNT %d BRN: DMPB", myOnlineId);
	if(!theGameInterface.Inject(macro, &reply))
		throw(InitFailedException(GetHwnd(myOwnerWnd), "Could not fetch brain"));


	int noLobes = ReadIntDesc(&reply);
	int noTracts = ReadIntDesc(&reply);

	if(noLobes == 0)
		throw(InitFailedException(GetHwnd(myOwnerWnd), "There are no lobes"));

	long dumpSize;
	for(int l = 0; l != noLobes; l++)
	{
		dumpSize = ReadIntDesc(&reply);
		if(dumpSize > theGameInterface.GetBufferSize())
		{
			char errorMsg[100] = "Lobe to big for transfer\nRequire transfer buffer of:";
			char sizeString[20];
			sprintf(sizeString, "%d", dumpSize);
			strcat(errorMsg, sizeString);
			throw(InitFailedException(GetHwnd(myOwnerWnd), errorMsg));
		}
	}

	for(int t = 0; t != noTracts; t++)
	{
		dumpSize = ReadIntDesc(&reply);
		if(dumpSize > theGameInterface.GetBufferSize())
		{
			char errorMsg[100] = "Tract to big for transfer\nRequire transfer buffer of:";
			char sizeString[20];
			sprintf(sizeString, "%d", dumpSize);
			strcat(errorMsg, sizeString);
			throw(InitFailedException(GetHwnd(myOwnerWnd), errorMsg));
		}
	}

	std::string version = reply;

	if(version == "END DUMP")
	{
		// engine version 1.0
		BrainAccess::BrainDumpVersion(1.0f);
	}
	else if(version == "END DUMP V1.1")
	{
		// engine version 1.0
		BrainAccess::BrainDumpVersion(1.1f);
	}


	// create brain
	if(!(myBrain = new Brain()))
		throw(InitFailedException(GetHwnd(myOwnerWnd), "Could not create brain"));

	myBrain->RegisterBiochemistry(myChemicals);
	
	try {
		for(int l = 0; l != noLobes; l++)
		{
			char macro[50];
			sprintf(macro, "execute\nTARG AGNT %d BRN: DMPL %d", myOnlineId, l);
			if(theGameInterface.Inject(macro, &reply))
			{
#ifdef _WIN32
				std::strstream in( reply, theGameInterface.GetBufferSize()-1, 
					std::ios_base::in | std::ios_base::binary );
#else
				std::strstream in( reply, theGameInterface.GetBufferSize()-1 );
#endif

				myBrain->InitLobeFromDescription(in);
			}
			else
			{
				throw(InitFailedException(GetHwnd(myOwnerWnd), "Could not download brain lobe"));
			}
		}

		for(int t = 0; t != noTracts; t++)
		{
			char macro[50];
			sprintf(macro, "execute\nTARG AGNT %d BRN: DMPT %d", myOnlineId, t);
			if(theGameInterface.Inject(macro, &reply))
			{
#ifdef _WIN32
				std::strstream in( reply, theGameInterface.GetBufferSize()-1, 
					std::ios_base::in | std::ios_base::binary );
#else
				std::strstream in( reply, theGameInterface.GetBufferSize()-1 );
#endif

				myBrain->InitTractFromDescription(in);
			}
			else
				throw(InitFailedException(GetHwnd(myOwnerWnd), "Could not download brain tract"));
		}
	}
	catch(GenomeInitFailedException)
	{
		throw(InitFailedException(GetHwnd(myOwnerWnd), "Could not initialize brain"));
	}


	if(!CreateBrainSupport(filename))
		throw(InitFailedException(GetHwnd(myOwnerWnd), "Could not create brain support"));


	return true;
}

// -------------------------------------------------------------------------
// Method:		Create
// Arguments:	window dimensions
// Returns:		handle to window created
// Description:	creates the window
// -------------------------------------------------------------------------
HWND BrainViewport::Create(int x, int y, int nWidth, int nHeight)
{
	myWCS.x = x;
	myWCS.y = y;
	myWCS.nWidth = nWidth;
	myWCS.nHeight = nHeight;

	if(!Viewport::Create())
		return NULL;

	// create tooltips
	try
	{
		if((myTips = new Tips(GetWindowInstance(myHWnd), myHWnd, 100, 1000)))
		{
			if(!myTips->Create())
			{
				delete myTips;
				myTips = NULL;
			}
		}
	}
	catch(Wnd::FailConstructorException){};


	// load online and offline menus and add view and windows popups
	myHOnlineMainMenu = LoadMenu(GetWindowInstance(myHWnd), MAKEINTRESOURCE(IDR_ONLINEACTIVEMENU));
	myHOfflineMainMenu = LoadMenu(GetWindowInstance(myHWnd), MAKEINTRESOURCE(IDR_OFFLINEACTIVEMENU));
	myHOfflineReconnectMainMenu = LoadMenu(GetWindowInstance(myHWnd), MAKEINTRESOURCE(IDR_OFFLINERECONNECTMENU));
	myHPopupMenus = LoadMenu(GetWindowInstance(myHWnd), MAKEINTRESOURCE(IDR_POPUPS));
	myHViewMenu = GetSubMenu(myHPopupMenus, 4);
	AppendMenu(myHOnlineMainMenu, MF_ENABLED | MF_POPUP, (UINT_PTR)myHViewMenu, (LPSTR)"&View");
	AppendMenu(myHOfflineMainMenu, MF_ENABLED | MF_POPUP, (UINT_PTR)myHViewMenu, (LPSTR)"&View");
	AppendMenu(myHOfflineReconnectMainMenu, MF_ENABLED | MF_POPUP, (UINT_PTR)myHViewMenu, (LPSTR)"&View");
	
	if(myFilename.length() != 0)
		LoadFromGenome();
	else
		LoadFromCreatures();	// uses myOnlineId
	
	myBrainDlgContainer->SetMenu(myHOnlineMainMenu, true);
	myBrainDlgContainer->SetMenu(myHOfflineReconnectMainMenu, true);
	myBrainDlgContainer->SetMenu(myHOfflineMainMenu, false);



	InitViewport(true);
	RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);


	return myHWnd;
}

// -------------------------------------------------------------------------
// Method:		CreateBrainSupport
// Arguments:	character string
// Returns:		none
// Description:	Create captions using genome filename, create inputlobedlgsfor 
//				the brain also creates brushes and pens for the brain
// -------------------------------------------------------------------------
bool BrainViewport::CreateBrainSupport(std::string filename)
{
	int l, t;

	// load sv captions
	if(!(mySVCaptions = new SVCaptions(filename, *myGenome, GetLobes(*myBrain), GetTracts(*myBrain), myFinalGeneAge)))
		return false;

	// create a path recorder for the brain

	// original version causes gcc internal error !?
//	if(!(myLobePath = new Path[GetNoLobes(*myBrain)]))
//		return false;
	myLobePath = new Path[ GetNoLobes(*myBrain) ];

	myNeuronPath = new Path*[GetNoLobes(*myBrain)];

	for(l=0; l != GetNoLobes(*myBrain); l++)
	{
		myLobePath[l].hidden = false;	// init lobe viewability	
		myNeuronPath[l] = new Path[GetLobeSize(GetLobe(*myBrain, l))];
	}



	myTractPath = new Path[GetNoTracts(*myBrain)];

	myDendritePath = new Path*[GetNoTracts(*myBrain)];

	for(t=0; t != GetNoTracts(*myBrain); t++)
	{
		myTractPath[t].hidden = false;	// init tract viewability
		myDendritePath[t] = new Path[GetTractSize(GetTract(*myBrain, t))];
	}


	// migration log
	if(!(myMigrationLog = new DendriteConnectionLog*[GetNoTracts(*myBrain)]))
		return false;

	for(t=0; t != GetNoTracts(*myBrain); t++)
	{
		if(!(myMigrationLog[t] = new DendriteConnectionLog[GetTractSize(GetTract(*myBrain, t))]))
			return false;

		for(int d = 0; d != GetTractSize(GetTract(*myBrain, t)); d++)
		{
			myMigrationLog[t][d].src = GetSrcNeuron(GetTract(*myBrain, t), d);
			myMigrationLog[t][d].dst = GetDstNeuron(GetTract(*myBrain, t), d);
		}
	}

	

	// create lobe pens and brushes in lobe colours
	if(!(myLobePens = new HPEN*[GetNoLobes(*myBrain)]))
		return false;

	if(!(myLobeBrushes = new HBRUSH*[GetNoLobes(*myBrain)]))
		return false;

	for(l=0; l != GetNoLobes(*myBrain); l++)
	{
		if(!(myLobePens[l] = new HPEN[totalColours]))
			return false;

		if(!(myLobeBrushes[l] = new HBRUSH[totalColours]))
			return false;

		Lobe &lobe = GetLobe(*myBrain, l);

		for(int c=0; c != totalColours; c++) {
			float intensity = 0.3f+(0.7f*(float)c/(float)totalColours);
			COLORREF rgbColour = RGB(
				(BYTE)(GetLobeRed(lobe)*intensity),
				(BYTE)(GetLobeGreen(lobe)*intensity),
				(BYTE)(GetLobeBlue(lobe)*intensity)
			);
			myLobePens[l][c] = CreatePen(
				PS_SOLID, 
				GetSystemMetrics(SM_CXBORDER),
				rgbColour
			);
			myLobeBrushes[l][c] = CreateSolidBrush(rgbColour);
		}
	}


	myLobeNames = new LobeNames(GetLobes(*myBrain));
	
	myBrainDlgContainer = new BrainDlgContainer(*myLobeNames);


	// create lobe input dialogs (offline version only)
	CreateLobeInputDlgs();


	return true;
}


// -------------------------------------------------------------------------
// Method:		DeleteBrainSupport
// Arguments:	none
// Returns:		none
// Description:	Deletes the brain and all attached windows, pens and brushes
// -------------------------------------------------------------------------
void BrainViewport::DeleteBrainSupport()
{
	// delete a path recorder for the brain

	if(myLobePath) 
	{
		delete []myLobePath;
		myLobePath = NULL;
	}

	if(myNeuronPath)
	{
		int l = 0;
		while(l != GetNoLobes(*myBrain) && myNeuronPath[l])
			delete []myNeuronPath[l++];
	
		delete []myNeuronPath;
		myNeuronPath = NULL;
	}

	
	if(myTractPath)	
	{
		delete []myTractPath;
		myTractPath = NULL;
	}

	if(myDendritePath)
	{
		int t = 0;
		while(t != GetNoTracts(*myBrain) && myDendritePath[t])
			delete []myDendritePath[t++];

		delete []myDendritePath;
		myDendritePath = NULL;
	}

	// delete migration log
	if(myMigrationLog)
	{
		int t = 0;
		while(t != GetNoTracts(*myBrain) && myMigrationLog[t])
			delete []myMigrationLog[t++];

		delete []myMigrationLog;
		myMigrationLog = NULL;
	}


	
	// delete pens and brushes

	if(myLobePens)
	{
		int l = 0;
		while(l != GetLobes(*myBrain).size() && myLobePens[l])
		{
			for(int c=0; c != totalColours; c++)
				DeletePen(myLobePens[l][c]);

			delete []myLobePens[l++];
		}
		delete []myLobePens;
		myLobePens = NULL;
	}

	if(myLobeBrushes)
	{
		int l = 0;
		while(l != GetLobes(*myBrain).size() && myLobeBrushes[l])
		{
			for(int c=0; c != totalColours; c++)
				DeleteObject(myLobeBrushes[l][c]);

			delete []myLobeBrushes[l++];
		}
		delete []myLobeBrushes;
		myLobeBrushes = NULL;
	}

	if(mySVCaptions) delete mySVCaptions;

	if(myLobeNames) delete myLobeNames;
	
	if(myBrainDlgContainer)	delete myBrainDlgContainer;

}




// -------------------------------------------------------------------------
// Method:		SetCanvasSpec
// Arguments:	none
// Returns:		none
// Description:	calculates the size of the brain to be used as printing 
//				dimensions
// -------------------------------------------------------------------------
void BrainViewport::SetCanvasSpec()
{
	// calc canvas extent based on size+position of lobes
	int i, xo, yo, xe, ye;
	const int border = 2;
	
	if(!myBrain)
	{
		// no brain so set empty canvas
		myCanvas.XOrg = 0;
		myCanvas.YOrg = 0;
		myCanvas.XExt = 1;
		myCanvas.YExt = 1;
		return;
	}	


	// find furthest extends of brain dimensions

	myCanvas.XOrg = GetLobeX(GetLobe(*myBrain, 0));
	myCanvas.YOrg = GetLobeY(GetLobe(*myBrain, 0));
	myCanvas.XExt = GetLobeX(GetLobe(*myBrain, 0)) + GetLobeWidth(GetLobe(*myBrain, 0));
	myCanvas.YExt = GetLobeY(GetLobe(*myBrain, 0)) + GetLobeHeight(GetLobe(*myBrain, 0));

	for (i=1; i != GetNoLobes(*myBrain); i++) 
	{
		Lobe &lobe = GetLobe(*myBrain, i);

		xo = GetLobeX(lobe);
		yo = GetLobeY(lobe);
		xe = GetLobeX(lobe) + GetLobeWidth(lobe);
		ye = GetLobeY(lobe) + GetLobeHeight(lobe);
		
		if(xe > myCanvas.XExt)
			myCanvas.XExt = xe;

		if(ye > myCanvas.YExt)
			myCanvas.YExt = ye;

		if(xo < myCanvas.XOrg)
			myCanvas.XOrg = xo;

		if(yo < myCanvas.YOrg)
			myCanvas.YOrg = yo;
	}


	// set 0,0 as base coordinate 
	// bitmap dc dont like negatives
	for (i=0; i != GetNoLobes(*myBrain); i++) 
	{
		Lobe &lobe = GetLobe(*myBrain, i);

		SetLobeX(lobe, GetLobeX(lobe)-myCanvas.XOrg);
		SetLobeY(lobe, GetLobeY(lobe)-myCanvas.YOrg);
	}

	myCanvas.XExt -= myCanvas.XOrg;
	myCanvas.YExt -= myCanvas.YOrg;
	myCanvas.XOrg = 0;
	myCanvas.YOrg = 0;
	myBorder = 30;


	return;
}


// -------------------------------------------------------------------------
// Method:		CreateLobeInputDlgs
// Arguments:	none
// Returns:		none
// Description:	Creates the lobe input dialog windows for the brain.
// -------------------------------------------------------------------------
void BrainViewport::CreateLobeInputDlgs()
{
	for(int l = 0; l != GetNoLobes(*myBrain); l++)
	{
		Lobe &lobe = GetLobe(*myBrain, l);
		const char *lobeName = GetLobeName(lobe);
		if(	(!strcmp(lobeName, "verb")) ||
			(!strcmp(lobeName, "noun")) ||
			(!strcmp(lobeName, "visn")) ||
			(!strcmp(lobeName, "smel")) ||
			(!strcmp(lobeName, "driv")) ||
			(!strcmp(lobeName, "forf")) ||
			(!strcmp(lobeName, "situ")) ||
			(!strcmp(lobeName, "detl")) ||
			(!strcmp(lobeName, "resp")) ||
			(!strcmp(lobeName, "prox")))
			myBrainDlgContainer->CreateLobeInputDlg(lobe, myHWnd);	
	}
}


// -------------------------------------------------------------------------
// Method:		Update
// Arguments:	update hidden only t/f
// Returns:		none
// Description:	Updates the brain, dumping lobe input and updating var dlgs
// -------------------------------------------------------------------------
void BrainViewport::Update(bool updateHidden)
{
	bool *fetchedTract;
	bool *fetchedLobe;
	fetchedTract = new bool[GetNoTracts(*myBrain)];
	fetchedLobe = new bool[GetNoLobes(*myBrain)];

	if(theGameInterface.Connected())
	{
		// online update

		try
		{
//			std::string macroRequest;
//			macroRequest = "execute\n";

			int n;
			// down load lobes from game
			n = GetNoLobes(*myBrain);
			for(int l = 0; l != n; l++)
			{
				// download half lobes each tick, gtb!
				if (l==0 || l==(int)(n/2))
					theGameInterface.Inject("execute\nDBG: TOCK", NULL);

				fetchedLobe[l] = false;
				if((updateHidden && myLobePath[l].hidden) || 
					(!updateHidden && !myLobePath[l].hidden) ||
					myBrainDlgContainer->HasUpdatableDlgs(GetLobe(*myBrain, l)))
				{
					OnlineUpdateLobe(l);
					fetchedLobe[l] = true;
				}
			}

			// down load tracts from game
			n = GetNoTracts(*myBrain);
			for(int t = 0; t != n; t++)
			{
				// download half tracts each tick, gtb!
				if (t==0 || t==(int)(n/2))
					theGameInterface.Inject("execute\nDBG: TOCK", NULL);

				fetchedTract[t] = false;
				if((updateHidden && myTractPath[t].hidden) ||
					(!updateHidden && !myTractPath[t].hidden) ||
					myBrainDlgContainer->HasUpdatableDlgs(GetTract(*myBrain, t)))
				{
					OnlineUpdateTract(t);
					fetchedTract[t] = true;
				}
			}

			// down load individual neurons that have not had their lobes downloaded
			std::vector<Lobe *> lobes;
			std::vector<Neuron *> neurons;
			if(myBrainDlgContainer->HasUpdatableDlgs(lobes, neurons))
			{
				for(int i = 0; i != neurons.size(); i++)
				{
					if(!fetchedLobe[LobeIdInList(*lobes[i])])
						OnlineUpdateNeuron(*lobes[i], *neurons[i]);
				}
			}
				
			// down load individual dendrites that have not had their tracts downloaded
			std::vector<Tract *> tracts;
			std::vector<Dendrite *> dendrites;
			if(myBrainDlgContainer->HasUpdatableDlgs(tracts, dendrites))
			{
				for(int i = 0; i != dendrites.size(); i++)
				{
					if(!fetchedTract[TractIdInList(*tracts[i])])
					{
						OnlineUpdateDendrite(*tracts[i], *dendrites[i]);
						CheckMigrations(TractIdInList(*tracts[i]), DendriteIdInList(*dendrites[i]));
					}
				}
			}
		}
		catch(OnlineFailedException)
		{
			return;
		}
	}
	else
	{
		myBrainDlgContainer->DumpToLobes();
		myBrain->Update();
		// clear inputs?
	}	
	


	// check if any that were being pathed have migrated
	// if so unpath the invalid old path
	for(int t=0; t != GetNoTracts(*myBrain); t++)
	{
		if(!theGameInterface.Connected() || fetchedTract[t])
		{
			if(IsMigrate(GetTract(*myBrain,t)))
			{
				for(int d = 0; d != GetTractSize(GetTract(*myBrain, t)); d++)
					CheckMigrations(t, d);
			}
		}
	}

	delete []fetchedTract;
	delete []fetchedLobe;

	// reprint brain
	RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);

	// update variable dialogs with new brain values
	myBrainDlgContainer->Update();
	
}


// -------------------------------------------------------------------------
// Method:		WndProc
// Arguments:	standard windprog args
// Returns:		result of message handling
// Description:	object specific message handler
// -------------------------------------------------------------------------
LRESULT BrainViewport::WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam)
{
	switch(msg)
	{
	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwnd, wParam, lParam, On_Command);
		break;

	case WM_MOUSEMOVE:
		HANDLE_WM_MOUSEMOVE(hwnd, wParam, lParam, On_MouseMove);
		break;

	case WM_LBUTTONDBLCLK:
		HANDLE_WM_LBUTTONDBLCLK(hwnd, wParam, lParam, On_LButtonDblClk);
		break;
	
	case WM_LBUTTONDOWN:
		HANDLE_WM_LBUTTONDOWN(hwnd, wParam, lParam, On_LButtonDown);
		break;

	case WM_RBUTTONDOWN:
		HANDLE_WM_RBUTTONDOWN(hwnd, wParam, lParam, On_RButtonDown);
		break;


	case WM_LBUTTONUP:
		HANDLE_WM_LBUTTONUP(hwnd, wParam, lParam, On_LButtonUp);
		break;

	case WM_MOUSELEAVE:
		On_MouseLeave();
		break;
		
	case WM_PAINT:	
		On_Paint();		
		break;

	default:
		// call superclass handler
		return(Viewport::WndProc(hwnd, msg, wParam, lParam));	
 
	}
	return(NULL);
}


// -------------------------------------------------------------------------
// Method:		On_Command
// Arguments:	standard On_Command args
// Returns:		none
// Description:	object specific command handler
// -------------------------------------------------------------------------
void BrainViewport::On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode)
{

	switch(id)
	{	

	// handle neuron/dendrite/lobe/tract popup menus

	case ID_GRAPHVARS:
		if(myPopupHover.over == isNeuron)
			myBrainDlgContainer->CreateNeuronVarGraphDlg(*myPopupHover.isNeuron, *myPopupHover.isLobe, myHWnd);
		else
			myBrainDlgContainer->CreateDendriteVarGraphDlg(*myPopupHover.isDendrite, *myPopupHover.isTract, myHWnd);
		break;

	case ID_VARIABLES:
		if(myPopupHover.over == isNeuron)
			myBrainDlgContainer->CreateNeuronVarDlg(*myPopupHover.isNeuron, *myPopupHover.isLobe, myHWnd);
		else
			myBrainDlgContainer->CreateDendriteVarDlg(*myPopupHover.isDendrite, *myPopupHover.isTract, myHWnd);
		break;

	case ID_SVRULE:
		if(myPopupHover.over == isLobe || myPopupHover.over == isNeuron)
			myBrainDlgContainer->CreateLobeDlg(*myPopupHover.isLobe, myHWnd, mySVCaptions->GetCaption(myPopupHover.isLobe));
		else
			myBrainDlgContainer->CreateTractDlg(*myPopupHover.isTract, myHWnd, mySVCaptions->GetCaption(myPopupHover.isTract));
		break;

	case ID_GRAPHLOBE:

		if(myPopupHover.over == isLobe || myPopupHover.over == isNeuron)
			myBrainDlgContainer->CreateLobeGraphDlg(*myPopupHover.isLobe, myNeuronViewVar, myHWnd);
		break;

	case ID_HIDE:
		if(myPopupHover.over == isLobe || myPopupHover.over == isNeuron)
		{
			// hide lobe and connecting tracts
			myLobePath[LobeIdInList(*myPopupHover.isLobe)].hidden = true;
			for(int t = 0; t != GetNoTracts(*myBrain); t++)
			{
				if(&GetSrcLobe(GetTract(*myBrain, t)) == myPopupHover.isLobe || &GetDstLobe(GetTract(*myBrain, t)) == myPopupHover.isLobe)
					myTractPath[t].hidden = true;
			}

		}
		else
		{
			myTractPath[TractIdInList(*myPopupHover.isTract)].hidden = true;
		}
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;





	// handle variables menu

	case ID_VIEW_NEURONVAR0:
		CheckNeuronMenu(0, ID_VIEW_NEURONVAR0);
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;

	case ID_VIEW_NEURONVAR1:
		CheckNeuronMenu(1, ID_VIEW_NEURONVAR1);
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;

	case ID_VIEW_NEURONVAR2:
		CheckNeuronMenu(2, ID_VIEW_NEURONVAR2);
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;

	case ID_VIEW_NEURONVAR3:
		CheckNeuronMenu(3, ID_VIEW_NEURONVAR3);
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;

	case ID_VIEW_NEURONVAR4:
		CheckNeuronMenu(4, ID_VIEW_NEURONVAR4);
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;

	case ID_VIEW_NEURONVAR5:
		CheckNeuronMenu(5, ID_VIEW_NEURONVAR5);
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;

	case ID_VIEW_NEURONVAR6:
		CheckNeuronMenu(6, ID_VIEW_NEURONVAR6);
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;

	case ID_VIEW_NEURONVAR7:
		CheckNeuronMenu(7, ID_VIEW_NEURONVAR7);
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;


	case ID_VIEW_DENDRITEVAR0:
		CheckDendriteMenu(0, ID_VIEW_DENDRITEVAR0);
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;

	case ID_VIEW_DENDRITEVAR1:
		CheckDendriteMenu(1, ID_VIEW_DENDRITEVAR1);	
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;

	case ID_VIEW_DENDRITEVAR2:
		CheckDendriteMenu(2, ID_VIEW_DENDRITEVAR2);
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;

	case ID_VIEW_DENDRITEVAR3:
		CheckDendriteMenu(3, ID_VIEW_DENDRITEVAR3);
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;

	case ID_VIEW_DENDRITEVAR4:
		CheckDendriteMenu(4, ID_VIEW_DENDRITEVAR4);
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;

	case ID_VIEW_DENDRITEVAR5:
		CheckDendriteMenu(5, ID_VIEW_DENDRITEVAR5);	
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;

	case ID_VIEW_DENDRITEVAR6:
		CheckDendriteMenu(6, ID_VIEW_DENDRITEVAR6);
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;

	case ID_VIEW_DENDRITEVAR7:
		CheckDendriteMenu(7, ID_VIEW_DENDRITEVAR7);
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;

	case ID_VIEW_HIDEDETAILS:
		CheckMenuItem(myHViewMenu, ID_VIEW_HIDEDETAILS, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR0, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR1, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR2, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR3, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR4, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR5, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR6, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR7, MF_UNCHECKED);

		CheckMenuItem(myHViewMenu, ID_VIEW_HIDEDETAILS, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR0, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR1, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR2, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR3, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR4, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR5, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR6, MF_UNCHECKED);
		CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR7, MF_UNCHECKED);

		CheckMenuItem(myHViewMenu, ID_VIEW_HIDEDETAILS, MF_CHECKED);
		myDendriteViewVar = hideDetails;
		myNeuronViewVar = hideDetails;
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		break;


	default:

		// default popup (show hiden stuff)
		// show a lobe and all connecting tracts
		if(id-ID_POPUP >= 0 && id-ID_POPUP < GetNoLobes(*myBrain))
		{
			myLobePath[id-ID_POPUP].hidden = false;
			bool ok = true;
	
			if(theGameInterface.Connected())
				ok = OnlineUpdateLobe(id-ID_POPUP);
	
			if(ok)
			{
				for(int t = 0; t != GetNoTracts(*myBrain); t++)
				{
					if(myTractPath[t].hidden && &GetSrcLobe(GetTract(*myBrain, t)) == &GetLobe(*myBrain, id-ID_POPUP) || &GetDstLobe(GetTract(*myBrain, t)) == &GetLobe(*myBrain, id-ID_POPUP))
					{
						myTractPath[t].hidden = false;
						if(theGameInterface.Connected())
							OnlineUpdateTract(t);
					}
				}
				RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
			}
		}

		// show a tract
		else if(id-(ID_POPUP+GetNoLobes(*myBrain)) >= 0 && id-(ID_POPUP+GetNoLobes(*myBrain)) < GetNoTracts(*myBrain))
		{
			myTractPath[id-(ID_POPUP+GetNoLobes(*myBrain))].hidden = false;
			if(theGameInterface.Connected())
				OnlineUpdateTract(id-(ID_POPUP+GetNoLobes(*myBrain)));

			RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		}



		// try activating a dlg selected from windows menu
		else if(myBrainDlgContainer->Activate(id))
			return;


		// do default behaviour
		else
			Viewport::On_Command(hwnd, id, hwndCtl, NotifyCode);
	}
}


// -------------------------------------------------------------------------
// Method:		On_Paint
// Arguments:	none
// Returns:		none
// Description:	paint the brain to the window client area
// -------------------------------------------------------------------------
void BrainViewport::On_Paint()
{
	HDC		hClientDC, hMemoryDC;
	HBITMAP hScreenBuffer;
	HBITMAP oldBitmap;
	HBRUSH	oldBrush;
	HPEN	oldPen;
	HFONT	oldFont;

    PAINTSTRUCT ps ;

	// create screen buffer
    hClientDC = BeginPaint (myHWnd, &ps) ;
	hMemoryDC = CreateCompatibleDC(hClientDC);
	hScreenBuffer = CreateCompatibleBitmap(hClientDC, myViewportXExt+1, myViewportYExt+1);
	oldBitmap = (HBITMAP) SelectObject(hMemoryDC, hScreenBuffer);

	//clear buffer
	oldPen = (HPEN) SelectObject(hMemoryDC, (HPEN) GetStockObject(BLACK_PEN));
	oldBrush = (HBRUSH)SelectObject(hMemoryDC, (HBRUSH) GetStockObject(BLACK_BRUSH));
	Rectangle(hMemoryDC, 0,0, myViewportXExt+1, myViewportYExt+1);


	// Select our font into the DC
	oldFont = (HFONT)SelectObject(hMemoryDC, myFont );


	// calculate scales for positioning neurons and 
	// dendrites based on current scaling factors
	myScalex1_4 = (int)Zoom(myScaleX/4);
	myScalex1_2 = (int)Zoom(myScaleX/2);
	myScalex3_4 = myScalex1_4+myScalex1_2;
	myScaley1_4 = (int)Zoom(myScaleY/4);
	myScaley1_2 = (int)Zoom(myScaleY/2);
	myScaley3_4 = myScaley1_4+myScaley1_2;


	// print tracts under lobes 
	if(myDendriteViewVar == hideDetails)
	{
		for (int t=0; t != GetNoTracts(*myBrain); t++) 
		{
			if(!myTractPath[t].hidden)
			{
				Tract &tract = GetTract(*myBrain, t);
				if(myTractPath[t].mySrc || myTractPath[t].myDst)
					PrintTract(tract, myHighlightPens, hMemoryDC);
				else
					PrintTract(tract, myLobePens[LobeIdInList(GetDstLobe(tract))], hMemoryDC);
			}
		}
	}
	else
	{

		int l, t;

		// print dendrites under neurons
		for ( t=0; t != GetNoTracts(*myBrain); t++) 
		{
			if(!myTractPath[t].hidden)
				PrintTractDendrites(GetTract(*myBrain, t), false, myLobePens[LobeIdInList(GetDstLobe(GetTract(*myBrain, t)))], hMemoryDC);
		}
	

		// print neurons over dens 
		for (l=0; l != GetNoLobes(*myBrain); l++) 
		{
			if(!myLobePath[l].hidden)
				PrintLobeNeurons(GetLobe(*myBrain, l), false, myLobePens[l], myLobeBrushes[l], hMemoryDC);
		}

		// print PATHED dendrites 
		for (t=0; t != GetNoTracts(*myBrain); t++) 
		{
			if(!myTractPath[t].hidden)
				PrintTractDendrites(GetTract(*myBrain, t), true, myHighlightPens, hMemoryDC);
		}


		// print PATHED neurons 
		for (l=0; l != GetNoLobes(*myBrain); l++) 
		{
			if(!myLobePath[l].hidden)
				PrintLobeNeurons(GetLobe(*myBrain, l), true, myHighlightPens, myHighlightBrushes, hMemoryDC);
		}
	}


	// print lobe & labels on very top
	for (int l=0; l != GetNoLobes(*myBrain); l++) 
	{
		if(!myLobePath[l].hidden)
		{
			Lobe &lobe = GetLobe(*myBrain, l);

			// print lobe
			if(myLobePath[l].mySrc || myLobePath[l].myDst || &GetLobe(*myBrain, l) == myMouseHover.isLobe)
				// curently pathing or hovering over
				SelectPen(hMemoryDC, myHighlightPens[0]);
			else
				SelectPen(hMemoryDC, myLobePens[l][0]);

			EmptyRectangle(hMemoryDC, BitMapXCoord(GetLobeX(lobe)), BitMapYCoord(GetLobeY(lobe)), 
				BitMapXCoord(GetLobeX(lobe)+GetLobeWidth(lobe)), 
				BitMapYCoord(GetLobeY(lobe)+GetLobeHeight(lobe)));


			// print label
			if(myLobePath[l].mySrc || myLobePath[l].myDst || &GetLobe(*myBrain, l) == myMouseHover.isLobe)
				SetTextColor(hMemoryDC, RGB(192, 192, 192));
			else
				SetTextColor(hMemoryDC, RGB(GetLobeRed(lobe), GetLobeGreen(lobe), GetLobeBlue(lobe)));

			SIZE textSize;
			std::string lobeName = myLobeNames->GetLobeFullName(LobeIdInList(lobe));
			GetTextExtentPoint( hMemoryDC, lobeName.begin(), lobeName.length(), &textSize);
			SetBkMode(hMemoryDC, TRANSPARENT );
			RECT textRect = {(int)BitMapXCoord(GetLobeX(lobe)),
							(int)BitMapYCoord(GetLobeY(lobe)) - textSize.cy,
							(int)BitMapXCoord(GetLobeX(lobe)) + textSize.cx,
							(int)BitMapYCoord(GetLobeY(lobe))};
			DrawText(hMemoryDC, lobeName.begin(), lobeName.length(), &textRect, 0 );
		}
	}



	// paint on screen 
	BitBlt(hClientDC, 0, 0, myViewportXExt+1, myViewportYExt+1, hMemoryDC, 0, 0, SRCCOPY);
	EndPaint (myHWnd, &ps) ;




	// reset dc
	SelectObject(hMemoryDC, oldFont);
	SelectObject(hMemoryDC, oldPen);
	SelectObject(hMemoryDC, oldBrush);
	SelectObject(hMemoryDC, oldBitmap);
	DeleteDC(hMemoryDC);
	DeleteObject(hScreenBuffer);
}


// -------------------------------------------------------------------------
// Method:		PrintLobeNeurons
// Arguments:	lobe to print, print pathed only, array of pens and brushes 
//				to use, device context to print to
// Returns:		none
// Description:	Prints all neurons in a lobe useing a pen whose brightness
//				reflects the strength of the myNeuronViewVar being printed
// -------------------------------------------------------------------------
void BrainViewport::PrintLobeNeurons(Lobe &lobe, bool pathedOnly, HPEN *hPens, HBRUSH *hBrushes, HDC hdc)
{
	// print all neurons in a lobe (called by on_paint)
	int n, l;
	int lobeX, lobeY;

	l = LobeIdInList(lobe);
	
	// get base coords
	lobeX = GetLobeX(lobe);
	lobeY = GetLobeY(lobe);

	for(n=0; n != GetLobeSize(lobe); n++)
	{
		float val = GetNeuron(lobe, n).states[myNeuronViewVar];
		// select highlight brush if currently pathed
		if((myNeuronPath[l][n].mySrc || myNeuronPath[l][n].myDst) && pathedOnly)
			PrintNeuron(hdc, lobeX, lobeY, lobe, n, myHighlightPens, myHighlightBrushes);
		else if(!myNeuronPath[l][n].mySrc && !myNeuronPath[l][n].myDst && !pathedOnly &&
			(myViewThreshHoldStyle == NO_THRESHHOLD ||
			(myViewThreshHoldStyle == NON_ZERO && val != 0) ||
			(myViewThreshHoldStyle == ABOVE_THRESHHOLD && val > myViewThreshHold) ||
			(myViewThreshHoldStyle == BELOW_THRESHHOLD  && val < myViewThreshHold)))
			// only view non pathed neurons if over threshold
			PrintNeuron(hdc, lobeX, lobeY, lobe, n, hPens, hBrushes);
	}

}


// -------------------------------------------------------------------------
// Method:		PrintDendriteTracts
// Arguments:	tract to print, print pathed only array of pens to use, device 
//				context to print to
// Returns:		none
// Description:	Prints all dendrites in a tract using a pen whose brightness
//				reflects the strength of the myDendriteViewVar being printed
// -------------------------------------------------------------------------
void BrainViewport::PrintTractDendrites(Tract &tract, bool pathedOnly, HPEN *hPens, HDC hdc)
{
	//print all dendrites in a tract (called by on_paint)
	int d, t;
	t = TractIdInList(tract);

	for(d=0; d != GetTractSize(tract); d++)
	{
		float val = GetDendrite(tract, d).weights[myDendriteViewVar];
		// select highlight brush if currently pathed
		if((myDendritePath[t][d].mySrc || myDendritePath[t][d].myDst) && pathedOnly)
			PrintDendrite(tract, GetDendrite(tract, d), myHighlightPens, hdc);
		else if(!myDendritePath[t][d].mySrc && !myDendritePath[t][d].myDst && !pathedOnly &&
			(myViewThreshHoldStyle == NO_THRESHHOLD ||
			(myViewThreshHoldStyle == NON_ZERO && val != 0) ||
			(myViewThreshHoldStyle == ABOVE_THRESHHOLD  && val > myViewThreshHold) ||
			(myViewThreshHoldStyle == BELOW_THRESHHOLD && val < myViewThreshHold)))
			// only view non pathed dendrites if over threshold
			PrintDendrite(tract, GetDendrite(tract, d), hPens, hdc);
	}
}


// -------------------------------------------------------------------------
// Method:		EmptyRectangle
// Arguments:	device contect, coords of rectangle
// Returns:		none
// Description:	Prints an unfilled rectangle, used for printing lobes
// -------------------------------------------------------------------------
void BrainViewport::EmptyRectangle(HDC hdc, float x1, float y1, float x2, float y2)
{
	// print an unfilled rectangle
	ClipRectangle(x1, y1, x2, y2);
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x1, y2);
	LineTo(hdc, x2, y2);
	LineTo(hdc, x2, y1);
	LineTo(hdc, x1, y1);
}


// -------------------------------------------------------------------------
// Method:		On_LButtonDblCLk
// Arguments:	standard double click message cracker paras
// Returns:		none
// Description:	in on an object - brings up edit dialog
// -------------------------------------------------------------------------
void BrainViewport::On_LButtonDblClk(HWND hwnd, BOOL DoubleClick, int x, int y, UINT keyFlags)
{
	// opens a dialog if mouse on dendrite or neuron
	switch(myMouseHover.over)
	{
	case isLobe:
		myBrainDlgContainer->CreateLobeDlg(*myMouseHover.isLobe, myHWnd, mySVCaptions->GetCaption(myMouseHover.isLobe));
	 	break;

	case isTract:
		myBrainDlgContainer->CreateTractDlg(*myMouseHover.isTract, myHWnd, mySVCaptions->GetCaption(myMouseHover.isTract));
		break;

	case isNeuron:
		myBrainDlgContainer->CreateNeuronVarDlg(*myMouseHover.isNeuron, *myMouseHover.isLobe, myHWnd);
		break;

	case isDendrite:
		myBrainDlgContainer->CreateDendriteVarDlg(*myMouseHover.isDendrite, *myMouseHover.isTract, myHWnd);
		break;
	}
}


// -------------------------------------------------------------------------
// Method:		On_LButtonDown
// Arguments:	standard left button down message cracker paras
// Returns:		none
// Description:	in on an a lobe - allows moving
// -------------------------------------------------------------------------
void BrainViewport::On_LButtonDown(HWND hwnd, BOOL DoubleClick, int x, int y, UINT keyFlags)
{
	if((!myMouseMoveLobe) && myMouseHover.isLobe != NULL)
	{
		// set lobe movement start positions
		myMoveMouseStartX = x;
		myMoveMouseStartY = y;
		myMoveLobeStartX = GetLobeX(*myMouseHover.isLobe);
		myMoveLobeStartY = GetLobeY(*myMouseHover.isLobe);
		myMouseMoveLobe = true;
		SetCursor(LoadCursor(NULL, IDC_SIZEALL));
	}

	// set up for tracing lobe/tract
	myLeftButtonDown = true;
	MapPath(myMouseHover, NULL);

}


// -------------------------------------------------------------------------
// Method:		On_LButtonUp
// Arguments:	standard left button up message cracker paras
// Returns:		none
// Description:	in on an a lobe - stop allowing moving
// -------------------------------------------------------------------------
void BrainViewport::On_LButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{

	// untrace lobe/tract
	myLeftButtonDown = false;
	POINT mousePos = {x, y};
	WhatIsMouseOver(mousePos);

	
	if(myMouseMoveLobe)	// may not have been on a lobe when button was pressed
	{
		// lobe stop movable
		myMouseMoveLobe = false;
		InitViewport(false);
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
}


// -------------------------------------------------------------------------
// Method:		On_RButtonDown
// Arguments:	standard left button down message cracker paras
// Returns:		none
// Description:	brings up popups
// -------------------------------------------------------------------------
void BrainViewport::On_RButtonDown(HWND hwnd, BOOL DoubleClick, int x, int y, UINT keyFlags)
{
	static bool popupActive = false;

	if(popupActive)
		return;
	
	popupActive = true;



	HMENU stdPopup = CreatePopupMenu();
	HMENU hMenuPopup;

	switch(myMouseHover.over)
	{
	case isNeuron:
		hMenuPopup = GetSubMenu(myHPopupMenus, 0);
		break;

	case isDendrite:
		hMenuPopup = GetSubMenu(myHPopupMenus, 1);
		break;

	case isLobe:
		hMenuPopup = GetSubMenu(myHPopupMenus, 2);
		break;

	case isTract:
		hMenuPopup = GetSubMenu(myHPopupMenus, 3);
		break;
	
	default:
		// use show organ menu 
		bool emptyMenu = true;
	
		int l;
		for(l = 0; l != GetNoLobes(*myBrain); l++)
		{
			if(myLobePath[l].hidden)
			{
				emptyMenu = false;
				char menuText[30] = "Show lobe ";
				strcat(menuText, GetLobeName(GetLobe(*myBrain, l)));
				AppendMenu(stdPopup, MF_STRING, ID_POPUP+l, menuText);
			}
		}

		int t;
		for(t = 0; t != GetNoTracts(*myBrain); t++)
		{
			if(myTractPath[t].hidden)
			{
				emptyMenu = false;
				char menuText[30] = "Show tract ";
				strcat(menuText, GetTractName(GetTract(*myBrain, t)));
				AppendMenu(stdPopup, MF_STRING, ID_POPUP+t+l, menuText);
			}
		}

		if(emptyMenu)
			AppendMenu(stdPopup, MF_GRAYED|MF_STRING, 0, "No Hidden Objects");
		
		hMenuPopup = stdPopup;
		break;
	}

	// dont show sv rules for version 1.0 cos they are not down loaded
	if(BrainAccess::BrainDumpVersion() == 1.0f && theGameInterface.Connected())
		EnableMenuItem(hMenuPopup, ID_SVRULE, MF_GRAYED);
	else
		EnableMenuItem(hMenuPopup, ID_SVRULE, MF_ENABLED);

	//position menu
	UINT align;
	if(x > myViewportXExt/2)
		align = TPM_RIGHTALIGN;
	else
		align = TPM_LEFTALIGN;
	if(y > myViewportYExt/2)
		align |= TPM_BOTTOMALIGN;
	else
		align |= TPM_TOPALIGN;

	POINT p;
	p.x = x;
	p.y = y;
	ClientToScreen(myHWnd, &p);

	
	myPopupHover = myMouseHover;	// record object menu if for
	
	//show menu
	int menuID = TrackPopupMenu(hMenuPopup, align, p.x, p.y, 0, myHWnd, NULL); 

	DestroyMenu(stdPopup);
	popupActive = false;

}


// -------------------------------------------------------------------------
// Method:		On_MouseMove
// Arguments:	standard mouse move message cracker paras
// Returns:		none
// Description:	if moving lobe - recalcs position and reprints brain
//				highlights and paths object currently on by calling 
//				whatismouseon(). Sets up to track if mouse is leaving
//				Makes sure have correct pointer
// -------------------------------------------------------------------------
void BrainViewport::On_MouseMove(HWND hwnd, int x, int y, UINT keyflags)
{

	POINT mousePos = {x, y};
/*	if(!myTrackMouseLeave)
	{
		TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT),TME_LEAVE, myHWnd, 0};
		_TrackMouseEvent(&tme);
		myTrackMouseLeave = true;
		SetCursor(LoadCursor(NULL, IDC_ARROW));	// make sure you have a normal cursor on reentry
	}*/

	if(myMouseMoveLobe)
	{
		int moveX, moveY;
		moveX = mousePos.x - myMoveMouseStartX;
		moveY = mousePos.y - myMoveMouseStartY;
		SetLobeX(*myMouseHover.isLobe, myMoveLobeStartX + (int)UnScaleX(UnZoom(moveX)));
		SetLobeY(*myMouseHover.isLobe, myMoveLobeStartY + (int)UnScaleY(UnZoom(moveY)));
		RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
	}
	else
		WhatIsMouseOver(mousePos);

}


// -------------------------------------------------------------------------
// Method:		On_MouseLeave
// Arguments:	none
// Returns:		none
// Description:	if moving lobe - recalcs position and reprints brain
//				highlights and paths object currently on by calling 
//				whatismouseon(). Changes pointer
// -------------------------------------------------------------------------
void BrainViewport::On_MouseLeave()
{
	myTrackMouseLeave = false;

	if(myMouseMoveLobe) 
	{
		myMouseMoveLobe = false;
		InitViewport(false);
	}

	myLeftButtonDown = false;

	if(myMouseHover.over != isNothing) UnMapPath();
	RedrawWindow(myHWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW |RDW_NOERASE);
	SetCursor(LoadCursor(NULL, IDC_ARROW));
}


// -------------------------------------------------------------------------
// Method:		WhatIsMouseOver
// Arguments:	coords of mouse
// Returns:		none
// Description:	finds out what object the mouse is on.
//				then calls mappath to map the path from that object
// -------------------------------------------------------------------------
void BrainViewport::WhatIsMouseOver(POINT mousePos)
{
	MSG peekMsg;	// var to check if a proper repaint sould be done first
	
	if(PeekMessage(&peekMsg, myHWnd, WM_PAINT, WM_PAINT, PM_NOREMOVE))	// not working properly?
		return;

	// vars to record what mouse is on
	MouseHover mouseNowHover = {isNothing, NULL, NULL, NULL, NULL};
	MouseOverLobeOrNeuron(mousePos, mouseNowHover);
	if(mouseNowHover.over != isNeuron)
		MouseOverTractOrDendrite(mousePos, mouseNowHover);

	MapPath(mouseNowHover, &mousePos);
}


// -------------------------------------------------------------------------
// Method:		MouseOverLobeOrNeuron
// Arguments:	coords of mouse, structure to store results
// Returns:		true if over neuron or lobe
// Description:	finds out if mouse is over a lobe or a neuron
// -------------------------------------------------------------------------
bool BrainViewport::MouseOverLobeOrNeuron(POINT mousePos, MouseHover& mouseNowHover)
{

	// check lobes
	for (int l=0; l != GetNoLobes(*myBrain); l++) 
	{

		if(!myLobePath[l].hidden)
		{

			// convert to viewport coords
			float lobeX = Zoom(ScaleX(GetLobeX(GetLobe(*myBrain, l)))-myWindowXOrg)+myBorder;
			float lobeY = Zoom(ScaleY(GetLobeY(GetLobe(*myBrain, l)))-myWindowYOrg)+myBorder;
			float lobeWidth = Zoom(ScaleX(GetLobeWidth(GetLobe(*myBrain, l))));
			float lobeHeight = Zoom(ScaleY(GetLobeHeight(GetLobe(*myBrain, l))));	
			
			if(mousePos.x >= lobeX && mousePos.y >=lobeY && mousePos.x<=lobeX+lobeWidth && mousePos.y <= lobeY+lobeHeight)
			{
				// on lobe
				mouseNowHover.over= isLobe;
				mouseNowHover.isLobe = &GetLobe(*myBrain, l);	// only assign to var after checked on as 
											// this is used when MapPath of dendrite

				if(myNeuronViewVar != hideDetails)
				{
					//details are viewable check neurons
					for(int n=0; n != GetLobeSize(*mouseNowHover.isLobe); n++)
					{
						float val = GetNeuron(GetLobe(*myBrain, l), n).states[myNeuronViewVar];
						if(myViewThreshHoldStyle == NO_THRESHHOLD ||
							(myViewThreshHoldStyle == NON_ZERO && val != 0) ||
							(myViewThreshHoldStyle == ABOVE_THRESHHOLD && val > myViewThreshHold) ||
							(myViewThreshHoldStyle == BELOW_THRESHHOLD  && val < myViewThreshHold))
						{
							// calc viewport coords
							float neuronX = lobeX+((Zoom(ScaleX(n%GetLobeWidth(*mouseNowHover.isLobe)))));
							float neuronY = lobeY+((Zoom(ScaleY(n/GetLobeWidth(*mouseNowHover.isLobe)))));

							if(mousePos.x >= neuronX+myScalex1_4 && mousePos.y >= neuronY+myScaley1_4 && mousePos.x<= neuronX+myScalex3_4 && mousePos.y <= neuronY+myScaley3_4)		
							{
								// on neuron
								mouseNowHover.over = isNeuron;
								mouseNowHover.isNeuron = &GetNeuron(*mouseNowHover.isLobe, n);
								return true;	
							}
						}
					}
				}

				return true;		// on lobe only

			}
		}
	}

	return false;
}


// -------------------------------------------------------------------------
// Method:		MouseOverTractOrDendrite
// Arguments:	coords of mouse, structure to store results in
// Returns:		true if over tract or dendrite
// Description:	finds out if mouse is over tract or dendrite
// -------------------------------------------------------------------------
bool BrainViewport::MouseOverTractOrDendrite(POINT mousePos, MouseHover& mouseNowHover)
{
	
	// check tracts
	for (int t=0; t != GetNoTracts(*myBrain); t++) 
	{

		if(!myTractPath[t].hidden)
		{

			Tract &checkTract = GetTract(*myBrain, t);

			// get source and destination lobe positions dimentions int viewport coords
			float srcLobeX = Zoom(ScaleX((float)GetLobeX(GetSrcLobe(checkTract)))-myWindowXOrg)+myBorder;
			float srcLobeY = Zoom(ScaleY((float)GetLobeY(GetSrcLobe(checkTract)))-myWindowYOrg)+myBorder;
			float srcLobeWidth = Zoom(ScaleX((float)GetLobeWidth(GetSrcLobe(checkTract))));
			float srcLobeHeight = Zoom(ScaleY((float)GetLobeHeight(GetSrcLobe(checkTract))));

			float dstLobeX = Zoom(ScaleX((float)GetLobeX(GetDstLobe(checkTract)))-myWindowXOrg)+myBorder;
			float dstLobeY = Zoom(ScaleY((float)GetLobeY(GetDstLobe(checkTract)))-myWindowYOrg)+myBorder;
			float dstLobeWidth = Zoom(ScaleX((float)GetLobeWidth(GetDstLobe(checkTract))));
			float dstLobeHeight = Zoom(ScaleY((float)GetLobeHeight(GetDstLobe(checkTract))));

			if(myDendriteViewVar != hideDetails)
			{
				// try and locate a dendrite
				// calculate a bounding box
				int x1 = srcLobeX;
				if(x1 > dstLobeX)
					x1 = dstLobeX;
		
				int y1 = srcLobeY;
				if(y1 > dstLobeY)
					y1 = dstLobeY;

				int x2 = srcLobeX+srcLobeWidth;
				if(x2 < dstLobeX+dstLobeWidth)
					x2 = dstLobeX+dstLobeWidth;
		
				int y2 = srcLobeY+srcLobeHeight;
				if(y2 < dstLobeY+dstLobeHeight)
					y2 = dstLobeY+dstLobeHeight;


				if(mousePos.x >= x1 && mousePos.y >=y1 && mousePos.x<=x2 && mousePos.y <= y2)
				{
					// potentually in the checkTract
					
					//check dendrites
					for(int d=0; d != GetTractSize(checkTract); d++)
					{
						float val = GetDendrite(checkTract, d).weights[myDendriteViewVar];
						// select highlight brush if currently pathed
						if(myViewThreshHoldStyle == NO_THRESHHOLD ||
							(myViewThreshHoldStyle == NON_ZERO && val != 0) ||
							(myViewThreshHoldStyle == ABOVE_THRESHHOLD  && val > myViewThreshHold) ||
							(myViewThreshHoldStyle == BELOW_THRESHHOLD && val < myViewThreshHold))
						{
							int srcNeuronPos = NeuronIdInList(*GetSrcNeuron(checkTract, d));
							int dstNeuronPos = NeuronIdInList(*GetDstNeuron(checkTract, d));

							// calculate dendrite line from and to in viewport coords 
							float srcNeuronX = srcLobeX+(Zoom(ScaleX(srcNeuronPos %GetLobeWidth(GetSrcLobe(checkTract)))));
							float srcNeuronY = srcLobeY+(Zoom(ScaleY(srcNeuronPos /GetLobeWidth(GetSrcLobe(checkTract)))));
							float dstNeuronX = dstLobeX+(Zoom(ScaleX(dstNeuronPos %GetLobeWidth(GetDstLobe(checkTract)))));
							float dstNeuronY = dstLobeY+(Zoom(ScaleY(dstNeuronPos /GetLobeWidth(GetDstLobe(checkTract)))));
					
							if(Online(mousePos, srcNeuronX+myScalex1_2, srcNeuronY+myScaley1_2, dstNeuronX+myScalex1_2, dstNeuronY+myScaley1_2))		
							{
								// on dendrite
								mouseNowHover.over = isDendrite;
								mouseNowHover.isTract = &checkTract;
								mouseNowHover.isDendrite = &GetDendrite(checkTract, d);
								return true;
							}

						}
					}
				}
			}
			else
			{
				// try and locate a tract (only possible if dendrites are hidden)
				if(Online(mousePos, srcLobeX+(srcLobeWidth/2), srcLobeY+(srcLobeHeight/2), dstLobeX+(dstLobeWidth/2), dstLobeY+(dstLobeHeight/2)))		
				{
					// on tract
					mouseNowHover.over = isTract;
					mouseNowHover.isTract = &checkTract;
					return true;
				}
			}
		}
	}

	return false;
}


// -------------------------------------------------------------------------
// Method:		MapPath
// Arguments:	type of object mouse is on, poition for tool tips
// Returns:		none
// Description:	Unhighlight the last path an maps the current path of the 
//				object the mouse is on.
//				mouse may be on more than one object at once.
//				if the mouse is on a lobe and the left button is down
//				that lobe is always pathed
//				else if on a tract and the left button is down then that is 
//				always pathed.
//				Method does not path if the object is the same at the last 
//				time it was called unless the left mouse button state has 
//				changed
//				Uses the myMouseHover, myMouseHover.isLobe, myMouseHover.isTract, myMouseHover.isDendrite and
//				myMouseHover.isNeuron members to tell what the mouse was on last and
//				changes these memebers if a new path is drawn
//				Also updates the status bar
// -------------------------------------------------------------------------
void BrainViewport::MapPath(MouseHover mouseNowHover, POINT *tipsPos)
{
	static bool leftButtonDownPreviously = false;
	
#ifdef PATHORGANS
	// do appropriate viewport display and status bar
	if(myLeftButtonDown && mouseNowOver.isLobe)
		mouseNowHover.over = isLobe;
	else if(myLeftButtonDown && mouseNowOver.isTract)
		mouseNowHover.over = isTract;
#endif
	

	// if was on lobe and lobe changed - unhighlight
	if(myMouseHover.isLobe && myMouseHover.isLobe != mouseNowHover.isLobe)
	{
		RECT lobeRect = {(int)BitMapXCoord(GetLobeX(*myMouseHover.isLobe)), 
						(int)BitMapYCoord(GetLobeY(*myMouseHover.isLobe)), 
						(int)BitMapXCoord(GetLobeX(*myMouseHover.isLobe)+GetLobeWidth(*myMouseHover.isLobe))+1, 
						(int)BitMapYCoord(GetLobeY(*myMouseHover.isLobe)+GetLobeHeight(*myMouseHover.isLobe))+1};
		InvalidateRect(myHWnd, &lobeRect, false);
	}

	
	// hovering over this lobe so highlight
	if(mouseNowHover.isLobe)
	{
		RECT lobeRect = {(int)BitMapXCoord(GetLobeX(*mouseNowHover.isLobe)), 
						(int)BitMapYCoord(GetLobeY(*mouseNowHover.isLobe)), 
						(int)BitMapXCoord(GetLobeX(*mouseNowHover.isLobe)+GetLobeWidth(*mouseNowHover.isLobe))+1, 
						(int)BitMapYCoord(GetLobeY(*mouseNowHover.isLobe)+GetLobeHeight(*mouseNowHover.isLobe))+1};
		InvalidateRect(myHWnd, &lobeRect, false);
	}


	// path is over object
	switch(mouseNowHover.over)
	{
	case isLobe:
		// note: lobe may be same but neuron may not be on a neuron any more
		if(myMouseHover.isLobe != mouseNowHover.isLobe 
			|| (myMouseHover.isNeuron != mouseNowHover.isNeuron && !myLeftButtonDown)
			|| (myMouseHover.isDendrite != mouseNowHover.isDendrite && !myLeftButtonDown) 
			|| (myLeftButtonDown && !leftButtonDownPreviously)	// highlight
			|| (leftButtonDownPreviously && !myLeftButtonDown))	// unhighlight
		{
			UnMapPath();

			myMouseHover.over = mouseNowHover.over;	
			myMouseHover.isLobe = mouseNowHover.isLobe;

			if(myNeuronViewVar == hideDetails) PathLobe(*myMouseHover.isLobe, true, true, true);

			InvalidateRect(myHWnd, NULL, false);

			char tip[100];
			sprintf(tip,"%s lobe", myLobeNames->GetLobeFullName(LobeIdInList(*myMouseHover.isLobe)));
			if(myTips && tipsPos) myTips->PopUp(tip, *tipsPos, true, true);
		}
 
		break;
		
	case isNeuron:		
		if(myMouseHover.isNeuron != mouseNowHover.isNeuron)
		{
			UnMapPath();

			myMouseHover.over = mouseNowHover.over;
			myMouseHover.isNeuron = mouseNowHover.isNeuron;
			myMouseHover.isLobe = mouseNowHover.isLobe;

			PathNeuron(*myMouseHover.isLobe, *myMouseHover.isNeuron, true, true, true);

			InvalidateRect(myHWnd, NULL, false);

			char tip[100];
			sprintf(tip,"%s in %s lobe", myLobeNames->GetNeuronName(LobeIdInList(*myMouseHover.isLobe), NeuronIdInList(*myMouseHover.isNeuron)), myLobeNames->GetLobeFullName(LobeIdInList(*myMouseHover.isLobe)));
			if(myTips && tipsPos) myTips->PopUp(tip, *tipsPos, true, true);
		}
		break;

	case isDendrite:		
		if(myMouseHover.isDendrite != mouseNowHover.isDendrite)
		{
			UnMapPath();

			myMouseHover.over = mouseNowHover.over;
			myMouseHover.isDendrite = mouseNowHover.isDendrite;
			myMouseHover.isTract = mouseNowHover.isTract;
			myMouseHover.isLobe = mouseNowHover.isLobe;

			PathDendrite(*myMouseHover.isTract, *myMouseHover.isDendrite, true, true, true);

			InvalidateRect(myHWnd, NULL, false);

			char tip[200];
			sprintf(tip,"Dendrite: (%d) from %s to %s in Tract: %s->%s", 
				DendriteIdInList(*myMouseHover.isDendrite), 
				myLobeNames->GetNeuronName(LobeIdInList(GetSrcLobe(*myMouseHover.isTract)), NeuronIdInList(*myMouseHover.isDendrite->srcNeuron)), 
				myLobeNames->GetNeuronName(LobeIdInList(GetDstLobe(*myMouseHover.isTract)), NeuronIdInList(*myMouseHover.isDendrite->dstNeuron)), 
				myLobeNames->GetLobeFullName(LobeIdInList(GetSrcLobe(*myMouseHover.isTract))), 
				myLobeNames->GetLobeFullName(LobeIdInList(GetDstLobe(*myMouseHover.isTract))));

			if(myTips && tipsPos) myTips->PopUp(tip, *tipsPos, true, true);
		}
		break;

	case isTract:
		if(myMouseHover.isTract != mouseNowHover.isTract
			|| (myLeftButtonDown && !leftButtonDownPreviously)	// highlight
			|| (leftButtonDownPreviously && !myLeftButtonDown))	// unhighlight
		{
			UnMapPath();

			myMouseHover.over = mouseNowHover.over;
			myMouseHover.isTract = mouseNowHover.isTract;
			
			if(myDendriteViewVar == -1) PathTract(*myMouseHover.isTract, true, true, true);

			InvalidateRect(myHWnd, NULL, false);
			
			char tip[100];
			sprintf(tip,"Tract: %s->%s",
				myLobeNames->GetLobeFullName(LobeIdInList(GetSrcLobe(*myMouseHover.isTract))), 
				myLobeNames->GetLobeFullName(LobeIdInList(GetDstLobe(*myMouseHover.isTract))));

			if(myTips && tipsPos) myTips->PopUp(tip, *tipsPos, true, true);
		}
		break;

	default:
		{
			if(myTips) myTips->GoAway();
			UnMapPath();
		}
	}

	SendMessage(myHWnd, WM_PAINT, 0, 0);	// force repaint now;

	leftButtonDownPreviously = myLeftButtonDown;
}


// -------------------------------------------------------------------------
// Method:		UnMapPath
// Arguments:	none
// Returns:		none
// Description:	Unhighlight the path indicated by the myMouseHover member
// -------------------------------------------------------------------------
void BrainViewport::UnMapPath()
{
	// unpath 
	switch(myMouseHover.over)
	{
	case isLobe:
		if(myNeuronViewVar == -1)
		{
			PathLobe(*myMouseHover.isLobe, true, true, false);
			InvalidateRect(myHWnd, NULL, false);
		}
		break;

	case isNeuron:
		PathNeuron(*myMouseHover.isLobe, *myMouseHover.isNeuron, true, true, false);
		InvalidateRect(myHWnd, NULL, false);
		break;
	
	case isDendrite:
		PathDendrite(*myMouseHover.isTract, *myMouseHover.isDendrite, true, true, false);
		InvalidateRect(myHWnd, NULL, false);
	
	case isTract:
		if(myDendriteViewVar == -1)
		{
			PathTract(*myMouseHover.isTract, true, true, false);
			InvalidateRect(myHWnd, NULL, false);
		}
		break;
	}

	// clear mouse on flags
	myMouseHover.over = isNothing;
	myMouseHover.isLobe = NULL;
	myMouseHover.isTract = NULL;
	myMouseHover.isNeuron = NULL;
	myMouseHover.isDendrite = NULL;
}




// -------------------------------------------------------------------------
// Method:		PathLobe
// Arguments:	none
// Returns:		lobe to path, whether to path src/dst, whether to use
//				highlight pen, device contect to print to
// Description:	Prints the lobe in the desired pen, paths source and/or
//				destinations. If viewing neuron details will print all 
//				neurons too
// -------------------------------------------------------------------------
void BrainViewport::PathLobe(Lobe &lobe, bool src, bool dst, bool highlight)
{
	int t, l;
	
	l = LobeIdInList(lobe);

	if((myLobePath[l].mySrc == highlight && src) || (myLobePath[l].myDst == highlight && dst))
		// already pathed this route with this pen
		return;
	else
	{
		// flag path being travelled
		if(src)
			myLobePath[l].mySrc = highlight;
		if(dst)
			myLobePath[l].myDst = highlight;
	}

	for(t = 0; t != GetNoTracts(*myBrain); t++)
	{
		if(src && GetDstLobe(GetTract(*myBrain, t)) == lobe)
			PathTract(GetTract(*myBrain, t), src, false, highlight);

		if(dst)
		{
			Tract &tract = GetTract(*myBrain, t);

			if(GetSrcLobe(tract) == lobe)
				PathTract(tract, false, dst, highlight);

		}
	}

}


// -------------------------------------------------------------------------
// Method:		PathNeuron
// Arguments:	lobe neuron is in, neuron to path whether to path src/dst, 
//				whether to use	highlight pen, device contect to print to
// Returns:		none
// Description:	Prints the neuron in the desired pen, paths source and/or
//				destinations. If NOT viewing neuron details will lobe
// -------------------------------------------------------------------------
void BrainViewport::PathNeuron(Lobe &lobe, Neuron &neuron, bool src, bool dst, bool highlight)
{
	int t, d, l, n;

	l = LobeIdInList(lobe);
	n = NeuronIdInList(neuron);

	if((myNeuronPath[l][n].mySrc == highlight && src) || (myNeuronPath[l][n].myDst == highlight && dst))
		// already pathed this route with this pen
		return;
	else
	{
		if(src)
			myNeuronPath[l][n].mySrc = highlight;
		if(dst)
			myNeuronPath[l][n].myDst = highlight;
	}

	for (t=0; t != GetNoTracts(*myBrain); t++) 
	{
		Tract &tract = GetTract(*myBrain, t);
		if(src && GetDstLobe(tract) == lobe)
		{
			// tract is a source to the lobe check for dens that are a source to
			// to the neuron
			for(d = 0; d != GetTractSize(tract); d++)
			{
				if(GetDstNeuron(tract, d) == &neuron)
					PathDendrite(tract, GetDendrite(tract, d), src, false, highlight);
			}
		}

		if(dst && GetSrcLobe(tract) == lobe)
		{
			// tract is a destination of the lobe check for dens that are a dst to
			// to the neuron
			for(d = 0; d != GetTractSize(tract); d++)
			{
				if(GetSrcNeuron(tract, d) == &neuron)
					PathDendrite(tract, GetDendrite(tract, d), false, dst, highlight);
			}
		}
	}
	

}



// -------------------------------------------------------------------------
// Method:		PathTract
// Arguments:	tract to path whether to path src/dst, 
//				whether to use	highlight pen, device contect to print to
// Returns:		none
// Description:	Prints the dendrite in the desired pen, paths source and/or
//				destinations. If viewing dendrite details will all dendrites
// -------------------------------------------------------------------------
void BrainViewport::PathTract(Tract &tract, bool src, bool dst, bool highlight)
{
	int t;

	t = TractIdInList(tract);

	if((myTractPath[t].mySrc == highlight && src) || (myTractPath[t].myDst == highlight && dst))
		// already pathed this route with this pen
		return;


	// path lobes after so they get printed on top
	if(src)
	{
		myTractPath[t].mySrc = highlight;
		PathLobe(GetSrcLobe(tract) , src, false, highlight);
	}
	if(dst)
	{
		myTractPath[t].myDst = highlight;
		PathLobe(GetDstLobe(tract) , false, dst, highlight);
	}
}


// -------------------------------------------------------------------------
// Method:		PathDendrite
// Arguments:	tract dendrite is in, dendrite to path whether to path src/dst, 
//				whether to use	highlight pen, device contect to print to
// Returns:		none
// Description:	Prints the dendrite in the desired pen, paths source and/or
//				destinations. If NOT viewing dendrite details will print tract
// -------------------------------------------------------------------------
void BrainViewport::PathDendrite(Tract &tract, Dendrite &dendrite, bool src, bool dst, bool highlight)
{
	int t, d;

	t = TractIdInList(tract);
	d = DendriteIdInList(dendrite);

	
	if((myDendritePath[t][d].mySrc == highlight && src) || (myDendritePath[t][d].myDst == highlight && dst))
		// already pathed this route with this pen
		return;

	if(src)
	{	
		myDendritePath[t][d].mySrc = highlight;
		if(myMigrationLog[t][d].src != dendrite.srcNeuron)
			PathNeuron(GetSrcLobe(tract) , *myMigrationLog[t][d].src, src, false, highlight);
		else
			PathNeuron(GetSrcLobe(tract) , *dendrite.srcNeuron, src, false, highlight);
	}
	if(dst)
	{
		myDendritePath[t][d].myDst = highlight;
		if(myMigrationLog[t][d].dst != dendrite.dstNeuron)
			PathNeuron(GetDstLobe(tract) , *myMigrationLog[t][d].dst, false, dst, highlight);
		else
			PathNeuron(GetDstLobe(tract) , *dendrite.dstNeuron, false, dst, highlight);
	}


}


// -------------------------------------------------------------------------
// Method:		CheckNeuronMenu
// Arguments:	neuron variable to view, menu id to tick
// Returns:		none
// Description:	unchecks all the neuron variable items in the view menu
// -------------------------------------------------------------------------
void BrainViewport::CheckNeuronMenu(int neuronViewVar, int neuronMenuID)
{
	static int lastNeuronViewVar = 0;
	static int lastNeuronMenuID = ID_VIEW_NEURONVAR0;

	CheckMenuItem(myHViewMenu, ID_VIEW_HIDEDETAILS, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR0, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR1, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR2, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR3, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR4, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR5, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR6, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_NEURONVAR7, MF_UNCHECKED);
	
	if(neuronViewVar == -1)
	{
		myNeuronViewVar = lastNeuronViewVar;
		CheckMenuItem(myHViewMenu, lastNeuronMenuID, MF_CHECKED);	
	}
	else
	{
		myNeuronViewVar = neuronViewVar;
		CheckMenuItem(myHViewMenu, neuronMenuID, MF_CHECKED);
		lastNeuronViewVar = neuronViewVar;
		lastNeuronMenuID = neuronMenuID;
	}
	
	if(myDendriteViewVar == hideDetails)
		CheckDendriteMenu(-1, -1);

}


// -------------------------------------------------------------------------
// Method:		CheckDendriteMenu
// Arguments:	dendrite variable to view, menu id to tick
// Returns:		none
// Description:	unchecks all the dendrite variable items in the view menu
// -------------------------------------------------------------------------
void BrainViewport::CheckDendriteMenu(int dendriteViewVar, int dendriteMenuID)
{
	static int lastDendriteViewVar = 0;
	static int lastDendriteMenuID = ID_VIEW_DENDRITEVAR0;

	CheckMenuItem(myHViewMenu, ID_VIEW_HIDEDETAILS, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR0, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR1, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR2, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR3, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR4, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR5, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR6, MF_UNCHECKED);
	CheckMenuItem(myHViewMenu, ID_VIEW_DENDRITEVAR7, MF_UNCHECKED);

	if(dendriteViewVar == -1)
	{
		myDendriteViewVar = lastDendriteViewVar;
		CheckMenuItem(myHViewMenu, lastDendriteMenuID, MF_CHECKED);	
	}
	else
	{
		myDendriteViewVar = dendriteViewVar;
		CheckMenuItem(myHViewMenu, dendriteMenuID, MF_CHECKED);
		lastDendriteViewVar = dendriteViewVar;
		lastDendriteMenuID = dendriteMenuID;
	}
	
	if(myNeuronViewVar == hideDetails)
		CheckNeuronMenu(-1, -1);
}


// -------------------------------------------------------------------------
// Method:		OnlineUpdateLobe
// Arguments:	lobe number
// Returns:		none
// Description:	down loads lobe from game
// -------------------------------------------------------------------------
bool BrainViewport::OnlineUpdateLobe(int lobe)
{
	char macro[50];
	char *reply;

	sprintf(macro, "execute\nTARG AGNT %d BRN: DMPL %d", myOnlineId, lobe);
	if(theGameInterface.Inject(macro, &reply))
	{
	#ifdef _WIN32
		std::strstream in( reply, theGameInterface.GetBufferSize()-1, 
			std::ios_base::in | std::ios_base::binary );
	#else
		std::strstream in( reply, theGameInterface.GetBufferSize()-1 );
	#endif

		GetLobe(*myBrain,lobe).DoUpdateFromDesc(in);
		return true;
	}
	else
	{
		throw(OnlineFailedException(GetHwnd(myOwnerWnd)));
	}
}


// -------------------------------------------------------------------------
// Method:		OnlineUpdateTract
// Arguments:	tract number
// Returns:		none
// Description:	down loads tract from game
// -------------------------------------------------------------------------
bool BrainViewport::OnlineUpdateTract(int tract)
{
	char macro[50];
	char *reply;

	sprintf(macro, "execute\nTARG AGNT %d BRN: DMPT %d", myOnlineId, tract);
	if(theGameInterface.Inject(macro, &reply))
	{
#ifdef _WIN32
				std::strstream in( reply, theGameInterface.GetBufferSize()-1, 
					std::ios_base::in | std::ios_base::binary );
#else
				std::strstream in( reply, theGameInterface.GetBufferSize()-1 );
#endif

		GetTract(*myBrain, tract).DoUpdateFromDesc(in);
		return true;
	}
	else
	{
		throw(OnlineFailedException(GetHwnd(myOwnerWnd)));
	}
}


// -------------------------------------------------------------------------
// Method:		OnlineUpdateNeuron
// Arguments:	lobe, neuron
// Returns:		none
// Description:	down loads neuron from game
// -------------------------------------------------------------------------
bool BrainViewport::OnlineUpdateNeuron(Lobe &lobe, Neuron &neuron)
{
	char macro[50];
	char *reply;

	sprintf(macro, "execute\nTARG AGNT %d BRN: DMPN %d %d", myOnlineId, LobeIdInList(lobe), NeuronIdInList(neuron));
	if(theGameInterface.Inject(macro, &reply))
	{
#ifdef _WIN32
		std::strstream in( reply, theGameInterface.GetBufferSize()-1, 
			std::ios_base::in | std::ios_base::binary );
#else
		std::strstream in( reply, theGameInterface.GetBufferSize()-1 );
#endif

		lobe.DoUpdateNeuronFromDesc(NeuronIdInList(neuron), in);
		return true;
	}
	else
	{
		throw(OnlineFailedException(GetHwnd(myOwnerWnd)));
	}
}


// -------------------------------------------------------------------------
// Method:		OnlineUpdateDendrite
// Arguments:	tract, dendrite
// Returns:		none
// Description:	down loads dendrite from game
// -------------------------------------------------------------------------
bool BrainViewport::OnlineUpdateDendrite(Tract &tract, Dendrite &dendrite)
{
	char macro[50];
	char *reply;

	sprintf(macro, "execute\nTARG AGNT %d BRN: DMPD %d %d", myOnlineId, TractIdInList(tract), DendriteIdInList(dendrite));
	if(theGameInterface.Inject(macro, &reply))
	{
	#ifdef _WIN32
		std::strstream in( reply, theGameInterface.GetBufferSize()-1, 
			std::ios_base::in | std::ios_base::binary );
	#else
		std::strstream in( reply, theGameInterface.GetBufferSize()-1 );
	#endif

		tract.DoUpdateDendriteFromDesc(DendriteIdInList(dendrite), in);
		return true;
	}
	else
	{
		throw(OnlineFailedException(GetHwnd(myOwnerWnd)));
	}
}


// -------------------------------------------------------------------------
// Method:		SetThreshHold
// Arguments:	none
// Returns:		opens a window to input the threshhold
// -------------------------------------------------------------------------
void BrainViewport::SetThreshHold()
{
	try 
	{
		ThreshHoldDlg *thd = new ThreshHoldDlg(GetWindowInstance(GetHwnd(myOwnerWnd)),
			GetHwnd(myOwnerWnd), myViewThreshHoldStyle, myViewThreshHold);
	}
	catch(Wnd::FailConstructorException)
	{
	}
}


// -------------------------------------------------------------------------
// Method:		SwitchInstinctProcessingOnOff(bool b)
// Arguments:	none
// Returns:		opens a window to input the threshhold
// -------------------------------------------------------------------------
void BrainViewport::SwitchInstinctProcessingOnOff(bool b)
{
	myBrain->SetWhetherToProcessInstincts(b);
}



// -------------------------------------------------------------------------
// Method:		GetGeneAge
// Arguments:	starting gene age
// Returns:		gets a gene expresion age
// -------------------------------------------------------------------------
int BrainViewport::GetGeneAge(int geneAge)
{
	GeneAgeDlg *gad = new GeneAgeDlg(GetWindowInstance(GetHwnd(myOwnerWnd)),
		GetHwnd(myOwnerWnd), geneAge);

	gad->ModalMessageLoop();
	delete gad;

	return geneAge;
}

