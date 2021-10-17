#ifndef ACTION_H_
#define ACTION_H_

#include <set>

#include "Metaroom.h"
#include "resource.h"

class CWorld;
class CC2ERoomEditorDoc;

// CAction is the base class for all actions performed on the document.
// The document will keep stacks of actions for undo and redo.
//
// the Do() and Undo() methods return true for success and false for failure.
// It is assumed that Do() will always be called before Undo() on any object.
// if Do fails then Undo will not be called for that action

class CAction {
public:
	// Enumeration of hint types for updating views
	enum{
		DESELECT_METAROOM = 1,
		DESELECT_ROOM,
		REFLECT_ROOM_PROPERTIES
	};
	// ---------------------------------------------------------------------
	// Method:		Destructor
	// Arguments:	None
	// Returns:		None
	// Description:	Does nothing - but must be virtual
	// ---------------------------------------------------------------------
	virtual ~CAction() {}

	// ---------------------------------------------------------------------
	// Method:		Do
	// Arguments:	doc - Document for action to be performed on
	// Returns:		true - Action performed successfully.
	//				false - Action failed
	// Description:	Does the action - may not be called more than once
	//				unless Undo() is called inbetween.
	// ---------------------------------------------------------------------
	virtual bool Do( CC2ERoomEditorDoc *doc ) {return false;}

	// ---------------------------------------------------------------------
	// Method:		Undo
	// Arguments:	doc - Document for action to be performed on
	// Returns:		true - Undo performed successfully.
	//				false - Undo failed
	// Description:	Undoes the action - may only be called after Do() has
	//				succeded
	// ---------------------------------------------------------------------
	virtual bool Undo( CC2ERoomEditorDoc *doc ) {return false;}

	// ---------------------------------------------------------------------
	// Method:		Describe
	// Arguments:	None
	// Returns:		Resource ID of string describing action
	// Description:	The string will be put on the menu
	// ---------------------------------------------------------------------
	virtual UINT Describe() const;
};

typedef handle<CAction> HAction;

class CActionMultiAction : public CAction {
public:
	CActionMultiAction( std::vector<HAction> const &actions, UINT description = IDS_ACTION_MULTIPLE );

	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	std::vector< HAction > m_Actions;
	UINT m_Description;
};

class CActionAddMetaroom : public CAction {
public:
	CActionAddMetaroom( CString const &bitmapName, CRect const &rect );

	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	CString m_BitmapName;
	CRect m_Rect;
	int m_Handle;
};

class CActionAddRoom : public CAction {
public:
	CActionAddRoom( CPoint *points );

	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	CPoint m_Points[4];
	int m_Handle;
};

class CActionMoveRoom : public CAction {
public:
	CActionMoveRoom( std::set< int > const &selection, CPoint const &move, int mode, CWorld const &world, int tolerance );

	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	std::vector< std::pair< int, CRoomShape > > m_Positions;
};

class CActionRemoveRoom : public CAction {
public:
	CActionRemoveRoom( std::set< int > const &selection );

	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	std::vector< std::pair< int, HRoom > > m_Rooms;
};

class CActionSetRoomProperty : public CAction {
public:
	CActionSetRoomProperty( std::set< int > const &selection, int property, int value );

	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	std::vector< std::pair< int, int > > m_Values; // Room index then old/new value
	int m_Property;
};


class CActionSetDoorProperty : public CAction {
public:
	CActionSetDoorProperty( CDoorSet const &selection, int property, int value );

	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	// struct for the use of CActionSetDoorProperty
	struct CDoorMomento {
		CDoorMomento( int room1, int room2, int value )
			: m_Room1( room1 ), m_Room2( room2 ), m_Value( value ) {}
		int m_Room1;
		int m_Room2;
		int m_Value;
	};
	std::vector< CDoorMomento > m_Values; // Room index then old/new value
	int m_Property;
};

class CActionRemoveMetaroom : public CAction {
public:
	CActionRemoveMetaroom( int handle );

	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	HMetaroom m_Metaroom;
	int m_Handle;
};

class CActionMoveMetaroom : public CAction {
public:
	CActionMoveMetaroom( int handle, CRect const &rect );

	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	CRect m_Rect;
	int m_Handle;
};

class CActionSetMetaroomBackground : public CAction {
public:
	CActionSetMetaroomBackground( int metaroom, CString const &background );

	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	int m_Metaroom;
	CString m_Background;
};

class CActionAddMetaroomBackground : public CAction {
public:
	CActionAddMetaroomBackground( int metaroom, CString const &background );

	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	int m_Metaroom;
	CString m_Background;
};

class CActionRemoveMetaroomBackground : public CAction {
public:
	CActionRemoveMetaroomBackground( int metaroom, int backgroundIndex );

	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	int m_Metaroom;
	int m_BackgroundIndex;
	CString m_Background;
};

class CActionChangeMetaroomBackground : public CAction {
public:
	CActionChangeMetaroomBackground( int metaroom, int backgroundIndex, CString const &background );

	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	int m_Metaroom;
	int m_BackgroundIndex;
	CString m_Background;
};

class CActionSizeWorld : public CAction {
public:
	CActionSizeWorld( CSize const &size, int metaroomBase, int roomBase );

	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	CSize m_Size;
	int m_MetaroomIndexBase;
	int m_RoomIndexBase;
};

class CActionChangeMetaroomMusic : public CAction {
public:
	CActionChangeMetaroomMusic( int metaroom, CString const &music );
	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	int m_Metaroom;
	CString m_Music;
};

class CActionChangeRoomMusic : public CAction {
public:
	CActionChangeRoomMusic( std::set< int > const &selection, CString const &music );

	virtual bool Do( CC2ERoomEditorDoc *doc );
	virtual bool Undo( CC2ERoomEditorDoc *doc );
	UINT Describe() const;

private:
	std::vector< std::pair< int, CString > > m_Music; // Room index then old/new value
};


#endif

