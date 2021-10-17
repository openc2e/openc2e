// $Id: Error.cpp,v 1.1 2001/05/02 11:01:07 dcarter Exp $

// InstallLib - library of code for making Windows installation programs
// Copyright (c) 2001, CyberLife Technology Ltd.
// All rights reserved.
 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in
//    the documentation and/or other materials provided with the
//    distribution.
//  * Neither the name of CyberLife Technology Ltd. nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//    
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CYBERLIFE TECHNOLOGY 
// LTD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//__declspec(dllexport) int _errno;

#include "stdafx.h"
#include "Error.h"
#include "Translate.h"
#include <stdio.h>
#include <crtdbg.h>

char ourErrBuf[1024];	// used by LastErr()

InstallLib::Error::Wobbly::Wobbly( const char* msg )
{
	strcpy( myMessage, msg );
}


const char* InstallLib::Error::LastErr( DWORD err )
{
	char buffer[4096];
	buffer[0] = 0;
	if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, err, 0, buffer, 4095, NULL) == 0)
		sprintf(ourErrBuf, gettext("(error %d)"), err);
	else
		sprintf(ourErrBuf, "\n%s", buffer);
	return ourErrBuf;
}

const char* InstallLib::Error::LastErr()
{
	return LastErr( GetLastError() );
}


void InstallLib::Error::BrownTrousersTime( const char* fmt, ... )
{
	char buf[1024];
    va_list ap;
    va_start(ap, fmt);
	vsprintf( buf, fmt, ap);
    va_end(ap);

//	Log( buf );
	throw Wobbly( buf );
}
