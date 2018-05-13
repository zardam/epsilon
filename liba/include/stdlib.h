#ifndef LIBA_MALLOC_H
#define LIBA_MALLOC_H

#include "private/macros.h"
#include <stddef.h>

LIBA_BEGIN_DECLS

void free(void *ptr);
void * malloc(size_t size);
void * realloc(void *ptr, size_t size);
void * bsearch (register const void *key, const void *base0,
         size_t nmemb, register size_t size,
         register int (*compar)(const void *, const void *));

void abort(void) __attribute__((noreturn));

LIBA_END_DECLS

#endif
