#include "mrb.h"

#include <clog.h>
#include <unistd.h>
#include <sys/mman.h>


struct mrb *
mrb_create(size_t size) {
    int pagesize = getpagesize();
    unsigned char *buff;
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
    int mod = size % pagesize;
    if (mod) {
        realsize = (size / pagesize + 1) * pagesize;
    }
    else {
        realsize = size;
    }

    /* Allocate the underlying backed buffer. */
    buff = mmap(
            NULL, 
            realsize * 2,
            PROT_NONE, 
            MAP_ANONYMOUS | MAP_PRIVATE,
            -1, 
            0
        );

    DEBUG("buff: %p", buff);
    return b;
}
