#include "mrb.h"

#include <cutest.h>
#include <unistd.h>
#include <fcntl.h>


static int
rand_open() {
    int fd = open("/dev/urandom", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        return -1;
    }
    return fd;
}


void
test_mrb_create_close() {
    size_t size = getpagesize();
    struct mrb *b = mrb_create(size);

    isnotnull(b);
    isnotnull(b->buff);
    eqint(size, b->size);
    eqint(0, b->writer);
    eqint(0, b->reader);

    memcpy(b->buff, "foo", 3);
    eqnstr("foo", b->buff, 3);
    eqint(0, mrb_destroy(b));
}


void
test_mrb_init_deinit() {
    size_t size = getpagesize();
    struct mrb b;
    eqint(0, mrb_init(&b, size));

    isnotnull(b.buff);
    eqint(size, b.size);
    eqint(0, b.writer);
    eqint(0, b.reader);

    memcpy(b.buff, "foo", 3);
    eqnstr("foo", b.buff, 3);
    eqint(0, mrb_deinit(&b));
}


void
test_mrb_put_get() {
    /* Setup */
    size_t size = getpagesize();
    struct mrb *b = mrb_create(size);

    /* Put 3 chars */
    eqint(3, mrb_put(b, "foo", 3));
    eqint(3, b->writer);
    eqint(3, mrb_space_used(b));
    eqint(size - 4, mrb_space_available(b));

    /* Ger 3 chars from buffer */ 
    char data[size];
    eqint(3, mrb_get(b, data, 3));
    eqnstr("foo", data, 3);

    /* Teardown */
    mrb_destroy(b);
}


/*
Uninitialize a virtual ring buffer in a static struct.
vrb_capacity
Obtain the total buffer capacity of a VRB.
vrb_data_len
Obtain the length of data in the buffer.
vrb_data_ptr
Obtain the pointer to the data in the buffer.
vrb_space_len
Obtain the length of empty space in the buffer.
vrb_space_ptr
Obtain the pointer to the empty space in the buffer.
vrb_is_empty
Determine if the buffer is currently empty.
vrb_is_full
Determine if the buffer is currently full.
vrb_is_not_empty
Determine if there is at least some data in the buffer.
vrb_is_not_full
Determine if there is at least some empty space in the buffer.
vrb_give
Indicate how much empty space had data put in by the caller.
vrb_take
Indicate how much data in the buffer was used by the caller.
vrb_get
Copy data from the virtual ring buffer to a caller location.
vrb_get_min
Copy a minimum amount of data from the VRB only if it will fit.
vrb_put
Copy data from a caller location to the virtual ring buffer.
vrb_put_all
Copy data to the VRB only if all of it will fit.
vrb_read
read(2) data into a VRB until EOF or full, or I/O would block.
vrb_read_min
read(2) a minimum amount of data into a VRB until EOF or full, or I/O would block.
vrb_write
write(2) data from a VRB until empty, or I/O would block.
vrb_resize
Change the size of a VRB while keeping any data in the buffer.
vrb_move
Move data from one VRB to another.
*/

int main() {
    test_mrb_create_close();
    test_mrb_init_deinit();
    test_mrb_put_get();
    return EXIT_SUCCESS;
}
