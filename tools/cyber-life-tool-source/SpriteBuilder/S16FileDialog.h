#if !defined(AFX_S16FILEDIALOG_H__FD925682_8CCF_11D1_8791_0060B07BFA18__INCLUDED_)
#define AFX_S16FILEDIALOG_H__FD925682_8CCF_11D1_8791_0060B07BFA18__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// S16FileDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CS16FileDialog dialog

class CS16FileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CS16FileDialog)

public:
	CS16FileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

protected:
	//{{AFX_MSG(CS16FileDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_S16FILEDIALOG_H__FD925682_8CCF_11D1_8791_0060B07BFA18__INCLUDED_)

