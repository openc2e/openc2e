#ifndef DOOR_H_
#define DOOR_H_

#include <set>
#include "handle.h"
#include "utils.h"

class CDoor {
public:
	CDoor( int room1, int room2,
		CPoint const &first, CPoint const &second, int dir,
		int opening = 100 ) : m_First( first ), m_Second( second ),
		m_Direction( dir ), m_Opening( opening )
	{
		if( room1 < room2 )
		{
			m_Room1 = room1;
			m_Room2 = room2;
		}
		else
		{
			m_Room1 = room2;
			m_Room2 = room1;
			m_Direction = 3 - dir;
		}
		m_Length = Round( Magnitude( m_First - m_Second ) );
	}

	enum Dir
	{
		Left = 0,	// Note these values are cunningly chosen so that 3 - dir
		Up,			// reverses direction.
		Down,
		Right
	};
	inline int GetRoom1() const { return m_Room1; }
	inline int GetRoom2() const { return m_Room2; }

	inline CPoint const &GetPoint1() const {return m_First;}
	inline CPoint const &GetPoint2() const {return m_Second;}

	int GetOpening() const { return m_Opening; }
	int GetDirection() const { return m_Direction; }
	int GetLength() const { return m_Length; }

	void SetOpening( int opening ) {m_Opening = opening;}

private:
	int m_Room1;
	int m_Room2;
	int m_Opening;
	int m_Direction;
	int m_Length;
	CPoint m_First;
	CPoint m_Second;
};

// Sort on door1 then door2
inline bool operator<( CDoor const &door1, CDoor const &door2 )
{
	if( door1.GetRoom1() == door2.GetRoom1() )
		return door1.GetRoom2() < door2.GetRoom2();

	return door1.GetRoom1() < door2.GetRoom1();
}

// Only compare rooms
inline bool operator==( CDoor const &door1, CDoor const &door2 )
{
	return door1.GetRoom1() == door2.GetRoom1() && door1.GetRoom2() == door2.GetRoom2();
}

typedef handle<CDoor> HDoor;
typedef std::set< HDoor > CDoorSet;
#endif

