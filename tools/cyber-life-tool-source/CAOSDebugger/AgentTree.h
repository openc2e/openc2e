#ifndef AGENT_TREE_H
#define AGENT_TREE_H

#pragma once

#include <vector>
#include <map>

// AgentTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAgentTree window

class FGSU
{
public:
	int f, g, s, unid;
	
	FGSU() : f(-1), g(-1), s(-1), unid(-1) {};
	FGSU(int lf, int lg, int ls, int lu) : f(lf), g(lg), s(ls), unid(lu) {};
	FGSU(const FGSU& fgsu) : f(fgsu.f), g(fgsu.g), s(fgsu.s), unid(fgsu.unid) {};

	bool operator<(const FGSU& other) const
	{
		return (f < other.f) ||
			(f == other.f && g < other.g) ||
			(f == other.f && g == other.g && s < other.s) ||
			(f == other.f && g == other.g && s == other.s && unid < other.unid);
	}

	bool operator==(const FGSU& other) const
	{
		return f == other.f && g == other.g && s == other.s && unid == other.unid;
	}
};

class CAgentTree : public CTreeCtrl
{
// Construction
public:
	CAgentTree();

// Attributes
private:
	std::vector<FGSU> myAgents;
	
	std::map<FGSU, HTREEITEM> myTreeHandle;

	CImageList m_ImageList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgentTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAgentTree();
	void RefreshFromGame();
	int GetSelectedUNID();
	FGSU GetSelectedFGSU();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAgentTree)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()

	void RefreshFGSUMapFromGame();
	void LaunchWatchWindow(HTREEITEM hNode);
	void AddAgentToTree(FGSU a);
	void DeleteAgentFromTree(FGSU a);
	int GetUNID(HTREEITEM hNode);
	FGSU GetFGSU(HTREEITEM hNode);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AGENT_TREE_H

