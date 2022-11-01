#ifndef MRB_H
#define MRB_H


#include <stdlib.h>


struct mrb {
    unsigned char *buff;
    size_t size;
    int writer;
    int reader;

    /* Private members */
    unsigned char *first;
    unsigned char *second;
};


int
mrb_init(struct mrb *b, size_t size);


struct mrb *
mrb_create(size_t size);


int
mrb_deinit(struct mrb *b);


int
mrb_destroy(struct mrb *b);


#endif
