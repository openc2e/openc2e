// ----------------------------------------------------------------------------
// Filename:	BrainViewport.h
// Class:		BrainViewport
// Purpose:		Displays, updates and edits brain object
// Description:	Contains a brain object created from genome of given filename
//				Updates the brain, has editing windows to edit brain variables
//				and inputs
//	Usage:		May be called from a MainFrame object orequires the  application	
//				instance and the mainframe window handle on construction. 
//				InitInstance must be called after the constructor
//				and after the window has been created
// -----------------------------------------------------------------------------

#ifndef BrainViewport_H
#define BrainViewport_H

#ifdef _MSC_VER
#pragma warning(disable:4786 4503)
#endif

#include <windowsx.h>
#include <math.h>

#include "../../engine/Creature/Brain/BrainAccess.h"
#include "MainFrame.h"
#include "Viewport.h"
#include "BrainDlgContainer.h"
#include "LobeNames.h"
#include "Tips.h"
#include "SVCaptions.h"


class BrainViewport : public Viewport, public BrainAccess
{	
	friend SVRuleDlg::~SVRuleDlg();
	friend LobeInputDlg::~LobeInputDlg();

	enum{totalColours = 11};
	enum{hideDetails = -1};
	
	enum BrainObject {isNothing, isLobe, isNeuron, isDendrite, isTract};
	

	typedef struct 
	{
		BrainObject over;
		Lobe *isLobe;
		Neuron *isNeuron;
		Tract *isTract;
		Dendrite *isDendrite;
	} MouseHover;


	class Path 
	{ 
		// used to record paths highlighted/unhighlighted
	public:

		bool mySrc;
		bool myDst;

		Path()
		{
			mySrc = false;
			myDst = false;
		}

		bool hidden;

	};

	typedef struct 
	{
		Neuron *src;
		Neuron *dst;
	} DendriteConnectionLog;

public:

	class InitFailedException
	{
	public:
		InitFailedException(HWND hWnd, const char *what)
		{
			MessageBox(hWnd, what, "Brain Initialization Error", MB_OK | MB_ICONERROR);
		};
	};

	class OnlineFailedException
	{
	public:
		OnlineFailedException(HWND hWnd)
		{
			// should auto stop play on disconnect
			FORWARD_WM_COMMAND(hWnd, ID_FILE_DISCONNECTFROMCREATURES, 0, 0, SendMessage);
			MessageBox(hWnd, "Could not download brain.\nDisconnected.", "Brain Update Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
		};
	};

	enum ThreshHoldStyle {NO_THRESHHOLD, NON_ZERO, ABOVE_THRESHHOLD, BELOW_THRESHHOLD};



	// added by gtb
	inline int GetOnlineId()
	{
		return myOnlineId;
	}


	// -------------------------------------------------------------------------
	// Method:		BrainViewport
	// Arguments:	hInstance - application instance                       
	//				hWndOwner - handle to owner window
	//				filename - genome filename - if none attempt to connect to game
	//				using id.
	//				id - online id of creature
	// Returns:		none
	// Description:	Constructor - uses hInstance and hWndOwner to register the 
	//				and create the Window
	// -------------------------------------------------------------------------
	BrainViewport(const HINSTANCE hInstance, const HWND hWndOwner, char filename[], int id = 0);

	// -------------------------------------------------------------------------
	// Method:		~BrainViewport
	// Arguments:	none
	// Returns:		none
	// Description:	Destructor
	// -------------------------------------------------------------------------
	~BrainViewport();

	// -------------------------------------------------------------------------
	// Method:		InitInstance
	// Arguments:	
	// Returns:		success or failure
	// Description:	Initialises the viewport, 
	//				Uses string as genome myFilename to load a genome and create 
	//				a brain from it 
	// -------------------------------------------------------------------------
	bool LoadFromGenome();

	// -------------------------------------------------------------------------
	// Method:		LoadFromCreatures
	// Arguments:	none
	// Returns:		success or failure
	// Description:	Initialises the viewport, loading creature using myOnlineId 
	// -------------------------------------------------------------------------
	bool LoadFromCreatures();

	// -------------------------------------------------------------------------
	// Method:		Create
	// Arguments:	window dimensions
	// Returns:		handle to window created
	// Description:	creates the window
	// -------------------------------------------------------------------------
	HWND Create(int x, int y, int nWidth, int nHeight);

	// -------------------------------------------------------------------------
	// Method:		Update
	// Arguments:	update hidden only t/f
	// Returns:		none
	// Description:	Updates the brain
	// -------------------------------------------------------------------------
	void Update(bool updateHidden = false);

	// -------------------------------------------------------------------------
	// Method:		GetHMenu
	// Arguments:	0 = offline menu, 1 = offline reconnect menu, 2 = online menu
	// Returns:		handle to this objects personal menu
	// -------------------------------------------------------------------------
	inline HMENU GetHMenu(int menu)
	{
		if(menu == 0)
			return myHOfflineMainMenu;
		else if (menu == 1)
			return myHOfflineReconnectMainMenu;
		else
			return myHOnlineMainMenu;
	};


	// -------------------------------------------------------------------------
	// Method:		BrainDumpVersion
	// Arguments:	none
	// Returns:		version number float
	// Description:	
	// -------------------------------------------------------------------------
	float BrainDumpVersion(){return BrainAccess::BrainDumpVersion();};

	// -------------------------------------------------------------------------
	// Method:		SetThreshHold
	// Arguments:	none
	// Returns:		opens a window to input the threshhold
	// -------------------------------------------------------------------------
	void SetThreshHold();

	// -------------------------------------------------------------------------
	// Method:		SwitchInstinctProcessingOnOff(bool b)
	// Arguments:	none
	// Returns:		opens a window to input the threshhold
	// -------------------------------------------------------------------------
	void SwitchInstinctProcessingOnOff(bool b);

	// -------------------------------------------------------------------------
	// Method:		GetGeneAge
	// Arguments:	starting gene age for combo
	// Returns:		gets a gene expresion age
	// -------------------------------------------------------------------------
	int GetGeneAge(int geneAge);

protected:

	// -------------------------------------------------------------------------
	// Method:		SetCanvasSpec
	// Arguments:	none
	// Returns:		none
	// Description:	calculates the size of the brain to be used as printing 
	//				dimensions
	// -------------------------------------------------------------------------
	virtual void SetCanvasSpec();

	// -------------------------------------------------------------------------
	// Method:		WndProc
	// Arguments:	standard windprog args
	// Returns:		result of message handling
	// Description:	object specific message handler
	// -------------------------------------------------------------------------
	virtual LRESULT WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam);
	
	// -------------------------------------------------------------------------
	// Method:		On_Paint
	// Arguments:	none
	// Returns:		none
	// Description:	paint the brain to the window client area
	// -------------------------------------------------------------------------
	void On_Paint();

private:

	static bool ourIsRegistered;
	char ourClassName[MAXRESOURCESTRING];

	std::string myFilename;
	int myOnlineId;	// UNID in game

	Brain *myBrain;
	float myChemicals[256];
	Genome *myGenome;
	SVCaptions *mySVCaptions;

	int myFinalGeneAge;
	
	HMENU myHOnlineMainMenu;
	HMENU myHOfflineMainMenu;
	HMENU myHOfflineReconnectMainMenu;
	HMENU myHViewMenu;
	HMENU myHPopupMenus;

	// printing vars
	int myScalex1_4, myScaley1_4, myScalex1_2, myScaley1_2, myScalex3_4, myScaley3_4;
	int myNeuronViewVar;			// variable to dislay on viewport
	int myDendriteViewVar;			// variable to dislay on viewport
	ThreshHoldStyle myViewThreshHoldStyle;
	float myViewThreshHold;

	HPEN **myLobePens;
	HPEN myHighlightPens[totalColours];
	HBRUSH **myLobeBrushes;
	HBRUSH myHighlightBrushes[totalColours];
	HFONT myFont;


	// pathing variables
	MouseHover myPopupHover;
	MouseHover myMouseHover;
	Path *myTractPath;
	Path *myLobePath;
	Path **myNeuronPath;
	Path **myDendritePath;
	DendriteConnectionLog **myMigrationLog;	// used for finding old paths of migrated dens 



	//  dialogs manager
	BrainDlgContainer *myBrainDlgContainer;

	// names nameager
	LobeNames *myLobeNames;

	// brain tips

	Tips *myTips;

	// mouse moving vars
	int myMoveLobeStartX, myMoveLobeStartY;
	int myMoveMouseStartX, myMoveMouseStartY;
	bool myMouseMoveLobe, myTrackMouseLeave, myLeftButtonDown;

	// -------------------------------------------------------------------------
	// Method:		CreateBrainSupport
	// Arguments:	character string
	// Returns:		none
	// Description:	Create captions using genome filename, create inputlobedlgsfor 
	//				the brain also creates brushes and pens for the brain
	// -------------------------------------------------------------------------
	bool CreateBrainSupport(std::string filename);

	// -------------------------------------------------------------------------
	// Method:		DeleteBrainSupport
	// Arguments:	none
	// Returns:		none
	// Description:	Deletes all attached windows, pens and brushes
	// -------------------------------------------------------------------------
	void DeleteBrainSupport();

	// -------------------------------------------------------------------------
	// Method:		CreateLobeInputDlgs
	// Arguments:	none
	// Returns:		none
	// Description:	Creates the lobe input dialog windows for the brain.
	// -------------------------------------------------------------------------
	void CreateLobeInputDlgs();
	
	// -------------------------------------------------------------------------
	// Method:		SetNeuronViewVar
	// Arguments:	variable number
	// Returns:		none
	// Description:	Sets the neuron variable being displayed in the viewport
	// -------------------------------------------------------------------------	
	void SetNeuronViewVar(int var){myNeuronViewVar = var % SVRule::noOfVariables;};

	// -------------------------------------------------------------------------
	// Method:		SetDendriteViewVar
	// Arguments:	nvariable number
	// Returns:		none
	// Description:	Sets the dendrite variable being viewed in the viewport
	// -------------------------------------------------------------------------
	void SetDendriteViewVar(const int var){myDendriteViewVar = var % SVRule::noOfVariables;};

	// -------------------------------------------------------------------------
	// Method:		On_Command
	// Arguments:	standard On_Command args
	// Returns:		none
	// Description:	object specific command handler
	// -------------------------------------------------------------------------
	void On_Command(HWND hwnd, int id, HWND hwndCtl, UINT NotifyCode);
	
	// -------------------------------------------------------------------------
	// Method:		On_LButtonDblCLk
	// Arguments:	standard double click message cracker paras
	// Returns:		none
	// Description:	in on an object - brings up edit dialog
	// -------------------------------------------------------------------------
	void On_LButtonDblClk(HWND hwnd, BOOL DoubleClick, int x, int y, UINT keyFlags);
	
	// -------------------------------------------------------------------------
	// Method:		On_LButtonDown
	// Arguments:	standard left button down message cracker paras
	// Returns:		none
	// Description:	in on an a lobe - allows moving
	//				if on a lobe or tract - paths whole lobe/tract
	// -------------------------------------------------------------------------
	void On_LButtonDown(HWND hwnd, BOOL DoubleClick, int x, int y, UINT keyFlags);

	// -------------------------------------------------------------------------
	// Method:		On_LButtonUp
	// Arguments:	standard left button up message cracker paras
	// Returns:		none
	// Description:	in on an a lobe - stop allowing moving
	//				if on a lobe or tract - unpaths paths whole lobe/tract
	// -------------------------------------------------------------------------
	void On_LButtonUp(HWND hwnd, int x, int y, UINT keyFlags);

	// -------------------------------------------------------------------------
	// Method:		On_RButtonDown
	// Arguments:	standard left button down message cracker paras
	// Returns:		none
	// Description:	brings up popups
	// -------------------------------------------------------------------------
	void BrainViewport::On_RButtonDown(HWND hwnd, BOOL DoubleClick, int x, int y, UINT keyFlags);

	// -------------------------------------------------------------------------
	// Method:		On_MouseMove
	// Arguments:	standard mouse move message cracker paras
	// Returns:		none
	// Description:	if moving lobe - recalcs position and reprints brain
	//				highlights and paths object currently on by calling 
	//				whatismouseon(). Sets up to track if mouse is leaving
	//				make sure have correct pointer
	// -------------------------------------------------------------------------
	void On_MouseMove(HWND hwnd, int x, int y, UINT keyflags);

	// -------------------------------------------------------------------------
	// Method:		On_MouseLeave
	// Arguments:	none
	// Returns:		none
	// Description:	if moving lobe - recalcs position and reprints brain
	//				highlights and paths object currently on by calling 
	//				whatismouseon(). Changes pointer
	// -------------------------------------------------------------------------
	void On_MouseLeave();

	// -------------------------------------------------------------------------
	// Method:		PrintLobeNeurons
	// Arguments:	lobe to print, print pathed only, array of pens and brushes 
	//				to use, device context to print to
	// Returns:		none
	// Description:	Prints all neurons in a lobe useing a pen whose brightness
	//				reflects the strength of the myNeuronViewVar being printed
	// -------------------------------------------------------------------------
	void PrintLobeNeurons(Lobe &lobe,  bool pathedOnly, HPEN *hPens, HBRUSH *hBrushes, HDC hdc);

	// -------------------------------------------------------------------------
	// Method:		PrintDendriteTracts
	// Arguments:	tract to print, print only pathed, array of pens to use, device 
	//				context to print to
	// Returns:		none
	// Description:	Prints all dendrites in a tract using a pen whose brightness
	//				reflects the strength of the myDendriteViewVar being printed
	// -------------------------------------------------------------------------
	void PrintTractDendrites(Tract &tract, bool pathedOnly, HPEN *hPens, HDC hdc);

	// -------------------------------------------------------------------------
	// Method:		EmptyRectangle
	// Arguments:	device contect, coords of rectangle
	// Returns:		none
	// Description:	Prints an unfilled rectangle, used for printing lobes
	// -------------------------------------------------------------------------
	void EmptyRectangle(HDC hdc, float x1, float y1, float x2, float y2);

	// -------------------------------------------------------------------------
	// Method:		PrintTract
	// Arguments:	tract to print, pens to use, device contect to print to
	// Returns:		none
	// Description:	Prints a tract in pen 0
	// -------------------------------------------------------------------------
	inline void PrintTract(Tract &tract, HPEN *hPens, HDC hdc);

	// -------------------------------------------------------------------------
	// Method:		PrintDendrite
	// Arguments:	tract dendrite is in, dendrite to print,
	//				pens to use, device contect to print to
	// Returns:		none
	// Description:	Prints a dendrite in a pen whose brightness reflects the
	//				strength of its myDendriteViewVar
	// -------------------------------------------------------------------------
	inline void PrintDendrite(Tract &tract, Dendrite &dendrite, HPEN *hPens, HDC hdc);

	// -------------------------------------------------------------------------
	// Method:		PrintNeuron
	// Arguments:	device context to print to, X & Y coords of lobe neuron is in
	//				lobe neuron is in,, (n)neuron number, pens and brushes to 
	//				use
	// Returns:		none
	// Description:	Prints the neuron using pens and brushes whose brightness 
	//				reflects the strength of its myNeuronViewVar
	// -------------------------------------------------------------------------
	inline void PrintNeuron(HDC hdc, int lobeX, int lobeY, Lobe &lobe, int n, HPEN *hPens, HBRUSH *hBrushs);
	
	// -------------------------------------------------------------------------
	// Method:		WhatIsMouseOver
	// Arguments:	coords of mouse
	// Returns:		none
	// Description:	finds out what object the mouse is on.
	//				then calls mappath to map the path from that object
	// -------------------------------------------------------------------------
	void WhatIsMouseOver(POINT mousePos);

	// -------------------------------------------------------------------------
	// Method:		MouseOverLobeOrNeuron
	// Arguments:	coords of mouse, structure to store results
	// Returns:		true if over neuron or lobe
	// Description:	finds out if mouse is over a lobe or a neuron
	// -------------------------------------------------------------------------
	bool MouseOverLobeOrNeuron(POINT mousePos, MouseHover& mouseNowHover);

	// -------------------------------------------------------------------------
	// Method:		MouseOverTractOrDendrite
	// Arguments:	coords of mouse, structure to storeresults in
	// Returns:		true if over tract or dendrite
	// Description:	finds out if mouse is over tract or dendrite
	// -------------------------------------------------------------------------
	bool MouseOverTractOrDendrite(POINT mousePos, MouseHover& mouseNowHover);

	// -------------------------------------------------------------------------
	// Method:		MapPath
	// Arguments:	type of object mouse is on, position for tool tips
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
	//				Uses the myMouseHover, myOnLobe, myOnTract, myOnDendrite and
	//				myOnNeuron members to tell what the mouse was on last and
	//				changes these memebers if a new path is drawn
	//				Also updates the status bar
	// -------------------------------------------------------------------------
	void MapPath(MouseHover mouseNowOn, POINT *tipsPos);

	// -------------------------------------------------------------------------
	// Method:		UnMapPath
	// Arguments:	none
	// Returns:		none
	// Description:	Unhighlight the path indicated by the myMouseHover, myOnLobe, 
	//				myOnTract, myOnDendrite and myOnNeuron members 
	// -------------------------------------------------------------------------
	void UnMapPath();

	// -------------------------------------------------------------------------
	// Method:		PathLobe
	// Arguments:	lobe to path, whether to path src/dst, whether to use
	//				highlight pen
	// Returns:		none
	// Description:	Prints the lobe in the desired pen, paths source and/or
	//				destinations. If viewing neuron details will print all 
	//				neurons too
	// -------------------------------------------------------------------------
	void PathLobe(Lobe &lobe, bool src, bool dst, bool highlight);
	
	// -------------------------------------------------------------------------
	// Method:		PathNeuron
	// Arguments:	lobe neuron is in, neuron to path whether to path src/dst, 
	//				whether to use	highlight pen
	// Returns:		none
	// Description:	Prints the neuron in the desired pen, paths source and/or
	//				destinations. If NOT viewing neuron details will lobe
	// -------------------------------------------------------------------------
	void PathNeuron(Lobe &lobe, Neuron &neuron, bool src, bool dst, bool highlight);

	// -------------------------------------------------------------------------
	// Method:		PathTract
	// Arguments:	tract to path whether to path src/dst, 
	//				whether to use	highlight pen
	// Returns:		none
	// Description:	Prints the dendrite in the desired pen, paths source and/or
	//				destinations. If viewing dendrite details will all dendrites
	// -------------------------------------------------------------------------
	void PathTract(Tract &tract, bool src, bool dst, bool highlight);

	// -------------------------------------------------------------------------
	// Method:		PathDendrite
	// Arguments:	tract dendrite is in, dendrite to path whether to path src/dst, 
	//				whether to use	highlight pen
	// Returns:		none
	// Description:	Prints the dendrite in the desired pen, paths source and/or
	//				destinations. If NOT viewing dendrite details will print tract
	// -------------------------------------------------------------------------
	void PathDendrite(Tract &tract, Dendrite &dendrite, bool src, bool dst, bool highlight);

	// -------------------------------------------------------------------------
	// Method:		Online
	// Arguments:	coords to test. x1,y1,x2,y2 - line coords
	// Returns:		true or false
	// Description:Tests if x,y are on line x1,y1,x2,y2
	// -------------------------------------------------------------------------
	inline bool Online(POINT pos, int x1, int y1, int x2, int y2);

	// -------------------------------------------------------------------------
	// Method:		CheckNeuronMenu
	// Arguments:	neuron variable to view, menu id to tick
	// Returns:		none
	// Description:	unchecks all the neuron variable items in the view menu
	// -------------------------------------------------------------------------
	void CheckNeuronMenu(int neuronViewVar, int neuronMenuID);

	// -------------------------------------------------------------------------
	// Method:		CheckDendriteMenu
	// Arguments:	dendrite variable to view, menu id to tick
	// Returns:		none
	// Description:	unchecks all the dendrite variable items in the view menu
	// -------------------------------------------------------------------------
	void CheckDendriteMenu(int dendriteViewVar, int dendriteMenuID);

	// -------------------------------------------------------------------------
	// Method:		OnlineUpdateLobe
	// Arguments:	lobe number
	// Returns:		sucess or failure
	// Description:	down loads lobe from game
	// -------------------------------------------------------------------------
	bool OnlineUpdateLobe(int lobe);

	// -------------------------------------------------------------------------
	// Method:		OnlineUpdateTract
	// Arguments:	tract number
	// Returns:		sucess or failure
	// Description:	down loads tract from game
	// -------------------------------------------------------------------------
	bool OnlineUpdateTract(int tract);

	// -------------------------------------------------------------------------
	// Method:		OnlineUpdateNeuron
	// Arguments:	lobe, neuron
	// Returns:		sucess or failure
	// Description:	down loads neuron from game
	// -------------------------------------------------------------------------
	bool OnlineUpdateNeuron(Lobe &lobe, Neuron &neuron);

	// -------------------------------------------------------------------------
	// Method:		OnlineUpdateDendrite
	// Arguments:	tract, dendrite
	// Returns:		sucess or failure
	// Description:	down loads dendrite from game
	// -------------------------------------------------------------------------
	bool OnlineUpdateDendrite(Tract &tract, Dendrite &dendrite);

	// -------------------------------------------------------------------------
	// Method:		CheckMigrations
	// Arguments:	tract number, dendrite number
	// Returns:		none
	// Description:	checks to see if a dendrite has migrated
	// -------------------------------------------------------------------------
	inline void CheckMigrations(int t, int d);

	// -------------------------------------------------------------------------
	// Method:		ReadIntDesc
	// Arguments:	pointer to pointer to integer description
	// Returns:		integer
	// Description:	convert character integer to int and moves pointer past it
	// -------------------------------------------------------------------------
	int ReadIntDesc(char **description)
	{
		char *str = *description;
		(*description) += strlen(str)+1;

		return atoi(str);
	}
};

///
///
///
///	INLINE FUNCTIONS FOLLOW
///
///
///



// -------------------------------------------------------------------------
// Method:		PrintTract
// Arguments:	tract to print, pens to use, device contect to print to
// Returns:		none
// Description:	Prints a tract in pen 0
// -------------------------------------------------------------------------
void BrainViewport::PrintTract(Tract &tract, HPEN *hPens, HDC hdc)
{	
	SelectPen(hdc, hPens[0]);

	// find dest pos
	Lobe &dstLobe = GetDstLobe(tract);
	float x1 = BitMapXCoord(GetLobeX(dstLobe))+(Zoom(ScaleX(GetLobeWidth(dstLobe)))/2);
	float y1 = BitMapYCoord(GetLobeY(dstLobe))+(Zoom(ScaleY(GetLobeHeight(dstLobe)))/2);

	// find src pos
	Lobe &srcLobe = GetSrcLobe(tract);
	float x2 = BitMapXCoord(GetLobeX(dstLobe))+(Zoom(ScaleX(GetLobeWidth(dstLobe)))/2);
	float y2 = BitMapYCoord(GetLobeY(dstLobe))+(Zoom(ScaleY(GetLobeHeight(dstLobe)))/2);
	ClipLine(x1, y1, x2, y2);
	MoveToEx(hdc, (int)x1, (int)y1, NULL);
	LineTo(hdc, (int)x2, (int)y2);

}


// -------------------------------------------------------------------------
// Method:		PrintDendrite
// Arguments:	tract dendrite is in, dendrite to print,
//				pens to use, device contect to print to
// Returns:		none
// Description:	Prints a dendrite in a pen whose brightness reflects the
//				strength of its myDendriteViewVar
// -------------------------------------------------------------------------
void BrainViewport::PrintDendrite(Tract &tract, Dendrite &dendrite, HPEN *hPens, HDC hdc)
{	

	int dn, sn;
	int lobeX, lobeY, neuronX, neuronY;

	float absDendriteValue = fabs(dendrite.weights[myDendriteViewVar]);
	int colourId = (int)(absDendriteValue * (float)(totalColours-1));
	SelectPen(hdc, hPens[colourId]);

		
	// find dest pos
	Lobe &dstLobe = GetDstLobe(tract);
	dn = NeuronIdInList(*dendrite.dstNeuron);
	lobeX = GetLobeX(dstLobe);
	lobeY = GetLobeY(dstLobe);
	neuronX = dn%GetLobeWidth(dstLobe);
	neuronY = dn/GetLobeWidth(dstLobe);
	float x1 = BitMapXCoord(lobeX+neuronX)+myScalex1_2;
	float y1 = BitMapYCoord(lobeY+neuronY)+myScaley1_2;

	// find src pos
	Lobe &srcLobe = GetSrcLobe(tract);
	sn = NeuronIdInList(*dendrite.srcNeuron);
	lobeX = GetLobeX(srcLobe);
	lobeY = GetLobeY(srcLobe);
	neuronX = sn%GetLobeWidth(srcLobe);
	neuronY = sn/GetLobeWidth(srcLobe);
	float x2 = BitMapXCoord(lobeX+neuronX)+myScalex1_2;
	float y2 = BitMapYCoord(lobeY+neuronY)+myScaley1_2;
	ClipLine(x1, y1, x2, y2);
	MoveToEx(hdc, (int)x1, (int)y1, NULL);
	LineTo(hdc, (int)x2, (int)y2);
}

// -------------------------------------------------------------------------
// Method:		PrintNeuron
// Arguments:	device context to print to, X & Y coords of lobe neuron is in
//				lobe neuron is in,, (n)neuron number, pens and brushes to 
//				use
// Returns:		none
// Description:	Prints the neuron using pens and brushes whose brightness 
//				reflects the strength of its myNeuronViewVar
// -------------------------------------------------------------------------
void BrainViewport::PrintNeuron(HDC hdc, int lobeX, int lobeY, Lobe &lobe, int n, HPEN *hPens, HBRUSH *hBrushes)
{
	HBRUSH oldBrush;
	int neuronX, neuronY;

	neuronX = lobeX+(n%GetLobeWidth(lobe));
	neuronY = lobeY+(n/GetLobeWidth(lobe));

	float absNeuronValue = fabs(GetNeuron(lobe, n).states[myNeuronViewVar]);
	int colourId = (int)(absNeuronValue * (float)(totalColours-1));
	oldBrush = SelectBrush(hdc, hBrushes[colourId]); 
	SelectPen(hdc, hPens[colourId]);
	float x1 = BitMapXCoord(neuronX)+myScalex1_4;
	float y1 = BitMapYCoord(neuronY)+myScaley1_4;
	float x2 = BitMapXCoord(neuronX)+myScalex3_4;
	float y2 = BitMapYCoord(neuronY)+myScaley3_4;		
	ClipRectangle(x1, y1, x2, y2);
	Rectangle(hdc, x1, y1, x2, y2);		

	SelectBrush(hdc, oldBrush);
}

// -------------------------------------------------------------------------
// Method:		Online
// Arguments:	coords to test. x1,y1,x2,y2 - line coords
// Returns:		true or false
// Description:Tests if x,y are on line x1,y1,x2,y2
// -------------------------------------------------------------------------
#define TOLERANCE_BV 2
bool BrainViewport::Online(POINT pos, int x1, int y1, int x2, int y2)
{
	int yy, yyy, xx, xxx;

	int xx1, xx2, yy1, yy2;

	if(x1 < x2)
	{
		xx1 = x1;
		xx2 = x2;
	}
	else
	{
		xx1 = x2;
		xx2 = x1;
	}

	if(y1 < y2)
	{
		yy1 = y1;
		yy2 = y2;
	}
	else
	{
		yy1 = y2;
		yy2 = y1;
	}
	if(pos.x < xx1 || pos.x> xx2 || pos.y < yy1 || pos.y > yy2)
		return false;

	xx = pos.x - x2;
	yy = pos.y - y2;
	xxx = x1 - x2;
	yyy = y1 -y2;

	if(abs(xxx) <= TOLERANCE_BV)
	{
		if(abs(xx) <= TOLERANCE_BV) return true;
	}
	else if(abs(yyy) <= TOLERANCE_BV)
	{
		if(abs(yy) <= TOLERANCE_BV) return true;
	}
	else if(abs(yyy) > abs(xxx))
	{
		if (abs(Round(((float)xxx/yyy)*yy)-xx) <= TOLERANCE_BV) return true;
	}
	else if(abs(xxx) > abs(yyy))
	{
		if (abs(Round(((float)yyy/xxx)*xx)-yy) <= TOLERANCE_BV) return true;
	}
	else if(yyy == xxx &&
			xxx == xx &&
			xx == yy)
		return true;

	return false;
}


// -------------------------------------------------------------------------
// Method:		CheckMigrations
// Arguments:	tract number, dendrite number
// Returns:		none
// Description:	checks to see if a dendrite has migrated
// -------------------------------------------------------------------------
void BrainViewport::CheckMigrations(int t, int d)
{
	// check for source migrations
	if(myMigrationLog[t][d].src != GetSrcNeuron(GetTract(*myBrain, t), d))
	{
		if(myDendritePath[t][d].mySrc)
		{
			// unpath old migrated path and repath
			PathDendrite(GetTract(*myBrain, t), GetDendrite(GetTract(*myBrain, t), d), true, false, false);
			myMigrationLog[t][d].src = GetSrcNeuron(GetTract(*myBrain, t), d);
			PathDendrite(GetTract(*myBrain, t), GetDendrite(GetTract(*myBrain, t), d), true, false, true);
		}
		else
			myMigrationLog[t][d].src = GetSrcNeuron(GetTract(*myBrain, t), d);
	}
	
	// check for dst migrations
	if(myMigrationLog[t][d].dst != GetDstNeuron(GetTract(*myBrain, t), d))
	{
		if(myDendritePath[t][d].myDst)
		{
			// unpath old migrated path and repath
			PathDendrite(GetTract(*myBrain, t), GetDendrite(GetTract(*myBrain, t), d), false, true, false);
			myMigrationLog[t][d].dst = GetDstNeuron(GetTract(*myBrain, t), d);
			PathDendrite(GetTract(*myBrain, t), GetDendrite(GetTract(*myBrain, t), d), false, true, true);
		}
		else
			myMigrationLog[t][d].dst = GetDstNeuron(GetTract(*myBrain, t), d);
	}
}

extern BrainViewport* theBrainViewport;



#endif

