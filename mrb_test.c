#include "mrb.h"

#include <cutest.h>
#include <unistd.h>


// TODO: make struct mrb opaque
void
test_mrb_create_close() {
    size_t size = getpagesize();
    struct mrb *b = mrb_create(size);

    isnotnull(b);
    isnotnull(b->buff);
    eqint(size, b->size);
    eqint(size, b->avail);
    eqint(0, b->used);

    memcpy(b->buff, "foo", 3);
    eqnstr("foo", b->buff, 3);
    mrb_close(b);
    isnull(b->buff);
}


int main() {
    test_mrb_create_close();
    return EXIT_SUCCESS;
}
