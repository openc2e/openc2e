/*
 *  caosVM_compound.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Mon May 31 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#include "caosVM.h"
#include "CompoundAgent.h"
#include "openc2e.h"

/**
 PART (command) part_id (integer)
 
 set the part number of the TARGed compound agent/vehicle to work on (ANIM/POSE use this, amongst other commands).
*/
void caosVM::c_PART() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(part_id)

	CompoundAgent *a = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(a);
	// TODO: this isn't entirely right, we should check it exists instead (maybe part should be a pointer?)
	caos_assert(part_id < a->partCount());
	part = part_id;
}

/**
 PAT: DULL (command) part (integer) sprite (string) first_image (integer) x (integer) y (integer) plane (integer)

 create a new 'dull' part for the TARGed compound agent/vehicle which does nothing but display an image.
 number part ids beginning at 1. x/y/plane are relative to the agent you're working on.
*/
void caosVM::c_PAT_DULL() {
	VM_VERIFY_SIZE(6)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_STRING(sprite)
	VM_PARAM_INTEGER(part)
	
	caos_assert(part > 0);
	caos_assert(targ);
	CompoundAgent *a = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(a);

	CompoundPart *p = new DullPart(part, sprite, first_image, x, y, plane);
	a->addPart(p);
}

/**
 PAT: BUTT (command) part (integer) sprite (string) first_image (integer) image_count (integer) x (integer) y (integer) plane (integer) hoveranim (byte-string) messageid (integer) option (integer)
 
 creates a new 'button' part for the TARGed compound agent/vehicle
 number part ids beginning at 1. x/y/plane are relative to the agent you're working on.
 hoveranim is the animation to use when the part is mouseovered - see ANIM for details
 messageid is the message sent when the button is clicked - _p1_ of the message is set to the part number
 if option is 1, mouseclicks/hovers only apply to non-transparent areas. otherwise, option should be 0.
*/
void caosVM::c_PAT_BUTT() {
	VM_VERIFY_SIZE(10)
	VM_PARAM_INTEGER(option)
	VM_PARAM_INTEGER(messageid)
	VM_PARAM_BYTESTR(hoveranim)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_STRING(sprite)
	VM_PARAM_INTEGER(part)
	
	caos_assert(part > 0);
	caos_assert((option == 0) || (option == 1));
	caos_assert(targ);
	CompoundAgent *a = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(a);

	// TODO TODO TODO we don't take image_count!!
	CompoundPart *p = new ButtonPart(part, sprite, first_image, x, y, plane, hoveranim, messageid, option);
	a->addPart(p);
}

/**
 PAT: FIXD (command) part (integer) sprite (string) first_image (integer) image_count (integer) x (integer) y (integer) plane (integer) fontsprite (string)
 %status maybe
*/
void caosVM::c_PAT_FIXD() {
	VM_PARAM_STRING(fontsprite)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_STRING(sprite)
	VM_PARAM_INTEGER(part)	
	
	caos_assert(part > 0);
	caos_assert(targ);
	CompoundAgent *a = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(a);
	
	// TODO TODO TODO we don't take image_count!!
	CompoundPart *p = new FixedTextPart(part, sprite, first_image, x, y, plane, fontsprite);
	a->addPart(p);
}

/**
 PAT: KILL (command) part (integer)
 
 kill the specified part of the TARGed compound agent/vehicle
*/
void caosVM::c_PAT_KILL() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(part)
	
	caos_assert(part > 0);
	caos_assert(targ);
	CompoundAgent *a = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(a);
	
	a->delPart(part);
}

/**
 FCUS (command)

 focus current targeted part, which must be a PAT: TEXT
 if target is null, then unfocus current part
*/
void caosVM::c_FCUS() {
	VM_VERIFY_SIZE(0)

	if (!targ) {
		// TODO
	} else {
		CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
		caos_assert(c);
		TextEntryPart *p = dynamic_cast<TextEntryPart *>(c->part(part));
		caos_assert(p);
		// TODO
	}
}
/* vim: set noet: */
