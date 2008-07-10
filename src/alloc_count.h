#ifndef ALLOC_COUNT_H
#define ALLOC_COUNT_H

// Generic structure allocation counter
//
// Usage: Place COUNT_ALLOC(classname) in your class's private members section.
// If PROFILE_ALLOCATION_COUNT is defined, DBG: SIZO will report the current
// and maximum allocation of the class.
//
// Note: if PROFILE_ALLOCATION_THREAD_SAFE is defined, this will attempt to be
// thread-safe, either with a mutex or with atomic operations where supported.
// If it is not defined, then counts may be inaccurate if objects of a given
// type are manipulated from multiple threads.

#ifndef PROFILE_ALLOCATION_COUNT

#define COUNT_ALLOC(classname)

#else

#include <iostream>
#include <boost/detail/atomic_count.hpp>

class AllocationCounter {
	protected:
		static AllocationCounter *alloc_count_walk;
#ifdef PROFILE_ALLOCATION_THREAD_SAFE
		volatile
#endif
		long curCount, maxCount, totalAllocs;
		AllocationCounter *next;

		void increment();
		void decrement();
		void walk_one(std::ostream &);

	public:
		virtual const char *getName() const = 0;
		long getCount() const { return curCount; }
		long getMaxCount() const { return maxCount; }
		long getTotalAllocs() const { return totalAllocs; }

		void dump(std::ostream &);
		static void walk(std::ostream &s);

		AllocationCounter() {
			next = alloc_count_walk;
			alloc_count_walk = this;
			curCount = maxCount = totalAllocs = 0;
		}
		~AllocationCounter() { }
		template <class T>
			friend class AllocationToken;
};

template<class T>
class AllocationCounterExt : public AllocationCounter {
	protected:
		const char *getName() const { return T::at__getname(); }
};

template<class T>
class AllocationToken {
	private:
		static AllocationCounterExt<T> counter;
	public:
		AllocationToken() { counter.increment(); }
		~AllocationToken() { counter.decrement(); }
};
template <class T>
AllocationCounterExt<T> AllocationToken<T>::counter;

#define COUNT_ALLOC(classname) AllocationToken<classname> at__; friend class AllocationCounterExt<classname>; static const char *at__getname() { return #classname; }

#endif // PROFILE_ALLOCATION_COUNT

#endif
