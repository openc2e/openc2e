#ifndef METAROOM_H_
#define METAROOM_H_

#include "handle.h"
#include "Room.h"
#include "Door.h"
#include "CARates.h"
#include <map>
#include <set>
#include <vector>

class CViewParams;
class CGame;
class CProgressDlg;
struct IDMap;

typedef handle<CBitmap> HBitmap;

class CMetaroom {
public:
	CMetaroom() {}
	CMetaroom( CRect const &rect, CString const &bitmapFilename );

	void Validate( std::set< int > *pRooms = 0 ) const;
// Attribute access
	CRect GetRect() const; //returns World size.
	void SetRect( CRect const &rect );

// Operations
	void Draw( CDC *dc, CViewParams *params ) const;
	void DrawHandles( CDC *dc, CViewParams *params ) const;

	bool AddRoom( int index, HRoom room );
	bool RemoveRoom( int index );
	bool HasRoom( int index ) const;
	HRoom GetRoom( int index ) const;
	int FindRoom( CPoint point, int tolerance ) const;
	int FindRooms( CRect const &rect, std::set< int > &set ) const;
	int GetRoomCount() const {return m_Rooms.size();}

	void SelectRoomWronguns( int Perm, int Height, std::set< int > &selection, 
		CDoorSet const &internal, CDoorSet const &external);

	void SnapRoom( CRoom *room, std::set<int> const *ignore, int tolerance ) const;

	void CalcDoors( CDoorSet &set ) const;
	void CalcExtDoors( CDoorSet const &internal, CDoorSet &external ) const;
	
	CSize LoadBackground( CString const &filename );
	CString GetCurrentBitmap() const { return m_CurrentBitmap; }
	void AddBackgroundBitmap( CString const &filename ) { m_BitmapFilenames.push_back( filename ); }
	void AddBackgroundBitmap( CString const &filename, int index ) { m_BitmapFilenames.insert( m_BitmapFilenames.begin() + index, filename ); }
	void RemoveBackgroundBitmap( int index ) { m_BitmapFilenames.erase(m_BitmapFilenames.begin() + index); }
	std::vector< CString > const &GetBackgroundList() const { return m_BitmapFilenames; }
	void SetBackgroundList( std::vector< CString > const &list ) { m_BitmapFilenames = list; }

// CA Functions
	void StepCA( std::vector< std::vector<CCARates> > const &rates );
	void StepCA2();
	void ReadCAValues( std::istream &stream, int index );
	void GetCACommand( CString &command );

// Music
	CString GetMusic() const {return m_Music;}
	void SetMusic( CString const &music ) {m_Music = music;}

// Persistance
	void Renumber( int &mBase, int &rBase );
	void Write( std::ostream &stream ) const;
	void Read( std::istream &stream, int version, CString &backgroundFailures );

	void SaveToGame( CGame &game, CProgressDlg &dlg ) const;
	void SaveToGameCleanUp( CGame &game, CProgressDlg &dlg ) const;
	void LoadFromGame( int id, CGame &game, CProgressDlg &dlg, CString &backgroundFailures );
	void DeleteFromGame( CGame &game, CProgressDlg &dlg ) const; // returns false if an agent is in the metaroom

	int GetGameID() const { return m_GameID; }
	void SetGameID( int id ) { m_GameID = id; }

	static CString CAOSCheckDeleteMetaRoomScript();
	static CString CAOSDeleteMetaRoomScript();

private:
	typedef std::map< int, HRoom > CRoomMap;
	CRect m_Rect;
	mutable HBitmap m_Bitmap;
	std::vector< CString > m_BitmapFilenames;
	CString m_CurrentBitmap;
	CRoomMap m_Rooms;
	CString m_Music;
	int m_GameID;
};

typedef handle<CMetaroom> HMetaroom;

#endif

