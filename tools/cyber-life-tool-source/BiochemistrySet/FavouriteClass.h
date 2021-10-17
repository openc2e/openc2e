// FavouriteClass.h: interface for the CFavouriteClass class.
//
//////////////////////////////////////////////////////////////////////

#ifndef FAVOURITE_CLASS_H
#define FAVOURITE_CLASS_H

#pragma once

#include <vector>
#include "DialogSpec.h"

class CFavouriteClass  
{
private:
	std::vector<CDialogSpec> vSpecs;
	static const int file_format_ver;

public:
	CFavouriteClass();
	void Add(const CDialogSpec& spec);
	void Delete(int i);
	int size() { return vSpecs.size(); };
	CDialogSpec GetSpec(int i) { ASSERT(i >= 0); ASSERT(i < vSpecs.size()); return vSpecs[i]; };
	void Save(const CString& filename);
	void Load(const CString& filename);
};

#endif // FAVOURITE_CLASS_H

