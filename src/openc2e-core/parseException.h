#pragma once

#include "common/Exception.h"

#include <memory>
#include <string>
#include <vector>

class parseException : public Exception {
  public:
	using Exception::Exception;

	std::shared_ptr<std::vector<struct caostoken> > context;
	int ctxoffset;
	std::string filename;
	int lineno = -1;

	std::string prettyPrint() const;
};
