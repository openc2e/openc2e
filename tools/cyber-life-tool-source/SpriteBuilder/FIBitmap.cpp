/*********************************************************************
* File:     FIBitmap.cpp
* Created:  30/10/97
* Author:   Robin E. Charlton
* 
*********************************************************************/

#include "stdafx.h"
#include "FIBitmap.h"


/*********************************************************************
* Public: CFIBitmap.
*********************************************************************/
CFIBitmap::CFIBitmap()
{
	Init();
	Create();
}


/*********************************************************************
* Public: CFIBitmap.
*********************************************************************/
CFIBitmap::CFIBitmap(CFIBitmap& rBitmap, BOOL bCreateBitmap)
{
	// BITMAPINFO.
	memcpy(&m_BitmapInfo, &rBitmap.m_BitmapInfo, sizeof(BITMAPINFO));

	// Pixel data.
	int iBytesNeeded = rBitmap.m_BitmapInfo.bmiHeader.biSizeImage;
	ASSERT(iBytesNeeded);
	
	m_pPixelData = new BYTE[iBytesNeeded];
	memcpy(m_pPixelData, rBitmap.m_pPixelData, iBytesNeeded);

	if (bCreateBitmap)
	{
		// Create DC.
		HDC hDC = ::GetDC(NULL);
		ASSERT(hDC);

		// HBITMAP -> create new bitmap GDI object.
		m_hBitmap = CreateDIBitmap(
			hDC,
			&m_BitmapInfo.bmiHeader,
			CBM_INIT,
			m_pPixelData,
			(BITMAPINFO*)&m_BitmapInfo,
			DIB_RGB_COLORS);
		ASSERT(m_hBitmap);

		// Clean up.
		::ReleaseDC(NULL, hDC);
		hDC = NULL;
	}
	else
	{
		m_hBitmap = NULL;
	}

}


/*********************************************************************
* Public: CFIBitmap. Converts 8bit to 24.
*********************************************************************/
CFIBitmap::CFIBitmap(
	int Width, 
	int Height, 
	LOGPALETTE* pLogicalPalette, 
	PIXEL_FORMAT Format,
	BYTE* pPixelData)
{
	ASSERT(Format == FORMAT8 || Format == FORMAT16_555 || Format == FORMAT16_565);
	ASSERT((Format == FORMAT8 && pLogicalPalette) || 
		((Format == FORMAT16_555 || Format == FORMAT16_565) && !pLogicalPalette));

	Init();

	BOOL bValid;
	bValid = (Width > 0 && -Height > 0);
	bValid &= (pPixelData != NULL);

	if (bValid && Format == FORMAT8)
	{
		// Data is 8 bit.
		CreateFrom8Bit(Width, Height, pLogicalPalette, pPixelData);
	}
	else if (bValid && Format == FORMAT16_555)
	{
		// Data is 16 bit.
		CreateFrom16Bit(Width, Height, (WORD*)pPixelData, FALSE);
	}
	else if (bValid && Format == FORMAT16_565)
	{
		// Data is 16 bit.
		CreateFrom16Bit(Width, Height, (WORD*)pPixelData, TRUE);
	}
	else
	{
		// Data is invalid, create a default bitmap.
		Create();
	}
}


/*********************************************************************
* Public: CFIBitmap. Loads BMP from file.
*********************************************************************/
CFIBitmap::CFIBitmap(const char* pFileName)
{
	ASSERT(pFileName);

	Init();

	if (!LoadBMP(pFileName))
	{
		Create();
	}
}


/*********************************************************************
* Public: CFIBitmap. Creates BMP normal data.
*********************************************************************/
CFIBitmap::CFIBitmap(BITMAPINFO* pBitmapInfo, BYTE* pPixelData)
{
	ASSERT(pBitmapInfo);
	ASSERT(pPixelData);

	Init();

	if (!Create(pBitmapInfo, pPixelData))
	{
		AfxMessageBox("Bitmap is in incorrect format.\nEnsure your bitmap is 24 bit and try again.");
		Create();
	}
}


/*********************************************************************
* Public: Init.
*********************************************************************/
void CFIBitmap::Init()
{
	ZeroMemory(&m_BitmapInfo, sizeof(BITMAPINFO));
	m_hBitmap = NULL;
	m_pPixelData = NULL;
}


/*********************************************************************
* Public: ~CFIBitmap.
*********************************************************************/
CFIBitmap::~CFIBitmap()
{
    DeleteObject(m_hBitmap);
	delete[] m_pPixelData;
	m_pPixelData = NULL;
}


/*********************************************************************
* Public: Detach.
*********************************************************************/
HBITMAP CFIBitmap::Detach()
{
	HBITMAP hTmp = m_hBitmap;
	m_hBitmap = NULL;
	return hTmp;
}


/*********************************************************************
* Public: CreateDefault.
*********************************************************************/
void CFIBitmap::Create()
{
	TRACE("CFIBitmap::CreateDefault\n");

    // Create DC.
    HDC hDC = ::GetDC(NULL);
    ASSERT(hDC);
                                                                                                                                                                                                           
    // Create 24bit bitmap.
    m_BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_BitmapInfo.bmiHeader.biWidth = DEFAULT_WIDTH;
    m_BitmapInfo.bmiHeader.biHeight = DEFAULT_HEIGHT;
    m_BitmapInfo.bmiHeader.biPlanes = 1;
    m_BitmapInfo.bmiHeader.biBitCount = 24;
    m_BitmapInfo.bmiHeader.biCompression = BI_RGB;
    m_BitmapInfo.bmiHeader.biSizeImage = DEFAULT_WIDTH * DEFAULT_HEIGHT * (m_BitmapInfo.bmiHeader.biBitCount / 8);
    m_BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    m_BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    m_BitmapInfo.bmiHeader.biClrUsed = 0;
    m_BitmapInfo.bmiHeader.biClrImportant = 0;

	int iPixelCnt = DEFAULT_WIDTH * DEFAULT_HEIGHT;
	int iBytesNeeded = iPixelCnt * 3;

	m_pPixelData = (BYTE*)new BYTE[iBytesNeeded];

	// Clear memory.
	ZeroMemory(m_pPixelData, iBytesNeeded);

    m_hBitmap = CreateDIBitmap(
        hDC,
        &m_BitmapInfo.bmiHeader,
        CBM_INIT,
        m_pPixelData,
        (BITMAPINFO*)&m_BitmapInfo,
        DIB_RGB_COLORS);
    ASSERT(m_hBitmap);

    // Clean up.
    ::ReleaseDC(NULL, hDC);
    hDC = NULL;
}


/*********************************************************************
* Public: CreateGDI.
*********************************************************************/
BOOL CFIBitmap::CreateGDI()
{
	// Create DC.
	HDC hDC = ::GetDC(NULL);
	ASSERT(hDC);

	// HBITMAP -> create new bitmap GDI object.
	if (m_hBitmap)
		DeleteObject(m_hBitmap);
	m_hBitmap = CreateDIBitmap(
		hDC,
		&m_BitmapInfo.bmiHeader,
		CBM_INIT,
		m_pPixelData,
		(BITMAPINFO*)&m_BitmapInfo,
		DIB_RGB_COLORS);
	
	// Clean up.
	::ReleaseDC(NULL, hDC);
	hDC = NULL;

	return (m_hBitmap != NULL);
}


/*********************************************************************
* Public: Create.
*********************************************************************/
BOOL CFIBitmap::Create(BITMAPINFO* pBitmapInfo, BYTE* pPixelData)
{
	// Quick check - need to do this properly later.
	if (pBitmapInfo->bmiHeader.biBitCount != 24 ||
		pBitmapInfo->bmiHeader.biCompression != BI_RGB)
	{
		return FALSE;
	}

    // Create DC.
    HDC hDC = ::GetDC(NULL);
    ASSERT(hDC);
                                                                                                                                                                                                         
	int iBytesNeeded = pBitmapInfo->bmiHeader.biSizeImage;

    // Create 24bit bitmap.
    m_BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_BitmapInfo.bmiHeader.biWidth = pBitmapInfo->bmiHeader.biWidth;
    m_BitmapInfo.bmiHeader.biHeight = pBitmapInfo->bmiHeader.biHeight;
    m_BitmapInfo.bmiHeader.biPlanes = pBitmapInfo->bmiHeader.biPlanes;
    m_BitmapInfo.bmiHeader.biBitCount = pBitmapInfo->bmiHeader.biBitCount;
    m_BitmapInfo.bmiHeader.biCompression = BI_RGB;
    m_BitmapInfo.bmiHeader.biSizeImage = iBytesNeeded;
    m_BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    m_BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    m_BitmapInfo.bmiHeader.biClrUsed = 0;
    m_BitmapInfo.bmiHeader.biClrImportant = 0;

	m_pPixelData = (BYTE*)new BYTE[iBytesNeeded];
	memcpy(m_pPixelData, pPixelData, iBytesNeeded);

	// Ensure bitmap is top-down (as in Creatures).
	if (m_BitmapInfo.bmiHeader.biHeight > 0)
	{
		FlipVerticalFormat();
	}

	m_hBitmap = CreateDIBitmap(
        hDC,
        &m_BitmapInfo.bmiHeader,
        CBM_INIT,
        m_pPixelData,
        (BITMAPINFO*)&m_BitmapInfo,
        DIB_RGB_COLORS);
    ASSERT(m_hBitmap);

    // Clean up.
    ::ReleaseDC(NULL, hDC);
    hDC = NULL;

	return TRUE;
}


/*********************************************************************
* Protected: CreateFrom8Bit.
*********************************************************************/
void CFIBitmap::CreateFrom8Bit(
	int Width, 
	int Height, 
	LOGPALETTE* pLogicalPalette, 
	BYTE* pPixelData)
{
    // Create DC.
    HDC hDC = ::GetDC(NULL);
    ASSERT(hDC);
                                                                                                                                                                                                           
	// Calculate bytes per line; padded to DWORD size.
	int iBytesPerLine = Width * 3;
	int iRem = iBytesPerLine % sizeof(DWORD);
	if (iRem != 0)
	{
		iBytesPerLine += sizeof(DWORD) - iRem;
	}

	int iBytesNeeded = iBytesPerLine * abs(Height);

    // Create 24bit bitmap.
    m_BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_BitmapInfo.bmiHeader.biWidth = Width;
    m_BitmapInfo.bmiHeader.biHeight = Height;
    m_BitmapInfo.bmiHeader.biPlanes = 1;
    m_BitmapInfo.bmiHeader.biBitCount = 24;
    m_BitmapInfo.bmiHeader.biCompression = BI_RGB;
    m_BitmapInfo.bmiHeader.biSizeImage = iBytesNeeded;
    m_BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    m_BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    m_BitmapInfo.bmiHeader.biClrUsed = 0;
    m_BitmapInfo.bmiHeader.biClrImportant = 0;

	//int iPixelCnt = Width * abs(Height);

	m_pPixelData = (BYTE*)new WORD[iBytesNeeded];//iBytesNeeded / 2];
	BYTE* pB = m_pPixelData;
	int iIndex = 0;

	// Convert each row of pixels...
	for (int r = 0; r < abs(Height); r++)
	{
		// Copy row pixels...
		for (int i = 0; i < Width; i++)
		{
			*pB++ = pLogicalPalette->palPalEntry[pPixelData[iIndex]].peBlue;
			*pB++ = pLogicalPalette->palPalEntry[pPixelData[iIndex]].peGreen;
			*pB++ = pLogicalPalette->palPalEntry[pPixelData[iIndex]].peRed;
			iIndex++;
		}

		// Pad to DWORD with zeros...
		for (int c = Width * 3; c < iBytesPerLine; c++)
		{
			*pB++ = 0x00;
		}
	}


    m_hBitmap = CreateDIBitmap(
        hDC,
        &m_BitmapInfo.bmiHeader,
        CBM_INIT,
        m_pPixelData,
        (BITMAPINFO*)&m_BitmapInfo,
        DIB_RGB_COLORS);
    ASSERT(m_hBitmap);

    // Clean up.
    ::ReleaseDC(NULL, hDC);
    hDC = NULL;
}


/*********************************************************************
* Protected: CreateFrom16Bit.
*********************************************************************/
void CFIBitmap::CreateFrom16Bit(
	int Width, 
	int Height, 
	WORD* pPixelData,
	BOOL b565)
{
    // Create DC.
    HDC hDC = ::GetDC(NULL);
    ASSERT(hDC);
                                                                                                                                                                                                           
	// Calculate bytes per line; padded to DWORD size.
	int iBytesPerLine = Width * 3;
	int iRem = iBytesPerLine % sizeof(DWORD);
	if (iRem != 0)
	{
		iBytesPerLine += sizeof(DWORD) - iRem;
	}

	int iBytesNeeded = iBytesPerLine * abs(Height);

    // Create 24bit bitmap.
    m_BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_BitmapInfo.bmiHeader.biWidth = Width;
    m_BitmapInfo.bmiHeader.biHeight = Height;
    m_BitmapInfo.bmiHeader.biPlanes = 1;
    m_BitmapInfo.bmiHeader.biBitCount = 24;
    m_BitmapInfo.bmiHeader.biCompression = BI_RGB;
    m_BitmapInfo.bmiHeader.biSizeImage = iBytesNeeded;
    m_BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    m_BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    m_BitmapInfo.bmiHeader.biClrUsed = 0;
    m_BitmapInfo.bmiHeader.biClrImportant = 0;

	m_pPixelData = (BYTE*)new BYTE[iBytesNeeded];
	BYTE* pB = m_pPixelData;
	int iIndex = 0;

	// Convert each row of pixels...
	for (int r = 0; r < abs(Height); r++)
	{
		// Copy row pixels...
		for (int i = 0; i < Width; i++)
		{
			BYTE r, g, b;
			WORD wPixel16 = pPixelData[iIndex];

			if (b565)
			{
				P565_TO_RGB(wPixel16, r, g, b);
			}
			else
			{
				P555_TO_RGB(wPixel16, r, g, b);
			}

			*pB++ = b;
			*pB++ = g;
			*pB++ = r;
			iIndex++;
		}

		// Pad to DWORD with zeros...
		for (int c = Width * 3; c < iBytesPerLine; c++)
		{
			*pB++ = 0x00;
		}
	}

    m_hBitmap = CreateDIBitmap(
        hDC,
        &m_BitmapInfo.bmiHeader,
        CBM_INIT,
        m_pPixelData,
        (BITMAPINFO*)&m_BitmapInfo,
        DIB_RGB_COLORS);
    ASSERT(m_hBitmap);

    // Clean up.
    ::ReleaseDC(NULL, hDC);
    hDC = NULL;
}


/*********************************************************************
* Public: CFIBitmap.
*********************************************************************/
BOOL CFIBitmap::LoadBMP(const char* pFileName)
{
	BOOL bSuccess = TRUE;

	try
	{
	    // Create DC.
	    HDC hDC = ::GetDC(NULL);
		ASSERT(hDC);

		// Open file.
		CFile BMPFile(pFileName, CFile::modeRead | CFile::shareDenyWrite);

		// Read BITMAPFILEHEADER.
		BITMAPFILEHEADER BMPFileHdr;
    	BMPFile.Read(&BMPFileHdr, sizeof(BITMAPFILEHEADER));

		// Read BITMAPINFOHEADER.
		BMPFile.Read(&m_BitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

		// Note: bitmap is 24bit -> no RGBQUAD data in the file.

		// Can only handle 24bit BMPs at this time.
		if (m_BitmapInfo.bmiHeader.biBitCount != 24 ||
			m_BitmapInfo.bmiHeader.biCompression != BI_RGB)
		{
			CString* pErr = new CString;
			*pErr = "BMP file must be 24 bit.";
			throw (pErr);
		}

		// Read pixel data.
		int iByteCnt;

		if (m_BitmapInfo.bmiHeader.biSizeImage > 0)
		{
			iByteCnt = m_BitmapInfo.bmiHeader.biSizeImage;
		}
		else
		{
			iByteCnt = BytesUsed(FORMAT24);
			m_BitmapInfo.bmiHeader.biSizeImage = iByteCnt;
		}
		m_pPixelData = new BYTE[iByteCnt];
		UINT uiBytesRead = BMPFile.Read(m_pPixelData, iByteCnt);

		BMPFile.Close();

		
		// Ensure bitmap is top-down (as in Creatures).
		if (m_BitmapInfo.bmiHeader.biHeight > 0)
		{
			FlipVerticalFormat();
		}

		m_hBitmap = CreateDIBitmap(
			hDC,
			&m_BitmapInfo.bmiHeader,
			CBM_INIT,
			m_pPixelData,
			(BITMAPINFO*)&m_BitmapInfo,
			DIB_RGB_COLORS);
		ASSERT(m_hBitmap);

		// Clean up.
		::ReleaseDC(NULL, hDC);
		hDC = NULL;
	}
	catch(CFileException* e)
	{
		e->ReportError();
		bSuccess = FALSE;
		delete[] m_pPixelData;
	}
	catch(CString* e)
	{
		AfxMessageBox(*e);
		bSuccess = FALSE;
		delete[] m_pPixelData;
	}

	return bSuccess;
}


/*********************************************************************
* Public: SaveBMP.
*********************************************************************/
BOOL CFIBitmap::SaveBMP(const char* pFileName)
{
	BOOL bSuccess = TRUE;
	try
	{
		// Open file.
		CFile BMPFile(pFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);

		// Calculate pixel data size.
		int iByteCnt;
		if (m_BitmapInfo.bmiHeader.biSizeImage > 0)
		{
			iByteCnt = m_BitmapInfo.bmiHeader.biSizeImage;
		}
		else
		{
			iByteCnt = BytesUsed(FORMAT24);
		}

		// Write BITMAPFILEHEADER.
		BITMAPFILEHEADER BMPFileHdr;
		BMPFileHdr.bfType = 0x4d42; // "BM"
		BMPFileHdr.bfSize = sizeof(BITMAPFILEHEADER) +  sizeof(BITMAPINFOHEADER) + iByteCnt;
		BMPFileHdr.bfReserved1 = BMPFileHdr.bfReserved2 = 0;
		BMPFileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    	BMPFile.Write(&BMPFileHdr, sizeof(BITMAPFILEHEADER));

		// Ensure bitmap is bottom-up.
		if (m_BitmapInfo.bmiHeader.biHeight < 0)
		{
			FlipVerticalFormat();
		}

		// Write BITMAPINFOHEADER.
		BMPFile.Write(&m_BitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

		// Note: bitmap is 24bit -> no RGBQUAD data in the file.

		// Write pixel data.
		BMPFile.Write(m_pPixelData, iByteCnt);

		// Flip data back.
		FlipVerticalFormat();

		BMPFile.Close();		
	}
	catch(CFileException* e)
	{
		e->ReportError();
		bSuccess = FALSE;
		delete[] m_pPixelData;
	}


	return bSuccess;
}


/*********************************************************************
* Public: BytesUsed.
*********************************************************************/
int CFIBitmap::BytesUsed(PIXEL_FORMAT Format) const
{
	int iBPP = 0;

	switch(Format)
	{
	case FORMAT8:
		iBPP = 1;
		break;

	case FORMAT16_555:
	case FORMAT16_565:
		iBPP = 2;
		break;

	case FORMAT24:
		iBPP = 3;
		break;
	}

	return m_BitmapInfo.bmiHeader.biWidth * abs(m_BitmapInfo.bmiHeader.biHeight) * iBPP;
}


/*********************************************************************
* Public: FlipVerticalFormat.
*********************************************************************/
void CFIBitmap::FlipVerticalFormat()
{
	ASSERT(m_BitmapInfo.bmiHeader.biSizeImage != 0);

	int iBytesPerLine = m_BitmapInfo.bmiHeader.biSizeImage / 
		abs(m_BitmapInfo.bmiHeader.biHeight); 
	BYTE* pTmp = new BYTE[iBytesPerLine];
	BYTE* pTop = m_pPixelData;
	BYTE* pBottom = m_pPixelData + m_BitmapInfo.bmiHeader.biSizeImage - iBytesPerLine;

	int iMidPoint = abs(m_BitmapInfo.bmiHeader.biHeight) / 2;

	for (int r = 0; r < iMidPoint; r++)
	{
		memcpy(pTmp, pTop, iBytesPerLine);
		memcpy(pTop, pBottom, iBytesPerLine);
		memcpy(pBottom, pTmp, iBytesPerLine);

		pTop += iBytesPerLine;
		pBottom -= iBytesPerLine;
	}

	delete[] pTmp;

	m_BitmapInfo.bmiHeader.biHeight *= -1;

}


/*********************************************************************
* Public: WriteAsS16.
*********************************************************************/
void CFIBitmap::WritePixelDataAsS16(CFile& rFile, BOOL b565)
{
	CWaitCursor wait;

	// Calculate bytes per line; padded to DWORD size.
	int iBytesPerLine = m_BitmapInfo.bmiHeader.biWidth * 3;
	int iRem = iBytesPerLine % sizeof(DWORD);
	if (iRem != 0)
	{
		iBytesPerLine += sizeof(DWORD) - iRem;
	}

	int iBytesNeeded = iBytesPerLine * abs(m_BitmapInfo.bmiHeader.biHeight);

	BYTE* pB = m_pPixelData;
	int iIndex = 0;

	// Convert each row of pixels...
	for (int r = 0; r < abs(m_BitmapInfo.bmiHeader.biHeight); r++)
	{
		// Copy row pixels...
		for (int i = 0; i < m_BitmapInfo.bmiHeader.biWidth; i++)
		{
			WORD wResult = 0x00;
			// Convert from 24 bit to 16 bit.
			// From BGR to RGB.
			if (b565)
			{
				RGB_TO_565(*(pB + 2), *(pB + 1), *pB, wResult);

				// If source was not transparent, make sure result isn't either.
				if ((wResult == 0) && ((*(pB + 2) != 0) || (*(pB + 1) != 0) || (*pB != 0)))
				{
					wResult = 0x0821; // Dark grey.
				}
			}
			else
			{
				RGB_TO_555(*(pB + 2), *(pB + 1), *pB, wResult);

				// If source was not transparent, make sure result isn't either.
				if ((wResult == 0) && ((*(pB + 2) != 0) || (*(pB + 1) != 0) || (*pB != 0)))
				{
					wResult = 0x0421; // Dark grey.
				}
			}

			rFile.Write(&wResult, sizeof(WORD)); 
			pB += 3;
			iIndex++;
		}

		// Skip padding...
		for (int c = m_BitmapInfo.bmiHeader.biWidth * 3; c < iBytesPerLine; c++)
		{
			*pB++;
		}
	}
}


// ----------------------------------------------------------------------
// Method:      EncodeTransparencies 
// Arguments:   rFile - file to write to
//				b656 - flag denoting 565 format or not (555)
//			
// Returns:     None
//
// Description: RLE encodes the pixel data like this:
//				each set of pixels are encoded by a byte.  For a run
//				of black pixels the low bit will be 0.  The rest of
//				the byte will be how many pixels in the run.
//				For a run of non black pixels the low bit will be 1.
//				The rest of the byte will be the number of coloured
//				pixels in the run.
//				The end of the sprite is a zero byte			
//			
// ----------------------------------------------------------------------
int CFIBitmap::EncodeTransparencies(CFile& rFile,
									BOOL b565,
									CDWordArray& scanLines)
{
	CWaitCursor wait;

	// Calculate bytes per line; padded to DWORD size.
	int iBytesPerLine = m_BitmapInfo.bmiHeader.biWidth * 3;
	int iRem = iBytesPerLine % sizeof(DWORD);
	if (iRem != 0)
		{
		iBytesPerLine += sizeof(DWORD) - iRem;
		}

	int iBytesNeeded = iBytesPerLine * abs(m_BitmapInfo.bmiHeader.biHeight);

	BYTE* pB = m_pPixelData;
	int iIndex = 0;
	
	//set the default to end of sprite zero byte
	BYTE count = 0;
	BYTE data = count;
	CWordArray pixelArray;
	pixelArray.SetSize(0,4);

	// we must count the total number of bytes used so that we can
	// update the offsets in the sprite header
	DWORD bytesUsed = 0;

	DWORD totalBytes = 0;

	// assume that we are dealing with transparent pixels first
	bool countingTransparents = true; 

	// Convert each row of pixels...
	for (int r = 0; r < abs(m_BitmapInfo.bmiHeader.biHeight); r++)
		{
	//	ASSERT(r !=156);
			// now record the scanlines
		DWORD x = rFile.GetPosition();
			scanLines[r] = rFile.GetPosition();
			totalBytes+=bytesUsed;
			bytesUsed =0;
		// Copy row pixels...
		for (int i = 0; i < m_BitmapInfo.bmiHeader.biWidth; i++)
			{

	

			WORD wResult = 0x00;
			// Convert from 24 bit to 16 bit.
			// From BGR to RGB.
			if (b565)
				{
				RGB_TO_565(*(pB + 2), *(pB + 1), *pB, wResult);

				// If source was not transparent, make sure result isn't either.
				if ((wResult == 0) && ((*(pB + 2) != 0) || (*(pB + 1) != 0) || (*pB != 0)))
					{
					wResult = 0x0821; // Dark grey.
					}
				}
			else
				{
				RGB_TO_555(*(pB + 2), *(pB + 1), *pB, wResult);

				// If source was not transparent, make sure result isn't either.
				if ((wResult == 0) && ((*(pB + 2) != 0) || (*(pB + 1) != 0) || (*pB != 0)))
					{
					wResult = 0x0421; // Dark grey.
					}
				}
			
			// on pixel down...
			count++;

			if(wResult == 0)
				{
				// for each  new line mark which type of pixel we are
				// dealing with
				if(i==0)
					{
					countingTransparents = true;
					}
				}
			else
				{
				if(i==0)
					{
					countingTransparents = false;
					}

				pixelArray.Add(wResult);
			
				}

			// we can only store up to 127 pixels
			// so store those and continue counting from zero
			if(count > 126)
				{
				if(wResult == 0 && countingTransparents)
					{
					// write the tag byte
					data = count << 1;

					// only one byte per run of black pixels gets written
					bytesUsed++;

					data |= TRANSPARENT_PIXEL;
					count = 0;
					ASSERT(data!=0);
					rFile.Write(&data, sizeof(BYTE)); 
					}
				else if(wResult != 0 && !countingTransparents) 
					{
					// write the tag byte
					data = count << 1;

					// two bytes get written for each colour
					bytesUsed += data; 

					// add one for the tag byte
					bytesUsed++;

					data |= COLOUR_PIXEL;
					ASSERT(data!=0);
					rFile.Write(&data, sizeof(BYTE)); 

					for(int i = 0; i < count; i++)
						{
						ASSERT(pixelArray[i]!=0);
						rFile.Write(&pixelArray[i], sizeof(WORD)); 
						}
					count = 0;
					pixelArray.RemoveAll();

					}
				}
			// if we encounter a colour while encoding transparencies
			// then reset the counter
			if(wResult != 0 && countingTransparents)
				{
				count--;
				// write the tag byte
				data = count << 1;
				data |= TRANSPARENT_PIXEL;
				count = 1;

			
				// this could be the first colour after a run of 127 transparent
				if(data!=0)
				{
				// only one byte per run of black pixels gets written
				bytesUsed++;
				rFile.Write(&data, sizeof(BYTE)); 
				}
					
				// we are done with this run of transparents prepare to count
				// colours
				countingTransparents = false;
				}
			else if (wResult == 0 && !countingTransparents)
				{
				count--;
				// write the tag byte
				data = count << 1;

				// two bytes get written for each colour
				bytesUsed += data; 

				// this could be the first transparent after a run of 127 colours
				if(data!=0)
				{
				// add one for the tag byte
				bytesUsed++;

				data |= COLOUR_PIXEL;
				
				rFile.Write(&data, sizeof(BYTE)); 

				for(unsigned char i = 0; i < count; i++)
					{
					ASSERT(pixelArray[i]!=0);
					rFile.Write(&pixelArray[i], sizeof(WORD)); 
					}
				}
				count = 1;
				pixelArray.RemoveAll();
				// we are done with this run of colours prepare to count
				// transparents
				countingTransparents = true;
				}
			
			pB += 3;
			iIndex++;
			}// end of row

			if(count)
		{
		if(pixelArray.GetSize())
			{
			// write the tag byte
			data = count << 1;
				
			// two bytes get written for each colour
			bytesUsed += data; 

			data |= COLOUR_PIXEL;
			
			// add one for the tag byte
			bytesUsed++;
					ASSERT(data!=0);
			rFile.Write(&data, sizeof(BYTE)); 
			
			for(int i = 0; i < count; i++)
				{
				ASSERT(pixelArray[i]!=0);
				rFile.Write(&pixelArray[i], sizeof(WORD)); 
				}

			pixelArray.RemoveAll();
			}
		else
			{
			// write the tag byte
			data = count << 1;
			data |= TRANSPARENT_PIXEL;

			// only one byte per run of black pixels gets written
			bytesUsed++;
			ASSERT(data!=0);
			rFile.Write(&data, sizeof(BYTE)); 
			}
		count = 0;
		}

			// mark the end of row with a zero
			// finally write a zero byte to show the end of file
			data = 0;
			rFile.Write(&data, sizeof(BYTE)); 
			bytesUsed++;
			// Skip padding...
			for (int c = m_BitmapInfo.bmiHeader.biWidth * 3; c < iBytesPerLine; c++)
				{
				*pB++;
				}
		}//end of height

	
	if(count)
		{
		if(pixelArray.GetSize())
			{
			// write the tag byte
			data = count << 1;
				
			// two bytes get written for each colour
			bytesUsed += data; 

			// add one for the tag byte
			bytesUsed++;


			data |= COLOUR_PIXEL;
				
			ASSERT(data!=0);
			rFile.Write(&data, sizeof(BYTE)); 
			
			for(int i = 0; i < count; i++)
				{
					ASSERT(pixelArray[i]!=0);
				rFile.Write(&pixelArray[i], sizeof(WORD)); 
				}

			pixelArray.RemoveAll();
			}
		else
			{
			// write the tag byte
			data = count << 1;
			data |= TRANSPARENT_PIXEL;

			// only one byte per run of black pixels gets written
			bytesUsed++;
	
			ASSERT(data!=0);
			rFile.Write(&data, sizeof(BYTE)); 
			}
		}
	
	// finally write a zero byte to show the end of file
	data = 0;
	rFile.Write(&data, sizeof(BYTE)); 
	totalBytes++;
	return totalBytes;
}

/*********************************************************************
* Public: ImageSize.
*********************************************************************/
int CFIBitmap::ImageSize() const
{
	if (m_BitmapInfo.bmiHeader.biSizeImage > 0)
	{
		return m_BitmapInfo.bmiHeader.biSizeImage;
	}
	else
	{
		return BytesUsed(FORMAT24);
	}
}

// ENCODING WITH A BYTE IS SLIGHTLY MORE EFFICIENT
// ----------------------------------------------------------------------
// Method:      EncodeTransparencies 
// Arguments:   rFile - file to write to
//				b656 - flag denoting 565 format or not (555)
//			
// Returns:     None
//
// Description: RLE encodes the pixel data like this:
//				each set of pixels are encoded by a byte.  For a run
//				of black pixels the low bit will be 0.  The rest of
//				the byte will be how many pixels in the run.
//				For a run of non black pixels the low bit will be 1.
//				The rest of the byte will be the number of coloured
//				pixels in the run.
//				The end of the sprite is a zero byte			
//			
// ----------------------------------------------------------------------
/*int CFIBitmap::EncodeTransparenciesUsingUINT16(CFile& rFile,
									BOOL b565,
									CDWordArray& scanLines)
{
	// Calculate bytes per line; padded to DWORD size.
	int iBytesPerLine = m_BitmapInfo.bmiHeader.biWidth * 3;
	int iRem = iBytesPerLine % sizeof(DWORD);
	if (iRem != 0)
		{
		iBytesPerLine += sizeof(DWORD) - iRem;
		}

	int iBytesNeeded = iBytesPerLine * abs(m_BitmapInfo.bmiHeader.biHeight);

	BYTE* pB = m_pPixelData;
	int iIndex = 0;
	
	//set the default to end of sprite zero byte
	WORD count = 0;
	WORD data = count;
	CWordArray pixelArray;
	pixelArray.SetSize(0,4);

	// we must count the total number of bytes used so that we can
	// update the offsets in the sprite header
	DWORD bytesUsed = 0;

	DWORD totalBytes = 0;

	// assume that we are dealing with transparent pixels first
	bool countingTransparents = true; 

	// Convert each row of pixels...
	for (int r = 0; r < abs(m_BitmapInfo.bmiHeader.biHeight); r++)
		{
			// now record the scanlines
		DWORD x = rFile.GetPosition();
			scanLines[r] = rFile.GetPosition();
			totalBytes+=bytesUsed;
			bytesUsed =0;
		// Copy row pixels...
		for (int i = 0; i < m_BitmapInfo.bmiHeader.biWidth; i++)
			{

	

			WORD wResult = 0x00;
			// Convert from 24 bit to 16 bit.
			// From BGR to RGB.
			if (b565)
				{
				RGB_TO_565(*(pB + 2), *(pB + 1), *pB, wResult);

				// If source was not transparent, make sure result isn't either.
				if ((wResult == 0) && ((*(pB + 2) != 0) || (*(pB + 1) != 0) || (*pB != 0)))
					{
					wResult = 0x0821; // Dark grey.
					}
				}
			else
				{
				RGB_TO_555(*(pB + 2), *(pB + 1), *pB, wResult);

				// If source was not transparent, make sure result isn't either.
				if ((wResult == 0) && ((*(pB + 2) != 0) || (*(pB + 1) != 0) || (*pB != 0)))
					{
					wResult = 0x0421; // Dark grey.
					}
				}
			
			// on pixel down...
			count++;

			if(wResult == 0)
				{
				// for each  new line mark which type of pixel we are
				// dealing with
				if(i==0)
					{
					countingTransparents = true;
					}
				}
			else
				{
				if(i==0)
					{
					countingTransparents = false;
					}

				pixelArray.Add(wResult);
			
				}

			// we can only store up to 32767 pixels
			// so store those and continue counting from zero
			// well you never know!!
			if(count > 32767)
				{
				if(wResult == 0 && countingTransparents)
					{
					// write the tag byte
					data = count << 1;

				// two bytes get written for the transparencies
			bytesUsed += data; 

					data |= TRANSPARENT_PIXEL;
					count = 0;

					rFile.Write(&data, sizeof(WORD)); 
					}
				else if(wResult != 0 && !countingTransparents) 
					{
					// write the tag byte
					data = count << 1;

					// two bytes get written for each colour
					bytesUsed += data; 

					data |= COLOUR_PIXEL;
					rFile.Write(&data, sizeof(WORD)); 

					for(int i = 0; i < count; i++)
						{
						rFile.Write(&pixelArray[i], sizeof(WORD)); 
						}
					count = 0;
					pixelArray.RemoveAll();

					}
				}
			// if we encounter a colour while encoding transparencies
			// then reset the counter
			if(wResult != 0 && countingTransparents)
				{
				count--;
				// write the tag byte
				data = count << 1;
				data |= TRANSPARENT_PIXEL;
				count = 1;

			// two bytes get written for the transparencies
			bytesUsed += data; 

				rFile.Write(&data, sizeof(WORD)); 
					
				// we are done with this run of transparents prepare to count
				// colours
				countingTransparents = false;
				}
			else if (wResult == 0 && !countingTransparents)
				{
				count--;
				// write the tag byte
				data = count << 1;

				// two bytes get written for each colour
				bytesUsed += data; 

				data |= COLOUR_PIXEL;
				rFile.Write(&data, sizeof(WORD)); 

				for(int i = 0; i < count; i++)
					{
					rFile.Write(&pixelArray[i], sizeof(WORD)); 
					}
				count = 1;
				pixelArray.RemoveAll();
				// we are done with this run of colours prepare to count
				// transparents
				countingTransparents = true;
				}
			
			pB += 3;
			iIndex++;
			}// end of row

			if(count)
		{
		if(pixelArray.GetSize())
			{
			// write the tag byte
			data = count << 1;
				
			// two bytes get written for each colour
			bytesUsed += data; 

			data |= COLOUR_PIXEL;
				
			rFile.Write(&data, sizeof(WORD)); 
			
			for(unsigned char i = 0; i < count; i++)
				{
				rFile.Write(&pixelArray[i], sizeof(WORD)); 
				}

			pixelArray.RemoveAll();
			}
		else
			{
			// write the tag byte
			data = count << 1;
			data |= TRANSPARENT_PIXEL;

			// two bytes get written for the transparencies
			bytesUsed += data; 
	
			rFile.Write(&data, sizeof(WORD)); 
			}
		count = 0;
		}

			// mark the end of row with a zero
			// finally write a zero byte to show the end of file
			data = 0;
			rFile.Write(&data, sizeof(WORD)); 
		// two bytes get written for the transparencies
			bytesUsed += data; 
			// Skip padding...
			for (int c = m_BitmapInfo.bmiHeader.biWidth * 3; c < iBytesPerLine; c++)
				{
				*pB++;
				}
		}//end of height

	
	if(count)
		{
		if(pixelArray.GetSize())
			{
			// write the tag byte
			data = count << 1;
				
			// two bytes get written for each colour
			bytesUsed += data; 

			data |= COLOUR_PIXEL;
				
			rFile.Write(&data, sizeof(WORD)); 
			
			for(unsigned char i = 0; i < count; i++)
				{
				rFile.Write(&pixelArray[i], sizeof(WORD)); 
				}

			pixelArray.RemoveAll();
			}
		else
			{
			// write the tag byte
			data = count << 1;
			data |= TRANSPARENT_PIXEL;

			// two bytes get written for the transparencies
			bytesUsed += data; 
	
			rFile.Write(&data, sizeof(WORD)); 
			}
		}
	
	// finally write a zero byte to show the end of file
	data = 0;
	rFile.Write(&data, sizeof(WORD)); 
	totalBytes++;
	return totalBytes;
}
*/

// ----------------------------------------------------------------------
// Method:      EncodeTransparencies 
// Arguments:   rFile - file to write to
//				b656 - flag denoting 565 format or not (555)
//			
// Returns:     None
//
// Description: RLE encodes the pixel data like this:
//				each set of pixels are encoded by a byte.  For a run
//				of black pixels the low bit will be 0.  The rest of
//				the byte will be how many pixels in the run.
//				For a run of non black pixels the low bit will be 1.
//				The rest of the byte will be the number of coloured
//				pixels in the run.
//				The end of the sprite is a zero byte			
//			
// ----------------------------------------------------------------------
int CFIBitmap::EncodeTransparenciesUsingUINT16(CFile& rFile,
									BOOL b565,
									CDWordArray& scanLines)
{
	CWaitCursor wait;

	// Calculate bytes per line; padded to DWORD size.
	int iBytesPerLine = m_BitmapInfo.bmiHeader.biWidth * 3;
	int iRem = iBytesPerLine % sizeof(DWORD);
	if (iRem != 0)
		{
		iBytesPerLine += sizeof(DWORD) - iRem;
		}

	int iBytesNeeded = iBytesPerLine * abs(m_BitmapInfo.bmiHeader.biHeight);

	BYTE* pB = m_pPixelData;
	int iIndex = 0;
	
	//set the default to end of sprite zero byte
	WORD count = 0;
	WORD data = count;
	CWordArray pixelArray;
	pixelArray.SetSize(0,4);

	// we must count the total number of bytes used so that we can
	// update the offsets in the sprite header
	DWORD bytesUsed = 0;

	DWORD totalBytes = 0;

	// assume that we are dealing with transparent pixels first
	bool countingTransparents = true; 

	// Convert each row of pixels...
	for (int r = 0; r < abs(m_BitmapInfo.bmiHeader.biHeight); r++)
		{
	//	ASSERT(r !=156);
			// now record the scanlines
		DWORD x = rFile.GetPosition();
			scanLines[r] = rFile.GetPosition();
			totalBytes+=bytesUsed;
			bytesUsed =0;
		// Copy row pixels...
		for (int i = 0; i < m_BitmapInfo.bmiHeader.biWidth; i++)
			{

	

			WORD wResult = 0x00;
			// Convert from 24 bit to 16 bit.
			// From BGR to RGB.
			if (b565)
				{
				RGB_TO_565(*(pB + 2), *(pB + 1), *pB, wResult);

				// If source was not transparent, make sure result isn't either.
				if ((wResult == 0) && ((*(pB + 2) != 0) || (*(pB + 1) != 0) || (*pB != 0)))
					{
					wResult = 0x0821; // Dark grey.
					}
				}
			else
				{
				RGB_TO_555(*(pB + 2), *(pB + 1), *pB, wResult);

				// If source was not transparent, make sure result isn't either.
				if ((wResult == 0) && ((*(pB + 2) != 0) || (*(pB + 1) != 0) || (*pB != 0)))
					{
					wResult = 0x0421; // Dark grey.
					}
				}
			
			// on pixel down...
			count++;

			if(wResult == 0)
				{
				// for each  new line mark which type of pixel we are
				// dealing with
				if(i==0)
					{
					countingTransparents = true;
					}
				}
			else
				{
				if(i==0)
					{
					countingTransparents = false;
					}

				pixelArray.Add(wResult);
			
				}

			// we can only store up to 32767 pixels
			// so store those and continue counting from zero
			if(count > 32767)
				{
				if(wResult == 0 && countingTransparents)
					{
					// write the tag byte
					data = count << 1;

					// only one byte per run of black pixels gets written
					bytesUsed+=2;

					data |= TRANSPARENT_PIXEL;
					count = 0;
					ASSERT(data!=0);
					rFile.Write(&data, sizeof(WORD)); 
					}
				else if(wResult != 0 && !countingTransparents) 
					{
					// write the tag byte
					data = count << 1;

					// two bytes get written for each colour
					bytesUsed += data; 

					// add one for the tag byte
					bytesUsed+=2;

					data |= COLOUR_PIXEL;
					ASSERT(data!=0);
					rFile.Write(&data, sizeof(WORD)); 

					for(int i = 0; i < count; i++)
						{
						ASSERT(pixelArray[i]!=0);
						rFile.Write(&pixelArray[i], sizeof(WORD)); 
						}
					count = 0;
					pixelArray.RemoveAll();

					}
				}
			// if we encounter a colour while encoding transparencies
			// then reset the counter
			if(wResult != 0 && countingTransparents)
				{
				count--;
				// write the tag byte
				data = count << 1;
				data |= TRANSPARENT_PIXEL;
				count = 1;

			
				// this could be the first colour after a run of 127 transparent
				if(data!=0)
				{
				// only one byte per run of black pixels gets written
				bytesUsed+=2;
				rFile.Write(&data, sizeof(WORD)); 
				}
					
				// we are done with this run of transparents prepare to count
				// colours
				countingTransparents = false;
				}
			else if (wResult == 0 && !countingTransparents)
				{
				count--;
				// write the tag byte
				data = count << 1;

				// two bytes get written for each colour
				bytesUsed += data; 

				// this could be the first transparent after a run of 127 colours
				if(data!=0)
				{
				// add one for the tag byte
				bytesUsed+=2;

				data |= COLOUR_PIXEL;
				
				rFile.Write(&data, sizeof(WORD)); 

				for(int i = 0; i < count; i++)
					{
					ASSERT(pixelArray[i]!=0);
					rFile.Write(&pixelArray[i], sizeof(WORD)); 
					}
				}
				count = 1;
				pixelArray.RemoveAll();
				// we are done with this run of colours prepare to count
				// transparents
				countingTransparents = true;
				}
			
			pB += 3;
			iIndex++;
			}// end of row

			if(count)
		{
		if(pixelArray.GetSize())
			{
			// write the tag byte
			data = count << 1;
				
			// two bytes get written for each colour
			bytesUsed += data; 

			data |= COLOUR_PIXEL;
			
			// add one for the tag byte
			bytesUsed+=2;
					ASSERT(data!=0);
			rFile.Write(&data, sizeof(WORD)); 
			
			for(int i = 0; i < count; i++)
				{
				ASSERT(pixelArray[i]!=0);
				rFile.Write(&pixelArray[i], sizeof(WORD)); 
				}

			pixelArray.RemoveAll();
			}
		else
			{
			// write the tag byte
			data = count << 1;
			data |= TRANSPARENT_PIXEL;

			// only one byte per run of black pixels gets written
			bytesUsed+=2;
			ASSERT(data!=0);
			rFile.Write(&data, sizeof(WORD)); 
			}
		count = 0;
		}

			// mark the end of row with a zero
			// finally write a zero byte to show the end of file
			data = 0;
			rFile.Write(&data, sizeof(WORD)); 
			bytesUsed+=2;
			// Skip padding...
			for (int c = m_BitmapInfo.bmiHeader.biWidth * 3; c < iBytesPerLine; c++)
				{
				*pB++;
				}
		}//end of height

	
	if(count)
		{
		if(pixelArray.GetSize())
			{
			// write the tag byte
			data = count << 1;
				
			// two bytes get written for each colour
			bytesUsed += data; 

			// add one for the tag byte
			bytesUsed+=2;


			data |= COLOUR_PIXEL;
				
			ASSERT(data!=0);
			rFile.Write(&data, sizeof(WORD)); 
			
			for(int i = 0; i < count; i++)
				{
					ASSERT(pixelArray[i]!=0);
				rFile.Write(&pixelArray[i], sizeof(WORD)); 
				}

			pixelArray.RemoveAll();
			}
		else
			{
			// write the tag byte
			data = count << 1;
			data |= TRANSPARENT_PIXEL;

			// only one byte per run of black pixels gets written
			bytesUsed+=2;
	
			ASSERT(data!=0);
			rFile.Write(&data, sizeof(WORD)); 
			}
		}
	
	// finally write a zero byte to show the end of file
	data = 0;
	rFile.Write(&data, sizeof(WORD)); 
	totalBytes+=2;
	return totalBytes;
}

int CFIBitmap::BytesPerLine()
{
	int Width = m_BitmapInfo.bmiHeader.biWidth;
	int iBytesPerLine = Width * 3;
	int iRem = iBytesPerLine % sizeof(DWORD);
	if (iRem != 0)
	{
		iBytesPerLine += sizeof(DWORD) - iRem;
	}
	return iBytesPerLine;
}

