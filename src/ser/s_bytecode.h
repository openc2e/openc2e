#ifndef SER_BYTECODE_H
#define SER_BYTECODE_H 1

#include "bytecode.h"
#include "ser/s_caosVar.h"
#include "serialization.h"
#include "dialect.h"

BOOST_CLASS_IMPLEMENTATION(caosOp, boost::serialization::object_serializable);
BOOST_CLASS_TRACKING(caosOp, boost::serialization::track_never);
SAVE(caosOp) {
	uint32_t op = obj.opcode | ((obj.argument + 0x800000) << 8);
	ar & op & obj.traceindex;
}

LOAD(caosOp) {
	uint32_t op;
	ar & op & obj.traceindex;
	obj.opcode = op & 0xFF;
	obj.argument = (op >> 8) - 0x800000;
}

#undef SER_xVxx

#endif

