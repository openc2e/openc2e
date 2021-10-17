#include "stdafx.h"

#include "NornConfig.h"


// Should be configurable at run-time

//#define ADVENTURES

#ifdef ADVENTURES
	#pragma message("Note: Using Creatures adventures body parts")
	const int cNumParts = 17;
	const char GameName[] = "Creatures Adventures";
#else
	#pragma message("Note: Using Creatures 3 body parts")
	const int cNumParts = 18;
	const char GameName[] = "Creatures 3";
#endif

const int cMaxNumAPs = 6;

Colour ourColours[COLOUR_MAX] =
{
	Colour(255, 0, 0),
	Colour(0, 255, 0),
	Colour(0, 0, 255),
	Colour(255, 255, 0),
	Colour(255, 0, 255),
	Colour(0, 255, 255),
	Colour(-1, -1, -1),
	Colour(-1, -1, -1),
};

const int cNumPositions = 16;

int CNornConfig::NumParts()
{
	return cNumParts;
}

int CNornConfig::NumPositions()
{
	return cNumPositions;
}

int CNornConfig::NumSets( int bodyPart )
{
#ifdef ADVENTURES
	static const int cNumSets[cNumParts] = {
		24,
		1, 1, 1,
		1, 1, 1,
		1, 1,
		1, 1,
		4,
		4, 4,
		3, 1, 1};
#else
	static const int cNumSets[cNumParts] = {
		12,
		6,
		1, 1, 1,
		1, 1, 1,
		1, 1,
		1, 1,
		4,
		0, 0,
		0, 1, 1};
#endif

	if( bodyPart < 0 || bodyPart >= cNumParts )
		return 0;
	else
		return cNumSets[ bodyPart ];
}

int CNornConfig::NumAPs( int bodyPart )
{
#ifdef ADVENTURES
	static const int cNumAPs[cNumParts] = {
		5,
		2, 2, 2,
		2, 2, 2,
		2, 2,
		2, 2,
		6,
		2, 2,
		1, 2, 2};
#else
	static const int cNumAPs[cNumParts] = {
		1,
		5,
		2, 2, 2,
		2, 2, 2,
		2, 2,
		2, 2,
		6,
		0, 0,
		0, 2, 2};
#endif

	if( bodyPart < 0 || bodyPart >= cNumParts )
		return 0;
	else
		return cNumAPs[ bodyPart ];
}

int CNornConfig::AttachPointColour( int bodyPart, int ap_index )
{
#ifdef ADVENTURES
	#error Need to fill in!
#else
	static const int cNumAPs[cNumParts][cMaxNumAPs] = {
		{GREEN, -1, -1, -1, -1, -1},
		{GREEN, RED, PADDING, PADDING, PADDING, -1},
		{MAGENTA, GREEN, -1, -1, -1, -1}, {GREEN, RED, -1, -1, -1, -1}, {RED, GREEN, -1, -1, -1, -1},
		{YELLOW, GREEN, -1, -1, -1, -1}, {GREEN, RED, -1, -1, -1, -1}, {RED, GREEN, -1, -1, -1, -1},
		{CYAN, GREEN, -1, -1, -1, -1}, {GREEN, RED, -1, -1, -1, -1},
		{BLUE, GREEN, -1, -1, -1, -1}, {GREEN, RED, -1, -1, -1, -1},
		{GREEN, MAGENTA, YELLOW, CYAN, BLUE, RED},
		{-1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1, -1}, {RED, GREEN, -1, -1, -1, -1}, {GREEN, RED, -1, -1, -1, -1}, };
#endif

	if( bodyPart < 0 || bodyPart >= cNumParts )
		return -1;
	else
	{	
		if (ap_index < 0 || ap_index >= cMaxNumAPs)
			return -1;

		return cNumAPs[ bodyPart ][ ap_index ];
	}
}

char CNornConfig::BodyID( int bodyPart )
{
#ifdef ADVENTURES
	static const char cBodyId[]={'a', //head
					'c',// order as above
					'd',
					'e',
					'f',
					'g',
					'h',
					'i',
					'j',
					'k',
					'l',
					'b' ,//body
					'o',//ears
					'p',//ears
					'q',//hair
					'm',//tail root
					'n'};//tail tip
#else
	static const char cBodyId[]={'z', //head (unused)
					'a',// order as above
					'c',// order as above
					'd',
					'e',
					'f',
					'g',
					'h',
					'i',
					'j',
					'k',
					'l',
					'b' ,//body
					'o',//ears
					'p',//ears
					'q',//hair
					'm',//tail root
					'n'};//tail tip
#endif
	if( bodyPart < 0 || bodyPart >= cNumParts )
		return 0;
	else
		return cBodyId[ bodyPart ];
}

CString CNornConfig::GetFilename( int part, int position, int set )
{
	CString filename;
	int start = 0, p = 0;
	while( part-- ) start += CNornConfig::NumSets( p++ ) * NumPositions();
	filename.Format("CA%04d.bmp",start + set * NumPositions() + position);

	return filename;
}

const char *CNornConfig::GetPartName( int index )
{
#ifdef ADVENTURES
	static const char *partNames[] = {
		"Head",
		"Left Thigh",
		"Left Shin",
		"Left Foot",
		"Right Thigh",
		"Right Shin",
		"Right Foot",
		"Left Upper Arm",
		"Left Lower Arm",
		"Right Upper Arm",
		"Right Lower Arm",
		"Body",
		"Left Ear",
		"Right Ear",
		"Hair",
		"Tail Root",
		"Tail Tip" };
#else
	static const char *partNames[] = {
		"Head",
		"Mouth",
		"Left Thigh",
		"Left Shin",
		"Left Foot",
		"Right Thigh",
		"Right Shin",
		"Right Foot",
		"Left Upper Arm",
		"Left Lower Arm",
		"Right Upper Arm",
		"Right Lower Arm",
		"Body",
		"Left Ear",
		"Right Ear",
		"Hair",
		"Tail Root",
		"Tail Tip" };
#endif
	return partNames[index];
}

const char *CNornConfig::GetPositionName( int index )
{
	static const char *positionNames[] = {
		"Right 1",
		"Right 2",
		"Right 3",
		"Right 4",
		"Left 1",
		"Left 2",
		"Left 3",
		"Left 4",
		"Towards 1",
		"Towards 2",
		"Towards 3",
		"Towards 4",
		"Away 1",
		"Away 2",
		"Away 3",
		"Away 4" };
	return positionNames[index];
}

