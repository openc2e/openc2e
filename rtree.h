/*
 *  rtree.h
 *  openc2e
 *
 *  Created by Bryan Donlan on Tue 10 Jan 2006.
 *  Copyright (c) 2006 Bryan Donlan. All rights reserved.
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
#ifndef RTREE_H
#define RTREE_H 1

#include <vector>
#include <string>
#include <climits>
#include <sstream>
#include <cassert>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <list>
#include <deque>
#include <boost/lambda/lambda.hpp>
#include "slaballoc.h"
#include "openc2e.h"

// REGION_MAX should be odd, or adjust REGION_MIN manually
#define REGION_MAX 5
#define REGION_MIN (int)(REGION_MAX / 2)

typedef unsigned long long u64_t;
using namespace boost::lambda;

struct Region {
	int xmin, xmax;
	int ymin, ymax;

	Region() { xmin = xmax = ymin = ymax = 0; }
	
	template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & xmin & xmax;
			ar & ymin & ymax;
		}

	bool contains(int x, int y) const {
		return (x >= xmin && x <= xmax &&
				y >= ymin && y <= ymax
			   );
	}

	bool overlaps(const Region &r2) const {
		return (xmin <= r2.xmax && xmax >= r2.xmin)
			&& (ymin <= r2.ymax && ymax >= r2.ymin);
	}
	
	Region expand(const Region &r2) {
		Region r1 = *this;
		if (r1.xmin > r2.xmin)
			r1.xmin = r2.xmin;
		if (r1.xmax < r2.xmax)
			r1.xmax = r2.xmax;
		if (r1.ymin > r2.ymin)
			r1.ymin = r2.ymin;
		if (r1.ymax < r2.ymax)
			r1.ymax = r2.ymax;
		return r1;
	}

	u64_t area() const {
		return (u64_t)(xmax - xmin) * (u64_t)(ymax - ymin);
	}

	std::string to_s() const {
		std::ostringstream oss;
		oss << "(" << xmin << " - " << xmax << ", " << ymin << " - " << ymax << ")";
		return oss.str();
	}

	bool operator==(const Region &r2) const {
		return (xmin == r2.xmin)
			&& (xmax == r2.xmax)
			&& (ymin == r2.ymin)
			&& (ymax == r2.ymax)
			;
	}

	bool operator!=(const Region &r2) const {
		return !(*this == r2);
	}

	Region(const Region &r2)
		: xmin(r2.xmin), xmax(r2.xmax),
		  ymin(r2.ymin), ymax(r2.ymax)
	{
		assert(xmin <= xmax);
		assert(ymin <= ymax);
	}

	Region(int xmin_, int xmax_, int ymin_, int ymax_)
		: xmin(xmin_), xmax(xmax_), ymin(ymin_), ymax(ymax_)
	{
		assert(xmin <= xmax);
		assert(ymin <= ymax);
	}
};

template <class T> struct RBranch;
template <class T> struct RData;
template <class T> class RTree;

template <class T>
struct RNode {
	Region r;
	RBranch<T> *parent;
	unsigned int parent_idx;
	RTree<T> *tree;

	virtual void scan(const Region &target, std::vector<RData<T> *> &list) = 0;
	virtual RData<T> *scan(const Region &target) = 0;

	virtual RBranch<T> *chooseleaf(const Region &)
	{ assert(false); }

	RNode(const Region &r_, RTree<T> *t) : r(r_), parent(NULL), parent_idx(UINT_MAX), tree(t)
	{
	}

	virtual ~RNode() { }

	virtual std::string dump() = 0;

	virtual size_t size() const = 0; // Leaves
	virtual size_t inner_size() const = 0; // All nodes
	virtual void expensive_checks() {}
};

template <class T>
struct RBranch : public RNode<T> {
	unsigned int child_count; // : CLD_BITS;
	bool is_leaf : 1; // if true, all of children are RLeafs 
	RNode<T> *children[REGION_MAX];

	size_t sz, isz;

	virtual size_t size() const { return sz; }

	virtual size_t inner_size() const { return isz; }

	void expensive_checks() {
#ifdef RTREE_DEBUG
		Region r_ = this->r;
		for (unsigned int i = 0; i < child_count; i++) {
			assert(children[i]->parent == this);
			assert(children[i]->parent_idx == i);
			children[i]->expensive_checks();
		}
		minimize();
		if (this->r != r_) {
			std::cerr << "expensive_checks() fail, was = " << r_.to_s() << std::endl;
			std::cerr << dump();
			abort();
		}
#endif
	}

	void minimize() {
		assert(child_count);
		this->sz = children[0]->size();
		this->isz = children[0]->inner_size() + 1;
		this->r = children[0]->r;
		for (unsigned int i = 1; i < child_count; i++) {
			this->r = this->r.expand(children[i]->r);
			this->sz += children[i]->size();
			this->isz += children[i]->inner_size();
		}
	}

	void recalc_up() {
		RBranch<T> *p = this;
		while(p) {
			p->minimize();
			p = p->parent;
		}
	}
	
	void add_kid(RNode<T> *n) {
		assert(child_count < REGION_MAX);
		children[child_count] = n;
		n->parent = this;
		n->parent_idx = child_count;
		child_count++;

		recalc_up();
	}

	void drop_kid(unsigned int idx, std::deque<RNode<T> *> &reloc, RBranch<T> *&root) { // not yet used
		assert(child_count && idx < child_count);
		child_count--;
		
		for (unsigned int i = idx; i < child_count; i++) {
			children[i] = children[i + 1];
			children[i]->parent_idx = i;
		}

		if (child_count < REGION_MIN) {
			reloc.push_back(this);
			if (this->parent)
				this->parent->drop_kid(this->parent_idx, reloc, root);
			else {
				assert(root == this);
				root = NULL;
			}
			return;
		}

		recalc_up();
	}

	void add_multi(RNode<T> **n, int count) {
		assert(child_count + count <= REGION_MAX);
		for (int i = 0; i < count; i++) {
			children[child_count + i] = n[i];
			children[child_count + i]->parent = this;
			children[child_count + i]->parent_idx = child_count + i;
		}

		child_count += count;

		recalc_up();
	}

	RBranch(const Region &r, RTree<T> *t) : RNode<T>(r, t) {
		// Convenience for the main RTree initializer
		child_count = 0;
		is_leaf = true;
		this->sz = 0;
		this->isz = 1;
	}

	virtual void scan(const Region &target, std::vector<RData<T> *> &list) {
		if (!this->r.overlaps(target))
			return;
		for (unsigned int i = 0; i < child_count; i++)
			if (children[i]->r.overlaps(target))
				children[i]->scan(target, list);
	}

	virtual RData<T> *scan(const Region &target) {
		if (!this->r.overlaps(target))
			return NULL;
		for (unsigned int i = 0; i < child_count; i++) {
			if (children[i]->r.overlaps(target)) {
				RData<T> *ret = children[i]->scan(target);
				if (ret) return ret;
			}
		}
		return NULL;
	}

	virtual RBranch<T> *chooseleaf(const Region &target) {
		if (is_leaf)
			return this;
		assert(child_count);
		RBranch<T> *best = dynamic_cast<RBranch<T> *>(children[0]);
		unsigned long expansion = ULONG_MAX;

		for (unsigned int i = 0; i < child_count; i++) {
			u64_t orig_area = children[i]->r.area();
			u64_t new_area  = children[i]->r.expand(target).area();
			u64_t exp       = new_area - orig_area;
			if (exp < expansion) {
				best = dynamic_cast<RBranch<T> *>(children[i]);
				assert(best);
				expansion = exp;
			}
		}
		return best->chooseleaf(target);
	}

	void insert(RNode<T> *node, RBranch<T> *&root) {
		if (is_leaf)
			assert(!dynamic_cast<RBranch<T> *>(node));
		else
			assert(dynamic_cast<RBranch<T> *>(node));
		
		if (child_count == REGION_MAX)
			split(root);
		add_kid(node);
		expensive_checks();
	}

	void divide(std::list<RNode<T> *> &inlist, std::list<RNode<T> *> &node1, std::list<RNode<T> *> &node2) {
		// quadratic time split
		// Described in Guttman, A. 1988. R-trees: a dynamic index structure for spatial searching.
		// section 3.5.2
		
		// PickSeeds
		size_t orig_size = inlist.size();

		assert(!node1.size() && !node2.size());
		
		typename std::list<RNode<T> *>::iterator best1, best2;
		u64_t best_d = 0;
		typename std::list<RNode<T> *>::iterator i1, i2;

		for (i1 = inlist.begin(); i1 != inlist.end(); i1++) {
			for (i2 = inlist.begin(); i2 != inlist.end(); i2++) {
				if (i1 == i2)
					continue;
				Region j = (*i1)->r.expand((*i2)->r);
				u64_t d = j.area() - (*i1)->r.area() - (*i2)->r.area();
				if (d >= best_d) {
					best_d = d;
					best1 = i1;
					best2 = i2;
				}
			}
		}

		Region r1((*best1)->r);
		Region r2((*best2)->r);
		
		node1.push_back(*best1);
		node2.push_back(*best2);
		inlist.erase(best1);
		inlist.erase(best2);


		while (inlist.size()) {
			if (inlist.size() + node1.size() == REGION_MIN) {
				std::copy(inlist.begin(), inlist.end(), std::inserter(node1, node1.end()));
				inlist.clear();
				return;
			}
			if (inlist.size() + node2.size() == REGION_MIN) {
				std::copy(inlist.begin(), inlist.end(), std::inserter(node2, node2.end()));
				inlist.clear();
				return;
			}
			// PickNext
			typename std::list<RNode<T> *>::iterator best;
			u64_t best_diff = 0;

			for (typename std::list<RNode<T> *>::iterator i = inlist.begin(); i != inlist.end(); i++) {
				Region &r = (*i)->r;
				u64_t d1 = r1.expand(r).area() - r1.area();
				u64_t d2 = r2.expand(r).area() - r2.area();
				u64_t diff = (d1 < d2) ? d2 - d1 : d1 - d2;
				if (diff >= best_diff) {
					best = i;
					best_diff = diff;
				}
			}

			Region &r = (*best)->r;
			u64_t d1 = r1.expand(r).area() - r1.area();
			u64_t d2 = r2.expand(r).area() - r2.area();
			int choice = 0; // node1, node2

			if (d1 > d2)
				choice = 2;
			else if (d2 > d2)
				choice = 1;
			else if (r1.area() > r2.area())
				choice = 2;
			else if (r2.area() > r1.area())
				choice = 1;
			else if (node1.size() > node2.size())
				choice = 2;
			else if (node2.size() > node1.size())
				choice = 2;
			else choice = 1; // arbitrary tiebreaker
			
			assert(choice);
			if (choice == 1) {
				r1 = r1.expand(r);
				node1.push_back(*best);
			} else {
				r2 = r2.expand(r);
				node2.push_back(*best);
			}

			inlist.erase(best);
		}

		assert(orig_size == node1.size() + node2.size());
	}

	void split(RBranch<T> *&root) {
		assert(child_count == REGION_MAX);
		
		unsigned int split = child_count / 2;
		RBranch<T> *newbranch = new(this->tree->branches) RBranch<T>(children[split]->r, this->tree);

		newbranch->is_leaf = this->is_leaf;

		std::list<RNode<T> *> l, n1, n2;
		std::copy(children, children + child_count, std::inserter(l, l.end()));
		assert(l.size() == child_count);
		divide(l, n1, n2);
		assert(l.size() == 0);
		assert(n1.size() + n2.size() == REGION_MAX);

		std::copy(n1.begin(), n1.end(), children);
		std::copy(n2.begin(), n2.end(), newbranch->children);
		
		child_count = n1.size();
		newbranch->child_count = n2.size();

		for (unsigned int i = 0; i < child_count; i++) {
			children[i]->parent = this;
			children[i]->parent_idx = i;
		}
		
		for (unsigned int i = 0; i < newbranch->child_count; i++) {
			newbranch->children[i]->parent = newbranch;
			newbranch->children[i]->parent_idx = i;
		}
		
		minimize();
		newbranch->minimize();
		
		if (!this->parent) { // Need to make a new root!
			root = new(this->tree->branches) RBranch<T>(this->r, this->tree);
			root->is_leaf = false;
			root->insert(this, root);
		}

		this->parent->insert(newbranch, root);
		
//		std::cerr << "Split! After split, myct " << this->child_count <<
//			" newbranch " << newbranch->child_count << " root " << root->child_count << std::endl;

		expensive_checks();
		root->expensive_checks();
	}

	virtual std::string dump() {
		assert(child_count);
		Region test(children[0]->r);
		std::ostringstream oss;
		oss << "RBranch " << this->r.to_s() << " {" << std::endl;
		for (unsigned int i = 0; i < child_count; i++) {
			oss << "#" << i << " " << children[i]->dump() << std::endl;
			test = test.expand(children[i]->r);
		}
		if (test == this->r)
			oss << "OK ";
		else {
			Region test2(test);
			oss << "NOK " << test.to_s();
			test2.expand(this->r);
			if (test2 != test) oss << " NOTMIN ";
		}
		oss << "}" << std::endl;
		return oss.str();
	}

	SLAB_CLASS(RBranch<T>)
};

template<class T>
struct RData : public RNode<T> {
	T obj;

	virtual size_t size() const { return 1; }

	virtual size_t inner_size() const { return 1; }
	
	virtual void scan(const Region &target, std::vector<RData<T> *> &list) {
		if (!this->r.overlaps(target))
			return;
		list.push_back(this);
	}
	virtual RData<T> *scan(const Region &target) {
		if (this->r.overlaps(target))
			return this;
		return NULL;
	}

	RData(const Region &r_, RTree<T> *t, const T &obj_) : RNode<T>(r_, t), obj(obj_) { }

	std::string dump() {
		std::ostringstream oss;
		oss << "RData " << this->r.to_s() << " -> " << /* obj.to_s() */ (void *)&obj << std::endl;
		return oss.str();
	}

	SLAB_CLASS(RData<T>)
};

template <class T>
class RTree {
	protected:
		RBranch<T> *root;
		SlabAllocator branches;
		DestructingSlab leaves;
	public:

		void free_node(RNode<T> *n) { delete n; }
		
		friend class ptr;
		friend class RBranch<T>;

		class ptr {
			// Wrapper to enforce encapsulation
			protected:
				RData<T> *node;
				RTree<T> *tree;
			public:
				const Region &region() const { assert(node); return node->r; }
				T &data() { assert(node); return node->obj; }
			protected:
				friend std::vector<ptr> RTree::find(const Region &r);
				ptr(RData<T> *n, RTree<T> *t) : node(n), tree(t) { }
				ptr(RData<T> &n, RTree<T> *t) : node(&n), tree(t) {}
			public:
				ptr(const ptr &p) : node(p.node), tree(p.tree) {}
				void erase() {
					std::deque<RNode<T> *> reloc;
					assert(node && node->parent);
					node->parent->drop_kid(node->parent_idx, reloc, tree->root);
					tree->free_node(node);

					while (!reloc.empty()) {
						RNode<T> *node = reloc.front();
						reloc.pop_front();
						RBranch<T> *br = dynamic_cast<RBranch<T> *>(node);
						if (br) {
							// This is probably slightly inefficient but should work
							for (unsigned int i = 0; i < br->child_count; i++) {
								reloc.push_back(br->children[i]);
							}
							tree->free_node(br);
						} else {
							if (!tree->root)
								tree->root = new(this->tree->branches) RBranch<T>(node->r, tree);
							tree->root->chooseleaf(node->r)->insert(node, tree->root);
						}
					}
				}
		};
		
						
		RTree() : root(NULL) {}
		
		void insert(const Region &r, const T &val) {
			RData<T> *node = new(leaves) RData<T>(r, this, val);
			if (!root)
				root = new(branches) RBranch<T>(r, this);
			root->chooseleaf(r)->insert(node, root);
			check();
		}
		std::vector<ptr> find(const Region &r) {
			if (!root) return std::vector<ptr>();
			std::vector<RData<T> *> list;
			root->scan(r, list);

			typename std::vector<RData<T> *>::iterator i = list.begin();
			std::vector<ptr> out;
			while (i != list.end()) {
				out.push_back(ptr(*i, this));
				i++;
			}

			return out;
		}

		std::vector<ptr> find(int x, int y) {
			Region r(x, y, x, y);
			return find(r);
		}

		T *find_one(const Region &r) {
			if (!root) return NULL;
			
			RData<T> *ret = root->scan(r);
			if (!ret)  return NULL;
			return &ret->obj;
		}

		std::string dump() {
			if (!root)
				return "NULL";
			return root->dump();
		}

		
		template<class Archive>
			void save(Archive & ar, const unsigned int version) const {
				std::vector<RData<T> *> l;
				Region za_warudo(INT_MIN, INT_MIN, INT_MAX, INT_MAX);
				if (root)
					root->scan(za_warudo, l);
				size_t len = l.size();
				assert(len == size());
				ar & len;
				std::for_each(l.start(), l.end(),
						ar & (*_1)->* &RData<T>::r & (*_1)->* &RData<T>::obj);
			}

		template<class Archive>
			void load(Archive & ar, const unsigned int version) {
				leaves.clear();
				branches.clear();
				root = NULL;

				size_t len;
				ar & len;
				
				for (size_t i = 0; i < len; i++) {
					Region r;
					T obj;

					ar & r & obj;
					insert(r, obj);
				}
			}

		void check() { if (root) root->expensive_checks(); }
		size_t size() const { return root ? root->size() : 0; }
		size_t inner_size() const { return root ? root->inner_size() : 0; }
};

#endif
// vim: set ts=4 tw=4 noet :
