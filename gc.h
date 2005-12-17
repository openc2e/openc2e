#ifndef GC_H
#define GC_H 1

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#ifndef _WIN32
#include <pthread.h>
#else
// TODO: evil and hacky pthread disabling
#define pthread_mutex_t int
#define pthread_mutex_lock(x)
#define pthread_mutex_unlock(x)
#endif

/* Thread-safety:
 * GCPools are tracked seperately in each thread.
 * GCObject->release, GCObject->retain are not thread-safe on the same object,
 * but are thread-safe wrt the pools involved, provided the pools are not being
 * cleared at the same time.
 *
 * In short, use your own locks around GCObject calls, and if a GCObject has a
 * refcount of zero and you want to pass it between threads, make sure it isn't
 * nuked in the meantime.
 *
 * Note: thread-safety is broken on platforms that don't do __thread ATM, sorry
 */

class GCObject;

class GCPool {
	private:
		pthread_mutex_t mutex;
		GCPool *parent;
		GCObject *chain;
		friend class GCObject;
	public:
		void clear();
		GCPool();
		~GCPool();
};

class GCObject {
	private:
		GCObject *_next, *_prev;
		GCPool *_owner;
		unsigned int _refcount;
		friend class GCPool;
		GCPool *_findPool();
	public:
		GCObject() {
			_owner = NULL;
			_next = _prev = NULL;
			_refcount = 1;
		}
		void retain() {
			if (_owner) {
				pthread_mutex_lock(&_owner->mutex);
				_next->_prev = _prev;
				_prev->_next = _next;
				pthread_mutex_unlock(&_owner->mutex);
				_owner = NULL;
			}
			_refcount++;
			assert(_refcount);
		}
		virtual ~GCObject() {
			if (!_refcount) {
				retain(); // remove ourselves from the list
			} else {
				std::cerr << "Warning: deleting live object at " << (void *)this
					<< std::endl;
			}
		}
		void release() {
			assert(_refcount);
			_refcount--;
			if (!_refcount) {
				_owner = _findPool();
				if (!_owner) {
					std::cerr << "Warning: releasing object " << (void *)this
						<< "with no pool; possible memory leak?" << std::endl;
					return;
				}
				pthread_mutex_lock(&_owner->mutex);
				_next = _owner->chain->_next;
				_prev = _owner->chain;
				pthread_mutex_unlock(&_owner->mutex);
				_next->_prev = this;
				_prev->_next = this;
			}
		}
		unsigned int getRefCount() {
			return _refcount;
		}
			
};
		

#endif

/* vim: set noet: */
