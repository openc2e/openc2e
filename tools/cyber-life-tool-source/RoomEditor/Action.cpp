#include "stdafx.h"


#include "Action.h"
#include "C2ERoomEditorDoc.h"

UINT CAction::Describe() const {return IDS_ACTION_NONE;}
UINT CActionAddMetaroom::Describe() const {return IDS_ACTION_ADD_METAROOM;}
UINT CActionRemoveMetaroom::Describe() const {return IDS_ACTION_REMOVE_METAROOM;}
UINT CActionMoveMetaroom::Describe() const {return IDS_ACTION_MOVE_METAROOM;}
UINT CActionAddRoom::Describe() const {return IDS_ACTION_ADD_ROOM;}
UINT CActionSetRoomProperty::Describe() const {return IDS_ACTION_ROOM_PROPERTY;}
UINT CActionMoveRoom::Describe() const {return IDS_ACTION_MOVE_ROOM;}
UINT CActionRemoveRoom::Describe() const {return IDS_ACTION_REMOVE_ROOM;}
UINT CActionSetDoorProperty::Describe() const {return IDS_ACTION_DOOR_PROPERTY;}
UINT CActionSetMetaroomBackground::Describe() const {return IDS_ACTION_SET_METAROOM_BACKGROUND;}
UINT CActionAddMetaroomBackground::Describe() const {return IDS_ACTION_ADD_METAROOM_BACKGROUND;}
UINT CActionRemoveMetaroomBackground::Describe() const {return IDS_ACTION_REMOVE_METAROOM_BACKGROUND;}
UINT CActionChangeMetaroomBackground::Describe() const {return IDS_ACTION_CHANGE_METAROOM_BACKGROUND;}
UINT CActionSizeWorld::Describe() const {return IDS_ACTION_WORLD_SIZE;}
UINT CActionChangeMetaroomMusic::Describe() const {return IDS_ACTION_SET_METAROOM_MUSIC;}
UINT CActionChangeRoomMusic::Describe() const {return IDS_ACTION_SET_ROOM_MUSIC;}
UINT CActionMultiAction::Describe() const {return m_Description;}

CActionMultiAction::CActionMultiAction( std::vector<HAction> const &actions, UINT description )
: m_Actions( actions ), m_Description( description )
{
}

bool CActionMultiAction::Do( CC2ERoomEditorDoc *doc )
{
	std::vector< HAction >::iterator itor;
	for( itor = m_Actions.begin(); itor != m_Actions.end(); ++itor )
		if( !(*itor)->Do( doc ) )
			return false;

	return true;
}

bool CActionMultiAction::Undo( CC2ERoomEditorDoc *doc )
{
	std::vector< HAction >::reverse_iterator itor;
	for( itor = m_Actions.rbegin(); itor != m_Actions.rend(); ++itor )
		if( !(*itor)->Undo( doc ) )
			return false;

	return true;
}

CActionAddMetaroom::CActionAddMetaroom( CString const &bitmapName, CRect const &rect )
: m_BitmapName( bitmapName ), m_Rect( rect )
{
}

bool CActionAddMetaroom::Do( CC2ERoomEditorDoc *doc )
{
	m_Handle = doc->AddMetaroom( m_BitmapName, m_Rect );
	return m_Handle != -1;
}

bool CActionAddMetaroom::Undo( CC2ERoomEditorDoc *doc )
{
	doc->UpdateAllViews( 0, DESELECT_METAROOM, (CObject *)&m_Handle );
	return doc->RemoveMetaroom( m_Handle );
}

CActionAddRoom::CActionAddRoom( CPoint *points )
{
	for( int i = 0; i < 4; ++i ) m_Points[i] = *points++;
}

bool CActionAddRoom::Do( CC2ERoomEditorDoc *doc )
{
	m_Handle = doc->AddRoom( m_Points );
	return m_Handle != -1;
}

bool CActionAddRoom::Undo( CC2ERoomEditorDoc *doc )
{
	doc->UpdateAllViews( 0, DESELECT_ROOM, (CObject *)&m_Handle );
	return doc->RemoveRoom( m_Handle );
}

CActionRemoveRoom::CActionRemoveRoom( std::set< int > const &sel )
{
	std::set< int >::const_iterator itor;
	for( itor = sel.begin(); itor != sel.end(); ++itor )
		m_Rooms.push_back( std::pair< int, HRoom > ( *itor, HRoom(0) ) );
}

bool CActionRemoveRoom::Do( CC2ERoomEditorDoc *doc )
{
	std::vector< std::pair< int, HRoom > >::iterator itor;
	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
	{
		itor->second = doc->GetWorld()->GetRoom( itor->first );
		doc->UpdateAllViews( 0, DESELECT_ROOM, (CObject *)&itor->first );
		doc->RemoveRoom( itor->first );
	}
	return true;
}

bool CActionRemoveRoom::Undo( CC2ERoomEditorDoc *doc )
{
	std::vector< std::pair< int, HRoom > >::iterator itor;
	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
		doc->AddRoom( itor->first, itor->second );
	return true;
}

CActionMoveRoom::CActionMoveRoom( std::set< int > const &sel, CPoint const &move, int mode, CWorld const &world, int tolerance )
{
	std::set< int >::const_iterator itor;
	for( itor = sel.begin(); itor != sel.end(); ++itor )
	{
		CRoom room = *world.GetRoom( *itor );
		room.Move( move, mode );
		world.SnapRoom( &room, &sel, tolerance, world.GetRoomMetaroom( *itor ) );
		m_Positions.push_back( std::pair< int, CRoomShape >( *itor, room.GetShape() ) );
	}
}

bool CActionMoveRoom::Do( CC2ERoomEditorDoc *doc )
{
	std::vector< std::pair< int, CRoomShape > >::iterator itor;
	for( itor = m_Positions.begin(); itor != m_Positions.end(); ++itor )
	{
		CRoomShape old = doc->GetWorld()->GetRoom( itor->first )->GetShape();
		doc->GetWorld()->GetRoom( itor->first )->SetShape( itor->second );
		itor->second = old;
	}
	doc->UpdateAllViews( 0, REFLECT_ROOM_PROPERTIES, 0 );
	doc->GetWorld()->CalcDoors();
	return true;
}

bool CActionMoveRoom::Undo( CC2ERoomEditorDoc *doc )
{
	return Do( doc );
}


CActionSetRoomProperty::CActionSetRoomProperty( std::set< int > const &selection, int property, int value )
{
	std::set< int >::const_iterator itor;
	m_Property = property;
	for( itor = selection.begin(); itor != selection.end(); ++itor )
		m_Values.push_back( std::pair< int, int >( *itor, value ) );
}

bool CActionSetRoomProperty::Do( CC2ERoomEditorDoc *doc )
{
	std::vector< std::pair< int, int > >::iterator itor;
	for( itor = m_Values.begin(); itor != m_Values.end(); ++itor )
	{
		int oldValue = doc->GetWorld()->GetRoom( itor->first )->GetProperty( m_Property );
		doc->GetWorld()->GetRoom( itor->first )->SetProperty( m_Property, itor->second );
		itor->second = oldValue;
	}
	doc->UpdateAllViews( 0, REFLECT_ROOM_PROPERTIES, 0 );
	if( m_Property < 6 ) doc->GetWorld()->CalcDoors();
	return true;
}

bool CActionSetRoomProperty::Undo( CC2ERoomEditorDoc *doc )
{
	return Do( doc );
}


CActionSetDoorProperty::CActionSetDoorProperty( CDoorSet const &selection, int property, int value )
{
	CDoorSet::const_iterator itor;
	m_Property = property;
	for( itor = selection.begin(); itor != selection.end(); ++itor )
		m_Values.push_back( CDoorMomento( (*itor)->GetRoom1(), (*itor)->GetRoom2(), value ) );
}

bool CActionSetDoorProperty::Do( CC2ERoomEditorDoc *doc )
{
	std::vector< CDoorMomento >::iterator itor;
	for( itor = m_Values.begin(); itor != m_Values.end(); ++itor )
	{
		HDoor door = doc->GetWorld()->GetDoor( itor->m_Room1, itor->m_Room2 );
		if (door.obj())
		{
			int oldValue = door->GetOpening();
			door->SetOpening( itor->m_Value );
			itor->m_Value = oldValue;
		}
	}
	doc->UpdateAllViews( 0, REFLECT_ROOM_PROPERTIES, 0 );
	return true;
}

bool CActionSetDoorProperty::Undo( CC2ERoomEditorDoc *doc )
{
	return Do( doc );
}

CActionRemoveMetaroom::CActionRemoveMetaroom( int index )
: m_Handle( index )
{
}

bool CActionRemoveMetaroom::Do( CC2ERoomEditorDoc *doc )
{
	doc->UpdateAllViews( 0, DESELECT_METAROOM, (CObject *)&m_Handle );
	m_Metaroom = doc->GetWorld()->GetMetaroom( m_Handle );
	if( !m_Metaroom.GetObj() ) return false;
	return doc->RemoveMetaroom( m_Handle );
}

bool CActionRemoveMetaroom::Undo( CC2ERoomEditorDoc *doc )
{
	return doc->AddMetaroom( m_Handle, m_Metaroom );
}

CActionMoveMetaroom::CActionMoveMetaroom( int index, CRect const &rect )
: m_Handle( index ), m_Rect( rect )
{
}

bool CActionMoveMetaroom::Do( CC2ERoomEditorDoc *doc )
{
	HMetaroom room = doc->GetWorld()->GetMetaroom( m_Handle );
	if( !room.GetObj() ) return false;
	CRect rect = room->GetRect();
	room->SetRect( m_Rect );
	m_Rect = rect;
	doc->GetWorld()->CalcDoors();
	doc->UpdateAllViews( 0 );
	return true;
}

bool CActionMoveMetaroom::Undo( CC2ERoomEditorDoc *doc )
{
	return Do( doc );
}

CActionSetMetaroomBackground::CActionSetMetaroomBackground( int metaroom, CString const &background )
: m_Metaroom( metaroom ), m_Background( background )
{
}

bool CActionSetMetaroomBackground::Do( CC2ERoomEditorDoc *doc )
{
	HMetaroom room = doc->GetWorld()->GetMetaroom( m_Metaroom );
	CString oldBackground = room->GetCurrentBitmap();
	room->LoadBackground( m_Background );
	m_Background = oldBackground;
	return true;
}

bool CActionSetMetaroomBackground::Undo( CC2ERoomEditorDoc *doc )
{
	return Do( doc );
}

CActionAddMetaroomBackground::CActionAddMetaroomBackground( int metaroom, CString const &background )
: m_Metaroom( metaroom ), m_Background( background )
{
}

bool CActionAddMetaroomBackground::Do( CC2ERoomEditorDoc *doc )
{
	HMetaroom room = doc->GetWorld()->GetMetaroom( m_Metaroom );
	room->AddBackgroundBitmap( m_Background );
	return true;
}

bool CActionAddMetaroomBackground::Undo( CC2ERoomEditorDoc *doc )
{
	HMetaroom room = doc->GetWorld()->GetMetaroom( m_Metaroom );
	room->RemoveBackgroundBitmap( room->GetBackgroundList().size() - 1 );
	return true;
}

CActionRemoveMetaroomBackground::CActionRemoveMetaroomBackground( int metaroom, int backgroundIndex )
: m_Metaroom( metaroom ), m_BackgroundIndex( backgroundIndex )
{
}

bool CActionRemoveMetaroomBackground::Do( CC2ERoomEditorDoc *doc )
{
	HMetaroom room = doc->GetWorld()->GetMetaroom( m_Metaroom );
	m_Background = room->GetBackgroundList()[ m_BackgroundIndex ];
	room->RemoveBackgroundBitmap( m_BackgroundIndex );
	return true;
}

bool CActionRemoveMetaroomBackground::Undo( CC2ERoomEditorDoc *doc )
{
	HMetaroom room = doc->GetWorld()->GetMetaroom( m_Metaroom );
	room->AddBackgroundBitmap( m_Background, m_BackgroundIndex );
	return true;
}

CActionChangeMetaroomBackground::CActionChangeMetaroomBackground( int metaroom, int backgroundIndex, CString const &background )
: m_Metaroom( metaroom ), m_BackgroundIndex( backgroundIndex ), m_Background( background )
{
}

bool CActionChangeMetaroomBackground::Do( CC2ERoomEditorDoc *doc )
{
	HMetaroom room = doc->GetWorld()->GetMetaroom( m_Metaroom );
	std::vector< CString > tempList = room->GetBackgroundList();
	std::swap( m_Background, tempList[ m_BackgroundIndex ] );
	room->SetBackgroundList( tempList );
	return true;
}

bool CActionChangeMetaroomBackground::Undo( CC2ERoomEditorDoc *doc )
{
	return Do( doc );
}

CActionSizeWorld::CActionSizeWorld( CSize const &size, int metaroomBase, int roomBase ) :
	m_Size( size ),
	m_MetaroomIndexBase( metaroomBase ),
	m_RoomIndexBase( roomBase )
{
}

bool CActionSizeWorld::Do( CC2ERoomEditorDoc *doc )
{
	CSize tempSize = doc->GetWorld()->GetSize();
	int tempMetaroomBase = doc->GetWorld()->GetMetaroomIndexBase();
	int tempRoomBase = doc->GetWorld()->GetRoomIndexBase();
	doc->GetWorld()->SetSize( m_Size );
	doc->GetWorld()->SetMetaroomIndexBase( m_MetaroomIndexBase );
	doc->GetWorld()->SetRoomIndexBase( m_RoomIndexBase );
	m_Size = tempSize;
	m_MetaroomIndexBase = tempMetaroomBase;
	m_RoomIndexBase = tempRoomBase;

	return true;
}

bool CActionSizeWorld::Undo( CC2ERoomEditorDoc *doc )
{
	return Do( doc );
}

CActionChangeMetaroomMusic::CActionChangeMetaroomMusic( int metaroom, CString const &music )
: m_Metaroom( metaroom ), m_Music( music )
{
}

bool CActionChangeMetaroomMusic::Do( CC2ERoomEditorDoc *doc )
{
	HMetaroom room = doc->GetWorld()->GetMetaroom( m_Metaroom );
	if( !room.GetObj() ) return false;
	CString oldMusic = room->GetMusic();
	room->SetMusic( m_Music );
	m_Music = oldMusic;
	return true;
}

bool CActionChangeMetaroomMusic::Undo( CC2ERoomEditorDoc *doc )
{
	return Do( doc );
}


CActionChangeRoomMusic::CActionChangeRoomMusic( std::set< int > const &selection, CString const &music  )
{
	std::set< int >::const_iterator itor;
	for( itor = selection.begin(); itor != selection.end(); ++itor )
		m_Music.push_back( std::pair< int, CString >( *itor, music ) );
}

bool CActionChangeRoomMusic::Do( CC2ERoomEditorDoc *doc )
{
	std::vector< std::pair< int, CString > >::iterator itor;
	for( itor = m_Music.begin(); itor != m_Music.end(); ++itor )
	{
		CString oldMusic = doc->GetWorld()->GetRoom( itor->first )->GetMusic();
		doc->GetWorld()->GetRoom( itor->first )->SetMusic( itor->second );
		itor->second = oldMusic;
	}
	return true;
}

bool CActionChangeRoomMusic::Undo( CC2ERoomEditorDoc *doc )
{
	return Do( doc );
}

