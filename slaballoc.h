#ifndef SLABALLOC_H
#define SLABALLOC_H 1

#include <cassert>
#include <cstdlib>

class SlabAllocator {
	protected:

		size_t objsz;

		size_t unitsz() {
			size_t sz = objsz;
			// Assumption: void ** is aligned to the same or stricter
			// boundraries than T
			//
			// Otherwise with sizeof T == 7 and sizeof void ** == 8
			// you'd end up with 54-byte blocks
			if (sz < sizeof(void **))
				sz = sizeof(void **);
			else if (sz % sizeof(void **))
				sz += (sizeof(void **)) - (sz % sizeof(void **));
			return sz;
		}
				
		size_t count, freect, block_mult;
		void **p_free; // A chain through free blocks
		void **p_head; // A chain through the beginnings of the blocks
		// head is used for destructor freeing and clear()

		
		void get_block() {
			assert(!freect);
			size_t unit = unitsz();
			size_t alloc = sizeof(void **) + unit * block_mult;
			void *chunk = malloc(alloc);
			if (!chunk)
				throw std::bad_alloc();
			*(void **)chunk = p_head;
			p_head = (void **)chunk;
			
			for (size_t i = 0; i < block_mult; i++) {
				unsigned char *p = (unsigned char *)chunk;
				p += sizeof(void **) + i * unit;
				void **pp = (void **)p;
				*pp = this->p_free;
				this->p_free = pp;
				freect++;
				count++;
			}
		}
	public:
		void *alloc(size_t sz) {
			if (sz > this->objsz) {
				// libcpp is playing nasty tricks on us, blah
				this->freect = 0;
				this->p_free = NULL;
				this->objsz = sz;
			}
			if (!this->p_free) get_block();
			assert(this->p_free);
			
			void *p = (void *)this->p_free;
			this->p_free = (void **)*this->p_free;
			freect--;
			return p;
		}

		void release(void *p) {
			void **p_ = (void **)p;
			*p_ = this->p_free;
			this->p_free = p_;
			this->freect++;
		}

		SlabAllocator(size_t bm = 32)
			: objsz(sizeof(void **)), count(0), freect(0),
			  block_mult(bm), p_free(NULL), p_head(NULL)
			  {}

		// Note: deleting using ~SlabAllocator or clear() do _NOT_
		// call destructors (yet)!

		~SlabAllocator() { clear(); }

		void clear() {
			while (p_head) {
				void **pp = p_head;
				p_head = (void **)*pp;
				free((void *)pp);
			}
			freect = count = 0;
			p_free = p_head = NULL;
		}

		size_t free_elements() const { return freect; }
		size_t total_elements() const { return count; }
		size_t used_elements() const { return count - freect; }

		static void *o_new(size_t sz, SlabAllocator &slab) {
			SlabAllocator *sa = &slab;
			size_t bsz = sizeof(sa) + sz;
			unsigned char *cp = (unsigned char *)slab.alloc(bsz);
			*(SlabAllocator **)cp = sa;
			return (void *)(cp + sizeof sa);
		}

		static void *o_new(size_t sz) {
			SlabAllocator *sa = NULL;
			size_t bsz = sizeof(sa) + sz;
			unsigned char *cp = (unsigned char *)malloc(bsz);
			*(SlabAllocator **)cp = sa;
			return (void *)(cp + sizeof sa);
		}

		static void o_del(void *p) {
			SlabAllocator *sa;
			unsigned char *cp = (unsigned char *)p;
			cp -= sizeof sa;
			sa = *(SlabAllocator **)cp;
			if (sa)
				sa->release((void *)cp);
			else
				free((void *)cp);
		}

};

// Use in public class member section
#define SLAB_CLASS(c) \
	void *operator new(size_t sz, SlabAllocator &slab) { \
		return SlabAllocator::o_new(sz, slab); \
	} \
	void *operator new(size_t sz) { \
		return SlabAllocator::o_new(sz); \
	} \
	void operator delete(void *p) { \
		SlabAllocator::o_del(p); \
	}

#endif
