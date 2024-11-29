#ifndef MEMORY_H
#define MEMORY_H

#include "stdint.h"

void *memset(void *ptr, int value, uint32_t num);
void *memcpy(void *destination, const void *source, uint32_t num);
int memcmp(const void * ptr1, const void * ptr2, uint32_t num);

#endif /* MEMORY_H */
