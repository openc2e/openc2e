#include "stdafx.h"
#include "PropertyType.h"

CString CPropertyType::GetEnumName( int enumVal ) const
{
	CString itemString;
	itemString.Format( "%d", enumVal );
	CString enums = m_EnumNames;

	int start = 0, len = enums.GetLength();
	while( start < len && enumVal )
		if( enums[start++] == '|' )
			--enumVal;

	int end = start;
	while( end < len && enums[end] != '|' )
		++end;

	if( end > start )
		itemString += " - " + enums.Mid( start, end - start );

	return itemString;
}

