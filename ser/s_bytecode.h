#ifndef SER_BYTECODE_H
#define SER_BYTECODE_H 1

#include "bytecode.h"
#include "ser/s_caosVar.h"
#include "serialization.h"
#include "dialect.h"

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
    ar & obj.p;
}

typedef cmdinfo *c_cmdinfo_t;

SAVE(simpleCaosOp) {
    SER_BASE(ar, caosOp);
    std::string key = obj.ci->key;
    ar & key;
}

LOAD(simpleCaosOp) {
    SER_BASE(ar, caosOp);
    
    std::string key;
    ar & key;

    if (op_key_map.find(key) == op_key_map.end())
        throw new creaturesException(std::string("Operation key not found: ") + key);
    obj.ci = op_key_map[key];
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

SERIALIZE(caosAssert) {
    SER_BASE(ar, caosOp);
}

SERIALIZE(caosFACE) {
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

SER_xVxx(opVAxx);
SER_xVxx(opOVxx);
SER_xVxx(opMVxx);

#undef SER_xVxx

#endif

