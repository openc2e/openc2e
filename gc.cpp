#include "gc.h"

#if defined(__linux__) && defined(__GNUC__)
static __thread GCPool *outer_pool = NULL;
#else
// If LINUX_TLS is not defined, do not use pools in multiple threads.
// And have a really good idea of what you're doing before release()ing or
// renew()ing in alternate threads
static GCPool *outer_pool = NULL;
#endif

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
	while (chain->_next != chain) {
		GCObject *ptr = chain->_next;
		pthread_mutex_unlock(&mutex);
		delete ptr;
		pthread_mutex_lock(&mutex);
	}
	pthread_mutex_unlock(&mutex);
}

/* vim: set noet: */
