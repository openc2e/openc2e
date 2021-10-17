//***************************************************************
// FlatToolBar.cpp
// (c) 1997, Roger Onslow

#include "stdafx.h"
#include "flattoolbar.h"

#ifdef _DEBUG
#undef THIS_FILE
#define new DEBUG_NEW
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CFlatToolBar, CToolBar)
//{{AFX_MSG_MAP(CFlatToolBar)
ON_WM_WINDOWPOSCHANGING()
ON_WM_PAINT()
ON_WM_NCPAINT()
ON_WM_NCCALCSIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CFlatToolBar,CToolBar)


BOOL CFlatToolBar::LoadToolBar(LPCTSTR lpszResourceName)
{
	if (!CToolBar::LoadToolBar(lpszResourceName))
		return FALSE;
	ModifyStyle(0, TBSTYLE_FLAT | 0x4000 | 0x8000); // make it flat
	return TRUE;
}


BOOL CFlatToolBar::LoadBitmap(LPCTSTR lpszResourceName)
{
	if (!CToolBar::LoadBitmap(lpszResourceName))
		return FALSE;
	ModifyStyle(0, TBSTYLE_FLAT | 0x4000 | 0x8000); // make it flat
	return TRUE;
}


// Because buttons are transparent, we need to repaint the background
void CFlatToolBar::RepaintBackground() {
     // get parent window (there should be one)
     CWnd* pParent = GetParent();
     if (pParent) {
          // get rect for this toolbar
          CRect rw; GetWindowRect(&rw);
          // convert rect to parent coords
          CRect rc = rw; pParent->ScreenToClient(&rc);
          // invalidate this part of parent
          pParent->InvalidateRect(&rc);
          // now do all the other toolbars (etc) that belong to the parent
          for (
               CWnd* pSibling = pParent->GetWindow(GW_CHILD);
               pSibling;
               pSibling = pSibling->GetNextWindow(GW_HWNDNEXT)
          ) {
               // but do not draw ourselves
               if (pSibling == this) continue;
               // convert rect to siblings coords
               CRect rc = rw; pSibling->ScreenToClient(&rc);
               // invalidate this part of sibling
               pSibling->InvalidateRect(&rc);
          }
     }
}

// Draw the separators in the client area
void CFlatToolBar::DrawSeparators() {
     // get a dc for the client area
     CClientDC dc(this);
     // draw the separators on it
     DrawSeparators(&dc);
}

// Draw the separators
void CFlatToolBar::DrawSeparators(CClientDC* pDC) {
     // horizontal vs vertical
     bool ishorz = (m_dwStyle & CBRS_ORIENT_HORZ) != 0;
     // get number of buttons
     int nIndexMax = (int)DefWindowProc(TB_BUTTONCOUNT, 0, 0);
     int nIndex;
     // try each button
     for (nIndex = 0; nIndex < nIndexMax; nIndex++) {
          UINT dwStyle = GetButtonStyle(nIndex);
          UINT wStyle = LOWORD(dwStyle);
          // if it is a separator
          if (wStyle == TBBS_SEPARATOR) {
               // get it's rectangle and width
               CRect rect;
               GetItemRect(nIndex,rect);
               // if small enough to be a true separator
               int w = rect.Width();
               if (w <= 8) {
                    if (ishorz) {
                         // draw the separator bar in the middle
                         CRect rectbar = rect;
                         int x = (rectbar.left+rectbar.right)/2;
                         rectbar.left = x-1; rectbar.right = x+1;
                         pDC->Draw3dRect(rectbar,::GetSysColor(COLOR_3DSHADOW),::GetSysColor(COLOR_3DHILIGHT));
                    } else {
                         // draw the separator bar in the middle
                         CRect rectbar = rect;
                         rectbar.left = rectbar.left - m_sizeButton.cx;
                         rectbar.right = rectbar.left + m_sizeButton.cx;
                         rectbar.top = rectbar.bottom+1;
                         rectbar.bottom = rectbar.top+3;
                         int y = (rectbar.top+rectbar.bottom)/2;
                         rectbar.top = y-1; rectbar.bottom = y+1;
                         pDC->Draw3dRect(rectbar,::GetSysColor(COLOR_3DSHADOW),::GetSysColor(COLOR_3DHILIGHT));
                    }
               }
          }
     }
}

// Draw the gripper at left or top
void CFlatToolBar::DrawGripper(CWindowDC *pDC, CRect& rectWindow) {
     // get the gripper rect (1 pixel smaller than toolbar)
     CRect gripper = rectWindow;
     gripper.DeflateRect(1,1);
     if (m_dwStyle & CBRS_FLOATING) {
          // no grippers
     } else if (m_dwStyle & CBRS_ORIENT_HORZ) {
          // gripper at left
          gripper.right = gripper.left+3;
          pDC->Draw3dRect(gripper,::GetSysColor(COLOR_3DHIGHLIGHT),::GetSysColor(COLOR_3DSHADOW));
          gripper.OffsetRect(+4,0);
          pDC->Draw3dRect(gripper,::GetSysColor(COLOR_3DHIGHLIGHT),::GetSysColor(COLOR_3DSHADOW));
          rectWindow.left += 8;
     } else {
          // gripper at top
          gripper.bottom = gripper.top+3;
          pDC->Draw3dRect(gripper,::GetSysColor(COLOR_3DHIGHLIGHT),::GetSysColor(COLOR_3DSHADOW));
          gripper.OffsetRect(0,+4);
          pDC->Draw3dRect(gripper,::GetSysColor(COLOR_3DHIGHLIGHT),::GetSysColor(COLOR_3DSHADOW));
          rectWindow.top += 8;
     }
}

// Erase the non-client area (borders) - copied from MFC implementation
void CFlatToolBar::EraseNonClient() {
     // get window DC that is clipped to the non-client area
     CWindowDC dc(this);
     CRect rectClient;
     GetClientRect(rectClient);
     CRect rectWindow;
     GetWindowRect(rectWindow);
     ScreenToClient(rectWindow);
     rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
     dc.ExcludeClipRect(rectClient);

     // draw borders in non-client area
     rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
     DrawBorders(&dc, rectWindow);

     // erase parts not drawn
     dc.IntersectClipRect(rectWindow);
     SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC);

     DrawGripper(&dc, rectWindow); // <-- my addition to draw gripper
}

// Because buttons are transparaent, we need to repaint background if style changes
void CFlatToolBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler) {
     static CUIntArray styles;
     // get the number of buttons
     int nIndexMax = (int)DefWindowProc(TB_BUTTONCOUNT, 0, 0);
     int nIndex;
     // save styles
     for (nIndex = 0; nIndex < nIndexMax; nIndex++) {
          UINT dwStyle = GetButtonStyle(nIndex);
          styles.SetAtGrow(nIndex,dwStyle);
     }
     // do base class processing
     CToolBar::OnUpdateCmdUI(pTarget,bDisableIfNoHndler);
     // make checked button appear pushed in
     for (nIndex = 0; nIndex < nIndexMax; nIndex++) {
          UINT dwStyle = GetButtonStyle(nIndex);
          if (dwStyle & TBBS_DISABLED) {
               // don't touch if disabled (avoids flicker)
          } else if (dwStyle & TBBS_CHECKBOX) {
               UINT dwStyleWas = dwStyle;
               // if checked, make it pressed, else not pressed
               if (dwStyle & TBBS_CHECKED) {
                    dwStyle |= TBBS_PRESSED;
               } else if (!(styles[nIndex]&TBBS_CHECKED) && (styles[nIndex]&TBBS_PRESSED)) {
                    dwStyle |= TBBS_PRESSED;
               } else {
                    dwStyle &= ~TBBS_PRESSED;
               }
               // set new style if changed
               if (dwStyleWas != dwStyle) SetButtonStyle(nIndex,dwStyle);
          }
     }
     // check for changes to style (buttons presssed/released)
     for (nIndex = 0; nIndex < nIndexMax; nIndex++) {
          UINT dwStyle = GetButtonStyle(nIndex);
          if (styles[nIndex] != dwStyle) {
               // repaint whole toolbar (not just this button)
               Invalidate();
               // no need to check any more
               break;
          }
     }
}

// Because buttons are transparent, we need to repaint background on size or move
void CFlatToolBar::OnWindowPosChanging(LPWINDOWPOS lpwp) {
     // default processing
     CToolBar::OnWindowPosChanging(lpwp);
     RepaintBackground();
}

// Paint the toolbar
void CFlatToolBar::OnPaint() {
     // standard tolbar
     CToolBar::OnPaint();
     // erase the background
     EraseNonClient();
     // plus separators
     DrawSeparators();
}

// Paint the non-client area - copied from MFC implementatios
void CFlatToolBar::OnNcPaint() {
     //   EraseNonClient(); don't do it here
}

// Calculate the non-client area - adjusting for grippers
void CFlatToolBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp) {
     CToolBar::OnNcCalcSize(bCalcValidRects,lpncsp);
     // adjust non-client area for gripper at left or top
     if (m_dwStyle & CBRS_FLOATING) {
          // no grippers
     } else if (m_dwStyle & CBRS_ORIENT_HORZ) {
          // move 2 pixels right to make room
          lpncsp->rgrc[0].left += 2;
          lpncsp->rgrc[0].right += 2;
     } else {
          // move 4 pixels downto make room
          lpncsp->rgrc[0].top += 4;
          lpncsp->rgrc[0].bottom += 4;
     }
}


/*********************************************************************
* Public: SetButtonInfo.
* Inserts the button if no space exists for it, otherwise sets the
* button configuration in the normal way.
*********************************************************************/
void CFlatToolBar::SetButtonInfo(
    int nIndex, UINT nID, UINT nStyle, int iImage)
{
    // Obtain actual system toolbar ctrl.
    CToolBarCtrl& rToolBarCtrl = GetToolBarCtrl();

    // Does button nIndex exist already?
    if (nIndex >= rToolBarCtrl.GetButtonCount())
    {
        TBBUTTON Button = {iImage, nID, (BYTE)TB_HIDEBUTTON, (BYTE)nStyle, 0, 0};
        rToolBarCtrl.InsertButton(nIndex, &Button);
    }
    else
    {
        CToolBar::SetButtonInfo(nIndex, nID, nStyle, iImage);
    }
}

