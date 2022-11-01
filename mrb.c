#include "mrb.h"

#include <clog.h>
#include <unistd.h>
#include <sys/mman.h>


int
mrb_init(struct mrb *b, size_t size) {
    int pagesize = getpagesize();
   
    /* Calculate the real size (multiple of pagesize). */
    if (size % pagesize) {
        ERROR(
            "Invalid size: %lu, size should be multiple of pagesize, see "
            "getpagesize(2).", 
            size
        );
        return -1;
    }

    b->size = size;
    b->writer = 0;
    b->reader = 0;

    /* Create a temporary file with requested size as the backend for mmap. */
    const int fd = fileno(tmpfile());
    ftruncate(fd, b->size);
  
    /* Allocate the underlying backed buffer. */
    b->buff = mmap(
            NULL, 
            b->size * 2,
            PROT_NONE, 
            MAP_ANONYMOUS | MAP_PRIVATE,
            -1, 
            0
        );
    if (b->buff == MAP_FAILED) {
        close(fd);
        return -1;
    }

    b->first = mmap(
            b->buff, 
            b->size, 
            PROT_READ | PROT_WRITE,
            MAP_FIXED | MAP_SHARED,
            fd,
            0
        );

    if (b->first == MAP_FAILED) {
        munmap(b->buff, b->size * 2);
        close(fd);
        return -1;
    }

    b->second = mmap(
            b->buff + b->size, 
            b->size, 
            PROT_READ | PROT_WRITE,
            MAP_FIXED | MAP_SHARED,
            fd,
            0
        );
 
    if (b->second == MAP_FAILED) {
        munmap(b->buff, b->size * 2);
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}


struct mrb *
mrb_create(size_t size) {
    struct mrb *b;

    /* Allocate memory for mrb structure. */
    b = malloc(sizeof(struct mrb));
    if (b == NULL) {
        return b;
    }

    if (mrb_init(b, size)) {
        free(b);
        return NULL;
    }

    return b;
}


int
mrb_deinit(struct mrb *b) {
    /* unmap second part */
    if (munmap(b->buff + b->size, b->size)) { 
        return -1;
    }

    /* unmap first part */
    if (munmap(b->buff, b->size)) {
        return -1;
    }

    /* unmap backed buffer */
    if (munmap(b->buff, b->size * 2)) {
        return -1;
    }
    return 0;
}


int
mrb_destroy(struct mrb *b) {
    if (mrb_deinit(b)) {
        return -1;
    }
    free(b);
    return 0;
}
