#include "stdafx.h"
#include "PoseEditor.h"

#include "PoseCommand.h"
#include "PoseEditorDoc.h"

void CPoseCommandModify::Do( CPoseEditorDoc *doc )
{
	m_Gene = doc->GetGenome()->SetMatchingGene( m_Gene );
	doc->RefreshGeneDlgs( m_Gene );
}

void CPoseCommandModify::Undo( CPoseEditorDoc *doc )
{
	Do( doc );
}

void CPoseCommandAdd::Do( CPoseEditorDoc *doc )
{
	HGene const &gene = doc->GetGenome()->AddGene( m_Gene );
	doc->UpdateAllViews( NULL, 1, (CObject *)&gene );
}

void CPoseCommandAdd::Undo( CPoseEditorDoc *doc )
{
	HGene gene = doc->GetGenome()->RemoveGene( m_Gene );
	doc->RemoveGeneDlgs( m_Gene );
	doc->RefreshGeneDlgs( m_Gene );
	doc->UpdateAllViews( NULL, 2, (CObject *)&gene );
}

void CPoseCommandRemove::Do( CPoseEditorDoc *doc )
{
	HGene gene = doc->GetGenome()->RemoveGene( m_Gene );
	doc->RemoveGeneDlgs( m_Gene );
	doc->RefreshGeneDlgs( m_Gene );
	doc->UpdateAllViews( NULL, 2, (CObject *)&gene );
}

void CPoseCommandRemove::Undo( CPoseEditorDoc *doc )
{
	HGene const &gene = doc->GetGenome()->AddGene( m_Gene );
	doc->UpdateAllViews( NULL, 1, (CObject *)&gene );
}

