#include "caosException.h"

#include "caosScript.h"

void caosException::trace(std::shared_ptr<class script> scr, int traceindex) throw() {
	this->script = scr;
	this->traceindex = traceindex;
}

std::string caosException::prettyPrint() const {
	std::string buf;
	buf += what();
	buf += "\n";
	if (!script)
		return buf + "Source information unavailable.\n";
	buf += "in file ";
	buf += script->filename;
	if (traceindex < 0 || (size_t)traceindex >= script->tokinfo->size()) {
		buf += "\n";
		return buf;
	}
	toktrace tr = (*script->tokinfo)[traceindex];
	buf += " near line " + std::to_string(tr.lineno);

	int start = 0;
	for (int i = 0; i < traceindex; i++) {
		start += (*script->tokinfo)[i].width + 1;
	}
	int end = start + tr.width;
	if ((size_t)start >= script->code->size() || (size_t)end >= script->code->size()) {
		buf += "\n";
		return buf;
	}
	buf += ":\n";

	int linelen = 73; // XXX, margins aren't being counted it seems
	int contextl = (linelen - tr.width) / 2;
	int contextr = linelen - contextl;
	int marginl = 0;
	int marginr = 0;

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
		buf += "...";
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
		buf += (*script->code)[i];
	if (marginr)
		buf += "...";
	buf += "\n";

	for (int i = 0; i < contextl + marginl; i++)
		buf += " ";
	for (int i = 0; i < tr.width; i++)
		buf += "^";
	buf += "\n";
	return buf;
}
