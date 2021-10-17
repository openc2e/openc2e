// langpick
//
// Simple selection dialog to display buttons with pictures on them
// specified by a config file.
//
// Returnval text of selected button is output to stdout.
//
//
// example conf file:
//-------------------------------------------------
// title "C3 Language Selector"
// label "Please Select a language"
//
// # button <label> <pic> <returnvalue>
// button "English" "en.xpm" "en"
// button "Deutsch" "de.xpm" "de"
// button "Français" "fr.xpm" "fr"
// button "Español" "es.xpm" "es"
// button "Italiano" "it.xpm" "it"
// button "Nederlands" "nl.xpm" "nl"
//
//-------------------------------------------------
//
//
//
// TODO:
//  * should do a multi-column layout system...
//  * error handling in PickWindow
//
// BenC 20jul2000  Initial version



#include "PickWindow.h"
#include <cstdio>

#include "../../common/SimpleLexer.h"

#include <fstream>
#include <algorithm>

bool ReadConfigFile( std::string const& cfgfile );
void Poo( std::string const& filename, SimpleLexer& lex );

PickWindow dlg;

#ifdef _WIN32
	#include <windows.h>
	#define argc __argc
	#define argv __argv
	int WINAPI WinMain(  HINSTANCE, HINSTANCE, LPSTR, int)
#else
	int main( int argc, char* argv[] )
#endif
{
	gtk_init( &argc, &argv );

	std::string cfgfile( "langpick.conf" );
	if( argc >= 2 )
		cfgfile = argv[1];

	if( !ReadConfigFile( cfgfile ) )
		return -1;

	if( dlg.Go() )
	{
		printf("%s\n", dlg.GetSelected().c_str() );
		return 0;
	}
	else
		return -1;
}



bool ReadConfigFile( std::string const& cfgfile )
{
	std::ifstream ins( cfgfile.c_str() );

	if( !ins )
	{
		fprintf( stderr,"Couldn't open %s\n", cfgfile.c_str() );
		return false;
	}

	SimpleLexer lex(ins);

	int toktype;
	std::string tok;
	while( true )
	{
		toktype = lex.GetToken( tok );
		switch( toktype )
		{
			case SimpleLexer::typeSymbol:
				std::transform( tok.begin(), tok.end(), tok.begin(), tolower );


				if( tok == "title" )
				{
					tok="";
					toktype = lex.GetToken( tok );
					if( toktype != SimpleLexer::typeString &&
						toktype != SimpleLexer::typeSymbol )
					{
						Poo( cfgfile,lex );
						return false;
					}

					dlg.SetTitle( tok );
				}
				else if( tok == "label" )
				{
					tok="";
					toktype = lex.GetToken( tok );
					if( toktype != SimpleLexer::typeString &&
						toktype != SimpleLexer::typeSymbol )
					{
						Poo( cfgfile,lex );
						return false;
					}
					dlg.SetLabel( tok );
				}
				else if (tok == "button" )
				{
					std::string buttontext, xpmfile, retcode;

					toktype = lex.GetToken( buttontext );
					if( toktype != SimpleLexer::typeString &&
						toktype != SimpleLexer::typeSymbol )
						{ Poo( cfgfile,lex ); return false; }

					toktype = lex.GetToken( xpmfile );
					if( toktype != SimpleLexer::typeString &&
						toktype != SimpleLexer::typeSymbol )
						{ Poo( cfgfile,lex ); return false; }

					toktype = lex.GetToken( retcode );
					if( toktype != SimpleLexer::typeString &&
						toktype != SimpleLexer::typeSymbol )
						{ Poo( cfgfile,lex ); return false; }
					dlg.AddItem( buttontext, xpmfile, retcode );
				}
				else
				{
					Poo( cfgfile, lex );
					return false;
				}
				break;
			case SimpleLexer::typeFinished:
				return true;
				break;
			case SimpleLexer::typeError:
			default:
				fprintf( stderr,"Error parsing %s (line %d)\n",
					cfgfile.c_str(), lex.GetLineNum() );
				return false;
		}
	}

	// Shouldn't reach here!
	return false;
}


void Poo( std::string const& filename, SimpleLexer& lex )
{
	fprintf( stderr, "Error parsing %s (line %d)\n",
		filename.c_str(), lex.GetLineNum() );
}




