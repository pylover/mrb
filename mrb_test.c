#include "mrb.h"

#include <cutest.h>
#include <unistd.h>


void
test_mrb_init() {
    int pagesize = getpagesize();
    struct mrb *b = mrb_create(pagesize);

    isnotnull(b);
    eqint(pagesize, b->size);
    eqint(pagesize, b->avail);
    eqint(0, b->used);
}


int main() {
    test_mrb_init();
    return EXIT_SUCCESS;
}
