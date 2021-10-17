/*********************************************************************
* File:     Sprite.h
* Created:  30/10/97
* Author:   Robin E. Charlton
* 
*********************************************************************/

#ifndef _SPRITE
#define _SPRITE

#include "afxtempl.h"
#include "FIBitmap.h"



/*********************************************************************
* Type definitions.
*********************************************************************/
typedef CTypedPtrArray<CPtrArray, CFIBitmap*> CFIBitmapArray;

// LOGPALETTE struct for a 256-colour palette. 
// Windows defines a LOGPALETTE struct to have only one entry in the 
// palPalEntry colour table, so have to define my own equivalent struct 
// for palettes that can contain 256 colours. 
typedef struct tagLOGPALETTE256 
{ 
    WORD         palVersion; 
    WORD         palNumEntries; 
    PALETTEENTRY palPalEntry[256]; 
} LOGPALETTE256; 


typedef struct 
{
	int iOffset;
	short Width;
	short Height;
} SPRITE_HEADER;


struct S16_HEADER
{
	DWORD Flags;
	short ImageCount;
};

const int SizeOf_S16_HEADER = 6;


enum
{
	S16_565_FORMAT = 0x00000001,
	C16_16BIT_FORMAT = 0x00000002,
};


/*********************************************************************
* CSprite.
*********************************************************************/
class CSprite
{
public:
    CSprite();
	CSprite(CSprite& rSprite);
    virtual ~CSprite();

	BOOL LoadS16(const char* pFileName, BOOL bOldS16Format = FALSE);
	BOOL LoadSPR(const char* pFileName);
	BOOL LoadC16(const char* pFileName);
	BOOL LoadC16With16BitTag(const char* pFileName);
	BOOL SaveS16(const char* pFileName, BOOL b565);
	BOOL SaveC16(const char* pFileName, BOOL b565);

	inline CFIBitmap* operator [](int i)
	{
		ASSERT(m_BitmapArray[i]);
		return m_BitmapArray[i];
	}

	inline int NumberOfBitmaps()
	{	return m_BitmapArray.GetSize();}

	inline void InsertAt(int nIndex, CFIBitmap* pBitmap)
	{	m_BitmapArray.InsertAt(nIndex, pBitmap);}

	inline void RemoveAt(int nIndex)
	{	m_BitmapArray.RemoveAt(nIndex);}

	BOOL CreateGDI();

protected:
	BOOL LoadPalette(const char* FileName);

protected:
	CFIBitmapArray m_BitmapArray;
	LOGPALETTE256 m_LogicalPalette;
};

#endif

