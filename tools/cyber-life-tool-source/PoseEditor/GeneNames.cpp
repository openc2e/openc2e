#include "stdafx.h"
#include <fstream>
#include <string>
#include <map>

#include "Genome.h"

CGeneDescription GetGeneDescription( BYTE type, BYTE subType, BYTE id )
{
	CGeneDescription description;
	static bool loaded = false;
	static std::map< int, CGeneDescription > map;
	if( !loaded )
	{
		std::ifstream file( "c:\\projects\\GeneGenie\\gold.gno",
			std::ios_base::binary );
		file.ignore( 4 );
		while( !file.eof() )
		{
			WORD type, subType, id, size;
			CGeneDescription description;

			file.read( (char *)&type, 2 );
			file.read( (char *)&subType, 2 );
			file.read( (char *)&id, 2 );
			file.ignore( 2 );

			file.read( (char *)&size, 2 );
			description.m_Caption.reserve( size );
			while( size-- ) description.m_Caption += file.get();

			file.read( (char *)&size, 2 );
			description.m_Description.reserve( size );
			while( size-- ) description.m_Description += file.get();

			map[ ( type << 16 ) | (subType << 8 ) | id ] = description;
		}
		loaded = true;
	}

	std::map< int, CGeneDescription >::const_iterator itor =
		map.find( ( type << 16 ) | (subType << 8 ) | id );

	if( itor != map.end() ) description = itor->second;
	return description;
}

std::vector< std::string > &GetPoseVector()
{
	static std::vector< std::string > poseNames( 512, "" );
	static bool init = false;
	if( !init )
	{
		char buff[4];
		for( int i = 0; i < 256; ++i )
		{
			poseNames[i] = itoa( i, buff, 10 );
			poseNames[i + 256] = itoa( i, buff, 10 );
		}
		init = true;
	}
	return poseNames;
}

void ImportPoseDescriptions( std::string filename )
{
	std::ifstream file( filename.c_str(), std::ios_base::binary );
	int i, nPoses = 0;

	char buff[4];
	for( i = 0; i < 256; ++i ) GetPoseVector()[i] = itoa( i, buff, 10 );

	while( !file.eof() )
	{
		WORD length;
		std::string name;
		file.read( (char *)&length, 2 );
		name.resize( length );
		i = 0;
		while( length-- ) file.read( &(name[i++]), 1 );
		if( nPoses < 256 ) GetPoseVector()[nPoses++] = name;
	}
}

std::string GetPoseDescription( int pose )
{
	std::string ret;
	if( pose < GetPoseVector().size() ) ret = GetPoseVector()[ pose ];
	return ret;
}

void SetPoseDescription( int pose, std::string const &name )
{
	if( pose < GetPoseVector().size() )  GetPoseVector()[ pose ] = name;
}

void SavePoseDescriptions( std::string filename )
{
	std::ofstream file( filename.c_str() );
	std::vector< std::string > &v = GetPoseVector();
	std::vector< std::string >::const_iterator i;

	for( i = v.begin(); i != v.end(); ++i )
		file << *i << std::endl;
}

void LoadPoseDescriptions( std::string filename )
{
	std::ifstream file( filename.c_str() );
	if( !file ) return;

	std::vector< std::string > &v = GetPoseVector();

	int i = 0;
	while( !file.eof() )
	{
		std::string caption;
		std::getline( file, caption );
		if( i < 512 ) v[i] = caption;
		++i;
	}
}

void ImportGaitDescriptions( std::string filename, CGenome *genome )
{
	std::ifstream file( filename.c_str(), std::ios_base::binary );

	char buff[4];
	for( int i = 0; i < 256; ++i ) GetPoseVector()[i + 256] = itoa( i, buff, 10 );

	file.ignore( 4 );
	while( !file.eof() )
	{
		WORD type, subType, id, size;

		file.read( (char *)&type, 2 );
		file.read( (char *)&subType, 2 );
		file.read( (char *)&id, 2 );
		file.ignore( 2 );

		file.read( (char *)&size, 2 );

		if( type == 2 && subType == 4 )
		{
			std::string caption;
			caption.reserve( size );
			while( size-- ) caption += file.get();

			for( CGeneIterator i = genome->GeneTypeBegin( 2, 4 );
				i != genome->GeneEnd(); ++i )
				{
					if( i->GetObj()->m_ID == id )
					{
						GetPoseVector()[ i->GetObj()->m_Data[0] + 256 ] = caption;
						break;
					}
				}
		}
		else
			file.ignore( size );


		file.read( (char *)&size, 2 );
		file.ignore( size );

	}

}

