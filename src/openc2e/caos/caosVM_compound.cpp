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

#include "Blackboard.h"
#include "ButtonPart.h"
#include "CameraPart.h"
#include "CompoundAgent.h"
#include "DullPart.h"
#include "Engine.h"
#include "FixedTextPart.h"
#include "GraphPart.h"
#include "TextEntryPart.h"
#include "TextPart.h"
#include "World.h"
#include "caosVM.h"
#include "common/throw_ifnot.h"

/**
 PART (command) part_id (integer)
 %status maybe
 %variants c1 c2 cv c3 sm
 
 Sets the part number of the TARGeted compound agent or vehicle to work on (ANIM/POSE use this, 
 amongst other commands).
*/
void c_PART(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(part_id)

	THROW_IFNOT((part_id >= 0) || (part_id == -1));
	vm->part = part_id;
}

/**
 PART (integer) part_id (integer)
 %status maybe

 Returns 1 if the given part number exists on the target agent, or 0 if otherwise.
*/
void v_PART(caosVM* vm) {
	VM_PARAM_INTEGER(part_id)

	THROW_IFNOT(part_id >= 0); // TODO: should we do this?
	valid_agent(vm->targ);
	if (vm->targ->part(part_id))
		vm->result.setInt(1);
	else
		vm->result.setInt(0);
}

/**
 NEW: PART (command) part (integer) x (integer) y (integer) first_image (integer) plane (integer)
 %status maybe
 %variants c1 c2
*/
void c_NEW_PART(caosVM* vm) {
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(partno)

	THROW_IFNOT(partno >= 0 && partno <= 9);
	valid_agent(vm->targ);
	CompoundAgent* a = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(a);

	// TODO: fix zordering
	CompoundPart* p = new DullPart(a, partno, a->getSpriteFile(), a->getFirstImage() + first_image, x, y, plane - a->zorder);
	a->addPart(p);

	vm->part = partno;
}

/**
 PAT: DULL (command) part (integer) sprite (string) first_image (integer) x (integer) y (integer) plane (integer)
 %status maybe
 %variants cv c3

 Adds a new 'dull' part to the TARGed compound agent/vehicle which does nothing but display an image.
 Part ID numbers begin at 1. x/y/plane are relative to the agent you're working on.
*/
void c_PAT_DULL(caosVM* vm) {
	VM_VERIFY_SIZE(6)
	VM_PARAM_INTEGER(plane)
	// TODO: should x/y be int? original docs say 'decimal'
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_STRING(sprite)
	VM_PARAM_INTEGER(part)

	THROW_IFNOT(part >= 0);
	valid_agent(vm->targ);
	CompoundAgent* a = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(a);

	CompoundPart* p = new DullPart(a, part, sprite, first_image, x, y, plane);
	a->addPart(p);
}

/**
 PAT: DULL (command) part (integer) sprite (string) first_image (integer) x (integer) y (integer) plane (integer) no_images (integer)
 %status maybe
 %variants sm
*/
void c_PAT_DULL_sm(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(no_images)

	c_PAT_DULL(vm);
}


/**
 PAT: BUTT (command) part (integer) sprite (string) first_image (integer) image_count (integer) x (integer) y (integer) plane (integer) hoveranim (bytestring) messageid (integer) option (integer)
 %status maybe
 
 Adds a new 'button' part to the TARGed compound agent/vehicle.
 Part ID numbers begin at 1. x/y/plane are relative to the agent you're working on.
 'hoveranim' is the animation to use when the part is mouse-overed - see ANIM for details.
 'messageid' is the message sent when the button is clicked - _p1_ of the message is set to the part number.
 If option is 1, mouseclicks/hovers only apply to non-transparent areas. otherwise, option should be 0.
*/
void c_PAT_BUTT(caosVM* vm) {
	VM_VERIFY_SIZE(10)
	VM_PARAM_INTEGER(option)
	VM_PARAM_INTEGER(messageid)
	VM_PARAM_BYTESTR(hoveranim)
	VM_PARAM_INTEGER(plane)
	// TODO: should x/y be int? original docs say 'decimal'
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER_UNUSED(image_count)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_STRING(sprite)
	VM_PARAM_INTEGER(part)

	THROW_IFNOT(part >= 0);
	// THROW_IFNOT((option == 0) || (option == 1)); .. I've seen '1000' and '255' used, so, gah. TODO: make sure all non-zero values are identical
	valid_agent(vm->targ);
	CompoundAgent* a = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(a);

	// TODO TODO TODO we don't take image_count!!
	CompoundPart* p = new ButtonPart(a, part, sprite, first_image, x, y, plane, hoveranim, messageid, option);
	a->addPart(p);
}

/**
 PAT: FIXD (command) part (integer) sprite (string) first_image (integer) x (integer) y (integer) plane (integer) fontsprite (string)
 %status maybe

 Adds a new 'fixed' text part to the TARGed compound agent/vehicle,
 Part ID numbers begin at 1. x/y/plane are relative to the agent you're working on.
 The 'first_image' frame of the given sprite file will be used underneath the text, which is set with PTXT.
*/
void c_PAT_FIXD(caosVM* vm) {
	VM_PARAM_STRING(fontsprite)
	VM_PARAM_INTEGER(plane)
	// TODO: should x/y be int? original docs say 'decimal'
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_STRING(sprite)
	VM_PARAM_INTEGER(part)

	THROW_IFNOT(part >= 0);
	valid_agent(vm->targ);
	CompoundAgent* a = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(a);

	CompoundPart* p = new FixedTextPart(a, part, sprite, first_image, x, y, plane, fontsprite);
	a->addPart(p);
}

/**
 PAT: TEXT (command) part (integer) sprite (string) first_image (integer) x (integer) y (integer) plane (integer) message_id (integer) fontsprite (string)
 %status maybe

 Adds a new text entry part to the TARGed compound agent/vehicle.
 Part ID numbers begin at 1. x/y/plane are relative to the agent you're working on.
 The 'first_image' frame of the given sprite file will be used underneath the text.  The part will 
 gain the focus when FCUS is called or when it is clicked.  A message of the given type will be sent.
*/
void c_PAT_TEXT(caosVM* vm) {
	VM_PARAM_STRING(fontsprite)
	VM_PARAM_INTEGER(message_id)
	VM_PARAM_INTEGER(plane)
	// TODO: should x/y be int? original docs say 'decimal'
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_STRING(sprite)
	VM_PARAM_INTEGER(part)

	THROW_IFNOT(part >= 0);
	valid_agent(vm->targ);
	CompoundAgent* a = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(a);

	CompoundPart* p = new TextEntryPart(a, part, sprite, first_image, x, y, plane, message_id, fontsprite);
	a->addPart(p);
}

/**
 PAT: CMRA (command) part (integer) sprite (string) first_image (integer) x (integer) y (integer) plane (integer) viewwidth (integer) viewheight (integer) camerawidth (integer) cameraheight(integer)
 %status maybe
*/
void c_PAT_CMRA(caosVM* vm) {
	VM_PARAM_INTEGER(cameraheight)
	VM_PARAM_INTEGER(camerawidth)
	VM_PARAM_INTEGER(viewheight)
	VM_PARAM_INTEGER(viewwidth)
	VM_PARAM_INTEGER(plane)
	// TODO: should x/y be int? original docs say 'decimal'
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_STRING(sprite)
	VM_PARAM_INTEGER(part)

	THROW_IFNOT(part >= 0);
	valid_agent(vm->targ);
	CompoundAgent* a = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(a);

	CompoundPart* p = new CameraPart(a, part, sprite, first_image, x, y, plane, viewwidth, viewheight, camerawidth, cameraheight);
	a->addPart(p);
}

/**
 PAT: GRPH (command) part (integer) sprite (string) first_image (integer) x (integer) y (integer) plane (integer) numvalues (integer)
 %status maybe

 
*/
void c_PAT_GRPH(caosVM* vm) {
	VM_PARAM_INTEGER(numvalues)
	VM_PARAM_INTEGER(plane)
	// TODO: should x/y be int? original docs say 'decimal'
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_STRING(sprite)
	VM_PARAM_INTEGER(part)

	THROW_IFNOT(part >= 0);
	valid_agent(vm->targ);
	CompoundAgent* a = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(a);

	CompoundPart* p = new GraphPart(a, part, sprite, first_image, x, y, plane, numvalues);
	a->addPart(p);
}

/**
 PAT: KILL (command) part (integer)
 %status maybe
 
 Kills the specified part of the TARGed compound agent or vehicle.
*/
void c_PAT_KILL(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(part)

	THROW_IFNOT(part > 0);
	valid_agent(vm->targ);
	CompoundAgent* a = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(a);
	if (!a->part(part))
		return; // Edynn does PAT: KILL on nonexistant parts

	a->delPart(part);
}

/**
 PAT: MOVE (command) part (integer) x (integer) y (integer)
 %status maybe

 move the compound part specified to the new relative position specified
*/
void c_PAT_MOVE(caosVM* vm) {
	// TODO: should x/y be int? original docs say 'decimal'
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(part)

	valid_agent(vm->targ);
	CompoundAgent* a = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(a);
	CompoundPart* p = a->part(part);
	THROW_IFNOT(p);

	p->x = x;
	p->y = y;
}

/**
 FCUS (command)
 %status maybe

 Focus the current targeted part, which must be a PAT: TEXT.
 If the target is null, then the current part will be unfocused.
*/
void c_FCUS(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	if (!vm->targ)
		world.setFocus(0);
	else {
		CompoundAgent* c = dynamic_cast<CompoundAgent*>(vm->targ.get());
		THROW_IFNOT(c);
		TextEntryPart* p = dynamic_cast<TextEntryPart*>(c->part(vm->part));
		THROW_IFNOT(p);
		world.setFocus(p);
	}
}

/**
 FRMT (command) left_margin (integer) top_margin (integer) right_margin (integer) button_margin (integer) line_spacing (integer) char_spacing (integer) justification (integer)
 %status maybe

 Alters the appearance of the target text part.  The spacing values and margins are to be specified in 
 pixels.  Justification can be 0 for left, 1 for right, 2 for center, 4 for bottom, 8 for middle or 16 
 for 'last page scroll' (TODO?), and you can add these together (except 0/1 are mutually exclusive, obviously).
*/
void c_FRMT(caosVM* vm) {
	VM_PARAM_INTEGER(justification)
	VM_PARAM_INTEGER(char_spacing)
	VM_PARAM_INTEGER(line_spacing)
	VM_PARAM_INTEGER(bottom_margin)
	VM_PARAM_INTEGER(right_margin)
	VM_PARAM_INTEGER(top_margin)
	VM_PARAM_INTEGER(left_margin)

	valid_agent(vm->targ);
	CompoundAgent* c = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(c);
	TextPart* p = dynamic_cast<TextPart*>(c->part(vm->part));
	THROW_IFNOT(p);

	horizontalalign h;
	if ((justification & 3) == 1)
		h = rightalign;
	else if ((justification & 3) == 2)
		h = centeralign;
	else
		h = leftalign;

	verticalalign v;
	if ((justification & 12) == 4)
		v = bottom;
	else if ((justification & 12) == 8)
		v = middle;
	else
		v = top; // TODO: i haven't verified this is the correct fallback - fuzzie

	if (char_spacing == 1)
		char_spacing = 0; // TODO: horrible hack to try and fix issues

	p->setFormat(left_margin, top_margin, right_margin, bottom_margin, line_spacing, char_spacing, h, v, (justification & 16));
}

/**
 PTXT (command) text (string)
 %status maybe
 
 Sets the text displayed in the current text part.
*/
void c_PTXT(caosVM* vm) {
	VM_PARAM_STRING(text)

	valid_agent(vm->targ);
	CompoundAgent* c = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(c);
	TextPart* p = dynamic_cast<TextPart*>(c->part(vm->part));
	THROW_IFNOT(p);

	p->setText(text);
}

/**
 PTXT (string)
 %status maybe

 Returns the text displayed in the current text part.
*/
void v_PTXT(caosVM* vm) {
	valid_agent(vm->targ);
	CompoundAgent* c = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(c);
	TextPart* p = dynamic_cast<TextPart*>(c->part(vm->part));
	THROW_IFNOT(p);

	vm->result.setString(p->getText());
}

/**
 PNXT (integer) previous_part (integer)
 %status maybe

 Returns the next part of the TARG compound agent or vehicle, (or -1 if you have reached the end, or 
 the first part if you go past -1).
*/
void v_PNXT(caosVM* vm) {
	VM_PARAM_INTEGER(previous_part)

	valid_agent(vm->targ);
	CompoundAgent* c = dynamic_cast<CompoundAgent*>(vm->targ.get());

	if (!c) { // handle non-compound agents
		if (previous_part == -1)
			vm->result.setInt(0);
		else
			vm->result.setInt(-1);
		return;
	}

	// TODO: this might not be the best way to do this..
	CompoundPart* curpart = 0;

	for (auto& part : c->parts) {
		unsigned int i = part->id;
		if ((int)i > previous_part)
			if (!curpart || i < curpart->id)
				curpart = part;
	}

	if (curpart)
		vm->result.setInt(curpart->id);
	else
		vm->result.setInt(-1);
}

/**
 PAGE (command) page (integer)
 %status maybe

 Sets the zero-based page number of the current text part.  This must be less than NPGS.
*/
void c_PAGE(caosVM* vm) {
	VM_PARAM_INTEGER(page)

	valid_agent(vm->targ);
	CompoundAgent* c = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(c);
	CompoundPart* p = c->part(vm->part);
	THROW_IFNOT(p);
	FixedTextPart* t = dynamic_cast<FixedTextPart*>(p);
	THROW_IFNOT(t);

	t->setPage(page);
}

/**
 PAGE (integer)
 %status maybe

 Returns the zero-based page number of the current text part.
*/
void v_PAGE(caosVM* vm) {
	valid_agent(vm->targ);
	CompoundAgent* c = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(c);
	CompoundPart* p = c->part(vm->part);
	THROW_IFNOT(p);
	FixedTextPart* t = dynamic_cast<FixedTextPart*>(p);
	THROW_IFNOT(t);

	vm->result.setInt(t->getPage());
}

/**
 NPGS (integer)
 %status maybe

 Returns the number of pages for the current text part.
*/
void v_NPGS(caosVM* vm) {
	valid_agent(vm->targ);
	CompoundAgent* c = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(c);
	CompoundPart* p = c->part(vm->part);
	THROW_IFNOT(p);
	FixedTextPart* t = dynamic_cast<FixedTextPart*>(p);
	THROW_IFNOT(t);

	vm->result.setInt(t->noPages());
}

/**
 GRPV (command) line (integer) value (float)
 %status stub

 Add the given value to the given line on a graph.
*/
void c_GRPV(caosVM* vm) {
	VM_PARAM_FLOAT_UNUSED(value)
	VM_PARAM_INTEGER_UNUSED(line)

	valid_agent(vm->targ);
	CompoundAgent* c = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(c);
	CompoundPart* p = c->part(vm->part);
	THROW_IFNOT(p);
	GraphPart* t = dynamic_cast<GraphPart*>(p);
	THROW_IFNOT(t);

	// TODO
}

/**
 GRPL (command) red (integer) green (integer) blue (integer) min (float) max (float)
 %status stub

 Add a new line to a graph created with PAT: GRPH with the given 
 characteristics.
*/
void c_GRPL(caosVM* vm) {
	VM_PARAM_FLOAT_UNUSED(max)
	VM_PARAM_FLOAT_UNUSED(min)
	VM_PARAM_INTEGER_UNUSED(blue)
	VM_PARAM_INTEGER_UNUSED(green)
	VM_PARAM_INTEGER_UNUSED(red)

	valid_agent(vm->targ);
	CompoundAgent* c = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(c);
	CompoundPart* p = c->part(vm->part);
	THROW_IFNOT(p);
	GraphPart* t = dynamic_cast<GraphPart*>(p);
	THROW_IFNOT(t);

	// TODO
}

/**
 BBD: WORD (command) index (integer) id (integer) text (string)
 %status maybe
 %variants c1 c2

 Change the word at index to target blackboard, setting to the provided id and text.
*/
void c_BBD_WORD(caosVM* vm) {
	VM_PARAM_STRING(text)
	VM_PARAM_INTEGER(id)
	VM_PARAM_INTEGER(index)

	if (engine.version == 1) {
		THROW_IFNOT(index < 16);
	} else {
		THROW_IFNOT(index < 48);
	}

	valid_agent(vm->targ);
	Blackboard* b = dynamic_cast<Blackboard*>(vm->targ.get());
	THROW_IFNOT(b);

	b->addBlackboardString(index, id, text);
}

/**
 BBD: SHOW (command) show (integer)
 %status maybe
 %variants c1 c2

 If show is 1, draw the current text onto part 0 of the target blackboard. If 0,
 remove it from the blackboard.
*/
void c_BBD_SHOW(caosVM* vm) {
	VM_PARAM_INTEGER(show)

	valid_agent(vm->targ);
	Blackboard* b = dynamic_cast<Blackboard*>(vm->targ.get());
	THROW_IFNOT(b);

	b->showText(show);
}

/**
 BBD: EMIT (command) audible (integer)
 %status maybe
 %variants c1 c2

 Broadcast the current word of the target blackboard. If audible is 1, broadcast
 to all nearby creatures. If 0, broadcast to all creatures looking at it.
*/
void c_BBD_EMIT(caosVM* vm) {
	VM_PARAM_INTEGER(audible)

	valid_agent(vm->targ);
	Blackboard* b = dynamic_cast<Blackboard*>(vm->targ.get());
	THROW_IFNOT(b);

	b->broadcast(audible);
}

/**
 BBD: EDIT (command) allow (integer)
 %status maybe
 %variants c1 c2

 If allow is 1, switch target blackboard into editing mode, give it focus. If it
 is 0, remove focus from target blackboard.
*/
void c_BBD_EDIT(caosVM* vm) {
	VM_PARAM_INTEGER(allow)

	valid_agent(vm->targ);

	Blackboard* b = dynamic_cast<Blackboard*>(vm->targ.get());
	THROW_IFNOT(b);
	BlackboardPart* p = b->getBlackboardPart();
	THROW_IFNOT(p);

	if (allow) {
		world.setFocus(p);
	} else {
		if (world.focusagent == vm->targ) // TODO: should this be throw_ifnot?
			world.setFocus(0);
	}
}

/**
 BBD: VOCB (command) blackboardstart (integer) globalstart (integer) count (integer)
 %status maybe
 %variants c2

 Copy count words into the blackboard word list from the global word list.
*/
void c_BBD_VOCB(caosVM* vm) {
	VM_PARAM_INTEGER(count)
	VM_PARAM_INTEGER(globalstart)
	VM_PARAM_INTEGER(blackboardstart)

	THROW_IFNOT(count >= 0);
	THROW_IFNOT(globalstart >= 0);
	THROW_IFNOT(blackboardstart >= 0);

	valid_agent(vm->targ);
	Blackboard* b = dynamic_cast<Blackboard*>(vm->targ.get());
	THROW_IFNOT(b);

	if (engine.wordlist.size() == 0)
		return; // no word list!
	THROW_IFNOT((unsigned int)globalstart < engine.wordlist.size());

	for (unsigned int i = 0; (i < (unsigned int)count) && ((unsigned int)globalstart + i < engine.wordlist.size()); i++) {
		b->addBlackboardString(blackboardstart + i, globalstart + i, engine.wordlist[globalstart + i]);
	}
}

/**
 NEW: BBTX (command) part (integer) x (integer) y (integer) width (integer)
 %status stub
 %variants c2

 Create a new C2 text part for a compound bubble object. Text will wrap as required to fit width.
*/
void c_NEW_BBTX(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(width)
	VM_PARAM_INTEGER_UNUSED(y)
	VM_PARAM_INTEGER_UNUSED(x)
	VM_PARAM_INTEGER_UNUSED(part)

	valid_agent(vm->targ);
	// TODO
}

/**
 BBTX (command) part (integer) stringindex (integer)
 %status stub
 %variants c2
*/
void c_BBTX(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(stringindex)
	VM_PARAM_INTEGER_UNUSED(part)

	valid_agent(vm->targ);
	// TODO
}

/**
 SPOT (command) spotno (integer) left (integer) top (integer) right (integer) bottom (integer)
 %status maybe
 %variants c1 c2
*/
void c_SPOT(caosVM* vm) {
	VM_PARAM_INTEGER(bottom)
	VM_PARAM_INTEGER(right)
	VM_PARAM_INTEGER(top)
	VM_PARAM_INTEGER(left)
	VM_PARAM_INTEGER(spotno)

	valid_agent(vm->targ);
	CompoundAgent* c = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(c);

	c->setHotspotLoc(spotno, left, top, right, bottom);
}

/**
 KNOB (command) function (integer) spotno (integer)
 %status maybe
 %variants c1 c2
*/
void c_KNOB(caosVM* vm) {
	VM_PARAM_INTEGER(spotno)
	VM_PARAM_INTEGER(function)

	valid_agent(vm->targ);
	CompoundAgent* c = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(c);

	c->setHotspotFunc(function, spotno);
}

/**
 KMSG (command) function (integer) flags (integer) message (integer)
 %status maybe
 %variants c2
*/
void c_KMSG(caosVM* vm) {
	VM_PARAM_INTEGER(message)
	VM_PARAM_INTEGER(flags)
	VM_PARAM_INTEGER(function)

	valid_agent(vm->targ);
	CompoundAgent* c = dynamic_cast<CompoundAgent*>(vm->targ.get());
	THROW_IFNOT(c);

	c->setHotspotFuncDetails(function, message, flags);
}

/**
 BBLE (command) text (string) ticks (integer) type (integer) track (integer)
 %status stub
 %variants c2

 Display the given text for the given number of ticks, in a bubble (type is 0 for speech or 1 for thought).
*/
void c_BBLE(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(track)
	VM_PARAM_INTEGER_UNUSED(type)
	VM_PARAM_INTEGER_UNUSED(ticks)
	VM_PARAM_STRING_UNUSED(text)

	// TODO
}

/**
 BBFD (command) part (integer) red (integer) green (integer) blue (integer)
 %status stub
 %variants c2
*/
void c_BBFD(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(blue)
	VM_PARAM_INTEGER_UNUSED(green)
	VM_PARAM_INTEGER_UNUSED(red)
	VM_PARAM_INTEGER_UNUSED(part)

	valid_agent(vm->targ);
	// TODO
}

/* vim: set noet: */
