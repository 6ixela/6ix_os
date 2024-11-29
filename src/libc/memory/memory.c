#include "memory.h"

void *memset(void *ptr, int value, uint32_t num)
{
    uint8_t *tmp = ptr;
    for (uint32_t i = 0; i < num; i++)
    {
        tmp[i] = (uint8_t)value;
    }
    return ptr;
}

void *memcpy(void *destination, const void *source, uint32_t num)
{
    uint8_t *dest = destination;
    const uint8_t *src = source;
    for (uint32_t i = 0; i < num; i++)
    {
        dest[i] = src[i];
    }
    return destination;
}

int memcmp(const void * ptr1, const void * ptr2, uint32_t num)
{
    const uint8_t *p1 = ptr1;
    const uint8_t *p2 = ptr2;
    for (uint32_t i = 0; i < num; i++)
    {
        if (*p1 != *p2)
        {
            return *p1 - *p2;
        }
    }
    return 0;
}