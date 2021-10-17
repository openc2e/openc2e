// CntrItem.h : interface of the CSpriteBuilderCntrItem class
//

#if !defined(AFX_CNTRITEM_H__A049CFCE_51D9_11D1_8791_0060B07BFA18__INCLUDED_)
#define AFX_CNTRITEM_H__A049CFCE_51D9_11D1_8791_0060B07BFA18__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSpriteBuilderDoc;
class CSpriteBuilderView;

class CSpriteBuilderCntrItem : public COleClientItem
{
	DECLARE_SERIAL(CSpriteBuilderCntrItem)

// Constructors
public:
	CSpriteBuilderCntrItem(CSpriteBuilderDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CSpriteBuilderDoc* GetDocument()
		{ return (CSpriteBuilderDoc*)COleClientItem::GetDocument(); }
	CSpriteBuilderView* GetActiveView()
		{ return (CSpriteBuilderView*)COleClientItem::GetActiveView(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpriteBuilderCntrItem)
	public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();
	protected:
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	//}}AFX_VIRTUAL

// Implementation
public:
	~CSpriteBuilderCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CNTRITEM_H__A049CFCE_51D9_11D1_8791_0060B07BFA18__INCLUDED_)

