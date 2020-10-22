/*!
 * @file     test_hmac.c
 * @brief    Test module for testing functions in HMAC module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "hmac.h"
#include "memory.h"
#include "string_util.h"
#include "mock_security.h"
#include "mcl_core/mcl_log_util.h"
#include <string.h>

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Data to calculate HMAC SHA256.
 * WHEN  : hmac_sha256() is called.
 * THEN  : MCL_OK and expected HMAC SHA256 with correct size are returned.
 */
void test_sha256_001(void)
{
    // Mock data.
    char *data = "ABC";

    // Mock key.
    char *key = "123";

    // Mock hash and hash size.
    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size = 0;

    // Mock sha value for first call of security_hash_sha256() function.
    mcl_uint8_t *sha_expected = MCL_MALLOC(32);

    // Mock hash value for second call of security_hash_sha256() function.
    mcl_uint8_t hash_expected_temp[32] = {0xC0, 0x55, 0x10, 0xB6, 0x7A, 0x6, 0xD7, 0x3B, 0x1, 0xEE, 0xB7, 0xD6, 0x63, 0xAB, 0x79, 0x21, 0xF, 0x70, 0x4A, 0x45, 0xDE, 0xDC, 0x1, 0x34, 0x52, 0xF7, 0xE0, 0x56, 0x30, 0x38, 0xED, 0x53};
    mcl_uint8_t *hash_expected = MCL_MALLOC(32);
    memcpy(hash_expected, hash_expected_temp, 32);

    // Mock size values.
    mcl_size_t sha_size_expected = 32;
    mcl_size_t hash_size_expected = 32;

    // Mock security_hash_sha256() function for first call.
    security_hash_sha256_ExpectAnyArgsAndReturn(MCL_OK);
    security_hash_sha256_ReturnThruPtr_hash(&sha_expected);
    security_hash_sha256_ReturnThruPtr_hash_size(&sha_size_expected);

    // Mock security_hash_sha256() function for second call.
    security_hash_sha256_ExpectAnyArgsAndReturn(MCL_OK);
    security_hash_sha256_ReturnThruPtr_hash(&hash_expected);
    security_hash_sha256_ReturnThruPtr_hash_size(&hash_size_expected);

    // Test.
    mcl_error_t code = hmac_sha256((mcl_uint8_t *)data, string_util_strlen(data), (mcl_uint8_t *)key, string_util_strlen(key), &hash, &hash_size);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(hash, "HMAC SHA256 calculation failed!");
    TEST_ASSERT_MESSAGE(32 == hash_size, "Hash size must be 32 bytes long!");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(hash_expected, hash, 32, "Hash could not be generated correctly!");

    // Clean up.
    MCL_FREE(hash);
}

/**
 * GIVEN : Data to calculate HMAC SHA256. Key size is bigger than maximum key size allowed.
 * WHEN  : hmac_sha256() is called.
 * THEN  : MCL_OK and expected HMAC SHA256 with correct size are returned.
 */
void test_sha256_002(void)
{
    // Mock data.
    char *data = "ABC";

    // Mock key.
    char *key = "0123456789012345678901234567890123456789012345678901234567890123456789";

    // Mock hash and hash size.
    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size = 0;

    // Mock key_resized value for first call of security_hash_sha256() function.
    mcl_uint8_t *key_resized_expected = MCL_MALLOC(32);

    // Mock sha value for second call of security_hash_sha256() function.
    mcl_uint8_t *sha_expected = MCL_MALLOC(32);

    // Mock hash value for third call of security_hash_sha256() function.
    mcl_uint8_t has_expected_temp[32] = { 0xC, 0xF7, 0x2F, 0xDD, 0xBD, 0x2E, 0xE9, 0x86, 0x48, 0x77, 0xB2, 0xE4, 0x6E, 0x5E, 0x2D, 0x9C, 0xFD, 0x1B, 0xF4, 0x61, 0xC2, 0x9B, 0x72, 0x88, 0x49, 0x7E, 0x7, 0x10, 0xF, 0x32, 0x87, 0x4E};
    mcl_uint8_t *hash_expected = MCL_MALLOC(32);
    memcpy(hash_expected, has_expected_temp, 32);

    // Mock size values.
    mcl_size_t key_resized_size_expected = 32;
    mcl_size_t sha_size_expected = 32;
    mcl_size_t hash_size_expected = 32;
    
    // Mock security_hash_sha256() function for first call.
    security_hash_sha256_ExpectAnyArgsAndReturn(MCL_OK);
    security_hash_sha256_ReturnThruPtr_hash(&key_resized_expected);
    security_hash_sha256_ReturnThruPtr_hash_size(&key_resized_size_expected);

    // Mock security_hash_sha256() function for second call.
    security_hash_sha256_ExpectAnyArgsAndReturn(MCL_OK);
    security_hash_sha256_ReturnThruPtr_hash(&sha_expected);
    security_hash_sha256_ReturnThruPtr_hash_size(&sha_size_expected);

    // Mock security_hash_sha256() function for third call.
    security_hash_sha256_ExpectAnyArgsAndReturn(MCL_OK);
    security_hash_sha256_ReturnThruPtr_hash(&hash_expected);
    security_hash_sha256_ReturnThruPtr_hash_size(&hash_size_expected);

    // Test.
    mcl_error_t code = hmac_sha256((mcl_uint8_t *)data, string_util_strlen(data), (mcl_uint8_t *)key, string_util_strlen(key), &hash, &hash_size);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(hash, "HMAC SHA256 calculation failed!");
    TEST_ASSERT_MESSAGE(32 == hash_size, "Hash size must be 32 bytes long!");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(hash_expected, hash, 32, "Hash could not be generated correctly!");

    // Clean up.
    MCL_FREE(hash);
}

/**
 * GIVEN : Key with size 0.
 * WHEN  : hmac_sha256() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_sha256_003(void)
{
    // Mock data.
    char *data = "ABC";

    // Mock key.
    char *key = "123";

    // Mock hash and hash size.
    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size = 0;

    // Test.
    mcl_error_t code = hmac_sha256((mcl_uint8_t *)data, string_util_strlen(data), (mcl_uint8_t *)key, 0, &hash, &hash_size);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "MCL_INVALID_PARAMETER expected as return code!");
    TEST_ASSERT_NULL_MESSAGE(hash, "Hash must be Null!");
}

/**
 * GIVEN : There is not enough memory to allocate for inner pad.
 * WHEN  : hmac_sha256() is called.
 * THEN  : MCL_OUT_OF_MEMORY is returned.
 */
void test_sha256_004(void)
{
    // Mock data.
    char *data = "ABC";

    // Mock key.
    char *key = "123";

    // Mock hash and hash size.
    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size = 0;

    // Mock security hash sha256.
    security_hash_sha256_ExpectAnyArgsAndReturn(MCL_OUT_OF_MEMORY); 

    // Test.
    mcl_error_t code = hmac_sha256((mcl_uint8_t *)data, string_util_strlen(data), (mcl_uint8_t *)key, string_util_strlen(key), &hash, &hash_size);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OUT_OF_MEMORY == code, "MCL_OUT_OF_MEMORY expected as return code!");
    TEST_ASSERT_NULL_MESSAGE(hash, "Hash must be Null!");
}

/**
 * GIVEN : There is not enough memory to allocate for outer pad.
 * WHEN  : hmac_sha256() is called.
 * THEN  : MCL_OUT_OF_MEMORY is returned.
 */
void test_sha256_005(void)
{
    // Mock data.
    char *data = "ABC";

    // Mock key.
    char *key = "123";

    // Mock hash and hash size.
    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size = 0;

    /// Mock sha value for security_hash_sha256() function.
    mcl_uint8_t *sha_expected = MCL_MALLOC(32);

    // Mock sha size.
    mcl_size_t sha_size_expected = 32;

    // Mock security_hash_sha256() function for first call.
    security_hash_sha256_ExpectAnyArgsAndReturn(MCL_OK);
    security_hash_sha256_ReturnThruPtr_hash(&sha_expected);
    security_hash_sha256_ReturnThruPtr_hash_size(&sha_size_expected);

    // Mock security_hash_sha256() function for second call.
    security_hash_sha256_ExpectAnyArgsAndReturn(MCL_OUT_OF_MEMORY);

    // Test.
    mcl_error_t code = hmac_sha256((mcl_uint8_t *)data, string_util_strlen(data), (mcl_uint8_t *)key, string_util_strlen(key), &hash, &hash_size);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OUT_OF_MEMORY == code, "MCL_OUT_OF_MEMORY expected as return code!");
    TEST_ASSERT_NULL_MESSAGE(hash, "Hash must be Null!");
}
