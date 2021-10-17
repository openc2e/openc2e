int Round(float f);
float Magnitude( CPoint const &point );
CPoint operator/( const CPoint &p, float d );
CPoint operator*( const CPoint &p, float m );
int Dot( CPoint const &a, CPoint const &b );
int PointToLineSegment( CPoint const &start, CPoint const &end, CPoint const &point );
bool LineEquation( CPoint const &start, CPoint const &end, double *m, double *c );

CString GetFileTitle( CString const &fullPath );
CString GetFileDirectory( CString const &fullPath );
CString GetExeDirectory();
CString GetImageDirectory();
CString GetPreviewDirectory();
CString GetBootstrapDirectory();

const char *GetLastErrorText();
