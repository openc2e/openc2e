#ifndef POSECOMMAND_H_
#define POSECOMMAND_H_

class CPoseEditorDoc;

#include "genome.h"

class CPoseCommand
{
public:
	virtual ~CPoseCommand() {}
	virtual void Do( CPoseEditorDoc *doc ) {}
	virtual void Undo( CPoseEditorDoc *doc ) {}
	virtual char const *Describe() {return "";}
};

class CPoseCommandModify : public CPoseCommand
{
public:
	CPoseCommandModify( CGene const &gene ) : m_Gene( gene ) {}
	virtual void Do( CPoseEditorDoc *doc );
	virtual void Undo( CPoseEditorDoc *doc );
	virtual char const *Describe() {return "Modify Gene";}
private:
	CGene m_Gene;
};

class CPoseCommandAdd : public CPoseCommand
{
public:
	CPoseCommandAdd( CGene const &gene ) : m_Gene( gene ) {}
	virtual void Do( CPoseEditorDoc *doc );
	virtual void Undo( CPoseEditorDoc *doc );
	virtual char const *Describe() {return "Add Gene";}
private:
	CGene m_Gene;
};

class CPoseCommandRemove : public CPoseCommand
{
public:
	CPoseCommandRemove( CGene const &gene ) : m_Gene( gene ) {}
	virtual void Do( CPoseEditorDoc *doc );
	virtual void Undo( CPoseEditorDoc *doc );
	virtual char const *Describe() {return "Remove Gene";}
private:
	CGene m_Gene;
};

typedef handle< CPoseCommand > HPoseCommand;
#endif

