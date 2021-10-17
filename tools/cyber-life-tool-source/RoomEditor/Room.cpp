#include "stdafx.h"

#include "Room.h"
#include "Game.h"

#include <algorithm>
#include <math.h>
#include <string>
#include <sstream>
#include "Door.h"

#include "ViewParams.h"

/*
CRoom::CRoom( CRect const &rect ) : m_Properties( 32, 0 ), m_CAValue( 0 ), m_CAInput( 0 )
{
	m_Shape.m_Left = rect.left;
	m_Shape.m_Right = rect.right;
	m_Shape.m_LeftTop = m_Shape.m_RightTop = rect.top;
	m_Shape.m_LeftBottom = m_Shape.m_RightBottom = rect.bottom;
}
*/

bool SortOnXThenY( CPoint const &p1, CPoint const &p2 )
{
	if( p1.x == p2.x )
		return p1.y < p2.y;
	else
		return p1.x < p2.x;
}

CRoom::CRoom(CPoint const *points ) : m_Properties( 32, 0 ), m_CAValue( 0 ), m_CAInput( 0 )
{
	int i;

	CPoint temp[4];

	for( i = 0; i < 4; ++i )
		temp[i] = points[i];

	std::sort( temp, temp + 4, SortOnXThenY );

	m_Shape.SetOrd( CRoomShape::Ord::Left, temp[0].x );
	m_Shape.SetOrd( CRoomShape::Ord::Right, temp[3].x );
	m_Shape.SetOrd( CRoomShape::Ord::LeftTop, temp[0].y );
	m_Shape.SetOrd( CRoomShape::Ord::RightTop, temp[2].y );
	m_Shape.SetOrd( CRoomShape::Ord::LeftBottom, temp[1].y );
	m_Shape.SetOrd( CRoomShape::Ord::RightBottom, temp[3].y );

}

int CRoom::GetProperty( int index ) const
{
	if( index < 6 )
		return m_Shape.GetOrd( index );
	else
		return m_Properties[ index - 6 ];
}

void CRoom::SetProperty( int index, int value )
{
	if( index < 6 )
		m_Shape.SetOrd( index, value );
	else
		m_Properties[ index - 6] = value;
}

void CRoom::Draw( CDC *dc, CViewParams const *context ) const
{
//	CPen pen( PS_SOLID, 0, RGB( 0, 0, 0 ) );
//	CPen *oldPen = dc->SelectObject( &pen );
	CPoint points[5];
	points[0] = context->WorldToWindow( m_Shape.TopLeft() );
	points[1] = context->WorldToWindow( m_Shape.TopRight() );
	points[2] = context->WorldToWindow( m_Shape.BottomRight() );
	points[3] = context->WorldToWindow( m_Shape.BottomLeft() );
	points[4] = points[0];

	dc->Polyline( points, 5 );
//	dc->SelectObject( oldPen );
}

void CRoom::Fill( CDC *dc, CViewParams const *context ) const
{
//	CPen pen( PS_SOLID, 0, RGB( 0, 0, 0 ) );
//	CPen *oldPen = dc->SelectObject( &pen );
	CPoint points[5];
	points[0] = context->WorldToWindow( m_Shape.TopLeft() );
	points[1] = context->WorldToWindow( m_Shape.TopRight() );
	points[2] = context->WorldToWindow( m_Shape.BottomRight() );
	points[3] = context->WorldToWindow( m_Shape.BottomLeft() );
	points[4] = points[0];

	dc->Polygon( points, 5 );
//	dc->SelectObject( oldPen );
}

void CRoom::DrawData( CDC *dc, CViewParams const *context, float value ) const
{
//	CPen pen( PS_SOLID, 0, RGB( 0, 0, 0 ) );
//	CPen *oldPen = dc->SelectObject( &pen );
	CPoint points[5];
	points[0] = context->WorldToWindow( m_Shape.BottomLeft() );
	points[1] = context->WorldToWindow( (m_Shape.TopLeft() - m_Shape.BottomLeft()) * value + m_Shape.BottomLeft() );
	points[2] = context->WorldToWindow( (m_Shape.TopRight() - m_Shape.BottomRight()) * value + m_Shape.BottomRight() );
	points[3] = context->WorldToWindow( m_Shape.BottomRight() );
	points[4] = points[0];

	dc->Polygon( points, 5 );
	std::stringstream stream;
	stream << value;
//	int x = (m_Shape.GetOrd( CRoomShape::Left ) + m_Shape.GetOrd( CRoomShape::Right )) / 2;
	int x = m_Shape.GetOrd( CRoomShape::Left );
	int y = (m_Shape.GetOrd( CRoomShape::LeftTop ) + m_Shape.GetOrd( CRoomShape::RightTop ) +
	 m_Shape.GetOrd( CRoomShape::LeftBottom ) + m_Shape.GetOrd( CRoomShape::RightBottom ) ) / 4;
	CPoint textPoint = context->WorldToWindow( CPoint( x, y ) );
	dc->TextOut( textPoint.x, textPoint.y, stream.str().c_str() );
//	dc->SelectObject( oldPen );
}

void CRoom::DrawHandles( CDC *dc, CViewParams const *context ) const
{
	CPoint points[4];
	points[0] = context->WorldToWindow( m_Shape.TopLeft() );
	points[1] = context->WorldToWindow( m_Shape.TopRight() );
	points[2] = context->WorldToWindow( m_Shape.BottomRight()  );
	points[3] = context->WorldToWindow( m_Shape.BottomLeft() );

	for( int i = 0; i < 4; ++i ) {
		CRect rect( points[i] - CSize( 2, 2 ), CSize( 5, 5 ) );
		dc->InvertRect( rect );
	}
}

bool CRoom::ContainsPoint(  CPoint point, int tolerance ) const
{
	return m_Shape.ContainsPoint( point, tolerance );
}

void CRoom::SnapWalls( CRoom const &other, int tolerance )
{
	m_Shape.SnapWalls( other.m_Shape, tolerance );
}

void CRoom::SnapFloors( CRoom const &other, int tolerance,
						   CPoint &leftTop, CPoint &rightTop,
						   CPoint &leftBottom, CPoint &rightBottom )
{
	m_Shape.SnapFloors( other.m_Shape, tolerance, leftTop, rightTop,
						   leftBottom, rightBottom );
}

void CRoom::SnapFloorTo( CPoint const &left, CPoint const &right )
{
	m_Shape.SnapFloorTo( left, right );
}

void CRoom::SnapCeilingTo( CPoint const &left, CPoint const &right )
{
	m_Shape.SnapCeilingTo( left, right );
}

bool CRoom::IsNeighbour( CRoom const &other, CPoint *first, CPoint *second, int *dir  ) const
{
	return m_Shape.IsNeighbour( other.m_Shape, first, second, dir );
}

void CRoom::Move( CPoint const &point, int mode )
{
	m_Shape.Move( point, mode );
}

void CRoom::Write( std::ostream &stream ) const
{
	m_Shape.Write( stream );
	stream << GetType() << '\n';
	stream << (const char *)m_Music << '\n';
}

CString CRoom::GetGameString( CString metaroom ) const
{
	CString ret;
	ret.Format( "  setv va00 addr %s %d %d %d %d %d %d\n"
		"    rtyp va00 %d\n"
		"    rmsc %d %d \"%s\"\n"
		"    setv game \"mapeditortmp_%d\" va00\n",
		(const char*)metaroom,
		m_Shape.GetOrd( CRoomShape::Left ),
		m_Shape.GetOrd( CRoomShape::Right ),
		m_Shape.GetOrd( CRoomShape::LeftTop ),
		m_Shape.GetOrd( CRoomShape::RightTop ),
		m_Shape.GetOrd( CRoomShape::LeftBottom ),
		m_Shape.GetOrd( CRoomShape::RightBottom ), GetType(),
		m_Shape.GetInsidePoint().x, m_Shape.GetInsidePoint().y, (const char *)CGame::Escape(m_Music),
		GetGameID());

	return ret;
}

CString CRoom::GetGameStringCleanUp() const
{
	CString ret;
	ret.Format( "delg \"mapeditortmp_%d\"\n", GetGameID());

	return ret;
}

void CRoom::LoadFromGame( int id, CGame &game )
{
	CString command;
	int type, left, right, leftTop, rightTop, leftBottom, rightBottom;

	std::stringstream stream( (const char *)game.Execute( "outv rtyp %d outs \" \" outs rloc %d", id, id ) );

	stream >> type >> left >> right >> leftTop >> rightTop >> leftBottom >> rightBottom;

	SetType( type );
	m_Shape.SetOrd( CRoomShape::Left, left );
	m_Shape.SetOrd( CRoomShape::Right, right );
	m_Shape.SetOrd( CRoomShape::LeftTop, leftTop );
	m_Shape.SetOrd( CRoomShape::RightTop, rightTop );
	m_Shape.SetOrd( CRoomShape::LeftBottom, leftBottom );
	m_Shape.SetOrd( CRoomShape::RightBottom, rightBottom );
	m_GameID = id;

}

void CRoom::Read( std::istream &stream, int version )
{
	m_Shape.Read( stream );
	if( version >= 1001 )
	{
		int type;
		stream >> type;
		SetType( type );
	}
	if( version >= 1002 )
	{
		stream.ignore( 10000, '\n' );
		std::string temp;
		std::getline( stream, temp );
		m_Music = temp.c_str();
	}
}

bool CRoomShape::GetCeilingEquation( double *m, double *c ) const
{
	return LineEquation( TopLeft(), TopRight(), m, c );
}

bool CRoomShape::GetFloorEquation( double *m, double *c ) const
{
	return LineEquation( BottomLeft(), BottomRight(), m, c );
}

void CRoomShape::Write( std::ostream &stream ) const
{
	stream << m_Left << ' ' << m_Right <<
		' ' << m_LeftTop << ' ' << m_LeftBottom <<
		' ' << m_RightTop << ' ' << m_RightBottom << '\n';
}

void CRoomShape::Read( std::istream &stream )
{
	stream >> m_Left >> m_Right >>
		m_LeftTop >> m_LeftBottom >>
		m_RightTop >> m_RightBottom;
}

bool Overlap( int l1, int l2, int r1, int r2, int *over1, int *over2 )
{
	*over1 = max( l1, r1 );
	*over2 = min( l2, r2 );
	return *over2 > *over1;
}

void CRoomShape::SnapWalls( CRoomShape const &other, int tolerance )
{
	int y1, y2;
	double mC, cC, mF, cF;

	if( Overlap( m_LeftTop, m_LeftBottom, other.m_RightTop, other.m_RightBottom,
		&y1, &y2 ) && abs( m_Left - other.m_Right ) < tolerance )
	{
		GetCeilingEquation( &mC, &cC );
		GetFloorEquation( &mF, &cF );
		m_Left = other.m_Right;
		m_LeftTop = m_Left * mC + cC + 0.5;
		m_LeftBottom = m_Left * mF + cF + 0.5;
		if( abs( m_LeftTop - other.m_RightTop ) < tolerance )
			m_LeftTop = other.m_RightTop;
		if( abs( m_LeftBottom - other.m_RightBottom ) < tolerance )
			m_LeftBottom = other.m_RightBottom;
	}

	if( Overlap( m_RightTop, m_RightBottom, other.m_LeftTop, other.m_LeftBottom,
		&y1, &y2 ) && abs( m_Right - other.m_Left ) < tolerance )
	{
		GetCeilingEquation( &mC, &cC );
		GetFloorEquation( &mF, &cF );
		m_Right = other.m_Left;
		m_RightTop = m_Right * mC + cC + 0.5;
		m_RightBottom = m_Right * mF + cF + 0.5;
		if( abs( m_RightTop - other.m_LeftTop ) < tolerance )
			m_RightTop = other.m_LeftTop;
		if( abs( m_RightBottom - other.m_LeftBottom ) < tolerance )
			m_RightBottom = other.m_LeftBottom;
	}

}

// This function only finds the leftmost and rightmost points which
// should be connected to this room-shape by floors or ceilings
// Call SnapFloorsTo with these points to do the actual adjustment.
// It does however snap walls.


void CRoomShape::SnapFloors( CRoomShape const &other, int tolerance,
						   CPoint &leftTop, CPoint &rightTop,
						   CPoint &leftBottom, CPoint &rightBottom )
{
	int x1, x2;
	double m, c, yLeftCeiling, yRightCeiling, yLeftFloor, yRightFloor;

	if( Overlap( m_Left, m_Right, other.m_Left, other.m_Right, &x1, &x2 ) )
	{
		// snap ceiling to other floor
		GetCeilingEquation( &m, &c );
		yLeftCeiling = x1 * m + c;
		yRightCeiling = x2 * m + c;

		other.GetFloorEquation( &m, &c );
		yLeftFloor = x1 * m + c;
		yRightFloor = x2 * m + c;

		if( fabs( yLeftCeiling -  yLeftFloor ) < tolerance &&
			fabs( yRightCeiling -  yRightFloor ) < tolerance )
		{
			if( x1 < leftTop.x ) leftTop = CPoint( x1, yLeftFloor + 0.5 );
			if( x2 > rightTop.x ) rightTop = CPoint( x2, yRightFloor + 0.5 );

			if( abs( m_Left - other.m_Left ) < tolerance )
				m_Left = other.m_Left;
			if( abs( m_Right - other.m_Right ) < tolerance )
				m_Right = other.m_Right;
//			m_LeftTop = m_Left * m + c + 0.5;
//			m_RightTop = m_Right * m + c + 0.5;
		}

		// snap floor to other ceiling
		GetFloorEquation( &m, &c );
		yLeftFloor = x1 * m + c;
		yRightFloor = x2 * m + c;

		other.GetCeilingEquation( &m, &c );
		yLeftCeiling = x1 * m + c;
		yRightCeiling = x2 * m + c;

		if( fabs( yLeftCeiling -  yLeftFloor ) < tolerance &&
			fabs( yRightCeiling -  yRightFloor ) < tolerance )
		{
			if( x1 < leftBottom.x ) leftBottom = CPoint( x1, yLeftCeiling + 0.5 );
			if( x2 > rightBottom.x ) rightBottom = CPoint( x2, yRightCeiling + 0.5 );

			if( abs( m_Left - other.m_Left ) < tolerance )
				m_Left = other.m_Left;
			if( abs( m_Right - other.m_Right ) < tolerance )
				m_Right = other.m_Right;
//			m_LeftBottom = m_Left * m + c + 0.5;
//			m_RightBottom = m_Right * m + c + 0.5;
		}
	}
}

void CRoomShape::SnapFloorTo( CPoint const &left, CPoint const &right )
{
	double m, c;
	LineEquation( left, right, &m, &c );

	m_LeftBottom = m * m_Left + c + 0.5;
	m_RightBottom = m * m_Right + c + 0.5;
}

void CRoomShape::SnapCeilingTo( CPoint const &left, CPoint const &right )
{
	double m, c;
	LineEquation( left, right, &m, &c );

	m_LeftTop = m * m_Left + c + 0.5;
	m_RightTop = m * m_Right + c + 0.5;
}

void CRoomShape::SnapInsideRect( CRect const &rect )
{
	if( m_Left < rect.left ) m_Left = rect.left;
	if( m_Right >= rect.right ) m_Right = rect.right - 1;
	if( m_LeftTop < rect.top ) m_LeftTop = rect.top;
	if( m_LeftBottom >= rect.bottom ) m_LeftBottom = rect.bottom - 1;
	if( m_RightTop < rect.top ) m_RightTop = rect.top;
	if( m_RightBottom >= rect.bottom ) m_RightBottom = rect.bottom - 1;
}


bool CRoomShape::IsNeighbour( CRoomShape const &other, CPoint *first, CPoint *second, int *dir ) const
{
	int y1, y2;
	const double tolerance = 1.0;

	if( Overlap( m_LeftTop, m_LeftBottom, other.m_RightTop, other.m_RightBottom,
		&y1, &y2 ) && ( m_Left == other.m_Right ) )
	{
		*first = CPoint( m_Left, y1 );
		*second = CPoint( m_Left, y2 );
		*dir = CDoor::Dir::Left;
		return true;
	}

	if( Overlap( m_RightTop, m_RightBottom, other.m_LeftTop, other.m_LeftBottom,
		&y1, &y2 ) && abs( m_Right == other.m_Left ) )
	{
		*first = CPoint( m_Right, y1 );
		*second = CPoint( m_Right, y2 );
		*dir = CDoor::Dir::Right;
		return true;
	}

	int x1, x2;
	double m, c, yLeft, yRight;

	if( Overlap( m_Left, m_Right, other.m_Left, other.m_Right, &x1, &x2 ) )
	{
		// snap ceiling to other floor
		GetCeilingEquation( &m, &c );
		yLeft = x1 * m + c;
		yRight = x2 * m + c;
		other.GetFloorEquation( &m, &c );

		if( fabs( yLeft - (x1 * m + c) ) <= tolerance && 
			fabs( yRight - (x2 * m + c) ) <= tolerance )
		{
			*first = CPoint( x1, yLeft + 0.5 );
			*second = CPoint( x2, yRight + 0.5 );
			*dir = CDoor::Dir::Up;
			return true;
		}

		// snap floor to other ceiling
		GetFloorEquation( &m, &c );
		yLeft = x1 * m + c;
		yRight = x2 * m + c;
		other.GetCeilingEquation( &m, &c );
		if( fabs( yLeft - (x1 * m + c) ) <= tolerance && 
			fabs( yRight - (x2 * m + c) ) <= tolerance )
		{
			*first = CPoint( x1, yLeft + 0.5 );
			*second = CPoint( x2, yRight + 0.5 );
			*dir = CDoor::Dir::Down;
			return true;
		}
	}
	return false;
}


// Tiny little struct for the following method: more effiicient than using
// CDoor
struct NewDoor{
	NewDoor( CPoint const &p1, CPoint const &p2, int dir )
		: m_p1( p1 ), m_p2( p2 ), m_Dir( dir ) {}
	CPoint m_p1, m_p2;
	int m_Dir;
};

void CRoomShape::CalcExtDoors( int index, CDoorSet const &internal, CDoorSet &external ) const
{
	std::vector< NewDoor > newDoors;
	newDoors.push_back( NewDoor( TopLeft(), TopRight(), CDoor::Dir::Up ) );
	newDoors.push_back( NewDoor( TopRight(), BottomRight(), CDoor::Dir::Right ) );
	newDoors.push_back( NewDoor( BottomLeft(), BottomRight(), CDoor::Dir::Down ) );
	newDoors.push_back( NewDoor( TopLeft(), BottomLeft(), CDoor::Dir::Left ) );

	CDoorSet::const_iterator itorDoors;
	std::vector< NewDoor >::iterator newItor;
	for( itorDoors = internal.begin(); itorDoors != internal.end(); ++itorDoors )
	{
		int dir;
		CPoint const &pi1 = (*itorDoors)->GetPoint1();
		CPoint const &pi2 = (*itorDoors)->GetPoint2();

		if( (*itorDoors)->GetRoom1() == index )
			dir = (*itorDoors)->GetDirection();
		else if( (*itorDoors)->GetRoom2() == index )
			dir = 3 - (*itorDoors)->GetDirection();
		else
			continue;

		// Note that internal door should only overlap one new door
		// so we break as soon as we find one (so we don't worry about
		// invalidating iterators)
		for( newItor = newDoors.begin(); newItor != newDoors.end(); ++newItor )
		{
			if( dir == newItor->m_Dir )
			{
				CPoint pe1 = newItor->m_p1, pe2 = newItor->m_p2;
				if( dir == CDoor::Dir::Left || dir == CDoor::Dir::Right )
				{
					int y1, y2;
					if( Overlap( pi1.y, pi2.y, pe1.y, pe2.y, &y1, &y2 ) )
					{
						newDoors.erase( newItor );
						if( y1 != pe1.y )
							newDoors.push_back( NewDoor( pe1, pi1, dir ) );
						if( y2 != pe2.y )
							newDoors.push_back( NewDoor( pi2, pe2, dir ) );
						break;
					}
				}
				else
				{
					int x1, x2;
					if( Overlap( pi1.x, pi2.x, pe1.x, pe2.x, &x1, &x2 ) )
					{
						newDoors.erase( newItor );
						if( x1 != pe1.x )
							newDoors.push_back( NewDoor( pe1, pi1, dir ) );
						if( x2 != pe2.x )
							newDoors.push_back( NewDoor( pi2, pe2, dir ) );
						break;
					}
				}
			}
		}
	}
	//Note we are reusing CDoorSet which requires doors to be uniquely 
	//identified by door1 & door2. We will therfore have to use a dummy 
	//value for door2, starting at index + 1 (so that room1 < room2)
	//and incrementing each time it is used.
	int dummy = index + 1;
	for( newItor = newDoors.begin(); newItor != newDoors.end(); ++newItor )
		external.insert( new CDoor( index, ++dummy, newItor->m_p1, newItor->m_p2, newItor->m_Dir, 0 ) );
}

bool CRoomShape::ContainsPoint(  CPoint point, int tolerance ) const
{
	CRect rect( m_Left, min( m_LeftTop, m_RightTop ),
		m_Right, max( m_LeftBottom, m_RightBottom ) );

	if( tolerance > 0 )
		rect.InflateRect( tolerance, tolerance );
	else
		rect.DeflateRect( 1, 1, 0, 0 );
	if( !rect.PtInRect( point ) ) return false;

	//Point is within Room's bounding box - now check if it is under the
	//ceiling and over the floor;
	double m, c;
	if( !GetCeilingEquation( &m, &c ) ) return false;
	if( point.y + tolerance < m * point.x + c ) return false;
	if( !GetFloorEquation( &m, &c ) ) return false;
	if( point.y - tolerance > m * point.x + c ) return false;
	return true;
}

void CRoomShape::Move( CPoint const &point, int mode )
{
	if( mode & Move_Left ) m_Left += point.x;
	if( mode & Move_Right ) m_Right += point.x;
	if( mode & Move_LeftTop ) m_LeftTop += point.y;
	if( mode & Move_LeftBottom ) m_LeftBottom += point.y;
	if( mode & Move_RightTop ) m_RightTop += point.y;
	if( mode & Move_RightBottom ) m_RightBottom += point.y;
	m_Perimeter = -1;
}

float CRoomShape::GetPerimeter() const
{
	if( m_Perimeter < 0 )
	{
		m_Perimeter = Magnitude( TopRight() - TopLeft() ) +
			Magnitude( BottomRight() - BottomLeft() ) +
			( m_LeftBottom - m_LeftTop ) +
			( m_RightBottom - m_RightTop );
	}
	return m_Perimeter;
}

CPoint CRoomShape::GetInsidePoint() const
{
	CPoint midPoint = (TopLeft() + BottomRight() ) / 2;
	return midPoint;
}

