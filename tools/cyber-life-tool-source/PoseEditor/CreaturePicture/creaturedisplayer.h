// Description: CCreatureDisplayer - Displays/animates creatures
// Author: Robert Dick
// Date: 7/12/98

#ifndef CREATURE_DISPLAYER_H
#define CREATURE_DISPLAYER_H


//
// Constants
//


#define UNREAL_MBR_VALUE		1000000

#define FILE_PROBLEM			-1
#define IMAGE_COUNT				16
#define POSE_DESCRIPTOR_SIZE	15
#define NUM_BODY_PARTS			18 // Larger then POSE_DESCRIPTOR_SIZE as ears &
								   // hair have no pose number
#define MAX_ROUGH_POSE_COUNT	9 // RJS Note I would have thought this would be 8
#define MAX_SMOOTH_POSE_COUNT	50

#define TOE_LEFT	0
#define TOE_RIGHT	1

#define CREATURE_NORN		0
#define CREATURE_GRENDEL	1
#define CREATURE_ETTIN		2

#ifdef CREATURE_DISPLAYER_CPP // Avoid clashes

#define DIRECTION		0
#define HEAD			1
#define BODY			2
#define LEFT_THIGH		3
#define LEFT_SHIN		4
#define LEFT_FOOT		5
#define RIGHT_THIGH		6
#define RIGHT_SHIN		7
#define RIGHT_FOOT		8
#define LEFT_HUMERUS	9
#define LEFT_RADIUS		10
#define RIGHT_HUMERUS	11
#define RIGHT_RADIUS	12
#define TAIL_ROOT		13
#define TAIL_TIP		14
#define LEFT_EAR		15
#define RIGHT_EAR		16
#define HAIR			17

#endif // CREATURE_DISPLAYER_CPP

#define IMAGE_RIGHT_LOWEST			0
#define IMAGE_RIGHT_LOW				1
#define IMAGE_RIGHT_HORIZONTAL		2
#define IMAGE_RIGHT_HIGH			3
#define IMAGE_LEFT_LOWEST			4
#define IMAGE_LEFT_LOW				5
#define IMAGE_LEFT_HORIZONTAL		6
#define IMAGE_LEFT_HIGH				7
#define IMAGE_OUTWARDS_LOWEST		8
#define IMAGE_OUTWARDS_LOW			9
#define IMAGE_OUTWARDS_HORIZONTAL	10
#define IMAGE_OUTWARDS_HIGH			11
#define IMAGE_INWARDS_LOWEST		12
#define IMAGE_INWARDS_LOW			13
#define IMAGE_INWARDS_HORIZONTAL	14
#define IMAGE_INWARDS_HIGH			15

#define DIRECTION_INWARDS	0
#define DIRECTION_OUTWARDS	1
#define DIRECTION_RIGHT		2
#define DIRECTION_LEFT		3


#define POSE_LOWEST						0
#define POSE_LOW						1
#define POSE_HORIZONTAL					2
#define POSE_UP							3
#define POSE_OUTWARDS					4
#define POSE_INWARDS					5


#define POSE_TO_IT						6
#define POSE_FROM_IT                    7
#define POSE_NO_CHANGE                  8

class CAnimateData
{
public:
	int				m_SmoothPoseDescriptors[MAX_SMOOTH_POSE_COUNT][POSE_DESCRIPTOR_SIZE];
	int				m_SmoothPoseCount;
	int				m_SmoothPoseNumber;
	CPoint          m_StartPosition;
	CPoint          m_EndPosition;
	CPoint          m_DownToeAnchorPoint;
	int             m_PreviousDownToe;
	CPoint			m_PreviousUpToePosition;
	double          m_ScaleFactor;
	CPoint          m_MidPoint;
};


class CCreatureDisplayer {

private:

	// Private Structures

	struct ImagePixelData_t {
		WORD Width;
		WORD Height;
		CRect MBR;
		COLORREF *PixelData;
	};

	struct ImageAttachmentData_t {
		CPoint AttachmentData[6];
	};

	struct BodyPartData_t {
		ImagePixelData_t ImagePixelData[IMAGE_COUNT];
		ImageAttachmentData_t ImageAttachmentData[IMAGE_COUNT]; 
	};


	// Private Data

//	BodyPartData_t	m_BodyPartData[POSE_DESCRIPTOR_SIZE];
	BodyPartData_t	m_BodyPartData[NUM_BODY_PARTS];
	BOOL			m_Initialized;



	// Private Member Functions

	void Cleanup(void);

	BOOL OpenMatchingFile
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
		 CFile & theFile);

	void GetCompressedPixelDataForImage
		(BYTE *				SpriteFileBuffer, 
		 ImagePixelData_t & ImagePixelData, 
		 int				ImageNumber);

	void GetPixelDataForImage
		(BYTE *				SpriteFileBuffer, 
		 ImagePixelData_t & ImagePixelData, 
		 int				ImageNumber);

	void GetAttachmentDataForImage
		(char * &				 ImageAttachmentDataPointer, 
		 ImageAttachmentData_t & ImageAttachmentData, 
		 int					 NumberOfAttachmentPoints);

	BOOL GetDataForBodyPart
		(int              iBodyPart,
		 int              iGenus,
		 int              iSex,				
	  	 int              iAge,			
		 int              iVariant,
		 CString		  strSpriteDirectory,
		 CString		  strAttachmentDirectory, 
		 BodyPartData_t & BodyPartData,
		 CString  & FailReason);

	void ConvertPoseDescriptor
		(int VirtualPoseDescriptor[POSE_DESCRIPTOR_SIZE],
		int PoseDescriptor[POSE_DESCRIPTOR_SIZE]);

	void GetImageNumbers
		(int PoseDescriptor[POSE_DESCRIPTOR_SIZE],
		 int ImageNumbers[POSE_DESCRIPTOR_SIZE]);

	void GetBodyPartVectors
		(int    ImageNumbers[POSE_DESCRIPTOR_SIZE],
		 CPoint BodyPartVectors[NUM_BODY_PARTS]);

	void GetZOrder(int Direction, int ZOrder[NUM_BODY_PARTS]);

	void GetSmoothPoseDescriptors
		(int RoughPoseDescriptors[MAX_ROUGH_POSE_COUNT][POSE_DESCRIPTOR_SIZE],
		 int RoughPoseCount,
		 int SmoothPoseDescriptors[MAX_SMOOTH_POSE_COUNT][POSE_DESCRIPTOR_SIZE],
		 int & SmoothPoseCount);

	void GetPoseInformation
		(int      PoseDescriptor[POSE_DESCRIPTOR_SIZE],
		 double   ScaleFactor,
		 int      PoseImageNumbers[POSE_DESCRIPTOR_SIZE],
		 CPoint   BodyPartVectors[NUM_BODY_PARTS],
		 int      ZOrder[NUM_BODY_PARTS],
		 int    & PoseWidth,
		 int    & PoseHeight,
		 CPoint & LeftToe,
		 CPoint & RightToe,
		 CPoint & PoseOffset);

	void DrawPoseIntoBitmap
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
		 CAnimateData *pAD);

	void GetFloorDefintion
		(double   Width, 
		 double   Height, 
		 double   Angle, 
		 CPoint & StartPosition, 
		 CPoint & EndPosition,
		 CPoint & MidPoint);

	
	void UpdatePose(int CurrPose[POSE_DESCRIPTOR_SIZE], int TargetPose[POSE_DESCRIPTOR_SIZE],
									int NextPose[POSE_DESCRIPTOR_SIZE]);

public:

	// Public Member Functions

	// Constructor	
	CCreatureDisplayer();
	
	// Destructor
	~CCreatureDisplayer();

	int Initialize
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
		 CString & Problem);

	void DisplayFloor
		(CStatic * StaticControl, 
		 CBitmap * Bitmap, 
		 int       Angle);

	void DisplayPose
		(CRect const &Rect, //CStatic * StaticControl,
		 CBitmap * Bitmap,
		 int       PoseDescriptor[POSE_DESCRIPTOR_SIZE],
		 double    ScaleFactor,
		 CAnimateData *pAD);

	void HighlightPose
		(CStatic * StaticControl, 
		 CBitmap * Bitmap,
		 COLORREF Color,
		 int Thickness);

	void DisplayBackgroundOnly
		(CStatic * StaticControl, 
		 CBitmap * Bitmap,
		 COLORREF Color);

	int PrepareWalk
		(CRect const &Rect,
		 int       RoughPoseDescriptors[MAX_ROUGH_POSE_COUNT][POSE_DESCRIPTOR_SIZE],
		 int       RoughPoseCount,
		 double    ScaleFactor,
		 int       Angle,
		 CAnimateData *pAD);

	int UpdateWalk(CRect const &Rect, CBitmap *Bitmap, CAnimateData *pAD);

	int PrepareAnimate
		(int       RoughPoseDescriptors[MAX_ROUGH_POSE_COUNT][POSE_DESCRIPTOR_SIZE],
		 int       RoughPoseCount,
		 double    ScaleFactor,
		 CAnimateData *pAD);

	int UpdateAnimate
		(CRect const &Rect,
		 CBitmap * Bitmap,
		 int       Angle,
		 CAnimateData *pAD);
};

#endif // CREATURE_DISPLAYER_H

