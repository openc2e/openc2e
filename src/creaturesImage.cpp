/*
 *  creaturesImage.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Jun 06 2004.
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

#include "creaturesImage.h"
#include "exceptions.h"

uint8 *creaturesImage::getCustomPalette() {
	throw creaturesException("Internal error: Tried to get a custom palette of a sprite which doesn't support that.");
}
	
bool creaturesImage::transparentAt(unsigned int frame, unsigned int x, unsigned int y) {
	return false;
}

std::shared_ptr<creaturesImage> creaturesImage::mutableCopy() {
	throw creaturesException("Internal error: Tried to make a mutable copy of a sprite which doesn't support that.");
}

void creaturesImage::tint(unsigned char r, unsigned char g, unsigned char b, unsigned char rotation, unsigned char swap) {
	throw creaturesException("Internal error: Tried to tint a sprite which doesn't support that.");
}

/* vim: set noet: */
