#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>

#include <Windows.h>
#include "REException.h"

#include "resource.h"
#include "ProgDlg.h"
#include "utils.h"

const int TILEWIDTH		=128; // 144;
const int TILEHEIGHT	= 128; //150;
// These are now worked out on the fly since world files can be of any 
// size
//const int MAPWIDTH		= 58;
//const int MAPHEIGHT		= 8*2;

//const int WORLDWIDTH	= MAPWIDTH*TILEWIDTH;
//const int WORLDHEIGHT	= MAPHEIGHT*TILEHEIGHT;

typedef struct _SpriteHeader
{
	int		offset;
	short	width;
	short	height;
} SpriteHeader,* pSpriteHeader;

typedef struct _RGB
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} RGB,* pRGB;

class CBitmapFile
{
public:
	CBitmapFile(HANDLE handle)
	{
		ZeroMemory(this,sizeof CBitmapFile);
		FileHandle=handle;
		FileMapping=CreateFileMapping(FileHandle,NULL,PAGE_READONLY,0,0,NULL);
		if (FileMapping!=NULL)
		{
			MemoryPtr=MapViewOfFile(FileMapping,FILE_MAP_READ,0,0,0);
			if (MemoryPtr)
			{
				FileHeader=(BITMAPFILEHEADER*)MemoryPtr;
				BitmapInfo=(BITMAPINFO*)(FileHeader+1);
				Image=(RGB*)((char*)(BitmapInfo)+BitmapInfo->bmiHeader.biSize);
			}
		}

		//now get the world dimensions from the bitmap itself
		SetDimensions();
	}

	BOOL FormatValid(void)
	{
	if (BitmapInfo->bmiHeader.biBitCount==24 
		&& BitmapInfo->bmiHeader.biCompression==BI_RGB)
		return true;
	
	return false;
		// &&
				/* BitmapInfo->bmiHeader.biWidth==WORLDWIDTH && BitmapInfo->bmiHeader.biHeight==WORLDHEIGHT)?TRUE:FALSE;*/
	}

	//C2e no longer specifies the world dimensions
	void SetDimensions()
	{
		WORLDWIDTH = BitmapInfo->bmiHeader.biWidth;
		WORLDHEIGHT = BitmapInfo->bmiHeader.biHeight;

		//if the backdrop is not a multiple of the tilewidth then we may need
		// an extra tile with blank bits on the end.
		myRealWidth = div(WORLDWIDTH,TILEWIDTH);
		myRealHeight = div(WORLDHEIGHT,TILEHEIGHT);
		
		MAPWIDTH = myRealWidth.quot;
		MAPHEIGHT = myRealHeight.quot;
		if(myRealWidth.rem)
		{
			MAPWIDTH++;
		}

		if(myRealHeight.rem)
		{
			MAPHEIGHT++;
		}

/*			std::cout << "World width " << WORLDWIDTH << ".\n";
			std::cout << "World height " << WORLDHEIGHT << ".\n";
			std::cout << "Map width " << MAPWIDTH << ".\n";
			std::cout << "Map height " << MAPHEIGHT << ".\n";
*/
	}

	int GetMAPWIDTH()
	{
		return MAPWIDTH;
	}

	int GetMAPHEIGHT()
	{
		return MAPHEIGHT;
	}

	int GetWORLDWIDTH()
	{
		return WORLDWIDTH;
	}

	int GetWORLDHEIGHT()
	{
		return WORLDHEIGHT;
	}

	~CBitmapFile()
	{
		if (MemoryPtr) UnmapViewOfFile(MemoryPtr);
		if (FileMapping) CloseHandle(FileMapping);
		CloseHandle(FileHandle);
	}

	void Get565Image(unsigned short* buffer,int x,int y,int w,int h)
	{
		int widthUse = 3 * BitmapInfo->bmiHeader.biWidth; //with in bytes
		widthUse = ((widthUse + 3)>>2)<<2;
		char* base_ptr=(char *)Image+((BitmapInfo->bmiHeader.biHeight-y)*widthUse)+x*3;
	
		for (;h--;)
		{
		
			base_ptr-=widthUse;
			RGB* pixel=(RGB*)base_ptr;
			for (int _w=w;_w--;)
			{
				*buffer++=((pixel->b&0xf8)<<8)|((pixel->g&0xfc)<<3)|((pixel->r&0xf8)>>3);
//				*buffer++=((pixel->b&0xf8)<<7)|((pixel->g&0xf8)<<2)|((pixel->r&0xf8)>>3);
				pixel++;
			}
			buffer += TILEWIDTH - w;
		}

	}

	void Get555Image(unsigned short* buffer,int x,int y,int w,int h)
	{
		int widthUse = 3 * BitmapInfo->bmiHeader.biWidth; //with in bytes
		widthUse = ((widthUse + 3)>>2)<<2;
		char* base_ptr=(char *)Image+((BitmapInfo->bmiHeader.biHeight-y)*widthUse)+x*3;

		for (;h--;)
		{
			base_ptr-=widthUse;
			RGB* pixel=(RGB*)base_ptr;
			for (int _w=w;_w--;)
			{
//				*buffer++=((pixel->b&0xf8)<<8)|((pixel->g&0xfc)<<3)|((pixel->r&0xf8)>>3);
				*buffer++=((pixel->b&0xf8)<<7)|((pixel->g&0xf8)<<2)|((pixel->r&0xf8)>>3);
				pixel++;
			}
			buffer += TILEWIDTH - w;
		}
	}

	RGB const *GetBits()
	{
		return Image;
	}

private:

	HANDLE				FileHandle;
	HANDLE				FileMapping;
	void*				MemoryPtr;
	BITMAPFILEHEADER*	FileHeader;
    BITMAPINFO*			BitmapInfo;
	RGB*				Image;
	//now read in the width and height of the world
	int					WORLDWIDTH;  
	int					WORLDHEIGHT;
	int					MAPWIDTH;
	int					MAPHEIGHT;
	div_t				myRealWidth;
	div_t				myRealHeight;
	
};


CSize BMPToSprite( CString const &input, CString const &output,
				 CString const &preview, bool b565,
				 int minContrast, int maxContrast )
{
	
	CProgressDlg dlg;
	unsigned long	i,j,bytes_written;
	dlg.Create();

	HANDLE input_handle=CreateFile(input,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (input_handle==INVALID_HANDLE_VALUE)
		throw CREException( "Cannot open %s", (LPCTSTR)input );

	dlg.SetStatus( "Loading backgound." );
	CBitmapFile* bitmap=new CBitmapFile(input_handle);
	if (!bitmap->FormatValid())
	{
		delete bitmap;
		throw CREException( "Invalid format in input file - must be true colour bmp" );
	}


	if( output != "" )
	{
		HANDLE output_handle=CreateFile(output,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (output_handle==INVALID_HANDLE_VALUE)
		{
			delete bitmap;
			CloseHandle(input_handle);
			throw CREException( "Cannot open %s", (LPCTSTR)output );
		}

		SpriteHeader	header = {(sizeof SpriteHeader * bitmap->GetMAPWIDTH() * bitmap->GetMAPHEIGHT()) + 6,
			TILEWIDTH, TILEHEIGHT};

		unsigned short	sprite[TILEWIDTH*TILEHEIGHT];
		ZeroMemory(sprite,(TILEWIDTH*TILEHEIGHT*sizeof(unsigned short)));

		// File Header: DWORD flags.
		if (b565)
			i = 0x00000001;
		else
			i = 0x00000000;
		WriteFile(output_handle, &i, 4, &bytes_written, NULL);

		// File Header: image count.
		i = bitmap->GetMAPWIDTH();
		WriteFile(output_handle, &i, 2, &bytes_written, NULL);
		i = bitmap->GetMAPHEIGHT();
		WriteFile(output_handle, &i, 2, &bytes_written, NULL);

		i = bitmap->GetMAPWIDTH() * bitmap->GetMAPHEIGHT();
		WriteFile(output_handle, &i, 2, &bytes_written, NULL);

		for (;i--;)
		{
			WriteFile(output_handle,&header,sizeof SpriteHeader,&bytes_written,NULL);
			header.offset+=TILEWIDTH*TILEHEIGHT*2;
		}
		dlg.SetStatus( "Writing tiles." );
		dlg.SetRange( 0, bitmap->GetMAPWIDTH() * bitmap->GetMAPHEIGHT() );

		try
		{
			for (i=0;i<bitmap->GetMAPWIDTH();i++)
			{
				for (j=0;j<bitmap->GetMAPHEIGHT();j++)
				{
					dlg.SetPos( i * bitmap->GetMAPHEIGHT() + j );
					ZeroMemory(sprite,(TILEWIDTH*TILEHEIGHT*sizeof(unsigned short)));
					int tileWidth = TILEWIDTH;
					int tileHeight = TILEHEIGHT;
					if( (i + 1) * TILEWIDTH >= bitmap->GetWORLDWIDTH() )
						tileWidth = bitmap->GetWORLDWIDTH() - i * TILEWIDTH;
					if( (j + 1) * TILEHEIGHT >= bitmap->GetWORLDHEIGHT() )
						tileHeight = bitmap->GetWORLDHEIGHT() - j * TILEHEIGHT;
					if (b565)
						bitmap->Get565Image(sprite,i*TILEWIDTH,j*TILEHEIGHT,tileWidth,tileHeight);
					else
						bitmap->Get555Image(sprite,i*TILEWIDTH,j*TILEHEIGHT,tileWidth,tileHeight);

					WriteFile(output_handle,sprite,TILEWIDTH*TILEHEIGHT*2,&bytes_written,NULL);
				}
			}
		}
		catch( ... )
		{
			CloseHandle(output_handle);
			delete bitmap;
			throw CREException( "Error reading file %s", (LPCTSTR)output );
		}
		CloseHandle(output_handle);
	}

	if( preview != "" )
	{
		unsigned char contrastMap[256];
		for( int index = 0; index < 256; ++index )
		{
			contrastMap[ index ] = (index * (maxContrast - minContrast)) / 256 + minContrast;
		}


		int bmpWidth = bitmap->GetWORLDWIDTH();
		int bmpHeight = bitmap->GetWORLDHEIGHT();
		int previewWidth = bmpWidth / 4;
		int previewHeight =  bmpHeight / 4;
		int nBytesPerLinePreview = (( previewWidth * 3 + 3 ) / 4 ) * 4;
		int nBytesPerLineSource = (( bmpWidth * 3 + 3 ) / 4 ) * 4;

		dlg.SetStatus( "Creating preview." );
		dlg.SetRange( 0, previewHeight );

		char *bitsPreview = new char[ nBytesPerLinePreview * previewHeight ];
		char *lineStartDEST = bitsPreview;
		char const *lineStartSRC = (char const *)bitmap->GetBits();
		if( bitsPreview )
		{
			int r, g, b, k, l;
			RGB rgb;
			for( i = 0; i < previewHeight; ++i )
			{
				dlg.SetPos( i );

				RGB const *bitsSRC = (RGB const *)lineStartSRC;
				RGB *bitsDEST = (RGB *)lineStartDEST;

				for( j = 0; j < previewWidth; ++j )
				{

					r = g = b = 0;
					for( k = 0; k < 4; ++k )
					{
						for( l = 0; l < 4; ++l )
						{
							r += bitsSRC->r;
							g += bitsSRC->g;
							b += bitsSRC->b;
							++bitsSRC;
						}
						bitsSRC += bmpWidth - 4;
					}
					rgb.r = contrastMap[ r >> 4 ];
					rgb.g = contrastMap[ g >> 4 ];
					rgb.b = contrastMap[ b >> 4 ];
					*bitsDEST++ = rgb;
//					*bitsDEST++ = *bitsSRC;
					bitsSRC += 4 - 4 * bmpWidth;
//					bitsSRC += 4;
				}
				lineStartSRC += nBytesPerLineSource * 4;
				lineStartDEST += nBytesPerLinePreview;
			}
			BITMAPINFOHEADER Info;

			Info.biSize = sizeof( BITMAPINFOHEADER );
			Info.biWidth = previewWidth;
			Info.biHeight = previewHeight;
			Info.biPlanes = 1;
			Info.biBitCount = 24;
			Info.biCompression = BI_RGB; 
			Info.biSizeImage = nBytesPerLinePreview * previewHeight;
			Info.biXPelsPerMeter = 1000;
			Info.biYPelsPerMeter = 1000;
			Info.biClrUsed = 0;
			Info.biClrImportant = 0;

			std::ofstream out( preview, std::ios_base::binary );

			BITMAPFILEHEADER FileHeader;
			FileHeader.bfType = 'B' + ('M' << 8);
			FileHeader.bfSize = nBytesPerLinePreview * previewHeight + sizeof( FileHeader ) + sizeof( Info );
			FileHeader.bfReserved1 = 0;
			FileHeader.bfReserved2 = 0;
			FileHeader.bfOffBits = sizeof( FileHeader ) + sizeof( Info );

			out.write( (const char *)&FileHeader, sizeof( FileHeader ) );
			out.write( (const char *)&Info, sizeof( Info ) );
			out.write( (const char *)bitsPreview, nBytesPerLinePreview * previewHeight );
			delete [] bitsPreview;
		}
	}

	delete bitmap;
	return CSize( bitmap->GetWORLDWIDTH(), bitmap->GetWORLDHEIGHT() );
}

