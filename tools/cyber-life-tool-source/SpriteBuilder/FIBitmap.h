/*********************************************************************
* File:     FIBitmap.h
* Created:  30/10/97
* Author:   Robin E. Charlton
* 
*********************************************************************/

#ifndef _FIBITMAP
#define _FIBITMAP


/*********************************************************************
* Type definitions.
*********************************************************************/
typedef enum
{
	FORMAT8,
	FORMAT16_555,
	FORMAT16_565,
	FORMAT24
} PIXEL_FORMAT;

// masks to set the low bit in rle encoding scheme
const unsigned char TRANSPARENT_PIXEL = 0x00;
const unsigned char COLOUR_PIXEL = 0x01; 

#define	ALIGN_TO(a, b)	((a) % (b)) ? (a) += ((b) - ((a) % (b));

#define RGB_TO_565(r, g, b, result) \
	result = (((r) & 0xf8) << 8) | (((g) & 0xfc) << 3)|(((b) & 0xf8) >> 3); 
	//result = (result == 0) ? 0x0821 : result;

#define RGB_TO_555(r, g, b, result) \
	result = (((r) & 0xf8) << 7) | (((g) & 0xf8) << 2)|(((b) & 0xf8) >> 3); 

#define P565_TO_RGB(pixel, r, g, b) \
	(r) = ((pixel) >> 8); \
	(g) = ((pixel) >> 3); \
	(b) = ((pixel) << 3); 

#define P555_TO_RGB(pixel, r, g, b) \
	(r) = ((pixel) >> 7); \
	(g) = ((pixel) >> 2); \
	(b) = ((pixel) << 3); 

#define DEFAULT_WIDTH	32
#define DEFAULT_HEIGHT	32

/*********************************************************************
* CFIBitmap.
*********************************************************************/
class CFIBitmap
{
public:
    CFIBitmap();
	CFIBitmap(CFIBitmap& rBitmap, BOOL bCreateBitmap = TRUE);
	CFIBitmap(int Width, int Height, LOGPALETTE* pLogicalPalette, PIXEL_FORMAT Format, BYTE* pPixelData);
	CFIBitmap(const char* pFileName);
	CFIBitmap(BITMAPINFO* pBitmapInfo, BYTE* pPixelData);
    virtual ~CFIBitmap();
	HBITMAP Detach();

	BOOL CreateGDI();

	BOOL LoadBMP(const char* pFileName);
	BOOL SaveBMP(const char* pFileName);
	void WritePixelDataAsS16(CFile& rFile, BOOL b565);
	int EncodeTransparencies(CFile& rFile, 
							BOOL b565,
							CDWordArray& scanLines);
	int EncodeTransparenciesUsingUINT16(CFile& rFile,
							BOOL b565,
							CDWordArray& scanLines);

	inline int Width() const
	{	return m_BitmapInfo.bmiHeader.biWidth;}

	inline int Height() const
	{	return abs(m_BitmapInfo.bmiHeader.biHeight);}

	int BytesUsed(PIXEL_FORMAT Format) const;
	int ImageSize() const;

	inline BYTE* PixelData()
	{	return m_pPixelData;}

	inline operator HBITMAP() const
	{	return m_hBitmap;}

	inline BITMAPINFO* BitmapInfo()
	{	return &m_BitmapInfo;}

	void FlipVerticalFormat();
	int BytesPerLine();

protected:
	void Init();
	void Create();
	BOOL Create(BITMAPINFO* pBitmapInfo, BYTE* pPixelData);
	void CreateFrom8Bit(int Width, int Height, LOGPALETTE* pLogicalPalette, BYTE* pPixelData);
	void CreateFrom16Bit(int Width, int Height, WORD* pPixelData, BOOL b565);

protected:
    BITMAPINFO m_BitmapInfo;
    HBITMAP m_hBitmap;
	BYTE* m_pPixelData;
};

#endif

