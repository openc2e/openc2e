#pragma once

#include "creaturesException.h"

#include <string>
#include <memory>
#include <vector>

class parseException : public creaturesException {
public:
    using creaturesException::creaturesException;

    std::shared_ptr<std::vector<struct token> > context;
    int ctxoffset;
    std::string filename;
    int lineno = -1;

    std::string prettyPrint() const;
};

