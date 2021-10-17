#include "stdafx.h"

#include "ColourRange.h"

bool CColourRange::SetRange( CString const &NewValues )
{
	int LPos = 0, RPos = 0;
	m_ColourValues.clear();
	while( LPos < NewValues.GetLength() )
	{
		while( LPos < NewValues.GetLength() && NewValues[LPos] != '{' ) ++LPos;
		RPos = LPos + 1;
		while( RPos < NewValues.GetLength() && NewValues[RPos] != '}' ) ++RPos;
		if( RPos < NewValues.GetLength() )
		{
			CString PointStr( NewValues.Mid( LPos, RPos - LPos + 1 ) );
			int v, r, g, b;
			if( sscanf( PointStr, "{ %d, %d, %d, %d }", &v, &r, &g, &b ) == 4 )
				m_ColourValues.push_back( CColourValue( v, r, g, b ) );
			else
				return false;
		}
		LPos = RPos + 1;
	}
	return true;
}

CString CColourRange::GetRange() const
{
	CString Ret;
	std::vector<CColourValue>::const_iterator itor;
	for( itor = m_ColourValues.begin(); itor != m_ColourValues.end(); ++itor )
	{
		CString Point;
		Point.Format( "{%d, %d, %d, %d} ",
			itor->m_Value, itor->m_Red, itor->m_Green, itor->m_Blue );
		Ret += Point;
	}
	return Ret;
}

COLORREF CColourRange::GetColour( int Value ) const
{
	CColourValue Left( -255, 0, 0, 0 );
	CColourValue Right( 255, 255, 255, 255 );
	int i, NValues = m_ColourValues.size();

	for( i = 0; i < NValues; ++i )
		if( m_ColourValues[ i ].m_Value <= Value ) Left = m_ColourValues[ i ];

	for( i = NValues - 1; i >= 0; --i )
		if( m_ColourValues[ i ].m_Value >= Value ) Right = m_ColourValues[ i ];


	int V = Value - Left.m_Value;
	int Div = Right.m_Value - Left.m_Value;
	if( Div != 0 )
		return RGB(
			MulDiv(	V, Right.m_Red - Left.m_Red, Div ) + Left.m_Red,
			MulDiv( V, Right.m_Green - Left.m_Green, Div ) + Left.m_Green,
			MulDiv( V, Right.m_Blue - Left.m_Blue, Div ) + Left.m_Blue
			);
	else
		return RGB( Right.m_Red, Right.m_Green, Right.m_Blue );
}

COLORREF CColourRange::GetColour( float Value ) const
{
	CColourValue Left( -255, 0, 0, 0 );
	CColourValue Right( 255, 255, 255, 255 );
	int i, NValues = m_ColourValues.size();

	for( i = 0; i < NValues; ++i )
		if( m_ColourValues[ i ].m_Value <= Value ) Left = m_ColourValues[ i ];

	for( i = NValues - 1; i >= 0; --i )
		if( m_ColourValues[ i ].m_Value >= Value ) Right = m_ColourValues[ i ];


	float V = Value - Left.m_Value;
	float Div = Right.m_Value - Left.m_Value;
	if( Div != 0 )
		return RGB(
			V*(Right.m_Red - Left.m_Red)/Div + Left.m_Red,
			V*(Right.m_Green - Left.m_Green)/Div + Left.m_Green,
			V*(Right.m_Blue - Left.m_Blue)/Div + Left.m_Blue
			);
	else
		return RGB( Right.m_Red, Right.m_Green, Right.m_Blue );
}

