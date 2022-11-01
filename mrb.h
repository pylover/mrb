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


size_t
mrb_space_available(struct mrb *b);


size_t
mrb_space_used(struct mrb *b);


size_t
mrb_put(struct mrb *b, char *source, size_t size);


size_t
mrb_get(struct mrb *b, char *dest, size_t size);


#endif
