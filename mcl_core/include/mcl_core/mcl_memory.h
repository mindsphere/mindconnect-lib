/*!
 * @file     mcl_memory.h
 * @brief    Memory module interface header file.
 *
 * This module contains functions and macro definitions for memory allocation/deallocation operations.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_MEMORY_H_
#define MCL_MEMORY_H_

#include "mcl_core/mcl_core_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * This function allocates memory.
 *
 * @param size Size of the memory to be allocated in bytes.
 * @return Pointer to the allocated memory.
 */
extern MCL_CORE_EXPORT void *mcl_memory_malloc(mcl_size_t size);

/**
 * This function allocates memory and sets it to zero.
 *
 * @param count Number of elements to be allocated. Total memory allocated will be (@p count*@p bytes)
 * @param bytes Size of each element in bytes.
 * @return Pointer to the allocated memory.
 */
extern MCL_CORE_EXPORT void *mcl_memory_calloc(mcl_size_t count, mcl_size_t bytes);

/**
 * This function reallocates memory.
 *
 * @param p Pointer to the memory to be reallocated.
 * @param bytes Size of the memory to be reallocated in bytes.
 * @return Pointer to the reallocated memory.
 */
extern MCL_CORE_EXPORT void *mcl_memory_realloc(void *p, mcl_size_t bytes);

/**
 * This function frees memory.
 *
 * @param p Pointer to the memory to be freed.
 */
extern MCL_CORE_EXPORT void mcl_memory_free(void *p);

#define MCL_MALLOC(bytes) mcl_memory_malloc(bytes)
#define MCL_NEW(p) ((p) = MCL_MALLOC((long)sizeof (*p)))
#define MCL_CALLOC(count, bytes) mcl_memory_calloc(count, bytes)
#define MCL_NEW_WITH_ZERO(p) ((p) = MCL_CALLOC(1, (long)sizeof *(p)))
#define MCL_RESIZE(p, bytes) ((p) = mcl_memory_realloc(p, bytes))
#define MCL_FREE(p) ((void)(mcl_memory_free(p), (p) = NULL))

#ifdef  __cplusplus
}
#endif

#endif //MCL_MCL_MEMORY_H_
