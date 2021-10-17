//----------------------------------------------------------------------
// showmodes - quick hack program to display available SDL screenmodes
//
// BenC   9 June 00  Initial version
//



#include <SDL.h>
#include <stdio.h>


void ShowSizes( SDL_PixelFormat* fmt, Uint32 flags );
void ShowFmt( const SDL_PixelFormat* fmt );

int main( int argc, char* argv[] )
{
	SDL_PixelFormat fmt;
	const SDL_VideoInfo* vidinf;

	SDL_Init( SDL_INIT_VIDEO );

	printf("---------------------------------\n");
	printf("SDL_GetVideoInfo() 'best' mode':\n\n" );

	vidinf = SDL_GetVideoInfo();
	ShowFmt( vidinf->vfmt );
	ShowSizes( vidinf->vfmt, 0 );
	printf("\n\n");



	printf("---------------------------------\n");
	printf("16bit 555 with SDL_FULLSCREEN:\n\n");

	fmt.palette = NULL;
	fmt.BitsPerPixel = 16;
	fmt.BytesPerPixel = 2;
	fmt.Rloss = 3;
	fmt.Gloss = 3;
	fmt.Bloss = 3;
	fmt.Aloss = 8;
	fmt.Rshift = 10;
	fmt.Gshift = 5;
	fmt.Bshift = 0;
	fmt.Ashift = 0;
	fmt.Rmask = 0x7c00;
	fmt.Gmask = 0x03e0;
	fmt.Bmask = 0x001f;
	fmt.Amask = 0x0000;

	ShowSizes( &fmt, SDL_FULLSCREEN );

	printf("---------------------------------\n");
	printf("16bit 565 with SDL_FULLSCREEN:\n\n");

	fmt.palette = NULL;
	fmt.BitsPerPixel = 16;
	fmt.BytesPerPixel = 2;
	fmt.Rloss = 3;
	fmt.Gloss = 2;
	fmt.Bloss = 3;
	fmt.Aloss = 8;
	fmt.Rshift = 11;
	fmt.Gshift = 5;
	fmt.Bshift = 0;
	fmt.Ashift = 0;
	fmt.Rmask = 0xf800;
	fmt.Gmask = 0x07e0;
	fmt.Bmask = 0x001f;
	fmt.Amask = 0x0000;

	ShowSizes( &fmt, SDL_FULLSCREEN );

	return 0;
}

void ShowFmt( const SDL_PixelFormat* fmt )
{
	printf( "%d BitsPerPixel,  %d BytesPerPixel,  ",
		(int)fmt->BitsPerPixel, (int)fmt->BytesPerPixel );
	if( fmt->palette )
		printf("palette\n");
	else
		printf("no palette\n");

	printf( "losses: (%d, %d, %d, %d)\n",
		(int)fmt->Rloss, (int)fmt->Gloss, (int)fmt->Bloss, (int)fmt->Aloss);
	printf( "shifts: (%d, %d, %d, %d)\n",
		(int)fmt->Rshift, (int)fmt->Gshift,
		(int)fmt->Bshift, (int)fmt->Ashift);
	printf( "masks:  (0x%02x, 0x%02x, 0x%02x, 0x%02x)\n",
		(int)fmt->Rmask, (int)fmt->Gmask, (int)fmt->Bmask, (int)fmt->Amask);

}


void ShowSizes( SDL_PixelFormat* fmt, Uint32 flags )
{
	SDL_Rect **p;
	SDL_Rect *r;


	p = SDL_ListModes( fmt, flags );
	if( p && p != 0xffffffff )	// docs don't mention 0xffffffff code!
	{
		while( *p )
		{
			printf( "%d x %d\n", (*p)->w, (*p)->h );
			++p;
		}
	}
	else
		printf("any/all/no sizes\n");
}




