#ifndef GRAPHER_DLG_H
#define GRAPHER_DLG_H

#pragma once

// GrapherDlg.h : header file
//

#include "../../common/Grapher.h"
#include "../../common/LabelThing.h"
#include <vector>
#include "BaseDlg.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CGrapherDlg dialog

class CGrapherDlg : public CBaseDlg
{
	typedef CBaseDlg base;

// Construction
public:
	CGrapherDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGrapherDlg)
	enum { IDD = IDD_GRAPHER_DIALOG };
	//}}AFX_DATA

private:
	Grapher	m_Graph;	// the embedded graph
	CLabelThing m_wndLabel;

	std::vector<GraphTrace> m_vTestTrace;

public:
	virtual CDialogSpec GetSpec();
	virtual void ChangeSpec(const CDialogSpec& spec);
	virtual void Sample(int i, float val);
	virtual void PostSample();
	virtual UINT MenuResourceID() {	return IDR_GRAPHER_MENU; };
	virtual void MenuChange(CMenu* pPopup);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrapherDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGrapherDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	virtual BOOL OnInitDialog();
	afx_msg void OnAxes();
	afx_msg void OnWhitePage();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnInject();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void MyRefresh();
	void LabelTrace(CPoint point);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // GRAPHER_DLG_H

