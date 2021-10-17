#ifndef GENE_H_
#define GENE_H_

#include <vector>
#include <string>
#include <algorithm>
#include <ostream>

class CGenome;

struct CGeneDescription
{
	std::string m_Caption;
	std::string m_Description;
};

CGeneDescription GetGeneDescription( BYTE type, BYTE subType, BYTE id );
std::string GetPoseDescription( int pose );
void SetPoseDescription( int pose, std::string const &name );
void LoadPoseDescriptions( std::string filename );
void SavePoseDescriptions( std::string filename );
void ImportPoseDescriptions( std::string filename );
void ImportGaitDescriptions( std::string filename, CGenome *genome );

class CGene
{
public:
	BYTE m_Type;
	BYTE m_SubType;
	BYTE m_ID;
	BYTE m_Generation;
	BYTE m_SwitchOnTime;
	BYTE m_Flags;
	BYTE m_Mutability;
	BYTE m_ExpressionVariant;
	std::vector<BYTE> m_Data;

	bool operator<( CGene const &other ) const {
		if( m_Data[0] == other.m_Data[0] )
			return m_SwitchOnTime < other.m_SwitchOnTime;
		else
			return  m_Data[0] < other.m_Data[0];
	}
	bool SameID( CGene const &other ) const {
		return m_Type == other.m_Type &&
			m_SubType == other.m_SubType &&
			m_ID == other.m_ID;
	}
	CGeneDescription GetDescription() const { return GetGeneDescription( m_Type, m_SubType, m_ID );}
	void Dump( std::ostream &stream ) const;

	enum
	{
		BP_Head,
		BP_Body,
		BP_Legs,
		BP_Arms,
		BP_Tail
	};

	std::string GetAgeName() const
	{
		static const char * names[7] = {"Embryo","Child","Adolescent","Youth","Adult","Old","Senile"};
		return GetAgeName( m_SwitchOnTime );
	}
	static std::string GetAgeName( BYTE age )
	{
		static const char * names[7] = {"Embryo","Child","Adolescent","Youth","Adult","Old","Senile"};
		return names[ std::_MIN( BYTE(6), std::_MAX( BYTE(0), age ) ) ];
	}
	void WriteClipboard() const;
	bool ReadClipboard();
	static bool CanPaste();
};
#endif

