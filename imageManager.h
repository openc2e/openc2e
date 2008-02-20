/*
 *  imageManager.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat May 22 2004.
 *  Copyright (c) 2004-2008 Alyssa Milburn. All rights reserved.
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

#ifndef _IMAGEMANAGER_H
#define _IMAGEMANAGER_H

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

class creaturesImage;

class imageManager {
protected:
	std::map<std::string, boost::weak_ptr<creaturesImage> > images;

public:
	boost::shared_ptr<creaturesImage> getImage(std::string name, bool is_background = false);
};

#endif
/* vim: set noet: */
