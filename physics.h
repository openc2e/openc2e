/*
 *  physics.h
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
#ifndef PHYSICS_H
#define PHYSICS_H 1

#include <iostream> // XXX debug
#include <cmath>   // sqrt
#include <algorithm> // swap
#include "openc2e.h" // FRIEND_SERIALIZE

struct Point {
	float x, y;
	Point() { x = y = 0; }
	Point(float _x, float _y) : x(_x), y(_y) {}
	Point(const Point &p) : x(p.x), y(p.y) { }

	bool operator==(const Point &p) { return x == p.x && y == p.y; }
	bool operator!=(const Point &p) { return !(*this == p); }
	Point &operator=(const Point &r) {
		x = r.x;
		y = r.y;
		return *this;
	}
};

enum linetype { NORMAL, HORIZONTAL, VERTICAL };

class Line {
	protected:
		FRIEND_SERIALIZE(Line);
		Point start, end;
		double x_icept, y_icept, slope;
		linetype type;
	public:

		void dump() const;

		Line() {
			start = Point(0,0);
			end = Point(1,1);
			x_icept = y_icept = 0;
			slope = 1;
			type = NORMAL;
		}

		Line(const Line &l) {
			start = l.start;
			end = l.end;
			x_icept = l.x_icept;
			y_icept = l.y_icept;
			slope = l.slope;
			type = l.type;
		}
		
		Line(Point s, Point e);

		Line &operator=(const Line &l) {
			start = l.start;
			end = l.end;
			x_icept = l.x_icept;
			y_icept = l.y_icept;
			slope = l.slope;
			type = l.type;
			return *this;
		}

		bool intersect(const Line &l, Point &where) const; 

		linetype getType() const { return type; }
		double xIntercept() const { return x_icept; }
		double yIntercept() const { return y_icept; }
		double getSlope() const { return slope; }
		const Point &getStart() const { return start; }
		const Point &getEnd() const { return end; }

		// TODO: this code hasn't really been tested - fuzzie
		bool containsPoint(Point p) const {
			if (type == VERTICAL) {
				bool is_x = fabs(start.x - p.x) < 1;
				bool is_y = containsY(p.y);
				// TODO
				//bool is_v = (start.x > (p.x + 0.5)) && (start.x < (p.x - 0.5));
				//bool is_h = (start.y > (p.y + 0.5)) && (start.y < (p.y - 0.5));
				return (is_x && is_y);
			} else if (type == HORIZONTAL) {
				bool is_y = fabs(start.y - p.y) < 1;
				bool is_x = containsX(p.x);

				return is_x && is_y;
			} else {
				Point point_on_line = pointAtX(p.x);
				return containsX(p.x) && fabs(point_on_line.y - p.y) < 1;
			}
		}
					
		Point pointAtX(double x) const
			{ 
				assert(type != VERTICAL);
				if (type == NORMAL)
					return Point(x, (x - start.x) * slope + start.y); 
				else
					return Point(x, start.y);
				
			}
		Point pointAtY(double y) const
			{
				assert(type != HORIZONTAL);
				if (type == NORMAL)
					return Point((y - start.y) / slope + start.x, y);
				else
					return Point(start.x, y);
			}

		bool containsX(double x) const {
			return x >= start.x && x <= end.x;
		}

		bool containsY(double y) const {
			if (start.y > end.y)
				return y <= start.y && y >= end.y;
			else
				return y >= start.y && y <= end.y;
		}

		void sanity_check() const;
};

class Vector {
	public:
		double x, y;
		Vector() {
			x = y = 0;
		}

		Vector(double _x, double _y) {
			x = _x;
			y = _y;
		}

		Vector(const Point &s, const Point &e) {
			x = e.x - s.x;
			y = e.y - s.y;
		}

		double getMagnitude() const { return sqrt(x*x+y*y); }
		
		Line extendFrom(const Point &p)  const {
			return Line(p, Point(p.x + x, p.y + y));
		}

		Vector scaleToMagnitude(double m) const {
			return Vector(x/getMagnitude()*m, y/getMagnitude()*m);
		}

		Vector scale(double multiplier) const {
			return Vector(x * multiplier, y * multiplier);
		}
		
		bool extendIntersect(const Point &start, Line barrier,
				Vector &residual) const {
			Line l = extendFrom(start);
			Point i;
			if (!l.intersect(barrier, i))
				return false;
			residual = Vector(i, Point(start.x + x, start.y + y));
			return true;
		}

		double getX() const { return x; }
		double getY() const { return y; }

		Vector operator*(double m) const {
			return scale(m);
		}
};
Point operator+(const Vector &v, const Point &p);
Point operator+(const Point &p, const Vector &v);

#endif
/* vim: set noet: */
