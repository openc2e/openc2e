// ----------------------------------------------------------------------------
// Filename:	BrainDlg.h
// Class:		BrainDlg
// Purpose:		Abstract base class for uniting brain dlg objects
// -----------------------------------------------------------------------------

#ifndef BrainDlg_H
#define BrainDlg_H

class BrainDlg
{
public:
	virtual void Activate() = 0;
	virtual const void *LinkedTo() = 0;
	virtual const void *OrganLinkedTo(){return NULL;};
	virtual void Update() {};
	virtual void DumpToLobe(){};
	virtual bool Updatable() {return false;};

};

#endif

