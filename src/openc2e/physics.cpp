/*
 *  physics.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue 08 Feb 2005.
 *  Copyright (c) 2005 Alyssa Milburn. All rights reserved.
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
#include "physics.h"

#include "common/throw_ifnot.h"

#include <cassert>
#include <fmt/core.h>

void Line::dump() const {
	fmt::print("pst = ({}, {}) end=({}, {})\n", start.x, start.y, end.x, end.y);
	fmt::print("xi = {} yi = {} m = {}\n", x_icept, y_icept, slope);
	fmt::print("type = {}\n", [&] {
		switch (type) {
			case NORMAL: return "NORMAL"; break;
			case HORIZONTAL: return "HORIZ"; break;
			case VERTICAL: return "VERT"; break;
		}
	}());
	sanity_check();
}

Line::Line(Point s, Point e) {
	if (s.x > e.x)
		std::swap(s, e);
	start = s;
	end = e;

	if (s.x == e.x) {
		type = VERTICAL;
		x_icept = s.x;
	} else if (s.y == e.y) {
		type = HORIZONTAL;
		y_icept = s.y;
		slope = 0;
	} else {
		type = NORMAL;
		slope = (end.y - start.y) / (end.x - start.x);
		/* y = mx + b
		 * b = y - mx
		 */
		y_icept = start.y - slope * start.x;
		/* 0 = mx + b
		 * x = -b/m
		 */
		x_icept = -y_icept / slope;
	}
}

bool Line::intersect(const Line& l, Point& where) const {
	if (type == HORIZONTAL) {
		if (l.type == HORIZONTAL)
			//return l.start.y == start.y;
			return false;
		// XXX: set where to something useful
		if (l.type == VERTICAL) {
			if (!(l.containsY(start.y) && containsX(l.start.x)))
				return false;
			where.x = l.start.x;
			where.y = start.y;
			return true;
		}
		if (l.type == NORMAL) {
			/* mx + b = y
			 * mx = y - b
			 * x = (y - b) / m
			 */
			double x = (start.y - l.y_icept) / l.slope;
			if (l.containsX(x) && containsX(x)) {
				where = Point(x, start.y);
				return true;
			} else
				return false;
		}
	}
	if (type == VERTICAL) {
		if (l.type == VERTICAL)
			//return l.start.x == start.x;
			return false;
		// XXX: set where to something useful
		if (l.type == HORIZONTAL) {
			if (!(l.containsX(start.x) && containsY(l.start.y)))
				return false;
			where.x = start.x;
			where.y = l.start.y;
			return true;
		}
		if (!l.containsX(start.x))
			return false;
		where = l.pointAtX(start.x);
		return containsY(where.y);
	}

	if (l.type != NORMAL)
		return l.intersect(*this, where);

	assert(l.type == NORMAL && type == NORMAL);

	double x, y;

	if (slope == l.slope)
		return false; // XXX handle parallel overlap sanely

	/* y = m1 * x + b1
	 * y = m2 * x + b2
	 *
	 * m1 * x + b1 = m2 * x + b2
	 * Solving for x:
	 * b1 - b2 = (m2 - m1) x
	 * x = (b1 - b2) / (m2 - m1)
	 */
	x = (y_icept - l.y_icept) / (l.slope - slope);
	y = slope * x + y_icept;

	if (containsX(x) && l.containsX(x)) {
		where = Point(x, y);
		return true;
	}
	return false;
}

void Line::sanity_check() const {
	if (type == NORMAL) {
		double xp = pointAtY(yIntercept()).x;
		double yp = pointAtX(xIntercept()).y;
		THROW_IFNOT(fabs(xp) < 1);
		THROW_IFNOT(fabs(yp) < 1);
	}
}

/* vim: set noet: */
