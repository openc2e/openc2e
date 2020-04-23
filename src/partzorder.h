#pragma once

class CompoundPart;

struct partzorder {
	bool operator()(const CompoundPart *s1, const CompoundPart *s2) const;
};