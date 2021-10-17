// Convert.cpp
//
// Functions for converting creatures3 imagefiles between 555 and 565
// pixel formats. Supports .c16, .s16 and .blk files.
//
//

#include "Convert.h"


#include <string>
#include <algorithm>

#include <cstdio>
#ifndef _WIN32
	#include <unistd.h>
#endif
#include <sys/stat.h>


typedef unsigned char byte;
typedef unsigned short int uint16;
typedef signed short int int16;
typedef unsigned long int uint32;
typedef signed long int int32;



int ConvertC16( void* data, uint32 size, int depth );
int ConvertS16( void* data, uint32 size, int depth );
int ConvertBLK( void* data, uint32 size, int depth );



#ifdef WIBBLE

// frontend for testing
int main( int argc, char* argv[] )
{
	if( argc < 3 )
	{
		printf("need args.\n");
		return 0;
	}


	try
	{

		if( !strcmp( argv[2], "-15" ) )
			ConvertFile( argv[1], 15 );
		else if( !strcmp( argv[2], "-16" ) )
			ConvertFile( argv[1], 16 );


		printf("Cool and groovy.\n");
	}
	catch( ... )
	{
		printf("bugger.\n");
	}
	


	return 0;
}

#endif


int ConvertFile( std::string const& filename, int depth )
{
	if( filename.size() < 4 )
		return CONV_ERR_MALFORMED_FILE;

	std::string ext = filename.substr( filename.size()-4 );
	std::transform( ext.begin(), ext.end(), ext.begin(), tolower );

	if( ext != ".s16" && ext != ".c16" && ext != ".blk" )
		return CONV_ERR_UNKNOWN_FILETYPE;

	FILE* fp = fopen( filename.c_str(), "rb" );

	uint32 flags;
	if( fread( &flags, 4, 1, fp )!= 1 )
	{
		fclose(fp);
		return CONV_ERR_READ;
	}

	// already in desired format?
	if( ( (flags&1) && (depth==16 )) ||
		( !(flags&1) && (depth==15 )) )
	{
		fclose( fp );
		return CONV_ALLOK;
	}

	// slurp file into memory

	struct stat statbuf;
	if( fstat( fileno(fp), &statbuf ) < 0 )
	{
		fclose( fp );
		return CONV_ERR_READ;
	}

	byte* buf = new byte[ statbuf.st_size ];

	*((uint32*)buf) = flags;
	if( fread( buf+4, statbuf.st_size-4, 1, fp ) != 1 )
	{
		delete buf;
		fclose(fp);
		return CONV_ERR_READ;
	}
	fclose(fp);

	// process it

	int ret=-1;

	if( ext == ".s16" )
		ret = ConvertS16( buf, (uint32)statbuf.st_size, depth );
	else if( ext == ".c16" )
		ret = ConvertC16( buf, (uint32)statbuf.st_size, depth );
	else if( ext == ".blk" )
		ret = ConvertBLK( buf, (uint32)statbuf.st_size, depth );


	if( ret != 0 )
	{
		printf("Failed!\n");
		delete buf;
		return CONV_ERR_MALFORMED_FILE;
	}


	// write it back out.

	// (rename the original, just in case...)
	std::string bakname( filename + ".bak" );
	if( rename( filename.c_str(), bakname.c_str() ) < 0 )
		return CONV_ERR_WRITE;

	fp = fopen( filename.c_str(), "wb" );
	if( !fp )
	{
		rename( bakname.c_str(), filename.c_str() );	// panic
		return CONV_ERR_WRITE;
	}

	if( fwrite( buf, statbuf.st_size, 1, fp ) != 1 )
	{
		fclose( fp );
		rename( bakname.c_str(), filename.c_str() );	// panic
		return CONV_ERR_WRITE;
	}
	fclose(fp);

	// zap the original
	if( unlink( bakname.c_str() ) < 0 )
		return CONV_ERR_WRITE;

	delete [] buf;

	return CONV_ALLOK;
}






// convert a run of pixels from 565 to 555
void To555( uint16* p, int count )
{
	while( count-- )
	{
		uint16 c = *p;
		if( c != 0x20 )	// don't go from non-black to black
			*p = ((c >> 1) & 0xffe0) | c & 0x1f;
		++p;
	}
}

// convert a run of pixels from 555 to 565
void To565( uint16* p, int count )
{
	while( count-- )
	{
		uint16 c = *p;
		*p++ = ((c & 0xffe0) << 1) | c & 0x1f;
	}
}




int ConvertS16( void* data, uint32 size, int depth )
{
	byte* p = (byte*)data;

	if( size < 6 )
		return -1;

//	uint32 flags = *((uint32*)p);
	p += 4;
	int numimages = *((uint16*)p);
	p += 2;

	if( depth == 15 )
		*((uint32*)data) = 0;
	else
		*((uint32*)data) = 1;

	if( size < 6 + numimages*(4+2+2) )
		return -1;

	int i;
	for( i=0; i<numimages; ++i )
	{
		uint32 offset = *((uint32*)p);
		p += 4;
		int w = *((uint16*)p);
		p += 2;
		int h = *((uint16*)p);
		p += 2;

		if( offset + w*h*2 > size )
			return -1;

		if( depth == 15 )
			To555( (uint16*)((byte*)data + offset), w*h );
		else
			To565( (uint16*)((byte*)data + offset), w*h );
	}


	return 0;	// all cool and groovy
}


int ConvertC16( void* data, uint32 size, int depth )
{
	byte* p = (byte*)data;

	if( size < 6 )
		return -1;

	uint32 flags = *((uint32*)p);
	p += 4;
	int numimages = *((uint16*)p);
	p += 2;

	if( !(flags&2) )
		return -1;

	if( depth == 15 )
		*((uint32*)data) = 2;
	else
		*((uint32*)data) = 3;

	int i;
	for( i=0; i<numimages; ++i )
	{
		if( p+(4+2+2) > ((byte*)data)+size )
			return -1;

		uint32 offset = *((uint32*)p);
		p += 4;
		int w = *((uint16*)p);
		p += 2;
		int h = *((uint16*)p);
		p += 2;

		// check room for offset table
		if( p+((h-1)*4) > ((byte*)data)+size )
			return -1;

		// for each line of image...
		int line;
		for( line=0; line<h; ++line )
		{
			if( line>0 )
			{
				offset = *((uint32*)p);
				p += 4;
			}

			byte* q = (byte*)data + offset;
			while( true )
			{
				if( (q+2) > (byte*)data + size )
					return -1;

				uint16 tag = *((uint16*)q);
				q+=2;
				if( !tag )	// line end?
					break;

				if( !(tag & 0x1) )	// transparent run?
					continue;

				int cnt = tag >> 1;
				if( q + cnt*2 > (byte*)data + size )
					return -1;

				if( depth == 15 )
					To555( (uint16*)q, cnt );
				else
					To565( (uint16*)q, cnt );

				q += cnt*2;
			}
		}
	}


	return 0;	// all cool and groovy
}



int ConvertBLK( void* data, uint32 size, int depth )
{
	byte* p = (byte*)data;

	if( size < 6 )
		return -1;

//	uint32 flags = *((uint32*)p);
	p += 4;

	// .blk specific fields
	uint16 blkswide = *((uint16*)p);
	p += 2;
	uint16 blkshigh = *((uint16*)p);
	p += 2;

	int numimages = *((uint16*)p);	// should be blkswide*blkshigh
	p += 2;

	if( depth == 15 )
		*((uint32*)data) = 0;
	else
		*((uint32*)data) = 1;

	// (tiles are 128x128 pixels)
	if( size < 4+2+2+2 + numimages*(4+2+2) + (128*128*2 * numimages) )
		return -1;


	// ignore spriteheaders (suspect offsets might be wrong...)
	p += numimages*(4+2+2);

	// just convert the whole lot in one go.
	if( depth == 15 )
		To555( (uint16*)p, 128*128*numimages );
	else
		To565( (uint16*)p, 128*128*numimages );


	return 0;	// all cool and groovy
}




