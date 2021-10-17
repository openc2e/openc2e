// ----------------------------------------------------------------------------
// Filename:	BrainDlgContainer.cpp
// Class:		BrainDlgContainer
// Purpose:		Container class for Brain Editing Dlg objects 
// Description:	Controls the creation and deletions of Dlg objects
//				keeping them in a lists. When this 
//				object is deleted it removes all the dialogs. All Dlg 
//				objects are	initialised with a referance to a BrainDlgContainer. 
//				When a dialog is closed it removes itself from its container
// Usage:		Create the object and use a Create funtions to create the 
//				required dlg
// -----------------------------------------------------------------------------
#include "BrainDlgContainer.h"
#include "LobeDefinition.h"


// -------------------------------------------------------------------------
// Method:		~BrainDlgContainer
// Arguments:	none
// Returns:		none
// Description:	destructor - cleans up all lists destroying windows
// -------------------------------------------------------------------------
BrainDlgContainer::~BrainDlgContainer()
{
	//destroy any remaining child windows
	while(myDlgs.size() != 0)
		DestroyWindow(myDlgs.begin()->hWnd);
}




// -------------------------------------------------------------------------
// Method:		CreateNeuronVarDlg
// Arguments:	neuron to create for, lobe neuron is in, owner window
// Returns:		none
// Description:	creates a variable dialog window  for the specified neuron
// -------------------------------------------------------------------------
void BrainDlgContainer::CreateNeuronVarDlg(Neuron &neuron, Lobe &lobe, HWND hWndOwner)
{
	HWND hWnd;
	NeuronVarDlg *wnd = NULL;

	if(!DlgExistsForObject(&neuron, isVariable))
	{
		// else create new dlg
		if((wnd = new NeuronVarDlg(GetWindowInstance(hWndOwner), hWndOwner, neuron, lobe, myLobeNames, *this)))
		{
			if((hWnd = wnd->Create()))
			{
				DlgObject dlgObj;
				dlgObj.dlg = wnd;
				dlgObj.hWnd = hWnd;
				dlgObj.type = isVariable;
				dlgObj.brainObject = isNeuron;
				myDlgs.push_back(dlgObj);
		
				char menuText[50];
				sprintf(menuText,"Variables Neuron:%d, %s", NeuronIdInList(neuron), GetLobeName(lobe));
				AddToMenu(menuText);
			}
			else
				delete wnd;
		}
	}
}


// -------------------------------------------------------------------------
// Method:		CreateDendriteVarDlg
// Arguments:	dendrite to create for, tract dendrite is in, owner window
// Returns:		none
// Description:	creates a variable dialog window  for the specified dendrite
// -------------------------------------------------------------------------
void BrainDlgContainer::CreateDendriteVarDlg(Dendrite &dendrite, Tract &tract, HWND hWndOwner)
{
	HWND hWnd;
	DendriteVarDlg *wnd = NULL;
	
	if(!DlgExistsForObject(&dendrite, isVariable))
	{
		//else create new dlg
		if((wnd = new DendriteVarDlg(GetWindowInstance(hWndOwner), hWndOwner, dendrite, tract, myLobeNames, *this)))
		{
			if((hWnd = wnd->Create()))
			{
				DlgObject dlgObj;
				dlgObj.dlg = wnd;
				dlgObj.hWnd = hWnd;
				dlgObj.type = isVariable;
				dlgObj.brainObject = isDendrite;
				myDlgs.push_back(dlgObj);

				char menuText[50];
				sprintf(menuText,"Variables Dendrite:%d, %s", DendriteIdInList(dendrite), GetTractName(tract));
				AddToMenu(menuText);
			}
			else
				delete wnd;
		}
	}
}


// -------------------------------------------------------------------------
// Method:		CreateNeuronVarGraphDlg
// Arguments:	neuron to create for, lobe neuron is in, owner window
// Returns:		none
// Description:	creates a variable graph window  for the specified neuron
// -------------------------------------------------------------------------
void BrainDlgContainer::CreateNeuronVarGraphDlg(Neuron &neuron, Lobe &lobe, HWND hWndOwner)
{
	HWND hWnd;
	NeuronVarGraphDlg *wnd = NULL;
	
	if(!DlgExistsForObject(&neuron, isGraph))
	{
		// else create new dlg	
		if((wnd = new NeuronVarGraphDlg(GetWindowInstance(hWndOwner), hWndOwner, neuron, lobe, myLobeNames, *this)))
		{
			if((hWnd = wnd->Create()))
			{
				DlgObject dlgObj;
				dlgObj.dlg = wnd;
				dlgObj.hWnd = hWnd;
				dlgObj.type = isGraph;
				dlgObj.brainObject = isNeuron;
				myDlgs.push_back(dlgObj);

				char menuText[50];
				sprintf(menuText,"Graph Neuron:%d, %s", NeuronIdInList(neuron), GetLobeName(lobe));
				AddToMenu(menuText);
			}
			else
				delete wnd;
		}
	}
}


// -------------------------------------------------------------------------
// Method:		CreateDendriteVarGraphDlg
// Arguments:	dendrite to create for, tract dendrite is in, owner window
// Returns:		none
// Description:	creates a variable graph window  for the specified dendrite
// -------------------------------------------------------------------------
void BrainDlgContainer::CreateDendriteVarGraphDlg(Dendrite &dendrite, Tract &tract, HWND hWndOwner)
{
	HWND hWnd;
	DendriteVarGraphDlg *wnd = NULL;
	if(!DlgExistsForObject(&dendrite, isGraph))
	{

		// else create new dlg
		if((wnd = new DendriteVarGraphDlg(GetWindowInstance(hWndOwner), hWndOwner, dendrite, tract, myLobeNames, *this)))
		{
			if((hWnd = wnd->Create()))
			{
				DlgObject dlgObj;
				dlgObj.dlg = wnd;
				dlgObj.hWnd = hWnd;
				dlgObj.type = isGraph;
				dlgObj.brainObject = isDendrite;
				myDlgs.push_back(dlgObj);

				char menuText[50];
				sprintf(menuText,"Graph Dendrite:%d, %s", DendriteIdInList(dendrite), GetTractName(tract));
				AddToMenu(menuText);
			}
			else
				delete wnd;
		}
	}
}

// -------------------------------------------------------------------------
// Method:		CreateLobeGraphDlg
// Arguments:	lobe to create for, variable to use, owner window
// Returns:		none
// Description:	creates a variable graph window  for the specified dendrite
// -------------------------------------------------------------------------
void BrainDlgContainer::CreateLobeGraphDlg(Lobe &lobe, int var, HWND hWndOwner)
{
	HWND hWnd;
	LobeGraphDlg *wnd = NULL;
	
	if(!DlgExistsForObject(&lobe, isGraph))
	{
		// else create new dlg
		if((wnd = new LobeGraphDlg(GetWindowInstance(hWndOwner), hWndOwner, lobe, var, myLobeNames ,*this)))
		{
			if((hWnd = wnd->Create()))
			{		
				DlgObject dlgObj;
				dlgObj.dlg = wnd;
				dlgObj.hWnd = hWnd;
				dlgObj.type = isGraph;
				dlgObj.brainObject = isLobe;
				myDlgs.push_back(dlgObj);

				char menuText[50];
				sprintf(menuText,"Graph Lobe:%s", GetLobeName(lobe));
				AddToMenu(menuText);
			}
			else
				delete wnd;
		}
	}
}


// -------------------------------------------------------------------------
// Method:		CreateLobeDlg
// Arguments:	lobe to create window for, owners window handle, caption
// Returns:		none
// Description:	creates a lobe dialog window and adds it to the list of 
//				SVRuleDlgs
// -------------------------------------------------------------------------
void BrainDlgContainer::CreateLobeDlg(Lobe &lobe, HWND hWndOwner, 
			  const SVCaptions::Caption * const captions)
{
	HWND hWnd;
	LobeDlg *wnd = NULL;
	
	if(!DlgExistsForObject(&lobe, isSVRule))
	{
		// create new window
		if((wnd = new LobeDlg(GetWindowInstance(hWndOwner), hWndOwner, lobe, captions, myLobeNames, *this)))
		{
			if((hWnd = wnd->Create()))
			{
				DlgObject dlgObj;
				dlgObj.dlg = wnd;
				dlgObj.hWnd = hWnd;
				dlgObj.type = isSVRule;
				dlgObj.brainObject = isLobe;
				myDlgs.push_back(dlgObj);

				char menuText[50];
				sprintf(menuText,"SV Rule %s", GetLobeName(lobe));
				AddToMenu(menuText);
			}
			else
				delete wnd;
		}
	}
}


// -------------------------------------------------------------------------
// Method:		CreateTractDlg
// Arguments:	tract to create window for, owners window handle, caption
// Returns:		none
// Description:	creates a tract dialog window and adds it to the list of 
//				SVRuleDlgs
// -------------------------------------------------------------------------
void BrainDlgContainer::CreateTractDlg(Tract &tract, HWND hWndOwner,
				 const SVCaptions::Caption * const captions)
{
	HWND hWnd;
	TractDlg *wnd = NULL;
	
	if(!DlgExistsForObject(&tract, isSVRule))
	{
		// create new window
		if((wnd = new TractDlg(GetWindowInstance(hWndOwner), hWndOwner, tract, captions, myLobeNames, *this)))
		{
			if((hWnd = wnd->Create()))
			{
				DlgObject dlgObj;
				dlgObj.dlg = wnd;
				dlgObj.hWnd = hWnd;
				dlgObj.type = isSVRule;
				dlgObj.brainObject = isTract;
				myDlgs.push_back(dlgObj);

				char menuText[50];
				sprintf(menuText,"SV Rule %s", GetTractName(tract));
				AddToMenu(menuText);
			}
			else
				delete wnd;
		}
	}
}


// -------------------------------------------------------------------------
// Method:		CreateLobeInputDlg
// Arguments:	lobe to create for, window handle of owner window
// Returns:		none
// Description:	Creates the lobe input dialog windows for the lobe.
// -------------------------------------------------------------------------
void BrainDlgContainer::CreateLobeInputDlg(Lobe &lobe, HWND hWndOwner)
{

	if(!DlgExistsForObject(&lobe, isLobeInput))
	{


		// doesnt alreadt exist so create

		HWND hWnd;
		LobeInputDlg *wnd = NULL;
		if((wnd = new LobeInputDlg(GetWindowInstance(hWndOwner), hWndOwner, lobe, myLobeNames, *this)))
		{
			if((hWnd = wnd->Create()))
			{
				DlgObject dlgObj;
				dlgObj.dlg = wnd;
				dlgObj.hWnd = hWnd;
				dlgObj.type = isLobeInput;
				dlgObj.brainObject = isLobe;
				myDlgs.push_back(dlgObj);
				char menuName[50] = "Inputs for lobe: ";
				strcat(menuName, myLobeNames.GetLobeFullName(LobeIdInList(lobe)));
				AddToMenu(menuName, true);
			}
			else
				delete wnd;
		}
	}

	return;
}


// -------------------------------------------------------------------------
// Method:		DlgExistsForObject
// Arguments:	pointer to object being queried, type of dialog
// Returns:		true of false
// Description:	checks if a dialog exists for the object. If it does then
//				the dialog is activated
// -------------------------------------------------------------------------
bool BrainDlgContainer::DlgExistsForObject(void *object, DlgType type)
{
	std::list<DlgObject>::iterator d = myDlgs.begin();
	
	// check if already exists
	while(d != myDlgs.end())
	{
		if(((d->dlg)->LinkedTo() == object) && d->type == type)
		{
			// exists so bring to foreground
			(d->dlg)->Activate();
			return true;
		}
		d++;
	}

	return false;
}


// -------------------------------------------------------------------------
// Method:		RemoveDlg
// Arguments:	dlg - pointer to dialog window object
// Returns:		none
// Description:	removes dialog window from list of dialog 
//				windows - only called by Dialogs destructors
// -------------------------------------------------------------------------
void BrainDlgContainer::RemoveDlg(BrainDlg *dlg)
{
	//removes dialog window from the list 
	
	// check vardlgs to see if it in there
	{
		std::list<DlgObject>::iterator d = myDlgs.begin();

		while(d != myDlgs.end())
		{
			if(d->dlg == dlg)
			{
				RemoveFromMenu(d->menuId);
				myDlgs.erase(d);
				return;
			}
			d++;
		}
	}
}



// -------------------------------------------------------------------------
// Method:		Update
// Arguments:	none
// Returns:		none
// Description:	Updates the owned windows - Using the neurons/dendrites/lobe 
//				tracts they are connected to a the update source
// -------------------------------------------------------------------------
void BrainDlgContainer::Update()
{
	std::list<DlgObject>::iterator d = myDlgs.begin();

	while(d != myDlgs.end())
	{
		d->dlg->Update();
		d++;
	}
}


// -------------------------------------------------------------------------
// Method:		DumpToLobes
// Arguments:	none
// Returns:		none
// Description:	Dumps the contents of dialogs to the brain.
//				Specifically lobeinputdlg dlgs to thier	respective lobes
// -------------------------------------------------------------------------
void BrainDlgContainer::DumpToLobes()
{
	std::list<DlgObject>::iterator d = myDlgs.begin();

	while(d != myDlgs.end())
	{
		d->dlg->DumpToLobe();
		d++;
	}
}


// -------------------------------------------------------------------------
// Method:		Activate
// Arguments:	lobe id
// Returns:		success or failure bool
// Description:	Activates the dlgs that corresponds to the menu id
// -------------------------------------------------------------------------
bool BrainDlgContainer::Activate(UINT menuId)
{
	std::list<DlgObject>::iterator d = myDlgs.begin();
	
	// check if already exists
	while(d != myDlgs.end())
	{
		if(d->menuId == menuId)
		{
			// exists so bring to foreground
			(d->dlg)->Activate();
			return true;
		}
		d++;
	}
	return false;
}


// -------------------------------------------------------------------------
// Method:		HasUpdatableDlgs
// Arguments:	lobe
// Returns:		whether it has updatable dlgs t/f
// Description:	i.e. graphs and other such dialogs need to be updated
// -------------------------------------------------------------------------
bool BrainDlgContainer::HasUpdatableDlgs(Lobe& lobe)
{
	std::list<DlgObject>::iterator d = myDlgs.begin();

	while(d != myDlgs.end())
	{
		if(d->brainObject == isLobe && d->dlg->Updatable() 
			&& &lobe == (Lobe *)d->dlg->LinkedTo())
			return true;
		d++;
	}
	return false;
}


// -------------------------------------------------------------------------
// Method:		HasUpdatableDlgs
// Arguments:	tract
// Returns:		whether it has updatable dlgs t/f
// Description:	i.e. graphs and other such dialogs need to be updated
// -------------------------------------------------------------------------
bool BrainDlgContainer::HasUpdatableDlgs(Tract& tract)
{
	std::list<DlgObject>::iterator d = myDlgs.begin();

	while(d != myDlgs.end())
	{
		if(d->brainObject == isTract && d->dlg->Updatable()
			&& &tract == (Tract *)d->dlg->LinkedTo())
			return true;
		d++;
	}
	return false;
}


// -------------------------------------------------------------------------
// Method:		HasUpdatableDlgs
// Arguments:	lobe, neuron,
// Returns:		whether it has updatable dlgs t/f
// Description:	fills in the lobe and neuron vectors with the neurons and thier
//				lobes that have updatable dlgs.
//				i.e. graphs and other such dialogs need to be updated
// -------------------------------------------------------------------------
bool BrainDlgContainer::HasUpdatableDlgs(std::vector<Lobe *> &lobes, std::vector<Neuron *> &neurons)
{
	std::list<DlgObject>::iterator d = myDlgs.begin();
	bool return_val = false;

	while(d != myDlgs.end())
	{
		if(d != myDlgs.end() && d->brainObject == isNeuron && d->dlg->Updatable())
		{
			neurons.push_back((Neuron *)d->dlg->LinkedTo());
			lobes.push_back((Lobe *)d->dlg->OrganLinkedTo());
			return_val = true;
		}
		d++;
	}
	return return_val;
}


// -------------------------------------------------------------------------
// Method:		HasUpdatableDlgs
// Arguments:	tract, dendrite
// Returns:		whether it has updatable dlgs t/f
// Description:	fills in the tract and dendrite vector with the dendrites and 
//				their tracts that have updatable dlgs
//				i.e. graphs and other such dialogs need to be updated
// -------------------------------------------------------------------------
bool BrainDlgContainer::HasUpdatableDlgs(std::vector<Tract *> &tracts, std::vector<Dendrite *> &dendrites)
{
	std::list<DlgObject>::iterator d = myDlgs.begin();
	bool return_val = false;

	while(d != myDlgs.end())
	{
		if(d != myDlgs.end() && d->brainObject == isDendrite && d->dlg->Updatable())
		{
			dendrites.push_back((Dendrite *)d->dlg->LinkedTo());
			tracts.push_back((Tract *)d->dlg->OrganLinkedTo());
			return_val = true;
		}
		d++;
	}
	return return_val;
}

// -------------------------------------------------------------------------
// Method:		AddToMenu
// Arguments:	text to add, disbale on Online menu t/f
// Returns:		none
// Description:	add text to the menu
// -------------------------------------------------------------------------
void BrainDlgContainer::AddToMenu(char *menuText, bool disableOnline)
{
	int id = 0;
	std::list<DlgObject>::iterator d = myDlgs.begin();

	// find an id
	while(d != myDlgs.end() && d->menuId != id) 
	{
		id++;
		d++;
	}

	if(myDlgs.size() == 1)
	{
		DeleteMenu(myHOnlineMenu, 0, MF_BYPOSITION);
		DeleteMenu(myHOfflineMenu, 0, MF_BYPOSITION);
	}
	myDlgs.rbegin()->menuId = id;
	AppendMenu(myHOnlineMenu, (disableOnline ? MF_GRAYED : MF_ENABLED) | MF_STRING, id, menuText);
	AppendMenu(myHOfflineMenu, MF_ENABLED | MF_STRING, id, menuText);
}


// -------------------------------------------------------------------------
// Method:		RemoveFromMenu
// Arguments:	item to delete
// Returns:		none
// Description:	removes and item from the online and offline menus
// -------------------------------------------------------------------------
void BrainDlgContainer::RemoveFromMenu(int menuItem)
{
	DeleteMenu(myHOnlineMenu, menuItem, MF_BYCOMMAND);
	DeleteMenu(myHOfflineMenu, menuItem, MF_BYCOMMAND);

	if(myDlgs.size() == 0)
	{
		AppendMenu(myHOnlineMenu, MF_STRING | MF_GRAYED, 0, "None");
		AppendMenu(myHOfflineMenu, MF_STRING | MF_GRAYED, 0, "None");
	}
}


/*
all dlgs are presently topmost
// -------------------------------------------------------------------------
// Method:		BringToFore
// Arguments:	none
// Returns:		none
// Description:	brings all dialogs to foreground
// -------------------------------------------------------------------------
void BrainDlgContainer::BringToFore()
{
	std::list<DlgObject>::iterator d = myDlgs.begin();
	while(d != myDlgs.end())
	{
		BringWindowToTop( d->hWnd );
		RedrawWindow(d->hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);	
		d++;
	}
}

*/

