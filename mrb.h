#ifndef MRB_H
#define MRB_H


#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>


typedef struct mrb *mrb_t;


int
mrb_init(struct mrb *b, size_t size);


struct mrb *
mrb_create(size_t size);


int
mrb_deinit(struct mrb *b);


int
mrb_destroy(struct mrb *b);


size_t
mrb_available(struct mrb *b);


size_t
mrb_used(struct mrb *b);


size_t
mrb_put(struct mrb *b, char *source, size_t size);


int
mrb_putall(struct mrb *b, char *source, size_t size);


size_t
mrb_get(struct mrb *b, char *dest, size_t size);


size_t
mrb_softget(struct mrb *b, char *dest, size_t size, size_t offset);


ssize_t
mrb_getmin(struct mrb *b, char *dest, size_t minsize, size_t maxsize);


int
mrb_skip(struct mrb *b, size_t size);


size_t
mrb_size(struct mrb *b);


bool
mrb_isempty(struct mrb *b);


bool
mrb_isfull(struct mrb *b);


ssize_t
mrb_readin(struct mrb *b, int fd, size_t size);


ssize_t
mrb_writeout(struct mrb *b, int fd, size_t size);


ssize_t
mrb_search(struct mrb *b, const char *needle, size_t needlelen, size_t start,
        ssize_t limit);


int
mrb_print(struct mrb *b, const char *format, ...);


int
mrb_vprint(struct mrb *b, const char *format, va_list args);


#endif
