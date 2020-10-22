/*!
 * @file     test_memory_standard.c
 * @brief    Unit test cases for testing mcl_memory module standard implementation.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_core/mcl_memory.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : No initial condition.
 * WHEN  : User requests allocation of a buffer.
 * THEN  : User expects the buffer is allocated and is not NULL.
 */
void test_malloc_001(void)
{
    char test_buffer[] = "This is a test buffer.";
    char *allocated_buffer = MCL_MALLOC(sizeof(test_buffer));
    TEST_ASSERT_NOT_NULL_MESSAGE(allocated_buffer, "New memory couldn't be allocated! Buffer is NULL.");

    for (size_t i = 0; i < sizeof(test_buffer); i++)
    {
        // If allocated buffer is not large enough, there must be crash here.
        allocated_buffer[i] = test_buffer[i];
    }

    MCL_FREE(allocated_buffer);
    TEST_ASSERT_NULL_MESSAGE(allocated_buffer, "Pointer is NOT NULL after freeing!");
}

/**
 * GIVEN : No initial condition.
 * WHEN  : mcl_memory_calloc() is called.
 * THEN  : User expects the buffer is allocated and content is all zero.
 */
void test_calloc_001(void)
{
    size_t test_size = 32;
    char *allocated_buffer = mcl_memory_calloc(test_size, 1);

    for (size_t i = 0; i < test_size; i++)
    {
        TEST_ASSERT_EQUAL(0, allocated_buffer[i]);
    }

    MCL_FREE(allocated_buffer);
    TEST_ASSERT_NULL_MESSAGE(allocated_buffer, "Pointer is NOT NULL after freeing!");
}

/**
 * GIVEN : User has an allocated buffer.
 * WHEN  : mcl_memory_realloc() is called to resize the buffer.
 * THEN  : User expects the buffer to be resized.
 */
void test_realloc_001(void)
{
    char test_buffer[] = "This is a test buffer.";
    char *allocated_buffer = MCL_MALLOC(sizeof(test_buffer));
    TEST_ASSERT_NOT_NULL_MESSAGE(allocated_buffer, "New memory couldn't be allocated! Buffer is NULL.");

    for (size_t i = 0; i < sizeof(test_buffer); i++)
    {
        // If allocated buffer is not large enough, there must be crash here.
        allocated_buffer[i] = test_buffer[i];
    }

    allocated_buffer = mcl_memory_realloc(allocated_buffer, 2 * sizeof(test_buffer));

    for (size_t i = 0; i < sizeof(test_buffer); i++)
    {
        // Content must remain.
        TEST_ASSERT_EQUAL(test_buffer[i], allocated_buffer[i]);

        // If allocated buffer is not large enough, there must be crash here.
        allocated_buffer[sizeof(test_buffer) + i] = test_buffer[sizeof(test_buffer) + i];
    }

    MCL_FREE(allocated_buffer);
    TEST_ASSERT_NULL_MESSAGE(allocated_buffer, "Pointer is NOT NULL after freeing!");
}
