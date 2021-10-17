/*
** spriteinfo - quick hack util to display info about Creatures spritefiles
*/



#include <stdio.h>


int DoCheck( const char* name );


int main( int argc, char* argv[] )
{
	int i;
	int ret;

	if( argc<2 )
	{
		printf("spriteinfo\n"
			" Prints info about Creatures 3 spritefiles"
			" (.s16 .c16 .blk)\n"
			" usage:  spriteinfo file1 file2 file3 ...\n" );
		return 0;
	}

	for( i=1; i<argc; ++i )
	{
		ret = DoCheck( argv[i] );
		if( ret != 0 )
		{
			return ret;
		}
	}

}


int DoCheck( const char* name )
{
	FILE* fp;
	unsigned long int flags;
	unsigned short int count;
	int i;
	int pad;


	pad=20-strlen(name);

	printf( "%s:", name );
	for( i=0;i<pad;++i)
		printf(" ");

	fp = fopen( name, "rb" );
	if( !fp )
	{
		perror( "fopen failed()" );
//		fprintf( stderr, "fopen failed on '%s':\n",name );
		return -1;
	}

	if( fread( &flags, 1, 4, fp) != 4 )
	{
		fclose(fp);
		perror( "Error reading flags" );
		return -1;
	}
	if( fread( &count, 1, 2, fp) != 2 )
	{
		fclose(fp);
		perror( "Error reading count" );
		return -1;
	}

	if( flags & 1 )
		printf( "565 " ); 
	else
		printf( "555 " );

	printf( "(0x%02lx), %3d images\n", flags, (int)count );

	fclose( fp );
	return 0;
}



