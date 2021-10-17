// $Id: Registry.h,v 1.1 2001/05/02 11:01:08 dcarter Exp $

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

#ifndef INSTALL_LIB_REGISTRY_HANDLER_H
#define INSTALL_LIB_REGISTRY_HANDLER_H

#ifdef _MSC_VER
#pragma warning(disable:4786 4503)
#endif

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "API.h"

namespace InstallLib
{
	class INSTALL_LIB_API Registry
	{
	public:
		virtual ~Registry();
		// TODO: Make registry take hive in constructor?
		Registry(const std::string& key);

		bool CreateValue(const std::string& value, const int& data, HKEY hive = HKEY_CURRENT_USER);
		bool CreateValue(const std::string& value, const std::string& data, HKEY hive = HKEY_CURRENT_USER);
		bool CreateValue(const std::string& value, const bool& data, HKEY hive = HKEY_CURRENT_USER);

		bool GetValue(const std::string& valueName, int& data, HKEY hive = HKEY_CURRENT_USER);
		bool GetValue(const std::string& valueName, std::string& data, HKEY hive = HKEY_CURRENT_USER);
		bool GetValue(const std::string& valueName, bool& data, HKEY hive = HKEY_CURRENT_USER);

		void DeleteEntireKey(const std::string& key, HKEY hive = HKEY_CURRENT_USER);
		void QuietlyDeleteEntireKey(const std::string& key, HKEY hive = HKEY_CURRENT_USER);
		
	private:		
		static DWORD RegDeleteKeyNT(HKEY hStartKey, LPTSTR pKeyName );

#ifdef _MSC_VER
#pragma warning (disable : 4251)
#endif
		HKEY GetKey(HKEY hive = HKEY_CURRENT_USER);
		HKEY MakeKey(HKEY hive = HKEY_CURRENT_USER);

		HKEY myKey;
		std::string myDefaultKeyPath;
#ifdef _MSC_VER
#pragma warning (default : 4251)
#endif
	};
};

#endif // REGISTRY_HANDLER_H
