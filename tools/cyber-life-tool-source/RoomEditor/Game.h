#include "..\..\common\clientside.h"

#include <map>

/*
typedef std::map< int, int > CIntMap;

struct IDMap {
	CIntMap metaroomMap;
	CIntMap roomMap;
};
*/

class CGame {
public:
	CGame( CString logName = "game.log", bool execute = true );
	~CGame();
	CString Execute( CString const &command );
	CString Execute( const char* fmt, ... );
//	int AddRoom() {return m_RoomCount++;}
//	int AddMetaroom() {return m_MetaroomCount++;}
	static CString Escape(CString const& str);

	bool ExecutingLive() { return m_Execute; }

private:
	ClientSide m_Client;
	CString m_LogName;
	bool m_Execute;
//	int m_RoomCount;
//	int m_MetaroomCount;
};

