// WatchDialog.cpp : implementation file
//

#include "stdafx.h"
#include "caosdebugger.h"
#include "WatchDialog.h"
#include "MainFrm.h"

#include "../../common/GameInterface.h"
#include <sstream>

/////////////////////////////////////////////////////////////////////////////
// CWatchDialog dialog


CWatchDialog::CWatchDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWatchDialog::IDD, pParent), m_bSourcePresence(false),
	myOptionalWatches(".optional.txt")
{
	//{{AFX_DATA_INIT(CWatchDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWatchDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWatchDialog)
	DDX_Control(pDX, IDC_LIST, m_ctrlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWatchDialog, CDialog)
	//{{AFX_MSG_MAP(CWatchDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_WATCH_ADD, OnWatchAdd)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST, OnKeydownList)
	ON_COMMAND(ID_WATCH_DELETE, OnWatchDelete)
	ON_WM_CREATE()
	ON_COMMAND(ID_SOURCE_TOGGLE, OnSourceToggle)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST, OnEndlabeleditList)
	ON_COMMAND(ID_EDIT_CAOS, OnEditCaos)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_COMMAND(ID_CYCLECLASSIFIER, OnCycleclassifier)
	ON_COMMAND(ID_ALWAYS_ON_TOP, OnAlwaysOnTop)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_SPLITTER_MOVED, OnSplitterMoved)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FOLLOW_AGENTS_CODE, OnSingleStepTack)
	ON_COMMAND_EX_RANGE(ID_WATCH_SET, ID_WATCH_SET + 10000, OnWatchSet)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchDialog message handlers

void CWatchDialog::OnDestroy() 
{
	CDialog::OnDestroy();
}

void CWatchDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if (::IsWindow(m_ctrlList.m_hWnd))
	{
		DoSizing();
	}
	
}

void CWatchDialog::DoSizing()
{
	CRect client;
	GetClientRect(client);

	int nFrame = GetSystemMetrics(SM_CYFRAME);
	int nBorder = GetSystemMetrics(SM_CYBORDER);

	CRect rect_list;
	m_ctrlList.GetClientRect(rect_list);
	rect_list.bottom = client.bottom;
	if (!m_bSourcePresence)
		rect_list.right = client.right;
	m_ctrlList.MoveWindow(rect_list, TRUE);


	if (m_bSourcePresence)
	{
		CRect rect_rich;
		m_ctrlRichEdit.GetClientRect(rect_rich);
		rect_rich.left = rect_list.right + nFrame - 2 * nBorder;
		rect_rich.right = client.right;
		rect_rich.bottom = client.bottom;
		m_ctrlRichEdit.MoveWindow(rect_rich, TRUE);	

		CRect rect_splitter;
		rect_splitter.top = 0;
		rect_splitter.bottom = client.bottom;
		rect_splitter.left = rect_list.right - nBorder;
		rect_splitter.right = rect_rich.left + nBorder;
		m_wndSplitterBar.MoveWindow(rect_splitter, TRUE);
		m_ctrlRichEdit.ShowWindow(true);
		m_wndSplitterBar.ShowWindow(true);
	}
	else
	{
		m_ctrlRichEdit.ShowWindow(false);
		m_wndSplitterBar.ShowWindow(false);
	}
	m_ctrlList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
}

BOOL CWatchDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctrlList.InsertColumn(0, _T("CAOS"), LVCFMT_LEFT, -1, -1);	
	m_ctrlList.InsertColumn(1, _T("Value"), LVCFMT_LEFT, -1, -1);

	m_nSourceWidth = 50;
	SetSourcePresence(true);
	m_ctrlList.SetColumnWidth(0, 100);
	m_ctrlList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	dynamic_cast<CMainFrame*>(AfxGetMainWnd())->AddWatchWindow(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWatchDialog::OnClose() 
{
	dynamic_cast<CMainFrame*>(AfxGetMainWnd())->RemoveWatchWindow(this);

	CDialog::OnClose();

	delete this;	
}

void CWatchDialog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_WATCH_POPUP));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL); 

	if (m_bSourcePresence)
		pPopup->CheckMenuItem(ID_SOURCE_TOGGLE, MF_BYCOMMAND | MF_CHECKED);
	if (!SourceDisplayPossible())
		pPopup->EnableMenuItem(ID_SOURCE_TOGGLE, MF_BYCOMMAND | MF_GRAYED);
	if (myFGSU.unid < 0)
	{
		pPopup->EnableMenuItem(ID_CYCLECLASSIFIER, MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem(ID_FOLLOW_AGENTS_CODE, MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
		if (dynamic_cast<CMainFrame*>(AfxGetMainWnd())->GetTackAgent() == myFGSU.unid)
			pPopup->CheckMenuItem(ID_FOLLOW_AGENTS_CODE, MF_BYCOMMAND | MF_CHECKED);
	}
	bool bOnTop = (GetExStyle() & WS_EX_TOPMOST) != 0;
	if (bOnTop)
		pPopup->CheckMenuItem(ID_ALWAYS_ON_TOP, MF_BYCOMMAND | MF_CHECKED);

	pPopup->InsertMenu(-1, MF_BYPOSITION | MF_SEPARATOR, 0, "");

	myOptionalWatches.StreamIn(myFGSU, myPrettyName);
	int n =	myOptionalWatches.myNames.size();
	ASSERT(n == myOptionalWatches.myCAOS.size());
	bool bLastNotSep = false;
	for (int i = 0; i < n; ++i)
	{
		if (!myOptionalWatches.myNames[i].empty())
		{
			ASSERT(!myOptionalWatches.myCAOS[i].empty());
			pPopup->InsertMenu(-1, MF_BYPOSITION, ID_WATCH_SET + i, myOptionalWatches.myNames[i].c_str());
			bLastNotSep = true;
		}
		else if (bLastNotSep)
		{
			pPopup->InsertMenu(-1, MF_BYPOSITION | MF_SEPARATOR, 0, "");
			bLastNotSep = false;
		}
	}

	CString title;
	pPopup->GetMenuString(pPopup->GetMenuItemCount() - 1, title, MF_BYPOSITION);
	if (title.IsEmpty())
		pPopup->RemoveMenu(pPopup->GetMenuItemCount() - 1, MF_BYPOSITION);

	SetActiveWindow();
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);	
}

void CWatchDialog::OnWatchAdd() 
{
	int ix;
	POSITION pos = m_ctrlList.GetFirstSelectedItemPosition();
	if (pos)
		ix = m_ctrlList.GetNextSelectedItem(pos) + 1;
	else
		ix = 0;

	int new_item = AddNewWatch(ix, std::string(""), std::string("outv"));
	if (new_item >= 0)
		EditCaos(new_item);
}

void CWatchDialog::RefreshFromGame()
{
	int n = m_ctrlList.GetItemCount();
	for (int i = 0; i < n; ++i)
	{
		std::string command = myTargPrefix + " " + myCAOS[i];
		std::string reply;
		theGameInterface.Inject(command, reply);
		m_ctrlList.SetItemText(i, 1, reply.c_str());
	}

	RefreshSourceFromGame();
}
 
void CWatchDialog::OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	UINT nChar = pLVKeyDow->wVKey;

	if (nChar == VK_F2)
	{
		OnEditCaos();
	}
	else if (nChar == VK_INSERT)
	{
		OnWatchAdd();
	}
	else if (nChar == VK_DELETE)
	{
		OnWatchDelete();
	}
	else if (nChar == 'y' || nChar == 'Y')
	{
		OnCycleclassifier();
	}
	else if (nChar == 'i' || nChar == 'I')
	{
		OnSingleStepTack();
	}
	else if (nChar != VK_UP && nChar != VK_DOWN && nChar != VK_F4 && nChar != VK_MENU)
	{
		AfxGetMainWnd()->PostMessage(WM_KEYDOWN, nChar, 0);
		AfxGetMainWnd()->PostMessage(WM_KEYUP, nChar, 0);
		m_ctrlList.SetActiveWindow();
		*pResult = 1;
		return;
	}

	*pResult = 0;
}


void CWatchDialog::OnWatchDelete() 
{
	if (m_ctrlList.GetSelectedCount() > 0)
	{
		POSITION pos = m_ctrlList.GetFirstSelectedItemPosition();
		DeleteWatch(m_ctrlList.GetNextSelectedItem(pos));
	}	
}

// otherwise return quits the dialog
void CWatchDialog::OnOK() 
{
}

void CWatchDialog::OnCancel() 
{
	OnClose();
}

void CWatchDialog::RefreshSourceFromGame()
{
	if (!m_bSourcePresence)
		return;

	// Get classifier of script being run, and position in that
	// script
	std::string command = myTargPrefix +
		" doif targ = null outv 0 else outv 1 outs \" \""
		" outv codf outs \" \""
		" outv codg outs \" \""
		" outv cods outs \" \""
		" outv code outs \" \""
		" outv codp outs \" \""
		" endi";
	std::string reply;
	if (theGameInterface.Inject(command, reply))
	{
		std::istringstream in(reply);
		int targ_valid = 0;
		int f = -1, g = -1, s = -1, e = -1, p = -1;
		in >> targ_valid >> f >> g >> s >> e >> p;

		if (targ_valid == 1)
		{
			if (f > -1 && g > -1 && s > -1 && e > -1 && p > -1)
			{
				Classifier c(f, g, s, e);

				// Get the script
				int poffset = 0;
				std::string script = dynamic_cast<CCAOSDebuggerApp*>(AfxGetApp())->GetScript(c, poffset);

				SetSourceInRichEdit(script.c_str());

				int pos_after = m_ctrlRichEdit.PositionBeforeToPositionAfter(p + poffset);
				m_ctrlRichEdit.SetSel(pos_after, pos_after + 4);
			}
			else
			{
				ASSERT(f == -1 && g == -1 && s == -1 && e == -1 && p == -1);
				SetSourceInRichEdit("Currently not in a script");
			}
		}
		else 
			SetSourceInRichEdit("");
	}
	else
	{
		SetSourceInRichEdit(reply.c_str());
	}
}

CWatchDialog::SetSourceInRichEdit(const char* script)
{
	// Load the script into the edit control, if it isn't there already
	if (myPreviousSource != script)
	{
		CPtrArray TokenList;
		int TokenCount;
		CPtrArray CommandList;
		int CommandCount;
		CUIntArray ErrorIndexList;
		int ErrorCount;

		// SetContentsFromMacroCode is const correct I think, but all
		// the code is written without consts.  So we recast.
		m_ctrlRichEdit.SetContentsFromMacroCode(const_cast<char *>(script), TokenList, 
			TokenCount, CommandList, CommandCount, ErrorIndexList, ErrorCount);
	}

	myPreviousSource = script;
}

int CWatchDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = 100;
	rect.bottom = 300;
	// Use Version 1.0 as we don't need multiple undo
	if (!m_ctrlRichEdit.CreateVersion10(rect, this, IDC_RICHEDIT, true))
	{
		ASSERT(false);
		return -1;
	}
	m_ctrlRichEdit.ShowWindow(SW_SHOW);
	
	CRect rect2(0, 0, 0, 0);
	m_wndSplitterBar.Create(WS_CHILD|WS_BORDER|WS_DLGFRAME|WS_VISIBLE, rect2, this, 999);

	return 0;
}

void CWatchDialog::OnSplitterMoved()
{
	m_ctrlList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
}

void CWatchDialog::OnSourceToggle() 
{
	SetSourcePresence(!m_bSourcePresence);
}

void CWatchDialog::SetSourcePresence(bool bNewPresence)
{
	if (m_bSourcePresence == bNewPresence)
		return;
	m_bSourcePresence = bNewPresence;
	if (m_bSourcePresence)
	{	
		// Resize main window
		CRect rect_this;
		GetWindowRect(rect_this);
		SetWindowPos(NULL, 0, 0, rect_this.Width() + m_nSourceWidth, rect_this.Height(), SWP_NOMOVE | SWP_NOOWNERZORDER);

		// Resize list and source windows
		CRect rect_rich;
		GetClientRect(rect_rich);
		rect_rich.left = rect_this.Width() + 1;
		rect_rich.right = rect_this.Width() + m_nSourceWidth;
		m_ctrlRichEdit.MoveWindow(rect_rich, TRUE);

		m_ctrlList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

		m_wndSplitterBar.SetPanes(&m_ctrlList, &m_ctrlRichEdit);
	}
	else
	{
		CRect rect_source;
		m_ctrlRichEdit.GetWindowRect(rect_source);
		m_nSourceWidth = rect_source.Width();
		// Resize main window
		CRect rect_this;
		GetWindowRect(rect_this);
		SetWindowPos(NULL, 0, 0, rect_this.Width() - m_nSourceWidth, rect_this.Height(), SWP_NOMOVE | SWP_NOOWNERZORDER);
	}

	DoSizing();
}

void CWatchDialog::OnEditCaos() 
{
	ASSERT(myCAOS.size() == m_ctrlList.GetItemCount());

	POSITION pos = m_ctrlList.GetFirstSelectedItemPosition();
	if (pos)
	{
		int ix = m_ctrlList.GetNextSelectedItem(pos);
		EditCaos(ix);
	}
}

int CWatchDialog::AddNewWatch(int ix, const std::string& name, std::string& caos)
{
	ASSERT(myCAOS.size() == m_ctrlList.GetItemCount());
	int reply = m_ctrlList.InsertItem(ix, name.empty() ? caos.c_str() : name.c_str());
	if (reply < 0)
	{
		ASSERT(myCAOS.size() == m_ctrlList.GetItemCount());
		return reply;
	}

	myCAOS.insert(myCAOS.begin() + ix, caos);
	ASSERT(myCAOS.size() == m_ctrlList.GetItemCount());

	return reply;
}

void CWatchDialog::DeleteWatch(int ix)
{
	ASSERT(myCAOS.size() == m_ctrlList.GetItemCount());
	m_ctrlList.DeleteItem(ix);
	myCAOS.erase(myCAOS.begin() + ix);
	ASSERT(myCAOS.size() == m_ctrlList.GetItemCount());
}

void CWatchDialog::EditCaos(int ix)
{
	m_ctrlList.SetItemText(ix, 0, myCAOS[ix].c_str());

	m_ctrlList.EditLabel(ix);
}


void CWatchDialog::OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	if (pDispInfo->item.pszText)
	{
		int ix = pDispInfo->item.iItem;
		std::string new_text = pDispInfo->item.pszText;
		myCAOS[ix] = new_text;
	}

	*pResult = TRUE;
}


void CWatchDialog::OnWatchSet(UINT nID)
{
	int i = nID - ID_WATCH_SET;
	if (i >= 0 && i < myOptionalWatches.myNames.size())
		AddNewWatch(myCAOS.size(), myOptionalWatches.myNames[i], myOptionalWatches.myCAOS[i]);
}

void CWatchDialog::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_ctrlList.GetSelectedCount() > 0)
	{
		FGSU fgsu;
		{
			POSITION pos = m_ctrlList.GetFirstSelectedItemPosition();
			int ix = m_ctrlList.GetNextSelectedItem(pos);
			std::string potential_unid = m_ctrlList.GetItemText(ix, 1);
			std::istringstream in(potential_unid);
			in >> fgsu.unid;
		}

		{
			std::ostringstream out;
			out << "targ agnt " << fgsu.unid << " outv fmly outs \" \" outv gnus outs \" \" outv spcs";
			std::string reply;
			if (theGameInterface.Inject(out.str(), reply))
			{
				std::istringstream in(reply);
				in >> fgsu.f >> fgsu.g >> fgsu.s;
			}
		}

		dynamic_cast<CCAOSDebuggerApp*>(AfxGetApp())->CreateWatch(fgsu);
	}
	
	*pResult = 0;
}

bool CWatchDialog::SourceDisplayPossible()
{
	return myTargPrefix != "";
}

void CWatchDialog::SetClassifier(const FGSU& fgsu, const std::string& non_fgsu_prefix, const std::string& non_fgsu_title)
{
	myFGSU = fgsu;

	// Calculate targ prefix and pretty name
	if (myFGSU.unid >= 0)
	{
		std::ostringstream targ_prefix;
		targ_prefix << "targ agnt " << myFGSU.unid;
		std::ostringstream pretty_name;
		pretty_name << "<" << myFGSU.f << "," << myFGSU.g << "," 
			<< myFGSU.s << ">";

		myTargPrefix = targ_prefix.str();
		myPrettyName = pretty_name.str();
	}
	else
	{
		myTargPrefix = non_fgsu_prefix;
		myPrettyName = non_fgsu_title;
	}

	// Only have source when needed
	SetSourcePresence(SourceDisplayPossible());

	// Set title name
	std::string title = "Watch";
	if (!myPrettyName.empty())
	{
		title += " - ";
		title += myPrettyName;
	}
	if (!myTargPrefix.empty())
	{
		title += " [";
		title += myTargPrefix;
		title += "]";
	}	
	SetWindowText(title.c_str());

	// if no watches yet, add in defaults
	if (myCAOS.empty())
	{
		WatchStreamer ws(".always.txt");
		ws.StreamIn(myFGSU, myPrettyName);
		int n =	ws.myNames.size();
		ASSERT(n == ws.myCAOS.size());
		for (int i = 0; i < n; ++i)
		{
			if (!ws.myNames[i].empty())
			{
				ASSERT(!ws.myCAOS[i].empty());
				AddNewWatch(myCAOS.size(), ws.myNames[i], ws.myCAOS[i]);
			}
		}
	}
}

void CWatchDialog::CycleClassifier()
{
	FGSU fgsu = myFGSU;
	std::ostringstream send;
	send << "doif agnt " << myFGSU.unid << " <> null ";
	send << "seta va00 null seta va01 null enum " << myFGSU.f << " " << myFGSU.g << " " << myFGSU.s << " ";
	send << "doif unid = " << myFGSU.unid << " seta va01 va00 endi ";
	send << "seta va00 targ next ";
	send << "doif va01 eq null seta va01 va00 endi ";
	send << "targ va01 ";
	send << "else rtar " << myFGSU.f << " " << myFGSU.g << " " << myFGSU.s << " ";
	send << "endi outv unid";
	std::string reply;
	if (theGameInterface.Inject(send.str(), reply))
	{
		std::istringstream in(reply);
		in >> fgsu.unid;
		SetClassifier(fgsu, "", "");
	}
}

void CWatchDialog::OnCycleclassifier() 
{
	CycleClassifier();
}


void CWatchDialog::OnAlwaysOnTop() 
{
	DWORD then = GetExStyle();
	bool bOnTop = (then & WS_EX_TOPMOST) != 0;
	if (bOnTop)
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	else
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	DWORD now = GetExStyle();
	bool bNowOnTop = (now & WS_EX_TOPMOST) != 0;
	ASSERT(bOnTop != bNowOnTop);
}

void CWatchDialog::OnSingleStepTack() 
{
	std::string command;
	if (dynamic_cast<CMainFrame*>(AfxGetMainWnd())->GetTackAgent() == myFGSU.unid)
		command = "dbg: tack null";
	else
		command = "dbg: play " + myTargPrefix + " dbg: tack targ";
	std::string reply;
	theGameInterface.Inject(command, reply);
	dynamic_cast<CMainFrame*>(AfxGetMainWnd())->RefreshAllFromGame();
}

