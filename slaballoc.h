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
		virtual size_t tail_data() const { return 0; };


		struct slab_head {
			bool reserved;
			struct slab_head *next;
			size_t extent;
			size_t elements;
			size_t objsz, tail;
			SlabAllocator *owner;
		};
			
		struct alloc_head {
			slab_head *slab;
			alloc_head *next; // Overlaps data
		};

		size_t unitsz(size_t objsz) {
			size_t sz = objsz;
			// Assumption: Pointers are aligned to the same or stricter
			// boundraries than all other types
			//
			// Otherwise with objsz == 7 and sizeof slab_head * == 8
			// you'd end up with 54-byte blocks
			//
			// XXX: if there's padding after alloc_head.slab we explode
			assert((size_t)&((alloc_head *)NULL)->next == sizeof (slab_head *));
			if (sz < sizeof(slab_head *))
				sz = sizeof(slab_head *);
			else if (sz % sizeof(slab_head *))
				sz += (sizeof(slab_head *)) - (sz % sizeof(slab_head *));
			return sz;
		}

		// Find how many unitsz fit in target - sizeof slab_head
		size_t alloc_count(size_t unitsz, size_t target) {
			target -= sizeof(slab_head);
			return target / unitsz;
		}
	
		size_t count, freect, memy_usage, memy_reserved, memy_free;
		ssize_t block_mult;
		alloc_head *p_free; // A chain through free blocks
		slab_head *p_head; // A chain through the beginnings of the blocks
		// head is used for destructor freeing and clear()
		slab_head *p_bad;  // A chain through blocks which can't be reallocated
		
		
		void get_block() {
			assert(!freect);
			size_t unit = unitsz(objsz + tail_data());
			size_t items = block_mult;
			if (block_mult < 0)
				items = alloc_count(unit, -block_mult);
			else
				items = block_mult;
			size_t extent = unit * items;
			size_t alloc = sizeof(slab_head) + extent;
			void *chunk = malloc(alloc);
			if (!chunk)
				throw std::bad_alloc();
			slab_head *old_head = p_head;
			p_head = (slab_head *)chunk;
			p_head->next = old_head;
			p_head->extent = extent;
			p_head->elements = items;
			p_head->owner = this;
			p_head->reserved = false;
			p_head->objsz = objsz;
			p_head->tail = tail_data();

			unsigned char *dataspace = (unsigned char *)(p_head + 1);
			
			for (size_t i = 0; i < items; i++) {
				unsigned char *p = dataspace + i * unit;
				alloc_head *ah = (alloc_head *)p;
				ah->slab = p_head;
				ah->next = p_free;
				p_free = ah;
				freect++;
				count++;
			}

			memy_usage += alloc;
			memy_free += extent;
		}

		static void null_destructor(void *) throw() {}

	protected:
		virtual alloc_head *_alloc(size_t sz) {
			sz += sizeof(slab_head *);
			if (sz < sizeof(alloc_head))
				sz = sizeof(alloc_head);
			if (sz > objsz) {
				// libcpp is playing nasty tricks on us, blah
				freect = 0;
				p_free = NULL;
				objsz = sz;
				memy_reserved += memy_free;
				memy_free = 0;
				p_bad = p_head;

				for (slab_head *h = p_bad; h && !h->reserved; h++)
					h->reserved = true;
			}
			if (!p_free) get_block();
			assert(p_free);

			alloc_head *p = p_free;
			p_free = p->next;

			freect--;
			memy_free -= objsz;

			assert(p->slab->owner == this);
			
			return p;
		}

		virtual void _release(alloc_head *p) {
			if (p->slab->reserved)
				return;
			p->next = p_free;
			p_free = p;
			freect++;
			memy_free += objsz;
		}

	public:
		/* Allocate sz bytes of data with the dest destructor callback,
		 * and return a pointer to the buffer.
		 *
		 * Note that this class does not use dest; use DestructingSlab
		 * if you need destruction when the slab goes away.
		 */
		virtual void *alloc(size_t sz, destructor_t dest = null_destructor) {
			(void)dest;
			alloc_head *ah = _alloc(sz);
			return (void *)&ah->next;
		}
		
		/* Free the previously allocated data pointed to by p. */
		virtual void release(void *p) {
			alloc_head ahs;
			unsigned char *cp = (unsigned char *)p;
			cp -= ((unsigned char *)&ahs.next - (unsigned char *)&ahs);
			_release((alloc_head *)cp);
		}

		/* Arguments:
		 *
		 * size_t objsz_ - the expected size of an allocation in the slab
		 *   Note that, while it is not an error to allocate something
		 *   larger than this, if said allocation is not the first to be
		 *   done in the slab it will result in wasted memory until and
		 *   unless clear() is called.
		 * ssize_t bm
		 *   If bm is positive, indicates the number of objects in each slab.
		 *   If negative, indicates the target size of a slab, in bytes.
		 */
		SlabAllocator(size_t objsz_ = sizeof(void **), ssize_t bm = -4096)
			: objsz(objsz_), count(0), freect(0),
			  memy_usage(0), memy_reserved(0), memy_free(0),
			  block_mult(bm), p_free(NULL), p_head(NULL)
		  {
			  objsz += sizeof(slab_head *);
		  }

		// Note: deleting using ~SlabAllocator or clear() do _NOT_
		// call destructors! Use DestructingSlab

		virtual ~SlabAllocator() { clear(); }

		/* Free all allocations in the slab, and release all slab memory
		 * to the system.
		 */
		virtual void clear() {
			while (p_head) {
				slab_head *pp = p_head;
				p_head = pp->next;
				free((void *)pp);
			}
			freect = count = 0;
			p_free = NULL;
			p_head = NULL;
			memy_usage = memy_reserved = memy_free = 0;
		}

		size_t free_elements() const { return freect; }
		size_t total_elements() const { return count; }
		size_t used_elements() const { return count - freect; }

		size_t memory_usage() const { return memy_usage; }
		size_t memory_free() const { return memy_free; }
		size_t memory_reserved() const { return memy_reserved; }

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
			return (void *)&slab._alloc(sz)->next;
		}

		static void *o_new(size_t sz) {
			alloc_head *ah = (alloc_head *)malloc(sizeof(&ah->slab) + sz);
			ah->slab = NULL;
			return (void *)&ah->next;
		}

		static void o_del(void *p) {
			alloc_head ahs;
			unsigned char *cp = (unsigned char *)p;
			cp -= ((unsigned char *)&ahs.next - (unsigned char *)&ahs);
			alloc_head *ap = (alloc_head *)cp;
			
			if (ap->slab)
				ap->slab->owner->_release(ap);
			else
				free((void *)ap);
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

		DestructingSlab(size_t objsz_ = sizeof(void **),
					size_t bm = 32) : SlabAllocator(objsz_, bm) {
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
