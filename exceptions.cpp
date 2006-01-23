#include "exceptions.h"
#include "bytecode.h"


const char *caosException::cacheDesc() const throw() {
    if (cache)
        return cache;
    if (filename) {
        std::ostringstream oss;
        oss << desc;
        oss << " at " << filename;
        if (line != -1)
            oss << ":" << line;
        if (op) {
            try {
                std::string od = op->dump();
                oss << " (op " << od << ")";
            } catch (std::exception &e) {
                oss << " (op dump failed with e.what() " << e.what() << ")";
            }
        }
        cache = strdup(oss.str().c_str());
    } else {
        cache = desc;
    }
    return cache;
}
