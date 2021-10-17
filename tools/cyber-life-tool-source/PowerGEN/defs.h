// --------------------------------------------------------------------------------------
// File:		defs.h
// Class:		(None)
// Purpose:		Miscellaneous definitions for NextGEN
// Description:	
//  Contains lots of definitions and information used to manage the NextGEN data.
// History:
//	16Jun99		DanS	Initial Version
//
// --------------------------------------------------------------------------------------

#ifndef DEFS_H
#define DEFS_H

#include <string>

#pragma once

/*
 * dictionaryMode == 0 => No dictionary operations (I.E. Use "long" filenames)
 * dictionaryMode == 1 => normal fourchar mode attempt to compute twodigit code & fourth is 1st of username
 * dictionaryMode == 2 => one char from dict per perm-level & then 1 from username, e.g. 12aabg.gen
 */
extern int dictionaryMode;

namespace NextGEN
{

//The byte type is used throughout the programs.
typedef unsigned char byte;

}

#endif //DEFS_H

