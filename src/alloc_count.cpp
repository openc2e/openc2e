#include "alloc_count.h"

#ifdef PROFILE_ALLOCATION_COUNT

#ifndef PROFILE_ALLOCATION_THREAD_SAFE
	#define atomic_increment(v) (++(v))
	#define atomic_decrement(v) (--(v))
	#define set_if_eq(var, checkval, newval) ( ((var) == (checkval)) ? (var) = (newval), true : false )
#else
	#ifdef _WIN32

		#include <windows.h>
		#define atomic_increment(v) InterlockedIncrement(&(v))
		#define atomic_decrement(v) InterlockedDecrement(&(v))
		#define set_if_eq(v, checkval, newval) (InterlockedCompareExchange(&(v), (newval), (checkval)) == (checkval))

	#else // GNU?

		#define atomic_increment(v) __sync_add_and_fetch(&(v), 1)
		#define atomic_decrement(v) __sync_sub_and_fetch(&(v), 1)
		#define set_if_eq(v, checkval, newval) __sync_bool_compare_and_swap(&(v), (checkval), (newval))
	#endif

#endif // PROFILE_ALLOCATION_THREAD_SAFE

void AllocationCounter::increment() {
	long newCount = atomic_increment(curCount);

	long oldMax;
	do {
		oldMax = maxCount;
	} while (oldMax < newCount && !set_if_eq(maxCount, oldMax, newCount));

	atomic_increment(totalAllocs);
}

void AllocationCounter::decrement() {
	atomic_decrement(curCount);
}

void AllocationCounter::dump(std::ostream &s) {
	s << getName() << "; Live objects:" << getCount() << " Max residency: " << getMaxCount() << " Total allocation count: " << getTotalAllocs() << std::endl;
}

void AllocationCounter::walk_one(std::ostream &s) {
	dump(s);
	if (next)
		next->walk_one(s);
}

void AllocationCounter::walk(std::ostream &s) {
	if (!alloc_count_walk)
		s << "No counters registered!" << std::endl;
	else
		alloc_count_walk->walk_one(s);
}

class AllocationCounter *AllocationCounter::alloc_count_walk = NULL;

#endif // PROFILE_ALLOCATION_COUNT
