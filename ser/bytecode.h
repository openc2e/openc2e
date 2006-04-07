#ifndef SER_BYTECODE_H
#define SER_BYTECODE_H 1

#include "ser/caosVar.h"
#include "serialization.h"
#include "bytecode.h"

SERIALIZE(caosOp) {
    ar & obj.index;
    ar & obj.evalcost;
    ar & obj.owned;
    ar & obj.yyline;
}

SERIALIZE(caosNoop) {
    SER_BASE(ar, caosOp);
}

SERIALIZE(caosJMP) {
    SER_BASE(ar, caosOp);
    ar & p;
}

SAVE(const cmdinfo *) {
    if (!obj) {
        ar & std::string("");
    } else {
        ar & std::string(obj->key);
    }
}

LOAD(const cmdinfo *) {
    std::string name;
    ar & name;

    if (name == "") {
        obj = NULL;
    } else {
        if (op_key_map.find(name) == op_key_map.end())
            throw new creaturesException(std::string("Operation key not found: ") + name);
        obj = op_key_map[name];
    }
}

SERIALIZE(simpleCaosOp) {
    SER_BASE(ar, caosOp);

    ar & obj.ci;
}

SERIALIZE(caosREPS) {
    SER_BASE(ar, caosOp);
    ar & obj.exit;
}

SERIALIZE(caosGSUB) {
    SER_BASE(ar, caosOp);
    ar & obj.targ;
}

SERIALIZE(caosCond) {
    SER_BASE(ar, caosOp);
    ar & obj.cond & obj.branch;
}

SERIALIZE(caosENUM_POP) {
    SER_BASE(ar, caosOp);
    ar & obj.exit;
}

SERIALIZE(caosSTOP) {
    SER_BASE(ar, caosOp);
}

SERIALIZE(opBytestr) {
    SER_BASE(ar, caosOp);
    ar & obj.bytestr;
}

SERIALIZE(ConstOp) {
    SER_BASE(ar, caosOp);
    ar & obj.constVal;
}

#define SER_xVxx(c) \
    SERIALIZE(c) { \
        SER_BASE(ar, caosOp); \
        ar & obj.index; \
    }

SER_xVxx(op_VAxx);
SER_xVxx(op_OAxx);
SER_xVxx(op_MAxx);

#undef SER_xVxx

#endif

