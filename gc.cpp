#include "gc.h"

// static __thread GCPool *outer_pool = NULL; <- more unportable code from bd_, it seems
static GCPool *outer_pool = NULL;

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
