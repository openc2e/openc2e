/*
 *  creaturesImage.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat May 22 2004.
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

#ifndef _CREATURESIMAGE_H
#define _CREATURESIMAGE_H

class creaturesImage {
protected:
  unsigned int m_numframes;
  unsigned short *widths, *heights;
  void **buffers;
  
public:
  unsigned int numframes() { return m_numframes; }
  virtual unsigned int width(unsigned int frame) { return widths[frame]; }
  virtual unsigned int height(unsigned int frame) { return heights[frame]; }
  virtual void *data(unsigned int frame) { return buffers[frame]; }
};

#endif
