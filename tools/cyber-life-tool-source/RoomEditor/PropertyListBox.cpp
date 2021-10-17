// PropertyListBox.cpp : implementation file
//

#include "stdafx.h"
#include "c2eroomeditor.h"
#include "PropertyListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyListBox

CPropertyListBox::CPropertyListBox() : m_ListOwner( 0 )
{
}

CPropertyListBox::~CPropertyListBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyListBox, CListBox)
	//{{AFX_MSG_MAP(CPropertyListBox)
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyListBox message handlers

void CPropertyListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( m_ListOwner ) m_ListOwner->OnListScroll( this );	
	CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
}

