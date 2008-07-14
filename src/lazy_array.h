/*
 *  lazy_array.h
 *  openc2e
 *
 *  Created by Bryan Donlan on Fri June 11, 2008.
 *  Copyright (c) 2008 Bryan Donlan. All rights reserved.
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

#ifndef LAZY_ARRAY_H
#define LAZY_ARRAY_H

#include <new>
#include <algorithm>
#include <cassert>
#include "boost/aligned_storage.hpp"
#include "boost/type_traits/alignment_of.hpp"

template<class T>
struct null_init_helper
{
	static inline void init(T &) { }
};

template<class T, int size, class init_helper>
class lazy_array
{
	protected:
		boost::aligned_storage<size * sizeof(T), boost::alignment_of<T>::value > storage;
		int allocated;
		inline T *lookup(int i) {
			assert(i >= 0 && i < size);
			return &(reinterpret_cast<T *>(storage.address())[i]);
		}
		inline void construct(T *p) {
			new((void *)p) T();
			init_helper::init(*p);
		}
		inline void destruct(T *p) {
			p->~T();
		}
	public:
		void ensure(int sz) {
			assert(sz <= size);
			assert(sz >= 0);
			if (allocated >= sz)
				return; // nothing to do
			for (int i = allocated; i < sz; i++)
				construct(lookup(i));
			allocated = sz;
		}
		void reduce(int sz) {
			if (sz >= allocated) {
				ensure(sz);
				return;
			}
			assert(sz >= 0);
			for (int i = sz; i < allocated; i++)
				destruct(lookup(i));
			allocated = sz;
		}

		T &operator[](int i) {
			assert(i < allocated);
			return *lookup(i);
		}
		~lazy_array() {
			reduce(0);
		}
		lazy_array() {
			allocated = 0;
		}
		lazy_array(lazy_array<T, size, init_helper> &other) {
			allocated = 0;
			*this = other;
		}
		void swap(lazy_array<T, size, init_helper> &other) {
			if (other.allocated > allocated)
				ensure(other.allocated);
			else
				other.ensure(allocated);
			for (int i = 0; i < allocated; i++)
				std::swap((*this)[i], (*other)[i]);
		}
		lazy_array<T, size, init_helper> &operator=(lazy_array<T, size, init_helper> &other) {
			reduce(0);
			ensure(other.allocated);
			for (int i = 0; i < allocated; i++)
				(*this)[i] = other[i];
			return *this;
		}
};

namespace std {
	template<class T, int size, class init_helper>
	void swap(lazy_array<T, size, init_helper> &a, lazy_array<T, size, init_helper> &b) {
		a.swap(b);
	}
}
#endif
