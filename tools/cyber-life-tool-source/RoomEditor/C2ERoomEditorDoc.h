// C2E Room EditorDoc.h : interface of the CC2ERoomEditorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_C2EROOMEDITORDOC_H__A50773AD_B9D4_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_C2EROOMEDITORDOC_H__A50773AD_B9D4_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "World.h"
#include "Action.h"
#include "PropertyType.h"
#include "CARates.h"
#include "Cheese.h"

#include<stack>
#include<vector>

const int NUM_CA_PROPERTIES = 20;
const int NUM_ROOM_TYPES = 16;

class CC2ERoomEditorDoc : public CDocument
{
protected: // create from serialization only
	CC2ERoomEditorDoc();
	DECLARE_DYNCREATE(CC2ERoomEditorDoc)

// Attributes
public:
	CWorld *GetWorld();
	bool Execute( HAction action );

	int AddMetaroom( CString const &filename, CRect const &rect );
	bool AddMetaroom( int index, HMetaroom metaRoom );

	int AddRoom( CPoint *points );
	bool AddRoom( int index, const HRoom &room );
	bool RemoveRoom( int index );
	bool RemoveMetaroom( int handle );

	std::vector< CPropertyType > const &GetPropertyTypes() const {return m_PropertyTypes;}
	std::vector< CPropertyType > const &GetDoorPropertyTypes() const {return m_DoorPropertyTypes;}

	CPropertyType const &GetPropertyType( int index ) const {return m_PropertyTypes[index];}
	void SetPropertyType( int index, CPropertyType const &type ) {m_PropertyTypes[index] = type;}
// Operations

// Persistance
	void Write( std::ostream &stream ) const;
	void Read( std::istream &stream );

// Cheese
	void AddCheese( CCheese const &cheese ) { m_Cheeses.push_back( cheese ); }
	std::vector< CCheese > const &GetCheeses() const { return m_Cheeses; }

	void StepCA() { OnToolsStepca(); }

	void SaveToGame(CGame &game, bool addon);

public:
	void DoLoadFromGame();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CC2ERoomEditorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SelectRoomWronguns( int Perm, int Height, std::set< int > &selection );
	virtual ~CC2ERoomEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CC2ERoomEditorDoc)
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnFilePropertyTypes();
	afx_msg void OnFileSaveToGame();
	afx_msg void OnFileSaveToGameAddon();
	afx_msg void OnFileWorldSize();
	afx_msg void OnToolsSetMetaroomInGame();
	afx_msg void OnFileCARates();
	afx_msg void OnToolsStepca();
	afx_msg void OnFileSaveToBootstrapWorld();
	afx_msg void OnFileSaveToBootstrapAddon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CWorld m_World;
	std::stack< HAction > m_Undo;
	std::stack< HAction > m_Redo;
	std::vector< CPropertyType > m_PropertyTypes;
	std::vector< CPropertyType > m_DoorPropertyTypes;
	std::vector< std::vector<CCARates> > m_CARates;
	std::vector< CCheese > m_Cheeses;
//	CCARates m_CARates[16][16];
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_C2EROOMEDITORDOC_H__A50773AD_B9D4_11D2_9D30_0090271EEABE__INCLUDED_)

