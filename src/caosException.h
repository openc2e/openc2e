#pragma once

#include "creaturesException.h"

#include <memory>

class caosException : public creaturesException {
    protected:
        std::shared_ptr<class script> script;
        int traceindex = -1;

    public:
        using creaturesException::creaturesException;

        /* debug hook, removeme */
        virtual const char *what() const throw() { return this->creaturesException::what(); }

        void trace(std::shared_ptr<class script> scr, int traceindex = -1) throw();
        virtual std::string prettyPrint() const;
};
