#include "stdafx.h"

#include "Genome.h"

#include <algorithm>
#include <fstream>

bool CGenome::Load( LPCTSTR lpszPathName )
{
	using std::ifstream;
	using std::vector;
	using std::search;

	char buff[5];
	buff[4] = 0;

	ifstream file( lpszPathName, std::ios_base::binary );
	if( !file ) return false;
	file.read( buff, 4 );

	if( strcmp( "dna2", buff ) == 0)
		m_Version = 2;
	else if( strcmp( "dna3", buff ) == 0 )
		m_Version = 3;
	else
	{
		MessageBox( NULL, "Not a genome File.", "Error reading file", MB_OK | MB_ICONWARNING );
		return false;
	}

	vector< char > fileImage;
	char ch;
	while( file.get( ch ) ) fileImage.push_back( ch );

	std::vector< char >::iterator geneStart, geneEnd;
	geneStart = fileImage.begin() + 4;

	const char geneDelimiter[] = "gene";
	const char geneEndMarker[] = "gend";

	bool gendFound = false;

	std::ofstream log( "gene.log" );
	while( !gendFound )
	{
		geneEnd = search( geneStart, fileImage.end(),
			geneDelimiter, geneDelimiter + 4 );
		if( geneEnd == fileImage.end() )
		{
			geneEnd = search( geneStart, fileImage.end(),
				geneEndMarker, geneEndMarker + 4 );
			if( geneEnd == fileImage.end() )
				MessageBox( NULL, "Expected token 'gene' or 'gend'.", "Error reading file", MB_OK | MB_ICONWARNING );
			gendFound = true;
		}
		if( geneEnd - geneStart < 7 )
			MessageBox( NULL, "Gene does not contain header.", "Error reading file", MB_OK | MB_ICONWARNING );
		else
		{
			HGene gene( new CGene() );
			gene->m_Type = *geneStart++;
			gene->m_SubType = *geneStart++;
			gene->m_ID = *geneStart++;
			gene->m_Generation = *geneStart++;
			gene->m_SwitchOnTime = *geneStart++;
			gene->m_Flags = *geneStart++;
			gene->m_Mutability = *geneStart++;
			if( m_Version >= 3 )
				gene->m_ExpressionVariant = *geneStart++;
			while( geneStart != geneEnd ) gene->m_Data.push_back( *geneStart++ );

			//trim pose genes with extra data

			if( gene->m_Type == 2 && gene->m_Type == 3 )
				while( gene->m_Data.size() > 16 )
					gene->m_Data.pop_back();

			gene->Dump( log );
			m_Genes.push_back( gene );
		}
		geneStart = geneEnd + 4;
	}

	return true;
}

bool CGenome::Save( LPCTSTR lpszPathName ) const
{
	using std::ofstream;

	ofstream file( lpszPathName, std::ios_base::binary );
	if(	m_Version == 2 )
		file << "dna2";
	else
		file << "dna3";

	CHGeneList::const_iterator itor;
	for( itor = m_Genes.begin(); itor != m_Genes.end(); ++itor )
	{
		file << "gene";
		CGene const *gene = itor->GetObj();
		file << gene->m_Type << gene->m_SubType << gene->m_ID <<
			gene->m_Generation << gene->m_SwitchOnTime  << gene->m_Flags <<
			gene->m_Mutability;
		if( m_Version >= 3 )
			file << gene->m_ExpressionVariant;
		std::vector< BYTE >::const_iterator itorData;
		for( itorData = gene->m_Data.begin();
			itorData != gene->m_Data.end(); ++itorData )
		{
			file << *itorData;
		}
	}
	file << "gend";
	return true;
}

int CGenome::GetGenus()
{
	int genus = 0;
	for( CGeneIterator itor = GeneTypeBegin( 2, 1 ); itor != GeneEnd(); ++itor )
	{
		genus = itor->GetObj()->m_Data[0];
	}
	return genus;
}


bool CGenome::GetBodyPart( int part, int *genus, int *variant )
{
	*genus = 0;
	*variant = 0;
	for( CGeneIterator itor = GeneTypeBegin( 2, 2 ); itor != GeneEnd(); ++itor )
	{
		if( itor->GetObj()->m_Data[0] == part )
		{
			*genus = itor->GetObj()->m_Data[2];
			*variant = itor->GetObj()->m_Data[1];
			return true;
		}
	}
	return false;
}

CGene CGenome::SetMatchingGene( CGene const &gene )
{
	CGene oldGene = gene;
	for( CGeneIterator itor = GeneTypeBegin( gene.m_Type, gene.m_SubType );
		itor != GeneEnd(); ++itor )
	{
		if( itor->GetObj()->m_ID == gene.m_ID )
		{
			oldGene = *itor->GetObj();
			*itor->GetObj() = gene;
			break;
		}
	}
	return oldGene;
}

HGene CGenome::RemoveGene( CGene const &gene )
{
	HGene oldGene( 0 );
	for( CGeneIterator itor = GeneTypeBegin( gene.m_Type, gene.m_SubType );
		itor != GeneEnd(); ++itor )
	{
		if( itor->GetObj()->m_ID == gene.m_ID )
		{
			oldGene = *itor;
			m_Genes.erase( itor.GetGeneItor() );
			break;
		}
	}
	return oldGene;
}

HGene const &CGenome::AddGene( CGene const &gene )
{
	m_Genes.push_back( HGene( new CGene( gene ) ) );
	return m_Genes.back();
}

int CGenome::NewID( int type, int subType )
{
	std::vector< bool > IDs( 256, false );
	for( CGeneIterator itor = GeneTypeBegin( type, subType );
		itor != GeneEnd(); ++itor )
		IDs[ itor->GetObj()->m_ID ] = true;
	for( int i = 1; i < 256; ++i )
		if( !IDs[i] )
			return i;
	return -1;
}

