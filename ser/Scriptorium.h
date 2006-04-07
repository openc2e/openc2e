#ifndef SER_SCRIPTORIUM_H
#define SER_SCRIPTORIUM_H 1

#include <ser/caosScript.h>
#include <Scriptorium.h>
#include <serialization.h>

SERIALIZE(Scriptorium) {
    ar & obj.scripts;
}

#endif

