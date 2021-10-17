#include "stdafx.h"

#include <stdio.h>
#include "REException.h"
// Stolen from Ben Campbell

CREException::CREException( const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	WriteMessage( fmt, args );
	va_end( args );
}

void CREException::WriteMessage( const char *format, va_list argptr )
{
//	vsprintf( myMessage, format, argptr );
	myMessage.FormatV( format, argptr );
}

CGameException::CGameException( int errorNo, const char* fmt, ... )
: myErrorNo( errorNo )
{
	va_list args;
	va_start( args, fmt );
	WriteMessage( fmt, args );
	va_end( args );
}

