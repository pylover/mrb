#include "mrb.h"

#include <clog.h>
#include <unistd.h>
#include <sys/mman.h>


struct mrb *
mrb_create(size_t size) {
    int pagesize = getpagesize();
    struct mrb *b;
    size_t realsize;

    /* Allocate memory for mrb structure. */
    b = malloc(sizeof(struct mrb));
    if (b == NULL) {
        return b;
    }

    b->size = size;
    b->avail = size;
    b->used = 0;
   
    /* Calculate the real size (multiple of pagesize). */
    if (size % pagesize) {
        realsize = (size / pagesize + 1) * pagesize;
    }
    else {
        realsize = size;
    }

    /* Create a temporary file with requested size as the backend for mmap. */
    const int fd = fileno(tmpfile());
    ftruncate(fd, realsize);
  
    /* Allocate the underlying backed buffer. */
    b->buff = mmap(
            NULL, 
            realsize * 2,
            PROT_NONE, 
            MAP_ANONYMOUS | MAP_PRIVATE,
            -1, 
            0
        );
    if (b->buff == MAP_FAILED) {
        close(fd);
        free(b);
        return NULL;
    }

    void *first = mmap(
            b->buff, 
            realsize, 
            PROT_READ | PROT_WRITE,
            MAP_FIXED | MAP_SHARED,
            fd,
            0
        );

    if (first == MAP_FAILED) {
        munmap(b->buff, realsize * 2);
        close(fd);
        free(b);
        return NULL;
    }

    void *second = mmap(
            b->buff + realsize, 
            realsize, 
            PROT_READ | PROT_WRITE,
            MAP_FIXED | MAP_SHARED,
            fd,
            0
        );
 
    if (second == MAP_FAILED) {
        munmap(first, realsize);
        munmap(b->buff, realsize * 2);
        close(fd);
        free(b);
        return NULL;
    }

    close(fd);
    return b;
}
