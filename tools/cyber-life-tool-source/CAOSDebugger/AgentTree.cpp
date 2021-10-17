// AgentTree.cpp : implementation file
//

#include "stdafx.h"
#include "caosdebugger.h"
#include "AgentTree.h"
#include "../../common/GameInterface.h"
#include <algorithm>
#include <sstream>

/////////////////////////////////////////////////////////////////////////////
// CAgentTree

CAgentTree::CAgentTree()
{
}

CAgentTree::~CAgentTree()
{
}


void CAgentTree::RefreshFGSUMapFromGame()
{
	std::string send =
		"enum 0 0 0 "
		"outv fmly "  
		"outs \" \" " 
		"outv gnus " 
		"outs \" \" " 
		"outv spcs " 
		"outs \" \" " 
		"outv unid " 
		"outs \"\\n\" " 
		"next ";
	std::string reply;

	if (!theGameInterface.Inject(send, reply))
		return;

	myAgents.clear();
	std::istringstream in(reply);

	while (in.good())
	{
		FGSU fgsu;
		in >> fgsu.f >> fgsu.g >> fgsu.s >> fgsu.unid;
		if (!in.good())
			break;
		myAgents.push_back(fgsu);
	}

	std::sort(myAgents.begin(), myAgents.end());	
}

void CAgentTree::RefreshFromGame()
{
	// Remember old agent list, and get new one from game
	std::vector<FGSU> oldAgents = myAgents;
	RefreshFGSUMapFromGame();
	
	// Find list of added agents and deleted agents
	std::vector<FGSU> added;
	std::vector<FGSU> deleted;

	int old_ix = 0, my_ix = 0;
	int old_n = oldAgents.size();
	int my_n = myAgents.size();
	while (old_ix < old_n || my_ix < my_n)
	{
		FGSU old_fgsu;
		if (old_ix < old_n)
			old_fgsu = oldAgents[old_ix];

		FGSU my_fgsu;
		if (my_ix < my_n)
			my_fgsu = myAgents[my_ix];

		if (old_ix >= old_n || (my_ix < my_n && my_fgsu < old_fgsu))
		{
			added.push_back(my_fgsu);
			++my_ix;
		}
		else if (my_ix >= my_n || (old_ix < old_n && old_fgsu < my_fgsu))
		{
			deleted.push_back(old_fgsu);
			++old_ix;
		}
		else
		{
			ASSERT(my_ix < my_n && old_ix < old_n);
			ASSERT(old_fgsu == my_fgsu);
			++old_ix;
			++my_ix;
		}
	}

	// Add in each added agent
	{
		int n = added.size();
		for (int i = 0; i < n; ++i)
			AddAgentToTree(added[i]);
	}

	// Delete each deleted agent
	{
		int n = deleted.size();
		for (int i = 0; i < n; ++i)
			DeleteAgentFromTree(deleted[i]);
	}
}

void CAgentTree::AddAgentToTree(FGSU a)
{
	ASSERT(myTreeHandle.find(a) == myTreeHandle.end());

	FGSU family(a.f, -1, -1, -1);	
	if (myTreeHandle.find(family) == myTreeHandle.end())
	{
		std::ostringstream display_name;
		display_name << a.f << '\0';

		HTREEITEM insert_after = TVI_FIRST;
		int f_after = -1;
		for (std::map<FGSU, HTREEITEM>::iterator it = myTreeHandle.begin(); it != myTreeHandle.end(); ++it)
		{
			FGSU fgsu = it->first;
			if (fgsu.g == -1 && fgsu.s == -1 && fgsu.unid == -1 && fgsu.f > f_after && fgsu.f < a.f)
			{
				f_after = fgsu.f;
				insert_after = it->second;
			}
		}

		HTREEITEM new_f = InsertItem(display_name.str().c_str(), 1, 1, TVI_ROOT, insert_after);
		FGSU* item_data = new FGSU(family);
		SetItemData(new_f, reinterpret_cast<DWORD>(item_data));
		myTreeHandle[family] = new_f;
	}
	
	FGSU genus(a.f, a.g, -1, -1);	
	if (myTreeHandle.find(genus) == myTreeHandle.end())
	{
		std::ostringstream display_name;
		display_name << a.g << '\0';

		HTREEITEM insert_after = TVI_FIRST;
		int g_after = -1;
		for (std::map<FGSU, HTREEITEM>::iterator it = myTreeHandle.begin(); it != myTreeHandle.end(); ++it)
		{
			FGSU fgsu = it->first;
			if (fgsu.f == a.f && fgsu.s == -1 && fgsu.unid == -1 && fgsu.g > g_after && fgsu.g < a.g)
			{
				g_after = fgsu.g;
				insert_after = it->second;
			}
		}

		HTREEITEM new_g = InsertItem(display_name.str().c_str(), 2, 2, myTreeHandle[family], insert_after);
		FGSU* item_data = new FGSU(genus);
		SetItemData(new_g, reinterpret_cast<DWORD>(item_data));
		myTreeHandle[genus] = new_g;
	}

	FGSU species(a.f, a.g, a.s, -1);	
	if (myTreeHandle.find(species) == myTreeHandle.end())
	{
		std::ostringstream display_name;
		display_name << a.s << '\0';

		HTREEITEM insert_after = TVI_FIRST;
		int s_after = -1;
		for (std::map<FGSU, HTREEITEM>::iterator it = myTreeHandle.begin(); it != myTreeHandle.end(); ++it)
		{
			FGSU fgsu = it->first;
			if (fgsu.f == a.f && fgsu.g == a.g && fgsu.unid == -1 && fgsu.s > s_after && fgsu.s < a.s)
			{
				s_after = fgsu.s;
				insert_after = it->second;
			}
		}

		HTREEITEM new_s = InsertItem(display_name.str().c_str(), 3, 3, myTreeHandle[genus], insert_after);
		FGSU* item_data = new FGSU(species);
		SetItemData(new_s, reinterpret_cast<DWORD>(item_data));
		myTreeHandle[species] = new_s;
	}

	std::ostringstream display_name;
	display_name << a.unid << '\0';

	HTREEITEM insert_after = TVI_FIRST;
	int unid_after = -1;
	for (std::map<FGSU, HTREEITEM>::iterator it = myTreeHandle.begin(); it != myTreeHandle.end(); ++it)
	{
		FGSU fgsu = it->first;
		if (fgsu.f == a.f && fgsu.g == a.g && fgsu.s == a.s && fgsu.unid > unid_after && fgsu.unid < a.unid)
		{
			unid_after = fgsu.unid;
			insert_after = it->second;
		}
	}

	HTREEITEM new_unid = InsertItem(display_name.str().c_str(), 0, 0, myTreeHandle[species], insert_after);
	FGSU* item_data = new FGSU(a);
	SetItemData(new_unid, reinterpret_cast<DWORD>(item_data));
	myTreeHandle[a] = new_unid;
}

void CAgentTree::DeleteAgentFromTree(FGSU a)
{
	{
		std::map<FGSU, HTREEITEM>::iterator it = myTreeHandle.find(a);
		ASSERT(it != myTreeHandle.end());
		HTREEITEM removing = it->second;
		FGSU* item_data = reinterpret_cast<FGSU*>(GetItemData(removing));
		DeleteItem(removing);
		delete item_data;
		myTreeHandle.erase(it);
	}

	{
		FGSU species(a.f, a.g, a.s, -1);
		std::map<FGSU, HTREEITEM>::iterator it = myTreeHandle.find(species);
		ASSERT(it != myTreeHandle.end());
		HTREEITEM removing = it->second;
		if (!ItemHasChildren(removing))
		{
			FGSU* item_data = reinterpret_cast<FGSU*>(GetItemData(removing));
			DeleteItem(removing);
			delete item_data;
			myTreeHandle.erase(it);
		}
		else
			return;
	}

	{
		FGSU genus(a.f, a.g, -1, -1);
		std::map<FGSU, HTREEITEM>::iterator it = myTreeHandle.find(genus);
		ASSERT(it != myTreeHandle.end());
		HTREEITEM removing = it->second;
		if (!ItemHasChildren(removing))
		{
			FGSU* item_data = reinterpret_cast<FGSU*>(GetItemData(removing));
			DeleteItem(removing);
			delete item_data;
			myTreeHandle.erase(it);
		}
		else
			return;
	}

	{
		FGSU family(a.f, -1, -1, -1);
		std::map<FGSU, HTREEITEM>::iterator it = myTreeHandle.find(family);
		ASSERT(it != myTreeHandle.end());
		HTREEITEM removing = it->second;
		if (!ItemHasChildren(removing))
		{
			FGSU* item_data = reinterpret_cast<FGSU*>(GetItemData(removing));
			DeleteItem(removing);
			delete item_data;
			myTreeHandle.erase(it);
		}
		else
			return;
	}
}

BEGIN_MESSAGE_MAP(CAgentTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CAgentTree)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CHAR()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgentTree message handlers

int CAgentTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CBitmap bm;
	bm.LoadBitmap(IDB_IMAGELIST);
	m_ImageList.Create(16, 16, ILC_COLOR24, 5, 1);
	m_ImageList.Add(&bm, CLR_NONE);
	SetImageList(&m_ImageList, TVSIL_NORMAL);
	
	return 0;
}

BOOL CAgentTree::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.style &= ~WS_BORDER;
	cs.style |= TVS_HASLINES | TVS_HASBUTTONS | TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS;
	
	return TRUE;
}

void CAgentTree::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_RETURN && GetSelectedItem())
	{
		DWORD dw = GetItemData(GetSelectedItem());
		FGSU* pfgsu = reinterpret_cast<FGSU*>(dw);
		if (pfgsu->unid >= 0)
			LaunchWatchWindow(GetSelectedItem());
		else
			Expand(GetSelectedItem(), TVE_EXPAND);
	}
	else 
		CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
}


void CAgentTree::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// Do a hit test on the location of the double-click
	TV_HITTESTINFO HitInfo;
	HitInfo.pt = point;
	HitTest(&HitInfo);
	// Is there a node at that location?
	HTREEITEM hNode = HitInfo.hItem;
	if (hNode != NULL) {
		if ((HitInfo.flags == TVHT_ONITEMICON) ||
			(HitInfo.flags == TVHT_ONITEMLABEL))
		{
			LaunchWatchWindow(hNode);
		}
	}

	// Call the default handler
	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

void CAgentTree::LaunchWatchWindow(HTREEITEM hNode)
{
	FGSU fgsu = GetFGSU(hNode);
	if (fgsu.unid > -1)
		dynamic_cast<CCAOSDebuggerApp*>(AfxGetApp())->CreateWatch(fgsu);
}

FGSU CAgentTree::GetSelectedFGSU()
{
	return GetFGSU(GetSelectedItem());
}

int CAgentTree::GetSelectedUNID()
{
	return GetUNID(GetSelectedItem());
}

int CAgentTree::GetUNID(HTREEITEM hNode)
{
	if (hNode == NULL)
		return -1;

	DWORD dw = GetItemData(hNode);
	FGSU* pfgsu = reinterpret_cast<FGSU*>(dw);

	return pfgsu->unid;
}

FGSU CAgentTree::GetFGSU(HTREEITEM hNode)
{
	if (hNode == NULL)
		return FGSU();
	DWORD dw = GetItemData(hNode);
	FGSU* pfgsu = reinterpret_cast<FGSU*>(dw);
	return *pfgsu;
}

]void CAgentTree::OnDestroy() 
{
	// The SetItemData uses new/delete, so we have to
	// delete each agent from the tree to make sure
	// all memory is freed.
	int n = myAgents.size();
	for (int i = 0; i < n; ++i)
	{
		DeleteAgentFromTree(myAgents[i]);
	}

	CTreeCtrl::OnDestroy();	
}

