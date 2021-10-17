// ImageConverter
//
// Converts C3 .s16, .c16 and .blk files between 555<=>565 formats.
//
//

#ifdef _MSC_VER
// turn off symbol-length warnings
#pragma warning(disable:4786 4503)
#endif

#include <sys/stat.h>
#ifndef _WIN32
	#include <unistd.h>
	#include <dirent.h>
	#include <fnmatch.h>
#endif
#include <sys/types.h>

#include <cstdio>
#include <vector>
#include <string>

#include <gtk/gtk.h>
#ifndef _WIN32
	#include <X11/Xlib.h>
#endif

#include "Convert.h"
#include "ProgressDlg.h"

int theDepth=0;	// destination depth (0=unset)
std::vector< std::string > theFileList;

bool SussArgs( int argc, char* argv[] );

int main( int argc, char* argv[] )
{
	gtk_init( &argc, &argv );

	if( !SussArgs(argc,argv) 
#ifdef _WIN32
			|| !theDepth
#endif
		)
	{
		printf("Usage:\n");
		printf("ImageConverter [-15] [-16] files...\n" );
		printf("Converts Creatures 3 .s16, .c16 and .blk files between\n" );
		printf("15 bit (555) and 16 bit (565) formats.\n" );
#ifndef _WIN32
		printf("If neither -15 or -16 is specified, the files are\n");
		printf("converted to the format best matching the root window");
		printf("format.\n");
		printf("If the root window is anything other than 15 or 16 bit,\n");
		printf("16 bit will be selected.\n");
#else
		printf("You must specify the destination format.\n");
#endif
		return -1;
	}


#ifndef _WIN32
	if( theDepth == 0 )
	{
		Display* dpy;
		int scr;
		dpy = XOpenDisplay( NULL );
		if( !dpy )
		{
			fprintf( stderr, "XOpenDisplay() failed\n");
			return -1;
		}
		scr = DefaultScreen( dpy );
		theDepth = DefaultDepth( dpy, scr );

		// if anything other than 15 bit, we'll use 16 bit.
		if( theDepth != 15 )
			theDepth = 16;
//		printf( "Depth: %d\n", theDepth );
	}
#endif

	char buf[512];
	sprintf( buf,
		"Please wait...\nConverting graphics to appropriate format\n"
		"(%d bit, %s RGB)", theDepth, (theDepth==15) ? "555" : "565" );

	ProgressDlg dlg( "C3 ImageConverter", buf );

	int i;
	bool allok = true;
	for( i=0; allok && i<theFileList.size(); ++i )
	{
		float f;
		f = ((float)i) / ((float)theFileList.size());

		char buf[512];

		dlg.SetProgress( f );
		dlg.EventPump();

//		printf("%s...", theFileList[i].c_str() );
		int ret = ConvertFile( theFileList[i].c_str(), theDepth );
		switch( ret )
		{
			case CONV_ALLOK:
//				printf("ok.\n");
				break;
			case CONV_ERR_READ:
				fprintf(stderr,"ERROR: couldn't read '%s'\n",
					theFileList[i].c_str());
				allok = false;
				break;
			case CONV_ERR_WRITE:
				fprintf( stderr,"ERROR: couldn't write '%s'\n",
					theFileList[i].c_str());
				fprintf( stderr,"(Maybe you need to log in as root?)\n");
				allok = false;
				break;
			case CONV_ERR_MALFORMED_FILE:
				fprintf( stderr, "ERROR: Malformed/corrupt file! ('%s')\n",
					theFileList[i].c_str() );
				allok = false;
				break;
			case CONV_ERR_UNKNOWN_FILETYPE:
				// we'll let this one through with a warning...
				fprintf( stderr,"WARNING: ignoring '%s' ",
					theFileList[i].c_str() );
				fprintf( stderr,"(not a .blk, .c16 or .s16 file)\n" );
				break;
			default:
				fprintf( stderr, "ERROR: Unknown error on '%s'\n",
					theFileList[i].c_str() );
				allok = false;
				break;
		}

	}

	return allok ? 0 : 1;
}


bool SussArgs( int argc, char* argv[] )
{
	int i;
	for( i=1; i<argc; ++i )
	{
		if( !strcmp(argv[i], "-15" ) || !strcmp(argv[i],"-555" ) )
		{
			if( theDepth )			// already set?
				return false;
			theDepth = 15;
		}
		else if( !strcmp(argv[i], "-16" ) || !strcmp(argv[i],"-565" ) )
		{
			if( theDepth )			// already set?
				return false;
			theDepth = 16;
		}
		else
		{
			if( argv[i][0] == '-' )
			{
				fprintf( stderr, "ERROR: Unrecognised option\n" );
				return false;
			}
			// assume everything else is a filename
			theFileList.push_back( argv[i] );
		}
	}

	if( theFileList.empty() )
		return false;
	else
		return true;
};



#ifdef LEFT_IN_JUST_IN_CASE_WE_NEED_IT

bool GetFilesInDirectory( const std::string& path,
	std::vector<std::string>& files,
	const std::string& wildcard /* = "*" */)
{
#ifdef _WIN32
	std::string str = path + wildcard;
	WIN32_FIND_DATA finddata;
	HANDLE h = FindFirstFile( str.c_str(), &finddata );
	if( h == INVALID_HANDLE_VALUE )
		return false;
	do
	{
		if( !(finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			files.push_back( finddata.cFileName  );
	}
	while( FindNextFile( h, &finddata ) );
	DWORD err = GetLastError();
	FindClose( h );

	if( err != ERROR_NO_MORE_FILES )
		return false;
	else
		return true;
#else
	// posix version
	DIR* dir;
	struct dirent* dir_entry;
	dir = opendir( path.c_str() );
	if( !dir )
	{
		closedir(dir);
		return false;
	}
	while( dir_entry = readdir(dir) ) 
	{
		struct stat s;
		int fnresult = fnmatch( wildcard.c_str(), dir_entry->d_name, 0 );
		if( fnresult == FNM_NOMATCH )
			continue;
		if( fnresult != 0 )	// error?
		{
			closedir(dir);
			return false;
		}

		std::string fullfilename( path + dir_entry->d_name );
		if( stat( fullfilename.c_str(), &s ) != 0 )
		{
			closedir(dir);
			return false;
		}

		if( !S_ISDIR( s.st_mode ) )
			files.push_back( dir_entry->d_name );
	}
	closedir(dir);
	return true;
#endif
}

#endif


