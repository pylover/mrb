#include "mrb.h"

#include <cutest.h>
#include <unistd.h>


void
test_mrb_init() {
    size_t size = getpagesize();
    struct mrb *b = mrb_create(size);

    isnotnull(b);
    isnull(b->buff);
    eqint(size, b->size);
    eqint(size, b->avail);
    eqint(0, b->used);
}


int main() {
    test_mrb_init();
    return EXIT_SUCCESS;
}
