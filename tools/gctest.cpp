#include "gc.h"
#include <cstdio>

class GCTest : public GCObject {
    public:
        GCTest() {
            printf("GCTest() create, p=%p\n", (void *)this);
        }
        ~GCTest() {
            printf("GCTest() destroy, p=%p\n", (void *)this);
        }
};

int main() {
    GCPool root;
    GCTest *p = new GCTest();
    p->release();
    root.clear();
    puts("1");
    {   GCPool leaf;
        p = new GCTest();
    }
    puts("2");
    p->release();
    root.clear();
    {   GCPool leaf;
        p = new GCTest();
        p->release();
    }
    puts("3");
    root.clear();
    puts("4");
    return 0;
}
