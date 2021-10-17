#include "stdafx.h"

#include "Game.h"
#include "REException.h"
#include <stdarg.h>
#include <fstream>
#include "..\..\common\WhichEngine.h"

CGame::CGame(CString logName, bool execute)
: m_LogName( logName ), m_Execute( execute ) //, m_RoomCount( 0 ), m_MetaroomCount( 0 )
{
//	m_Client.SetTimeout( 20000 );
	if( m_Execute )
		if( !m_Client.Open( theWhichEngine.ServerName().c_str() ) )
			throw CREException( "Unable to establish a connection with the game." );
}


CGame::~CGame()
{
	if( m_Execute )
		m_Client.Close();
}

CString CGame::Execute( const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	CString command;
	command.FormatV( fmt, args );
	va_end( args );
	return Execute( command );
}

CString CGame::Execute( CString const &command )
{
	std::ofstream file( m_LogName, std::ios::app );
	file << (const char *)command << '\n';

	if( m_Execute )
	{
		CString executeCommand = "execute\n" + command;

		if( !m_Client.StartTransaction(
			(const unsigned char *)(const char *)executeCommand,
			executeCommand.GetLength() + 1 ) )
		{
			throw CREException( "Cannot execute \"%s\"", (const char *)command );
		}

		CString ret( (const char *)m_Client.GetResultBuffer(), m_Client.GetResultSize() );
		int error = m_Client.GetReturnCode();
		m_Client.EndTransaction();
		if( error )
		{
			throw CGameException( error, "Error: %s\nexecuting \"%s\"",
				(const char *)ret, (const char *)command  );
		}
		return ret;
	}
	return "";
}


CString CGame::Escape(CString const& str)
{
	CString newstr;
	for (int i = 0; i < str.GetLength(); ++i)
	{
		if (str[i] == '\\')
			newstr += '\\';
		newstr += str[i];
	}
	return newstr;
}
