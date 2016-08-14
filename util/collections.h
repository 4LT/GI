#ifndef COLLECTIONS_H_
#define COLLECTIONS_H_

#include <stdlib.h>

/* means of freeing data in a collection */
typedef void (*free_datum_fp)(void *datum);

void free_nothing(void *datum);

#endif

