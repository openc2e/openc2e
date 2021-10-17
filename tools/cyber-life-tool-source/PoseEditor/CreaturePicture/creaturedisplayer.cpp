// Description: CCreatureDisplayer - Displays/animates creatures
// Author: Robert Dick
// Date: 7/12/98

#include "stdafx.h"

#include <vector>

#define CREATURE_DISPLAYER_CPP
#include "CreatureDisplayer.h"

#include <math.h>

 
static CString GenusNames[3] = {
	"Norn", 
	"Grendel", 
	"Ettin"
};

static CString BodyPartNames[] = {
	"",				// this is direction
	"head",
	"body",
	"left thigh",
	"left shin",
	"left foot",
	"right thigh"
	"right shin",
	"right foot",
	"left humerus",
	"left radius",
	"right humerus",
	"right radius",
	"tail root",
	"tail tip",
	"left ear",
	"right ear",
	"hair"
};

static CString SexNames[2] = {
	"male", 
	"female"
};

static CString AgeNames[7] = {
	"baby",
	"child",
	"adolescent",
	"youth",
	"adult",
	"old",
	"senile"
};


//
// Private Member Functions
//



void CCreatureDisplayer::Cleanup(void)
{
	int ImageNumber;
	int BodyPartNumber;

	// Make sure that all pixel data memory is destroyed and that all pixel
	// data pointers are reset to NULL
	for (BodyPartNumber=HEAD; BodyPartNumber<NUM_BODY_PARTS; BodyPartNumber++) {
		for (ImageNumber=0; ImageNumber<IMAGE_COUNT; ImageNumber++) {
			if (m_BodyPartData[BodyPartNumber].ImagePixelData[ImageNumber].PixelData != NULL) {
				delete []m_BodyPartData[BodyPartNumber].ImagePixelData[ImageNumber].PixelData;
				m_BodyPartData[BodyPartNumber].ImagePixelData[ImageNumber].PixelData = NULL;
			}
		}
	}
}


// Sex is 0 male, 1 female
// Genus is 0 norn, 1 grendel, 2 ettin
// Age is 0 to 6
// Variant 0 to 26
// Body part is HEAD to TAIL_TIP = 1 to 14
// Sex, age and variant are preferences
BOOL CCreatureDisplayer::OpenMatchingFile
	(int     iGenus,
	 int     iBodyPart,
	 int     iPreferredSex,				
	 int     iPreferredAge,			
	 int     iPreferredVariant,
	 int     & iSexFound,				
	 int     & iAgeFound,			
	 int     & iVariantFound,
	 CString strExtension,
	 CString strDirectory,
	 CFile & theFile)				
{
	int         iCreatureType;
	int         iAge;
	int         iVariant;
	CString     strFilename;
	CFileStatus dummy;

	iBodyPart--;

	iCreatureType = iGenus + iPreferredSex*4 + '0';
	iBodyPart += 'a';
	iPreferredSex += '0';
	iPreferredAge += '0';
	iPreferredVariant += 'a';
	
	//
	// Try with the preferred sex
	//

	for (iVariant=iPreferredVariant; iVariant>='a'; iVariant--) {
		for (iAge=iPreferredAge; iAge>='0'; iAge--) {
			// Construct a filename
			strFilename.Format("%s%c%c%c%c%s", strDirectory, iBodyPart, 
				iCreatureType, iAge, iVariant, strExtension);
	
			if (CFile::GetStatus(strFilename, dummy)) { 
				if( theFile.Open(strFilename, CFile::modeRead) )
				{
					iSexFound = iPreferredSex - '0';
					iAgeFound = iPreferredAge - '0';
					iVariantFound = iPreferredVariant - 'a';
					return TRUE;
				}
			}
		}
	}

	// Try with the opposite sex, as it were
	if (iCreatureType <= '2') {
		iCreatureType += 4;
		iPreferredSex++;
	}
	else  {
		iCreatureType -= 4;
		iPreferredSex--;
	}

	for (iVariant=iPreferredVariant; iVariant>='a'; iVariant--) {
		for (iAge=iPreferredAge; iAge>='0'; iAge--) {
			// Construct a filename
			strFilename.Format("%s%c%c%c%c%s", strDirectory, iBodyPart, 
				iCreatureType, iAge, iVariant, strExtension);

			if (CFile::GetStatus(strFilename, dummy)) {

				if( theFile.Open(strFilename, CFile::modeRead) )
				{

					iSexFound = iPreferredSex - '0';
					iAgeFound = iPreferredAge - '0';
					iVariantFound = iPreferredVariant - 'a';

					return TRUE;
				}
			}
		}
	}
	return FALSE;
}


void CCreatureDisplayer::GetPixelDataForImage
	(BYTE *				SpriteFileBuffer, 
	 ImagePixelData_t & ImagePixelData, 
	 int				ImageNumber)
{
	BYTE *	ImageHeaderStart;
	DWORD *	pdw;
	WORD *	pw;
	WORD	Width;
	WORD	Height;
	WORD	Pixel;
	DWORD	Offset;
	int		Row;
	int		Column;
	BYTE	Red;
	BYTE	Green;
	BYTE	Blue;
	CPoint	TopLeft;
	CPoint	BottomRight;
	DWORD   SpriteFormat;

	// Get the sprite format indicator: 0 = 555, non-zero = 565
	pdw = (DWORD *)SpriteFileBuffer;
	SpriteFormat = *pdw;
	int nImages = *((WORD *)(SpriteFileBuffer + 4));
	if( !nImages )
	{
		ImagePixelData.Width = 0;
		ImagePixelData.Height = 0;
		ImagePixelData.PixelData = 0;
		ImagePixelData.MBR.SetRect(0, 0, 0, 0);
		return;
	}
	// Skip past the flags and the image count
	ImageHeaderStart = SpriteFileBuffer + 6;
	// Point at the appropriare image header
	ImageHeaderStart += ImageNumber * 8;
	// Get the offset, width and height of the image
	pdw = (DWORD *)ImageHeaderStart;
	Offset = *pdw;
	pw = (WORD *)(ImageHeaderStart + 4);
	Width = *pw;
	pw = (WORD *)(ImageHeaderStart + 6);
	Height = *pw;
	// Store the image width and height
	ImagePixelData.Width = Width;
	ImagePixelData.Height = Height;
	// Create heap memory for the pixel data 
	ImagePixelData.PixelData = new COLORREF[Width * Height];
	// Set default MBR
	TopLeft.x = UNREAL_MBR_VALUE;
	TopLeft.y = UNREAL_MBR_VALUE;
	BottomRight.x = -UNREAL_MBR_VALUE;
	BottomRight.y = -UNREAL_MBR_VALUE;
	// Fill in the pixel data array and calculate the image's MBR
	for (Row=0; Row<Height; Row++) {
		for (Column=0; Column<Width; Column++) {
			pw = (WORD *)(SpriteFileBuffer + Offset + Row*Width*2 + Column*2);
			Pixel = *pw;
			// Black pixel is transparent and is not considered to
			// be a part of the image
			if (Pixel != 0) {
				// Update the MBR top left point 
				if (Column < TopLeft.x)
					TopLeft.x = Column;
				if (Row < TopLeft.y)
					TopLeft.y = Row;
				// Update the MBR bottom right point
				if (Column > BottomRight.x)
					BottomRight.x = Column;
				if (Row > BottomRight.y)
					BottomRight.y = Row;
				// Extract the RGB value from the 16-bit pixel value
				if (SpriteFormat == 1) {
					// We've got 565
					Red = (BYTE)((Pixel >> 8) & 0xF8);
					Green = (BYTE)((Pixel >> 3) & 0xFC);
					Blue = (BYTE)((Pixel << 3) & 0xF8);
				}
				else {
					// We've got 555
					Red = (BYTE)((Pixel >> 7) & 0xF8);
					Green = (BYTE)((Pixel >> 2) & 0xF8);
					Blue = (BYTE)((Pixel << 3) & 0xF8);
				}
				ImagePixelData.PixelData[Row*Width + Column] = RGB(Red, Green, Blue);
			}
			else
				// this can never naturally come about since RGB is 24-bit
				ImagePixelData.PixelData[Row*Width + Column] = 0xFFFFFFFF;
		}
	}


	// Store the MBR of the image 
	ImagePixelData.MBR.SetRect(TopLeft.x, TopLeft.y, BottomRight.x, BottomRight.y);
}


void CCreatureDisplayer::GetCompressedPixelDataForImage
	(BYTE *				SpriteFileBuffer, 
	 ImagePixelData_t & ImagePixelData, 
	 int				ImageNumber)
{
	BYTE *	ImageHeaderStart;
	DWORD *	pdw;
	WORD *	pw;
	WORD	Width;
	WORD	Height;
	WORD	Pixel;
	DWORD	Offset;
	int		Row;
	int		Column;
	BYTE	Red;
	BYTE	Green;
	BYTE	Blue;
	CPoint	TopLeft;
	CPoint	BottomRight;
	DWORD   SpriteFormat;

	// Get the sprite format indicator: 0 = 555, non-zero = 565
	pdw = (DWORD *)SpriteFileBuffer;
	SpriteFormat = *pdw;
	int nImages = *((WORD *)(SpriteFileBuffer + 4));
	if( nImages <= ImageNumber )
	{
		ImagePixelData.Width = 0;
		ImagePixelData.Height = 0;
		ImagePixelData.PixelData = 0;
		ImagePixelData.MBR.SetRect(0, 0, 0, 0);
		return;
	}
	// Skip past the flags and the image count
	ImageHeaderStart = SpriteFileBuffer + 6;
	//Skip thru images until we reach ours
	while( ImageNumber-- )
	{
		// Get the offset, width and height of the image
		pdw = (DWORD *)ImageHeaderStart;
		Offset = *pdw;
		pw = (WORD *)(ImageHeaderStart + 4);
		Width = *pw;
		pw = (WORD *)(ImageHeaderStart + 6);
		Height = *pw;
		// Point at the appropriare image header
		ImageHeaderStart += 8 + 4 * (Height - 1);
	}
	pdw = (DWORD *)ImageHeaderStart;
	Offset = *pdw;
	pw = (WORD *)(ImageHeaderStart + 4);
	Width = *pw;
	pw = (WORD *)(ImageHeaderStart + 6);
	Height = *pw;

	//read scanline offsets (we already have one in Offset)
	std::vector< int > offsets;
	offsets.reserve( Height );
	offsets.push_back( Offset );

	pdw = (DWORD *)(ImageHeaderStart + 8);
	for( int i = 1; i < Height; ++i ) offsets.push_back( *pdw++ );
	// Store the image width and height

	ImagePixelData.Width = Width;
	ImagePixelData.Height = Height;

	// Create heap memory for the pixel data 
	ImagePixelData.PixelData = new COLORREF[Width * Height];
	COLORREF *pRGB = ImagePixelData.PixelData;

	// Set default MBR
	TopLeft.x = UNREAL_MBR_VALUE;
	TopLeft.y = UNREAL_MBR_VALUE;
	BottomRight.x = -UNREAL_MBR_VALUE;
	BottomRight.y = -UNREAL_MBR_VALUE;

	// Fill in the pixel data array and calculate the image's MBR
	for (Row=0; Row<Height; Row++)
	{
		WORD *pb = (WORD *)(SpriteFileBuffer + offsets[Row]);
		Column = 0;
		while( Column < Width )
		{
			int count = *pb >> 1;
			if( *pb & 1 ) //opaque
			{
				if (Column < TopLeft.x)
					TopLeft.x = Column;
				if (Row < TopLeft.y)
					TopLeft.y = Row;
				Column += count;
				if (Column > BottomRight.x)
					BottomRight.x = Column;
				if (Row > BottomRight.y)
					BottomRight.y = Row;
				pw = (WORD *)(pb + 1);
				while( count-- )
				{
					Pixel = *pw++;
					if (SpriteFormat & 1) {
						// We've got 565
						Red = (BYTE)((Pixel >> 8) & 0xF8);
						Green = (BYTE)((Pixel >> 3) & 0xFC);
						Blue = (BYTE)((Pixel << 3) & 0xF8);
					}
					else {
						// We've got 555
						Red = (BYTE)((Pixel >> 7) & 0xF8);
						Green = (BYTE)((Pixel >> 2) & 0xF8);
						Blue = (BYTE)((Pixel << 3) & 0xF8);
					}
					*pRGB++= RGB(Red, Green, Blue);
				}
				pb = pw;
			}
			else //transparent
			{
				Column += count;
				while( count-- ) *pRGB++ = 0xFFFFFFFF;
				++pb;
			}
		}
	}


	// Store the MBR of the image 
	ImagePixelData.MBR.SetRect(TopLeft.x, TopLeft.y, BottomRight.x, BottomRight.y);
}




void CCreatureDisplayer::GetAttachmentDataForImage
	(char * &				 ImageAttachmentDataPointer,
	 ImageAttachmentData_t & ImageAttachmentData, 
	 int					 NumberOfAttachmentPoints)
{
	int    PointNumber;
	WORD   x, y;
	char * TempPointer;
	CPoint TempPoint;

	for (PointNumber=0; PointNumber<NumberOfAttachmentPoints; PointNumber++) {
		// Read the base-10 x coordinate
		x = (WORD)strtol(ImageAttachmentDataPointer, &TempPointer, 10);
		ImageAttachmentDataPointer = TempPointer;
		// Read the base-10 y coordinate
		y = (WORD)strtol(ImageAttachmentDataPointer, &TempPointer, 10);
		ImageAttachmentDataPointer = TempPointer;
		// Store the point in the attachment point array
		TempPoint.x = x;
		TempPoint.y = y;
		ImageAttachmentData.AttachmentData[PointNumber] = TempPoint;
	}
}



BOOL CCreatureDisplayer::GetDataForBodyPart
	(int              iBodyPart,
	 int              iGenus,
	 int              iSex,				
	 int              iAge,			
	 int              iVariant,
	 CString		  strSpriteDirectory,
	 CString		  strAttachmentDirectory, 
	 BodyPartData_t & BodyPartData,
	 CString  & FailReason)
{
	BYTE *	SpriteBuffer;
	char *	AttachmentBuffer;
	char *	AttachmentBufferPointer;
	int		FileLength;
	BOOL	Opened;
	int		ImageNumber;
	int		NumberOfAttachmentPoints;
	CFile	f;

	int            iSexFound;				
	int            iAgeFound;			
	int            iVariantFound;

	if (iBodyPart == BODY) 
		// Body attachment files have six points
		NumberOfAttachmentPoints = 6;
	else if (iBodyPart == HEAD) 
		// Body attachment files have six points
		NumberOfAttachmentPoints = 5;
	else if (iBodyPart == HAIR) 
		// Body attachment files have six points
		NumberOfAttachmentPoints = 1;
	else
		// All other files just have two
		NumberOfAttachmentPoints = 2;
	
	// Try to open the sprite file
	Opened = OpenMatchingFile(iGenus, iBodyPart, iSex, iAge, iVariant,
		iSexFound, iAgeFound, iVariantFound, ".c16", strSpriteDirectory, f);

	if (!Opened) {
		// Couldn't open the sprite file, inform the caller
		FailReason.Format("Unable to find a %s sprite file for %s %s %s of variant %c.\n[Tried '%s%c%c%c%c.s16']",
			BodyPartNames[iBodyPart], AgeNames[iAge], SexNames[iSex], GenusNames[iGenus], 
			iVariant+'a', strSpriteDirectory, iBodyPart-1+'a', iGenus+iSex*4+'0', iAge+'0', iVariant+'a');
		return FALSE;
	}
	// Create a heap memory buffer and read the sprite file into it
	FileLength = f.GetLength();
	SpriteBuffer = new BYTE[FileLength];
	f.Read(SpriteBuffer, FileLength);
	// Close the sprite file
	f.Close();

	// Try to open the attachment file
	Opened = OpenMatchingFile(iGenus, iBodyPart, iSex, iAge, iVariant,
		iSexFound, iAgeFound, iVariantFound, ".att", strAttachmentDirectory, f);
	if (!Opened) {
		// Couldn't open the attachment file, inform the caller
		FailReason.Format("Unable to find a %s attachment file for %s %s %s of variant %c.\n[Tried '%s%c%c%c%c.att']",
			BodyPartNames[iBodyPart], AgeNames[iAge], SexNames[iSex], GenusNames[iGenus], 
			iVariant+'a', strAttachmentDirectory, iBodyPart-1+'a', iGenus+iSex*4+'0', iAge+'0', iVariant+'a');
		delete []SpriteBuffer;
		return FALSE;
	}
	// Create a heap memory buffer and read the attachment file into it
	FileLength = f.GetLength();
	AttachmentBuffer = new char[FileLength];
	f.Read(AttachmentBuffer, FileLength);
	// Close the attachment file
	f.Close();

	AttachmentBufferPointer = AttachmentBuffer;
	for (ImageNumber=0; ImageNumber<IMAGE_COUNT; ImageNumber++) {
		// Get the pixel data for each image
		GetCompressedPixelDataForImage(SpriteBuffer, 
			BodyPartData.ImagePixelData[ImageNumber], ImageNumber);
		// Get the attachment data for each image
		GetAttachmentDataForImage(AttachmentBufferPointer, 
			BodyPartData.ImageAttachmentData[ImageNumber], 
			NumberOfAttachmentPoints);
	}
	// Destroy the temporary heap buffers
	delete []SpriteBuffer;
	delete []AttachmentBuffer;

	// No problems
	return TRUE;
}


void CCreatureDisplayer::ConvertPoseDescriptor
	(int VirtualPoseDescriptor[POSE_DESCRIPTOR_SIZE],
	int PoseDescriptor[POSE_DESCRIPTOR_SIZE])
{
	int i;

	for (i=0; i<POSE_DESCRIPTOR_SIZE; i++)
		PoseDescriptor[i] = VirtualPoseDescriptor[i] & 0x7;
	// Deal with direction
	if (PoseDescriptor[DIRECTION] == POSE_TO_IT)
		PoseDescriptor[DIRECTION] = DIRECTION_RIGHT;
	else if (PoseDescriptor[DIRECTION] == POSE_FROM_IT)
		PoseDescriptor[DIRECTION] = DIRECTION_LEFT;
	// Deal with head
	if (PoseDescriptor[HEAD] == POSE_TO_IT)
		PoseDescriptor[HEAD] = POSE_UP;
}


void CCreatureDisplayer::GetImageNumbers
	(int PoseDescriptor[POSE_DESCRIPTOR_SIZE],
	 int ImageNumbers[POSE_DESCRIPTOR_SIZE])
{
	int Head;
	int Direction;
	int BodyPartNumber;

	int DirStart[4];
	DirStart[ DIRECTION_INWARDS ] = IMAGE_INWARDS_LOWEST;
	DirStart[ DIRECTION_OUTWARDS ] = IMAGE_OUTWARDS_LOWEST;
	DirStart[ DIRECTION_RIGHT ] = IMAGE_RIGHT_LOWEST;
	DirStart[ DIRECTION_LEFT ] = IMAGE_LEFT_LOWEST;

	// Which direction is the creature facing in this pose?
	Direction = PoseDescriptor[DIRECTION];

	// What is the position of the head in this pose?
	Head = PoseDescriptor[HEAD];

	for (BodyPartNumber=HEAD; BodyPartNumber<POSE_DESCRIPTOR_SIZE; BodyPartNumber++)
		ImageNumbers[BodyPartNumber] = DirStart[ Direction ] + PoseDescriptor[BodyPartNumber];

	if( Head == POSE_OUTWARDS ) ImageNumbers[HEAD] = IMAGE_OUTWARDS_HORIZONTAL;
	if( Head == POSE_INWARDS ) ImageNumbers[HEAD] = IMAGE_INWARDS_HORIZONTAL;
/*
	if (Direction == DIRECTION_INWARDS) {
		if (Head >= POSE_LOWEST && Head <= POSE_UP)
			// Invalid head position, force to be facing inwards
			ImageNumbers[HEAD] = IMAGE_INWARDS;
		else	
			// Head faces either inwards or outwards
			ImageNumbers[HEAD] = Head+4;
		// All other body parts must face inwards
		for (BodyPartNumber=BODY; BodyPartNumber<POSE_DESCRIPTOR_SIZE; BodyPartNumber++) {
			ImageNumbers[BodyPartNumber] = IMAGE_INWARDS;
		}
	}
	else if (Direction == DIRECTION_OUTWARDS) {
		if (Head >= POSE_LOWEST && Head <= POSE_UP)
			// Invalid head position, force to be facing outwards
			ImageNumbers[HEAD] = IMAGE_OUTWARDS;
		else	
			// Head faces either inwards or outwards
			ImageNumbers[HEAD] = Head+4;
		// All other body parts must face outwards
		for (BodyPartNumber=BODY; BodyPartNumber<POSE_DESCRIPTOR_SIZE; BodyPartNumber++) {
			ImageNumbers[BodyPartNumber] = IMAGE_OUTWARDS;
		}			
	}
	else if (Direction == DIRECTION_RIGHT) {
		if (Head >= POSE_LOWEST && Head <= POSE_UP)
			ImageNumbers[HEAD] = Head;
		else
			ImageNumbers[HEAD] = Head+4;
		for (BodyPartNumber=BODY; BodyPartNumber<POSE_DESCRIPTOR_SIZE; BodyPartNumber++) {
			ImageNumbers[BodyPartNumber] = PoseDescriptor[BodyPartNumber];
		}
	}
	else {
		// Facing Left
		ImageNumbers[HEAD] = Head+4;	
		for (BodyPartNumber=BODY; BodyPartNumber<POSE_DESCRIPTOR_SIZE; BodyPartNumber++) {
			ImageNumbers[BodyPartNumber] = PoseDescriptor[BodyPartNumber] + 4;
		}
	}
*/
}


void CCreatureDisplayer::GetBodyPartVectors
	(int    ImageNumbers[POSE_DESCRIPTOR_SIZE], 
	 CPoint BodyPartVectors[POSE_DESCRIPTOR_SIZE]) 
{
	CPoint p1, p2, p3;
	int    BodyImageNumber;
	int    ImageNumber;

	// There's no need to shift the body since all body parts are relative to it
	BodyPartVectors[BODY].x = 0;
	BodyPartVectors[BODY].y = 0;
	// Get the image number of the body
	BodyImageNumber = ImageNumbers[BODY];

	// Connect the head to the body
	p1 = m_BodyPartData[BODY].ImageAttachmentData[BodyImageNumber].AttachmentData[0];
	ImageNumber = ImageNumbers[HEAD];
	p2 = m_BodyPartData[HEAD].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	BodyPartVectors[HEAD] = p3;

	// Connect the left thigh to the body
	p1 = m_BodyPartData[BODY].ImageAttachmentData[BodyImageNumber].AttachmentData[1];
	ImageNumber = ImageNumbers[LEFT_THIGH];
	p2 = m_BodyPartData[LEFT_THIGH].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	BodyPartVectors[LEFT_THIGH] = p3;

	//.Connect the right thigh to the body
	p1 = m_BodyPartData[BODY].ImageAttachmentData[BodyImageNumber].AttachmentData[2];
	ImageNumber = ImageNumbers[RIGHT_THIGH];
	p2 = m_BodyPartData[RIGHT_THIGH].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	BodyPartVectors[RIGHT_THIGH] = p3;

	//.Connect the left humerus to the body
	p1 = m_BodyPartData[BODY].ImageAttachmentData[BodyImageNumber].AttachmentData[3];
	ImageNumber = ImageNumbers[LEFT_HUMERUS];
	p2 = m_BodyPartData[LEFT_HUMERUS].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	BodyPartVectors[LEFT_HUMERUS] = p3;

	//.Connect the right humerus to the body
	p1 = m_BodyPartData[BODY].ImageAttachmentData[BodyImageNumber].AttachmentData[4];
	ImageNumber = ImageNumbers[RIGHT_HUMERUS];
	p2 = m_BodyPartData[RIGHT_HUMERUS].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	BodyPartVectors[RIGHT_HUMERUS] = p3;

	// Connect the tail root to the body
	p1 = m_BodyPartData[BODY].ImageAttachmentData[BodyImageNumber].AttachmentData[5];
	ImageNumber = ImageNumbers[TAIL_ROOT];
	p2 = m_BodyPartData[TAIL_ROOT].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	BodyPartVectors[TAIL_ROOT] = p3;
	
	//.Connect the left radius to the left humerus
	ImageNumber = ImageNumbers[LEFT_HUMERUS];
	p1 = m_BodyPartData[LEFT_HUMERUS].ImageAttachmentData[ImageNumber].AttachmentData[1];
	ImageNumber = ImageNumbers[LEFT_RADIUS];
	p2 = m_BodyPartData[LEFT_RADIUS].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	p3 += BodyPartVectors[LEFT_HUMERUS];
	BodyPartVectors[LEFT_RADIUS] = p3;

	//.Connect the right radius to the right humerus
	ImageNumber = ImageNumbers[RIGHT_HUMERUS];
	p1 = m_BodyPartData[RIGHT_HUMERUS].ImageAttachmentData[ImageNumber].AttachmentData[1];
	ImageNumber = ImageNumbers[RIGHT_RADIUS];
	p2 = m_BodyPartData[RIGHT_RADIUS].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	p3 += BodyPartVectors[RIGHT_HUMERUS];
	BodyPartVectors[RIGHT_RADIUS] = p3;

	//.Connect the left shin to the left thigh
	ImageNumber = ImageNumbers[LEFT_THIGH];
	p1 = m_BodyPartData[LEFT_THIGH].ImageAttachmentData[ImageNumber].AttachmentData[1];
	ImageNumber = ImageNumbers[LEFT_SHIN];
	p2 = m_BodyPartData[LEFT_SHIN].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	p3 += BodyPartVectors[LEFT_THIGH];
	BodyPartVectors[LEFT_SHIN] = p3;

	//.Connect the right shin to the right thigh
	ImageNumber = ImageNumbers[RIGHT_THIGH];
	p1 = m_BodyPartData[RIGHT_THIGH].ImageAttachmentData[ImageNumber].AttachmentData[1];
	ImageNumber = ImageNumbers[RIGHT_SHIN];
	p2 = m_BodyPartData[RIGHT_SHIN].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	p3 += BodyPartVectors[RIGHT_THIGH];
	BodyPartVectors[RIGHT_SHIN] = p3;

	//.Connect the left foot to the left shin
	ImageNumber = ImageNumbers[LEFT_SHIN];
	p1 = m_BodyPartData[LEFT_SHIN].ImageAttachmentData[ImageNumber].AttachmentData[1];
	ImageNumber = ImageNumbers[LEFT_FOOT];
	p2 = m_BodyPartData[LEFT_FOOT].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	p3 += BodyPartVectors[LEFT_SHIN];
	BodyPartVectors[LEFT_FOOT] = p3;

	//.Connect the right foot to the right shin
	ImageNumber = ImageNumbers[RIGHT_SHIN];
	p1 = m_BodyPartData[RIGHT_SHIN].ImageAttachmentData[ImageNumber].AttachmentData[1];
	ImageNumber = ImageNumbers[RIGHT_FOOT];
	p2 = m_BodyPartData[RIGHT_FOOT].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	p3 += BodyPartVectors[RIGHT_SHIN];
	BodyPartVectors[RIGHT_FOOT] = p3;

	// Connect the tail tip to the tail root
	ImageNumber = ImageNumbers[TAIL_ROOT];
	p1 = m_BodyPartData[TAIL_ROOT].ImageAttachmentData[ImageNumber].AttachmentData[1];
	ImageNumber = ImageNumbers[TAIL_TIP];
	p2 = m_BodyPartData[TAIL_TIP].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	p3 += BodyPartVectors[TAIL_ROOT];
	BodyPartVectors[TAIL_TIP] = p3;

// And the head bone's connected to the... ear bone?
	ImageNumber = ImageNumbers[HEAD];
	p1 = m_BodyPartData[HEAD].ImageAttachmentData[ImageNumber].AttachmentData[2];
	p2 = m_BodyPartData[LEFT_EAR].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	p3 += BodyPartVectors[HEAD];
	BodyPartVectors[LEFT_EAR] = p3;

	ImageNumber = ImageNumbers[HEAD];
	p1 = m_BodyPartData[HEAD].ImageAttachmentData[ImageNumber].AttachmentData[3];
	p2 = m_BodyPartData[RIGHT_EAR].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	p3 += BodyPartVectors[HEAD];
	BodyPartVectors[RIGHT_EAR] = p3;


	ImageNumber = ImageNumbers[HEAD];
	p1 = m_BodyPartData[HEAD].ImageAttachmentData[ImageNumber].AttachmentData[4];
	p2 = m_BodyPartData[HAIR].ImageAttachmentData[ImageNumber].AttachmentData[0];
	p3 = p1 - p2;
	p3 += BodyPartVectors[HEAD];
	BodyPartVectors[HAIR] = p3;


}


void CCreatureDisplayer::GetZOrder(int Direction, 
								   int ZOrder[NUM_BODY_PARTS]) 
{
	int i = 0;
	if (Direction == DIRECTION_INWARDS) {
		ZOrder[i++] = LEFT_EAR;
		ZOrder[i++] = RIGHT_EAR;
		ZOrder[i++] = HEAD;
		ZOrder[i++] = HAIR;
		ZOrder[i++] = LEFT_HUMERUS;
		ZOrder[i++] = LEFT_RADIUS;
		ZOrder[i++] = RIGHT_HUMERUS,
		ZOrder[i++] = RIGHT_RADIUS;
		ZOrder[i++] = LEFT_SHIN;
		ZOrder[i++] = RIGHT_SHIN;
		ZOrder[i++] = LEFT_THIGH;
		ZOrder[i++] = RIGHT_THIGH;
		ZOrder[i++] = LEFT_FOOT;
		ZOrder[i++] = RIGHT_FOOT;
		ZOrder[i++] = BODY;
		ZOrder[i++] = TAIL_ROOT;
		ZOrder[i++] = TAIL_TIP;
	}
	else if (Direction == DIRECTION_OUTWARDS) {
		ZOrder[i++] = LEFT_SHIN;
		ZOrder[i++] = RIGHT_SHIN;
		ZOrder[i++] = LEFT_THIGH;
		ZOrder[i++] = RIGHT_THIGH;
		ZOrder[i++] = LEFT_FOOT;
		ZOrder[i++] = RIGHT_FOOT,
		ZOrder[i++] = TAIL_TIP;
		ZOrder[i++] = TAIL_ROOT;
		ZOrder[i++] = LEFT_EAR;
		ZOrder[i++] = RIGHT_EAR;
		ZOrder[i++] = HAIR;
		ZOrder[i++] = BODY;
		ZOrder[i++] = HEAD; // maybe should be after arms?
		ZOrder[i++] = LEFT_HUMERUS;
		ZOrder[i++] = LEFT_RADIUS;
		ZOrder[i++] = RIGHT_HUMERUS;
		ZOrder[i++] = RIGHT_RADIUS;
	}
	else if (Direction == DIRECTION_RIGHT) {
		ZOrder[i++] = LEFT_HUMERUS;
		ZOrder[i++] = LEFT_RADIUS;
		ZOrder[i++] = LEFT_THIGH;
		ZOrder[i++] = LEFT_SHIN;
		ZOrder[i++] = LEFT_FOOT,
		ZOrder[i++] = BODY;
		ZOrder[i++] = TAIL_ROOT;
		ZOrder[i++] = TAIL_TIP;
		ZOrder[i++] = LEFT_EAR;
		ZOrder[i++] = HEAD;
		ZOrder[i++] = HAIR;
		ZOrder[i++] = RIGHT_EAR;
		ZOrder[i++] = RIGHT_FOOT;
		ZOrder[i++] = RIGHT_SHIN;
		ZOrder[i++] = RIGHT_THIGH;
		ZOrder[i++] = RIGHT_HUMERUS;
		ZOrder[i++] = RIGHT_RADIUS;
	}
	else {
		// Facing Left
		ZOrder[i++] = RIGHT_HUMERUS;
		ZOrder[i++] = RIGHT_RADIUS;
		ZOrder[i++] = RIGHT_THIGH;
		ZOrder[i++] = RIGHT_SHIN;
		ZOrder[i++] = RIGHT_FOOT,
		ZOrder[i++] = BODY;
		ZOrder[i++] = TAIL_ROOT;
		ZOrder[i++] = TAIL_TIP;
		ZOrder[i++] = RIGHT_EAR;
		ZOrder[i++] = HEAD;
		ZOrder[i++] = HAIR;
		ZOrder[i++] = LEFT_EAR;
		ZOrder[i++] = LEFT_THIGH;
		ZOrder[i++] = LEFT_SHIN;
		ZOrder[i++] = LEFT_FOOT;
		ZOrder[i++] = LEFT_HUMERUS;
		ZOrder[i++] = LEFT_RADIUS;
	}
}



void CCreatureDisplayer::UpdatePose
	(int CurrentPose[POSE_DESCRIPTOR_SIZE], 
	 int TargetPose[POSE_DESCRIPTOR_SIZE],
	 int NextPose[POSE_DESCRIPTOR_SIZE])
{

	// This is taken from the Creatures code...

    int    Pose[POSE_DESCRIPTOR_SIZE];
    int     Status;
    int     NextDirn;
	int     CurrDir = CurrentPose[0];
	int i;

    static int Next[6][4] = {
// target: N    S    E    W
		{ '52','42','12','42' },    // Body=E Head=Straight
		{ '50','41','12','41' },    // Body=E Head=Askance

		{ '53','43','43','13' },    // Body=W Head=Straight
		{ '50','41','41','13' },    // Body=W Head=Askance

		{ '50','41','12','13' },    // Body=N/S Head=Straight
		{ '50','41','12','13' },    // Body=N/S Head=Askance
	};


	for (i=0; i<POSE_DESCRIPTOR_SIZE; i++)
		NextPose[i] = TargetPose[i];

	for (i=0; i<POSE_DESCRIPTOR_SIZE; i++)
		Pose[i] = TargetPose[i];

    if  ((Pose[DIRECTION]!=CurrentPose[DIRECTION])) {      // nowt to do, otherwise...

        switch (CurrDir) {                      // get hi bits of curr status
        case DIRECTION_RIGHT:                              // ie. E=0 W=2 N/S=4
            Status=0;
            break;
        case DIRECTION_LEFT:
            Status=2;
            break;
        default:
            Status=4;
            break;
        }

        if  (CurrentPose[HEAD]>=4)              // get lo bit of status
		{
            Status++;                           // ie. straight=0 askance=1
			if (CurrentPose[HEAD]==4 && CurrDir==POSE_INWARDS)
			{
				Pose[DIRECTION]=DIRECTION_RIGHT;			// if facing north, but looking
			}								// to camera, face east.
		}

        NextDirn = Next[Status]
                       [Pose[DIRECTION]];       // get new dirn & head angle


        Pose[HEAD] = (NextDirn>>8) - '0';              // store new direction
        Pose[DIRECTION] = (NextDirn&0x00FF) - '0';          // and head angle in pose

   

    }

   
    NextPose[DIRECTION] = Pose[DIRECTION];                  // dirn is already incrmntl

    for (i=1; i<POSE_DESCRIPTOR_SIZE; i++) {              // fr each entry in pose
                                                    // (excl. dirn)
                 // if relevant...

        if  (Pose[i]>CurrentPose[i]) {             // next pose is
            NextPose[i] = CurrentPose[i]+1;        // incrementally different
							// update emitter locus
		}
        else if (Pose[i]<CurrentPose[i]) {
            NextPose[i] = CurrentPose[i]-1;
							// update emitter locus
		}

       
    }

    if  (Pose[HEAD]>=4)                          // DON'T do incrementally
        NextPose[HEAD]=Pose[HEAD];                // when turning to or from
    else if ((CurrentPose[HEAD]>=4)                 // south or
             &&(Pose[HEAD]<4))                   // north (cos would pass
        NextPose[HEAD]=1;                        // through south first)
}




void CCreatureDisplayer::GetSmoothPoseDescriptors
	(int   RoughPoseDescriptors[MAX_ROUGH_POSE_COUNT][POSE_DESCRIPTOR_SIZE],
	 int   RoughPoseCount,
	 int   SmoothPoseDescriptors[MAX_SMOOTH_POSE_COUNT][POSE_DESCRIPTOR_SIZE],
	 int & SmoothPoseCount)
{
	int  i, j;
	int  pose;
	int  BodyPartNumber;
	int  CurrentPoseDescriptor[POSE_DESCRIPTOR_SIZE];
	int  TempPoseDescriptor[POSE_DESCRIPTOR_SIZE];
	int  TargetPoseDescriptor[POSE_DESCRIPTOR_SIZE];
	BOOL TargetPoseReached;

	// Add the first pose descriptor onto the end
	for (i=0; i<POSE_DESCRIPTOR_SIZE; i++) {
		RoughPoseDescriptors[RoughPoseCount][i] = RoughPoseDescriptors[0][i];
	}


	for (i=0; i<=RoughPoseCount; i++) {
		// Deal with direction
		if (RoughPoseDescriptors[i][DIRECTION] == POSE_TO_IT)
			RoughPoseDescriptors[i][DIRECTION] = DIRECTION_RIGHT;
		else if (RoughPoseDescriptors[i][DIRECTION] == POSE_FROM_IT)
			RoughPoseDescriptors[i][DIRECTION] = DIRECTION_LEFT;
		// Deal with head
		if (RoughPoseDescriptors[i][HEAD] == POSE_TO_IT)
			RoughPoseDescriptors[i][HEAD] = POSE_UP;

		for (j=0; j<POSE_DESCRIPTOR_SIZE; j++) {
			pose = RoughPoseDescriptors[i][j];
			if ((pose & POSE_NO_CHANGE) == POSE_NO_CHANGE) {
				// no change flag is set
				if (i == 0)
					RoughPoseDescriptors[i][j] = (pose & 0x7);
				else
					RoughPoseDescriptors[i][j] = RoughPoseDescriptors[i-1][j];
			}
		}
	}



	SmoothPoseCount = 0;

	for (i=0; i<RoughPoseCount; i++) {
		for (BodyPartNumber=0; BodyPartNumber<POSE_DESCRIPTOR_SIZE; BodyPartNumber++) {
			// Initialize the current pose
			CurrentPoseDescriptor[BodyPartNumber] = 
				RoughPoseDescriptors[i][BodyPartNumber];
			// Initialize the target pose
			TargetPoseDescriptor[BodyPartNumber] = 
				RoughPoseDescriptors[i+1][BodyPartNumber];
			// Add the first pose to the list of smooth poses
			SmoothPoseDescriptors[SmoothPoseCount][BodyPartNumber] = 
				CurrentPoseDescriptor[BodyPartNumber];
		}
		// Smooth pose added
		SmoothPoseCount++;
		do {
			UpdatePose(CurrentPoseDescriptor, TargetPoseDescriptor, TempPoseDescriptor);
			// Compare the temp pose with the target pose
			TargetPoseReached = TRUE;
			for (BodyPartNumber=0; BodyPartNumber<POSE_DESCRIPTOR_SIZE; BodyPartNumber++) {
				if (TempPoseDescriptor[BodyPartNumber] != 
					TargetPoseDescriptor[BodyPartNumber])
					TargetPoseReached = FALSE;
			}

			if (!TargetPoseReached) {
				// Add the temp pose to the list of smooth poses and update the
				// current pose
				for (BodyPartNumber=0; BodyPartNumber<POSE_DESCRIPTOR_SIZE; BodyPartNumber++) {
					SmoothPoseDescriptors[SmoothPoseCount][BodyPartNumber] = 
						TempPoseDescriptor[BodyPartNumber];
					CurrentPoseDescriptor[BodyPartNumber] = 
						TempPoseDescriptor[BodyPartNumber];
				}
				// Smooth pose added
				SmoothPoseCount++;
			}
		} while (!TargetPoseReached);
	}
}



void CCreatureDisplayer::GetPoseInformation
	(int      PoseDescriptor[POSE_DESCRIPTOR_SIZE],
	 double   ScaleFactor,
	 int      PoseImageNumbers[POSE_DESCRIPTOR_SIZE],
	 CPoint   BodyPartVectors[POSE_DESCRIPTOR_SIZE],
	 int      ZOrder[POSE_DESCRIPTOR_SIZE],
	 int    & PoseWidth,
	 int    & PoseHeight,
	 CPoint & LeftToe,
	 CPoint & RightToe,
	 CPoint & PoseOffset)
{	
	int    Direction;
	int    BodyPartNumber;
	int    ImageNumber;
	CPoint TopLeft;
	CPoint BottomRight;
	CPoint PoseTopLeft;
	CPoint PoseBottomRight;
	CPoint BodyPartVector;

	// Get an image number for each body part
	GetImageNumbers(PoseDescriptor, PoseImageNumbers); 
	// Get a shifting vector for each body part
	GetBodyPartVectors(PoseImageNumbers, BodyPartVectors);	
	// Get the Z-Order for each body part
	Direction = PoseDescriptor[DIRECTION];
	GetZOrder(Direction, ZOrder);

	// Get the MBR of the pose as a whole
	PoseTopLeft.x = UNREAL_MBR_VALUE;
	PoseTopLeft.y = UNREAL_MBR_VALUE;
	PoseBottomRight.x = -UNREAL_MBR_VALUE;
	PoseBottomRight.y = -UNREAL_MBR_VALUE;
	for (BodyPartNumber=HEAD; BodyPartNumber<POSE_DESCRIPTOR_SIZE; BodyPartNumber++) {
		ImageNumber = PoseImageNumbers[BodyPartNumber];
		BodyPartVector = BodyPartVectors[BodyPartNumber]; 

		TopLeft = m_BodyPartData[BodyPartNumber].ImagePixelData[ImageNumber].MBR.TopLeft() + BodyPartVector;
		BottomRight = m_BodyPartData[BodyPartNumber].ImagePixelData[ImageNumber].MBR.BottomRight() + BodyPartVector;

		if (TopLeft.x < PoseTopLeft.x)
			PoseTopLeft.x = TopLeft.x;
		if (TopLeft.y < PoseTopLeft.y)
			PoseTopLeft.y = TopLeft.y;
		if (BottomRight.x > PoseBottomRight.x)
			PoseBottomRight.x = BottomRight.x;
		if (BottomRight.y > PoseBottomRight.y)
			PoseBottomRight.y = BottomRight.y;		
	}

	PoseWidth = PoseBottomRight.x - PoseTopLeft.x + 1;
	PoseHeight = PoseBottomRight.y - PoseTopLeft.y + 1;
	PoseWidth = (int) ((double)PoseWidth * ScaleFactor);
	PoseHeight = (int) ((double)PoseHeight * ScaleFactor);	

	// Get coordinates of left foot 
	ImageNumber = PoseImageNumbers[LEFT_FOOT];
	LeftToe = m_BodyPartData[LEFT_FOOT].ImageAttachmentData[ImageNumber].AttachmentData[1];
	LeftToe += BodyPartVectors[LEFT_FOOT];
	LeftToe -= PoseTopLeft;
	LeftToe.x = (int)( (double)LeftToe.x * ScaleFactor);
	LeftToe.y = (int)( (double)LeftToe.y * ScaleFactor);

	// Get coordinates of right foot
	ImageNumber = PoseImageNumbers[RIGHT_FOOT];
	RightToe = m_BodyPartData[RIGHT_FOOT].ImageAttachmentData[ImageNumber].AttachmentData[1];
	RightToe += BodyPartVectors[RIGHT_FOOT];
	RightToe -= PoseTopLeft;
	RightToe.x = (int)( (double)RightToe.x * ScaleFactor);
	RightToe.y = (int)( (double)RightToe.y * ScaleFactor);

	// Return the location of the first non-transparent pixel
	PoseOffset = PoseTopLeft;
}




void CCreatureDisplayer::DrawPoseIntoBitmap
	(int       PoseImageNumbers[POSE_DESCRIPTOR_SIZE],
	 CPoint    BodyPartVectors[NUM_BODY_PARTS],
	 int       ZOrder[NUM_BODY_PARTS],
	 double    ScaleFactor,
	 BOOL      DrawFloor,
	 CPoint    PoseOffset,
	 CPoint    PoseAnchorPoint,
 	 int       BitmapWidth, 
	 int       BitmapHeight, 
	 CPoint    BitmapAnchorPoint,
	 CBitmap * Bitmap,
	 CAnimateData *pAD)
{					
	CDC       TempDC;
	CBitmap * OldBitmap;
	CPoint    BitmapOffset;
	CPoint    BodyPartVector;
	CPoint    TempPoint;
	int       ZPosition;
	int       BodyPartNumber;
	int       ImageNumber;
	int       BodyPartWidth;
	int       BodyPartHeight;
	int       Row;
	int       Column;
	COLORREF  Color;

	// Create a temporary memory device context that is compatible with the
	// system display
	TempDC.CreateCompatibleDC(NULL);
	// Select the given bitmap into the device context where we can drawn onto it
	OldBitmap = TempDC.SelectObject(Bitmap);
	// Whitewash the bitmap
//	TempDC.FillSolidRect(0, 0, BitmapWidth, BitmapHeight, RGB(255,255,255));
	// Blackout the bitmap
	TempDC.FillSolidRect(0, 0, BitmapWidth, BitmapHeight, RGB(0,0,0));

	if (DrawFloor) {
		CPen   Pen;
		CPen * OldPen;
		Pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		OldPen = TempDC.SelectObject(&Pen);
		TempDC.MoveTo(pAD->m_StartPosition);
		TempDC.LineTo(pAD->m_EndPosition);
		TempDC.SelectObject(OldPen);
	}

	BitmapOffset = BitmapAnchorPoint - PoseAnchorPoint;  
	for (ZPosition=0; ZPosition<NUM_BODY_PARTS-1; ZPosition++) {
		BodyPartNumber = ZOrder[ZPosition];
		if( BodyPartNumber == LEFT_EAR || BodyPartNumber == RIGHT_EAR || BodyPartNumber == HAIR )
			ImageNumber = PoseImageNumbers[HEAD];
		else
			ImageNumber = PoseImageNumbers[BodyPartNumber];
		if( m_BodyPartData[BodyPartNumber].ImagePixelData[ImageNumber].PixelData )
		{
			if( BodyPartNumber == LEFT_EAR || BodyPartNumber == RIGHT_EAR || BodyPartNumber == HAIR )
				ImageNumber = PoseImageNumbers[HEAD];
			else
				ImageNumber = PoseImageNumbers[BodyPartNumber];
			BodyPartVector = BodyPartVectors[BodyPartNumber]; 
			BodyPartWidth = m_BodyPartData[BodyPartNumber].ImagePixelData[ImageNumber].Width;
			BodyPartHeight = m_BodyPartData[BodyPartNumber].ImagePixelData[ImageNumber].Height;
			if( ScaleFactor > 0.99 && ScaleFactor < 1.01 )
			{
				COLORREF *pColor = m_BodyPartData[BodyPartNumber].ImagePixelData[ImageNumber].PixelData;
				for (Row=0; Row<BodyPartHeight; Row++) {
					TempPoint.x = 0;
					TempPoint.y = Row;
					TempPoint -= PoseOffset;
					TempPoint += BodyPartVector;
					TempPoint += BitmapOffset;
					for (Column=0; Column<BodyPartWidth; Column++) {
						if (*pColor != 0xFFFFFFFF) {
							// Only display non-transparent pixels. Note that PoseOffset
							// represents the location of the first non-transparent pixel
							TempDC.SetPixel(TempPoint.x, TempPoint.y, *pColor);
						}
						++TempPoint.x;
						++pColor;
					}
				}
			}
			else
			{
				for (Row=0; Row<BodyPartHeight; Row++) {
					for (Column=0; Column<BodyPartWidth; Column++) {
						Color = m_BodyPartData[BodyPartNumber].ImagePixelData[ImageNumber].PixelData[Row*BodyPartWidth + Column];
						if (Color != 0xFFFFFFFF) {
							// Only display non-transparent pixels. Note that PoseOffset
							// represents the location of the first non-transparent pixel
							TempPoint.x = Column;
							TempPoint.y = Row;
							TempPoint -= PoseOffset;
							TempPoint += BodyPartVector;
							TempPoint.x = (int) ((double)TempPoint.x * ScaleFactor);
							TempPoint.y = (int) ((double)TempPoint.y * ScaleFactor);
							TempPoint += BitmapOffset;
							// Draw a square with sides of ScaleFactor pixels
							if (ScaleFactor < 1.0)
								TempDC.SetPixel(TempPoint.x, TempPoint.y, Color);
							else
								TempDC.FillSolidRect(TempPoint.x, TempPoint.y,
									int(ScaleFactor+1), int(ScaleFactor+1), Color);
						}
						if (ScaleFactor < 1.0) 
							Column++;
					}
					if (ScaleFactor < 1.0) 
							Row++;
				}
			}
		}
	}
	// Release the drawn-onto bitmap from the temporary device context
	TempDC.SelectObject(OldBitmap);
}


void CCreatureDisplayer::GetFloorDefintion
	(double   Width, 
	 double   Height, 
	 double   Angle, 
	 CPoint & StartPosition, 
	 CPoint & EndPosition,
	 CPoint & MidPoint)
{
	double x, y;
	double HalfHeight = Height/2.0;
	double HalfWidth = Width/2.0;
	int LargeY;
	int Diff;
	int MaxY;

	// Convert the angle from degrees to radians
	Angle = Angle / 180.0 * 3.141593;
	
	y = tan(Angle)*HalfWidth;

	if (fabs(y) <= HalfHeight) {
		StartPosition.x = 0;
		StartPosition.y = (int)(HalfHeight + y);
		EndPosition.x = (int)Width;
		EndPosition.y = (int)(HalfHeight - y);

		// Move the ground closer to the bottom
		MaxY = (int)Height - 5;
		LargeY = max(StartPosition.y, EndPosition.y);
		Diff = MaxY - LargeY;
		StartPosition.y += Diff;
		EndPosition.y += Diff;
	}
	else {
		x = HalfHeight / tan(Angle);
		if (Angle > 0) {
			StartPosition.x = (int)(HalfWidth - x);
			EndPosition.x = (int)(HalfWidth + x);
			StartPosition.y = (int)Height;
			EndPosition.y = 0;
		}
		else {
			StartPosition.x = (int)(HalfWidth + x);
			EndPosition.x = (int)(HalfWidth - x);
			StartPosition.y = 0;
			EndPosition.y = (int)Height;
		}	
	}

	MidPoint.x = StartPosition.x + (EndPosition.x - StartPosition.x)/2;
	MidPoint.y = StartPosition.y + (EndPosition.y - StartPosition.y)/2;
}


//
// Public Member Functions
//


// Constructor
CCreatureDisplayer::CCreatureDisplayer()
{
	int BodyPartNumber;
	int ImageNumber;

	// Set all pixel data pointers to be NULL
	for (BodyPartNumber=HEAD; BodyPartNumber<NUM_BODY_PARTS; BodyPartNumber++) {
		for (ImageNumber=0; ImageNumber<IMAGE_COUNT; ImageNumber++) {
			m_BodyPartData[BodyPartNumber].ImagePixelData[ImageNumber].PixelData = NULL;
		}
	}

	m_Initialized = FALSE;
}


// Destructor
CCreatureDisplayer::~CCreatureDisplayer() 
{
	// Destroy all pixel data heap memory
	Cleanup();
}


int CCreatureDisplayer::Initialize
	(CString SpriteDirectory, 
	 CString AttachmentDirectory,
	 int iHeadGenus,
	 int iBodyGenus,
	 int iLegGenus,
	 int iArmGenus,
	 int iTailGenus,
	 int iEarGenus,
	 int iHairGenus,
	 int iSex,
	 int iAge,
	 int iHeadVariant, 
	 int iBodyVariant, 
	 int iLegVariant, 
	 int iArmVariant,
	 int iTailVariant,
	 int iEarVariant,
	 int iHairVariant,
	 CString & Problem)
{
	BOOL	GotData;

	m_Initialized = FALSE;

	// Destroy all pixel data heap memory and reset all pixel data pointers to
	// be NULL
	Cleanup();

// Head
	GotData = GetDataForBodyPart(HEAD, iHeadGenus, iSex, iAge, iHeadVariant, 
		SpriteDirectory, AttachmentDirectory, m_BodyPartData[HEAD], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}

// Body
	GotData = GetDataForBodyPart(BODY, iBodyGenus, iSex, iAge, iBodyVariant, 
		SpriteDirectory, AttachmentDirectory, m_BodyPartData[BODY], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}

// Legs
	GotData = GetDataForBodyPart(LEFT_THIGH, iLegGenus, iSex, iAge, iLegVariant, 
		SpriteDirectory, AttachmentDirectory, m_BodyPartData[LEFT_THIGH], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}
	GotData = GetDataForBodyPart(LEFT_SHIN, iLegGenus, iSex, iAge, iLegVariant, 
		SpriteDirectory, AttachmentDirectory, m_BodyPartData[LEFT_SHIN], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}
	GotData = GetDataForBodyPart(LEFT_FOOT, iLegGenus, iSex, iAge, iLegVariant, 
		SpriteDirectory, AttachmentDirectory, m_BodyPartData[LEFT_FOOT], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}
	GotData = GetDataForBodyPart(RIGHT_THIGH, iLegGenus, iSex, iAge, iLegVariant, 
		SpriteDirectory, AttachmentDirectory, m_BodyPartData[RIGHT_THIGH], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}
	GotData = GetDataForBodyPart(RIGHT_SHIN, iLegGenus, iSex, iAge, iLegVariant, 
		SpriteDirectory, AttachmentDirectory, m_BodyPartData[RIGHT_SHIN], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}
	GotData = GetDataForBodyPart(RIGHT_FOOT, iLegGenus, iSex, iAge, iLegVariant, 
		SpriteDirectory, AttachmentDirectory, m_BodyPartData[RIGHT_FOOT], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}
	
// Arms
	GotData = GetDataForBodyPart(LEFT_HUMERUS, iArmGenus, iSex, iAge, iArmVariant, 
		SpriteDirectory, AttachmentDirectory, m_BodyPartData[LEFT_HUMERUS], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}
	GotData = GetDataForBodyPart(LEFT_RADIUS, iArmGenus, iSex, iAge, iArmVariant, 
		SpriteDirectory, AttachmentDirectory, m_BodyPartData[LEFT_RADIUS], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}
	GotData = GetDataForBodyPart(RIGHT_HUMERUS, iArmGenus, iSex, iAge, iArmVariant, 
		SpriteDirectory, AttachmentDirectory, m_BodyPartData[RIGHT_HUMERUS], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}
	GotData = GetDataForBodyPart(RIGHT_RADIUS, iArmGenus, iSex, iAge, iArmVariant, 
		SpriteDirectory, AttachmentDirectory, m_BodyPartData[RIGHT_RADIUS], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}

// Tail
	GotData = GetDataForBodyPart(TAIL_ROOT, iTailGenus, iSex, iAge, iTailVariant, 
		SpriteDirectory, AttachmentDirectory, m_BodyPartData[TAIL_ROOT], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}
	GotData = GetDataForBodyPart(TAIL_TIP, iTailGenus, iSex, iAge, iTailVariant, 
	SpriteDirectory, AttachmentDirectory, m_BodyPartData[TAIL_TIP], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}

// Ears
	GotData = GetDataForBodyPart(LEFT_EAR, iEarGenus, iSex, iAge, iEarVariant, 
	SpriteDirectory, AttachmentDirectory, m_BodyPartData[LEFT_EAR], Problem);
	if (!GotData) {
//		Cleanup();
		m_Initialized = TRUE;
		return 0;
	}
	GotData = GetDataForBodyPart(RIGHT_EAR, iEarGenus, iSex, iAge, iEarVariant, 
	SpriteDirectory, AttachmentDirectory, m_BodyPartData[RIGHT_EAR], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}
// Hair
	GotData = GetDataForBodyPart(HAIR, iHairGenus, iSex, iAge, iHairVariant, 
	SpriteDirectory, AttachmentDirectory, m_BodyPartData[HAIR], Problem);
	if (!GotData) {
		Cleanup();
		return FILE_PROBLEM;
	}
	m_Initialized = TRUE;

	// OK
	return 0;
}


void CCreatureDisplayer::DisplayFloor
	(CStatic * StaticControl, 
	 CBitmap * Bitmap, 
	 int       Angle)
{
	int       Width;
	int       Height;
	CRect     Rect;
	CDC       TempDC;	
	CBitmap * OldBitmap;
	CPen      Pen;
	CPen *    OldPen;
	CPoint    StartPosition;
	CPoint    EndPosition;
	CPoint    MidPoint;

	if (!m_Initialized) 
		return;

	// Get the width and height of the static control/bitmap
	StaticControl->GetClientRect(&Rect);
	Width = Rect.Width();
	Height = Rect.Height();

	Pen.CreatePen(PS_SOLID, 2, RGB(255,0,0));
	TempDC.CreateCompatibleDC(NULL);
	OldPen = TempDC.SelectObject(&Pen);
	OldBitmap = TempDC.SelectObject(Bitmap);
	TempDC.PatBlt(0, 0, Width, Height, WHITENESS);
	// Get start and end points
	GetFloorDefintion(Width, Height, Angle, StartPosition, EndPosition,
		MidPoint);
	TempDC.MoveTo(StartPosition);
	TempDC.LineTo(EndPosition);
	TempDC.SelectObject(OldBitmap);
	TempDC.SelectObject(OldPen);
	StaticControl->SetBitmap(*Bitmap);	
}


void CCreatureDisplayer::DisplayPose
	(CRect const &Rect, //CStatic * StaticControl, 
	 CBitmap * Bitmap,
	 int       VirtualPoseDescriptor[POSE_DESCRIPTOR_SIZE],
	 double    ScaleFactor,
	 CAnimateData *pAD)
{
	int    PoseImageNumbers[POSE_DESCRIPTOR_SIZE];
	CPoint BodyPartVectors[NUM_BODY_PARTS];
	int    ZOrder[NUM_BODY_PARTS];
	CPoint PoseOffset;
	CPoint LeftToe;
	CPoint RightToe;
	CPoint PoseAnchorPoint;
	CPoint BitmapAnchorPoint;
//	CRect  Rect;
	int    BitmapWidth;
	int    BitmapHeight;
	int    PoseWidth;
	int    PoseHeight;
	CDC    TempDC;
	int    PoseDescriptor[POSE_DESCRIPTOR_SIZE];
	
	if (!m_Initialized) 
		return;

	// Get the width and height of the static control/bitmap

//	StaticControl->GetClientRect(&Rect);
	BitmapWidth = Rect.Width();
	BitmapHeight = Rect.Height();


	ConvertPoseDescriptor(VirtualPoseDescriptor, PoseDescriptor);

	GetPoseInformation(PoseDescriptor, ScaleFactor, PoseImageNumbers, 
		BodyPartVectors, ZOrder, PoseWidth, PoseHeight, LeftToe, 
		RightToe, PoseOffset);

	if (LeftToe.y > RightToe.y)
		PoseAnchorPoint = LeftToe;
	else
		PoseAnchorPoint = RightToe;

	BitmapAnchorPoint.x = (BitmapWidth - PoseWidth)/2 + PoseAnchorPoint.x;;
	BitmapAnchorPoint.y = (BitmapHeight - PoseHeight)/2 + PoseAnchorPoint.y;;

	DrawPoseIntoBitmap(PoseImageNumbers, BodyPartVectors, ZOrder, ScaleFactor, 
		FALSE, PoseOffset, PoseAnchorPoint, BitmapWidth, BitmapHeight, 
		BitmapAnchorPoint, Bitmap, pAD);

//	StaticControl->SetBitmap(*Bitmap);
}



void CCreatureDisplayer::HighlightPose
	(CStatic * StaticControl, 
	 CBitmap * Bitmap,
	 COLORREF Color,
	 int Thickness)
{
	int    BitmapWidth;
	int    BitmapHeight;
	CRect Rect;
	CDC memDC;
	CPen Pen;
	CPen * OldPen;
	Pen.CreatePen(PS_SOLID, Thickness, Color);
	
	if (!m_Initialized) 
		return;

	// Get the width and height of the static control/bitmap
	StaticControl->GetClientRect(&Rect);
	BitmapWidth = Rect.Width();
	BitmapHeight = Rect.Height();
	
	memDC.CreateCompatibleDC(NULL);
	OldPen = memDC.SelectObject(&Pen);
	CBitmap *old = memDC.SelectObject(Bitmap);
	memDC.MoveTo(1, 1);
	memDC.LineTo(BitmapWidth-1, 1);
	memDC.LineTo(BitmapWidth-1, BitmapHeight-1);
	memDC.LineTo(1, BitmapHeight-1);
	memDC.LineTo(1, 1);
	memDC.SelectObject(old);
	memDC.SelectObject(OldPen);
	StaticControl->SetBitmap(*Bitmap);
}



void CCreatureDisplayer::DisplayBackgroundOnly
	(CStatic * StaticControl, 
	 CBitmap * Bitmap,
	 COLORREF Color)
{
	int    BitmapWidth;
	int    BitmapHeight;
	CRect Rect;
	CDC memDC;
	
	// Get the width and height of the static control/bitmap
	StaticControl->GetClientRect(&Rect);
	BitmapWidth = Rect.Width();
	BitmapHeight = Rect.Height();
	
	memDC.CreateCompatibleDC(NULL);
	CBitmap *old = memDC.SelectObject(Bitmap);
	
	memDC.FillSolidRect(0, 0, BitmapWidth, BitmapHeight,
		Color);
	memDC.SelectObject(old);
	StaticControl->SetBitmap(*Bitmap);
}



int CCreatureDisplayer::PrepareWalk
	(CRect const &Rect,
	 int       RoughPoseDescriptors[MAX_ROUGH_POSE_COUNT][POSE_DESCRIPTOR_SIZE],
	 int       RoughPoseCount,
	 double    ScaleFactor,
	 int       Angle,
	 CAnimateData *pAD)
{
	CPoint BodyPartVector;
	CPoint LeftToe;
	CPoint RightToe;
	int    PoseImageNumbers[POSE_DESCRIPTOR_SIZE];
	CPoint BodyPartVectors[NUM_BODY_PARTS];
	int    ZOrder[NUM_BODY_PARTS];
	CPoint PoseOffset;
	int    PoseWidth;
	int    PoseHeight;
	CPoint MidPoint;

	if (!m_Initialized) 
		return 0;


	pAD->m_ScaleFactor = ScaleFactor;
	// Get start and end points
	GetFloorDefintion(Rect.Width(), Rect.Height(), Angle, pAD->m_StartPosition, 
		pAD->m_EndPosition, MidPoint);

	pAD->m_DownToeAnchorPoint = MidPoint;

	pAD->m_SmoothPoseNumber = 0;


	GetSmoothPoseDescriptors(RoughPoseDescriptors, RoughPoseCount,
		pAD->m_SmoothPoseDescriptors, pAD->m_SmoothPoseCount);


	GetPoseInformation(pAD->m_SmoothPoseDescriptors[0], ScaleFactor, 
		PoseImageNumbers, BodyPartVectors, ZOrder, PoseWidth, PoseHeight, 
		LeftToe, RightToe, PoseOffset);
	if (LeftToe.y > RightToe.y) {
		pAD->m_PreviousDownToe = TOE_LEFT;
		BodyPartVector = pAD->m_DownToeAnchorPoint - LeftToe;
		pAD->m_PreviousUpToePosition = RightToe + BodyPartVector;
	}
	else {
		pAD->m_PreviousDownToe = TOE_RIGHT;
		BodyPartVector = pAD->m_DownToeAnchorPoint - RightToe;
		pAD->m_PreviousUpToePosition = LeftToe + BodyPartVector;
	}
	return pAD->m_SmoothPoseCount;
}


int CCreatureDisplayer::UpdateWalk(CRect const &Rect, CBitmap *Bitmap, CAnimateData *pAD)
{
	CPoint LeftToe;
	CPoint RightToe;
	CPoint DownToe;
	CPoint UpToe;
	CPoint CurrentUpToePosition;
	int    FloorHeight;
	CPoint PoseAnchorPoint;
	BOOL   IsFalling;
	double Gradient;
	int    PoseImageNumbers[POSE_DESCRIPTOR_SIZE];
	CPoint BodyPartVectors[NUM_BODY_PARTS];
	CPoint BodyPartVector;
	int    ZOrder[NUM_BODY_PARTS];
	CPoint PoseOffset;
	int    PoseWidth;
	int    PoseHeight;
	int    Temp;

	if (!m_Initialized) 
		return 0;

	Temp = pAD->m_SmoothPoseNumber;
	GetPoseInformation(pAD->m_SmoothPoseDescriptors[pAD->m_SmoothPoseNumber], 
		pAD->m_ScaleFactor, PoseImageNumbers, BodyPartVectors, ZOrder, 
		PoseWidth, PoseHeight, LeftToe, 
		RightToe, PoseOffset);

	if (pAD->m_PreviousDownToe == TOE_LEFT) {
		UpToe = RightToe;
		DownToe = LeftToe;
	}
	else {
		UpToe = LeftToe;
		DownToe = RightToe;
	}
	BodyPartVector = pAD->m_DownToeAnchorPoint - DownToe;
	CurrentUpToePosition = UpToe + BodyPartVector;

	Gradient = ((double)(pAD->m_EndPosition.y - pAD->m_StartPosition.y)/
				(double)(pAD->m_EndPosition.x - pAD->m_StartPosition.x));
	
	FloorHeight = (int)((double)pAD->m_StartPosition.y + 
				(double)(CurrentUpToePosition.x - pAD->m_StartPosition.x)*Gradient);

	IsFalling = CurrentUpToePosition.y > pAD->m_PreviousUpToePosition.y;

	if ((CurrentUpToePosition.y > FloorHeight) && (IsFalling)) {
		// the current up toe is now below the floor
		pAD->m_PreviousUpToePosition = pAD->m_DownToeAnchorPoint;
		pAD->m_PreviousUpToePosition.y += FloorHeight - CurrentUpToePosition.y;
		pAD->m_DownToeAnchorPoint.x = CurrentUpToePosition.x;
		pAD->m_DownToeAnchorPoint.y = FloorHeight;
		PoseAnchorPoint = UpToe;
		// Swap the down toes
		if (pAD->m_PreviousDownToe == TOE_LEFT)
			pAD->m_PreviousDownToe = TOE_RIGHT;
		else
			pAD->m_PreviousDownToe = TOE_LEFT;
	}
	else {
		pAD->m_PreviousUpToePosition = CurrentUpToePosition;
		PoseAnchorPoint = DownToe;
	}

	// Wrap around (assumes start x < end x)
	if (pAD->m_DownToeAnchorPoint.x < pAD->m_StartPosition.x) 
		pAD->m_DownToeAnchorPoint = pAD->m_EndPosition;
	if (pAD->m_DownToeAnchorPoint.x > pAD->m_EndPosition.x)
		pAD->m_DownToeAnchorPoint = pAD->m_StartPosition;

	DrawPoseIntoBitmap(PoseImageNumbers, BodyPartVectors, ZOrder, pAD->m_ScaleFactor, 
		TRUE, PoseOffset, PoseAnchorPoint, Rect.Width(), Rect.Height(), 
		pAD->m_DownToeAnchorPoint, Bitmap, pAD);

	pAD->m_SmoothPoseNumber++;
	if (pAD->m_SmoothPoseNumber == pAD->m_SmoothPoseCount) {
		pAD->m_SmoothPoseNumber = 0;
	}
	return Temp;
}



int CCreatureDisplayer::PrepareAnimate
	(int       RoughPoseDescriptors[MAX_ROUGH_POSE_COUNT][POSE_DESCRIPTOR_SIZE],
	 int       RoughPoseCount,
	 double    ScaleFactor,
		 CAnimateData *pAD)
{
	CPoint BodyPartVector;
	CPoint LeftToe;
	CPoint RightToe;

	pAD->m_ScaleFactor = ScaleFactor;
	pAD->m_SmoothPoseNumber = 0;

	if (!m_Initialized) 
		return 0;

	GetSmoothPoseDescriptors(RoughPoseDescriptors, RoughPoseCount,
		pAD->m_SmoothPoseDescriptors, pAD->m_SmoothPoseCount);

	// Get start and end points

	return pAD->m_SmoothPoseCount;
}


int CCreatureDisplayer::UpdateAnimate
	(CRect const &Rect,
	 CBitmap * Bitmap,
	 int       Angle,
	 CAnimateData *pAD)
{
	CPoint LeftToe;
	CPoint RightToe;
	CPoint PoseAnchorPoint;
	int    PoseImageNumbers[POSE_DESCRIPTOR_SIZE];
	CPoint BodyPartVectors[NUM_BODY_PARTS];
	int    ZOrder[NUM_BODY_PARTS];
	CPoint PoseOffset;
	int    PoseWidth;
	int    PoseHeight;
	int    Temp;
	CPoint MidPoint;

	if (!m_Initialized) 
		return 0;
	 
	GetFloorDefintion(Rect.Width(), Rect.Height(), Angle, pAD->m_StartPosition, 
		pAD->m_EndPosition, MidPoint);

	
	Temp = pAD->m_SmoothPoseNumber;
	GetPoseInformation(pAD->m_SmoothPoseDescriptors[pAD->m_SmoothPoseNumber], pAD->m_ScaleFactor, 
		PoseImageNumbers, BodyPartVectors, ZOrder, PoseWidth, 
		PoseHeight, LeftToe, RightToe, PoseOffset);

	if (LeftToe.y > RightToe.y)
		PoseAnchorPoint = LeftToe;
	else
		PoseAnchorPoint = RightToe;

	DrawPoseIntoBitmap(PoseImageNumbers, BodyPartVectors, ZOrder, 
		pAD->m_ScaleFactor, TRUE, PoseOffset, PoseAnchorPoint, 
		Rect.Width(), Rect.Height(), MidPoint, Bitmap, pAD);

	pAD->m_SmoothPoseNumber++;
	if (pAD->m_SmoothPoseNumber == pAD->m_SmoothPoseCount) {
		pAD->m_SmoothPoseNumber = 0;
	}

	return Temp;
}

