#include "gc.h"
#include <cstdio>

class GCTest : public GCObject {
    public:
        GCTest() {
            fprintf(stderr, "GCTest() create, p=%p t=%d\n", (void *)this, (int)pthread_self());
        }
        ~GCTest() {
            fprintf(stderr, "GCTest() destroy, p=%p t=%d\n", (void *)this, (int)pthread_self());
        }
};

void *threadf(void *unused) {
    GCPool root;
    GCTest *p = new GCTest();
    GCTest *p2 = new GCTest();
    GCTest *p3 = (GCTest *)unused;
    p->release();
    p3->retain();
    p3->release();
    return (void *)p2;
}
    

int main() {
    GCPool root;
    GCTest *p = new GCTest();
    p->release();
    root.clear();
    fputs("1\n", stderr);
    {   GCPool leaf;
        p = new GCTest();
    }
    fputs("2\n", stderr);
    p->release();
    root.clear();
    {   GCPool leaf;
        p = new GCTest();
        p->release();
    }
    fputs("3\n", stderr);
    root.clear();
    fputs("4\n", stderr);
    {   GCPool leaf;
        pthread_t thread;
        p = new GCTest();
        pthread_create(&thread, NULL, threadf, (void *)p);
        p = new GCTest();
        p->release();
        leaf.clear();
        void *ret;
        pthread_join(thread, &ret);
        p = (GCTest *)ret;
        p->release();
    }
    fputs("5\n", stderr);
    return 0;
}
