// $Id: Platform.cpp,v 1.1 2001/05/02 11:01:07 dcarter Exp $

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

#include "stdafx.h"
#include <windows.h>
#include "Error.h"
#include "Platform.h"
#include "Translate.h"

// Keep this here to avoid polluting the header
// with includes for OSVERSIONINFO 
static GetOSVersion(OSVERSIONINFO& osv)
{
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if( !GetVersionEx(&osv) )
		InstallLib::Error::BrownTrousersTime( gettext("GetVersionEx() failed!") );
}

bool InstallLib::Platform::IsWinNT2000()
{
	OSVERSIONINFO osv;
	GetOSVersion(osv);
	return (osv.dwPlatformId == VER_PLATFORM_WIN32_NT);
}

bool InstallLib::Platform::IsWinNT()
{
	OSVERSIONINFO osv;
	GetOSVersion(osv);
	return (osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
		&& (osv.dwMajorVersion <= 4);
}

