/*
 *  PathResolver.h
 *  openc2e
 *
 *  Created by Bryan Donlan
 *  Copyright (c) 2005 Bryan Donlan. All rights reserved.
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

#ifndef PATHRESOLVER_H
#define PATHRESOLVER_H 1

#include <ghc/filesystem.hpp>
#include <string>
#include <vector>

/* resolveFile
 *
 * Find a file's name, without caring about its capitalization.
 */
std::string resolveFile(ghc::filesystem::path path);
std::vector<std::string> findByWildcard(std::string dir, std::string wild);

#endif

/* vim: set noet: */
