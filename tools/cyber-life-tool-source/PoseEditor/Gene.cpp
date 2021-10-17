#include "stdafx.h"

#include "Gene.h"

void CGene::WriteClipboard() const
{
	HGLOBAL hMem = GlobalAlloc( GMEM_DDESHARE | GMEM_MOVEABLE, 7 + m_Data.size() );
	if( hMem )
	{
		BYTE *pByte = (BYTE *)GlobalLock( hMem );
		if( pByte )
		{
			*pByte++ = m_Type;
			*pByte++ = m_SubType;
			*pByte++ = m_ID;
			*pByte++ = m_Generation;
			*pByte++ = m_SwitchOnTime;
			*pByte++ = m_Flags;
			*pByte++ = m_Mutability;
			for( int i = 0; i < m_Data.size(); ++i )
				*pByte++ = m_Data[i];
			GlobalUnlock( hMem );
			SetClipboardData( RegisterClipboardFormat( "PoseEditorGene" ), hMem );
		}
		else
			GlobalFree( hMem );
	}
}

bool CGene::ReadClipboard()
{
	HGLOBAL hMem = GetClipboardData( RegisterClipboardFormat( "PoseEditorGene" ) );
	if( hMem )
	{
		BYTE *pByte = (BYTE *)GlobalLock( hMem );
		if( pByte )
		{
			m_Type = *pByte++;
			m_SubType = *pByte++;
			m_ID = *pByte++;
			m_Generation = *pByte++;
			m_SwitchOnTime = *pByte++;
			m_Flags = *pByte++;
			m_Mutability = *pByte++;
			m_Data.resize( GlobalSize( hMem ) - 7 );
			for( int i = 0; i < m_Data.size(); ++i )
				m_Data[i] = *pByte++;
			GlobalUnlock( hMem );
			return true;
		}
	}
	return false;
}

bool CGene::CanPaste()
{
	return IsClipboardFormatAvailable( RegisterClipboardFormat( "PoseEditorGene" ) ) != 0;
}

void CGene::Dump( std::ostream &stream ) const
{
	stream
		<< int(  m_Type ) << ' '
		<< int(  m_SubType ) << ' '
		<< int(  m_ID ) << ' '
		<< int(  m_Generation ) << ' '
		<< int(  m_SwitchOnTime ) << ' '
		<< int(  m_Flags ) << ' '
		<< int(  m_Mutability );
	for( int i = 0; i < m_Data.size(); ++i )
		stream << ' ' << int( m_Data[i] );
	stream << '\n';
}

