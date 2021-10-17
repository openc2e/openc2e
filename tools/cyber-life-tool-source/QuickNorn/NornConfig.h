#ifndef NORN_CONFIG_H
#define NORN_CONFIG_H

enum COLOUR
{
	RED = 0,
	GREEN,
	BLUE,
	YELLOW,
	MAGENTA,
	CYAN,
	ANY,
	PADDING,

	COLOUR_MAX
};

class Colour
{
public: 
	unsigned char R, G, B;
	Colour(unsigned char r, unsigned char g, unsigned char b) : R(r), G(g), B(b) {};
};

extern Colour ourColours[COLOUR_MAX];
extern const char GameName[];

class CNornConfig {
public:
	static int NumParts();
	static int NumPositions();
	static int NumSets( int bodyPart );
	static int NumAPs( int bodyPart );
	static int AttachPointColour( int bodyPart, int ap_index );
	static char BodyID( int bodyPart ); 
	static CString GetFilename( int part, int position, int set );
	static const char *CNornConfig::GetPartName( int index );
	static const char *CNornConfig::GetPositionName( int index );
};

#endif

