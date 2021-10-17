// ScriptoriumChildFrame.cpp : implementation file
//

#include "stdafx.h"
#include "Caos.h"
#include "../../Common/WindowState.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// The main MDI application window
extern CMainFrame *theFrame;
extern CGameInterface theGame;

#include "windows.h"
WINDOWPLACEMENT swp;

/////////////////////////////////////////////////////////////////////////////
// CScriptoriumChildFrame

IMPLEMENT_DYNCREATE(CScriptoriumChildFrame, CMDIChildWnd)

// Constructor
CScriptoriumChildFrame::CScriptoriumChildFrame()
{
	m_ScriptCount = 0;
}

// Destructor
CScriptoriumChildFrame::~CScriptoriumChildFrame()
{
}


// This is a static function
int CALLBACK CScriptoriumChildFrame::CompareNodes
	(LPARAM _Node1, LPARAM _Node2, LPARAM _ChildIndex)
{
	int ChildIndex = _ChildIndex;
	NodeStructure *Node1;
	NodeStructure *Node2;
	int ID1, ID2;
	
	Node1 = (NodeStructure *)_Node1;
	ID1 = Node1->FGSEValues[ChildIndex];

	Node2 = (NodeStructure *)_Node2;
	ID2 = Node2->FGSEValues[ChildIndex];

	if (ID2 > ID1)
		return -1;
	else
		return 1;
}





HTREEITEM CScriptoriumChildFrame::AddChildNodeToParentNode
	(HTREEITEM hParentNode, int FGSEValues[4], int ChildIndex)
{
	std::vector< std::string >& scriptNames = CMacroParse::theCAOSDescription.GetScriptNames();

	CString Text;
	CString EventName;
	int ID;
	HTREEITEM hChildNode;
	NodeStructure *Node;
	int i;
	DWORD dw;

	// ChildIndex is 0 for family, 1 for genus, 2 for species and 3 
	// for event
	// Get the ID of the family, genus, species or event
	ID = FGSEValues[ChildIndex];
	// Get the text for the node label
	if (((ChildIndex == 1) || (ChildIndex == 2)) && (ID == 0))
		Text = "0 (Generic)";
	else if (ChildIndex == 3) {
		if (ID < 256) {
			EventName = scriptNames[ID].c_str();
			if (EventName.IsEmpty())
				Text.Format("%d", ID);
			else
				Text.Format("%d (%s)", ID, EventName);
		}
		else 
			Text.Format("%d", ID);
	}
	else {	
		Text.Format("%d", ID);
	}

	// Insert a new child node
	hChildNode = m_ctrlTree.InsertItem(Text, ChildIndex+1, 
		ChildIndex+1, hParentNode);

	// Store the level and the family, genus, species, event IDs 
	// with the node
	Node = new NodeStructure;
	Node->Level = ChildIndex+1;
	for (i=0; i<=ChildIndex; i++) 
		Node->FGSEValues[i] = FGSEValues[i];
	dw = (DWORD)Node;
	m_ctrlTree.SetItemData(hChildNode, dw);

	return hChildNode;
}



HTREEITEM CScriptoriumChildFrame::AddChildNodeInSortedOrder
	(HTREEITEM hParentNode, int FGSEValues[4], int ChildIndex)
{
	DWORD dw;
	NodeStructure *Node;
	BOOL AlreadyThere;
	HTREEITEM hChildNode, hNextChildNode;
	TV_SORTCB Sort;

	AlreadyThere = FALSE;
	// Is the child node ID already there?
	if (m_ctrlTree.ItemHasChildren(hParentNode)) {
		hChildNode = m_ctrlTree.GetChildItem(hParentNode);
		do {
			hNextChildNode = m_ctrlTree.GetNextSiblingItem(hChildNode);
			dw = m_ctrlTree.GetItemData(hChildNode);
			Node = (NodeStructure *)dw;
			if (Node->FGSEValues[ChildIndex] == FGSEValues[ChildIndex]) {
				AlreadyThere = TRUE;
				break;
			}
			hChildNode = hNextChildNode;
		} while (hChildNode != NULL);	
	}
	if (AlreadyThere)
		return hChildNode;

	// Insert the new child node
	hChildNode = AddChildNodeToParentNode(hParentNode, FGSEValues, 
		ChildIndex);

	// Sort the child nodes into numerical order
	Sort.hParent = hParentNode;
	Sort.lpfnCompare = CompareNodes;
	Sort.lParam = ChildIndex;
	m_ctrlTree.SortChildrenCB(&Sort);

	return hChildNode;
}




HTREEITEM CScriptoriumChildFrame::FindChildNode
	(HTREEITEM hParentNode, int FGSEValues[4], int ChildIndex)
{
	DWORD dw;
	NodeStructure *Node;
	BOOL Found;
	HTREEITEM hChildNode, hNextChildNode;

	Found = FALSE;
	if (m_ctrlTree.ItemHasChildren(hParentNode)) {
		hChildNode = m_ctrlTree.GetChildItem(hParentNode);
		do {
			hNextChildNode = m_ctrlTree.GetNextSiblingItem(hChildNode);
			dw = m_ctrlTree.GetItemData(hChildNode);
			Node = (NodeStructure *)dw;
			if (Node->FGSEValues[ChildIndex] == FGSEValues[ChildIndex]) {
				Found = TRUE;
				break;
			}
			hChildNode = hNextChildNode;
		} while (hChildNode != NULL);	
	}
	if (Found)
		return hChildNode;
	else
		return NULL;
}




void CScriptoriumChildFrame::LoadTreeHelper
	(HTREEITEM hParentNode, int FGSEValues[4], int ChildIndex)
{
	CString IDList;
	CString IDListCopy;
	char *IDListBuffer;
	CString Command;
	char *p;
	char *temp;
	int ID;
	HTREEITEM hChildNode;

	// Work out the command to get the child IDs
	switch (ChildIndex) {
	case 0:
		Command = "execute\ngids root";
		break;
	case 1:
		Command.Format("execute\ngids fmly %d", FGSEValues[0]);
		break;
	case 2:
		Command.Format("execute\ngids gnus %d %d", FGSEValues[0], 
			FGSEValues[1]);
		break;
	case 3:
		Command.Format("execute\ngids spcs %d %d %d", FGSEValues[0], 
			FGSEValues[1], FGSEValues[2]);
		break;
	}

	std::string reply;
	theGame.Inject((LPCTSTR)Command, reply, false);
	IDList = reply.c_str();
	IDListCopy = IDList;
	IDListBuffer = IDListCopy.GetBuffer(1);
	
	// Add each child node to the parent node
	p = IDListBuffer;
	while (*p != 0) {
		// Extract the next ID
		ID = strtol(p, &temp, 10);
		// Store the ID
		FGSEValues[ChildIndex] = ID;
		hChildNode = AddChildNodeToParentNode(hParentNode, FGSEValues, 
			ChildIndex);
		if (ChildIndex < 3)
			// Recursively add all descandants
			LoadTreeHelper(hChildNode, FGSEValues, ChildIndex+1);
		p = temp;
		// Skip past the following space character
		p++;
	}
	// Clean up
	IDListCopy.ReleaseBuffer();
}


void CScriptoriumChildFrame::UnloadTreeHelper(HTREEITEM hParentNode) 
{
	DWORD dw;
	HTREEITEM hChildNode, hNextChildNode;
	NodeStructure *Node;

	// Does this node have children?
	if (m_ctrlTree.ItemHasChildren(hParentNode)) {
		hChildNode = m_ctrlTree.GetChildItem(hParentNode);
		do {
			hNextChildNode = m_ctrlTree.GetNextSiblingItem(hChildNode);
			// Recursively destroy descandants
			UnloadTreeHelper(hChildNode);
			hChildNode = hNextChildNode;
		} while (hChildNode != NULL);
	}
	// Clean up each node's memory 
	dw = m_ctrlTree.GetItemData(hParentNode);
	Node = (NodeStructure *)dw;
	delete Node;
	m_ctrlTree.DeleteItem(hParentNode);
}




void CScriptoriumChildFrame::ExpandCollapseHelper
	(HTREEITEM hParentNode, int Operation)
{
	HTREEITEM hChildNode, hNextChildNode;

	m_ctrlTree.Expand(hParentNode, Operation);
	// Does this item have children
	if (m_ctrlTree.ItemHasChildren(hParentNode)) {
		hChildNode = m_ctrlTree.GetChildItem(hParentNode);
		do {
			hNextChildNode = m_ctrlTree.GetNextSiblingItem(hChildNode);
			// Recurse through descandants
			ExpandCollapseHelper(hChildNode, Operation);
			hChildNode = hNextChildNode;
		} while (hChildNode != NULL);
	}
}



void CScriptoriumChildFrame::DeleteScriptsHelper(HTREEITEM hParentNode) 
{
	DWORD dw;
	HTREEITEM hChildNode, hNextChildNode;
	NodeStructure *Node;
	CString Command;

	// Does this item have children?
	if (m_ctrlTree.ItemHasChildren(hParentNode)) {
		hChildNode = m_ctrlTree.GetChildItem(hParentNode);
		do {
			hNextChildNode = m_ctrlTree.GetNextSiblingItem(hChildNode);
			// Recurse through descandants
			DeleteScriptsHelper(hChildNode);
			hChildNode = hNextChildNode;
		} while (hChildNode != NULL);
	}
	// Clean up memory
	dw = m_ctrlTree.GetItemData(hParentNode);
	Node = (NodeStructure *)dw;
	if (Node->Level == 4) {
		// Delete the script from the Scriptorium
		Command.Format("execute\nscrx %d %d %d %d", 
			Node->FGSEValues[0], Node->FGSEValues[1], 
			Node->FGSEValues[2], Node->FGSEValues[3]);
		std::string dummy;
		theGame.Inject((LPCTSTR)Command, dummy, false);
	}
	delete Node;
	m_ctrlTree.DeleteItem(hParentNode);
}



void CScriptoriumChildFrame::GetScriptsHelper(HTREEITEM hParentNode)
{
	DWORD dw;
	HTREEITEM hChildNode, hNextChildNode;
	NodeStructure *Node;
	CString Command;
	std::string Script;
	CString ScriptID;

	// Does this item have children
	if (m_ctrlTree.ItemHasChildren(hParentNode)) {
		hChildNode = m_ctrlTree.GetChildItem(hParentNode);
		do {
			hNextChildNode = m_ctrlTree.GetNextSiblingItem(hChildNode);
			// Recurse through descendants
			GetScriptsHelper(hChildNode);
			hChildNode = hNextChildNode;
		} while (hChildNode != NULL);
	}
	
	dw = m_ctrlTree.GetItemData(hParentNode);
	Node = (NodeStructure *)dw;
	if (Node->Level == 4) {
		ScriptID.Format("%d %d %d %d", 
			Node->FGSEValues[0], Node->FGSEValues[1], 
			Node->FGSEValues[2], Node->FGSEValues[3]);
		Command.Format("execute\nouts sorc %s", ScriptID);
		theGame.Inject((LPCTSTR)Command, Script, false);
		if (m_ScriptCount >= m_ScriptListSize) {
			m_ScriptListSize *= 2;
			m_ScriptList.SetSize(m_ScriptListSize);
			m_ScriptNodeList.SetSize(m_ScriptListSize);
		}
		m_ScriptList[m_ScriptCount].Format("scrp %s %s endm", ScriptID, 
			Script.c_str());
		m_ScriptNodeList[m_ScriptCount] = Node;
		m_ScriptCount++;
	}
}



void CScriptoriumChildFrame::RemoveFGSE(int FGSEValues[4])
{
	HTREEITEM hNode, hParentNode, hNewParentNode;
	DWORD dw;
	NodeStructure *Node;
	int Level;

	hNode = m_ctrlTree.GetRootItem();
	hNode = FindChildNode(hNode, FGSEValues, 0);	
	if (hNode == NULL)
		return;
	hNode = FindChildNode(hNode, FGSEValues, 1);
	if (hNode == NULL)
		return;
	hNode = FindChildNode(hNode, FGSEValues, 2);
	if (hNode == NULL)
		return;
	hNode = FindChildNode(hNode, FGSEValues, 3);
	if (hNode == NULL)
		return;
	hParentNode = m_ctrlTree.GetParentItem(hNode);
	dw = m_ctrlTree.GetItemData(hNode);
	Node = (NodeStructure *)dw;
	delete Node;
	m_ctrlTree.DeleteItem(hNode);

	// Make sure that there are no childless anscestors
	Level = 3;
	do {
		hNewParentNode = m_ctrlTree.GetParentItem(hParentNode);
		if (m_ctrlTree.ItemHasChildren(hParentNode))
			break;
		dw = m_ctrlTree.GetItemData(hParentNode);
		Node = (NodeStructure *)dw;
		delete Node;
		m_ctrlTree.DeleteItem(hParentNode);
		hParentNode = hNewParentNode;
		Level--;
	} while (Level > 0); // don't remove top level node ever
}


void CScriptoriumChildFrame::AddFGSE(int FGSEValues[4])
{
	HTREEITEM hNode;

	hNode = m_ctrlTree.GetRootItem();
	hNode = AddChildNodeInSortedOrder(hNode, FGSEValues, 0);
	hNode = AddChildNodeInSortedOrder(hNode, FGSEValues, 1);
	hNode = AddChildNodeInSortedOrder(hNode, FGSEValues, 2);
	hNode = AddChildNodeInSortedOrder(hNode, FGSEValues, 3);
}





void CScriptoriumChildFrame::LoadTree() 
{
	DWORD dw;
	NodeStructure *Node;
	int FGSEValues[4];
	HTREEITEM hNode;

	hNode = m_ctrlTree.InsertItem("Scriptorium", 0, 0, TVI_ROOT);
	Node = new NodeStructure;
	Node->Level = 0;
	dw = (DWORD)Node;
	m_ctrlTree.SetItemData(hNode, dw);
	LoadTreeHelper(hNode, FGSEValues, 0);	
	hNode = m_ctrlTree.GetRootItem();
	m_ctrlTree.Select(hNode, TVGN_CARET);
	m_ctrlTree.SetFocus();
}




void CScriptoriumChildFrame::UnloadTree() 
{
	HTREEITEM hRootNode;
	
	hRootNode = m_ctrlTree.GetRootItem();
	UnloadTreeHelper(hRootNode);
}



void CScriptoriumChildFrame::Refresh() 
{
	BOOL OK;
	CWaitCursor WaitCursor;

	OK = theGame.Connect();
	if (!OK) {
		::Beep(450, 100);
		::AfxMessageBox("Unable to establish a connection with the game", 
			MB_OK | MB_ICONSTOP);
		return;
	}
	WaitCursor.Restore();
	UnloadTree();
	LoadTree();
	theGame.Disconnect();
}



void CScriptoriumChildFrame::Expand()
{
	HTREEITEM hNode;
	CWaitCursor WaitCursor;

	hNode = m_ctrlTree.GetSelectedItem();
	ExpandCollapseHelper(hNode, TVE_EXPAND);
}



void CScriptoriumChildFrame::Collapse()
{
	HTREEITEM hNode;
	CWaitCursor WaitCursor;

	hNode = m_ctrlTree.GetSelectedItem();
	ExpandCollapseHelper(hNode, TVE_COLLAPSE);
}




void CScriptoriumChildFrame::DeleteScripts()
{
	HTREEITEM hNode, hParentNode, hNewParentNode;
	DWORD dw;
	NodeStructure *Node;
	int Result;
	CString Message;
	int Level;
	BOOL OK;

	hNode = m_ctrlTree.GetSelectedItem();
	dw = m_ctrlTree.GetItemData(hNode);
	Node = (NodeStructure *)dw;

	if (Node->Level == 0) {
		::Beep(450, 100);
		::AfxMessageBox("You cannot delete the entire Scriptorium",
			MB_OK | MB_ICONEXCLAMATION);
		m_ctrlTree.SetFocus();
		return;
	}

	if (Node->Level <= 2) {
		::Beep(450, 100);
		Result = ::AfxMessageBox("This operation may result in losing many scripts. Do you wish to continue?", MB_YESNO | MB_ICONEXCLAMATION);
		m_ctrlTree.SetFocus();
		if (Result == IDNO)
			return;
	}

	switch (Node->Level) {
	case 1:
		Message.Format("Are you sure you want to delete all scripts belonging to family %d?", 
			Node->FGSEValues[0]);		
		break;
	case 2:
		Message.Format("Are you sure you want to delete all scripts belonging to genus [%d, %d]?",
			Node->FGSEValues[0], Node->FGSEValues[1]);		
		break;
	case 3:
		Message.Format("Are you sure you want to delete all scripts belonging to species [%d, %d, %d]?",
			Node->FGSEValues[0], Node->FGSEValues[1], Node->FGSEValues[2]);		
		break;
	case 4:
		Message.Format("Are you sure you want to delete script [%d, %d, %d, %d]?",
			Node->FGSEValues[0], Node->FGSEValues[1], Node->FGSEValues[2],
			Node->FGSEValues[3]);		
		break;
	}

	::Beep(450, 100);
	Result = ::AfxMessageBox(Message, MB_YESNO | MB_ICONEXCLAMATION);
	m_ctrlTree.SetFocus();

	if (Result == IDNO)
		return;

	CWaitCursor WaitCursor;

	OK = theGame.Connect();
	if (!OK) {
		::Beep(450, 100);
		::AfxMessageBox("Unable to establish a connection with the game", 
			MB_OK | MB_ICONSTOP);
		m_ctrlTree.SetFocus();
		return;
	}

	WaitCursor.Restore();

	Level = Node->Level-1;
	hParentNode = m_ctrlTree.GetParentItem(hNode);
	DeleteScriptsHelper(hNode);
	
	// Make sure that there are no childless anscestors
	while (Level > 0) {
		hNewParentNode = m_ctrlTree.GetParentItem(hParentNode);
		if (m_ctrlTree.ItemHasChildren(hParentNode))
			break;
		dw = m_ctrlTree.GetItemData(hParentNode);
		Node = (NodeStructure *)dw;
		delete Node;
		m_ctrlTree.DeleteItem(hParentNode);
		hParentNode = hNewParentNode;
		Level--;
	}

	theGame.Disconnect();
}



void CScriptoriumChildFrame::WriteScriptorium(CString Filename)
{
	CStdioFile f;
	int i, j, k, Tab;
	int StartIndex, EndIndex;
	CommandStructure *Command;
	TokenStructure *Token;
	CString Contents;
	char *Script;
	CPtrArray TokenList;
	int TokenCount;
	CPtrArray CommandList;
	int CommandCount;
	CUIntArray ErrorIndexList;
	int ErrorCount;
	HTREEITEM hRootNode;
	CStringArray ScriptList;
	CPtrArray ScriptNodeList;
	int ScriptCount;
	static char Buffer[20000];
	char *ptr;
	CMacroParse Parser;   
	BOOL OK;
	CString Message;
	CWaitCursor WaitCursor;

	OK = theGame.Connect();
	if (!OK) {
		::Beep(450, 100);
		::AfxMessageBox("Unable to establish a connection with the game", 
			MB_OK | MB_ICONSTOP);
		m_ctrlTree.SetFocus();
		return;
	}

	WaitCursor.Restore();

	// Open the output file for writing
	OK = f.Open(Filename, CFile::typeBinary|CFile::modeCreate|
		CFile::modeWrite);
	if (!OK) {
		::Beep(450, 100);
		Message.Format("Error opening file '%s' for writing", 
			Filename);
		::AfxMessageBox(Message, MB_OK | MB_ICONSTOP);
		m_ctrlTree.SetFocus();
		theGame.Disconnect();
		return;
	}

	// Get all the scripts in the Scriptoprium 
	hRootNode = m_ctrlTree.GetRootItem();
	GetScripts(hRootNode, ScriptList, ScriptNodeList, ScriptCount);

	for (i=0; i<ScriptCount; i++) {
		ptr = Buffer;
		// Get and parse each script
		Script = ScriptList[i].GetBuffer(1);
		Parser.Parse(Script);	
		Parser.GetParseResults(TokenList,TokenCount, CommandList, CommandCount, 
			ErrorIndexList, ErrorCount);
		// Write the script into the buffer in a neat, indented fashion
		for (j=0; j<CommandCount; j++) {
			Command = (CommandStructure *)CommandList[j];
			StartIndex = Command->StartIndex;
			EndIndex = Command->EndIndex;
			if (StartIndex == -1)
				// Ignore out-of-source errors
				continue;
			for (Tab=0; Tab<Command->IndentLevel; Tab++) {
				*ptr = '\t';
				ptr++;
			}				
			for (k=StartIndex; k<=EndIndex; k++) {
				Token = (TokenStructure *)TokenList[k];
				memcpy(ptr, Token->Name, Token->Size);
				ptr += Token->Size;
				if (k < EndIndex) {
					*ptr = ' ';
					ptr++;
				}
				else {
					*ptr = '\r';
					ptr++;
				}
			}
		}
		*ptr = '\0';
		// Write the buffer into the output file
		f.Write(Buffer, ptr - Buffer);
		ScriptList[i].ReleaseBuffer();
	}
	// Close the output file
	f.Close();
	theGame.Disconnect();
}




void CScriptoriumChildFrame::GetScripts
	(HTREEITEM hNode, 
	 CStringArray & ScriptList, 
	 CPtrArray & ScriptNodeList,
	 int & ScriptCount)
{
	m_ScriptListSize = 1000;
	m_ScriptCount = 0;
	m_ScriptList.SetSize(m_ScriptListSize);
	m_ScriptNodeList.SetSize(m_ScriptListSize);
	GetScriptsHelper(hNode);
	ScriptList.Copy(m_ScriptList);
	ScriptNodeList.Copy(m_ScriptNodeList);
	ScriptCount = m_ScriptCount;
}


BEGIN_MESSAGE_MAP(CScriptoriumChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CScriptoriumChildFrame)
	ON_WM_MDIACTIVATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_SCRIPT_FETCH, OnScriptFetch)
	ON_MESSAGE(WM_NODE_CHANGED, OnNodeChanged)
	ON_MESSAGE(WM_SCRIPT_DELETE, OnScriptDelete)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptoriumChildFrame message handlers



void CScriptoriumChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CString Blanks;

	// Call the default
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	if ((bActivate) && ::IsWindow(m_ctrlTree)) {
		// The scriptorium frame has been activated, so clear the 
		// line number pane and set the focus to the contained tree
		Blanks.LoadString(IDM_BLANKS);
		theFrame->m_wndStatusBar.SetPaneText(1, Blanks, TRUE);
		m_ctrlTree.SetFocus();
	}

	if (GetWindowPlacement(&swp),swp.showCmd = SW_SHOWNORMAL)
		theFrame->SetWindowText(theFrame->m_defaultTitleString + " - Scriptorium Viewer");
	else
		theFrame->SetWindowText(theFrame->m_defaultTitleString);

}


void CScriptoriumChildFrame::OnClose() 
{
	WindowState::Save("Windows", "Scriptorium", m_hWnd);

	// Kill the scriptorium frame
	CMDIChildWnd::OnClose();
	// Notify the main MDI frame that the scriptorium frame is dead
	theFrame->m_ScriptoriumFrame = NULL;
}


void CScriptoriumChildFrame::OnDestroy() 
{
	// Clean up memory used by the scriptorium tree
	UnloadTree();	
	CMDIChildWnd::OnDestroy();
}


void CScriptoriumChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	CRect client;

	// Call the default
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	// Get the client area of the scriptorium frame
	GetClientRect(&client);
	if (::IsWindow(m_ctrlTree.m_hWnd))
		// Force the contained tree to completely fill the client
		// area of the scriptorium frame
		m_ctrlTree.SetWindowPos(NULL, 0, 0, client.Width(), client.Height(), 
			SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);	
	if (GetWindowPlacement(&swp),swp.showCmd = SW_SHOWNORMAL)
		theFrame->SetWindowText(theFrame->m_defaultTitleString + " - Scriptorium Viewer");
	else
		theFrame->SetWindowText(theFrame->m_defaultTitleString);
}



LONG CScriptoriumChildFrame::OnScriptFetch(UINT a, LONG dummy)
{
	HTREEITEM hNode;

	hNode = (HTREEITEM)a;
	theFrame->DoGenericFetch(hNode);
	return 0;
}


LONG CScriptoriumChildFrame::OnScriptDelete(UINT a, LONG dummy)
{
	DeleteScripts();
	return 0;
}


LONG CScriptoriumChildFrame::OnNodeChanged(UINT a, LONG b)
{
	HTREEITEM hNode;
	NodeStructure *Node;
	CString TitleBarMessage, Temp;

	hNode = (HTREEITEM)a;
	Node = (NodeStructure *)b;

	TitleBarMessage = "Viewer - ";
	if (Node->Level == 0) {
		TitleBarMessage += "Scriptorium";
	}
	else if (Node->Level == 1) {
		Temp.Format("Family %d", Node->FGSEValues[0]);
		TitleBarMessage += Temp;
	}
	else if (Node->Level == 2) {
		Temp.Format("Genus [%d, %d]", Node->FGSEValues[0],
			Node->FGSEValues[1]);
		TitleBarMessage += Temp;
	}
	else if (Node->Level == 3) {
		Temp.Format("Species [%d, %d, %d]", Node->FGSEValues[0],
			Node->FGSEValues[1], Node->FGSEValues[2]);
		TitleBarMessage += Temp;
	}
	else {
		Temp.Format("Script [%d, %d, %d, %d]", Node->FGSEValues[0],
			Node->FGSEValues[1], Node->FGSEValues[2],
			Node->FGSEValues[3]);
		TitleBarMessage += Temp;
	}
	// Set the title bar of the scriptorium frame window
	SetWindowText(TitleBarMessage);
	return 0;
}

int CScriptoriumChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	WindowState::Load("Windows", "Scriptorium", m_hWnd);
	
	return 0;
}

