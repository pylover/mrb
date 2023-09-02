#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <clog.h>
#include <cutest.h>

#include "mrb.h"


static int
rand_open() {
    int fd = open("/dev/urandom", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        return -1;
    }
    return fd;
}


struct mrb {
    unsigned char *buff;
    size_t size;
    int writer;
    int reader;
};


void
test_mrb_create_close() {
    size_t size = getpagesize();
    mrb_t b = mrb_create(size);

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
    eqint(0, errno);

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
    mrb_t b = mrb_create(size);
    int ufd = rand_open();

    /* Put 3 chars */
    eqint(3, mrb_put(b, "foo", 3));
    eqint(3, b->writer);
    eqint(3, mrb_used(b));
    eqint(size - 4, mrb_available(b));

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
    eqint(4095, mrb_available(b));
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
    mrb_t b = mrb_create(size);
    int ufd = rand_open();
    istrue(mrb_isempty(b));

    /* Provide some random data and put them */
    read(ufd, in, size);
    eqint(size - 1, mrb_put(b, in, size));
    eqint(4095, b->writer);
    eqint(0, b->reader);
    istrue(mrb_isfull(b));
    eqint(4095, mrb_used(b));

    /* Get all available data */
    eqint(size - 1, mrb_get(b, out, size));
    istrue(mrb_isempty(b));
    eqint(4095, b->writer);
    eqint(4095, b->reader);
    eqint(4095, mrb_available(b));
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
    mrb_t b = mrb_create(size);
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
    mrb_t b = mrb_create(size);

    /* Put 3 chars */
    eqint(3, mrb_put(b, "foo", 3));
    eqint(3, b->writer);

    /* Try to read at least 4 bytes */
    eqint(-1, mrb_getmin(b, out, 4, 10));
    eqint(3, mrb_getmin(b, out, 3, 10));

    /* Teardown */
    mrb_destroy(b);
}


struct tfile {
    FILE *file;
    int fd;
};


static struct tfile
tmpfile_open() {
    struct tfile t = {
        .file = tmpfile(),
    };

    t.fd = fileno(t.file);
    return t;
}


void
test_mrb_readin_writeout() {
    /* Setup */
    size_t size = getpagesize();
    char in[size];
    char out[size];
    mrb_t b = mrb_create(size);
    int ufd = rand_open();
    struct tfile infile = tmpfile_open();
    struct tfile outfile = tmpfile_open();

    /* Provide some random data and put them */
    read(ufd, in, size);
    write(infile.fd, in, size);
    lseek(infile.fd, 0, SEEK_SET);

    /* Read some data from fd into the buffer */
    eqint(size - 1, mrb_readin(b, infile.fd, size));
    eqint(size - 1, mrb_used(b));

    /* Write out */
    eqint(size -1, mrb_writeout(b, outfile.fd, size));
    eqint(0, mrb_used(b));

    /* Compare */
    lseek(outfile.fd, 0, SEEK_SET);
    read(outfile.fd, out, size);
    eqnstr(in, out, size);

    /* Teardown */
    close(ufd);
    fclose(infile.file);
    fclose(outfile.file);
    mrb_destroy(b);
}


int
test_mrb_search() {
    /* Setup */
    mrb_t b = mrb_create(getpagesize());
    char *key = "u";
    int startindex = 0;

    /* Compare */
    eqint(3, mrb_put(b, "foo", 3));
    eqint(-1, mrb_search(b, key, &startindex));
    eqint(3, startindex);

    eqint(6, mrb_put(b, "barbaz", 6));
    eqint(-1, mrb_search(b, key, &startindex));
    eqint(9, startindex);

    eqint(3, mrb_put(b, "qux", 3));
    eqint(10, mrb_search(b, key, &startindex));
    eqint(11, startindex);

    /* Teardown */
    mrb_destroy(b);
}


int main() {
    test_mrb_create_close();
    test_mrb_init_deinit();
    test_mrb_put_get();
    test_mrb_isfull_isempty();
    test_mrb_putall();
    test_mrb_put_getmin();
    test_mrb_readin_writeout();
    // test_mrb_search();
    return EXIT_SUCCESS;
}
