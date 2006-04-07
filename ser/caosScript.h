#ifndef CAOSSCRIPT_H
#define CAOSSCRIPT_H 1

#include "serialization.h"
#include "ser/caosVar.h"
#include "caosScript.h"
#include "ser/bytecode.h"

LOAD(Variant *) {
    std::string name;
    bool isNull;
    ar & isNull;
    
    if (isNull) {
        obj = NULL;
        return;
    }

    ar & name;
    if (name) {
        obj = variants[name];
        if (!obj)
            throw new creaturesException(std::string("Variant ") + name + " not available in this build of openc2e.");
    } else {
        obj = NULL;
    }
}

SAVE(Variant *) {
    if (obj == NULL) {
        ar & (bool)false;
    } else {
        ar & obj->name;
    }
}

SERIALIZE(script) {
    ar & obj.linked;
    ar & obj.relocations;
    ar & obj.allOps;
    ar & obj.fmly & obj.gnus & obj.spcs & obj.scrp;

    ar & obj.variant;
    ar & obj.filename;
    ar & obj.gsub;
}

SERIALIZE(caosScript) {
    ar & obj.v;
    ar & obj.filename;
    ar & obj.installer & obj.removal;
    ar & obj.scripts;
    ar & obj.current;
}

#endif

