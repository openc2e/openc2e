// ----------------------------------------------------------------------------
// Filename:	BrainDlgContainer.h
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
#ifndef BrainDlgContainer_H
#define BrainDlgContainer_H

#ifdef _MSC_VER
#pragma warning(disable:4786 4503)
#endif

#include <list>
#include <vector>
#include "BrainDlg.h"
#include "VarDlg.h"
#include "SVRuleDlg.h"
#include "LobeInputDlg.h"
#include "VarGraphDlg.h"
#include "LobeGraphDlg.h"
#include "../../engine/Creature/Brain/BrainAccess.h"
#include "LobeNames.h"
#include "SVCaptions.h"



class BrainDlgContainer : public BrainAccess
{
	friend VarDlg::~VarDlg();
	friend VarGraphDlg::~VarGraphDlg();
	friend LobeGraphDlg::~LobeGraphDlg();
	friend SVRuleDlg::~SVRuleDlg();
	friend LobeInputDlg::~LobeInputDlg();


	enum DlgType {isVariable, isGraph, isSVRule, isLobeInput};
	enum BrainObject {isLobe, isTract, isNeuron, isDendrite};

	typedef struct
	{
		int menuId;
		HWND hWnd;
		BrainDlg *dlg;
		DlgType type;
		BrainObject brainObject;
	} DlgObject;

public:
	
	// -------------------------------------------------------------------------
	// Method:		BrainDlgContainer
	// Arguments:	none
	// Returns:		none
	// Description:	constructor
	// -------------------------------------------------------------------------
	BrainDlgContainer(LobeNames &lobeNames) :
	myLobeNames(lobeNames)
	{
		myHOnlineMenu = CreateMenu();
		AppendMenu(myHOnlineMenu, MF_STRING | MF_GRAYED, 0, "None");
		myHOfflineMenu = CreateMenu();
		AppendMenu(myHOfflineMenu, MF_STRING | MF_GRAYED, 0, "None");
	};

	// -------------------------------------------------------------------------
	// Method:		~BrainDlgContainer
	// Arguments:	none
	// Returns:		none
	// Description:	destructor - cleans up all lists destroying windows
	// -------------------------------------------------------------------------
	~BrainDlgContainer();

	// -------------------------------------------------------------------------
	// Method:		SetMenu
	// Arguments:	handle to menu, whether to append on or Offline menu t=Online
	// Returns:		none
	// Description:	uses menu passed as menu to add dlg object popup to
	// -------------------------------------------------------------------------
	void SetMenu(HMENU menu, bool Online) 
	{ 
		AppendMenu(menu, MF_ENABLED | MF_POPUP, 
		(UINT_PTR)(Online ? myHOnlineMenu : myHOfflineMenu), (LPSTR)"&Windows");
	};

	// -------------------------------------------------------------------------
	// Method:		CreateNeuronVarDlg
	// Arguments:	neuron to create for, lobe neuron is in, owner window
	// Returns:		none
	// Description:	creates a variable dialog window  for the specified neuron
	// -------------------------------------------------------------------------
	void CreateNeuronVarDlg(Neuron &neuron, Lobe &lobe, HWND hWndOwner);

	// -------------------------------------------------------------------------
	// Method:		CreateDendriteVarDlg
	// Arguments:	dendrite to create for, tract dendrite is in, owner window
	// Returns:		none
	// Description:	creates a variable dialog window  for the specified dendrite
	// -------------------------------------------------------------------------
	void CreateDendriteVarDlg(Dendrite &dendrite, Tract &tract, HWND hWndOwner);

	// -------------------------------------------------------------------------
	// Method:		CreateNeuronVarGraphDlg
	// Arguments:	neuron to create for, lobe neuron is in, owner window
	// Returns:		none
	// Description:	creates a variable graph window  for the specified neuron
	// -------------------------------------------------------------------------
	void CreateNeuronVarGraphDlg(Neuron &neuron, Lobe &lobe, HWND hWndOwner);

	// -------------------------------------------------------------------------
	// Method:		CreateDendriteVarGraphDlg
	// Arguments:	dendrite to create for, tract dendrite is in, owner window
	// Returns:		none
	// Description:	creates a variable graph window  for the specified dendrite
	// -------------------------------------------------------------------------
	void CreateDendriteVarGraphDlg(Dendrite &dendrite, Tract &tract, HWND hWndOwner);

	// -------------------------------------------------------------------------
	// Method:		CreateLobeVarGraphDlg
	// Arguments:	lobe to create for, variable to use, owner window
	// Returns:		none
	// Description:	creates a variable graph window  for the specified dendrite
	// -------------------------------------------------------------------------
	void CreateLobeGraphDlg(Lobe &lobe, int var, HWND hWndOwner);

	// -------------------------------------------------------------------------
	// Method:		CreateLobeDlg
	// Arguments:	lobe to create window for, owners window handle, captions
	// Returns:		none
	// Description:	creates a lobe dialog window and adds it to the list of 
	//				SVRuleDlgs
	// -------------------------------------------------------------------------
	void CreateLobeDlg(Lobe &lobe, HWND hWndOwner, 
				const SVCaptions::Caption * const captions);

	// -------------------------------------------------------------------------
	// Method:		CreateTractDlg
	// Arguments:	tract to create window for, owners window handle, caption
	// Returns:		none
	// Description:	creates a tract dialog window and adds it to the list of 
	//				SVRuleDlgs
	// -------------------------------------------------------------------------
	void CreateTractDlg(Tract &tract, HWND hWndOwner,
				const SVCaptions::Caption * const captions);

	// -------------------------------------------------------------------------
	// Method:		CreateLobeInputDlg
	// Arguments:	lobe to create for, window handle of owner window
	// Returns:		none
	// Description:	Creates the lobe input dialog windows for the lobe.
	// -------------------------------------------------------------------------
	void CreateLobeInputDlg(Lobe &lobe, HWND hWndOwner);


	// -------------------------------------------------------------------------
	// Method:		Update
	// Arguments:	none
	// Returns:		none
	// Description:	Updates the owned windows - Using the neurons/dendrites/lobe
	//				tracts they are connected to a the update source
	// -------------------------------------------------------------------------
	void Update();

	// -------------------------------------------------------------------------
	// Method:		DumpToLobes
	// Arguments:	none
	// Returns:		none
	// Description:	Dumps the contents from dialogs to the brain.
	//				Specifically lobeinputdlgs to thier	respective lobes
	// -------------------------------------------------------------------------
	void DumpToLobes();

	// -------------------------------------------------------------------------
	// Method:		Activate
	// Arguments:	lobe id
	// Returns:		success or failure bool
	// Description:	Activates the dlgs that corresponds to the menu id
	// -------------------------------------------------------------------------
	bool Activate(UINT menuID);

	// -------------------------------------------------------------------------
	// Method:		HasUpdatableDlgs
	// Arguments:	lobe
	// Returns:		whether it has updatable dlgs t/f
	// Description:	i.e. graphs and other such dialogs need to be updated
	// -------------------------------------------------------------------------
	bool HasUpdatableDlgs(Lobe &lobe);

	// -------------------------------------------------------------------------
	// Method:		HasUpdatableDlgs
	// Arguments:	tract
	// Returns:		whether it has updatable dlgs t/f
	// Description:	i.e. graphs and other such dialogs need to be updated
	// -------------------------------------------------------------------------
	bool HasUpdatableDlgs(Tract &tract);

	// -------------------------------------------------------------------------
	// Method:		HasUpdatableDlgs
	// Arguments:	lobe, neuron,
	// Returns:		whether it has updatable dlgs t/f
	// Description:	fills in the lobe and neuron vectors with the neurons and thier
	//				lobes that have updatable dlgs
	//				i.e. graphs and other such dialogs need to be updated
	// -------------------------------------------------------------------------
	bool HasUpdatableDlgs(std::vector<Lobe *> &lobes, std::vector<Neuron *> &neurons);


	// -------------------------------------------------------------------------
	// Method:		HasUpdatableDlgs
	// Arguments:	tract, dendrite
	// Returns:		whether it has updatable dlgs t/f
	// Description:	fills in the tract and dendrite vectors with the dendrites and 
	//				their tracts that have updatable dlgs
	//				i.e. graphs and other such dialogs need to be updated
	// -------------------------------------------------------------------------
	bool HasUpdatableDlgs(std::vector<Tract *> &tracts, std::vector<Dendrite *> &dendrites);

/*	// -------------------------------------------------------------------------
	// Method:		BringToFore
	// Arguments:	none
	// Returns:		none
	// Description:	brings all dialogs to foreground
	// -------------------------------------------------------------------------
	void BringToFore();
*/
private:

	std::list<DlgObject> myDlgs;

	HMENU myHOnlineMenu;
	HMENU myHOfflineMenu;

	LobeNames &myLobeNames;

	// -------------------------------------------------------------------------
	// Method:		DlgExistsForObject
	// Arguments:	pointer to object being queried, type of dialog
	// Returns:		true of false
	// Description:	checks if a dialog exists for the object. If it does then
	//				the dialog is activated
	// -------------------------------------------------------------------------
	bool BrainDlgContainer::DlgExistsForObject(void *object, DlgType type);

	// -------------------------------------------------------------------------
	// Method:		RemoveDlg
	// Arguments:	dlg - pointer to dialog window object
	// Returns:		none
	// Description:	removes dialog window from list of dialog 
	//				windows - only called by Dialogs destructors
	// -------------------------------------------------------------------------
	void RemoveDlg(BrainDlg *dlg);

	// -------------------------------------------------------------------------
	// Method:		AddToMenu
	// Arguments:	text to add, disable on Online menu t/f
	// Returns:		none
	// Description:	add text to the menu
	// -------------------------------------------------------------------------
	void AddToMenu(char *menuText, bool disableOnline = false);
	
	// -------------------------------------------------------------------------
	// Method:		RemoveFromMenu
	// Arguments:	item to delete
	// Returns:		none
	// Description:	removes and item from the online and offline menus
	// -------------------------------------------------------------------------
	void RemoveFromMenu(int menuItem);

};
#endif

