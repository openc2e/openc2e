#ifndef GENOME_H_
#define GENOME_H_

#include "Gene.h"
#include <list>
#include "handle.h"

typedef handle< CGene > HGene;
typedef std::list< HGene > CHGeneList;

class CGeneIterator
{
public:
	CGeneIterator( CHGeneList *geneList, CHGeneList::iterator itor, BYTE type, BYTE subType )
		: m_GeneList( geneList ), m_Itor( itor ), m_Type( type ), m_SubType( subType )
	{
		IncToType();
	}

	HGene *operator->() { return m_Itor.operator->(); }
	HGene &operator*() { return m_Itor.operator*(); }
		
		
	CGeneIterator &operator++() { ++m_Itor; IncToType(); return *this; }
	CGeneIterator operator++(int) { CGeneIterator itor = *this; ++m_Itor; IncToType(); return itor; }

	bool operator!=( CGeneIterator const &other )
	{
		return other.m_Itor != m_Itor;
	}

	bool operator==( CGeneIterator const &other )
	{
		return other.m_Itor == m_Itor;
	}

	CHGeneList::iterator GetGeneItor() {return m_Itor; }

private:
	void IncToType()
	{
		while( m_Itor != m_GeneList->end() &&
			   ( m_Itor->GetObj()->m_Type != m_Type ||
			     m_Itor->GetObj()->m_SubType != m_SubType ) )
			++m_Itor;
	}
	CHGeneList *m_GeneList;
	CHGeneList::iterator m_Itor;
	BYTE m_Type;
	BYTE m_SubType;
};

class CGenome
{
public:
	bool Load( LPCTSTR lpszPathName );
	bool Save( LPCTSTR lpszPathName ) const;

	CGeneIterator GeneTypeBegin( BYTE type, BYTE subType )
	{
		return CGeneIterator( &m_Genes, m_Genes.begin(), type, subType );
	}
	CGeneIterator GeneEnd()
	{
		return CGeneIterator( &m_Genes, m_Genes.end(), 0, 0 );
	}
	HGene const &AddGene( CGene const &gene );
	HGene RemoveGene( CGene const &gene );
	CGene SetMatchingGene( CGene const &gene );

	int NewID( int type, int subType );

	int GetGenus();
	bool GetBodyPart( int part, int *genus, int *variant );

private:
	CHGeneList m_Genes;
	int m_Version;
};
#endif

