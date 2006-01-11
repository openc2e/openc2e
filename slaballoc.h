#ifndef SLABALLOC_H
#define SLABALLOC_H 1

#include <cassert>
#include <cstdlib>

/* Destructor callback for allocations.
 *
 * Note that this _must not_ allocate from the pool it is registered in.
 */
typedef void (*destructor_t)(void *p);

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

		static void null_destructor(void *) throw() {}
	public:
		virtual void *alloc(size_t sz, destructor_t dest = null_destructor) {
			(void)dest;
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

		virtual void release(void *p) {
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
		// call destructors! Use DestructingSlab

		virtual ~SlabAllocator() { clear(); }

		virtual void clear() {
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

		/* Some workarounds for C++ weirdness.
		 * 
		 * We stuff a pointer to the slab right before the class
		 * unconditionally. If the user doesn't pass us a slab, store
		 * NULL and use malloc. o_del detects the NULL and delegates
		 * to the appropriate releaser function.
		 *
		 * Note that C++ guarentees that the object destructor will
		 * be called before operator delete (and thus, o_del).
		 */
		static void *o_new(size_t sz, destructor_t dest, SlabAllocator &slab) {
			SlabAllocator *sa = &slab;
			size_t bsz = sizeof(sa) + sz;
			unsigned char *cp = (unsigned char *)slab.alloc(bsz, dest);
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

// Subclass to destruct members on clear() or slab destruction.
// Note that explicitly calling release won't destruct the objects
// for you.
class DestructingSlab : public SlabAllocator {
	protected:
		struct destructor_info {
			destructor_info *next;
			destructor_info *prev;
			destructor_t dest;
		};

		destructor_info *p_dest;
	public:

		DestructingSlab(size_t bm = 32) : SlabAllocator(bm) {
			p_dest = NULL;
		}
		
		virtual void *alloc(size_t sz, destructor_t dest) {
			sz += sizeof(destructor_info);
			unsigned char *cp =
				(unsigned char *)SlabAllocator::alloc(sz, SlabAllocator::null_destructor);
			destructor_info *dp = (destructor_info *)cp;

			if (p_dest && p_dest->next)
				assert(p_dest->next->prev == p_dest);

			
			dp->next = p_dest;
			if (dp->next)
				dp->next->prev = dp;
			dp->prev = NULL;
			dp->dest = dest;
			p_dest = dp;
			return (void *)(cp + sizeof (*dp));
		}

		virtual void release(void *p) {
			unsigned char *cp = (unsigned char *)p;
			cp -= sizeof (destructor_info);
			destructor_info *dp = (destructor_info *)cp;

			if (dp->next)
				dp->next->prev = dp->prev;
			if (dp->prev)
				dp->prev->next = dp->next;
			else
				p_dest = dp->next;

			SlabAllocator::release((void *)cp);
		}

		virtual void clear() {
			while (p_dest) {
				unsigned char *cp = (unsigned char *)p_dest;
				cp += sizeof(destructor_info);
				p_dest->dest((void *)cp);
				/* It'd be faster to just run all the destructors then zot the entire
				 * pool, but if any destructor frees memory in the meantime this
				 * *might* cause problems (needs more analysis).
				 *
				 * For now, release properly.
				 */
				release(p_dest);
			}
			SlabAllocator::clear();
		}
};

// Use in public class member section
#define SLAB_CLASS(c) \
	static void _SLAB_destruct(void *p) { \
		c *pc = (c *)p; \
		pc->~c(); \
	} \
	void *operator new(size_t sz, SlabAllocator &slab) { \
		return SlabAllocator::o_new(sz, _SLAB_destruct, slab); \
	} \
	void *operator new(size_t sz) { \
		return SlabAllocator::o_new(sz); \
	} \
	void operator delete(void *p) { \
		SlabAllocator::o_del(p); \
	}

#endif
