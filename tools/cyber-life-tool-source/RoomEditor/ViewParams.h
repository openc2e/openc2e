#include "ColourRange.h"

class CViewParams
{
public:
	CViewParams( CRect const &World, CRect const &Window,
		int Param = -1,
		bool DrawDoors = false,
		bool DrawRooms = true,
		bool DrawBackground = true);

	CRect GetWorldRect() const { return m_WorldRect; }
	CRect GetWindowRect() const { return m_WindowRect; }

	int WorldToWindow( int Value ) const;
	CPoint WorldToWindow( CPoint const &Point ) const;
	CRect WorldToWindow( CRect const &Rect ) const;

	int WindowToWorld( int Value ) const;
	CPoint WindowToWorld( CPoint const &Point ) const;
	CRect WindowToWorld( CRect const &Rect ) const;

	COLORREF ParamColour( int Value ) const;
	COLORREF ParamColour( float Value ) const;
	int ParamIndex() const { return m_Param; }
	void SetParamRange( CString const &range ) {m_Range.SetRange( range );}

	bool DrawDoors() const { return m_DrawDoors; }
	bool DrawRooms() const { return m_DrawRooms; }
	bool DrawBackground() const { return m_DrawBackground; }
	
	static CPoint TransformPoint( CPoint const &Point, CRect const &From, CRect const &To );

	bool PtNear( CPoint const &point1, CPoint const &point2 );
private:
	CRect m_WindowRect;
	CRect m_WorldRect;
	int m_Param;
	CColourRange m_Range;
	bool m_DrawDoors;
	bool m_DrawRooms;
	bool m_DrawBackground;
};

