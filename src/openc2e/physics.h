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
#pragma once

#include "serfwd.h"

#include <cassert>
#include <cmath> // sqrt

// OS X header files use Point and Line, so..
#define Point MyPoint
#define Line MyLine

struct Point {
	float x, y;
	Point() { x = y = 0; }
	Point(float _x, float _y)
		: x(_x), y(_y) {}
	Point(const Point& p)
		: x(p.x), y(p.y) {}

	bool operator==(const Point& p) { return x == p.x && y == p.y; }
	bool operator!=(const Point& p) { return !(*this == p); }
	Point& operator=(const Point& r) {
		x = r.x;
		y = r.y;
		return *this;
	}
};

enum linetype { NORMAL,
	HORIZONTAL,
	VERTICAL };

class Line {
  protected:
	FRIEND_SERIALIZE(Line)
	Point start, end;
	double x_icept, y_icept, slope;
	linetype type;

  public:
	void dump() const;

	Line() {
		start = Point(0, 0);
		end = Point(1, 1);
		x_icept = y_icept = 0;
		slope = 1;
		type = NORMAL;
	}

	Line(const Line& l) {
		start = l.start;
		end = l.end;
		x_icept = l.x_icept;
		y_icept = l.y_icept;
		slope = l.slope;
		type = l.type;
	}

	Line(Point s, Point e);

	Line& operator=(const Line& l) {
		start = l.start;
		end = l.end;
		x_icept = l.x_icept;
		y_icept = l.y_icept;
		slope = l.slope;
		type = l.type;
		return *this;
	}

	bool intersect(const Line& l, Point& where) const;

	linetype getType() const { return type; }
	double xIntercept() const { return x_icept; }
	double yIntercept() const { return y_icept; }
	double getSlope() const { return slope; }
	const Point& getStart() const { return start; }
	const Point& getEnd() const { return end; }

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

	Point pointAtX(double x) const {
		assert(type != VERTICAL);
		if (type == NORMAL)
			return Point(static_cast<float>(x), static_cast<float>((x - start.x) * slope + start.y));
		else
			return Point(static_cast<float>(x), start.y);
	}
	Point pointAtY(double y) const {
		assert(type != HORIZONTAL);
		if (type == NORMAL)
			return Point(static_cast<float>((y - start.y) / slope + start.x), static_cast<float>(y));
		else
			return Point(start.x, static_cast<float>(y));
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

template <class T = double>
class Vector {
  public:
	T x, y;
	Vector() {
		x = y = 0;
	}

	Vector(T _x, T _y) {
		x = _x;
		y = _y;
	}

	Vector(const Point& s, const Point& e) {
		x = e.x - s.x;
		y = e.y - s.y;
	}

	T getMagnitude() const { return sqrt(x * x + y * y); }

	Line extendFrom(const Point& p) const {
		return Line(p, Point(p.x + x, p.y + y));
	}

	Vector scaleToMagnitude(T m) const {
		return Vector(x / getMagnitude() * m, y / getMagnitude() * m);
	}

	Vector scale(T multiplier) const {
		return Vector(x * multiplier, y * multiplier);
	}

	bool extendIntersect(const Point& start, Line barrier,
		Vector& residual) const {
		Line l = extendFrom(start);
		Point i;
		if (!l.intersect(barrier, i))
			return false;
		residual = Vector(i, Point(start.x + x, start.y + y));
		return true;
	}

	T getX() const { return x; }
	T getY() const { return y; }

	Vector operator*(T m) const {
		return scale(m);
	}

	Vector operator+(const Vector& v) const {
		return Vector(x + v.x, y + v.y);
	}

	Vector operator-(const Vector& v) const {
		return Vector(x - v.x, y - v.y);
	}

	bool operator==(const Vector& v) const {
		return (x == v.x) && (y == v.y);
	}

	static Vector unitVector(T angle) {
		return Vector(cos(angle), sin(angle));
	}
};

template <class T>
Point operator+(const Vector<T>& v, const Point& p) {
	return Point(v.x + p.x, v.y + p.y);
}

template <class T>
Point operator+(const Point& p, const Vector<T>& v) {
	return v + p;
}

/* vim: set noet: */
