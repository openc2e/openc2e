#pragma once

#include <fstream>
#include <ghc/filesystem.hpp>

class namedifstream : public std::ifstream {
  public:
	namedifstream() = default;
	namedifstream(ghc::filesystem::path p)
		: std::ifstream(p, std::ios_base::binary), name_(p) {}
	const ghc::filesystem::path& name() const {
		return name_;
	}
	namedifstream(namedifstream&& other)
		: std::ifstream(std::move(other)), name_(std::move(other.name_)) {}
	namedifstream& operator=(namedifstream&& other) {
		std::ifstream::operator=(std::move(other));
		name_ = std::move(other.name_);
		return *this;
	}

  private:
	ghc::filesystem::path name_;
};