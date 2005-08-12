#include "gc.h"
#include <deque>
#include <climits>
#include <iostream>

#include <csignal>

void *last_alloc = NULL;
std::list<Collectable *> Collectable::collect_queue;

bool Collectable::gc__nowcollecting = false;

void *Collectable::operator new(size_t size) {
	void *buf = malloc(size);
	if (!buf)
		throw std::bad_alloc();
	assert(!last_alloc);
	last_alloc = buf;
	return buf;
}

void Collectable::operator delete(void *ptr) {
	Collectable *c = reinterpret_cast<Collectable *>(ptr);
	assert(c);
	assert(c->baseptr == last_alloc);
	assert(gc__nowcollecting);
	free(c->baseptr);
	last_alloc = NULL;
}

void Collectable::doCollect() {
	while (!collect_queue.empty()) {
		std::list<Collectable *>::iterator head = collect_queue.begin();
		Collectable *item = *head;
		collect_queue.erase(head);
		assert(item->refcount >= 0);
		if (item->refcount) {
			std::cerr << "WARN: gc found a live object in the collect queue, this should not happen." << std::endl << '\t'; item->dbgdump();
			continue;
		}
		if (!item->queued) {
			std::cerr << "WARN: gc found an object which thinks it's not queued, this should not happen. ptr="  << std::endl << '\t'; item->dbgdump();
			continue;
		}
		assert(!gc__nowcollecting);
		gc__nowcollecting = true;
		last_alloc = item->baseptr;
		delete item;
		assert(!last_alloc);
		gc__nowcollecting = false;
	}
}
/* vim: set noet: */
