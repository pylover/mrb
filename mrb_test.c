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
    istrue(mrb_isempty(b));

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
    char in[size];
    char out[size];
    struct mrb *b = mrb_create(size);
    int ufd = rand_open();

    /* Put 3 chars */
    eqint(3, mrb_put(b, "foo", 3));
    eqint(3, b->writer);
    eqint(3, mrb_space_used(b));
    eqint(size - 4, mrb_space_available(b));

    /* Ger 3 chars from buffer */ 
    eqint(3, mrb_get(b, out, 3));
    eqnstr("foo", out, 3);
    eqint(3, b->writer);
    eqint(3, b->reader);

    /* Provide some random data and put them */
    read(ufd, in, size);
    eqint(size - 2, mrb_put(b, in, size - 2));
    eqint(1, b->writer);
    eqint(3, b->reader);

    /* Get all available data */
    eqint(size - 2, mrb_get(b, out, size - 2));
    eqint(1, b->writer);
    eqint(1, b->reader);
    eqint(4095, mrb_space_available(b));
    istrue(mrb_isempty(b));
    eqnstr(in, out, size - 2);

    /* Teardown */
    close(ufd);
    mrb_destroy(b);
}


void
test_mrb_isfull_isempty() {
    /* Setup */
    size_t size = getpagesize();
    char in[size];
    char out[size];
    struct mrb *b = mrb_create(size);
    int ufd = rand_open();
    istrue(mrb_isempty(b));

    /* Provide some random data and put them */
    read(ufd, in, size);
    eqint(size - 1, mrb_put(b, in, size));
    eqint(4095, b->writer);
    eqint(0, b->reader);
    istrue(mrb_isfull(b));
    eqint(4095, mrb_space_used(b));

    /* Get all available data */
    eqint(size - 1, mrb_get(b, out, size));
    istrue(mrb_isempty(b));
    eqint(4095, b->writer);
    eqint(4095, b->reader);
    eqint(4095, mrb_space_available(b));
    istrue(mrb_isempty(b));
    eqnstr(in, out, size - 1);

    /* Teardown */
    close(ufd);
    mrb_destroy(b);
}


void
test_mrb_putall() {
    /* Setup */
    size_t size = getpagesize();
    char in[size];
    struct mrb *b = mrb_create(size);
    int ufd = rand_open();

    /* Provide some random data and put them */
    read(ufd, in, size);
    eqint(-1, mrb_putall(b, in, size));
    eqint(0, mrb_putall(b, in, size - 1));

    /* Teardown */
    close(ufd);
    mrb_destroy(b);
}


void
test_mrb_put_getmin() {
    /* Setup */
    size_t size = getpagesize();
    char in[size];
    char out[size];
    struct mrb *b = mrb_create(size);
    int ufd = rand_open();

    /* Put 3 chars */
    eqint(3, mrb_put(b, "foo", 3));
    eqint(3, b->writer);

    /* Try to read at least 4 bytes */
    eqint(-1, mrb_getmin(b, out, 4, 10));
    eqint(3, mrb_getmin(b, out, 3, 10));

    /* Teardown */
    close(ufd);
    mrb_destroy(b);
}


// void
// test_mrb_readin_writeout() {
//     /* Setup */
//     size_t size = getpagesize();
//     char in[size];
//     char out[size];
//     struct mrb *b = mrb_create(size);
//     int ufd = rand_open();
// 
//     /* Put 3 chars */
//     eqint(3, mrb_put(b, "foo", 3));
//     eqint(3, b->writer);
// 
//     /* Try to read at least 4 bytes */
//     eqint(-1, mrb_getmin(b, out, 4, 10));
//     eqint(3, mrb_getmin(b, out, 3, 10));
// 
//     /* Teardown */
//     close(ufd);
//     mrb_destroy(b);
// }
/*
vrb_read
read(2) data into a VRB until EOF or full, or I/O would block.
vrb_read_min
read(2) a minimum amount of data into a VRB until EOF or full, or I/O would block.
vrb_write
write(2) data from a VRB until empty, or I/O would block.
*/

int main() {
    test_mrb_create_close();
    test_mrb_init_deinit();
    test_mrb_put_get();
    test_mrb_isfull_isempty();
    test_mrb_putall();
    test_mrb_put_getmin();
    return EXIT_SUCCESS;
}
