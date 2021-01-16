#pragma once

#include "serfwd.h"

#include <memory>
#include <string>

class shared_str {
	FRIEND_SERIALIZE(shared_str)
  protected:
	std::shared_ptr<std::string> p;

  public:
	shared_str()
		: p(std::make_shared<std::string>()) {}
	shared_str(const std::string& p)
		: p(std::make_shared<std::string>(p)) {}

	std::string& operator*() { return *p.get(); }
	const std::string& operator*() const { return *p.get(); }
	std::string* operator->() { return p.get(); }
	const std::string* operator->() const { return p.get(); }
};
