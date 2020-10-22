/*!
 * @file     test_base64.c
 * @brief    This file contains test case functions to test base64 module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "base64.h"
#include "log_util.h"
#include "memory.h"
#include "string_util.h"
#include "mcl_core/mcl_log_util.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Data to be encoded and size of data is 0.
 * WHEN  : base64_encode() is called.
 * THEN  : MCL_OK and expected encoded data with correct size are returned.
 */
void test_encode_001(void)
{
    // Variable declaration.
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!}>\"}";
    mcl_size_t data_size;
    char *encoded_data;

    data_size = string_util_strlen(data);

    // Test.
    mcl_error_t code = base64_encode(data, data_size, &encoded_data);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_MESSAGE((string_util_strlen(encoded_data) > 0) && (string_util_strlen(encoded_data) % 4 == 0), "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("eyJtZXNzYWdlIiA6ICI8e0hlbGxvIFdvcmxkIX0+In0=", encoded_data, "Expected base64 encoding not returned!");

    // Clean up.
    MCL_FREE(encoded_data);
}

/**
 * GIVEN : Data to be encoded whose size causes different padding (count of '=' at the end).
 * WHEN  : base64_encode() is called.
 * THEN  : MCL_OK and expected encoded data with correct size are returned.
 */
void test_encode_002(void)
{
    // Variable declaration.
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!!}>\"}";
    mcl_size_t data_size;
    char *encoded_data;

    data_size = string_util_strlen(data);

    // Test.
    mcl_error_t code = base64_encode(data, data_size, &encoded_data);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_MESSAGE((string_util_strlen(encoded_data) > 0) && (string_util_strlen(encoded_data) % 4 == 0), "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("eyJtZXNzYWdlIiA6ICI8e0hlbGxvIFdvcmxkISF9PiJ9", encoded_data, "Expected base64 encoding not returned!");

    // Clean up.
    MCL_FREE(encoded_data);
}

/**
 * GIVEN : Data to be encoded whose size causes different padding (count of '=' at the end).
 * WHEN  : base64_encode() is called.
 * THEN  : MCL_OK and expected encoded data with correct size are returned.
 */
void test_encode_003(void)
{
    // Variable declaration.
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!!!}>\"}";
    mcl_size_t data_size;
    char *encoded_data;

    data_size = string_util_strlen(data);

    // Test.
    mcl_error_t code = base64_encode(data, data_size, &encoded_data);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_MESSAGE((string_util_strlen(encoded_data) > 0) && (string_util_strlen(encoded_data) % 4 == 0), "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("eyJtZXNzYWdlIiA6ICI8e0hlbGxvIFdvcmxkISEhfT4ifQ==", encoded_data, "Expected base64 encoding not returned!");

    // Clean up.
    MCL_FREE(encoded_data);
}

/**
 * GIVEN : Data to be encoded  and size of data is less than original.
 * WHEN  : base64_encode() is called.
 * THEN  : MCL_OK and expected encoded data with correct size are returned.
 */
void test_encode_004(void)
{
    // Variable declaration.
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!}>\"}";
    const mcl_size_t data_size = 5;
    char *encoded_data;

    // Test.
    mcl_error_t code = base64_encode(data, data_size, &encoded_data);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_MESSAGE((string_util_strlen(encoded_data) > 0) && (string_util_strlen(encoded_data) % 4 == 0), "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("eyJtZXM=", encoded_data, "Expected base64 encoding not returned!");

    // Clean up.
    MCL_FREE(encoded_data);
}

/**
 * GIVEN : Encoded data.
 * WHEN  : base64_url_decode() is called.
 * THEN  : MCL_OK and expected decoded data with correct size are returned.
 */
void test_decode_001(void)
{
    // Variable declaration.
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!}>\"}";
    mcl_size_t data_size;
    char *encoded_data;

    data_size = string_util_strlen(data);

    // Encode data.
    mcl_error_t code = base64_url_encode(data, data_size, &encoded_data);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    // Test.
    code = base64_url_decode(encoded_data, &decoded_data, &decoded_data_size);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_MESSAGE(data_size, decoded_data_size, "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data, decoded_data, "Expected base64 decoded data not returned!");

    // Clean up.
    MCL_FREE(encoded_data);
    MCL_FREE(decoded_data);
}

/**
 * GIVEN : Encoded data whose size causes different padding (count of '=' at the end).
 * WHEN  : base64_url_decode() is called.
 * THEN  : MCL_OK and expected decoded data with correct size are returned.
 */
void test_decode_002(void)
{
    // Variable declaration.
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!!}>\"}";
    mcl_size_t data_size;
    char *encoded_data;

    data_size = string_util_strlen(data);

    // Encode data.
    mcl_error_t code = base64_url_encode(data, data_size, &encoded_data);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    // Test.
    code = base64_url_decode(encoded_data, &decoded_data, &decoded_data_size);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_MESSAGE(data_size, decoded_data_size, "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data, decoded_data, "Expected base64 decoded data not returned!");

    // Clean up.
    MCL_FREE(encoded_data);
    MCL_FREE(decoded_data);
}

/**
* GIVEN : Encoded data whose size causes different padding (count of '=' at the end).
* WHEN  : base64_url_decode() is called.
* THEN  : MCL_OK and expected decoded data with correct size are returned.
*/
void test_decode_003(void)
{
    // Variable declaration.
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!!!}>\"}";
    mcl_size_t data_size;
    char *encoded_data;

    data_size = string_util_strlen(data);

    // Encode data.
    mcl_error_t code = base64_url_encode(data, data_size, &encoded_data);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    // Test.
    code = base64_url_decode(encoded_data, &decoded_data, &decoded_data_size);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_MESSAGE(data_size, decoded_data_size, "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data, decoded_data, "Expected base64 decoded data not returned!");

    // Clean up.
    MCL_FREE(encoded_data);
    MCL_FREE(decoded_data);
}

/**
* GIVEN : Encoded data.
* WHEN  : base64_decode() is called.
* THEN  : MCL_OK and expected decoded data with correct size are returned.
*/
void test_decode_004(void)
{
    // Variable declaration.
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!}>\"}";
    mcl_size_t data_size;
    char *encoded_data;

    data_size = string_util_strlen(data);

    // Encode data.
    mcl_error_t code = base64_encode(data, data_size, &encoded_data);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    // Test.
    code = base64_decode(encoded_data, &decoded_data, &decoded_data_size);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_MESSAGE(data_size, decoded_data_size, "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data, decoded_data, "Expected base64 decoded data not returned!");

    // Clean up.
    MCL_FREE(encoded_data);
    MCL_FREE(decoded_data);
}

/**
 * GIVEN : Data encoded with base64.
 * WHEN  : base64_url_decode().
 * THEN  : MCL_BAD_CONTENT_ENCODING is returned.
 */
void test_decode_005(void)
{
    // Variable declaration.
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!}>\"}";
    mcl_size_t data_size;
    char *encoded_data;

    data_size = string_util_strlen(data);

    // Encode data with base64.
    mcl_error_t code = base64_encode(data, data_size, &encoded_data);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    // Test.
    code = base64_url_decode(encoded_data, &decoded_data, &decoded_data_size);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_BAD_CONTENT_ENCODING == code, "MCL_BAD_CONTENT_ENCODING expected as return code!");

    // Clean up.
    MCL_FREE(encoded_data);
}

/**
 * GIVEN : Empty data to be decoded and size of data is 0.
 * WHEN  : base64_url_decode() is called.
 * THEN  : MCL_BAD_CONTENT_ENCODING is returned.
 */
void test_decode_006(void)
{
    // Variable declaration.
    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    // Test.
    mcl_error_t code = base64_url_decode(MCL_NULL, &decoded_data, &decoded_data_size);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_BAD_CONTENT_ENCODING == code, "MCL_BAD_CONTENT_ENCODING expected as return code!");
}

/**
 * GIVEN : Empty data to be decoded and size of data is set to a number different than multiples of 4.
 * WHEN  : base64_url_decode() is called.
 * THEN  : MCL_BAD_CONTENT_ENCODING is returned.
 */
void test_decode_007(void)
{
    // Variable declaration.
    char *encoded_data = string_util_strdup("abc");

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    // Test.
    mcl_error_t code = base64_url_decode(encoded_data, &decoded_data, &decoded_data_size);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_BAD_CONTENT_ENCODING == code, "MCL_BAD_CONTENT_ENCODING expected as return code!");

    // Clean up.
    MCL_FREE(encoded_data);
}

/**
 * GIVEN : Empty data to be decoded and position of padding is at wrong place.
 * WHEN  : base64_decode() is called.
 * THEN  : Expected error code is asserted. Incomplete encoded data results in failure of decoding.
 */
void test_decode_008(void)
{
    // Variable declaration.
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!}>\"}";
    mcl_size_t data_size;
    char *encoded_data;

    data_size = string_util_strlen(data);

    // Encode data.
    mcl_error_t code = base64_encode(data, data_size, &encoded_data);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    encoded_data[string_util_strlen(encoded_data) - 4] = '=';

    // Test.
    code = base64_decode(encoded_data, &decoded_data, &decoded_data_size);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_BAD_CONTENT_ENCODING == code, "MCL_BAD_CONTENT_ENCODING expected as return code!");

    // Clean up.
    MCL_FREE(encoded_data);
}

/**
 * GIVEN : Data to be decoded which contains invalid count of padding char.
 * WHEN  : base64_decode() is called.
 * THEN  : MCL_BAD_CONTENT_ENCODING is returned.
 */
void test_decode_009(void)
{
    // Variable declaration.
    char *encoded_data = string_util_strdup("eyJtZ===");

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    // Test.
    mcl_error_t code = base64_decode(encoded_data, &decoded_data, &decoded_data_size);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_BAD_CONTENT_ENCODING == code, "MCL_BAD_CONTENT_ENCODING expected as return code!");

    // Clean up.
    MCL_FREE(encoded_data);
}
