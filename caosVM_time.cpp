/*
 *  caosVM_time.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat Jun 05 2004.
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

#include "caosVM.h"

/**
 PACE (float)

 return speed factor of last ten ticks.
 
 < 1.0 = engine is ticking slower than BUZZ rating. > 1.0 = engine is ticking faster.

 You might want to use this to reduce the amount of agents created or the amount of processing done if the speed factor is below 1.0.
*/
void caosVM::v_PACE() {
	// todo: we should calculate this
	result.setFloat(1.0f);
}
