#include "stdafx.h"
#include <windows.h>
#include <assert.h>

#include "FlexibleRegistry.h"

// This file was created to use instead of class in the existing Registry.h because it
// is rigidly set to read a specific hive.  I have not simply replaced the original file
// because it is extensively used in Creatures2. These both will hopefully get ripped out when
// evolution takes place and will be replaced with a better Registry handler.

// This class allows you to read any hive you specify 
//  it also makes sure that the keys are all closed

FlexibleRegistry::FlexibleRegistry(char* keypath, HKEY hive)
	{
	static char KeyClass[] = "";
	DWORD Opened;
	long err;

	err=RegCreateKeyEx(hive,
				 		keypath,
				 		0,KeyClass,REG_OPTION_NON_VOLATILE,
				 		KEY_ALL_ACCESS,NULL,
				 		&originalKey,&Opened);
	assert(err==ERROR_SUCCESS);   
	}

bool FlexibleRegistry::DoesKeyExist(char* keypath, HKEY hive)
	{
	HKEY key;
		if(RegOpenKeyEx(hive,keypath,0,KEY_ALL_ACCESS, &key) == ERROR_SUCCESS)
			{
			RegCloseKey(key);
			return true;
			}
		return false;
	}

bool FlexibleRegistry::CreateValue(char* key, char* value, char* data, DWORD type, HKEY hive)
	{

	static char KeyClass[] = "";
	DWORD Opened;
	long err2;
	DWORD dataSize = 200;
	
	err2 = RegCreateKeyEx(hive,
				 		key,
				 		0,KeyClass,REG_OPTION_NON_VOLATILE,
				 		KEY_ALL_ACCESS,NULL,
				 		&originalKey,&Opened);

	err2 = RegSetValueEx(originalKey,			// handle of key to set
							value,	// address of name of value to query
							NULL,			// reserved
							type,			// value type
							(BYTE*)data,		// address of data buffer
							strlen(data) + 1 );		
	RegCloseKey(originalKey);
	if(err2 != ERROR_SUCCESS)
		return false;
	else
		return true;
	}

bool FlexibleRegistry::CreateValue(char* key, char* value, DWORD data, DWORD type, HKEY hive)
	{

	static char KeyClass[] = "";
	DWORD Opened;
	long err2;
	DWORD dataSize = 200;
	
	err2 = RegCreateKeyEx(hive,
				 		key,
				 		0,KeyClass,REG_OPTION_NON_VOLATILE,
				 		KEY_ALL_ACCESS,NULL,
				 		&originalKey,&Opened);

	err2 = RegSetValueEx(originalKey,			// handle of key to set
							value,	// address of name of value to query
							NULL,			// reserved
							type,			// value type
							(BYTE*)&data,		// address of data buffer
							sizeof(int) );		
	RegCloseKey(originalKey);
	if(err2 != ERROR_SUCCESS)
		return false;
	else
		return true;
	}


bool FlexibleRegistry::GetValue(char* data, DWORD& dataSize, char* valueName, char* keyString, DWORD& type,
							 HKEY Hive)
	{
		bool rc = false;
		if (ERROR_SUCCESS == GetKey(keyString, Hive) && // Sets originalKey
			ERROR_SUCCESS == RegQueryValueEx(
								originalKey,	// handle of key to query
								valueName,	// address of name of value to query
								NULL,			// reserved
								&type,		// address of buffer for value type
								(BYTE*)data,			// address of data buffer
								&dataSize )) {
			rc = true;
		}
		RegCloseKey(originalKey);
		return rc;
	}

	// This one returns the key
long FlexibleRegistry::GetKey(char* Key, HKEY hive)
	{
		static char KeyClass[] = "";
	DWORD Opened;

	return RegCreateKeyEx(hive,
				 		Key,
				 		0,KeyClass,REG_OPTION_NON_VOLATILE,
				 		KEY_ALL_ACCESS,NULL,
				 		&originalKey,&Opened);
	}

