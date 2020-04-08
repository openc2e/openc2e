/*
 *  exceptions.cpp
 *  openc2e
 *
 *  Created by Bryan Donlan on Sun 22 Jan 2006.
 *  Copyright (c) 2006 Bryan Donlan. All rights reserved.
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
#include "exceptions.h"
#include "bytecode.h"
#include "token.h"
#include "caosScript.h"

std::string parseFailure::prettyPrint() const {
	std::ostringstream oss;
	std::string filename = this->filename;
	if (filename == "")
		filename = std::string("(UNKNOWN)");
	oss << "Parse error at line ";
	if (lineno == -1)
		oss << "(UNKNOWN)";
	else
		oss << lineno;
	oss << " in file " << filename << ": " << what();
	if (!context)
		oss << std::endl;
	else {
		oss << " near:" << std::endl;
		int toklen = -1, stlen = 0;
		for (size_t i = 0; i < context->size(); i++) {
			std::string tokstr = (*context)[i].format();
			if (i == (size_t)ctxoffset) {
				toklen = tokstr.size(); 
			} else if (toklen == -1) {
				stlen += tokstr.size() + 1;
			}
			oss << tokstr << " ";
		}
		oss << std::endl;
		if (toklen != -1) {
			for (int i = 0; i < stlen; i++)
				oss << " ";
			for (int i = 0; i < toklen; i++)
				oss << "^";
			oss << std::endl;
		}
	}
	return oss.str();
}

void caosException::trace(std::shared_ptr<class script> scr, int traceindex) throw() {
	this->script = scr;
	this->traceindex = traceindex;
}

std::string caosException::prettyPrint() const {
	std::ostringstream oss;
	oss << what() << std::endl;
	if (!script) return oss.str() + "Source information unavailable.\n";
	oss << "in file " << script->filename;
	if (traceindex < 0 || (size_t)traceindex >= script->tokinfo->size()) {
		oss << std::endl;
		return oss.str();
	}
	toktrace tr = (*script->tokinfo)[traceindex];
	oss << " near line " << tr.lineno;

	int start = 0;
	for (int i = 0; i < traceindex; i++) {
		start += (*script->tokinfo)[i].width + 1;
	}
	int end = start + tr.width;
	if ((size_t)start >= script->code->size() || (size_t)end >= script->code->size()) {
		oss << std::endl;
		return oss.str();
	}
	oss << ":" << std::endl;

	int linelen  = 73; // XXX, margins aren't being counted it seems
	int contextl = (linelen - tr.width) / 2;
	int contextr = linelen - contextl;
	int marginl  = 0;
	int marginr  = 0;

	if (linelen <= tr.width) {
		contextl = contextr = 0;
	}

	if (contextl > start) {
		contextr += contextl - start;
		contextl = start;
	}

	if ((size_t)(end + contextr) >= script->code->size()) {
		int overflow = end + contextr - script->code->size();
		contextr -= overflow;
		contextl += overflow;
		if (contextl > start)
			contextl = start;
	}

	if (contextl < start) {
		marginl = 3;
		oss << "...";
		contextl -= 3;
		if (contextl < 0) {
			contextr += contextl;
			contextl = 0;
		}
		if (contextr < 0)
			contextr = 0;
	}

	if ((size_t)(end + contextr) < script->code->size()) {
		marginr = 3;
		contextr -= 3;
		if (contextr < 0) {
			contextl += contextr;
			contextr = 0;
			if (contextl < 0)
				contextl = 0;
		}
	}

	for (int i = start - contextl; i < end + contextr; i++)
		oss << (*script->code)[i];
	if (marginr)
		oss << "...";
	oss << std::endl;

	for (int i = 0; i < contextl + marginl; i++)
		oss << " ";
	for (int i = 0; i < tr.width; i++)
		oss << "^";
	oss << std::endl;
	return oss.str();
}




