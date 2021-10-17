// $Id: Error.h,v 1.1 2001/05/02 11:01:07 dcarter Exp $

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

#ifndef INSTALL_LIB_ERROR_H
#define INSTALL_LIB_ERROR_H

#include <windows.h>
#include "API.h"

//_CRTIMP extern int * __cdecl _errno(void);
//#define errno       (*_errno())
//extern __declspec(dllexport) int _errno;

namespace InstallLib
{
	class INSTALL_LIB_API Error
	{
	public:
		class Wobbly
		{
		public:
			Wobbly( const char* msg );
			const char* what()
				{ return myMessage; }
		private:
			char myMessage[1024];
		};

		static void BrownTrousersTime( const char* fmt, ... );
		static const char* LastErr( DWORD err );
		static const char* LastErr();

	private:
	};
};

#endif
