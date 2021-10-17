#include "stdafx.h"
#include <math.h>
#include "C2ERoomEditor.h"
#include "../../common/WhichEngine.h"

int Round(float f) {
		int i;
		__asm {
			fld		f
			lea		eax, i
			fistp	dword ptr[eax]
		}
		return i;
	}

float Magnitude( CPoint const &point )
{
	return float( sqrt(  point.x * point.x + point.y * point.y ) );
}

int Dot( CPoint const &a, CPoint const &b )
{
	return b.x * a.x + b.y * a.y;
}

int PointToLineSegment
	( CPoint const &start, CPoint const &end, CPoint const &point )
{
	CPoint a = end - start, b = point - start, p;
	float t, tx, ty;

	t = float( Dot( a, b ) ) / ( float(a.x * a.x + a.y * a.y) );
	if( t < 0 ) t = 0;
	if( t > 1 ) t = 1;
	tx = t * a.x;
	ty = t * a.y;

	p = CPoint( start.x + Round(tx), start.y + Round(ty) );

	return Round( Magnitude( p - point ) );
}

bool LineEquation( CPoint const &start, CPoint const &end, double *m, double *c )
{
	if( start.x == end.x ) return false;

	double x1 = start.x, y1 = start.y, x2 = end.x, y2 = end.y;

	*m = (y2 - y1)/(x2 - x1);
	*c = y1 - *m * x1;

	return true;
}


CString GetFileTitle( CString const &fullPath )
{
	int end = fullPath.GetLength() - 1;
	while( end && fullPath[end] != '.' ) --end;
	int start = end;
	while( start > -1 && fullPath[start] != '\\' ) --start;
	return fullPath.Mid(start + 1, end - start - 1 );
}

CString GetFileDirectory( CString const &fullPath )
{
	int end = fullPath.GetLength() - 1;
	while( end && fullPath[end] != '\\' ) --end;
	return fullPath.Left( end ) + '\\';
}

CString GetExeDirectory()
{
	CString exeDir;
	GetModuleFileName( AfxGetApp()->m_hInstance, exeDir.GetBuffer(1000), 1000 );
	exeDir.ReleaseBuffer();
	return GetFileDirectory( exeDir );
}

CString GetPreviewDirectory()
{
	return ((CC2ERoomEditorApp *)AfxGetApp())->GetPreviewDirectory();
}

CString GetImageDirectory()
{
	std::string temp = theWhichEngine.GetStringKey( "Backgrounds Directory" );
	return CString( temp.c_str() );
}

CString GetBootstrapDirectory()
{
	std::string temp = theWhichEngine.GetStringKey( "Bootstrap Directory" );
	return CString( temp.c_str() );
}

CPoint operator/( const CPoint &p, float d )
{
	return CPoint( int(p.x / d), int(p.y / d) );
}

CPoint operator*( const CPoint &p, float m )
{
	return CPoint( int(p.x * m), int(p.y * m) );
}


char ourErrBuf[1024];
const char* GetLastErrorText()
{
	DWORD err=GetLastError();
	char buffer[4096];
	buffer[0] = 0;
	if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, err, 0, buffer, 4095, NULL) == 0)
		sprintf(ourErrBuf, "(error %d)", err);
	else
		sprintf(ourErrBuf, "\n%s", buffer);
	return ourErrBuf;
}

