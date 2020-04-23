#pragma once

class renderable;

struct renderablezorder {
	bool operator()(const renderable *s1, const renderable *s2) const;
};