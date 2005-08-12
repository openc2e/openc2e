#ifndef BYTESTRING_H
#define BYTESTRING_H 1

#include <vector>
#include <cmalloc>
#include <cstring>

struct bytestr_c {
	unsigned int refcount;
	unsigned int length;
	unsigned int bs[0];
};

class bytestring {
	protected:
		bytestr_c *d;
	public:
		bytestring(const vector<unsigned int> &src) {
			d = malloc(sizeof(*d) + sizeof(unsigned int) * src.size());
			if (!d)
				throw std::bad_alloc();
			const vector<unsigned int>::iterator it = src.begin();
			int i = 0;
			while (it != src.end())
				d->bs[i++] = *it++;
			d->refcount = 1;
			d->length = src.size();
		}

		bytestring(unsigned int length, unsigned int arr[]) {
			d = malloc(sizeof (*d) + sizeof (unsigned int) * arr.size());
			if (!d)
				throw std::bad_alloc();
			d->refcount = 1;
			d->length = length;
			memcpy(d, arr, sizeof arr[0] * length);
		}

		bytestring(const bytestring &bs) {
			d = bs.d;
			d->refcount++;
		}

		~bytestring() {
			if(!--d->refcount)
				free(d);
		}

		unsigned int length() {
			return d->length;
		}

		unsigned int operator[](int idx) {
			return d->bs[idx];
		}
};
			

#endif

/* vim: set noet: */
