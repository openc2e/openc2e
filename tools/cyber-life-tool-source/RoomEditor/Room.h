#ifndef ROOM_H_
#define ROOM_H_

#include <ostream>
#include <istream>
#include <vector>

#include "handle.h"
#include "Door.h"

class CRect;
class CViewParams;
class CDC;

const int c_NumProperties = 32;
class CRoom;
class CGame;
struct IDMap;

class CRoomShape {
//	friend class CRoom;
public:
	CRoomShape() {m_Perimeter = -1;}
	// ---------------------------------------------------------------------
	// Method:		TopLeft()... BottomRight()
	// Arguments:	None
	// Returns:		Corresponding point
	// Description:	I HOPE these are self explanatory
	// ---------------------------------------------------------------------
	inline CPoint TopLeft() const { return CPoint( m_Left, m_LeftTop ); }
	inline CPoint TopRight() const { return CPoint( m_Right, m_RightTop ); }
	inline CPoint BottomLeft() const { return CPoint( m_Left, m_LeftBottom ); }
	inline CPoint BottomRight() const { return CPoint( m_Right, m_RightBottom ); }
	CPoint GetInsidePoint() const;
	
	bool ContainsPoint(  CPoint point, int tolerance ) const;

	enum{
		Move_Left = 1,
		Move_Right = 2,
		Move_LeftTop = 4,
		Move_LeftBottom = 8,
		Move_RightTop = 16,
		Move_RightBottom = 32,
		Move_All = 63
	};
	void Move( CPoint const &point, int mode = Move_All );

	// enum for GetOrd() & SetOrd()
	enum Ord {
		Left,
		Right,
		LeftTop,
		RightTop,
		LeftBottom,
		RightBottom
	};

	// ---------------------------------------------------------------------
	// Method:		GetOrd
	// Arguments:	index - index of value to get (use Ord enum)
	// Returns:		corresponding point
	// Description:	Allows access to the shape dimensions as an array
	//				This facilitates CRoom treating them as properties
	// ---------------------------------------------------------------------
	inline int GetOrd( int index ) const {return (&m_Left)[index];}

	// ---------------------------------------------------------------------
	// Method:		SetOrd
	// Arguments:	index - index of value to get (use Ord enum)
	//				value - new value for dimension
	// Returns:		corresponding point
	// Description:	Allows access to the shape dimensions as an array
	//				This facilitates CRoom treating them as properties
	// ---------------------------------------------------------------------
	inline void SetOrd( int index, int value ) { (&m_Left)[index] = value; m_Perimeter = -1;}
	
	// ---------------------------------------------------------------------
	// Method:		SnapWalls
	// Arguments:	other		The other shape to compare and snap to
	//				tolerance	Walls will be snapped to other walls
	//							within this distance
	// Returns:		None
	// Description:	
	// ---------------------------------------------------------------------
	void SnapWalls( CRoomShape const &other, int tolerance );

	// ---------------------------------------------------------------------
	// Method:		SnapFloors
	// Arguments:	other		the other shape to compare and snap to
	//				tolerance	walls will be snapped to other walls
	//							within this distance
	//				leftTop...rightBottom (Out)
	//							leftmost and rightmost points which
	//							this room should snap to
	// Returns:		None
	// Description:	
	//		This function only finds the leftmost and rightmost points which
	//		should be connected to this room-shape by floors or ceilings
	//		Call SnapFloorsTo with these points to do the actual adjustment.
	//		It does however snap walls.
	// ---------------------------------------------------------------------
	void SnapFloors( CRoomShape const &other, int tolerance,
				     CPoint &leftTop, CPoint &rightTop,
				     CPoint &leftBottom, CPoint &rightBottom );

	// ---------------------------------------------------------------------
	// Method:		SnapFloorTo
	// Arguments:	left & right - points to snap to
	// Returns:		None
	// Description:	Adjusts BottomLeft & BottomRight such that the equation
	//				of the floor line segment is the same as the equation
	//				of the line connecting the points passed in
	// ---------------------------------------------------------------------
	void SnapFloorTo( CPoint const &left, CPoint const &right );

	// ---------------------------------------------------------------------
	// Method:		SnapCeilingTo
	// Arguments:	left & right - points to snap to
	// Returns:		None
	// Description:	Adjusts TopLeft & TopRight such that the equation
	//				of the ceiling line segment is the same as the equation
	//				of the line connecting the points passed in
	// ---------------------------------------------------------------------
	void SnapCeilingTo( CPoint const &left, CPoint const &right );
	void SnapInsideRect( CRect const &rect );

	bool IsNeighbour( CRoomShape const &other, CPoint *first, CPoint *second, int *dir ) const;
	bool GetCeilingEquation( double *m, double *c ) const;
	bool GetFloorEquation( double *m, double *c ) const;

	void CalcExtDoors( int index, CDoorSet const &internal, CDoorSet &external ) const;

	float GetPerimeter() const;

// Persistance
	void Write( std::ostream &stream ) const;
	void Read( std::istream &stream );

private:
	int m_Left;
	int m_Right;
	int m_LeftTop;
	int m_RightTop;
	int m_LeftBottom;
	int m_RightBottom;
	mutable float m_Perimeter;
};

class CRoom {
public:
	CRoom() : m_Properties( 32, 0 ), m_CAValue( 0 ), m_CAInput( 0 ) {}
//	CRoom( CRect const &rect );
	CRoom( CPoint const *points );

	void Draw( CDC *dc, CViewParams const *context ) const;
	void Fill( CDC *dc, CViewParams const *context ) const;
	void DrawData( CDC *dc, CViewParams const *context, float value ) const;
	void DrawHandles( CDC *dc, CViewParams const *context ) const;

	CRoomShape const &GetShape() const { return m_Shape; }
	void SetShape( CRoomShape const &shape ) { m_Shape = shape; }

	bool ContainsPoint(  CPoint point, int tolerance ) const;

	void SnapWalls( CRoom const &other, int tolerance );
	void SnapFloors( CRoom const &other, int tolerance,
				     CPoint &leftTop, CPoint &rightTop,
				     CPoint &leftBottom, CPoint &rightBottom );
	void SnapFloorTo( CPoint const &left, CPoint const &right );
	void SnapCeilingTo( CPoint const &left, CPoint const &right );

	bool IsNeighbour( CRoom const &other, CPoint *first, CPoint *second, int *dir ) const;

	float GetDoorage() const { return m_Doorage; }
	void SetDoorage( float doorage ) { m_Doorage = doorage; }

	void Move( CPoint const &point, int mode = CRoomShape::Move_All );

	int GetProperty( int index ) const;
	void SetProperty( int index, int value );

	int GetType() const {return GetProperty( 6 ); }
	void SetType( int type ) { SetProperty( 6, type ); }
// Persistance
	void Write( std::ostream &stream ) const;
	void Read( std::istream &stream, int version );

	// Music
	CString GetMusic() const {return m_Music;}
	void SetMusic( CString const &music ) {m_Music = music;}

	CString GetGameString( CString ) const;
	CString GetGameStringCleanUp() const;
	void LoadFromGame( int id, CGame &game );

	float m_CAValue;
	float m_CAInput;
	float m_TempCAValue;

	int GetGameID() const { return m_GameID; }
	void SetGameID( int id ) { m_GameID = id; }

private:
	CRoomShape m_Shape;
	std::vector<int> m_Properties;
	float m_Doorage; //sum of opening * door length for all doors
	CString m_Music;
	int m_GameID;
};

typedef handle< CRoom > HRoom;

#endif // ROOM_H_

