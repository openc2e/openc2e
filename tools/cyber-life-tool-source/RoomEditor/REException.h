// -------------------------------------------------------------------------
// Filename:    CAOSException.h
// Class:       CAOSException
// Purpose:
// Description:
//
//
// Usage:
//
//
// History:
// -------------------------------------------------------------------------
// Stolen from Ben Campbell
#ifndef CREEXCEPTION_H
#define CREEXCEPTION_H

#include <stdarg.h>

class CREException
{
public:
	CREException( const char* fmt, ... );
	CREException()
	{
	// myMessage[0] = '\0';
	}

	const char* what() const
		{ return myMessage; }
protected:
	void WriteMessage( const char *format, va_list argptr );

private:
//	char myMessage[512];
	CString myMessage;
};

class CGameException : public CREException
{
public:
	CGameException( int errorNo, const char* fmt, ... );

	int error() const
		{ return myErrorNo; }
private:
	int myErrorNo;
};

#endif // CREEXCEPTION_H


