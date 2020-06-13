#include "parseException.h"
#include "token.h"
#include <sstream>

std::string parseException::prettyPrint() const {
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

