// FavouriteClass.cpp: implementation of the CFavouriteClass class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BiochemistrySet.h"
#include "FavouriteClass.h"
#include <iostream>
#include <fstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int CFavouriteClass::file_format_ver = 5;

CFavouriteClass::CFavouriteClass()
{

}

void CFavouriteClass::Add(const CDialogSpec& spec)
{
	vSpecs.push_back(spec);	
}

void CFavouriteClass::Delete(int i)
{
	ASSERT(i >= 0 && i < vSpecs.size());
	vSpecs.erase(vSpecs.begin() + i);
}

void CFavouriteClass::Save(const CString& filename)
{
	std::ofstream out( filename );
	out << file_format_ver << std::endl;
	int n = vSpecs.size();
	out << n << std::endl;	
	for (int i = 0; i < n; ++i)
	{
		out << vSpecs[i];
	}
}

void CFavouriteClass::Load(const CString& filename)
{
	std::ifstream in( filename );
	if (!in)
	{
		MessageBox(NULL, _T("Failed to load saved windows.\nCheck ") + filename, _T("Biochemistry Set"), MB_OK | MB_ICONWARNING);
		return;
	}

	int format;
	in >> format;
	if (format != file_format_ver)
	{
		if (MessageBox(NULL, _T("The saved window file format has not been\nrecognised. Press OK to lose your saved windows,\nor cancel to quit the Biochemistry Set."), _T("Biochemistry Set"), MB_OKCANCEL | MB_ICONINFORMATION) == IDOK) 
			return;
		else
			exit(EXIT_FAILURE);
	}

	int n;
	in >> n;
	vSpecs.resize(n);
	int i = 0;
	while (!in.eof() && !in.bad() && !in.fail() && i < n)
	{
		in >> vSpecs[i];
		++i;
	}
}

