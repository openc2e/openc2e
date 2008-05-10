/*
 *  prayManager.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Jan 22 2006.
 *  Copyright (c) 2006 Alyssa Milburn. All rights reserved.
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

#ifndef PRAYMANAGER_H
#define PRAYMANAGER_H

#include "pray.h"

namespace fs = boost::filesystem;

class prayManager {
protected:
	std::vector<prayFile *> files;

public:
	std::map<std::string, prayBlock *> blocks;

	~prayManager();
	void addFile(prayFile *);
	void removeFile(prayFile *);
	void update();

	static std::string getResourceDir(unsigned int id);
};

#endif
/* vim: set noet: */
