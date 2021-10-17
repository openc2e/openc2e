#ifndef PROPERTYTYPE_H_
#define PROPERTYTYPE_H_

struct CPropertyType
{
public:
	CPropertyType( CString name = "", int minVal = 0, int maxVal = 128,
		bool enumerated = false, CString enumNames = "" )
		: m_Name( name ), m_Min( minVal ), m_Max( maxVal ),
		m_Enumerated( enumerated ), m_EnumNames( enumNames )
	{}

	CString GetEnumName( int enumVal ) const;

	CString m_Name;
	int m_Min;
	int m_Max;
	bool m_Enumerated;
	CString m_EnumNames;
};

#endif

