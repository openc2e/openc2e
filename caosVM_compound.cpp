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
#include "World.h"

/**
 PART (command) part_id (integer)
 %status maybe
 
 set the part number of the TARGed compound agent/vehicle to work on (ANIM/POSE use this, amongst other commands).
*/
void caosVM::c_PART() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(part_id)

	caos_assert((part_id >= 0) || (part_id == -1));
	caos_assert(targ);
	CompoundAgent *a = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(a);
	caos_assert(a->part(part_id) || (part_id == -1));
	part = part_id;
}

/**
 PART (integer) part_id (integer)
 %status maybe

 return 1 if the given part number exists on the target agent, or 0 otherwise.
*/
void caosVM::v_PART() {
	VM_PARAM_INTEGER(part_id)

	caos_assert(part_id >= 0); // TODO: should we do this?
	caos_assert(targ);
	CompoundAgent *a = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(a);
	if (a->part(part_id))
		result.setInt(1);
	else
		result.setInt(0);
}	

/**
 PAT: DULL (command) part (integer) sprite (string) first_image (integer) x (integer) y (integer) plane (integer)
 %status maybe

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
	
	caos_assert(part >= 0);
	caos_assert(targ);
	CompoundAgent *a = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(a);

	CompoundPart *p = new DullPart(a, part, sprite, first_image, x, y, plane);
	a->addPart(p);
}

/**
 PAT: BUTT (command) part (integer) sprite (string) first_image (integer) image_count (integer) x (integer) y (integer) plane (integer) hoveranim (byte-string) messageid (integer) option (integer)
 %status maybe
 
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
	
	caos_assert(part >= 0);
	caos_assert((option == 0) || (option == 1));
	caos_assert(targ);
	CompoundAgent *a = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(a);

	// TODO TODO TODO we don't take image_count!!
	CompoundPart *p = new ButtonPart(a, part, sprite, first_image, x, y, plane, hoveranim, messageid, option);
	a->addPart(p);
}

/**
 PAT: FIXD (command) part (integer) sprite (string) first_image (integer) x (integer) y (integer) plane (integer) fontsprite (string)
 %status maybe
*/
void caosVM::c_PAT_FIXD() {
	VM_PARAM_STRING(fontsprite)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_STRING(sprite)
	VM_PARAM_INTEGER(part)	
	
	caos_assert(part >= 0);
	caos_assert(targ);
	CompoundAgent *a = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(a);
	
	CompoundPart *p = new FixedTextPart(a, part, sprite, first_image, x, y, plane, fontsprite);
	a->addPart(p);
}

/**
 PAT: TEXT (command) part (integer) sprite (string) first_image (integer) x (integer) y (integer) plane (integer) message_id (integer) fontsprite (string)
 %status maybe
*/
void caosVM::c_PAT_TEXT() {
	VM_PARAM_STRING(fontsprite)
	VM_PARAM_INTEGER(message_id)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_STRING(sprite)
	VM_PARAM_INTEGER(part)	
	
	caos_assert(part >= 0);
	caos_assert(targ);
	CompoundAgent *a = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(a);
	
	CompoundPart *p = new TextEntryPart(a, part, sprite, first_image, x, y, plane, message_id, fontsprite);
	a->addPart(p);
}


/**
 PAT: KILL (command) part (integer)
 %status maybe
 
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
 %status maybe

 focus current targeted part, which must be a PAT: TEXT
 if target is null, then unfocus current part
*/
void caosVM::c_FCUS() {
	VM_VERIFY_SIZE(0)

	if (!targ)
		world.setFocus(0, 0);
	else {
		CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
		caos_assert(c);
		TextEntryPart *p = dynamic_cast<TextEntryPart *>(c->part(part));
		caos_assert(p);
		world.setFocus(c, p);
	}
}

/**
 FRMT (command) left_margin (integer) top_margin (integer) right_margin (integer) button_margin (integer) line_spacing (integer) char_spacing (integer) justification (integer)
 %status maybe

 alters the appearance of the target text part. the spacing values and margins are to be specified in pixels. justification can be 0 for left, 1 for right, 2 for center, 4 for bottom, 8 for middle or 16 for 'last page scroll' (TODO?), and you can add these together (except 0/1 are mutually exclusive, obviously).
*/
void caosVM::c_FRMT() {
	VM_PARAM_INTEGER(justification)
	VM_PARAM_INTEGER(char_spacing)
	VM_PARAM_INTEGER(line_spacing)
	VM_PARAM_INTEGER(bottom_margin)
	VM_PARAM_INTEGER(right_margin)
	VM_PARAM_INTEGER(top_margin)
	VM_PARAM_INTEGER(left_margin)

	caos_assert(targ);
	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(c);
	TextPart *p = dynamic_cast<TextPart *>(c->part(part));
	caos_assert(p);

	p->setFormat(left_margin, top_margin, right_margin, bottom_margin, line_spacing, char_spacing, 
			(!(justification & 1)), (justification & 2), (justification & 4), (justification & 8), (justification & 16));
}

/**
 PTXT (command) text (string)
 %status maybe
 
 sets the text of the current text part
*/
void caosVM::c_PTXT() {
	VM_PARAM_STRING(text)

	caos_assert(targ);
	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(c);
	TextPart *p = dynamic_cast<TextPart *>(c->part(part));
	caos_assert(p);

	p->setText(text);
}	

/**
 PTXT (string)
 %status maybe

 returns the text of the current text part
*/
void caosVM::v_PTXT() {
	caos_assert(targ);
	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(c);
	TextPart *p = dynamic_cast<TextPart *>(c->part(part));
	caos_assert(p);
	
	result.setString(p->getText());
}

/**
 PNXT (integer) previous_part (integer)
 %status maybe

 returns the next part of the TARG compound agent, or the first part if you pass -1, or -1 if you're at the end
*/
void caosVM::v_PNXT() {
	VM_PARAM_INTEGER(previous_part)

	caos_assert(targ);
	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(c);

	// TODO: this might not be the best way to do this..
	CompoundPart *curpart = 0;
	
	for (std::vector<CompoundPart *>::iterator x = c->parts.begin(); x != c->parts.end(); x++) {
		unsigned int i = (*x)->id;
		if ((int)i > previous_part)
			if (!curpart || i < curpart->id)
				curpart = *x;
	}

	if (curpart) result.setInt(curpart->id);
	else result.setInt(-1);
}

/**
 PAGE (command) page (integer)
 %status maybe

 Set the zero-based page number of the current text part.
*/
void caosVM::c_PAGE() {
	VM_PARAM_INTEGER(page)

	caos_assert(targ);
	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(c);
	CompoundPart *p = c->part(part);
	caos_assert(p);
	FixedTextPart *t = dynamic_cast<FixedTextPart *>(p);
	caos_assert(t);

	t->setPage(page);
}

/**
 PAGE (integer)
 %status maybe

 Returns the zero-based page number of the current text part.
*/
void caosVM::v_PAGE() {
	caos_assert(targ);
	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(c);
	CompoundPart *p = c->part(part);
	caos_assert(p);
	FixedTextPart *t = dynamic_cast<FixedTextPart *>(p);
	caos_assert(t);

	result.setInt(t->getPage());
}

/**
 NPGS (integer)
 %status maybe

 Returns the number of pages for the current text part.
*/
void caosVM::v_NPGS() {
	caos_assert(targ);
	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	caos_assert(c);
	CompoundPart *p = c->part(part);
	caos_assert(p);
	FixedTextPart *t = dynamic_cast<FixedTextPart *>(p);
	caos_assert(t);

	result.setInt(t->noPages());
}

/* vim: set noet: */
