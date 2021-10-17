// ScriptoriumTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Caos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CScriptoriumTreeCtrl

// Constructor
CScriptoriumTreeCtrl::CScriptoriumTreeCtrl()
{
}

// Destructor
CScriptoriumTreeCtrl::~CScriptoriumTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CScriptoriumTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CScriptoriumTreeCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnNodeChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CScriptoriumTreeCtrl message handlers


void CScriptoriumTreeCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_RETURN) {
		// This stops the control beeping
		return;
	}
	// Call the default
	CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
}



void CScriptoriumTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	DWORD dw;
	NodeStructure *Node;
	HTREEITEM hNode;
	CWnd *Parent;

	// Get the scriptorium frame window
	Parent = GetParent();
	// Get details about the selected node
	hNode = GetSelectedItem();
	dw = GetItemData(hNode);
	Node = (NodeStructure *)dw;

	if (nChar == VK_DELETE) {		
		if (Parent != NULL)
			// Inform the scriptorium frame
			Parent->SendMessage(WM_SCRIPT_DELETE, (UINT)hNode, 0);
		return;
	}
	else if (nChar == VK_RETURN) {
		if (Parent != NULL) 
			// Inform the scriptorium frame
			Parent->SendMessage(WM_SCRIPT_FETCH, (UINT)hNode, 0);
		return;
	}

	// Call the default handler
	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CScriptoriumTreeCtrl::OnNodeChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DWORD dw;
	NodeStructure *Node;
	HTREEITEM hNode;
	CWnd *Parent;

	// Get the scriptorium frame window
	Parent = GetParent();
	// Get details about the selected node
	hNode = GetSelectedItem();
	dw = GetItemData(hNode);
	Node = (NodeStructure *)dw;

	if (Parent != NULL)
		// Inform the scriptorium frame
		Parent->SendMessage(WM_NODE_CHANGED, (UINT)hNode, (LONG)Node);

	*pResult = 0;
}


void CScriptoriumTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CWnd *Parent;
	DWORD dw;
	NodeStructure *Node;
	TV_HITTESTINFO HitInfo;
	HTREEITEM hNode;
	BOOL OK;
	
	// Do a hit test on the location of the double-click
	HitInfo.pt = point;
	HitTest(&HitInfo);
	// Is there a node at that location?
	hNode = HitInfo.hItem;
	OK = FALSE;
	if (hNode != NULL) {
		if ((HitInfo.flags == TVHT_ONITEMICON) ||
			(HitInfo.flags == TVHT_ONITEMLABEL)) {
			// The user double-clicked on either the node image or the 
			// node text...
			// Get details about the node
			dw = GetItemData(hNode);
			Node = (NodeStructure *)dw;
			if (Node->Level == 4) {		
				Parent = GetParent();
				if (Parent != NULL) 
					OK = TRUE;
			}
		}
	}

	// Call the default handler
	CTreeCtrl::OnLButtonDblClk(nFlags, point);

	if (OK) {
		// Inform the scriptorium frame
		Parent->SendMessage(WM_SCRIPT_FETCH, (UINT)hNode, (LONG)Node);
	}
}

