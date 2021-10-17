#ifndef CHEESE_H
#define CHEESE_H

class CCheese {
public:
	CCheese( CPoint const &point = CPoint( 0, 0 ) ) : m_Point( point ) {}

	CPoint GetPoint() const {return m_Point;}

private:
	CPoint m_Point;
};

#endif

