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
#include <cstring> // memcpy
#include <cmath>   // sqrt
#include <algorithm> // swap

class physicsHandler {
private:
	float finalx, finaly;
	
public:
	bool collidePoints(float, float, float, float, float, float, float, float);
	float getCollisionX() { return finalx; }
	float getCollisionY() { return finaly; }
};

struct Point {
	float x, y;
	Point() { x = y = 0; }
	Point(float _x, float _y) : x(_x), y(_y) {}
	Point(const Point &p) { memcpy(this, &p, sizeof p); }

	bool operator==(const Point &p) { return x == p.x && y == p.y; }
	bool operator!=(const Point &p) { return !(*this == p); }
	Point &operator=(const Point &r) {
		memcpy(this, &r, sizeof r); return *this;
	}
};

enum linetype { NORMAL, HORIZONTAL, VERTICAL };

class Line {
	protected:
		Point start, end;
		double x_icept, y_icept, slope;
		linetype type;
	public:

		void dump() {
			std::cout << "pst = (" << start.x << "," << start.y << ") end=(" << end.x << "," << end.y << ")" << std::endl;
			std::cout << "xi = " << x_icept << " yi = " << y_icept << " m=" << slope << std::endl;
			std::cout << "type = ";
			switch (type) {
				case NORMAL: std::cout << "NORMAL"; break;
				case HORIZONTAL: std::cout << "HORIZ"; break;
				case VERTICAL: std::cout << "VERT"; break;
				default: std::cout << "?? (" << type << ")"; break;
			}
			std::cout << std::endl;
		}

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
		
		Line(Point s, Point e) {
			if (s.x > e.x)
				std::swap(s, e);
			start = s;
			end = e;
				
			if (s.x == e.x) {
				type = VERTICAL;
				x_icept = s.y;
			} else if (s.y == e.y) {
				type = HORIZONTAL;
				y_icept = s.x;
				slope = 0;
			} else {
				type = NORMAL;
				slope = (end.y - start.y) / (end.x - start.x);
				/* y = mx + b
				 * b = y - mx
				 */
				x_icept = start.y - slope * start.x;
				/* 0 = mx + b
				 * x = -b/m
				 */
				y_icept = -x_icept/slope;
			}
		}

		Line &operator=(const Line &l) {
			start = l.start;
			end = l.end;
			x_icept = l.x_icept;
			y_icept = l.y_icept;
			slope = l.slope;
			type = l.type;
			return *this;
		}

		
		bool intersect(const Line &l, Point &where) const {
			if (type == HORIZONTAL) {
				if (l.type == HORIZONTAL)
					return l.start.y == start.y;
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
					if (l.containsX(x)) {
						where = Point(x, start.y);
						return true;
					}
					else return false;
				}
				
			}
			if (type == VERTICAL) {
				if (l.type == VERTICAL)
					return l.start.x == start.x;
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
				double y = l.slope * start.x + l.y_icept;
				where = Point(start.x, y);
				return true;
			}

			if (l.type != NORMAL)
				return l.intersect(*this, where);
			
			assert(l.type == NORMAL && type == NORMAL);
			
			double x, y;

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
			
			if (x > start.x && x < end.x &&
					x > l.start.x && x < l.end.x) {
				where = Point(x,y);
				return true;
			}
			return false;
		}

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
			{ return Point(x, (x - start.x) * slope + start.y); }
		Point pointAtY(double y) const
			{ return Point((y - start.y) / slope + start.x, y); }

		bool containsX(double x) const {
			return x >= start.x && x <= end.x;
		}

		bool containsY(double y) const {
			if (start.y > end.y)
				return y <= start.y && y >= end.y;
			else
				return y >= start.y && y <= end.y;
		}

};

class Vector {
	public:
		double x, y;
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
