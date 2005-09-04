#include "gc.h"

static __thread GCPool *outer_pool = NULL;

GCPool *GCObject::_findPool() {
	return outer_pool;
}

GCPool::GCPool() {
	pthread_mutexattr_t ma;
	pthread_mutexattr_init(&ma);
#ifdef PTHREAD_MUTEX_ADAPTIVE_NP
	pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_ADAPTIVE_NP);
#endif
	pthread_mutex_init(&mutex, &ma);
	pthread_mutexattr_destroy(&ma);
	
	chain = new GCObject();
	chain->_next = chain->_prev = chain;
	chain->_owner = this;
	chain->_refcount = 0;
	parent = outer_pool;
	outer_pool = this;
}

GCPool::~GCPool() {
	clear();
	delete chain;
	outer_pool = parent;
}

void GCPool::clear() {
	pthread_mutex_lock(&mutex);
	GCObject *ptr = chain->_next;
	while (ptr != chain) {
		GCObject *next = ptr->_next;
		// We must manually remove it from the chain since we
		// hold the lock.
		assert(!ptr->_refcount);
		ptr->_owner = NULL;
		ptr->_next->_prev = ptr->_prev;
		ptr->_prev->_next = ptr->_next;
		ptr->_next = ptr->_prev = NULL;

		delete ptr;
		ptr = next;
	}
	pthread_mutex_unlock(&mutex);
}

/* vim: set noet: */
