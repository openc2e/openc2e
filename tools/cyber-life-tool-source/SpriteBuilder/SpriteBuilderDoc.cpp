// SpriteBuilderDoc.cpp : implementation of the CSpriteBuilderDoc class
//
#ifdef _MSC_VER
#pragma warning(disable:4786 4503)
#endif

#include "stdafx.h"
#include "SpriteBuilder.h"

#include "SpriteBuilderDoc.h"
#include "CntrItem.h"
#include "MainFrm.h"
#include "SpriteBuilderView.h"
#include "S16FileDialog.h"

#include "ScrollyBitmapViewer.h"
#include "SpriteCutter.h"
#include <map>

#include <direct.h>
#include <afxadv.h>
#include <string>

#include "OverlayFileSelector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <afxadv.h>

extern CSpriteBuilderApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSpriteBuilderDoc

IMPLEMENT_DYNCREATE(CSpriteBuilderDoc, COleDocument)

BEGIN_MESSAGE_MAP(CSpriteBuilderDoc, COleDocument)
	//{{AFX_MSG_MAP(CSpriteBuilderDoc)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_EXPORT_BITMAP, OnExportBitmap)
	ON_COMMAND(ID_INSERT_BITMAP, OnInsertBitmap)
	ON_COMMAND(ID_REPLACE_BITMAP, OnReplaceBitmap)
	ON_UPDATE_COMMAND_UI(ID_REPLACE_BITMAP, OnUpdateReplaceBitmap)
	ON_UPDATE_COMMAND_UI(ID_INSERT_BITMAP, OnUpdateInsertBitmap)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_BITMAP, OnUpdateExportBitmap)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_INITIATECUT, OnEditInitiatecut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INITIATECUT, OnUpdateEditInitiatecut)
	ON_COMMAND(ID_EDIT_ZOOMCUTTER2X, OnEditZoomcutter2x)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ZOOMCUTTER2X, OnUpdateEditZoomcutter2x)
	ON_UPDATE_COMMAND_UI(ID_AUTOMATIC_UNCUT, OnUpdateAutomaticUncut)
	ON_COMMAND(ID_AUTOMATIC_UNCUT, OnAutomaticUncut)
	ON_COMMAND(ID_EDIT_EXPORTALL, OnEditExportall)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EXPORTALL, OnUpdateEditExportall)
	ON_COMMAND(ID_EDIT_IMPORTALL, OnEditImportall)
	ON_COMMAND(ID_FILE_BATCHCONVERT_TOC16, OnFileBatchconvertToc16)
	ON_COMMAND(ID_FILE_BATCHCONVERT_TOS16, OnFileBatchconvertTos16)
	ON_COMMAND(ID_NORN_BUILDBODYPARTS, OnNornBuildbodyparts)
	ON_COMMAND(ID_NORN_CREATURES3, OnNornCreatures3)
	ON_UPDATE_COMMAND_UI(ID_NORN_CREATURES3, OnUpdateNornCreatures3)
	ON_COMMAND(ID_EDIT_IMPORTALL4DIGIT, OnEditImportall4digit)
	ON_COMMAND(ID_EDIT_COSTUMES, OnEditCostumes)
	ON_COMMAND(ID_REMOVE_MIRRORS, OnRemoveMirrors)
	ON_COMMAND(ID_ANISTRIP_BUTTON, OnAnistripButton)
	ON_UPDATE_COMMAND_UI(ID_ANISTRIP_BUTTON, OnUpdateAnistripButton)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_EDIT_INITIATEAUTOCUT, OnEditInitiateAutocut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INITIATEAUTOCUT, OnUpdateEditInitiatecut)

	ON_COMMAND(ID_EDIT_PASTEOVER, OnEditPasteOver)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTEOVER, OnUpdateEditPasteOver)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditCut)
	ON_COMMAND_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE4, OnFileMruFile1)
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, COleDocument::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, COleDocument::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, COleDocument::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, COleDocument::OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, COleDocument::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, COleDocument::OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST, COleDocument::OnUpdateObjectVerbMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpriteBuilderDoc construction/destruction

CSpriteBuilderDoc::CSpriteBuilderDoc() :
	m_RectArray(),
	m_UndoArray(),
	m_UndoSelectedBitmapArray(),
	m_isZoomed(true),
	myC3Flag(true),
	myAniStripFlag(true)
{
	// Use OLE compound files
	EnableCompoundFile();

	m_pSprite = NULL;
	m_b565 = TRUE;
}


CSpriteBuilderDoc::~CSpriteBuilderDoc()
{
	if (m_pSprite)
	{
		delete m_pSprite;
		m_pSprite = NULL;
	}

	CleanUp();
}


/*********************************************************************
* Protected: CleanUp.
*********************************************************************/
void CSpriteBuilderDoc::CleanUp()
{
    int iCnt = m_RectArray.GetSize();

    for(int i = 0; i < iCnt; i++)
    {
        CRect* pTmpRect = m_RectArray[0];
		m_RectArray.RemoveAt(0);
		delete pTmpRect;
		pTmpRect = NULL;
	}


    iCnt = m_UndoArray.GetSize();

    for(i = 0; i < iCnt; i++)
    {
        CSprite* pTmpSprite = m_UndoArray[0];
		m_UndoArray.RemoveAt(0);
		delete pTmpSprite;
		pTmpSprite = NULL;
	}
}


BOOL CSpriteBuilderDoc::OnNewDocument()
{
	if (!COleDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	CleanUp();

	if (m_pSprite)
	{
		delete m_pSprite;
		m_pSprite = NULL;
	}
	m_pSprite = new CSprite;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSpriteBuilderDoc serialization

void CSpriteBuilderDoc::Serialize(CArchive& ar)
{
	ASSERT(FALSE); // Never used.

	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}

	// Calling the base class COleDocument enables serialization
	//  of the container document's COleClientItem objects.
	COleDocument::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CSpriteBuilderDoc diagnostics

#ifdef _DEBUG
void CSpriteBuilderDoc::AssertValid() const
{
	COleDocument::AssertValid();
}

void CSpriteBuilderDoc::Dump(CDumpContext& dc) const
{
	COleDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSpriteBuilderDoc commands
/*********************************************************************
* Protected: OnFileOpen.
*********************************************************************/
void CSpriteBuilderDoc::OnFileOpen() 
{
	CS16FileDialog FileDlg(
		TRUE, 
		"s16",
		NULL,
		OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"Sprite Files (*.spr, *.s16,*.c16)|*.spr; *.s16; *.c16||",
		AfxGetMainWnd());

	if (FileDlg.DoModal() == IDOK)
	{
		CString sPathName = FileDlg.GetPathName();
		BOOL bOldS16Format = FileDlg.GetReadOnlyPref();
		LoadGeneral(sPathName, bOldS16Format);
	}
}

/*********************************************************************
* Protected: OnFileMruFile1.
*********************************************************************/
void CSpriteBuilderDoc::OnFileMruFile1(UINT uiID) 
{
	CRecentFileList& rRecentFileList = theApp.RecentFileList();
	CString sPathName = rRecentFileList[uiID - ID_FILE_MRU_FILE1];
	LoadGeneral(sPathName);
}

void CSpriteBuilderDoc::LoadGeneral(CString sPathName, BOOL bOldS16Format)
{
	CleanUp();

	if (m_pSprite)
	{
		delete m_pSprite;
		m_pSprite = NULL;
	}
	m_pSprite = new CSprite;

	CString sExt = sPathName.Right(3);
	sExt.MakeLower();

	BOOL bSuccess;
	if (sExt == "spr")
	{
		bSuccess = m_pSprite->LoadSPR(sPathName);
	}
	else if (sExt == "s16")
	{
		bSuccess = m_pSprite->LoadS16(sPathName, bOldS16Format);
	}
	// now read in compressed format too
	else if( sExt == "c16")
	{
		bSuccess = m_pSprite->LoadC16(sPathName);
	}

	if (bSuccess)
	{
		((CFrameWnd*)AfxGetMainWnd())->GetActiveView()->OnInitialUpdate();
		AfxGetMainWnd()->Invalidate();
		SetPathName(sPathName, TRUE);
	}
}


/*********************************************************************
* Protected: OnFileSave.
*********************************************************************/
void CSpriteBuilderDoc::OnFileSave() 
{
	CString sExt = GetPathName().Right(3);
	sExt.MakeLower();

	if (sExt != "c16" && sExt != "s16")
	{
		OnFileSaveAs();
	}
	else
	{
		if(myAniStripFlag)
		{
			CString sNameStub = GetPathName().Left(GetPathName().GetLength() - 4);
			sNameStub += ".bmp";

			SaveAniStrip(sNameStub);
		}
		else if(sExt == "c16" )
		{
			m_pSprite->SaveC16(GetPathName(), m_b565);
		}
		else if(sExt == "s16")
		{
			m_pSprite->SaveS16(GetPathName(), m_b565);
		}
		else
			ASSERT(false);
	}
}


/*********************************************************************
* Protected: OnFileSaveAs.
*********************************************************************/
void CSpriteBuilderDoc::OnFileSaveAs() 
{
	CString sFileName = GetTitle();

	if(myAniStripFlag)
	{
		CS16FileDialog FileDlg(
		FALSE, 
		NULL,
		sFileName,
		OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"Animation Strip Bitmap Files (*.bmp)|*.bmp||",
		AfxGetMainWnd());

		if (FileDlg.DoModal() == IDOK)
		{
			CString sPathName = FileDlg.GetPathName();
			ASSERT(m_pSprite);

			// Save bitmap data.
			int x = sPathName.ReverseFind('.');

			CString sNameStub = sPathName;

			if(x != -1)
			{
				sNameStub = sPathName.Left(sPathName.GetLength() - 4);
			}
				
			sNameStub += ".bmp";

			SaveAniStrip(sNameStub);
		}
	}
	else
	{

		CS16FileDialog FileDlg(
			FALSE, 
			NULL,
			sFileName,
			OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
			"Sprite Files (*.s16,*c16)|*.s16;*.c16||",
			AfxGetMainWnd());

		if (FileDlg.DoModal() == IDOK)
		{
			CString sPathName = FileDlg.GetPathName();
			ASSERT(m_pSprite);

			// Save bitmap data.
			CString sExt = sPathName.Right(3);
			sExt.MakeLower();


			// default extension
		
				if (!(sExt == "s16" || sExt =="c16"))
				{
				sPathName += ".c16";
				sExt = "c16";
				}

				BOOL bSuccess = false;

				m_b565 = FileDlg.GetReadOnlyPref();
				if(sExt == "c16" )
					{
					bSuccess = m_pSprite->SaveC16(sPathName, m_b565);
					}
				else if(sExt == "s16")
					{
					bSuccess = m_pSprite->SaveS16(sPathName, m_b565);
					}
				else
					ASSERT(false);

				SetPathName(sPathName, TRUE);
			
		}
	}
}


/*********************************************************************
* Protected: OnExportBitmap.
*********************************************************************/
void CSpriteBuilderDoc::OnExportBitmap() 
{
	CFileDialog FileDlg(
		FALSE, 
		"bmp",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"Bitmap Files (*.bmp)|*.bmp||",
		AfxGetMainWnd());

	if (FileDlg.DoModal() == IDOK)
	{
		CString sPathName = FileDlg.GetPathName();
		CString sExt = sPathName.Right(3);
		sExt.MakeLower();

		if (sExt == "bmp")
		{
			CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
			CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
			int iIndex = pView->SelectedBitmap();
			ASSERT(iIndex != -1);

			(*m_pSprite)[iIndex]->SaveBMP(sPathName);
		}
	}
	
}


/*********************************************************************
* Protected: OnUpdateExportBitmap.
*********************************************************************/
void CSpriteBuilderDoc::OnUpdateExportBitmap(CCmdUI* pCmdUI) 
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
	pCmdUI->Enable(pView->SelectedBitmap() != -1);
}


/*********************************************************************
* Protected: OnInsertBitmap.
*********************************************************************/
void CSpriteBuilderDoc::OnInsertBitmap() 
{
	// Store state before change.
	StoreForUndo();

	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
	int iIndex = pView->SelectedBitmap();
	//ASSERT(iIndex != -1);

	// If no bitmap selected add bitmap to end.
	if (iIndex == -1)
	{
		iIndex = m_pSprite->NumberOfBitmaps();
	}

	// Insert new (EMPTY) bitmap.
	CFIBitmap* pBitmap = new CFIBitmap;
	m_pSprite->InsertAt(iIndex, pBitmap);

	// Insert rect.
	m_RectArray.InsertAt(iIndex, new CRect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT));

	// Force recalculation & redraw of view.
	pView->Invalidate(TRUE, iIndex);
}


/*********************************************************************
* Protected: OnUpdateInsertBitmap.
*********************************************************************/
void CSpriteBuilderDoc::OnUpdateInsertBitmap(CCmdUI* pCmdUI) 
{
	//CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	//CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
	pCmdUI->Enable(TRUE);//pView->SelectedBitmap() != -1);
}


/*********************************************************************
* Protected: OnReplaceBitmap.
*********************************************************************/
void CSpriteBuilderDoc::OnReplaceBitmap() 
{
	CFileDialog FileDlg(
		TRUE, 
		"bmp",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"Bitmap Files (*.bmp)|*.bmp||",
		AfxGetMainWnd());

	if (FileDlg.DoModal() == IDOK)
	{
		CString sPathName = FileDlg.GetPathName();
		CString sExt = sPathName.Right(3);
		sExt.MakeLower();

		if (sExt == "bmp")
		{
			// Store state before change.
			StoreForUndo();

			CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
			CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
			int iIndex = pView->SelectedBitmap();
			ASSERT(iIndex != -1);

			// Remove original bitmap.
			CFIBitmap* pTmp = (*m_pSprite)[iIndex];
			m_pSprite->RemoveAt(iIndex);
			delete pTmp;

			// Insert new bitmap.
			CFIBitmap* pBitmap = new CFIBitmap(sPathName);
			m_pSprite->InsertAt(iIndex, pBitmap);

			// Force recalculation & redraw of view.
			pView->Invalidate();
		}
	}
}


/*********************************************************************
* Protected: OnUpdateReplaceBitmap.
*********************************************************************/
void CSpriteBuilderDoc::OnUpdateReplaceBitmap(CCmdUI* pCmdUI) 
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
	pCmdUI->Enable(pView->SelectedBitmap() != -1);
}


/*********************************************************************
* Public: StoreForUndo.
*********************************************************************/
void CSpriteBuilderDoc::StoreForUndo()
{
	int iSize = m_UndoArray.GetSize();
	CSprite *pTmpSprite;

	// Is undo array too big?
	if (iSize > MAX_UNDO)
	{
		// Destroy oldest sprite.
		pTmpSprite = m_UndoArray[0];
		m_UndoArray.RemoveAt(0);
		delete pTmpSprite;
		pTmpSprite = NULL;

		m_UndoSelectedBitmapArray.RemoveAt(0);
	}

	// Copy sprite.
	CSprite* pCurrent = new CSprite(*m_pSprite);
	m_UndoArray.Add(pCurrent);

	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
	int iIndex = pView->SelectedBitmap();

	m_UndoSelectedBitmapArray.Add(iIndex);
}



/*********************************************************************
* Protected: OnEditUndo.
*********************************************************************/
void CSpriteBuilderDoc::OnEditUndo() 
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();

	// Destroy current sprite.
	delete m_pSprite;

	// Retrieve newest in array.
	int iSize = m_UndoArray.GetSize();
	m_pSprite = m_UndoArray[iSize - 1];
	m_UndoArray.RemoveAt(iSize - 1);

	m_pSprite->CreateGDI();

	int iSelected = m_UndoSelectedBitmapArray[iSize - 1];
	m_UndoSelectedBitmapArray.RemoveAt(iSize - 1);

	// Empty rectangle array, so view recreates it.
	// This is necessary as we do not save undo m_RectArray &
	// cannot predict what the change should be.
    int iCnt = m_RectArray.GetSize();

    for(int i = 0; i < iCnt; i++)
    {
        CRect* pTmpRect = m_RectArray[0];
		m_RectArray.RemoveAt(0);
		delete pTmpRect;
		pTmpRect = NULL;
	}

	// Force recalculation & redraw of view.
	pView->Invalidate(TRUE, iSelected);
}


/*********************************************************************
* Protected: OnUpdateEditUndo.
*********************************************************************/
void CSpriteBuilderDoc::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_UndoArray.GetSize() != 0);
}


/*********************************************************************
* Protected: OnEditCut.
*********************************************************************/
void CSpriteBuilderDoc::OnEditCut() 
{
	//Copy data to clipboard.
	OnEditCopy();

	// delete - does StoreForUndo for us
	OnEditDelete();
}

void CSpriteBuilderDoc::OnEditDelete() 
{
	DoDelete(true);
}

void CSpriteBuilderDoc::DoDelete(bool undo)
{
	// Store state before change.
	if (undo)
		StoreForUndo();

	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
	int iIndex = pView->SelectedBitmap();
	ASSERT(iIndex != -1);

	// Notify view.
	pView->Cut(iIndex);

	// Remove & destroy bitmap;
	CFIBitmap* pTmpBitmap = (*m_pSprite)[iIndex];
	m_pSprite->RemoveAt(iIndex);
	delete pTmpBitmap;

	// Remove & destroy rectangle;
	CRect* pTmpRect = m_RectArray[iIndex];
	m_RectArray.RemoveAt(iIndex);
	delete pTmpRect;

	pView->SelectBitmap(iIndex);

	// Force recalculation & redraw of view.
	pView->Invalidate(TRUE, -2);
}


/*********************************************************************
* Protected: OnUpdateEditCut.
*********************************************************************/
void CSpriteBuilderDoc::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
	pCmdUI->Enable(pView->SelectedBitmap() != -1);
}


/*********************************************************************
* Protected: OnEditCopy.
*********************************************************************/
void CSpriteBuilderDoc::OnEditCopy() 
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
	int iIndex = pView->SelectedBitmap();
	ASSERT(iIndex != -1);

	// Obtain bitmap
	CFIBitmap* pBitmap = (*m_pSprite)[iIndex];
	DoCopy(pBitmap);
}

void CSpriteBuilderDoc::DoCopy(CFIBitmap* pBitmap)
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();

	// Make it bottom-up.
	pBitmap->FlipVerticalFormat();
	
	// Create copy of data.
	HGLOBAL hData = ::GlobalAlloc(
		GMEM_MOVEABLE | GMEM_DDESHARE, 
		sizeof(BITMAPINFO) + pBitmap->ImageSize());
	ASSERT(hData);
	BYTE* pData = (BYTE*)::GlobalLock(hData);
	ASSERT(pData);

	int iCnt = sizeof(BITMAPINFOHEADER);
	BITMAPINFO* pInfo = pBitmap->BitmapInfo();
	memcpy(pData, &pInfo->bmiHeader, iCnt);
	pData += sizeof(BITMAPINFOHEADER);
	memcpy(pData, pBitmap->PixelData(), pBitmap->ImageSize());

	// Release memory & restore bitmap.
	::GlobalUnlock(hData);
	pBitmap->FlipVerticalFormat();

	// Write to clipboard.
	VERIFY(pFrameWnd->OpenClipboard());
	VERIFY(::EmptyClipboard());
	if(!::SetClipboardData(CF_DIB, hData))
		TRACE("GetLastError = %d 0x%x\n", GetLastError(), GetLastError());
	VERIFY(::CloseClipboard());

}


/*********************************************************************
* Protected: OnUpdateEditCopy.
*********************************************************************/
void CSpriteBuilderDoc::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
	pCmdUI->Enable(pView->SelectedBitmap() != -1);
}


/*********************************************************************
* Protected: OnEditPaste.
*********************************************************************/
void CSpriteBuilderDoc::OnEditPaste() 
{
	// Store state before change.
	StoreForUndo();

	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
	int iIndex = pView->SelectedBitmap();

	// If no bitmap selected add bitmap to end.
	if (iIndex == -1)
	{
		iIndex = m_pSprite->NumberOfBitmaps();
	}

	DoPaste(iIndex);
}

void CSpriteBuilderDoc::OnEditPasteOver()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
	int iIndex = pView->SelectedBitmap();

	// If no bitmap selected add bitmap to end.
	if (iIndex == -1)
	{
		iIndex = m_pSprite->NumberOfBitmaps();
		StoreForUndo();
	}
	else
	{
		// does StoreForUndo
		OnEditDelete();
	}

	DoPaste(iIndex);
}

void CSpriteBuilderDoc::DoPaste(int iIndex)
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();

	// Obtain bitmap from clipboard.
	VERIFY(pFrameWnd->OpenClipboard());
	HGLOBAL hData = ::GetClipboardData(CF_DIB);
	BYTE* pData = (BYTE*)GlobalLock(hData);

	// Copy data.
	BITMAPINFO BitmapInfo;
	memcpy(&BitmapInfo.bmiHeader, pData, sizeof(BITMAPINFOHEADER));
	pData += sizeof(BITMAPINFOHEADER);
	
	ASSERT(BitmapInfo.bmiHeader.biSizeImage);
	BYTE* pPixelData = new BYTE[BitmapInfo.bmiHeader.biSizeImage];
	ASSERT(pPixelData);

	memcpy(pPixelData, pData, BitmapInfo.bmiHeader.biSizeImage);
	
	// Release memory.
	::GlobalUnlock(hData);
	VERIFY(::CloseClipboard());

	CFIBitmap* pBitmap = new CFIBitmap(&BitmapInfo, pPixelData);
	ASSERT(pBitmap);

	delete pPixelData;

	// Add to sprite file.
	m_pSprite->InsertAt(iIndex, pBitmap);
	m_RectArray.InsertAt(iIndex, new CRect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT));

	// Force recalculation & redraw of view.
	pView->Invalidate(TRUE, iIndex);
}


/*********************************************************************
* Protected: OnUpdateEditPaste.
*********************************************************************/
void CSpriteBuilderDoc::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(::IsClipboardFormatAvailable(CF_DIB));
	
}

void CSpriteBuilderDoc::OnUpdateEditPasteOver(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(::IsClipboardFormatAvailable(CF_DIB));
}

void CSpriteBuilderDoc::OnUpdateEditInitiatecut(CCmdUI* pCmdUI) 
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
	int iIndex = pView->SelectedBitmap();

	// If no bitmap selected abort
	pCmdUI->Enable(iIndex != -1);
}

void CSpriteBuilderDoc::OnEditZoomcutter2x() 
{
	// TODO: Add your command handler code here
	m_isZoomed = !m_isZoomed;
}

void CSpriteBuilderDoc::OnUpdateEditZoomcutter2x(CCmdUI* pCmdUI) 
{
	// If no bitmap selected abort
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(m_isZoomed);
}

void CSpriteBuilderDoc::OnEditInitiatecut() 
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
	int iIndex = pView->SelectedBitmap();

	// If no bitmap selected abort
	if (iIndex == -1)
	{
		return;
	}

	ScrollyBitmapViewer* bmpview;
	bmpview = new ScrollyBitmapViewer((*m_pSprite)[iIndex]);
	CFIBitmap* srcbmp = bmpview->m_fiBitmap;
	bmpview->myCutter = new SpriteCutter();
	bmpview->myCutter->SetSprite(new CSprite());
	bmpview->myCutter->SetBmp(srcbmp);
	bmpview->m_rects = new CRectArray;
	bmpview->m_zoomer = m_isZoomed;
	//bmpview->myCutter->SetMyCount(iIndex+1);
	bmpview->DoModal();

	DoDelete(false);

	while (bmpview->myCutter->GetSprite()->NumberOfBitmaps() > 0)
	{
		CFIBitmap* bmp = (*(bmpview->myCutter->GetSprite()))[0];
		bmpview->myCutter->GetSprite()->RemoveAt(0);
		m_pSprite->InsertAt(iIndex,bmp);
		m_RectArray.InsertAt(iIndex,(*(bmpview->m_rects))[0]);
		bmpview->m_rects->RemoveAt(0);
		iIndex++;
	}
	delete bmpview->myCutter->GetSprite();
	delete bmpview->myCutter;
	delete bmpview->m_rects;
	delete bmpview;
	pView->Invalidate(true, -1);
}


void CSpriteBuilderDoc::OnEditInitiateAutocut() 
{
	CWaitCursor waitCursor;

	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
	int iIndex = pView->SelectedBitmap();

	// If no bitmap selected abort
	if (iIndex == -1)
	{
		return;
	}

	ScrollyBitmapViewer* bmpview;
	bmpview = new ScrollyBitmapViewer((*m_pSprite)[iIndex]);
	CFIBitmap* srcbmp = bmpview->m_fiBitmap;
	bmpview->myCutter = new SpriteCutter();
	bmpview->myCutter->myReplaceColour = true;
	bmpview->myCutter->SetSprite(new CSprite());
	bmpview->myCutter->SetBmp(srcbmp);
	bmpview->m_rects = new CRectArray;
	bmpview->m_zoomer = m_isZoomed;
	//bmpview->myCutter->SetMyCount(iIndex+1);
	// bmpview->DoModal();
	bmpview->AutoCut();

	DoDelete(false);

	while (bmpview->myCutter->GetSprite()->NumberOfBitmaps() > 0)
	{
		CFIBitmap* bmp = (*(bmpview->myCutter->GetSprite()))[0];
		bmpview->myCutter->GetSprite()->RemoveAt(0);

		m_pSprite->InsertAt(iIndex,bmp);
		m_RectArray.InsertAt(iIndex,(*(bmpview->m_rects))[0]);
		bmpview->m_rects->RemoveAt(0);

		iIndex++;
	}
	delete bmpview->myCutter->GetSprite();
	delete bmpview->myCutter;
	delete bmpview->m_rects;
	delete bmpview;
	pView->Invalidate(true, -1);
}


void CSpriteBuilderDoc::OnUpdateAutomaticUncut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_RectArray.GetSize() > 0);
	
}

// Create a cut sheet from all the sprites
void CSpriteBuilderDoc::OnAutomaticUncut() 
{
	CWaitCursor wc;

	// Guess a width to make the new sprite...
	// find maximum width of sprites in file
	int widthToUse = -1;
	int n = m_RectArray.GetSize();
	if (n < 1)
		return;
	for (int i = 0; i < n; ++i)
	{
		CRect rect = *m_RectArray[i];
		if (rect.Width() > widthToUse)
			widthToUse = rect.Width();
	}
	const int margin = 5;
	// extra margin
	widthToUse += margin * 2;
	// minimum
	if (widthToUse < 640)
		widthToUse = 640;

	// Calculate the height
	int heightToUse;
	{
		int x = margin;
		int y = margin;
		int maxY = 1;
		for (int i = 0; i < n; ++i)
		{
			CRect rect = *m_RectArray[i];
			if (x + rect.Width() + margin > widthToUse)
			{
				ASSERT(x > margin);
				y += maxY + margin;
				x = margin;
				maxY = 1;
			}
			// bitmap slotted here
			x += rect.Width() + margin;
			if (rect.Height() > maxY)
				maxY = rect.Height();
		}	

		y += maxY;
		y += margin;
		heightToUse = y;
	}

	int iBytesNeeded = widthToUse * heightToUse * 2; // *2 => 16bit.
	int len = iBytesNeeded / 2;
	WORD* wData = new WORD[len];
	byte* pData = (byte*)wData;
	ASSERT(pData);

	// decide on background colour
	std::map< int, bool > usedColours;
	{
		for (int i = 0; i < n; ++i)
		{
			CRect rect = *m_RectArray[i];
			CFIBitmap* bitmap = (*m_pSprite)[i];
			BYTE* bytePixelData = bitmap->PixelData();
			int byteLineWidth = bitmap->BytesPerLine();
			for (int r = 0; r < rect.Width(); ++r)
			{
				for (int s = 0; s < rect.Height(); ++s)
				{
					int blue = bytePixelData[r * 3 + byteLineWidth * s];
					int green = bytePixelData[r * 3 + 1 + byteLineWidth * s];
					int red = bytePixelData[r * 3 + 2 + byteLineWidth * s];
					int data;
					RGB_TO_565(red, green, blue, data);
					usedColours[data] = true;
				}
			}
		}
	}
	int maxSize = 1 << 16;
	int used = usedColours.size();
	ASSERT(used <= maxSize);
	if (used == maxSize)
	{
		AfxMessageBox("Your sprites contain every available colour - all 65536!\nAt least one needs to be free for the background of the sprite sheet!");
		return;
	}
	int backgroundColour = -1;
	for (int tries = 0; tries < 100000; ++tries)
	{
		int data = ((double)rand() / (double)RAND_MAX) * maxSize;
		if (data >= maxSize)
			data = maxSize - 1;
		if (data < 0)
			data = 0;
		if (usedColours.find(data) == usedColours.end())
		{
			backgroundColour = data;
			break;
		}
	}
	if (backgroundColour == -1)
	{
		for (int data = 0; data < maxSize; ++data)
		{
			if (usedColours.find(data) == usedColours.end())
			{
				backgroundColour = data;
				break;
			}
		}
	}

	ASSERT(backgroundColour >= 0 && backgroundColour < maxSize);

	// fill in background
	{
		for (int i = 0; i < len; ++i)
		{
			wData[i] = backgroundColour;
		}
	}

	{
		int x = margin;
		int y = margin;
		int maxY = 1;
		for (int i = 0; i < n; ++i)
		{
			CRect rect = *m_RectArray[i];
			if (x + rect.Width() + margin > widthToUse)
			{
				ASSERT(x > margin);
				y += maxY + margin;
				x = margin;
				maxY = 1;
			}

			CFIBitmap* bitmap = (*m_pSprite)[i];
			BYTE* bytePixelData = bitmap->PixelData();
			int byteLineWidth = bitmap->BytesPerLine();
			for (int r = 0; r < rect.Width(); ++r)
			{
				for (int s = 0; s < rect.Height(); ++s)
				{
					int blue = bytePixelData[r * 3 + byteLineWidth * s];
					int green = bytePixelData[r * 3 + 1 + byteLineWidth * s];
					int red = bytePixelData[r * 3 + 2 + byteLineWidth * s];

					int data;
					RGB_TO_565(red, green, blue, data);
					ASSERT(data != backgroundColour); // double check we found a decent background colour before!
					wData[x + r + (y + s) * widthToUse] = data;
				}
			}

			x += rect.Width() + margin;
			if (rect.Height() > maxY)
				maxY = rect.Height();
		}	
	}


	CFIBitmap bitmap(
		widthToUse, 
		-heightToUse, // SPR's are top-down bitmaps.
		NULL,
		FORMAT16_565,
		pData);
	DoCopy(&bitmap);
	delete[] pData;
}

void CSpriteBuilderDoc::OnUpdateEditExportall(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_RectArray.GetSize() > 0);
}

void CSpriteBuilderDoc::OnEditExportall() 
{
	CFileDialog FileDlg(
		FALSE, 
		"bmp",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"Bitmap Files (*.bmp)|*.bmp||",
		AfxGetMainWnd());

	if (FileDlg.DoModal() == IDOK)
	{
		CString sPathName = FileDlg.GetPathName();
		CString sExt = sPathName.Right(4);
		sExt.MakeLower();

		if (sExt == ".bmp")
		{
			CString sNameStub = sPathName.Left(sPathName.GetLength() - 4);

			for (int i = 0; i < m_RectArray.GetSize(); ++i)
			{
				CString sName;
				sName.Format(sNameStub + "-%d.bmp", i);
				(*m_pSprite)[i]->SaveBMP(sName);
			}
		}
	}
	
	
}


void CSpriteBuilderDoc::OnEditImportall() 
{
	CFileDialog FileDlg(
		TRUE, 
		"bmp",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"Initial Bitmap Files (*-0.bmp)|*-0.bmp||",
		AfxGetMainWnd());

	if (FileDlg.DoModal() == IDOK)
	{
		CWaitCursor wc;

		CString sPathName = FileDlg.GetPathName();
		CString sExt = sPathName.Right(6);
		sExt.MakeLower();

		if (sExt == "-0.bmp")
		{
			CString sNameStub = sPathName.Left(sPathName.GetLength() - 6);

			// Store state before change.
			StoreForUndo();

			CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
			CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
			int iIndex = pView->SelectedBitmap();
			if (iIndex < 0)
				iIndex = m_RectArray.GetSize();

			int i = 0;
			while(true)
			{
				CString sName;
				sName.Format(sNameStub + "-%d.bmp", i);
				// see if file is there
				if (GetFileAttributes(sName) == 0xFFFFFFFF)
					break;

				// Insert new bitmap.
				CFIBitmap* pBitmap = new CFIBitmap(sName);
				m_pSprite->InsertAt(iIndex, pBitmap);
				m_RectArray.InsertAt(iIndex, new CRect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT));

				++i;
				++iIndex;
			}

			// Force recalculation & redraw of view.
			pView->Invalidate();
		}
		else
			AfxMessageBox("Please choose the first file, whose name ends -0.bmp");
	}	
}

void CSpriteBuilderDoc::OnFileBatchconvertToc16() 
{
	BatchConvertTo("c16");
	
}

void CSpriteBuilderDoc::OnFileBatchconvertTos16() 
{
	BatchConvertTo("s16");
}

void CSpriteBuilderDoc::BatchConvertTo(CString extension)
{
	CFileDialog fileDlg(
		TRUE, 
		"",
		NULL,
		OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
		"Sprite Files (*.spr, *.s16,*.c16)|*.spr; *.s16; *.c16||",
		AfxGetMainWnd());

	if (fileDlg.DoModal() == IDOK)
	{
		POSITION pos = fileDlg.GetStartPosition();
		int converted = 0;

		while (pos != NULL)
		{
			CString filename = fileDlg.GetNextPathName(pos);

			int dot = filename.ReverseFind('.');
			CString newFilename = filename;
			if (dot != -1)
				newFilename = newFilename.Left(dot);
			newFilename += "." + extension;

			if (filename != newFilename)
			{
				++converted;
				LoadGeneral(filename);
				if(extension == "c16" )
				{
					m_pSprite->SaveC16(newFilename, m_b565);
				}
				else if(extension == "s16")
				{
					m_pSprite->SaveS16(newFilename, m_b565);
				}
				else
					ASSERT(false);
			}
		}
		if (converted > 0)
			OnNewDocument();
	}
}


void CSpriteBuilderDoc::OnNornCreatures3() 
{
	myC3Flag = !myC3Flag;
}

void CSpriteBuilderDoc::OnUpdateNornCreatures3(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(myC3Flag);	
}

void CSpriteBuilderDoc::OnNornBuildbodyparts() 
{
	BuildBodyParts("Norn", NornMale, NornFemale);
	BuildBodyParts("Grendel", GrendelMale, GrendelFemale);
	BuildBodyParts("Ettin", EttinMale, EttinFemale);
	BuildBodyParts("Geat", GeatMale, GeatFemale);

	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
}

void CSpriteBuilderDoc::BuildBodyParts(CString genusName, BYTE male, BYTE female) 
{
	CString pathName;

	char path[_MAX_PATH];
	_getcwd(path,_MAX_PATH);
	CString currentDir(path);
	CString origDir = currentDir;
	
	CFileFind finder;
	if(!finder.FindFile(genusName))
		return;

	currentDir += "\\" + genusName;
	_chdir(currentDir );
	
	CString wasWindowText;
	((CFrameWnd*)AfxGetMainWnd())->GetWindowText(wasWindowText);

	// for each variant folder create 
	ProcessFiles(currentDir,male,female); // magic numbers!!
	
	((CFrameWnd*)AfxGetMainWnd())->SetWindowText(wasWindowText);

	_chdir(origDir);
}

// checks the current directory for all files
// and add all non system folders to the given array
void CSpriteBuilderDoc::AddFilesNamesToArray(CStringArray& array)
{
	array.RemoveAll();

	char path[_MAX_PATH];

	_getcwd(path,_MAX_PATH);

	CFileFind finder;
	CString name;
	// put all folder names
	bool bWorking = finder.FindFile("*.*") == TRUE;
	while (bWorking)
	{
		bWorking = finder.FindNextFile() == TRUE;
		name = finder.GetFileName();
		name.MakeLower();

		if(name != "." && name != ".." && name.Right(4)!= ".exe")
			array.Add(name);
	}
}

// send the current directory
// the male /species indicator
// femnale /species indicator
void CSpriteBuilderDoc::ProcessFiles(CString& currentDir,BYTE male,BYTE female)
{
	CStringArray files;

	// put all folder names
	AddFilesNamesToArray(files);

	// for each folder we found go in and see what's there
	ProcessVariants(files, currentDir,male,female);
}

// look at the male and female files
void CSpriteBuilderDoc::ProcessVariants(CStringArray& array,
										   CString& currentDir,
										   BYTE male,BYTE female)
{
	// make up a name to call the finished file
	CString fileName;
	
	// use the correct extension
	fileName = "****.c16";

	CStringArray genders;

	CString nextDir;

	//for each variant
	for(int i =0; i< array.GetSize(); i++)
	{
		// we are moving down a folder
		nextDir = currentDir + "\\"+  array.GetAt(i);
		_chdir(nextDir);

		// store the variant as part of the new file name
		CString variant = array.GetAt(i).Right(1);
		fileName.SetAt(3,variant[0]);
		//look for the sex files tee hee!
		// put all folder names
		AddFilesNamesToArray(genders);
		//do each gender file found
		ProcessGenders(genders,nextDir,fileName,male,female);
	}

}

void CSpriteBuilderDoc::ProcessGenders(CStringArray& genders,
									   CString& currentDir,
									   CString& fileName,
									   BYTE male,BYTE female)
{
	CStringArray ages;

	CString nextDir;

	for(int i = 0; i<genders.GetSize();i++)
	{
		// we are moving down a folder
		nextDir = currentDir +  "\\" + genders.GetAt(i);
		_chdir(nextDir);

		CString gender;
	
		if(genders.GetAt(i) == "male")
		{
			gender.Format("%d",male);
			fileName.SetAt(1,gender[0]);
		}
		else
		{
			gender.Format("%d",female);
			fileName.SetAt(1,gender[0]);
		}

		// put all folder names
		AddFilesNamesToArray(ages);
		ProcessAges(ages,nextDir,fileName);
	}
}

bool DeleteDirectory( std::string directory )
{
	SHFILEOPSTRUCT fileOp;

	//this struct requires a *double* zero terminated string!
	char *buffer = new char[ directory.size() + 2 ];
	directory.copy( buffer, directory.size() );
	buffer[ directory.size() ] = 0;
	buffer[ directory.size() + 1 ] = 0;

	fileOp.hwnd = 0;
	fileOp.wFunc = FO_DELETE;
	fileOp.pFrom = buffer;
	fileOp.pTo = 0;
	fileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;

	SHFileOperation( &fileOp );

	delete [] buffer;
	return true;
}

void CSpriteBuilderDoc::ProcessAges(CStringArray& ages,CString& currentDir,CString& fileName)
{

	CString nextDir;
	CString age;

	// for each age folder
	for(int i = 0; i<ages.GetSize();i++)
	{
		// we are moving down a folder
		nextDir = currentDir +  "\\" + ages.GetAt(i);
		_chdir(nextDir);

		// fill in the age part of the file name
		age = ages.GetAt(i).Right(1);
		fileName.SetAt(2,age[0]);
		DeleteDirectory((LPCTSTR)CString(nextDir + "\\" + "Sprites"));
		_mkdir("Sprites");

		// now we should have our complete set of bitmaps
		CString progress;
		if(myC3Flag)
		{
			BuildC3BodyParts(fileName,nextDir);
		}
		else
		{
			BuildCAdvBodyParts(fileName,nextDir);
		}
	}
}

// build each body part proper
void CSpriteBuilderDoc::BuildCAdvBodyParts(CString& filename,CString& currentDir)
{


	const int NUMPARTS = 17;// really 17 parts but we might as well
							//create the left and right ear,
							// hair and body overlays here too
	int x;//= 200;
	// for each body part create this many sprites
	const int Parts[NUMPARTS]={x=384, //heads//head overlays
					x+=16, //lthigh
					x+=16,//lshin
					x+=16,//lfoot
					x+=16,//rthigh
					x+=16,//rshin
					x+=16,//rfoot
					x+=16,//lupper arm
					x+=16,//llower arm
					x+=16,//rupper arm
					x+=16,//rlower arm
					x+=64,//body, // body overlays
					x+=64,// left ear, // left ear overlays
					x+=64,//right ear//right ear overlays
					x+=48, //hair, //hair overlays
					x+=16,//tailroot
					x+=16 //tailtip
					};// add ears and hair!!!
			
	const char bodyid[]={'a', //head//head overlays
					'c',// order as above
					'd',
					'e',
					'f',
					'g',
					'h',
					'i',
					'j',
					'k',
					'l',
					'b' ,//body		// body overlay
					'o',//left ear,//left ear overlay
					'p',//rightear,//rightear overlay
					'q',//hair,//hair overlay
					'm',//tail root
					'n'};//tail tip
	CString thisBodyPartFile("Sprites\\" + filename);
	int part=0;
	// for each individual body part
	for(int i = 0; i< NUMPARTS; i++)
	{
		OnNewDocument();

		// fill in the final part of the filename
		thisBodyPartFile = "Sprites\\" + filename;
		thisBodyPartFile.SetAt(8,bodyid[i]);

		CString progress;
		progress.Format("Working... %d %d", thisBodyPartFile, currentDir);
		((CFrameWnd*)AfxGetMainWnd())->SetWindowText(progress);

		CString temp;
		
		int index = 0;
		for(; part<Parts[i];part++)
		{
			if(part >= 608 && part <= 623)
			{
				static int tempPart = 624;
				if(part == 608)
				{
					tempPart = 624;
				}

				temp.Format("\\CA%04d.bmp",tempPart);
				tempPart++;
			}
			else if(part >=624 && part <=639)
			{
				static int tempPart = 608;
				if(part ==624)
				{
					tempPart = 608;
				}

				// findout the bitmap name
				temp.Format("\\CA%04d.bmp",tempPart);
				tempPart++;
			}
			else if(part >= 672 && part <= 687)
			{
				static int tempPart = 688;
				if(part ==672)
				{
					tempPart = 688;
				}
				// findout the bitmap name
				temp.Format("\\CA%04d.bmp",tempPart);
				tempPart++;
			}
			else if(part >=688 && part <= 703)
			{
				static int tempPart = 672;
				if(part ==688)
				{
					tempPart = 672;
				}
				// findout the bitmap name
				temp.Format("\\CA%04d.bmp",tempPart);
				tempPart++;
			}
			else
			{
				temp.Format("\\CA%04d.bmp",part);
			}
		//	OnInsertBitmap();
			AddBodyPart(index,currentDir + temp );
			index++;
		}

		// now save the file
		m_pSprite->SaveC16(thisBodyPartFile, m_b565);
	}
}

// build each body part proper
void CSpriteBuilderDoc::BuildC3BodyParts(CString& filename,CString& currentDir)
{
	const int NUMPARTS = 15;// really 17 parts but we might as well
							// create the left and right ear,
							// hair and body overlays here too
	int x;//= 200;
	// for each body part create this many sprites
	const int Parts[NUMPARTS]={x=192, //heads + overlays
						//	x+=176,//head overlays
							x+=96, //face overlays
					x+=16, //lthigh
					x+=16,//lshin
					x+=16,//lfoot
					x+=16,//rthigh
					x+=16,//rshin
					x+=16,//rfoot
					x+=16,//lupper arm
					x+=16,//llower arm
					x+=16,//rupper arm
					x+=16,//rlower arm
					x+=64,//body + overlays
				//	x+=48, // body overlays
				//	x+=16,// left ear
				//	x+=48, // left ear overlays
				//	x+=16,//right ear
				//	x+=48,//right ear overlays
				//	x+=16, //hair
				//	x+=32, //hair overlays
					x+=16,//tailroot
					x+=16 //tailtip
					};// add ears and hair!!!
			
	const char bodyid[]={'a', //head
					//	'0',//head overlays
						'0',//face overlays
					'c',// order as above
					'd',
					'e',
					'f',
					'g',
					'h',
					'i',
					'j',
					'k',
					'l',
					'b' ,//body
				//	'1',		// body overlay
				//	'o',//left ear
				//	'2',//left ear overlay
				//	'p',//rightear
				//	'3',//rightear overlay
				//	'q',//hair
				//	'4',//hair overlay
					'm',//tail root
					'n'};//tail tip


	const int NUM_BLANK_PARTS = 3;

	const int BlankParts[NUM_BLANK_PARTS] = {x=16, //left ear
												x+=16, //right ear
												x+=16}; //hair

	const char Blankbodyid[] = {'o', //head
						'p',//head overlays
						'q'};// order as above
				

	CString thisBodyPartFile("Sprites\\" + filename);
	int part=0;
	// for each individual body part
	for(int i = 0; i< NUMPARTS; i++)
	{
		OnNewDocument();

		// fill in the final part of the filename
		thisBodyPartFile = "Sprites\\" + filename;
		thisBodyPartFile.SetAt(8,bodyid[i]);

		CString progress;
		progress.Format("Working... %s %s", thisBodyPartFile, currentDir);
		((CFrameWnd*)AfxGetMainWnd())->SetWindowText(progress);
		Sleep(1);

		int index = 0;
		bool partUpdated = false;
		for(; part<Parts[i];part++)
		{
			CString sourceFile;
			sourceFile.Format("\\CA%04d.bmp",part);

			AddBodyPart(index, currentDir + sourceFile );
			index++;
		}
	
		// now save the file
		m_pSprite->SaveC16(thisBodyPartFile, m_b565);
	}


/*	for(i = 0; i< NUM_BLANK_PARTS; i++)
	{
		OnNewDocument();

		// fill in the final part of the filename
		thisBodyPartFile = "Sprites\\" + filename;
		thisBodyPartFile.SetAt(8,Blankbodyid[i]);

		CString temp;
	
		int index = 0;
		bool partUpdated = false;
		for(; part<Parts[i];part++)
		{
			temp = "CA999.bmp";

			AddBodyPart(index,currentDir + temp );
			index++;
		}
	
	// now save the file
		m_pSprite->SaveC16(thisBodyPartFile, m_b565);
	}*/
}

void CSpriteBuilderDoc::AddBodyPart(int iIndex,CString bitmapName)
{
	// Insert new bitmap.
	CFIBitmap* pBitmap = new CFIBitmap(bitmapName);
	m_pSprite->InsertAt(iIndex, pBitmap);

	// Force recalculation & redraw of view.
//	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
//	CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
}


void CSpriteBuilderDoc::OnEditImportall4digit() 
{
	// TODO: Add your command handler code here
//	temp.Format("\\CA%04d.bmp",tempPart);

	CFileDialog FileDlg(
		TRUE, 
		"bmp",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"Initial Bitmap Files (*0000.bmp)|*0000.bmp||",
		AfxGetMainWnd());

	if (FileDlg.DoModal() == IDOK)
	{
		CWaitCursor wc;

		CString sPathName = FileDlg.GetPathName();
		CString sExt = sPathName.Right(8);
		sExt.MakeLower();

		if (sExt == "0000.bmp")
		{
			CString sNameStub = sPathName.Left(sPathName.GetLength() - 8);

			// Store state before change.
			StoreForUndo();

			CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
			CSpriteBuilderView* pView = (CSpriteBuilderView*)pFrameWnd->GetActiveView();
			int iIndex = pView->SelectedBitmap();
			if (iIndex < 0)
				iIndex = m_RectArray.GetSize();

			int i = 0;
			while(true)
			{
				CString sName;
				sName.Format(sNameStub + "%04d.bmp", i);
				// see if file is there
				if (GetFileAttributes(sName) == 0xFFFFFFFF)
					break;

				// Insert new bitmap.
				CFIBitmap* pBitmap = new CFIBitmap(sName);
				m_pSprite->InsertAt(iIndex, pBitmap);
				m_RectArray.InsertAt(iIndex, new CRect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT));

				++i;
				++iIndex;
			}

			// Force recalculation & redraw of view.
			pView->Invalidate();
		}
		else
			AfxMessageBox("Please choose the first file, whose name ends -0.bmp");
	}	
}

void CSpriteBuilderDoc::OnEditCostumes() 
{
	CString sPathName;

	CFileDialog FileDlg(
		TRUE, 
		"bmp",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"Initial S16 Files (*.c16)|*.c16||",
		AfxGetMainWnd());

	if (FileDlg.DoModal() == IDOK)
	{
		sPathName = FileDlg.GetPathName();

	//	CString sNameStub = sPathName.Left(sPathName.GetLength() - 14);

	}
	// TODO: Add your command handler code here
	OverlayFileSelector dlg ;

//	dlg.SetDoc(this);

	CString file;

	if(dlg.DoModal() == IDOK)
	{
		SetSelection(dlg.GetSelection());
		for(int i = 0; i < mySelection.GetSize(); i++)
		{
			char a = mySelection.GetAt(i).GetAt(0);
			sPathName.SetAt(sPathName.GetLength() - 8,a);
			LoadGeneral(sPathName, false);

		//	OnNewDocument();

			int bodypart = 0;

			if(mySelection.GetAt(i) == "a")
			{
				bodypart = 0;
			//	bodypart = 816;
			}

			if(mySelection.GetAt(i) == "b")
			{
				bodypart= 544;
			//	bodypart= 560;
			}

			if(mySelection.GetAt(i) == "c")
			{
				bodypart = 384;
			}
				
			if(mySelection.GetAt(i) == "d")
			{
				bodypart = 400;
			}
	
			if(mySelection.GetAt(i) == "e")
			{
				bodypart = 416;
			}
			
			if(mySelection.GetAt(i) == "f")
			{
				bodypart = 432;
			}
			
			if(mySelection.GetAt(i) == "g")
			{
				bodypart= 448;
			}
			
			if(mySelection.GetAt(i) == "h")
			{
				bodypart = 464;
			}
			
			if(mySelection.GetAt(i) == "i")
			{
				bodypart= 480;
			}
			
			if(mySelection.GetAt(i) == "j")
			{
				bodypart = 496;
			}
			
			if(mySelection.GetAt(i) == "k")
			{
				bodypart = 512;
			}
			
			if(mySelection.GetAt(i) == "l")
			{
				bodypart = 528;
			}
			
			if(mySelection.GetAt(i) == "m")
			{
				bodypart = 784;
			}
													
			if(mySelection.GetAt(i) == "n")
			{
				bodypart = 800;
			}
															
			if(mySelection.GetAt(i) == "o")
			{
			//	bodypart = 608;
				bodypart =0;
			}
																
			if(mySelection.GetAt(i) == "p")
			{
			//	bodypart= 624;
				bodypart = 0;
			}	
			
			if(mySelection.GetAt(i) == "q")
			{
			//	bodypart = 0;
				bodypart = 736;
			//	bodypart = 752;
			}

			CString sNameStub = "SPACE_S";
			int iIndex = m_pSprite->NumberOfBitmaps();
//	int iIndex = 272;
			int stoppoint = bodypart+16;
		//		int stoppoint = bodypart+192;
			for(; bodypart < stoppoint; ++bodypart)
			{
				CString sName;
				sName.Format(sNameStub + "%03d.bmp", bodypart);
				// see if file is there
				if (GetFileAttributes(sName) == 0xFFFFFFFF)
					break;

				// Insert new bitmap.
				CFIBitmap* pBitmap = new CFIBitmap(sName);
				m_pSprite->InsertAt(iIndex, pBitmap);
				m_RectArray.InsertAt(iIndex, new CRect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT));

			//	++i;
				++iIndex;
			
			}
			m_pSprite->SaveC16(sPathName, m_b565);	
		}
	}



	

}

void CSpriteBuilderDoc::SaveAniStrip(const char* pFileName)
{
// Create a cut sheet from all the sprites

	CWaitCursor wc;

	// Guess a width to make the new sprite...
	// find maximum width of sprites in file
	int widthToUse = -1;
	int n = m_RectArray.GetSize();
	if (n < 1)
		return;

	// all widths should be the same for each bitmap
	for (int i = 0; i < n; ++i)
	{
		CRect rect = *m_RectArray[i];
		if (rect.Width() > widthToUse)
		{
			if(widthToUse != -1)
			{
				AfxMessageBox("All bitmaps must be exactly the same width for this to work!");
			}
			widthToUse = rect.Width();
			
		}
	}

	// all heights should be the same for each bitmap
	int tempHeight =-1;
	for (i = 0; i < n; ++i)
	{
		CRect rect = *m_RectArray[i];
		if (rect.Height() > tempHeight)
		{
			if(tempHeight != -1)
			{
				AfxMessageBox("All bitmaps must be exactly the same height for this to work!");
			}
			tempHeight = rect.Height();
			
		}
	}

//	const int margin = 5;
	const int margin = 0;
	// extra margin
//	widthToUse += margin * 2;
	// half everything until we have a width less than 640
	int maxPixelWidth = 1000;
	int tempPixelWidth = n* widthToUse;
	int div =2;
	while(tempPixelWidth > maxPixelWidth)
	{
		if((n*widthToUse)%div == 0)
		tempPixelWidth = (n* widthToUse)/div;

		div++;
	}

	widthToUse = tempPixelWidth;
	// minimum
//	if (widthToUse < 640)
	//	widthToUse = 640;

	// Calculate the height
	int heightToUse;
	{
		int x = margin;
		int y = margin;
	//	int maxY = 1;
		int maxY = 0;
		for (int i = 0; i < n; ++i)
		{
			CRect rect = *m_RectArray[i];
			if (x + rect.Width() + margin > widthToUse)
			{
				ASSERT(x > margin);
				y += maxY + margin;
				x = margin;
				maxY = 1;
			}
			// bitmap slotted here
			x += rect.Width() + margin;
			if (rect.Height() > maxY)
				maxY = rect.Height();
		}	

		y += maxY;
		y += margin;
		heightToUse = y;
	}

	int iBytesNeeded = widthToUse * heightToUse * 2; // *2 => 16bit.
	int len = iBytesNeeded / 2;
	WORD* wData = new WORD[len];
	byte* pData = (byte*)wData;
	ASSERT(pData);

	// decide on background colour
	std::map< int, bool > usedColours;
	{
		for (int i = 0; i < n; ++i)
		{
			CRect rect = *m_RectArray[i];
			CFIBitmap* bitmap = (*m_pSprite)[i];
			BYTE* bytePixelData = bitmap->PixelData();
			int byteLineWidth = bitmap->BytesPerLine();
			for (int r = 0; r < rect.Width(); ++r)
			{
				for (int s = 0; s < rect.Height(); ++s)
				{
					int blue = bytePixelData[r * 3 + byteLineWidth * s];
					int green = bytePixelData[r * 3 + 1 + byteLineWidth * s];
					int red = bytePixelData[r * 3 + 2 + byteLineWidth * s];
					int data;
					RGB_TO_565(red, green, blue, data);
					usedColours[data] = true;
				}
			}
		}
	}
	int maxSize = 1 << 16;
	int used = usedColours.size();
	ASSERT(used <= maxSize);
	if (used == maxSize)
	{
		AfxMessageBox("Your sprites contain every available colour - all 65536!\nAt least one needs to be free for the background of the sprite sheet!");
		return;
	}
	int backgroundColour = -1;
	for (int tries = 0; tries < 100000; ++tries)
	{
		int data = ((double)rand() / (double)RAND_MAX) * maxSize;
		if (data >= maxSize)
			data = maxSize - 1;
		if (data < 0)
			data = 0;
		if (usedColours.find(data) == usedColours.end())
		{
			backgroundColour = data;
			break;
		}
	}
	if (backgroundColour == -1)
	{
		for (int data = 0; data < maxSize; ++data)
		{
			if (usedColours.find(data) == usedColours.end())
			{
				backgroundColour = data;
				break;
			}
		}
	}

	ASSERT(backgroundColour >= 0 && backgroundColour < maxSize);

	// fill in background
	{
		for (int i = 0; i < len; ++i)
		{
			//wData[i] = backgroundColour;
			// let's fill it in transparent
			wData[i] = 0;
		}
	}

	{
		int x = margin;
		int y = margin;
		//int maxY = 1;
		int maxY = 0;
		for (int i = 0; i < n; ++i)
		{
			CRect rect = *m_RectArray[i];
			if (x + rect.Width() + margin > widthToUse)
			{
				ASSERT(x > margin);
				y += maxY + margin;
				x = margin;
				maxY = 1;
			}

			CFIBitmap* bitmap = (*m_pSprite)[i];
			BYTE* bytePixelData = bitmap->PixelData();
			int byteLineWidth = bitmap->BytesPerLine();
			for (int r = 0; r < rect.Width(); ++r)
			{
				for (int s = 0; s < rect.Height(); ++s)
				{
					int blue = bytePixelData[r * 3 + byteLineWidth * s];
					int green = bytePixelData[r * 3 + 1 + byteLineWidth * s];
					int red = bytePixelData[r * 3 + 2 + byteLineWidth * s];

					int data;
					RGB_TO_565(red, green, blue, data);
					ASSERT(data != backgroundColour); // double check we found a decent background colour before!
					wData[x + r + (y + s) * widthToUse] = data;
				}
			}

			x += rect.Width() + margin;
			if (rect.Height() > maxY)
				maxY = rect.Height();
		}	
	}


	CFIBitmap bitmap(
		widthToUse, 
		-heightToUse, // SPR's are top-down bitmaps.
		NULL,
		FORMAT16_565,
		pData);

	
	bitmap.SaveBMP(pFileName);

// Create a header file to store the images block width, height and
// number of images

	CString headerFile = pFileName;
	CString C16Filename = pFileName;

	int x = headerFile.ReverseFind('.');

	if(x != -1)
	{
		headerFile = headerFile.Left(x+1);
		headerFile+= "hed";
			
		C16Filename = C16Filename.Left(x+1);
		C16Filename+="c16";
	}
	else
	{
		headerFile+= ".hed";
		C16Filename+=".c16";
	}

	int blockWidth = m_RectArray[0]->Width();
	int blockHeight = m_RectArray[0]->Height();

	CFile File(headerFile, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);
	File.Write(&blockWidth,sizeof(int));
	File.Write(&blockHeight,sizeof(int));
	File.Write(&n,sizeof(int));
	File.Close();

	// now save as C16 as well
	OnNewDocument();


	m_pSprite->InsertAt(0, &bitmap);
	m_RectArray.InsertAt(0, new CRect(0, 0, blockWidth, blockHeight));

	m_pSprite->SaveC16(C16Filename, m_b565);

	m_pSprite->RemoveAt(0);

	delete[] pData;
}

void CSpriteBuilderDoc::OnUpdateAnistripButton(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetCheck(myAniStripFlag);
//	pCmdUI->Enable(true);
}

void CSpriteBuilderDoc::OnAnistripButton() 
{
	// TODO: Add your command handler code here
	myAniStripFlag = ! myAniStripFlag;
}

void CSpriteBuilderDoc::OnRemoveMirrors() 
{

	// TODO: Add your command handler code here
	// get the bitmap first

	CString sPathName;
	CFileDialog FileDlg2(
		TRUE, 
		"bmp",
		NULL,
		 OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"Initial Bitmap Files (*.bmp)|*.bmp||",
		AfxGetMainWnd());

	if (FileDlg2.DoModal() == IDOK)
	{
		sPathName = FileDlg2.GetPathName();

	}

	// then get all the files to process
	CString sFilePathName;

		CFileDialog FileDlg(
		TRUE, 
		"bmp",
		NULL,
		OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"Initial C16 Files (*.c16)|*.c16||",
		AfxGetMainWnd());

	char namebuffer[4096] = {' '};

	FileDlg.m_ofn.lpstrFile = namebuffer;
	FileDlg.m_ofn.nMaxFile = 4096;

	if (FileDlg.DoModal() == IDOK)
	{
		sFilePathName = FileDlg.GetPathName();
	


		POSITION pos = FileDlg.GetStartPosition( );
	


		while(pos)
		{

			sFilePathName = FileDlg.GetNextPathName(pos);

			LoadGeneral(sFilePathName, false);

			int numBitmaps = m_pSprite->NumberOfBitmaps();

			int numberOfSets = numBitmaps/16;

			for(int i=0; i < numberOfSets; i++)
			{
				for(int j=4; j<8; j++)
				{
					int index = (i * 16) + j;
					CFIBitmap* pTmp = (*m_pSprite)[index];
					m_pSprite->RemoveAt(index);
					delete pTmp;

					// Insert new bitmap.
					CFIBitmap* pBitmap = new CFIBitmap(sPathName);
					m_pSprite->InsertAt(index, pBitmap);
				}

			}

			m_pSprite->SaveC16(sFilePathName, m_b565);	
			
		}
	}
}


