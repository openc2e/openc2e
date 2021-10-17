// $Id: Registry.cpp,v 1.1 2001/05/02 11:01:07 dcarter Exp $

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
#ifdef _MSC_VER
#pragma warning(disable:4786 4503)
#endif

#include "Registry.h"
#include "Error.h"
#include "Platform.h"
#include "Translate.h"

InstallLib::Registry::Registry(const std::string& key)
:myKey(NULL)
{
	myDefaultKeyPath = key;
}

InstallLib::Registry::~Registry()
{
	if(myKey) 
	{
		RegCloseKey(myKey);
		myKey = NULL;
	}
}

HKEY InstallLib::Registry::MakeKey(HKEY hive /*= HKEY_CURRENT_USER*/)
{
	if(myKey) 
	{
		RegCloseKey(myKey);
		myKey = NULL;
	}

	static char KeyClass[] = "";
	unsigned long opened;
	int err;

	err=RegCreateKeyEx(hive,
				 		myDefaultKeyPath.c_str(),
				 		0,KeyClass,REG_OPTION_NON_VOLATILE,
				 		KEY_ALL_ACCESS,NULL,
				 		&myKey,&opened);
	return myKey;
}

HKEY InstallLib::Registry::GetKey(HKEY hive /*= HKEY_CURRENT_USER*/)
{
	if(myKey) 
	{
		RegCloseKey(myKey);
		myKey = NULL;
	}

	int err = RegOpenKeyEx(hive,
				 	myDefaultKeyPath.c_str(),
				 	0,KEY_ALL_ACCESS,
				 	&myKey);
	return myKey;
}

bool InstallLib::Registry::CreateValue(const std::string& value,
						   const std::string& data,
							HKEY hive)/* = HKEY_CURRENT_USER */
{
	int err2;
	DWORD dataSize = 200;

	MakeKey(hive);

	if(myKey)
	{
		err2 = RegSetValueEx(myKey,			// handle of key to set
							value.c_str(),	// address of name of value to query
							NULL,			// reserved
							REG_SZ,			// value type string
							(BYTE*)data.c_str(),		// address of data buffer
							data.size() +1 ); // must add null terminator		
		RegCloseKey(myKey);
		myKey = NULL;
	}
	else
		return false;

	if(err2 != ERROR_SUCCESS)
		return false;
	else
		return true;
}

bool InstallLib::Registry::CreateValue(const std::string& value,
								   const int& data,
								   HKEY hive)/* = HKEY_CURRENT_USER */
{
	long err2;
	DWORD dataSize = sizeof(unsigned int);
	MakeKey(hive);

	if(myKey)
	{
		err2 =	RegSetValueEx(myKey,			// handle of key to set
							value.c_str(),	// address of name of value to query
							NULL,			// reserved
							REG_DWORD,			// value type
							(BYTE*)&data,		// address of data buffer
							dataSize );	
	
		RegCloseKey(myKey);
		myKey = NULL;
	}
	else
		return false;

	if(err2 != ERROR_SUCCESS)
		return false;
	else
		return true;
}

bool InstallLib::Registry::CreateValue(const std::string& value,
							const bool& data,
							HKEY hive)
{
	int boolean = data ? 1 : 0;
	return CreateValue(value, boolean, hive);
}

bool InstallLib::Registry::GetValue(const std::string& valueName,
						std::string& data,
						HKEY hive) /* = HKEY_CURRENT_USER */
{
	char buf[_MAX_PATH];
	DWORD dataSize = _MAX_PATH;
	DWORD type = REG_SZ;

	int err;
	myKey = GetKey(hive);

	if(myKey)
	{
		err = RegQueryValueEx(myKey,	// handle of key to query
								valueName.c_str(),	// address of name of value to query
								NULL,			// reserved
								&type,		// address of buffer for value type
								(BYTE*)buf,			// address of data buffer
								&dataSize );
		RegCloseKey(myKey);
		myKey = NULL;
	}
	else
		return false;

	if(err != ERROR_SUCCESS)
	{
		CreateValue(valueName, data, hive);
		return false;
	}
	else
	{
		data = buf;
		return true;
	}
}

bool InstallLib::Registry::GetValue(const std::string& valueName,
						int& data,	
						HKEY hive) /* = HKEY_CURRENT_USER */
{
	unsigned int defaultData = data;

	DWORD type;//= REG_DWORD;

	DWORD dataSize = sizeof(unsigned int);
	int err;
	myKey = GetKey(hive);

	if(myKey)
	{
		err = RegQueryValueEx(myKey,	// handle of key to query
								valueName.c_str(),	// address of name of value to query
								NULL,			// reserved
								&type,		// address of buffer for value type
								(BYTE*)&data,			// address of data buffer
								&dataSize );
		RegCloseKey(myKey);
		myKey = NULL;
	}
	else 
		return false;

	if(err != ERROR_SUCCESS)
	{
		CreateValue(valueName, data, hive);

		return false;
	}
	else
		return true;
}

bool InstallLib::Registry::GetValue(const std::string& valueName,
						bool& data,	
						HKEY hive)
{
	int boolean = data ? 1 : 0;
	if (GetValue(valueName, boolean, hive))
	{
		data = (boolean == 0) ? false : true;
		return true;
	}
	else
		return false;
}

// Taken from Microsoft Knowledgebase article Q142491
DWORD InstallLib::Registry::RegDeleteKeyNT(HKEY hStartKey, LPTSTR pKeyName )
{
	DWORD   dwRtn, dwSubKeyLength;
	LPTSTR  pSubKey = NULL;
	// (256) this should be dynamic.
	#define MAX_KEY_LENGTH 256
	TCHAR   szSubKey[MAX_KEY_LENGTH];
	HKEY    hKey;
	
	// Do not allow NULL or empty key name
	if ( pKeyName &&  lstrlen(pKeyName))
	{
		if( (dwRtn=RegOpenKeyEx(hStartKey,pKeyName,
			0, KEY_ENUMERATE_SUB_KEYS | DELETE, &hKey )) == ERROR_SUCCESS)
		{
			while (dwRtn == ERROR_SUCCESS )
			{
				dwSubKeyLength = MAX_KEY_LENGTH;
				dwRtn=RegEnumKeyEx(
					hKey,
					0,       // always index zero
					szSubKey,
					&dwSubKeyLength,
					NULL,
					NULL,
					NULL,
					NULL
					);
				
				if(dwRtn == ERROR_NO_MORE_ITEMS)
				{
					dwRtn = RegDeleteKey(hStartKey, pKeyName);
					break;
				}
				else if(dwRtn == ERROR_SUCCESS)
					dwRtn=RegDeleteKeyNT(hKey, szSubKey);
			}
			RegCloseKey(hKey);
			// Do not save return code because error
			// has already occurred
		}
	}
	else
		dwRtn = ERROR_BADKEY;
	
	return dwRtn;
}
 
void InstallLib::Registry::DeleteEntireKey(const std::string& key, HKEY hive)
{
	myKey = GetKey(hive);

	if (!myKey)
		return;

	// RegDeleteKey isn't recursive on NT/Win2K, so we call a special 
	// recursive one (can't use SHDeleteKey as that requires IE4 
	// on NT 4 _and_ we would have to import it manually depending 
	// on the OS)
	DWORD ret;
	if (Platform::IsWinNT2000())
		ret = RegDeleteKeyNT(myKey, (char*)key.c_str());
	else
		ret = RegDeleteKey(myKey, key.c_str());

	if (ret != ERROR_SUCCESS)
	{
		Error::BrownTrousersTime(gettext("Failed to delete registry key\n%s\\%s"),
			myDefaultKeyPath.c_str(), key.c_str());
	}
	RegCloseKey(myKey);

}

void InstallLib::Registry::QuietlyDeleteEntireKey(const std::string& key, HKEY hive)
{
	// Ignore errors
	try 
	{
		DeleteEntireKey(key, hive);
	}
	catch (Error::Wobbly&)
	{
	}
}

