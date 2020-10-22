/*!
 * @file     test_random.c
 * @brief    This file contains test case functions to test random module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_core/mcl_random.h"
#include "random.h"
#include "memory.h"
#include "string_util.h"
#include "mock_security.h"
#include "mcl_core/mcl_log_util.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : No initial condition.
 * WHEN  : random_generate_number() is called.
 * THEN  : MCL_OK and random number are returned.
 */
void test_generate_number_001()
{
    mcl_uint32_t *random_number = MCL_MALLOC(sizeof(mcl_uint32_t));
    unsigned char expected_buffer[] = { 'M', 'I', 'N', 'D'};

    security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);
    security_generate_random_bytes_ReturnArrayThruPtr_buffer(expected_buffer, sizeof(expected_buffer));

    mcl_error_t code = random_generate_number(random_number);

    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE((mcl_uint8_t *) expected_buffer, (mcl_uint8_t *) random_number, sizeof(expected_buffer), "Random number could not be generated as expected!");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK expected as return code!");

    MCL_FREE(random_number);
}

/**
 * GIVEN : No initial condition.
 * WHEN  : mcl_random_generate_guid() is called.
 * THEN  : MCL_OK and guid are returned.
 */
void test_generate_guid_001()
{
    char *guid = MCL_NULL;
    char random_buffer[] = {0xe4, 0x4c, 0xea, 0xb5, 0x5b, 0xb8, 0xe2, 0x45, 0x84, 0xff, 0xe2, 0xe6, 0xab, 0x21, 0xc7, 0x9c};
    char expected_guid[] = "b5ea4ce4-b85b-45e2-84ff-e2e6ab21c79c";

    security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);
    security_generate_random_bytes_ReturnArrayThruPtr_buffer(random_buffer, sizeof(random_buffer));

    mcl_error_t code = mcl_random_generate_guid(&guid);

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_guid, guid, "Guid could not be generated as expected!");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK expected as return code!");

    MCL_FREE(guid);
}

/**
 * GIVEN : No initial condition.
 * WHEN  : mcl_random_generate_bytes() is called.
 * THEN  : MCL_OK and random buffer are returned.
 */
void test_generate_bytes_001()
{
    unsigned char expected_buffer[] = { 'M', 'I', 'N', 'D' };
    unsigned char *buffer = MCL_MALLOC(sizeof(expected_buffer));

    security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);
    security_generate_random_bytes_ReturnArrayThruPtr_buffer(expected_buffer, sizeof(expected_buffer));

    mcl_error_t code = mcl_random_generate_bytes(buffer, sizeof(expected_buffer));

    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE((mcl_uint8_t *) expected_buffer, (mcl_uint8_t *) buffer, sizeof(expected_buffer), "Buffer for random bytes could not be generated as expected!");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK expected as return code!");

    MCL_FREE(buffer);
}
