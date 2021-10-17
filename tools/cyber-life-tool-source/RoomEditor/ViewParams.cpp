#include "stdafx.h"

#include "ViewParams.h"

CViewParams::CViewParams( CRect const &World, CRect const &Window, int Param,
		bool DrawDoors,
		bool DrawRooms,
		bool DrawBackground)
	: m_WorldRect( World ), m_WindowRect( Window ), m_Param( Param ),
	m_DrawDoors( DrawDoors ),
	m_DrawRooms( DrawRooms ),
	m_DrawBackground( DrawBackground )
{
	// Correct the world coordinates to maintain aspect ratio.
	if( !Window.Width() || !Window.Height() ) return;

	if( !World.Width() ) m_WorldRect.right += 1;
	if( !World.Height() ) m_WorldRect.bottom += 1;

	double WorldA = double( m_WorldRect.Height() ) / double( m_WorldRect.Width() );
	double WindowA = double( Window.Height() ) / double( Window.Width() );
	if( WorldA > WindowA )
	{
		int extraWidth = World.Height() / WindowA - World.Width();
		m_WorldRect.left -= extraWidth / 2;
		m_WorldRect.right += extraWidth / 2;
	}
	else
	{
		int extraHeight = World.Width() * WindowA  - World.Height();
		m_WorldRect.top -= extraHeight / 2;
		m_WorldRect.bottom += extraHeight / 2;
	}

	// Ensure scale is not too small
	if( m_WorldRect.Width() < m_WindowRect.Width() )
	{
		int extraWidth = m_WindowRect.Width() - m_WorldRect.Width();
		m_WorldRect.left -= extraWidth / 2;
		m_WorldRect.right += extraWidth / 2;
		int extraHeight = m_WindowRect.Height() - m_WorldRect.Height();
		m_WorldRect.top -= extraHeight / 2;
		m_WorldRect.bottom += extraHeight / 2;
	}

/*
	switch( m_Param )
	{
	case CRoom::Prop_Temperature:
	case CRoom::Prop_Pressure:
		m_Range.SetRange( "{0,0,0,255} {128,0,255,0} {255,255,0,0}" );
		break;
	case CRoom::Prop_LightLevel:
		m_Range.SetRange( "{0,0,0,0} {255,255,255,255}" );
		break;
	case CRoom::Prop_LightSource:
		m_Range.SetRange( "{0,0,0,0} {255,255,255,255}" );
		break;
	case CRoom::Prop_Radiation:
	case CRoom::Prop_RadiationSource:
		m_Range.SetRange( "{0,0,0,0} {32,128,128,0} {255,255,255,0}" );
		break;
	case CRoom::Prop_HeatSource:
	case CRoom::Prop_PressureSource:
		m_Range.SetRange( "{-255,0,0,255} {-32,0,128,128} {0,0,255,0} {32,128,128,0} {255,255,0,0}" );
		break;
	case CRoom::Prop_OrganicNutrient:
	case CRoom::Prop_InorganicNutrient:
		m_Range.SetRange( "{0,255,255,255} {255,0,255,0}" );
		break;
	case -2:
		m_Range.SetRange( "{0,255,255,255} {32,128,128,128} {255,0,0,0}" );
		break;
	default:
*/
	if( m_Param != -1 )
		m_Range.SetRange( "{0,0,0,0} {100,255,255,255}" );
}

int CViewParams::WorldToWindow( int Value ) const
{
	return MulDiv( Value, m_WindowRect.Width(), m_WorldRect.Width() );
}
int CViewParams::WindowToWorld( int Value ) const
{
	return MulDiv( Value, m_WorldRect.Width(), m_WindowRect.Width() );
}

CPoint CViewParams::WorldToWindow( CPoint const &Point ) const
{
	return TransformPoint( Point, m_WorldRect, m_WindowRect );
}
CPoint CViewParams::WindowToWorld( CPoint const &Point ) const
{
	return TransformPoint( Point, m_WindowRect, m_WorldRect );
}
CRect CViewParams::WorldToWindow( CRect const &Rect ) const
{
	return CRect( WorldToWindow( Rect.TopLeft() ), WorldToWindow( Rect.BottomRight() ) );
}
CRect CViewParams::WindowToWorld( CRect const &Rect ) const
{
	return CRect( WindowToWorld( Rect.TopLeft() ), WindowToWorld( Rect.BottomRight() ) );
}

COLORREF CViewParams::ParamColour( int Value ) const
{
	return m_Range.GetColour( Value );
}

COLORREF CViewParams::ParamColour( float Value ) const
{
	return m_Range.GetColour( Value );
}

CPoint CViewParams::TransformPoint( CPoint const &Point, CRect const &From, CRect const &To )
{
	return CPoint( MulDiv( Point.x - From.left, To.Width(), From.Width() ) + To.left ,
		MulDiv( Point.y - From.top, To.Height(), From.Height() ) + To.top );
}

bool CViewParams::PtNear( CPoint const &point1, CPoint const &point2 )
{
	int tol = WindowToWorld( 3 );
	CPoint point = point1 - point2;
	return abs( point.x ) < tol && abs( point.y ) < tol;
}

