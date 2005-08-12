#ifndef GC_H
#define GC_H 1

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <list>

// Do _NOT_ exception out of a Collectable constructor.
class Collectable {
	private:
		void *baseptr;
		static void *last_alloc;
		
		static std::list<Collectable *> collect_queue;
		int refcount;
		std::list<Collectable *>::iterator sched;
		bool queued;
		static bool gc__nowcollecting;
	public:
		
		static void doCollect();
		
		inline void dbgdump() {
#ifdef GC_DEBUG
			std::cerr << "refcount=" << refcount << " queued " << queued << " nc " << gc__nowcollecting << " ptr " << (void *) this << " base " << (void *)baseptr << std::endl;
#endif
		}
		
		inline void release() {
#ifdef GC_DEBUG
			std::cerr << "GC: release "; dbgdump();
#endif
			assert(refcount > 0);
			if (!--refcount && !queued) {
				sched = collect_queue.insert(collect_queue.end(), this);
				queued = true;
			}
#ifdef GC_DEBUG
			std::cerr << "GC: after release "; dbgdump();
#endif
		}

		void retain() {
#ifdef GC_DEBUG
			std::cerr << "GC: retain "; dbgdump();
#endif
			assert(++refcount);
			if (queued) {
				collect_queue.erase(sched);
				sched = collect_queue.end();
				queued = false;
			}
#ifdef GC_DEBUG
			std::cerr << "GC: after retain "; dbgdump();
#endif
		}
		Collectable() : refcount(1), queued(false) {
			assert(last_alloc);
			baseptr = last_alloc;
			last_alloc = NULL;
#ifdef GC_DEBUG
			std::cerr << "GC alloc ptr "; dbgdump();
#endif
			release();
#ifdef GC_DEBUG
			std::cerr << "post initial release "; dbgdump();
#endif
		}
		virtual ~Collectable() {
			assert(last_alloc == baseptr);
#ifdef GC_DEBUG
			std::cerr << "GC prefree ptr "; dbgdump();
#endif
			assert(gc__nowcollecting);
			assert(!refcount);
			assert(queued);
			refcount = -400;
#ifdef GC_DEBUG
			std::cerr << "GC free ptr " << (void*) this << std::endl;
#endif
		}
};
		

#endif

/* vim: set noet: */
