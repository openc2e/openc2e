#pragma once

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
//
// Note(2): Your class must not have a custom operator new(), as this code hooks
// it for heap usage reporting.

#ifndef PROFILE_ALLOCATION_COUNT

// to prevent bugs from occuring only with alloc profiling on, make sure the
// visibility change still occurs even if we don't do anything else
#define COUNT_ALLOC(classname) private:

#else

#include <cstdlib>
#include <iosfwd>
#include <string>

class AllocationCounter {
  protected:
	static AllocationCounter* alloc_count_walk;
#ifdef PROFILE_ALLOCATION_THREAD_SAFE
	volatile
#endif
		long curCount,
		maxCount, totalAllocs;
	AllocationCounter* next;

	void walk_one(std::ostream&);

  public:
	virtual std::string getName() const = 0;
	long getCount() const { return curCount; }
	long getMaxCount() const { return maxCount; }
	long getTotalAllocs() const { return totalAllocs; }

	void dump(std::ostream&);
	static void walk(std::ostream& s);

	AllocationCounter() {
		next = alloc_count_walk;
		alloc_count_walk = this;
		curCount = maxCount = totalAllocs = 0;
	}
	~AllocationCounter() {}
	void increment();
	void decrement();
};

template <class T>
class AllocationCounterMain : public AllocationCounter {
  protected:
	std::string getName() const { return std::string(T::at__getname()); }
};

template <class T>
class AllocationCounterHeap : public AllocationCounter {
  protected:
	std::string getName() const { return std::string(T::at__getname()) + " (heap)"; }

  public:
	static AllocationCounterHeap counter;
};

template <class T>
class AllocationToken {
  private:
	static AllocationCounterMain<T> counter;

  public:
	AllocationToken() { counter.increment(); }
	~AllocationToken() { counter.decrement(); }
};
template <class T>
AllocationCounterMain<T> AllocationToken<T>::counter;
template <class T>
AllocationCounterHeap<T> AllocationCounterHeap<T>::counter;

#define COUNT_ALLOC(classname) \
  private: \
	AllocationToken<classname> at__; \
	friend class AllocationCounterMain<classname>; \
	friend class AllocationCounterHeap<classname>; \
	static const char* at__getname() { return #classname; } \
\
  public: \
	static void* operator new(size_t len) throw(std::bad_alloc) { \
		void* p = malloc(len); \
		if (!p) \
			throw std::bad_alloc(); \
		AllocationCounterHeap<classname>::counter.increment(); \
		return p; \
	} \
	static void operator delete(void* p) throw() { \
		free(p); \
		AllocationCounterHeap<classname>::counter.decrement(); \
	} \
\
  private:

#endif // PROFILE_ALLOCATION_COUNT
