
#ifndef FlexibleRegistry_h
#define FlexibleRegistry_h

// This file was created to use instead of class in the existing Registry.h because it
// is rigidly set to read a specific hive.  I have not simply replaced the original file
// because it is extensively used in Creatures2. These both will hopefully get ripped out when
// evolution takes place and will be replaced with a better Registry handler.

// This class allows you to read any hive you specify 
//  it also makes sure that the keys are all closed
const int MIN_BUF_LENGTH = 20;

class FlexibleRegistry{

	public:

		FlexibleRegistry(){};

		FlexibleRegistry(char* keypath, HKEY hive = HKEY_CURRENT_USER);
		
		bool CreateKey(char* keypath, DWORD& Opened);

		long GetKey(char* keypath, HKEY hive = HKEY_CURRENT_USER);

		bool OpenKey(char* keypath, HKEY hive = HKEY_CURRENT_USER);

		bool DuplicateRegistryKey(HKEY original, HKEY copy);

		bool DuplicateRegistryValues(HKEY original, HKEY copy, DWORD maxValueName, DWORD maxValueData);
 
		bool CreateValue(char* key, char* value, char* data, DWORD type = REG_SZ, HKEY Hive = HKEY_CURRENT_USER);
		bool CreateValue(char* key, char* value, DWORD data, DWORD type = REG_SZ, HKEY Hive = HKEY_CURRENT_USER);

		bool GetValue(char* data, DWORD& dataSize,char* valueName, char* keypath,
		DWORD& type, HKEY hive = HKEY_CURRENT_USER );

		bool DeleteKey(char* keypath, HKEY hive = HKEY_CURRENT_USER);

		bool DoesKeyExist(char* keypath, HKEY hive = HKEY_CURRENT_USER);

	private:
		HKEY originalKey;
		HKEY copyKey;
	};
#endif