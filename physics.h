#ifndef PHYSICS_H
#define PHYSICS_H 1

#include <cstring> // memcpy
#include <cmath>   // sqrt

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
		
		Line(const Point &_s, const Point &_e) {
			Point s, e;
			if (s.x > e.x) {
				/* _s and _e are references, but we can't use = because it's
				 * overloaded. Ick.
				 */
				s = _e;
				e = _s;
			} else {
				s = _s;
				e = _e;
			}
			start = s;
			end = e;
				
			/* Use Cramer's law to solve for x and y intercepts.
			 * We solve for a and b in:
			 *
			 * ax1 + by1 = 1
			 * ax2 + by2 = 1
			 *
			 * And take the reciprocal of a and b. If one is zero, then
			 * we have a horizontal or vertical line.
			 */
			double d, a, b;
			if (s.x == e.x) {
				type = VERTICAL;
				x_icept = s.y;
			} else if (s.y == e.y) {
				type = HORIZONTAL;
				y_icept = s.x;
				slope = 0;
			} else {
				/* | x1 y1 |
				 * |       | = d
				 * | x2 y2 |
				 */
				d = (s.x * e.y) - (s.y * e.x);
				/* | 1  y1 |
				 * |       | = dx
				 * | 1  y2 |
				 *
				 * a = dx / d
				 */
				a = (s.y - e.y) / d;
				/* | x1  1 |
				 * |       | = dy
				 * | x2  1 |
				 *
				 * b = dy / d
				 */
				b = (s.x - e.x) / d;
				x_icept = 1/a;
				y_icept = 1/b;
				type = NORMAL;
				slope = (e.y - s.y)/(e.x - s.x);
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

		
		bool intersect(Line l, Point &where) const {
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
				if (l.containsX(start.x))
					return false;
				double y = l.slope * start.x + l.y_icept;
				where = Point(start.x, y);
				return true;
			}
			double d, dx, dy, a1, b1, a2, b2, x, y;
			/*
			 * a1x + b1y = 1
			 * a2x + b2y = 1
			 */
			a1 = 1/y_icept;
			b1 = 1/x_icept;
			a2 = 1/l.y_icept;
			b2 = 1/l.x_icept;

			d = a1 * b2 - a2 * b1;
			dx = b1 - b2;
			dy = a1 - a2;
			x = dx / d;
			y = dy / d;
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

		Point pointAtX(double x) const
			{ return Point(x, (x - start.x) * slope + start.y); }
		Point pointAtY(double y) const
			{ return Point((y - start.y) / slope + start.x, y); }

		bool containsX(double x) const {
			return x > start.x && x < end.x;
		}

		bool containsY(double y) const {
			if (start.y > end.y)
				return y < start.y && y > end.y;
			else
				return y > start.y && y < end.y;
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
