/*
 *  mappedfile.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat Jul 24 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#pragma once

#include <ghc/filesystem.hpp>
#include <stdexcept>
#include <string>

class mappedfileerror : public std::runtime_error {
  public:
	using runtime_error::runtime_error;
};

class mappedfile final {
  public:
	mappedfile();
	mappedfile(const ghc::filesystem::path& filename);
	mappedfile(const mappedfile&) = delete;
	mappedfile& operator=(const mappedfile&) = delete;
	mappedfile(mappedfile&&);
	mappedfile& operator=(mappedfile&&);
	~mappedfile();

	bool is_open() const;
	void close();
	const uint8_t* data() const;
	size_t size() const;

  private:
#ifdef _WIN32
	void* m_file = (void*)-1; // INVALID_HANDLE_VALUE
	void* m_mapping = nullptr;
	void* m_view = nullptr;
#else
	void* m_map = nullptr;
#endif
	size_t m_size = 0;
};
