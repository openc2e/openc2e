// ComboList.cpp : implementation file
//

#include "stdafx.h"
#include "c2eroomeditor.h"
#include "ComboList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboList

CComboList::CComboList() : m_ListOwner( 0 )
{
}

CComboList::~CComboList()
{
}


BEGIN_MESSAGE_MAP(CComboList, CListBox)
	//{{AFX_MSG_MAP(CComboList)
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboList message handlers

void CComboList::OnKillFocus(CWnd* pNewWnd) 
{
	CListBox::OnKillFocus(pNewWnd);
	
	ShowWindow( SW_HIDE );	
}

void CComboList::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CListBox::OnLButtonUp(nFlags, point);
	if( m_ListOwner )
		m_ListOwner->OnChangeSelection( this, GetCurSel() );
	ShowWindow( SW_HIDE );	
}

void CComboList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
	if( nChar == 13 )
	{
		if( m_ListOwner )
			m_ListOwner->OnChangeSelection( this, GetCurSel() );
		ShowWindow( SW_HIDE );
	}
}

BOOL CComboList::Create( CComboListOwner *owner )//, HWND owner )
{
	m_ListOwner = owner;
	return CreateEx( 0, _T("LISTBOX"), "", WS_VSCROLL | WS_BORDER | WS_POPUP, 0, 0, 100, 100, 0, 0 );
}

void CComboList::OnSelchange() 
{
}

