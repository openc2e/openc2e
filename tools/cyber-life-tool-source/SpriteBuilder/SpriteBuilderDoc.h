// SpriteBuilderDoc.h : interface of the CSpriteBuilderDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPRITEBUILDERDOC_H__A049CFCA_51D9_11D1_8791_0060B07BFA18__INCLUDED_)
#define AFX_SPRITEBUILDERDOC_H__A049CFCA_51D9_11D1_8791_0060B07BFA18__INCLUDED_

#ifdef _MSC_VER
#pragma warning(disable:4786 4503)
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Sprite.h"

// when building variant file names we need this number
// that represents both the gender and the species 
enum GenderSpeciesCombo{NornMale =0,
					GrendelMale,
					EttinMale,
					GeatMale,
					NornFemale,
					GrendelFemale,
					EttinFemale,
					GeatFemale
					};

typedef CTypedPtrArray<CPtrArray, CRect*> CRectArray;
typedef CTypedPtrArray<CPtrArray, CSprite*> SpriteArray;
typedef CArray<int, int> IntArray;

#define MAX_UNDO	20

class CSpriteBuilderDoc : public COleDocument
{
protected: // create from serialization only
	CSpriteBuilderDoc();
	DECLARE_DYNCREATE(CSpriteBuilderDoc)

// Attributes
public:
	CSprite* m_pSprite;
	CRectArray m_RectArray;
	BOOL m_b565;
	bool m_isZoomed;

// Operations
public:
	void StoreForUndo();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpriteBuilderDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSpriteBuilderDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void LoadGeneral(CString sPathName, BOOL bOldS16Format = FALSE);
	void DoCopy(CFIBitmap* pBitmap);
	void BuildBodyParts(CString genusName, BYTE male, BYTE female);

protected:
	void CleanUp();

// Generated message map functions
protected:
	//{{AFX_MSG(CSpriteBuilderDoc)
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnExportBitmap();
	afx_msg void OnInsertBitmap();
	afx_msg void OnReplaceBitmap();
	afx_msg void OnUpdateReplaceBitmap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInsertBitmap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExportBitmap(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditInitiatecut();
	afx_msg void OnUpdateEditInitiatecut(CCmdUI* pCmdUI);
	afx_msg void OnEditZoomcutter2x();
	afx_msg void OnUpdateEditZoomcutter2x(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAutomaticUncut(CCmdUI* pCmdUI);
	afx_msg void OnAutomaticUncut();
	afx_msg void OnEditExportall();
	afx_msg void OnUpdateEditExportall(CCmdUI* pCmdUI);
	afx_msg void OnEditImportall();
	afx_msg void OnFileBatchconvertToc16();
	afx_msg void OnFileBatchconvertTos16();
	afx_msg void OnNornBuildbodyparts();
	afx_msg void OnNornCreatures3();
	afx_msg void OnUpdateNornCreatures3(CCmdUI* pCmdUI);
	afx_msg void OnEditImportall4digit();
	afx_msg void OnEditCostumes();
	afx_msg void OnRemoveMirrors();
	afx_msg void OnAnistripButton();
	afx_msg void OnUpdateAnistripButton(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnEditDelete();
	afx_msg void OnEditPasteOver();
	afx_msg void OnUpdateEditPasteOver(CCmdUI* pCmdUI);
	afx_msg void OnEditInitiateAutocut();

	afx_msg void OnFileMruFile1(UINT uiID);
	DECLARE_MESSAGE_MAP()

	void DoPaste(int iIndex);
	void DoDelete(bool undo);
	void BatchConvertTo(CString extension);

	void ProcessFiles(CString& currentDir,BYTE male,BYTE female);
	void AddFilesNamesToArray(CStringArray& array);
	void ProcessVariants(CStringArray& array,
										   CString& currentDir,
										   BYTE male,BYTE female);
	void ProcessGenders(CStringArray& genders,
									   CString& currentDir,
									   CString& fileName,
									   BYTE male,BYTE female);
	void ProcessAges(CStringArray& ages,CString& currentDir,CString& fileName);

	void BuildCAdvBodyParts(CString& filename,CString& currentDir);
	void BuildC3BodyParts(CString& filename,CString& currentDir);
	void AddBodyPart(int iIndex,CString bitmapName);

	void SetSelection(CStringArray& array){mySelection.RemoveAll(),
											mySelection.Append(array);}

	void SaveAniStrip(const char* pFileName);

private:
	SpriteArray m_UndoArray;
	IntArray m_UndoSelectedBitmapArray;
	bool myC3Flag;
	bool myAniStripFlag;
	CStringArray mySelection;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPRITEBUILDERDOC_H__A049CFCA_51D9_11D1_8791_0060B07BFA18__INCLUDED_)

