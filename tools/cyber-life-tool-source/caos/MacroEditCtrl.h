#if !defined(AFX_MACROEDITCTRL_H__5AF16B92_367B_11D2_A804_0060B05E3B36__INCLUDED_)
#define AFX_MACROEDITCTRL_H__5AF16B92_367B_11D2_A804_0060B05E3B36__INCLUDED_

#pragma once


//
// Extra messages.
//

// Sent to parent when a line of macro code has produced a syntax error
#define WM_LINE_ERROR			(WM_USER+1)

// Sent to parent when the contents of the control have been edited
#define WM_CONTENTS_EDITED		(WM_USER+2)

// Sent to parent when the selection/cursor has changed
#define WM_SELECTION_CHANGED	(WM_USER+3)


//
// Class definition
//

class CMacroEditCtrl: public CRichEditCtrl
{
private:
	//
	// Private Member Variables
	//

	CMacroParse m_Parser;
	CHARFORMAT	m_CharFormat;
	CHARFORMAT	m_DefaultCharFormat;
	BOOL		m_DoingBlockIndent;
	BOOL		m_ContentsChanged;
	BOOL		m_ControlKeyDown;
	int 		m_ChangedLineNumber;
	char	   *m_ErrorString;
	bool		m_ReadOnly;

	// Private constants

	enum {
		MAX_LINE_SIZE = 1000
	};

	//
	// Private Functions
	//

	static DWORD CALLBACK InEditStreamCallback
		(DWORD SourceBuffer, 
		 LPBYTE DestinationBuffer, 
		 LONG BytesToBeCopied, 
		 LONG FAR *AmountCopied);
	
	static DWORD CALLBACK OutEditStreamCallback
		(DWORD SourceBuffer, 
		 LPBYTE DestinationBuffer, 
		 LONG BytesToBeCopied, 
		 LONG FAR *AmountCopied);


	void NotifyParentAboutContentChange();
	void NotifyParentAboutSelectionChange();
	void NotifyParentAboutLineError(int LineNumber);

	void ParseLine(int LineNumber);
	void ParseChangedLine(int LineNumber);

public:

	//
	// Public Functions
	//

	CMacroEditCtrl();
	virtual ~CMacroEditCtrl();
	BOOL CreateVersion10(const RECT& rect, CWnd* pParentWnd, UINT nID, bool bReadOnly = false);
	BOOL CreateVersion20(const RECT& rect, CWnd* pParentWnd, UINT nID, bool bReadOnly = false);	
	BOOL CreateRichEdit20(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	void SetContentsFromMacroCode
		(char *       MacroCode,
		 CPtrArray  & TokenList,
		 int        & TokenCount,
		 CPtrArray  & CommandList,
		 int        & CommandCount,
		 CUIntArray & ErrorIndexList,
		 int        & ErrorCount);

	void SetContentsFromRTF(char *RTF);

	void CheckContents
		(CPtrArray  & TokenList,
		 int        & TokenCount,
		 CPtrArray  & CommandList,
		 int        & CommandCount,
		 CUIntArray & ErrorIndexList,
		 int        & ErrorCount);

	BOOL Save(const char *Filename, bool setModified = true);

	void GetScriptInformation
		(CPtrArray  & ScriptList,
		 int        & ScriptCount);

	void Nullify();

	BOOL IsModified();
	BOOL HasSelection();
	void SetLineNumber(int LineNumber);
	void GetLineNumbers(int & StartLineNumber, int & EndLineNumber);

	char *GetLineErrorString();

	void BlockComment();
	void BlockUncomment();
	void BlockIndent();
	void BlockOutdent();
	
	void SelectAll();
	void Cut();
	void Paste();
	
	BOOL Find(char *SearchText, DWORD SearchFlags, BOOL DirectionDown);
	BOOL Replace(char *SearchText, char *ReplaceText, DWORD SearchFlags);
	int ReplaceAll
		(char *SearchText, 
		char *ReplaceText, 
		DWORD SearchFlags, 
		BOOL WholeFile);

	int PositionBeforeToPositionAfter(int pos_before);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMacroEditCtrl)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CMacroEditCtrl)
	//afx_msg void OnChange();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MACROEDITCTRL_H__5AF16B92_367B_11D2_A804_0060B05E3B36__INCLUDED_)

