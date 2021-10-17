// ----------------------------------------------------------------------------
// Filename:	SVRuleDlg
// Class:		SVRuleDlg
// Purpose:		Dialog for editing SV rules
// Description:	Displays SV rules for a lobe or tract and allows editing of
//				value fields (floats)
// Usage:		May be created by a BrainViewport. Must call initialise instance
//				after the window is created
// -----------------------------------------------------------------------------
#ifndef SVRuleDlg_H
#define SVRuleDlg_H

#include "Resource.h"
#include "Wnd.h"
#include "../../engine/Creature/Brain/BrainAccess.h"
#include "BrainDlg.h"
#include "LobeNames.h"
#include "TextViewport.h"
#include "SVCaptions.h"
#include "../../common/GameInterface.h"
extern CGameInterface theGameInterface;

class BrainDlgContainer;
class Edit;

class SVRuleDlg : public Wnd, public BrainAccess, public BrainDlg
{	
	enum Sign {posative, negative};
	enum {noOfSVs = 2};
	enum {totalNoRules = SVRule::length*noOfSVs};

public:


	virtual void Activate() {SetActiveWindow(myHWnd);};
	
protected:

	SVRuleDlg(HINSTANCE hInstance, HWND hWndOwner, const SVCaptions::Caption * const captions, LobeNames &lobeNames, BrainDlgContainer &container, SVRule &svUpdateRule, SVRule &svInitRule, bool useInit);
	~SVRuleDlg();
	HWND Create();

	LobeNames &myLobeNames;
	SVRule &myInitSVRule;
	SVRule &myUpdateSVRule;
	bool myUseInit;
	const SVCaptions::Caption * const myCaptions;
	HWND myHWndDescription;
	HWND myHWndGeneralNotes;
//	TextViewport *myGeneralNotesWnd;
	Edit *myGeneralNotesWnd;

private:

	static bool ourIsRegistered;
	char ourClassName[MAXRESOURCESTRING];

	BrainDlgContainer &myContainer; 
	HWND myHWndSVTrack[totalNoRules];		// trackbars
	HWND myHWndSVValue[totalNoRules];		// trackbar values
	HWND myHWndSVComment[totalNoRules];	// sv comments
	Sign mySVSign[totalNoRules];			// sign to be used in value window

	HFONT myFont;

	int myNoOfLines;	// no of editable sv lines 

	int mySVRuleEntryNo[totalNoRules];	// maps entry line on dialog to 
											// entry number in actual rule
											// does 2 rule init and update
	
	struct SVRuleEntry mySVRuleEntry[totalNoRules];	//editable sv entries

	int mySVLineHeight;
	int mySVDlgSpacer;
	int mySVLineSpacer;
	int mySVDescHeight;
	int myGeneralNoteHeight;
	int mySVTrackbarWidth;
	int mySVCommentWidth;
	int mySVValueWidth;
	int mySVWidth;
	int mySVTop;



	virtual LRESULT WndProc(HWND hwnd, UINT msg, UINT wParam, LONG lParam);
	void CreateLine(const int i, const struct SVRuleEntry& sVRuleEntry, const std::string comment);
	void On_Scroll(HWND hwnd);
	virtual void SetOnlineValue(int line, float value) = 0;
	inline SVRule& GetInitOrUpdateRule(int l) { return (l  < SVRule::length ? myInitSVRule : myUpdateSVRule);};
	inline int GetInitOrUpdateLine(int l) { return (l < SVRule::length ? l : l-SVRule::length);};
	int GetSVCaptionNo(int l) {return (l/SVRule::length);};


};


class LobeDlg : public SVRuleDlg
{	
public:
	LobeDlg(HINSTANCE hInstance, HWND hWndOwner, Lobe &lobe, const SVCaptions::Caption * const caption, LobeNames &lobeNames, BrainDlgContainer &container) : 
	  SVRuleDlg(hInstance, hWndOwner, caption, lobeNames, container, GetUpdateSVRule(lobe), GetInitSVRule(lobe), GetInitRunsAlways(lobe)),
		myLobe(lobe){};
	HWND Create();
	const void *LinkedTo(){return (void *)&myLobe;};

private:
	void SetOnlineValue(int line, float value);
	Lobe &myLobe;
};


class TractDlg : public SVRuleDlg
{	
public:

	TractDlg(HINSTANCE hInstance, HWND hWndOwner, Tract &tract, const SVCaptions::Caption * const captions, LobeNames &lobeNames, BrainDlgContainer &container) : 
		SVRuleDlg(hInstance, hWndOwner, captions, lobeNames, container, GetUpdateSVRule(tract), GetInitSVRule(tract), GetInitRunsAlways(tract)), 
		myTract(tract){};
	HWND Create();
	const void *LinkedTo(){return (void *)&myTract;};
private:

	void SetOnlineValue(int line, float value);
	Tract &myTract;
};


#endif

