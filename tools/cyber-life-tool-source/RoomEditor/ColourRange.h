#ifndef COLOURRANGE_H
#define COLOURRANGE_H

#include <vector>


class CColourRange
{
	//Utility class for CColourRange
	class CColourValue
	{
	public:
		CColourValue( int v = 0, int r = 0, int g = 0, int b = 0 ) :
				m_Value(v), m_Red(r), m_Green(g), m_Blue(b) {}

		int m_Value;
		int m_Red;
		int m_Green;
		int m_Blue;
	};
public:
	// ---------------------------------------------------------------------
	// Method:		SetRange
	// Arguments:	NewValues - a string in the following format:
	//				{{<value1>,<red1>,<green1>,<blue1>},
	//					{<value2>,<red2>,<green2>,<blue2>}...} 
	// Returns:		true - no error
	//				false - error parsing string
	// Description:	Sets the valid range of values and associated colours.
	//				the values should increase from left to right
	//				- i.e. first value is the minimum, last value is the
	//				maximum
	// ---------------------------------------------------------------------
	bool SetRange( CString const &NewValues );

	// ---------------------------------------------------------------------
	// Method:		GetRange
	// Arguments:	None
	// Returns:		String in the format:
	//				{{<value1>,<red1>,<green1>,<blue1>},{<value2>,<red2>,<green2>,<blue2>}...} 
	// Description:	May be used to store the state of a CColourRange object
	// ---------------------------------------------------------------------
	CString GetRange() const;

	// ---------------------------------------------------------------------
	// Method:		GetColour
	// Arguments:	Value - integer within the valid range
	// Returns:		
	// Description:	
	// ---------------------------------------------------------------------
	COLORREF GetColour( int Value ) const;
	COLORREF GetColour( float Value ) const;
private:
	std::vector<CColourValue> m_ColourValues;
};

#endif

