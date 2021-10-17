#include "stdafx.h"

#include "World.h"
#include "REException.h"
#include "ViewParams.h"
#include "utils.h"
#include "Game.h"
#include "ProgDlg.h"
#include "ColourRange.h"
#include "RoomCA.h"

#include<ostream>
#include<istream>
#include<sstream>
#include<algorithm>

CWorld::CWorld() : m_Size( 10000, 10000 ),
	m_MetaroomIndexBase(0),
	m_RoomIndexBase(0)

{
}

CSize CWorld::GetSize() const
{
	return m_Size;
}

void CWorld::SetSize( CSize const &size )
{
	m_Size = size;
}

int CWorld::AddMetaroom( CString const &filename, CRect const &rect )
{
	CMetaroomMap::const_iterator itor;

	int nextIndex = 0;
	while( m_Metarooms.find( nextIndex ) != m_Metarooms.end() ) ++nextIndex;

	m_Metarooms[ nextIndex ] = HMetaroom( new CMetaroom( rect, filename ) );

	return nextIndex;
}

bool CWorld::AddMetaroom( int index, HMetaroom const &metaroom )
{
	if( m_Metarooms.find( index ) != m_Metarooms.end() ) return false;
	m_Metarooms[ index ] = metaroom;
	return true;
}

bool CWorld::RemoveMetaroom( int handle )
{
	CMetaroomMap::iterator itor = m_Metarooms.find( handle );

	if( itor == m_Metarooms.end() ) return false;

	m_Metarooms.erase( itor );

	CalcDoors();

	return true;
}

HMetaroom CWorld::GetMetaroom( int handle ) const
{
	CMetaroomMap::const_iterator itor = m_Metarooms.find( handle );

	if( itor == m_Metarooms.end() )
		throw CREException( "Unable to find metaroom %d.", handle );
	return itor->second;
}

int CWorld::FindMetaroom( CPoint point, int tolerance ) const
{
	CMetaroomMap::const_iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
	{
		CRect rect = itor->second->GetRect();
		rect.InflateRect( tolerance, tolerance );
		if( rect.PtInRect( point ) ) return itor->first;
	}
	return -1;
}

int CWorld::FindRoom( CPoint point, int tolerance ) const
{
	CMetaroomMap::const_iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
	{
		int room = itor->second->FindRoom( point, tolerance );
		if( room != -1 ) return room;
	}
	return -1;
}

int CWorld::FindRooms( CRect const &rect, std::set< int > &set ) const
{
	int ret = 0;
	CMetaroomMap::const_iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
	{
		ret += itor->second->FindRooms( rect, set );
	}
	return ret;
}

int CWorld::GetRoomCount() const
{
	int ret = 0;
	CMetaroomMap::const_iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
	{
		ret += itor->second->GetRoomCount();
	}
	return ret;
}

HDoor CWorld::FindDoor( CPoint point, int tolerance ) const
{
	UpdateDoors();

	CDoorSet::const_iterator itor;
	for( itor = m_DoorSet.begin(); itor != m_DoorSet.end(); ++itor )
		if( PointToLineSegment( (*itor)->GetPoint1(), (*itor)->GetPoint2(), point ) <  tolerance )
			return *itor;
	for( itor = m_ExtDoorSet.begin(); itor != m_ExtDoorSet.end(); ++itor )
		if( PointToLineSegment( (*itor)->GetPoint1(), (*itor)->GetPoint2(), point ) <  tolerance )
			return *itor;

	return HDoor( 0 );
}

HDoor CWorld::GetDoor( int room1, int room2 ) const
{
	UpdateDoors();

	CDoorSet::const_iterator itor;
	for( itor = m_DoorSet.begin(); itor != m_DoorSet.end(); ++itor )
		if( (*itor)->GetRoom1() == room1 && (*itor)->GetRoom2() == room2 )
			return *itor;

	return HDoor( 0 );
}

int CWorld::AddRoom( CPoint const *points )
{
	int newIndex = 0;
	while( HasRoom( newIndex ) ) ++newIndex;
	AddRoom( newIndex, HRoom( new CRoom( points ) ) );
	return newIndex;
}

bool CWorld::AddRoom( int index, HRoom const &room )
{
	HMetaroom metaroom = GetMetaroom( FindMetaroom( room->GetShape().TopLeft(), 0 ) );
	bool ret = metaroom->AddRoom( index, room );
	if( ret ) CalcDoors();
	return ret;
}

bool CWorld::RemoveRoom( int index )
{
	CMetaroomMap::iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
	{
		if( itor->second->RemoveRoom( index ) )
		{
			CalcDoors();
			return true;
		}
	}
	return false;
}

bool CWorld::HasRoom( int index )
{
	CMetaroomMap::const_iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
	{
		if( itor->second->HasRoom( index ) ) return true;
	}
	return false;
}

HRoom CWorld::GetRoom( int index ) const
{
	CMetaroomMap::const_iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
	{
		HRoom room = itor->second->GetRoom( index );
		if( room.obj() ) return room;
	}
	return HRoom( 0 );
}

int CWorld::GetRoomMetaroom( int index ) const
{
	CMetaroomMap::const_iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
	{
		if( itor->second->HasRoom( index ) ) return itor->first;
	}
	return -1;
}

void CWorld::SnapRoom( CRoom *room, std::set<int> const *ignore, int tolerance, int metaroomIndex ) const
{
	if( metaroomIndex == -1 )
	{
		metaroomIndex = FindMetaroom( (room->GetShape().TopLeft() +
			room->GetShape().BottomRight() )/2, tolerance );
	}

	if( metaroomIndex != -1 )
	{
		HMetaroom metaroom = GetMetaroom( metaroomIndex );
		if( metaroom.obj() )
			metaroom->SnapRoom( room, ignore, tolerance );
	}
//	CMetaroomMap::const_iterator itor;
//	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
//		itor->second->SnapRoom( room, ignore, tolerance );
}

void CWorld::CalcDoors()
{
	m_DoorsUpToDate = false;
}

class CompareDoorHandles {
public:
	CompareDoorHandles( HDoor const &d ) : m_d( d ) {}
	bool operator()( HDoor const &d ) { return *m_d == *d; }
private:
	HDoor m_d;
};

void CWorld::UpdateDoors() const
{
	if( !m_DoorsUpToDate )
	{
		CDoorSet doorSet;
		CMetaroomMap::const_iterator itor;
		for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
			itor->second->CalcDoors( doorSet );

		CDoorSet::iterator itorNew = doorSet.begin();
		CDoorSet::const_iterator itorOld = m_DoorSet.begin();
		while( itorNew != doorSet.end() && itorOld != m_DoorSet.end() )
		{
			if( **itorNew == **itorOld )
			{
				itorNew->obj()->SetOpening( itorOld->obj()->GetOpening() );
				++itorNew;
				++itorOld;
			}
			else if( *itorNew < *itorOld ) // extra new door (might be in unused set)
			{
				CDoorSet::iterator itorUnused = std::find_if( m_UnusedDoorSet.begin(), 
					m_UnusedDoorSet.end(), CompareDoorHandles( *itorNew ) );
				if( itorUnused != m_UnusedDoorSet.end() )
				{
					itorNew->obj()->SetOpening( (*itorUnused)->GetOpening() );
					m_UnusedDoorSet.erase( itorUnused );
				}
				++itorNew;
			}
			else //  extra old door (add to unused set)
			{
				m_UnusedDoorSet.insert( *itorOld );
				++itorOld;
			}
		}
		m_DoorSet = doorSet;

		m_ExtDoorSet.clear();
		for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
			itor->second->CalcExtDoors( doorSet, m_ExtDoorSet );
	}
	m_DoorsUpToDate = true;
}

void CWorld::Draw( CDC *dc, CViewParams *params ) const
{
	UpdateDoors();

	dc->SelectStockObject( WHITE_BRUSH );
	dc->SelectStockObject( BLACK_PEN );
	CRect rect = params->WorldToWindow( CRect( CPoint( 0, 0 ), GetSize() ) );
	dc->Rectangle( &rect );

	dc->SelectStockObject( BLACK_BRUSH );
	CRect rectShadow( rect.right, rect.top + 4, rect.right + 4, rect.bottom + 4 );
	dc->Rectangle( &rectShadow );
	rectShadow = CRect( rect.left + 4, rect.bottom, rect.right + 4, rect.bottom + 4 );
	dc->Rectangle( &rectShadow );

	CPen closedPen( PS_SOLID, 0, RGB( 0, 255, 255 ) );
	CPen openPen( PS_SOLID, 0, RGB( 255, 255, 255 ) );

	CPen *oldPen = dc->SelectObject( &closedPen );

	CMetaroomMap::const_iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
		itor->second->Draw( dc, params );

	CDoorSet::const_iterator itorDoors;

	CColourRange range;
	range.SetRange( "{0,255,0,0} {50,255,255,0} {100,0,255,0}" );

	for( itorDoors = m_DoorSet.begin(); itorDoors != m_DoorSet.end(); ++itorDoors )
	{
/*
		if( itorDoors->obj()->GetOpening() )
			dc->SelectObject( &openPen );
		else
			dc->SelectObject( &closedPen );
*/
		CPen aPen( PS_SOLID, 0, range.GetColour( itorDoors->obj()->GetOpening() ) );
		CPen *oldPen = dc->SelectObject( &aPen );
		dc->MoveTo( params->WorldToWindow( itorDoors->obj()->GetPoint1() ) );
		dc->LineTo( params->WorldToWindow( itorDoors->obj()->GetPoint2() ) );
		dc->SelectObject( oldPen );
	}

	dc->SelectObject( &closedPen );
	for( itorDoors = m_ExtDoorSet.begin(); itorDoors != m_ExtDoorSet.end(); ++itorDoors )
	{
		dc->MoveTo( params->WorldToWindow( itorDoors->obj()->GetPoint1() ) );
		dc->LineTo( params->WorldToWindow( itorDoors->obj()->GetPoint2() ) );
	}
	dc->SelectObject( oldPen );
}

void CWorld::Renumber()
{
	int start = 0;
	CMetaroomMap::iterator itor;
	int mBase = m_MetaroomIndexBase;
	int rBase = m_RoomIndexBase;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
		itor->second->Renumber( mBase, rBase );
}

void CWorld::Write( std::ostream &stream ) const
{
	UpdateDoors();

	stream << m_Size.cx << ' ' << m_Size.cy << '\n';
	stream << m_MetaroomIndexBase << ' ' <<  m_RoomIndexBase << '\n';

	stream << m_Metarooms.size() << '\n';
	CMetaroomMap::const_iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
	{
		stream << itor->first << '\n';
		itor->second->Write( stream );
	}
	stream << m_DoorSet.size() << '\n';
	CDoorSet::const_iterator itorDoors;
	for( itorDoors = m_DoorSet.begin(); itorDoors != m_DoorSet.end(); ++itorDoors )
	{
		stream << (*itorDoors)->GetRoom1() << ' ' << (*itorDoors)->GetRoom2() << '\n' <<
		1 << '\n' << (*itorDoors)->GetOpening() << '\n';
	}
}

void CWorld::Read( std::istream &stream, int version )
{
	stream >> m_Size.cx >> m_Size.cy;
	if( version >= 1004 )
		stream >> m_MetaroomIndexBase >> m_RoomIndexBase;

	int nMetarooms;
	stream >> nMetarooms;
	CString backgroundFailures;
	while( nMetarooms-- )
	{
		int i;
		stream >> i;
		HMetaroom room( new CMetaroom() );
		room->Read( stream, version, backgroundFailures );
		m_Metarooms[i] = room;
	}
	if( backgroundFailures != "" )
		MessageBox( NULL, backgroundFailures, "Error Loading Backgound(s)", MB_OK | MB_ICONEXCLAMATION );
	CalcDoors();
	int nDoors, room1, room2, dummy, opening;
	stream >> nDoors;
	while( nDoors-- )
	{
		stream >> room1 >> room2 >> dummy >> opening;
		HDoor door = GetDoor( room1, room2 );
		if( door.obj() ) door->SetOpening( opening );
	}
}

void CWorld::Validate( std::set< int > *pRooms, CDoorSet *pDoorSet )
{
	UpdateDoors();

	CMetaroomMap::const_iterator itor1, itor2;
	for( itor1 = m_Metarooms.begin(); itor1 != m_Metarooms.end(); ++itor1 )
	{
		CRect rect = itor1->second->GetRect();
		//check rectangle is inside the world
		if( rect.left < 0 || rect.top < 0 ||
			rect.right > m_Size.cx || rect.bottom > m_Size.cy )
			throw CREException( "Metaroom outside world." );


		//check it doesn't overlap any others
		for( itor2 = itor1, ++itor2; itor2 != m_Metarooms.end(); ++itor2 )
			if( CRect().IntersectRect( rect, itor2->second->GetRect() ) )
				throw CREException( "Metarooms overlap." );

		itor1->second->Validate( pRooms );
	}

	CDoorSet::const_iterator itorDoors;

	for( itorDoors = m_DoorSet.begin(); itorDoors != m_DoorSet.end(); ++itorDoors )
		if( itorDoors->obj()->GetLength() < 5 )
		{
			if( pDoorSet ) pDoorSet->insert( *itorDoors );
			throw CREException( "An internal door has been generated which is too small (doors must be at least 5 units long)." );
		}

	for( itorDoors = m_ExtDoorSet.begin(); itorDoors != m_ExtDoorSet.end(); ++itorDoors )
		if( itorDoors->obj()->GetLength() < 5 )
		{
			if( pDoorSet ) pDoorSet->insert( *itorDoors );
			throw CREException( "An external door has been generated which is too small (doors must be at least 5 units long)." );
		}


}

void CWorld::SaveToGame( CGame &game, CDoorSet &doorErrors, CProgressDlg &dlg, bool addon )
{
	Renumber();
	UpdateDoors();
	dlg.Create();
	if (!addon)
		game.Execute( "mapd %d %d", m_Size.cx, m_Size.cy );

	CMetaroomMap::const_iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
	{
		CString status;
		status.Format( "Writing Metaroom %d", itor->first );
		dlg.SetStatus( status );
		if (addon)
			itor->second->DeleteFromGame( game, dlg );
		itor->second->SaveToGame( game, dlg );
	}

	dlg.SetStatus( "Writing Doors" );
	dlg.SetRange( 0, m_DoorSet.size() + 1 );
	int i = 0;

	CDoorSet::const_iterator itorDoors;
	CString execute;
	int DoorCount = 0;
	for( itorDoors = m_DoorSet.begin(); itorDoors != m_DoorSet.end(); ++itorDoors )
	{
		dlg.SetPos( i++ );
		CString door;
		door.Format( "door game \"mapeditortmp_%d\" game \"mapeditortmp_%d\" %d\n", 
				GetRoom((*itorDoors)->GetRoom1() )->GetGameID(),
				GetRoom((*itorDoors)->GetRoom2())->GetGameID(),  
				(*itorDoors)->GetOpening() );
		execute += door;
		if( ++DoorCount == 10 )
		{
			game.Execute( execute );
			execute = "";
			DoorCount = 0;
		}
		if( dlg.CheckCancelButton() )
			throw CREException( "Save to game canceled by user" );
	}
	if( DoorCount )
		game.Execute( execute );

	dlg.SetStatus( "Clean Up Code" );
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
	{
		itor->second->SaveToGameCleanUp( game, dlg );
	}

}

void CWorld::LoadFromGame( CGame &game, CDoorSet &doorErrors, CProgressDlg &dlg )
{
	dlg.Create();

	//Get world size
	std::stringstream stream( (const char *)game.Execute( "outv mapw outs \" \" outv maph" ) );
	stream >> m_Size.cx >> m_Size.cy;

	std::stringstream metaIDs( (const char *)game.Execute( "outs emid" ) );
	CString backgroundFailures;
	int metaID;
	while( metaIDs >> metaID )
	{
		CString status;
		status.Format( "Loading Metaroom %d", metaID );
		dlg.SetStatus( status );
		HMetaroom metaRoom( new CMetaroom() );
		metaRoom->LoadFromGame( metaID, game, dlg, backgroundFailures );
		m_Metarooms[metaID] = metaRoom;
	}
	if( backgroundFailures != "" )
		MessageBox( NULL, backgroundFailures, "Error Loading Backgound(s)", MB_OK | MB_ICONEXCLAMATION );

	CalcDoors();
	UpdateDoors();

	dlg.SetStatus( "Reading Doors" );
	dlg.SetRange( 0, m_DoorSet.size() + 1 );
	int i = 0;

	CDoorSet::iterator itorDoors;
	CString execute;
	for( itorDoors = m_DoorSet.begin(); itorDoors != m_DoorSet.end(); ++itorDoors )
	{
		CString door;
		door.Format( "outv door %d %d outs \" \"\n", (*itorDoors)->GetRoom1(), (*itorDoors)->GetRoom2() );
		execute += door;
	}

	std::stringstream doorStream( (const char *)game.Execute( execute ) );

	for( itorDoors = m_DoorSet.begin(); itorDoors != m_DoorSet.end(); ++itorDoors )
	{
		int perm;
		doorStream >> perm;
		(*itorDoors)->SetOpening( perm );
	}
}

void CWorld::StepCA( std::vector< std::vector<CCARates> > const &rates,
					 std::vector< CCheese > const &cheeses )
{
	UpdateDoors();

	std::vector< CCheese >::const_iterator itorCheese;
	for( itorCheese = cheeses.begin(); itorCheese != cheeses.end(); ++itorCheese )
	{
		HRoom room = GetRoom( FindRoom( itorCheese->GetPoint(), 0 ) );
		if( room.obj() ) room->m_CAInput = 1;
	}


	CMetaroomMap::iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
		itor->second->StepCA( rates );

	CDoorSet::iterator itorDoors;
/*
	//Update 'doorage'
	for( itorDoors = m_DoorSet.begin(); itorDoors != m_DoorSet.end(); ++itorDoors )
	{
		HRoom room1 = GetRoom( (*itorDoors)->GetRoom1() );
		HRoom room2 = GetRoom( (*itorDoors)->GetRoom2() );

		float doorage = 1;//(*itorDoors)->GetLength() * (*itorDoors)->GetOpening();

		room1->SetDoorage( room1->GetDoorage() + doorage );
		room2->SetDoorage( room2->GetDoorage() + doorage );
	}
*/

	for( itorDoors = m_DoorSet.begin(); itorDoors != m_DoorSet.end(); ++itorDoors )
	{
		HRoom room1 = GetRoom( (*itorDoors)->GetRoom1() );
		HRoom room2 = GetRoom( (*itorDoors)->GetRoom2() );
		float doorSize = (*itorDoors)->GetLength() * (*itorDoors)->GetOpening();
		float doorage1 = doorSize /
			( float(100) * room1->GetShape().GetPerimeter() );//1;//(*itorDoors)->GetLength() * (*itorDoors)->GetOpening();
		float doorage2 = doorSize /
			( float(100) * room2->GetShape().GetPerimeter() );//1;//(*itorDoors)->GetLength() * (*itorDoors)->GetOpening();
		UpdateDoorCA( room1->m_TempCAValue, rates[room1->GetType()][0], doorage1,// / room1->GetDoorage(),
					  room2->m_TempCAValue, rates[room2->GetType()][0], doorage2,// / room2->GetDoorage(),
					  (*itorDoors)->GetOpening() / 100.0,
					  room1->m_CAValue, room2->m_CAValue );
		room1->SetDoorage( room1->GetDoorage() + doorage1 );
		room2->SetDoorage( room2->GetDoorage() + doorage2 );
	}

	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
		itor->second->StepCA2();
}

void CWorld::ReadCAValues( std::istream &stream, int index )
{
	CMetaroomMap::iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
		itor->second->ReadCAValues( stream, index );
}

void CWorld::GetCACommand( CString &command )
{
	CMetaroomMap::iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
		itor->second->GetCACommand( command );
}


void CWorld::SelectRoomWronguns( int perm, int height, std::set< int > &selection )
{
	CMetaroomMap::iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
		itor->second->SelectRoomWronguns( perm, height, selection, m_DoorSet, m_ExtDoorSet );
}

std::vector<int> CWorld::GetMetaroomIndices() const
{
	std::vector<int> indices;
	CMetaroomMap::iterator itor;
	for( itor = m_Metarooms.begin(); itor != m_Metarooms.end(); ++itor )
		indices.push_back(itor->first);
	return indices;
}


