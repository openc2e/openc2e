#include "physics.h"

Point operator+(const Vector &v, const Point &p) {
	return Point(p.x + v.x, p.y + v.y);
}
		
Point operator+(const Point &p, const Vector &v)  {
	return Point(p.x + v.x, p.y + v.y);
}

/*
 * collidePoints
 *
 * pass it the beginning and end points of two line segments
 * it will return true if they collide, and false otherwise
 * 
 * getCollisionX() and getCollisionY() will give you the point
 * we haven't entirely checked if points are right if you pass
 * two segments of a same line
 *
 * written in a few hours of equation-driven insanity by fuzzie
 * and bz2 on 8th Feb 2005
 *
 * TODO:
 *
 * we can precalculate some 'a' line segments because they're already known
 * (eg, room boundaries) .. so we should do this
*/
bool physicsHandler::collidePoints(float ax1, float ay1, float ax2, float ay2,
		float bx1, float by1, float bx2, float by2) {

	float am, ab, bm, bb;

	bool gotx = false;
	float x, y;

	if ((ax1 - ax2) != 0) {
		if (ax1 == 0) {
			ab = ((ax2 * ay1) - (ax1 * ay2)) / (ax2 - ax1);
			am = (ay2 - ab) / ax2;
		} else {
			ab = ((ax1 * ay2) - (ax2 * ay1)) / (ax1 - ax2);
			am = (ay1 - ab) / ax1;
		}
	} else {
		gotx = true;
		x = ax1;
	}

	if ((bx1 - bx2) != 0) {
		if (bx1 == 0) {
			bb = ((bx2 * by1) - (bx1 * by2)) / (bx2 - bx1);
			bm = (by2 - bb) / bx2;
		} else {
			bb = ((bx1 * by2) - (bx2 * by1)) / (bx1 - bx2);
			bm = (by1 - bb) / bx1;
		}
		if(gotx)
			y = (bm * x) + bb;
	} else {
		if (gotx) {
			if(ax1 == bx1) {
				float higha = (ay1 > ay2 ? ay1 : ay2);
				float highb = (by1 > by2 ? by1 : by2);
				float lowa = (ay1 < ay2 ? ay1 : ay2);
				float lowb = (by1 < by2 ? by1 : by2);
				if(higha >= highb && lowa <= highb) { finalx = ax1; finaly = highb; return true; }
				else if(highb >= higha && lowb <= higha) { finalx = ax1; finaly = lowb; return true; }
				return false;
			} else
				return false;
		} else {
			gotx = true;
			x = bx1;
			y = (am * x) + ab;
		}
	}

	if(! gotx) {
		if(bm == am) {
			if(bb == ab) {
				float higha = (ax1 > ax2 ? ax1 : ax2);
				float highb = (bx1 > bx2 ? bx1 : bx2);
				float lowa = (ax1 < ax2 ? ax1 : ax2);
				float lowb = (bx1 < bx2 ? bx1 : bx2);
				if(higha >= highb && lowa <= highb) finalx = highb;
				else if(highb >= higha && lowb <= higha) finalx = lowb;
				else return false;
				finaly = (am * finalx) + ab;
				return true;
			} else {
				return false;
			}
		}
		y = ((bm * ab) - (bb * am)) / (bm - am);
		x = (y - ab) / am;
	}

	if (ay1 > ay2) {
		if (y < ay2) return false;
		if (y > ay1) return false;
	} else {
		if (y > ay2) return false;
		if (y < ay1) return false;
	}

	if (by1 > by2) {
		if (y < by2) return false;
		if (y > by1) return false;
	} else {
		if (y > by2) return false;
		if (y < by1) return false;
	}

	finalx = x;
	finaly = y;

	//cout << "collision at (" << x << ", " << y << ")\n";

	return true;
}

#if 0
#include <iostream>
#include <assert.h>
using namespace std;

// test code
int main() {
	physicsHandler p;
	
	// Single horizontal line, not overlapping
	assert(! p.collidePoints(0, 2, 2, 2, 3, 2, 4, 2));
	// Single horizontal line, overlapping
	assert(p.collidePoints(1, 2, 3, 2, 2, 2, 0, 2));
	cout << "collision at (" << p.getCollisionX() << ", " << p.getCollisionY() << ")\n";
	// Parallel vertical lines
	assert(! p.collidePoints(1, 4, 1, 0, 2, 4, 2, 0));
	// Parallel horizontal lines
	assert(! p.collidePoints(0, 2, 2, 2, 1, 3, 2, 3));
	// Single vertical line, overlapping
	assert(p.collidePoints(0, 5, 0, 0, 0, 1, 0, 2));
	cout << "collision at (" << p.getCollisionX() << ", " << p.getCollisionY() << ")\n";
	// Single vertical line, not overlapping
	assert(! p.collidePoints(0, 1, 0, 2, 0, 4, 0, 5));
	// Single diagonal line, not overlapping
	assert(! p.collidePoints(0, 0, 2, 2, 3, 3, 4, 4));
	// Single diagonal line, overlapping
	assert(p.collidePoints(2, 4, 4, 2, 3, 3, 5, 1));
	cout << "collision at (" << p.getCollisionX() << ", " << p.getCollisionY() << ")\n";
	// Parallel diagonal lines
	assert(! p.collidePoints(2, 2, 0, 4, 2, 4, 4, 2));
	// Colliding diagonal lines
	assert(p.collidePoints(0, 0, 4, 4, 4, 0, 0, 4));
	cout << "collision at (" << p.getCollisionX() << ", " << p.getCollisionY() << ")\n";
	// Non-colliding diagonal lines
	assert(! p.collidePoints(2, 1, 1, 4, 3, 1, 4, 4));
}
#endif

