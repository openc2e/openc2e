#pragma once

#include "common/Exception.h"

#include <memory>

class caosException : public Exception {
  protected:
	std::shared_ptr<class script> script;
	int traceindex = -1;

  public:
	using Exception::Exception;

	/* debug hook, removeme */
	virtual const char* what() const throw() { return this->Exception::what(); }

	void trace(std::shared_ptr<class script> scr, int traceindex = -1) throw();
	virtual std::string prettyPrint() const;
};
