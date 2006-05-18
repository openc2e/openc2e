#ifndef SER_CAOSSCRIPT_H
#define SER_CAOSSCRIPT_H 1

#include <serialization.h>
#include "ser/s_caosVar.h"
#include <caosScript.h>
#include "ser/s_bytecode.h"

typedef Variant *v_type;
static inline Variant *findVariant(const std::string &name) {
    if (name == "  (NULL)  ")
        return NULL;
    if (variants.find(name) == variants.end())
        throw new creaturesException(std::string("Variant ") + name + " not available in this build of openc2e.");
    return variants[name];
}

LOAD(script) {
    ar & obj.linked;
    ar & obj.relocations;
    ar & obj.allOps;
    ar & obj.fmly & obj.gnus & obj.spcs & obj.scrp;

    std::string variant;
    ar & variant;
    obj.variant = findVariant(variant);

    ar & obj.filename;
    ar & obj.gsub;
}

SAVE(script) {
    ar & obj.linked;
    ar & obj.relocations;
    ar & obj.allOps;
    ar & obj.fmly & obj.gnus & obj.spcs & obj.scrp;

    std::string variant = "  (NULL)  ";
    if (obj.variant != NULL)
        variant = obj.variant->name;
    ar & variant;
    ar & obj.filename;
    ar & obj.gsub;
}


SAVE(caosScript) {
    if (obj.v)
        ar & obj.v->name;
    else {
        std::string v = "  (NULL)  ";
        ar & v;
    }
    ar & obj.filename;
    ar & obj.installer & obj.removal;
    ar & obj.scripts;
    ar & obj.current;
}

LOAD(caosScript) {
    std::string v;
    ar & v;
    obj.v = findVariant(v);

    ar & obj.filename;
    ar & obj.installer & obj.removal;
    ar & obj.scripts;
    ar & obj.current;
}
#endif

