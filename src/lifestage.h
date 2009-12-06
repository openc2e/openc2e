/*
 *  lifestage.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun 06 Dec 2009.
 *  Copyright (c) 2009 Alyssa Milburn. All rights reserved.
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
#ifndef _C2E_LIFESTAGE_H
#define _C2E_LIFESTAGE_H

/*
  not 100% sure about this enum ... is 'baby' equal to 'embryo', or is 'embryo'
  zero and the rest shifted?
*/
enum lifestage { baby = 0, child = 1, adolescent = 2, youth = 3, adult = 4, old = 5, senile = 6 };

#endif

/* vim: set noet: */
