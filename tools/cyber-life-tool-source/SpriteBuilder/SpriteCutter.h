// SpriteCutter.h: interface for the SpriteCutter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPRITECUTTER_H__DE8CFDA4_D14B_11D2_AC73_00105A56C8C7__INCLUDED_)
#define AFX_SPRITECUTTER_H__DE8CFDA4_D14B_11D2_AC73_00105A56C8C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FIBitmap.h"
#include "Sprite.h"

#define BORDER_WIDTH 5
#define PIXEL_SIZE 3

typedef DWORD Pixel;

class SpriteCutter  
{
public:
	CRect GetFoundRect();
	bool CutAtAnyPosition();
	bool CutAtPosition(CPoint position);
	void SetSprite(CSprite* sprite);
	bool bIsCutting;
	bool Finish();
	bool bHasSprites;
	CSprite* GetSprite();
	CFIBitmap* GetBitmap();
	void SetBmp(CFIBitmap* bmp);
	SpriteCutter(CFIBitmap* image);
	SpriteCutter(CSprite* sprite);
	SpriteCutter(CFIBitmap* image, CSprite* sprite);
	SpriteCutter();
	~SpriteCutter();

	inline void SetMyCount(int c) { Count = c; }
	int Count;
	bool myReplaceColour;
private:
	int BitmapPadding;

protected:
	inline bool ComparePixel(int position);
	CFIBitmap* CutSprite(CRect position);
	CRect FindSprite(int position);
	int ImageHeight;
	int ImageWidth;
	int TotalWidth;
	int ImageSize;
	Pixel ReferencePixel;
	BYTE* ImageData;
	CSprite* Result;
	CFIBitmap* SourceImage;
	CRect FoundRect;

	int AutoCutLastI;
	int AutoCutLastJ;
};

#endif // !defined(AFX_SPRITECUTTER_H__DE8CFDA4_D14B_11D2_AC73_00105A56C8C7__INCLUDED_)

