/*********************************************************************
* File:     Sprite.cpp
* Created:  31/10/97
* Author:   Robin E. Charlton
* 
*********************************************************************/

#include "stdafx.h"
#include "Sprite.h"

#define COMPANY "CyberLife Technology Ltd"
#define TITLE "Creatures"
#define BUILD "1.0"


/*********************************************************************
* Public: CSprite.
*********************************************************************/
CSprite::CSprite() :
    m_BitmapArray()
{
	/*
	HKEY hKey;
	CString sKey = "SOFTWARE\\";
	sKey += COMPANY;
	sKey += "\\";
	sKey += TITLE;
	sKey += "\\";
	sKey += BUILD;

	BOOL bSuccess = FALSE;

	// Attempt to find creatures keys in registry.
	if (RegOpenKey(HKEY_LOCAL_MACHINE, sKey, &hKey) == ERROR_SUCCESS)
	{
		char pValue[] = "Palette Directory";
		char pPaletteDir[MAX_PATH];
		DWORD dwCnt = MAX_PATH;
		if (RegQueryValueEx(hKey, pValue, NULL, NULL, (LPBYTE)pPaletteDir, &dwCnt) == ERROR_SUCCESS)
		{
			strcat(pPaletteDir, "PALETTE.DTA");
			bSuccess = LoadPalette(pPaletteDir);
		}
	}

	if (!bSuccess)
	{
		// Try CWD.
		bSuccess = LoadPalette("PALETTE.DTA");
	}


	if (!bSuccess)
	{
		AfxMessageBox("Could not find palette.\nInstall Creatures or copy file palette.dta into the same folder as SpriteBuilder.exe");
	}
	*/
}


/*********************************************************************
* Public: CSprite.
*********************************************************************/
CSprite::CSprite(CSprite& rSprite) :
    m_BitmapArray()
{
	// Copy palette.
	memcpy(&m_LogicalPalette, &rSprite.m_LogicalPalette, sizeof(LOGPALETTE256));
	memcpy(&m_LogicalPalette.palPalEntry, &rSprite.m_LogicalPalette.palPalEntry, 256);

	// Copy bitmap array.
	int iCnt = rSprite.m_BitmapArray.GetSize();
	for (int i = 0; i < iCnt; i++)
	{
		// NOTE: does not create GDI bitmap (Resource are too precious).
		CFIBitmap* pBitmap = new CFIBitmap(*rSprite.m_BitmapArray[i], FALSE);
		m_BitmapArray.Add(pBitmap);
	}
}


/*********************************************************************
* Public: ~CSprite.
*********************************************************************/
CSprite::~CSprite()
{
    int iCnt = m_BitmapArray.GetSize();

    for(int i = 0; i < iCnt; i++)
    {
        CFIBitmap* pTmp = m_BitmapArray[0];
		m_BitmapArray.RemoveAt(0);
		delete pTmp;
		pTmp = NULL;
	}
}


/*********************************************************************
* Public: LoadS16.
*********************************************************************/
BOOL CSprite::LoadS16(const char* pFileName, BOOL bOldS16Format)
{
	CWaitCursor waitCursor;
	BOOL bSuccess = TRUE;

    try
    {
        // Open & read file header information.
        CFile File(pFileName, CFile::modeRead | CFile::shareExclusive);

		S16_HEADER FileHeader = {0x00000000, 0x0000};
		if (!bOldS16Format)
		{
	        File.Read(&FileHeader, SizeOf_S16_HEADER);   
		}
		else
		{
	        File.Read(&FileHeader.ImageCount, 2);   
			FileHeader.Flags |= S16_565_FORMAT;
		}

		// Read header information.
		SPRITE_HEADER* pSpriteHdr = new SPRITE_HEADER[FileHeader.ImageCount];
		int i;
		for (i = 0; i < FileHeader.ImageCount; i++)
		{
			File.Read(&pSpriteHdr[i], sizeof(SPRITE_HEADER));
		}

		byte* pData = NULL;
		int iBytesNeeded = 0;
		PIXEL_FORMAT Format;
		if (FileHeader.Flags & S16_565_FORMAT)
			Format = FORMAT16_565;
		else
			Format = FORMAT16_555;

		// Read image data.
		for (i = 0; i < FileHeader.ImageCount; i++)
		{
			File.Seek(pSpriteHdr[i].iOffset, CFile::begin);

			iBytesNeeded = pSpriteHdr[i].Width * pSpriteHdr[i].Height * 2; // *2 => 16bit.

			pData = (byte*)new WORD[iBytesNeeded / 2];
			ASSERT(pData);
		    UINT uiBytesRead = File.Read(pData, iBytesNeeded); 

			CFIBitmap* pBitmap = new CFIBitmap(
				pSpriteHdr[i].Width, 
				-pSpriteHdr[i].Height, // SPR's are top-down bitmaps.
				NULL,
				Format,
				pData);
			m_BitmapArray.Add(pBitmap);
			delete[] pData;
			pData = NULL;
		}

		delete[] pSpriteHdr;
		pSpriteHdr = NULL;

		File.Close();
    }
    catch(CFileException* e)
    {
        e->ReportError();
		bSuccess = FALSE;
    }
	catch(CMemoryException* e)
    {
        e->ReportError();
		bSuccess = FALSE;
    }

    return bSuccess;
}


/*********************************************************************
* Public: LoadSPR.
*********************************************************************/
BOOL CSprite::LoadSPR(const char* pFileName)
{
	CWaitCursor waitCursor;
	BOOL bSuccess = TRUE;

    try
    {
        // Open & read image count.
        CFile File(pFileName, CFile::modeRead | CFile::shareExclusive);
	    WORD ImageCount = 0;
        File.Read(&ImageCount, 2);   

		// Read header information.
		SPRITE_HEADER* pSpriteHdr = new SPRITE_HEADER[ImageCount];
		int i;
		for (i = 0; i < ImageCount; i++)
		{
			File.Read(&pSpriteHdr[i], sizeof(SPRITE_HEADER));
		}

		byte* pData = NULL;
		int iBytesNeeded = 0;

		// Read  image data.
		for (i = 0; i < ImageCount; i++)
		{
			File.Seek(pSpriteHdr[i].iOffset, CFile::begin);

			iBytesNeeded = pSpriteHdr[i].Width * pSpriteHdr[i].Height;
			if (iBytesNeeded % 2)
				iBytesNeeded++;

			pData = (byte*)new WORD[iBytesNeeded / 2];
			ASSERT(pData);
		    UINT uiBytesRead = File.Read(pData, pSpriteHdr[i].Width * pSpriteHdr[i].Height); 

			CFIBitmap* pBitmap = new CFIBitmap(
				pSpriteHdr[i].Width, 
				-pSpriteHdr[i].Height, // SPR's are top-down bitmaps.
				(LOGPALETTE*)&m_LogicalPalette,
				FORMAT8,
				pData);
			m_BitmapArray.Add(pBitmap);
			delete[] pData;
			pData = NULL;
		}

		delete[] pSpriteHdr;
		pSpriteHdr = NULL;

		File.Close();
    }
    catch(CFileException* e)
    {
        e->ReportError();
		bSuccess = FALSE;
    }
	catch(CMemoryException* e)
    {
        e->ReportError();
		bSuccess = FALSE;
    }

    return bSuccess;
}

// ----------------------------------------------------------------------
// Method:      LoadC16 
// Arguments:   pFileName - file to read from
//			
// Returns:     None
//
// Description: Reads rle encoded pixel data like this:
//				each set of pixels are encoded by a byte.  For a run
//				of black pixels the low bit will be 0.  The rest of
//				the byte will be how many pixels in the run.
//				For a run of non black pixels the low bit will be 1.
//				The rest of the byte will be the number of coloured
//				pixels in the run.
//				The end of the sprite is a zero byte			
//			
// ----------------------------------------------------------------------
BOOL CSprite::LoadC16(const char* pFileName)
{
	CWaitCursor waitCursor;

	BOOL bSuccess = TRUE;

    try
    {
        // Open & read file header information.
        CFile File(pFileName, CFile::modeRead | CFile::shareExclusive);

		S16_HEADER FileHeader = {0x00000000, 0x0000};
   
		File.Read(&FileHeader, SizeOf_S16_HEADER);   

		if (FileHeader.Flags & C16_16BIT_FORMAT)
		{
			File.Close();
			return LoadC16With16BitTag(pFileName);
		}

		// Read header information.
		SPRITE_HEADER* pSpriteHdr = new SPRITE_HEADER[FileHeader.ImageCount];
		int i;
		for (i = 0; i < FileHeader.ImageCount; i++)
		{
		//	File.Read(&pSpriteHdr[i], sizeof(SPRITE_HEADER));

			//read in the first offset
			File.Read(&(pSpriteHdr[i].iOffset), sizeof(int));

			// now the width and height
			File.Read(&(pSpriteHdr[i].Width), sizeof(short));
				// now the width and height
			File.Read(&(pSpriteHdr[i].Height), sizeof(short));
			// skip the other offsets
			File.Seek((pSpriteHdr[i].Height -1) * sizeof(int), CFile::current);
		}

		byte* pData = NULL;
		int iBytesNeeded = 0;
		PIXEL_FORMAT Format;
		if (FileHeader.Flags & S16_565_FORMAT)
			Format = FORMAT16_565;
		else
			Format = FORMAT16_555;

		// Read image data.
		for (i = 0; i < FileHeader.ImageCount; i++)
		{
			if(i == 0)
			{
				File.Seek(pSpriteHdr[i].iOffset, CFile::begin);
			}

			iBytesNeeded = pSpriteHdr[i].Width * pSpriteHdr[i].Height * 2; // *2 => 16bit.

			// create the bits needed
			pData = (byte*)new WORD[iBytesNeeded / 2];

			ASSERT(pData);
			byte* pixels = pData;

			//read the first byte from the file
			BYTE filedata = 0;
			File.Read(&filedata,1);


			int height = pSpriteHdr[i].Height;
			int testCount =0;
			while(height--)
			{
				while(filedata !=0)
				{
				
				// check whether the run is transparent or colour
				if(filedata & 0x01)
					{
					//find the number of colours to plot
					// by getting rid of the flag bit
					filedata >>= 1;

					// we are reading bytes so double the number
					File.Read(pixels, filedata<<1);
					pixels += filedata<<1;
					
					}
				else if(filedata == 0)
					{
					File.Read(&filedata,1);
					continue;
					}
				else 
					{
					//black
					// filedata is already multiplied by two
					// so keep it for the amount of bytes to write
					// since it is 2 bytes per pixel
					for(byte i = 0; i < filedata; i++ )
						{
						*pixels++ = 0x00;
						}
					}
				File.Read(&filedata,1);
		
				}
				File.Read(&filedata,1);
			}

		    //UINT uiBytesRead = File.Read(pData, iBytesNeeded); 

			CFIBitmap* pBitmap = new CFIBitmap(
				pSpriteHdr[i].Width, 
				-pSpriteHdr[i].Height, // SPR's are top-down bitmaps.
				NULL,
				Format,
				pData);

			m_BitmapArray.Add(pBitmap);
			delete[] pData;
			pData = NULL;
		}

		delete[] pSpriteHdr;
		pSpriteHdr = NULL;

		File.Close();
    }
    catch(CFileException* e)
    {
        e->ReportError();
		bSuccess = FALSE;
    }
	catch(CMemoryException* e)
    {
        e->ReportError();
		bSuccess = FALSE;
    }

    return bSuccess;
}


// ----------------------------------------------------------------------
// Method:      LoadC16With16BitTag 
// Arguments:   pFileName - file to read from
//			
// Returns:     None
//
// Description: Reads rle encoded pixel data like this:
//				each set of pixels are encoded by a byte.  For a run
//				of black pixels the low bit will be 0.  The rest of
//				the byte will be how many pixels in the run.
//				For a run of non black pixels the low bit will be 1.
//				The rest of the byte will be the number of coloured
//				pixels in the run.
//				The end of the sprite is a zero byte			
//			
// ----------------------------------------------------------------------
BOOL CSprite::LoadC16With16BitTag(const char* pFileName)
{
	BOOL bSuccess = TRUE;

    try
    {
        // Open & read file header information.
        CFile File(pFileName, CFile::modeRead | CFile::shareExclusive);

		S16_HEADER FileHeader = {0x00000000, 0x0000};
		   
		File.Read(&FileHeader, SizeOf_S16_HEADER);   
		
		// Read header information.
		SPRITE_HEADER* pSpriteHdr = new SPRITE_HEADER[FileHeader.ImageCount];
		int i;
		for (i = 0; i < FileHeader.ImageCount; i++)
		{
		//	File.Read(&pSpriteHdr[i], sizeof(SPRITE_HEADER));

			//read in the first offset
			File.Read(&(pSpriteHdr[i].iOffset), sizeof(int));

			// now the width and height
			File.Read(&(pSpriteHdr[i].Width), sizeof(short));
				// now the width and height
			File.Read(&(pSpriteHdr[i].Height), sizeof(short));
			// skip the other offsets
			File.Seek((pSpriteHdr[i].Height -1) * sizeof(int), CFile::current);
		}

		byte* pData = NULL;
		int iBytesNeeded = 0;
		PIXEL_FORMAT Format;
		if (FileHeader.Flags & S16_565_FORMAT)
			Format = FORMAT16_565;
		else
			Format = FORMAT16_555;

		// Read image data.
		for (i = 0; i < FileHeader.ImageCount; i++)
		{
			if(i == 0)
			{
				File.Seek(pSpriteHdr[i].iOffset, CFile::begin);
			}

			iBytesNeeded = pSpriteHdr[i].Width * pSpriteHdr[i].Height * 2; // *2 => 16bit.

			// create the bits needed
			pData = (byte*)new WORD[iBytesNeeded / 2];

			ASSERT(pData);
			byte* pixels = pData;

			//read the first byte from the file
			WORD filedata = 0;
			File.Read(&filedata,2);


			int height = pSpriteHdr[i].Height;
			int testCount =0;
			while(height--)
			{
				while(filedata !=0)
				{
				
				// check whether the run is transparent or colour
				if(filedata & 0x01)
					{
					//find the number of colours to plot
					// by getting rid of the flag bit
					filedata >>= 1;

					// we are reading bytes so double the number
					File.Read(pixels, filedata<<1);
					pixels += filedata<<1;
					
					}
				else if(filedata == 0)
					{
					File.Read(&filedata,2);
					continue;
					}
				else 
					{
					//black
					// filedata is already multiplied by two
					// so keep it for the amount of bytes to write
					// since it is 2 bytes per pixel
					for(int i = 0; i < filedata; i++ )
						{
						*pixels++ = 0x00;
						}
					}
				File.Read(&filedata,2);
		
				}
				File.Read(&filedata,2);
			}

		    //UINT uiBytesRead = File.Read(pData, iBytesNeeded); 

			CFIBitmap* pBitmap = new CFIBitmap(
				pSpriteHdr[i].Width, 
				-pSpriteHdr[i].Height, // SPR's are top-down bitmaps.
				NULL,
				Format,
				pData);

			m_BitmapArray.Add(pBitmap);
			delete[] pData;
			pData = NULL;
		}

		delete[] pSpriteHdr;
		pSpriteHdr = NULL;

		File.Close();
    }
    catch(CFileException* e)
    {
        e->ReportError();
		bSuccess = FALSE;
    }
	catch(CMemoryException* e)
    {
        e->ReportError();
		bSuccess = FALSE;
    }

    return bSuccess;
}

/*********************************************************************
* Public: SaveS16.
*********************************************************************/
BOOL CSprite::SaveS16(const char* pFileName, BOOL b565)
{
	BOOL bSuccess = TRUE;

	DeleteFile(pFileName);

    try
    {
        // Open & write image count.
        CFile File(pFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);

		// Write file header information.
		S16_HEADER FileHeader = {0x00000000, 0x0000};
		PIXEL_FORMAT Format = FORMAT16_555;
		if (b565)
		{
			FileHeader.Flags |= S16_565_FORMAT;
			Format = FORMAT16_565;
		}
		FileHeader.ImageCount = m_BitmapArray.GetSize();
        File.Write(&FileHeader, SizeOf_S16_HEADER);   

		int iOffset = SizeOf_S16_HEADER + (FileHeader.ImageCount * 8); // 1st ofset.

		// Write header information.
		SPRITE_HEADER SpriteHdr;
		int i;
		for (i = 0; i < FileHeader.ImageCount; i++)
		{
			SpriteHdr.iOffset = iOffset;
			SpriteHdr.Width = m_BitmapArray[i]->Width();
			SpriteHdr.Height = m_BitmapArray[i]->Height();
			File.Write(&SpriteHdr, sizeof(SPRITE_HEADER));

			iOffset += m_BitmapArray[i]->BytesUsed(Format);
		}

		byte* pData = NULL;
		int iBytesNeeded = 0;

		// Write image data.
		for (i = 0; i < FileHeader.ImageCount; i++)
		{
			CFIBitmap* pBitmap = m_BitmapArray[i];
			ASSERT(pBitmap);

			pBitmap->WritePixelDataAsS16(File, b565);
			
		}

		File.Close();
    }
    catch(CFileException* e)
    {
        e->ReportError();
		bSuccess = FALSE;
    }
	catch(CMemoryException* e)
    {
        e->ReportError();
		bSuccess = FALSE;
    }

    return bSuccess;
}

// ----------------------------------------------------------------------
// Method:      SaveC16 
// Arguments:   pFileName - file to write to
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
BOOL CSprite::SaveC16(const char* pFileName, BOOL b565)
{
	BOOL bSuccess = TRUE;

	DeleteFile(pFileName);

    try
    {
        // Open & write image count.
        CFile File(pFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);

		// Write file header information.
		S16_HEADER FileHeader = {0x00000000, 0x0000};
		PIXEL_FORMAT Format = FORMAT16_555;
		if (b565)
		{
			FileHeader.Flags |= S16_565_FORMAT;
			Format = FORMAT16_565;
		}
		FileHeader.ImageCount = m_BitmapArray.GetSize();
		FileHeader.Flags |= C16_16BIT_FORMAT;
        File.Write(&FileHeader, SizeOf_S16_HEADER);   

		int iOffset = SizeOf_S16_HEADER + (FileHeader.ImageCount * 8)
			; // 1st offset.


		// Write header information.
		SPRITE_HEADER SpriteHdr;
		int i;

		CDWordArray offsetPositions;
		CDWordArray offsets;
		CDWordArray scanlines;
		
		offsets.SetSize(0,4);
		offsetPositions.SetSize(0,4);

		for (i = 0; i < FileHeader.ImageCount; i++)
		{
			// collect the file positions where the offsets were writtten
			// so that we can update them later
			int pos = File.GetPosition();
			offsetPositions.Add(File.GetPosition());

			// only the first offset must be correct here the
			// others get udated on the fly.  Account for the additional
			// scanline offsets
			iOffset +=(sizeof(int) * (m_BitmapArray[i]->Height()-1));

			SpriteHdr.iOffset = iOffset;
			offsets.Add(iOffset);

			// do the width and height next so that I know 
			// howmany scanlines to read
			SpriteHdr.Width = m_BitmapArray[i]->Width();
			SpriteHdr.Height = m_BitmapArray[i]->Height();

			// now write space for the scanlines
			// rembebring that the first line is already stored
			// in the offset
			int size = SpriteHdr.Height-1;
			CDWordArray array;
			array.SetSize(size,4);

		
		//	File.Write(&SpriteHdr, sizeof(SPRITE_HEADER));

			File.Write(&(SpriteHdr.iOffset), sizeof(int));

			File.Write(&(SpriteHdr.Width), sizeof(short));
			File.Write(&(SpriteHdr.Height), sizeof(short));
		
			for(int x =0; x < SpriteHdr.Height-1; x++)
			{
			File.Write(&array[x],sizeof(int));
			}
			
	

			// these dont matter but we may as well have some value here
			iOffset += m_BitmapArray[i]->BytesUsed(Format) + (2 * (SpriteHdr.Height-1));
		}

		byte* pData = NULL;
		int iBytesNeeded = 0;

		int bytesUsed = 0;
		int data =0;
		DWORD currentPos = File.GetPosition();
		// Write image data.
		for (i = 0; i < FileHeader.ImageCount; i++)
		{
			scanlines.SetSize(m_BitmapArray[i]->Height(),4);
			CFIBitmap* pBitmap = m_BitmapArray[i];
			ASSERT(pBitmap);

			// fill in the very first offset now

			// write the first data offset
			File.Seek(offsetPositions[i],CFile::begin);
			File.Write(&currentPos,sizeof(DWORD));

			// go back to where the data should go
			File.Seek(currentPos,CFile::begin);
	
			// encoding the transparencies using a byte is marginally
			// more efficient
		//	bytesUsed = pBitmap->EncodeTransparencies(File, b565,scanlines);
			bytesUsed = pBitmap->EncodeTransparenciesUsingUINT16(File, b565,scanlines);
			
			//where are we now?
			currentPos = File.GetPosition();

				// write the scan lines next
			File.Seek(offsetPositions[i] + 8,CFile::begin);
			//now write the scan offsets
		
			for(int s = 0; s<m_BitmapArray[i]->Height()-1;s++)
				{
				data =  scanlines[s+1];
				File.Write(&data,sizeof(int));
				}
			
			File.Seek(currentPos,CFile::begin);
		}

		File.Close();
    }
    catch(CFileException* e)
    {
        e->ReportError();
		bSuccess = FALSE;
    }
	catch(CMemoryException* e)
    {
        e->ReportError();
		bSuccess = FALSE;
    }

    return bSuccess;
}

/*********************************************************************
* Private: LoadPalette.
*********************************************************************/
BOOL CSprite::LoadPalette(const char* FileName)
{
    ASSERT(FileName);

    HDC hDC = ::GetDC(NULL);
    int i;
    m_LogicalPalette.palVersion = 0x300;
    m_LogicalPalette.palNumEntries = 256;
    BOOL bSuccess = TRUE;

    // Create blank palette.
    for (i = 0; i < 256; i++)
    {
        m_LogicalPalette.palPalEntry[i].peRed = 0;
        m_LogicalPalette.palPalEntry[i].peGreen = 0;
        m_LogicalPalette.palPalEntry[i].peBlue = 0;
        m_LogicalPalette.palPalEntry[i].peFlags = PC_NOCOLLAPSE;
    }

    // Realize palette to clean up system colours (alledgedly!)
    HPALETTE hPalette = CreatePalette((LOGPALETTE*)&m_LogicalPalette);
    bSuccess &= (hPalette != NULL);
    HPALETTE hOldPalette = SelectPalette(hDC, hPalette, FALSE);
    bSuccess &= (hOldPalette != NULL);
    UINT uiCnt = ::RealizePalette(hDC);
    bSuccess &= (SelectPalette(hDC, hOldPalette, FALSE) != NULL);
    DeleteObject(hPalette);

    // Read system colours.
    int i0 = GetSystemPaletteEntries(hDC, 0, 10, m_LogicalPalette.palPalEntry);
    int i1 = GetSystemPaletteEntries(hDC, 246, 10, &(m_LogicalPalette.palPalEntry[246]));

    // Set flags.
    for (i = 0; i < i0; i++)
        m_LogicalPalette.palPalEntry[i].peFlags = 0;

    for (i = 246; i < 246 + i1; i++)
        m_LogicalPalette.palPalEntry[i].peFlags = 0;

    try
    {
        // Read palette colours from file.
        CFile File(FileName, CFile::modeRead | CFile::typeBinary);
        File.Seek(10 * 3, CFile::begin); 
        byte B = 0;

	    for (i = 10; i < 246; i++) 
	    {
            // Fill in the entries from the given RGB file
            File.Read(&B, 1);
		    m_LogicalPalette.palPalEntry[i].peRed = LOBYTE(B * 4);	// (file is 0-63, but Windows wants 0-255!)

            File.Read(&B, 1);
		    m_LogicalPalette.palPalEntry[i].peGreen = LOBYTE(B * 4);

            File.Read(&B, 1);
		    m_LogicalPalette.palPalEntry[i].peBlue = LOBYTE(B * 4);	

            m_LogicalPalette.palPalEntry[i].peFlags = PC_NOCOLLAPSE;

        
            // BIG PALETTE FIX:
            // Set these palette entries to almost black rather then black.
            // This prevents them being mapped to system colour 0 when
            // the palette is realised.
            // (This applies to 256 colour mode only).
            if (i == 243 || i == 244 || i == 245)
            {
                // 6 = a dark colour that is different enough from black.
    		    m_LogicalPalette.palPalEntry[i].peRed = 6;
    		    m_LogicalPalette.palPalEntry[i].peGreen = 6;
    		    m_LogicalPalette.palPalEntry[i].peBlue = 6;
            }            
        }
	    
        File.Close();
    }
    catch(CFileException* )
    {
        //e->ReportError();
        bSuccess &= FALSE;
    }

    // Select & realize game palette.
    hPalette = CreatePalette((LOGPALETTE*)&m_LogicalPalette);
    bSuccess &= (hPalette != NULL);
    hOldPalette = SelectPalette(hDC, hPalette, FALSE);
    bSuccess &= (hOldPalette != NULL);
    ::RealizePalette(hDC);
    ::ReleaseDC(NULL, hDC);

    return bSuccess;
}


/*********************************************************************
* Public: CreateGDI.
*********************************************************************/
BOOL CSprite::CreateGDI()
{
	int iCnt = m_BitmapArray.GetSize();
	BOOL bSuccess = TRUE;

	for (int i = 0; i < iCnt; i++)
	{
		bSuccess &= m_BitmapArray[i]->CreateGDI();
		if (!bSuccess)
			break;
	}

	return bSuccess;
}

