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

BOOST_EXPORT_CLASS(caosNoop);
SERIALIZE(caosNoop) {
    SER_BASE(ar, caosOp);
}

BOOST_EXPORT_CLASS(caosJMP);
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

BOOST_EXPORT_CLASS(simpleCaosOp);
SERIALIZE(simpleCaosOp) {
    SER_BASE(ar, caosOp);

    ar & obj.ci;
}

BOOST_EXPORT_CLASS(caosREPS);
SERIALIZE(caosREPS) {
    SER_BASE(ar, caosOp);
    ar & obj.exit;
}

BOOST_EXPORT_CLASS(caosGSUB);
SERIALIZE(caosGSUB) {
    SER_BASE(ar, caosOp);
    ar & obj.targ;
}

BOOST_EXPORT_CLASS(caosCond);
SERIALIZE(caosCond) {
    SER_BASE(ar, caosOp);
    ar & obj.cond & obj.branch;
}

BOOST_EXPORT_CLASS(caosENUM_POP);
SERIALIZE(caosENUM_POP) {
    SER_BASE(ar, caosOp);
    ar & obj.exit;
}

BOOST_EXPORT_CLASS(caosSTOP);
SERIALIZE(caosSTOP) {
    SER_BASE(ar, caosSTOP);
}

#endif

