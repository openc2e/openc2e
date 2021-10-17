// QuickNornDlg.h : header file
//

#if !defined(AFX_QUICKNORNDLG_H__901E1CCA_CBD9_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_QUICKNORNDLG_H__901E1CCA_CBD9_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "PictureEx.h"
#include "NornConfig.h"

/////////////////////////////////////////////////////////////////////////////
// CQuickNornDlg dialog

class CQuickNornDlg : public CDialog, CPictureExClient
{
// Construction
public:
	CQuickNornDlg(CWnd* pParent = NULL);	// standard constructor

	virtual void OnPictureClick( CPictureEx *picture, CPoint point );
	void ShowPicture();

	void ClearData();
	void ReadAttatchmentFile( int part,
							std::vector< std::vector< CPoint  > > &points );
	void WriteAttatchmentFile( int part,
							std::vector< std::vector< CPoint  > > &points );


// Dialog Data
	//{{AFX_DATA(CQuickNornDlg)
	enum { IDD = IDD_QUICKNORN_DIALOG };
	CComboBox	m_SetCombo;
	CListBox	m_DirectionList;
	CListBox	m_BodyPartList;
	CListBox	m_PointList;
	CPictureEx	m_Picture;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuickNornDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CQuickNornDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDir();
	afx_msg void OnSelchangeBitmapDir();
	afx_msg void OnSelchangeBodyPartList();
	afx_msg void OnSelchangeDirectionList();
	afx_msg void OnSelchangeSet();
	afx_msg void OnSave();
	afx_msg void OnClear();
	afx_msg void OnAutoOne();
	afx_msg void OnAutoPart();
	afx_msg void OnAutoAll();
	//}}AFX_MSG
	virtual void OnCancel();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()

	int PixelB(int x, int y);
	int PixelG(int x, int y);
	int PixelR(int x, int y);

	CPoint FindColourPixel(COLOUR col);
private:
	CString m_BitmapDir;
	CString m_AttatchmentDir;
	CString m_Suffix;
	int m_Position;
	int m_Set;
	int m_Part;
//	m_Points[part][set][position][attatchpoint]
	std::vector< std::vector< std::vector< std::vector< CPoint  > > > > m_Points;
	bool m_DataLoaded;

	int m_Width;
	int m_PaddedWidth;
	int m_Height;
	std::vector<unsigned char> m_PixelData;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUICKNORNDLG_H__901E1CCA_CBD9_11D2_9D30_0090271EEABE__INCLUDED_)

