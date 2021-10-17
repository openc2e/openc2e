#if !defined(AFX_CREATUREPICTURECTL_H__11DF2DE0_A937_11D2_9D30_0090271EEABE__INCLUDED_)
#define AFX_CREATUREPICTURECTL_H__11DF2DE0_A937_11D2_9D30_0090271EEABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CreatureDisplayer.h"
#include "Appearance.h"
#include <list>
#include "handle.h"

// CreaturePictureCtl.h : Declaration of the CCreaturePictureCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CCreaturePictureCtrl : See CreaturePictureCtl.cpp for implementation.

class CDisplayerMapEntry
{
public:
	CDisplayerMapEntry() : m_Displayer( 0 ) {}

	CAppearance m_Appearance;
	handle< CCreatureDisplayer > m_Displayer;
	int m_Count;
};


class CCreaturePictureCtrl : public COleControl
{
	DECLARE_DYNCREATE(CCreaturePictureCtrl)

// Constructor
public:
	CCreaturePictureCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreaturePictureCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CCreaturePictureCtrl();

	DECLARE_OLECREATE_EX(CCreaturePictureCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CCreaturePictureCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CCreaturePictureCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CCreaturePictureCtrl)		// Type name and misc status

	// Subclassed control support
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

// Message maps
	//{{AFX_MSG(CCreaturePictureCtrl)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CCreaturePictureCtrl)
	float m_scale;
	afx_msg void OnScaleChanged();
	BOOL m_friction;
	afx_msg void OnFrictionChanged();
	double m_angle;
	afx_msg void OnAngleChanged();
	afx_msg short GetPoseCount();
	afx_msg void SetPoseCount(short nNewValue);
	afx_msg void SetAppearance(short HeadGenus, short HeadVariant, short BodyGenus, short BodyVariant, short LegsGenus, short LegsVariant, short ArmsGenus, short ArmsVariant, short TailGenus, short TailsVariant, short EarGenus, short EarVariant, short HairGenus, short HairVariant, short Sex, short Age);
	afx_msg BSTR GetPoseString(short Index);
	afx_msg void SetPoseString(short Index, LPCTSTR lpszNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CCreaturePictureCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CCreaturePictureCtrl)
	dispidPoseCount = 4L,
	dispidScale = 1L,
	dispidFriction = 2L,
	dispidAngle = 3L,
	dispidPoseString = 6L,
	dispidSetAppearance = 5L,
	//}}AFX_DISP_ID
	};
private:
	CCreatureDisplayer *m_Displayer;
	short m_PoseCount;
	CString m_PoseString[8];
	CString m_ImageDir;
	CString m_BodyDataDir;
	bool m_Prepared;
	void Prepare();
	int m_Timer;
	CAnimateData m_AD;
	CAppearance m_Appearance;
	static std::list< CDisplayerMapEntry > m_DisplayerMap;
	CCreatureDisplayer *GetDisplayer( CAppearance const &appearance );
	static void CCreaturePictureCtrl::UnlinkDisplayer( CCreatureDisplayer *pCD );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATUREPICTURECTL_H__11DF2DE0_A937_11D2_9D30_0090271EEABE__INCLUDED)

