#include <windows.h>
#include <string>

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// Find directory
	HKEY key;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				 	"Software\\CyberLife Technology\\Sea-Monkeys",
				 	0,KEY_READ,
				 	&key) != ERROR_SUCCESS)
	{
		MessageBox(NULL, "RegOpenKeyEx Software\\CyberLife Technology\\Sea-Monkeys failed", "Creature Engine Screen Saver", MB_OK);
		return 0;
	}
	DWORD type = REG_SZ;
	char buf[_MAX_PATH];
	DWORD dataSize = _MAX_PATH;
	if (RegQueryValueEx(key,	// handle of key to query
						"Main Directory",	// address of name of value to query
						NULL,			// reserved
						&type,		// address of buffer for value type
						(BYTE*)buf,			// address of data buffer
						&dataSize ) != ERROR_SUCCESS)
	{
		MessageBox(NULL, "RegQueryValueEx Main Directory failed", "Creature Engine Screen Saver", MB_OK);
		return 0;
	}
	RegCloseKey(key);

	std::string curdir = buf;
	std::string cmd = curdir + std::string("engine.exe ") + lpCmdLine + " Sea-Monkeys";

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset( &si, 0, sizeof(si) );
	si.cb = sizeof(si);

	// MessageBox(NULL, cmd.c_str(), "Creature Engine Screen Saver", MB_OK);

	if( !CreateProcess(NULL, (char*)cmd.c_str(), NULL, NULL, false,
		0, NULL, curdir.c_str(), &si, &pi ) )
	{
		std::string message = "Screen saver stub launcher failed to run " + cmd;
		MessageBox(NULL, message.c_str(), "Creature Engine Screen Saver", MB_OK);
	}

	// Wait for it to finish
	WaitForSingleObject( pi.hProcess, INFINITE );

	// This could be improved to return the return code
	CloseHandle( pi.hProcess );

	return 0;
}

