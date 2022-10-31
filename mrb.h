#ifndef MRB_H
#define MRB_H


#include <stdlib.h>


struct mrb {
    unsigned char *buff;
    size_t size;
    size_t avail;
    size_t used;
};


struct mrb *
mrb_create(size_t pagesize);


void
mrb_close(struct mrb *m);


#endif
