#include "stdafx.h"

#include <ostream>
#include <istream>
#include <strstream>
#include <sstream>
#include <string>

#include "Metaroom.h"
#include "ViewParams.h"
#include "Door.h"
#include "Game.h"
#include "ProgDlg.h"
#include "utils.h"
#include "RoomCA.h"

#include "REException.h"

CMetaroom::CMetaroom( CRect const &rect, CString const &bitmapFilename )
	: m_Rect( rect )
{
	m_Rect = rect;
	m_BitmapFilenames.push_back( bitmapFilename );
	CString preview = GetPreviewDirectory() + GetFileTitle( bitmapFilename ) + ".bmp";
	CSize size = LoadBackground( preview ); //may throw and prevent construction
	m_Rect.right = m_Rect.left + size.cx * 4;
	m_Rect.bottom = m_Rect.top + size.cy * 4;
}

void CMetaroom::Validate( std::set< int > *pRooms ) const
{
	CRoomMap::const_iterator itor, itor2;
	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
	{
		const CRoomShape &shape = itor->second->GetShape();
		if( shape.BottomLeft().x >= shape.BottomRight().x ||
			shape.BottomLeft().y <= shape.TopLeft().y ||
			shape.BottomRight().y <= shape.TopRight().y )
		{
			if( pRooms ) pRooms->insert( itor->first );
			throw CREException( "Room bad shape" );
		}

		if( !m_Rect.PtInRect( shape.BottomLeft() ) ||
			!m_Rect.PtInRect( shape.TopLeft() ) ||
			!m_Rect.PtInRect( shape.BottomRight() ) ||
			!m_Rect.PtInRect( shape.TopRight() ) )
		{
			if( pRooms ) pRooms->insert( itor->first );
			throw CREException( "Room not inside metaroom" );
		}
		for( itor2 = m_Rooms.begin(); itor2 != m_Rooms.end(); ++itor2 )
		{
			if( itor != itor2 &&
				(itor2->second->ContainsPoint( shape.BottomLeft(), -1 ) ||
				 itor2->second->ContainsPoint( shape.TopLeft(), -1 ) ||
				 itor2->second->ContainsPoint( shape.BottomRight(), -1 ) ||
				 itor2->second->ContainsPoint( shape.TopRight(), -1 ) ) )
			{
				if( pRooms )
				{
					pRooms->insert( itor->first );
					pRooms->insert( itor2->first );
				}
				throw CREException( "Rooms must not overlap" );
			}
		}
	}
}

CSize CMetaroom::LoadBackground( CString const &filename )
{
	CSize size;
	HANDLE Image = LoadImage( NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	if( Image )
	{
		HBitmap BMP( new CBitmap() );
		BMP->Attach( Image );
		m_Bitmap = BMP;

		BITMAP Bitmap;
		m_Bitmap->GetBitmap( &Bitmap );
		size = CSize( Bitmap.bmWidth, Bitmap.bmHeight );
		m_CurrentBitmap = filename;
	}
	else
		throw CREException( "Unable to load bitmap: %s", (char const *)filename );

	return size;
}

CRect CMetaroom::GetRect() const
{
	return m_Rect;
}

void CMetaroom::SetRect( CRect const &rect )
{
	CPoint offset = rect.TopLeft() - m_Rect.TopLeft();
	m_Rect = rect;
	CRoomMap::iterator itor;
	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
		itor->second->Move( offset );
}

void CMetaroom::Draw( CDC *dc, CViewParams *params ) const
{
	if( params->DrawBackground() && m_Bitmap.GetObj() )	{
		CDC aDC;
		aDC.CreateCompatibleDC( dc );
		CBitmap *oldBitmap = aDC.SelectObject( &(*m_Bitmap) );
		BITMAP Bitmap;
		m_Bitmap->GetBitmap( &Bitmap );

		CRect rect = params->WorldToWindow( m_Rect );

		dc->StretchBlt( rect.left, rect.top, rect.Width(), rect.Height(), 
			&aDC,  0, 0, Bitmap.bmWidth, Bitmap.bmHeight, SRCCOPY );

		aDC.SelectObject( oldBitmap );
	}

	int p = params->ParamIndex();
	if( p != -1 )
	{


		CRoomMap::const_iterator itor;
		CBrush *halftoneBrush = dc->GetHalftoneBrush();
		CBrush *oldBrush = dc->SelectObject( halftoneBrush );
		dc->SetBkColor( RGB( 0, 0, 0)  );
		for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
		{
			int oldMode = dc->SetROP2( R2_MASKNOTPEN );
			dc->SetTextColor( RGB( 255, 255, 255)  );
			itor->second->DrawData( dc, params, itor->second->m_CAValue );
//			itor->second->Fill( dc, params );

			dc->SetTextColor( params->ParamColour( itor->second->m_CAValue * 256 )  );
//			dc->SetTextColor( RGB( 0, 255, 255 )  );
			dc->SetROP2( R2_MERGEPEN );
			itor->second->DrawData( dc, params, itor->second->m_CAValue );
//			itor->second->Fill( dc, params );
			dc->SetROP2( oldMode );

		}
		dc->SelectObject( oldBrush );
/*	

		CRoomMap::const_iterator itor;
		for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
		{
//			CBrush brush( params->ParamColour( itor->second->GetProperty( p + 6 ) ) );
			CBrush brush( params->ParamColour( itor->second->m_CAValue * 256 ) );
			CBrush *oldBrush = dc->SelectObject( &brush );
			itor->second->DrawData( dc, params, itor->second->m_CAValue );
			dc->SelectObject( oldBrush );
		}
*/
	}
}

void CMetaroom::DrawHandles( CDC *dc, CViewParams *params ) const
{
	CPoint points[4];
	CRect rect = params->WorldToWindow( m_Rect );
	points[0] = rect.TopLeft();
	points[1] = CPoint( rect.right, rect.top );
	points[2] = rect.BottomRight();
	points[3] = CPoint( rect.left, rect.bottom );

	for( int i = 0; i < 4; ++i ) {
		CRect rect( points[i] - CSize( 2, 2 ), CSize( 5, 5 ) );
		dc->InvertRect( rect );
	}
}

bool CMetaroom::HasRoom( int index ) const
{
	return m_Rooms.find( index ) != m_Rooms.end();
}

HRoom CMetaroom::GetRoom( int index ) const
{
	CRoomMap::const_iterator itor = m_Rooms.find( index );
	if( itor == m_Rooms.end() ) return HRoom( 0 );
	return itor->second;
}

int CMetaroom::FindRoom( CPoint point, int tolerance ) const
{
	CRoomMap::const_iterator itor;
	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
		if( itor->second->ContainsPoint( point, tolerance ) )
			return itor->first;

	return -1;
}

int CMetaroom::FindRooms( CRect const &rect, std::set< int > &set ) const
{
	int ret = 0;

	CRoomMap::const_iterator itor;
	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
	{
		CRoomShape const &shape = itor->second->GetShape();
		if( rect.PtInRect( shape.BottomLeft() ) &&
			rect.PtInRect( shape.TopLeft() ) &&
			rect.PtInRect( shape.BottomRight() ) &&
			rect.PtInRect( shape.TopRight() ) )
		{
			set.insert( itor->first );
			++ret;
		}
	}
	return ret;
}

void CMetaroom::SnapRoom( CRoom *room, std::set<int> const *ignore, int tolerance ) const
{
	CRoomMap::const_iterator itor;
	CRoomShape shape = room->GetShape();
	shape.SnapInsideRect( m_Rect );
	room->SetShape( shape );
	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
		if( !ignore || (ignore->find( itor->first ) == ignore->end()) )
		{
			room->SnapWalls( *(itor->second), tolerance );
		}
	CPoint leftTop, rightTop, leftBottom, rightBottom;
	leftTop.x = 1000000;
	rightTop.x = -1;
	leftBottom.x = 1000000;
	rightBottom.x = -1;
	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
		if( !ignore || (ignore->find( itor->first ) == ignore->end()) )
		{
			room->SnapFloors( *(itor->second), tolerance, leftTop, rightTop, leftBottom, rightBottom );
		}

	if( leftTop.x != 1000000 ) room->SnapCeilingTo( leftTop, rightTop );
	if( leftBottom.x != 1000000 ) room->SnapFloorTo( leftBottom, rightBottom );
}

bool CMetaroom::RemoveRoom( int index )
{
	CRoomMap::iterator itor = m_Rooms.find( index );
	if( itor == m_Rooms.end() ) return false;
	m_Rooms.erase( itor );
	return true;
}

bool CMetaroom::AddRoom( int index, HRoom room )
{
	m_Rooms[ index ] = room;
	return true;
}

void CMetaroom::Write( std::ostream &stream ) const
{
	stream << m_Rect.left << ' ' << m_Rect.top << ' ' << 
		m_Rect.right << ' ' << m_Rect.bottom << '\n' << m_BitmapFilenames.size() << '\n';
	for( std::vector< CString >::const_iterator itorB = m_BitmapFilenames.begin();
		itorB != m_BitmapFilenames.end(); ++itorB )
		stream << (const char *)*itorB << '\n';

	stream << (const char *)m_Music << '\n';

	stream << m_Rooms.size() << '\n';
	CRoomMap::const_iterator itor;
	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
	{
		stream << itor->first << '\n';
		itor->second->Write( stream );
	}
}

void CMetaroom::Read( std::istream &stream, int version, CString &backgroundFailures )
{
	int backCount;
	stream >> m_Rect.left >> m_Rect.top >>
		m_Rect.right >> m_Rect.bottom >> backCount;
	stream.ignore( 10000, '\n' );
	while( backCount-- )
	{
		std::string temp;
		std::getline( stream, temp );
		m_BitmapFilenames.push_back( temp.c_str() );
	}

	if( version >= 1002 )
	{
		std::string temp;
		std::getline( stream, temp );
		m_Music = temp.c_str();
	}

	if( m_BitmapFilenames.size() )
	{
		try
		{
			CString preview = GetPreviewDirectory() + GetFileTitle( *m_BitmapFilenames.begin() ) + ".bmp";
			LoadBackground( preview );
		}
		catch( CREException const &exept)
		{
//			MessageBox( NULL, exept.what(), "Error Loading Backgound", MB_OK | MB_ICONEXCLAMATION );
			backgroundFailures += exept.what();
			backgroundFailures += "\n";
		}
	}

	int nRooms;
	stream >> nRooms;
	while( nRooms-- )
	{
		int i;
		stream >> i;
		HRoom room( new CRoom() );
		room->Read( stream, version );
		m_Rooms[i] = room;
	}
}

void CMetaroom::CalcDoors( CDoorSet &set ) const
{
	CPoint first, second;
	int dir;

	CRoomMap::const_iterator itor1;
	for( itor1 = m_Rooms.begin(); itor1 != m_Rooms.end(); ++itor1 )
	{
		CRoomMap::const_iterator itor2 = itor1;
		for( ++itor2; itor2 != m_Rooms.end(); ++itor2 )
			if( itor1->second->IsNeighbour( *itor2->second, &first, &second, &dir ) )
				set.insert( HDoor( new CDoor( itor1->first, itor2->first, first, second, dir ) ) );
	}
}

void CMetaroom::CalcExtDoors( CDoorSet const &internal, CDoorSet &external ) const
{
	CRoomMap::const_iterator itor1;
	for( itor1 = m_Rooms.begin(); itor1 != m_Rooms.end(); ++itor1 )
		itor1->second->GetShape().CalcExtDoors( itor1->first, internal, external );
}

// va00 must be set before this script code.  i.e. Append
// the return from this function with the code to set va00
// with the metaroom number.
CString CMetaroom::CAOSCheckDeleteMetaRoomScript()
{
	return
		 "	* Delete any map editor alerters still left over\n"
		 "	enum 1 1 229\n"
		 "		kill targ\n"
		 "	next\n" 
		 "	* Check for any agents in the metaroom\n"
		 "	enum 0 0 0\n"
		 "		setv va32 attr\n"
		 "		andv va32 32\n"
		 "		doif va32 ne 32\n"
		 "			doif targ ne pntr\n"
		 "				doif gmap posx posy = va00 and carr = null\n"
		 "					cmrt 0\n"
		 "					seta va33 targ\n"
		 "					new: comp 1 1 229 \"blank\" 1 0 9000\n"
		 "					attr 32\n"
		 "					frel va33\n"
		 "					pat: fixd 1 \"blank\" 0 0 0 0 \"whiteontransparentchars\"\n"
		 "					flto 0 30\n"
		 "					part 1\n"
		 "					ptxt \"Map editor can't change this meta room; I'm still in it\"\n"
		 "					outs \"agent\"\n"
		 "					stop\n"
		 "				endi\n"
		 "			endi\n"
		 "		endi\n"
		 "	next\n";
}

// va00 must be set before this script code.  i.e. Append
// the return from this function with the code to set va00
// with the metaroom number.
CString CMetaroom::CAOSDeleteMetaRoomScript()
{
	return
		 "  * delete all the rooms in the meta room\n"
		 "	sets va01 erid va00\n"
		 "  \n"
		 "	doif strl va01 > 0\n"
		 "		setv va03 1\n"
		 "		sets va98 \"\"\n"
		 "		loop\n"
		 "			setv va99 char va01 va03\n"
		 "			doif va99 = ' '\n"
		 "				doif va98 <> \"\"\n"
		 "					delr stoi va98\n"
		 "					sets va98 \"\"\n"
		 "				endi\n"
		 "			else\n"
		 "				sets va50 \" \"\n"
		 "				char va50 1 va99\n"
		 "				adds va98 va50\n"
		 "			endi\n"
		 "			addv va03 1\n"
		 "		untl va03 > strl va01\n"
		 "		doif va98 <> \"\"\n"
		 "			delr stoi va98\n"
		 "			sets va98 \"\"\n"
		 "		endi\n"
		 "	endi\n"
		 "  \n"
		 "  * delete the metaroom itself\n"
		 "	delm va00\n";
}

// Tries to delete the existing room if it can
void CMetaroom::DeleteFromGame( CGame &game, CProgressDlg &dlg ) const
{
	if( m_BitmapFilenames.empty() )
		throw CREException( "Metaroom has no background." );

	CString bkg;
	std::vector< CString >::const_iterator itorBack = m_BitmapFilenames.begin();
	for( ; itorBack != m_BitmapFilenames.end(); ++itorBack )
	{
		if( itorBack != m_BitmapFilenames.begin() )
			bkg += ",";
		bkg = bkg + GetFileTitle(*itorBack);
	}

	CString result = game.Execute(
				 "setv va00 gmap %d %d\n"
				 "doif va00 <> -1\n"

				 "  * check it is made from the same background\n"
				 "  doif bkds va00 <> \"%s\"\n"
				 "		outs \"background\"\n"
				 "		stop\n"
				 "	endi\n"
				
				 "%s\n"
				 "endi\n",
		m_Rect.CenterPoint().x, m_Rect.CenterPoint().y,
		(const char*)bkg, 
		(const char*)(CAOSCheckDeleteMetaRoomScript() + CAOSDeleteMetaRoomScript()));

	if (result == "agent")
		throw CREException( "The existing metaroom can't be deleted because there is an agent in it.  The camera has moved to the agent and it has been marked.  Please remove the agent and try again." );
	if (result == "background")
		throw CREException( "There is a metaroom in the same place with a different background, so it is probably from a different addon.  Please move your metaroom, or start a new world and try again." );
}

void CMetaroom::SaveToGame( CGame &game, CProgressDlg &dlg ) const
{
	if( m_BitmapFilenames.empty() )
		throw CREException( "Metaroom has no background." );

	std::vector< CString >::const_iterator itorBack = m_BitmapFilenames.begin();

	game.Execute( "setv game \"mapeditortmp\" addm %d %d %d %d \"%s\"",
		m_Rect.left, m_Rect.top, m_Rect.Width(), m_Rect.Height(), (const char *)GetFileTitle(*itorBack) );

	game.Execute( "mmsc %d %d \"%s\"", m_Rect.CenterPoint().x, m_Rect.CenterPoint().y,
		(const char *)CGame::Escape(m_Music) );
	
	for( ++itorBack; itorBack != m_BitmapFilenames.end(); ++itorBack )
		game.Execute( "addb game \"mapeditortmp\" \"%s\"", (const char *)GetFileTitle(*itorBack) );

	dlg.SetRange( 0, m_Rooms.size() + 1 );
	int i = 0;
	CRoomMap::const_iterator itor;
	CString execute;

	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
	{
		dlg.SetPos( i++ );
		execute += itor->second->GetGameString("game \"mapeditortmp\"");
		if( dlg.CheckCancelButton() )
			throw CREException( "Save to game canceled by user" );
	}

	game.Execute( execute );
}

void CMetaroom::SaveToGameCleanUp( CGame &game, CProgressDlg &dlg ) const
{
	dlg.SetRange( 0, m_Rooms.size() + 1 );
	int i = 0;
	CRoomMap::const_iterator itor;
	CString execute;

	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
	{
		dlg.SetPos( i++ );
		execute += itor->second->GetGameStringCleanUp();
		if( dlg.CheckCancelButton() )
			throw CREException( "Save to game canceled by user" );
	}

	game.Execute( execute );
}

void CMetaroom::LoadFromGame( int id, CGame &game, CProgressDlg &dlg, CString &backgroundFailures )
{
	std::stringstream streamLoc( (const char *)game.Execute( "outs mloc %d", id ) );

	int width, height;
	streamLoc >> m_Rect.left >> m_Rect.top >> width >> height;
	m_Rect.right = m_Rect.left + width;
	m_Rect.bottom = m_Rect.top + height;

	std::stringstream streamMusic( (const char *)game.Execute( "outs mmsc %d %d", m_Rect.CenterPoint().x, m_Rect.CenterPoint().y ) );
	m_Music = streamMusic.str().c_str();

	std::stringstream streamBack( (const char *)game.Execute( "outs bkds %d", id ) );
	std::string back;
	while( std::getline( streamBack, back, ',' ) )
	{
		m_BitmapFilenames.push_back( CString( back.c_str() ) + ".bmp" );
	}
	if( m_BitmapFilenames.size() )
	{
		try
		{
			CString preview = GetPreviewDirectory() + *m_BitmapFilenames.begin();
			LoadBackground( preview );
		}
		catch( CREException const &exept)
		{
			backgroundFailures += exept.what();
			backgroundFailures += "\n";
//			MessageBox( NULL, exept.what(), "Error Loading Backgound", MB_OK | MB_ICONEXCLAMATION );
		}
	}
	

//	for( ++itorBack; itorBack != m_BitmapFilenames.end(); ++itorBack )
//		game.Execute( "addb %d \"%s\"", idGame, (const char *)GetFileTitle(*itorBack) );

	std::stringstream streamRooms( (const char *)game.Execute( "outs erid %d", id ) );

	int roomID;
	while( streamRooms >> roomID )
	{
		HRoom room = HRoom( new CRoom() );
		room->LoadFromGame( roomID, game );
		m_Rooms[roomID] = room;
	}
}

void CMetaroom::Renumber( int &mBase, int &rBase )
{
	CRoomMap::iterator itor;

	SetGameID( mBase++ );

	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
		itor->second->SetGameID( rBase++ );
}

void CMetaroom::StepCA( std::vector< std::vector<CCARates> > const &rates )
{
	std::vector<int> newValues( m_Rooms.size() );

	CRoomMap::iterator itor;
	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
	{
		UpdateRoomCA( rates[ itor->second->GetType() ][0], itor->second->m_CAInput,
			itor->second->m_CAValue, itor->second->m_TempCAValue );
		itor->second->SetDoorage( 0 );
	}

}

void CMetaroom::StepCA2()
{
	std::vector<int> newValues( m_Rooms.size() );

	CRoomMap::iterator itor;
	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
		UpdateRoomCA2( itor->second->m_CAValue,
			itor->second->GetDoorage(),
			itor->second->m_TempCAValue );
}

void CMetaroom::ReadCAValues( std::istream &stream, int index )
{
	CRoomMap::iterator itor;
	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
		stream >> itor->second->m_CAValue;
}

void CMetaroom::GetCACommand( CString &command )
{
	CRoomMap::iterator itor;
	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
	{
		CString temp;

		CPoint inside = itor->second->GetShape().GetInsidePoint();
		temp.Format( "outv prop grap %d %d va00\nouts \"\\n\"\n", 
			inside.x, inside.y );
		command += temp;
	}
}

void CMetaroom::SelectRoomWronguns( int Perm, int Height, std::set< int > &selection, 
		CDoorSet const &internal, CDoorSet const &external)
{
	CRoomMap::iterator itor;
	CDoorSet::const_iterator doorItor;
	for( itor = m_Rooms.begin(); itor != m_Rooms.end(); ++itor )
	{
		int id = itor->first;
		int left = itor->second->GetShape().GetOrd( CRoomShape::Ord::Left );
		int right = itor->second->GetShape().GetOrd( CRoomShape::Ord::Right );
		bool solidFloor = false;

		for( doorItor = internal.begin(); doorItor != internal.end(); ++doorItor )
			if((((*doorItor)->GetRoom1() == id && (*doorItor)->GetDirection() == CDoor::Down ) ||
				((*doorItor)->GetRoom2() == id && (*doorItor)->GetDirection() == CDoor::Up ) ) && 
				(*doorItor)->GetOpening() < Perm )
				solidFloor = true;

		for( doorItor = external.begin(); doorItor != external.end(); ++doorItor )
			if((*doorItor)->GetRoom1() == id && (*doorItor)->GetDirection() == CDoor::Down )
				solidFloor = true;

		if( solidFloor )
		{
			bool solidCeiling = false;
			for( doorItor = internal.begin(); doorItor != internal.end(); ++doorItor )
			{
				int doorLeft = (*doorItor)->GetPoint1().x;
				int doorRight = (*doorItor)->GetPoint2().x;
				if( ( (*doorItor)->GetDirection() == CDoor::Down ||
					(*doorItor)->GetDirection() == CDoor::Up ) &&
					(*doorItor)->GetOpening() < Perm &&
					doorLeft < right &&
					doorRight > left )
				{
					int x1 = std::_cpp_max( left, doorLeft );
					int x2 = std::_cpp_min( right, doorRight );
					double mFloor, cFloor, mCeiling, cCeiling;
					LineEquation( itor->second->GetShape().BottomLeft(), itor->second->GetShape().BottomRight(), &mFloor, &cFloor );
					LineEquation( (*doorItor)->GetPoint1(),(*doorItor)->GetPoint2(), &mCeiling, &cCeiling );
					double height1 = ( x1 * mFloor + cFloor ) - ( x1 * mCeiling + cCeiling );
					double height2 = ( x2 * mFloor + cFloor ) - ( x2 * mCeiling + cCeiling );
					if( ( height1 > 1 && height1 <= Height ) ||
						( height2 > 1 && height2 <= Height ) )
						solidCeiling = true;
				}
			}
			for( doorItor = external.begin(); doorItor != external.end(); ++doorItor )
			{

				int doorLeft = (*doorItor)->GetPoint1().x;
				int doorRight = (*doorItor)->GetPoint2().x;
				if( ( (*doorItor)->GetDirection() == CDoor::Down ||
					(*doorItor)->GetDirection() == CDoor::Up ) &&
					(*doorItor)->GetOpening() < Perm &&
					doorLeft < right &&
					doorRight > left )
				{
					int x1 = std::_cpp_max( left, doorLeft );
					int x2 = std::_cpp_min( right, doorRight );
					double mFloor, cFloor, mCeiling, cCeiling;
					LineEquation( itor->second->GetShape().BottomLeft(), itor->second->GetShape().BottomRight(), &mFloor, &cFloor );
					LineEquation( (*doorItor)->GetPoint1(),(*doorItor)->GetPoint2(), &mCeiling, &cCeiling );
					double height1 = ( x1 * mFloor + cFloor ) - ( x1 * mCeiling + cCeiling );
					double height2 = ( x2 * mFloor + cFloor ) - ( x2 * mCeiling + cCeiling );
					if( ( height1 > 1 && height1 <= Height ) ||
						( height2 > 1 && height2 <= Height ) )
						solidCeiling = true;
				}
			}

			if( solidCeiling )
				selection.insert( id );
		}
	}
}

