// Class representing World (list of Metarooms)

#include "Metaroom.h"
#include "Door.h"
#include "Cheese.h"

#include <map>

class CViewParams;
class CGame;
class CProgressDlg;
struct IDMap;

class CWorld {
public:
	CWorld();

// Attribute access
	CSize GetSize() const; //returns World size.
	void SetSize( CSize const &size );

	int GetMetaroomIndexBase() const {return m_MetaroomIndexBase;}
	void SetMetaroomIndexBase( int base ) {m_MetaroomIndexBase = base;}
	int GetRoomIndexBase() const {return m_RoomIndexBase;}
	void SetRoomIndexBase( int base ) {m_RoomIndexBase = base;}

	void Validate( std::set< int > *pRooms = 0, CDoorSet *pDoorSet = 0 );

// Metaroom functions
	int GetMetaroomCount() { return m_Metarooms.size(); }
	int AddMetaroom( CString const &bitmapName, CRect const &rect ); //returns new index
	bool AddMetaroom( int index, HMetaroom const &metaroom ); //returns true on success
	bool RemoveMetaroom( int index ); //returns true on success
	HMetaroom GetMetaroom( int index ) const;//returns handle of Metaroom, Null handle if it doesn't exist
	int FindMetaroom( CPoint point, int tolerance ) const; //returns index of Metaroom under point (or within tolerance) or -1 if no Metaroom is found
	std::vector<int> GetMetaroomIndices() const;

// Room functions
	int AddRoom( CPoint const *points );
	bool AddRoom( int index, HRoom const &room );
	bool RemoveRoom( int index );
	bool HasRoom( int Index );
	HRoom GetRoom( int index ) const;//returns handle of Room, Null handle if it doesn't exist
	int FindRoom( CPoint point, int tolerance ) const;
	int FindRooms( CRect const &rect, std::set< int > &set ) const;
	 
	void SnapRoom( CRoom *room, std::set<int> const *ignore, int tolerance, int metaroomIndex = -1 ) const;
	int GetRoomMetaroom( int room ) const;
	int GetRoomCount() const;

// Door functions
	CDoorSet const &GetDoorSet() const {return m_DoorSet;}
	HDoor FindDoor( CPoint point, int tolerance ) const;
	HDoor GetDoor( int room1, int room2 ) const;
	void CalcDoors();
	void UpdateDoors() const;

// Operations
	void Draw( CDC *dc, CViewParams *params ) const;
	void SelectRoomWronguns( int Perm, int Height, std::set< int > &selection );

// CA Functions
	void StepCA( std::vector< std::vector<CCARates> > const &rates,
					 std::vector< CCheese > const &cheeses  );
	void ReadCAValues( std::istream &stream, int index );
	void GetCACommand( CString &command );

// Persistance
	void Renumber();
	void Write( std::ostream &stream ) const;
	void Read( std::istream &stream, int version );

	void SaveToGame( CGame &game, CDoorSet &doorErrors, CProgressDlg &dlg, bool addon );
	void LoadFromGame( CGame &game, CDoorSet &doorErrors, CProgressDlg &dlg );
private:
	typedef std::map< int, HMetaroom > CMetaroomMap;
	CSize m_Size;
	CMetaroomMap m_Metarooms;
	int m_MetaroomIndexBase;
	int m_RoomIndexBase;
	// These must be mutable for 'lazy' evaluation
	mutable CDoorSet m_DoorSet;
	mutable CDoorSet m_ExtDoorSet;
	mutable CDoorSet m_UnusedDoorSet;
	mutable bool m_DoorsUpToDate;
};

