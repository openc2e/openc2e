// SpriteCutter.cpp: implementation of the SpriteCutter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SpriteBuilder.h"
#include "SpriteCutter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SpriteCutter::SpriteCutter() :
	FoundRect(),
	ReferencePixel(0),
	Count(0),
	bHasSprites(false),
	myReplaceColour(false),
	bIsCutting(false),
	AutoCutLastI(0),
	AutoCutLastJ(0)
{
	Result = NULL;
	SourceImage = NULL;
}

SpriteCutter::SpriteCutter(CFIBitmap* image) :
	SourceImage(image),
	FoundRect(),
	ReferencePixel(0),
	Count(0),
	bHasSprites(false),
	myReplaceColour(false),
	bIsCutting(false),
	AutoCutLastI(0),
	AutoCutLastJ(0)
{
	Result = NULL;
}

SpriteCutter::SpriteCutter(CSprite* sprite) :
	Result(sprite),
	FoundRect(),
	ReferencePixel(0),
	Count(0),
	bHasSprites(false),
	myReplaceColour(false),
	bIsCutting(false),
	AutoCutLastI(0),
	AutoCutLastJ(0)
{
	SourceImage = NULL;
	Result = NULL;
}

SpriteCutter::SpriteCutter(CFIBitmap* image, CSprite* sprite) :
	SourceImage(image),
	Result(sprite),
	FoundRect(),
	ReferencePixel(0),
	Count(0),
	bHasSprites(false),
	bIsCutting(false),
	AutoCutLastI(0),
	AutoCutLastJ(0)
{
}

SpriteCutter::~SpriteCutter()
{
	Result = NULL;
	SourceImage = NULL;
}

void SpriteCutter::SetBmp(CFIBitmap* bmp)
{
	//Only accept 24 bit bitmaps
	if (bmp->BitmapInfo()->bmiHeader.biBitCount != 24 ||
		bmp->BitmapInfo()->bmiHeader.biCompression != BI_RGB)
		return;

	if(!SourceImage)
		SourceImage = bmp;
	else
	{
		SourceImage = NULL;
		SourceImage = bmp;
	}
	int zSize = SourceImage->BitmapInfo()->bmiHeader.biSize;
	TRACE("%d", zSize);

	/*
	if(!bIsCutting)
	{
		if(Result)
		{
			delete Result;
			Result = NULL;
			Result = new CSprite();
		}
		Count = 0;
	}
	*/

	ImageWidth = SourceImage->Width() * PIXEL_SIZE;
	ImageHeight = (SourceImage->Height());
	BitmapPadding = (4 - (ImageWidth & 3)) & 3;
	TotalWidth = ImageWidth + BitmapPadding;
	ImageSize = ImageHeight * TotalWidth;	

	ImageData = SourceImage->PixelData();

	ReferencePixel = 0;
	for(int i = 0; i < PIXEL_SIZE; i++)
	{
		ReferencePixel |= ImageData[i];
		if(i < PIXEL_SIZE - 1)
			ReferencePixel <<= 8;
	}
}

CFIBitmap* SpriteCutter::GetBitmap()
{
	return SourceImage;
}

CSprite* SpriteCutter::GetSprite()
{
	return Result;
}


bool SpriteCutter::ComparePixel(int position)
{
	// test for out of range
	if (position < 0 || (position >= ImageWidth * ImageHeight))
		return true;

	// test for background colour
	for(int i = 0; i < PIXEL_SIZE; i++)
	{
		BYTE pixelPortion = (BYTE)(ReferencePixel >> (16 - (8 * i)));		
		if(pixelPortion != ImageData[position + i])
			return false;		
	}
	return true;
}

CRect SpriteCutter::FindSprite(int position)
{
	CRect sprite;

	int imageLeftEdge = position - (position % TotalWidth);
	int imageRightEdge = imageLeftEdge + ImageWidth;
	int imageBottomEdge = (position % TotalWidth) + ((ImageHeight * TotalWidth) - TotalWidth);
	int imageTopEdge = position % TotalWidth;
	
	//Find the top edge
	for(int i = position; i >= imageTopEdge; i-= TotalWidth)
	{
		if(ComparePixel(i))
		{
			//sprite->top = i + TotalWidth;
			sprite.top = (i / TotalWidth) + 1;
			break;
		}
	}

	//Find the left edge
	for(i = position; i >= imageLeftEdge; i-=PIXEL_SIZE)
	{
		if(ComparePixel(i))
		{
			//sprite->left = i + PIXEL_SIZE;
			sprite.left = ((i % TotalWidth) / 3) + 1;
			break;
		}
	}

	//Find the bottom edge
	for(i = position; i <= imageBottomEdge; i+=TotalWidth)
	{
		if(ComparePixel(i))
		{
			//sprite->bottom = i;
			sprite.bottom = i / TotalWidth;
			break;
		}
	}

	//Find the right edge
	for(i = position; i <= imageRightEdge; i+=PIXEL_SIZE)
	{
		if(ComparePixel(i))
		{
			sprite.right = (i % TotalWidth) / 3;
			break;
		}
	}

	if(sprite.top < 0 ||
	   sprite.left < 0 ||
	   sprite.bottom > ImageHeight ||
	   sprite.right > ImageWidth / PIXEL_SIZE)
	{
		sprite.top = 0;
		sprite.left = 0;
		sprite.bottom = 0;
		sprite.right = 0;
	}

	return sprite;
}

CFIBitmap* SpriteCutter::CutSprite(CRect position)
{
	//Translate positional data from 2 dimensional rectangle coodinates to
	//1 dimenensional array positions.
	int width = position.Width() * PIXEL_SIZE;
	int topLeft = ((position.top * TotalWidth) + (position.left * PIXEL_SIZE));
	int bottomLeft = ((position.bottom * TotalWidth) + (position.left * PIXEL_SIZE));
	int height = position.Height();
	int bitmapPadding = (4 - (width & 3)) & 3;
	int totalWidth = width + bitmapPadding;

	//Store for sprite image data
	BYTE* spriteData = new BYTE[totalWidth * height];

	//Itirate through bitmap data, copying the sprite data from the bitmap
	//to the sprite array, then remove the sprite from the original bitmap.
	for(int i = topLeft, j = 0; i < bottomLeft; i+=TotalWidth)		
	{
		for(int k = 0; k < width; k++, j++)
		{
			//Get sprite data
//			if(j < 100)
			spriteData[j] = ImageData[i + k];

			//Replace with background colour
			if (myReplaceColour)
			{
				BYTE pixelPortion = (BYTE)(ReferencePixel >> (16 - (8 * (k % PIXEL_SIZE))));		
				ImageData[i + k] = pixelPortion;
			}
		}
		for(int l = 0; l < bitmapPadding; l++, j++)
			spriteData[j] = '0';
	}

	//Get infoheader data
	BITMAPINFO bmpInfo = *SourceImage->BitmapInfo();
	
	//Set new info header data for the sprite
	bmpInfo.bmiHeader.biHeight = -height;
	bmpInfo.bmiHeader.biWidth = width / PIXEL_SIZE;
	bmpInfo.bmiHeader.biSizeImage = totalWidth * height;

	//Create a new sprite
	CFIBitmap* iFoundImage = new CFIBitmap(&bmpInfo, spriteData);
	ASSERT(iFoundImage);

	//Ignore this lot, previous experiments!
//	CFIBitmap iFoundImage(&bmpInfo, spriteData);

//	CFIBitmap* nonGDIbmp = new CFIBitmap(*iFoundImage, false);
/*	
	CFIBitmap* iFoundImage = new CFIBitmap(
		width / PIXEL_SIZE, 
		-height, // SPR's are top-down bitmaps.
		NULL,
		FORMAT16_565,
		spriteData);
*/
	//Clean up
	delete[] spriteData;
	spriteData = NULL;
	iFoundImage->CreateGDI();
	return iFoundImage;
}

bool SpriteCutter::Finish()
{
	Count = 0;
	bIsCutting = false;
	bHasSprites = false;
	/*
	if(Result)
	{
		delete Result;
		Result = NULL;
		Result = new CSprite();
	}
	*/
	return true;
}

void SpriteCutter::SetSprite(CSprite* sprite)
{
	if(!Result)
		Result = sprite;
	else
	{
		Result = NULL;
		Result = sprite;
	}
}

bool SpriteCutter::CutAtAnyPosition()
{
	if(!SourceImage)
		return false;

	if (AutoCutLastI != 0 || AutoCutLastJ != 0)
	{
		for (int i = AutoCutLastI; i < ImageHeight; ++i)
		{
			for (int j = AutoCutLastJ; j < ImageWidth; ++j)
			{
				CPoint point(j, i);
				if (CutAtPosition(point))
				{
					AutoCutLastI = i;
					AutoCutLastJ = 0;
					return true;
				}
			}
		}
	}

	{
		for (int i = 0; i < ImageHeight; ++i)
		{
			for (int j = 0; j < ImageWidth; ++j)
			{
				CPoint point(j, i);
				if (CutAtPosition(point))
				{
					AutoCutLastI = i;
					AutoCutLastJ = 0;
					return true;
				}
			}
		}
	}

	AutoCutLastI = 0;
	AutoCutLastJ = 0;

	return false;
}

bool SpriteCutter::CutAtPosition(CPoint position)
{
	if(!SourceImage)
		return false;

	int cutAtpoint = position.x * PIXEL_SIZE + position.y * TotalWidth;

	if(!ComparePixel(cutAtpoint))
	{
		FoundRect = FindSprite(cutAtpoint);
		if (FoundRect.Width() < 0 || FoundRect.Height() < 0)
			return false;

		if(FoundRect.Width() != 0 &&
		   FoundRect.Height() != 0)
		{
			TRACE("Sprite %d %d\n", position.x, position.y);
			Result->InsertAt(Count, CutSprite(FoundRect));
			Count++;
			bHasSprites = true;
		}
		else
			return false;
	}
	else
	{
		return false;
	}

	return true;
}

CRect SpriteCutter::GetFoundRect()
{
	if(FoundRect)
		return FoundRect;
	else
		return NULL;
}

//DEL void SpriteCutter::ConvertToS16(bool b565)
//DEL {
//DEL 
//DEL 	// Calculate bytes per line; padded to DWORD size.
//DEL 	int iBytesPerLine = m_BitmapInfo.bmiHeader.biWidth * 3;
//DEL 	int iRem = iBytesPerLine % sizeof(DWORD);
//DEL 	if (iRem != 0)
//DEL 	{
//DEL 		iBytesPerLine += sizeof(DWORD) - iRem;
//DEL 	}
//DEL 
//DEL 	int iBytesNeeded = iBytesPerLine * abs(m_BitmapInfo.bmiHeader.biHeight);
//DEL 
//DEL 	BYTE* pB = m_pPixelData;
//DEL 	int iIndex = 0;
//DEL 
//DEL 	// Convert each row of pixels...
//DEL 	for (int r = 0; r < abs(m_BitmapInfo.bmiHeader.biHeight); r++)
//DEL 	{
//DEL 		// Copy row pixels...
//DEL 		for (int i = 0; i < m_BitmapInfo.bmiHeader.biWidth; i++)
//DEL 		{
//DEL 			WORD wResult = 0x00;
//DEL 			// Convert from 24 bit to 16 bit.
//DEL 			// From BGR to RGB.
//DEL 			if (b565)
//DEL 			{
//DEL 				RGB_TO_565(*(pB + 2), *(pB + 1), *pB, wResult);
//DEL 
//DEL 				// If source was not transparent, make sure result isn't either.
//DEL 				if ((wResult == 0) && ((*(pB + 2) != 0) || (*(pB + 1) != 0) || (*pB != 0)))
//DEL 				{
//DEL 					wResult = 0x0821; // Dark grey.
//DEL 				}
//DEL 			}
//DEL 			else
//DEL 			{
//DEL 				RGB_TO_555(*(pB + 2), *(pB + 1), *pB, wResult);
//DEL 
//DEL 				// If source was not transparent, make sure result isn't either.
//DEL 				if ((wResult == 0) && ((*(pB + 2) != 0) || (*(pB + 1) != 0) || (*pB != 0)))
//DEL 				{
//DEL 					wResult = 0x0421; // Dark grey.
//DEL 				}
//DEL 			}
//DEL 
//DEL 			rFile.Write(&wResult, sizeof(WORD)); 
//DEL 			pB += 3;
//DEL 			iIndex++;
//DEL 		}
//DEL 
//DEL 		// Skip padding...
//DEL 		for (int c = m_BitmapInfo.bmiHeader.biWidth * 3; c < iBytesPerLine; c++)
//DEL 		{
//DEL 			*pB++;
//DEL 		}
//DEL 	}
//DEL }

