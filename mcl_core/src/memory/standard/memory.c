/*!
 * @file     memory.c
 * @brief    Memory module implementation file.
 *
 * This module implements mcl_core/mcl_memory.h interface with standard C library.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "memory.h"
#include <stdlib.h>

void *mcl_memory_malloc(mcl_size_t size)
{
    if (0 == size)
    {
        return NULL;
    }

    return malloc(size);
}

void *mcl_memory_calloc(mcl_size_t count, mcl_size_t bytes)
{
    if ((0 == count) || (0 == bytes))
    {
        return NULL;
    }

    return calloc(count, bytes);
}

void *mcl_memory_realloc(void *p, mcl_size_t bytes)
{
    if (0 == bytes)
    {
        free(p);
        return NULL;
    }

    return realloc(p, bytes);
}

void mcl_memory_free(void *p)
{
    free(p);
}
