/*!
 * @file     test_http_response.c
 * @brief    Unit test cases for http_response module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_core/mcl_http_response.h"
#include "http_response.h"
#include "mcl_core/mcl_log_util.h"
#include "list.h"
#include "memory.h"
#include "string_util.h"

void setUp()
{

}

void tearDown()
{

}

/**
 * GIVEN : @p header parameter is null.
 * WHEN  : #mcl_http_response_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_001()
{
    // Variable declaration.
    mcl_uint8_t *payload = MCL_NULL;
    mcl_http_response_t *http_response = MCL_NULL;
    E_MCL_HTTP_STATUS_CODE status_code = MCL_HTTP_STATUS_CODE_SUCCESS;

    // Test.
    mcl_error_t code = mcl_http_response_initialize(MCL_NULL, payload, 5, status_code, &http_response);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "Null check for header does not work.");

    // Clean up.
    MCL_FREE(payload);
}

/**
 * GIVEN : @p http_response parameter is null.
 * WHEN  : #mcl_http_response_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002()
{
    // Variable declaration.
    mcl_list_t *headers = MCL_NULL;
    mcl_uint8_t *payload = MCL_NULL;
    E_MCL_HTTP_STATUS_CODE status_code = MCL_HTTP_STATUS_CODE_SUCCESS;

    // Test.
    mcl_error_t code = mcl_http_response_initialize(headers, payload, 5, status_code, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "Null check for http_response does not work.");

    // Clean up.
    MCL_FREE(payload);
}

/**
 * GIVEN : All parameters are valid.
 * WHEN  : #mcl_http_response_initialize() is called.
 * THEN  : MCL_OK is returned.
 */
void test_initialize_003()
{
    // String initialization.
    char *header1 = MCL_MALLOC(12);
    char *header2 = MCL_MALLOC(12);
    string_util_strncpy(header1, "key1:value1", 11);
    string_util_strncpy(header2, "key2:value2", 11);

    // List initialization.
    mcl_list_t *headers = MCL_NULL;
    mcl_list_initialize(&headers);
    mcl_list_add(headers, header1);
    mcl_list_add(headers, header2);

    // Variable declaration.
    mcl_size_t payload_size = 5;
    mcl_uint8_t *payload = (mcl_uint8_t*)MCL_MALLOC(payload_size);
    E_MCL_HTTP_STATUS_CODE status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    mcl_http_response_t *http_response = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_http_response_initialize(headers, payload, payload_size, status_code, &http_response);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_http_response_initialize() failed");
    TEST_ASSERT_EQUAL_MESSAGE(headers, http_response->header, "Field header was not set correctly.");
    TEST_ASSERT_EQUAL_MESSAGE(payload_size, http_response->payload_size, "Field payload_size was not set correctly.");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(payload, http_response->payload, payload_size, "Field payload was not set correctly.");
    TEST_ASSERT_EQUAL_MESSAGE(status_code, http_response->status_code, "Field status_code was not set correctly.");

    // Clean up.
    mcl_http_response_destroy(&http_response);
}

/**
 * GIVEN : User provides valid parameters.
 * WHEN  : #mcl_http_response_get_header() is called.
 * THEN  : Correct header value is returned.
 */
void test_get_header_001()
{
    // String initialization.
    char *header1 = MCL_MALLOC(13);
    char *header2 = MCL_MALLOC(12);
    string_util_memcpy(header1, "key1: value1", 13);
    string_util_memcpy(header2, "key2:value2", 12);

    // List initialization.
    mcl_list_t *headers = MCL_NULL;
    mcl_list_initialize(&headers);
    mcl_list_add(headers, header1);
    mcl_list_add(headers, header2);

    // Variable declaration.
    mcl_size_t payload_size = 5;
    mcl_uint8_t *payload = (mcl_uint8_t*)MCL_MALLOC(payload_size);
    E_MCL_HTTP_STATUS_CODE status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    mcl_http_response_t *http_response = MCL_NULL;

    mcl_error_t code = mcl_http_response_initialize(headers, payload, payload_size, status_code, &http_response);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_http_response_initialize() failed");

    // Get the value of the header with space after ":".
    char* header_value_1 = MCL_NULL;
    code = mcl_http_response_get_header(http_response, "key1", &header_value_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_http_response_get_header() failed");

    TEST_ASSERT_EQUAL_STRING_MESSAGE("value1", header_value_1, "Returned header was incorrect.");

    // Get the value of the header without space after ":".
    char* header_value_2 = MCL_NULL;
    code = mcl_http_response_get_header(http_response, "key2", &header_value_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_http_response_get_header() failed");

    TEST_ASSERT_EQUAL_STRING_MESSAGE("value2", header_value_2, "Returned header_2 was incorrect.");

    // Clean up.
    mcl_http_response_destroy(&http_response);
    MCL_FREE(header_value_1);
    MCL_FREE(header_value_2);
}

/**
 * GIVEN : User provides valid parameters. But header does not exist.
 * WHEN  : #mcl_http_response_get_header() is called.
 * THEN  : MCL_FAIL is returned.
 */
void test_get_header_002()
{
    // String initialization.
    char *header1 = MCL_MALLOC(12);
    char *header2 = MCL_MALLOC(12);
    string_util_strncpy(header1, "key1:value1", 11);
    string_util_strncpy(header2, "key2:value2", 11);

    // List initialization.
    mcl_list_t *headers = MCL_NULL;
    mcl_list_initialize(&headers);
    mcl_list_add(headers, header1);
    mcl_list_add(headers, header2);

    // Variable declaration.
    mcl_size_t payload_size = 5;
    mcl_uint8_t *payload = (mcl_uint8_t*)MCL_MALLOC(payload_size);
    E_MCL_HTTP_STATUS_CODE status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    mcl_http_response_t *http_response = MCL_NULL;

    // Initialize http_response object.
    mcl_error_t code = mcl_http_response_initialize(headers, payload, payload_size, status_code, &http_response);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_http_response_initialize() failed");

    // Test.
    char* header_value = MCL_NULL;
    code = mcl_http_response_get_header(http_response, "key5", &header_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_FAIL, code, "mcl_http_response_get_header() did not return MCL_FAIL when header was not found.");

    // Clean up.
    mcl_http_response_destroy(&http_response);
}

/**
 * GIVEN : @p http_response parameter is null.
 * WHEN  : #mcl_http_response_get_header() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_header_003()
{
    // Variable declaration.
    char* header_value = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_http_response_get_header(MCL_NULL, "dummy header name", &header_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_http_response_get_header() failed");
}

/**
 * GIVEN : @p header_name parameter is null.
 * WHEN  : #mcl_http_response_get_header() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_header_004()
{
    // Variable declaration.
    mcl_http_response_t http_response;
    char* header_value = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_http_response_get_header(&http_response, MCL_NULL, &header_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_http_response_get_header() failed");
}

/**
 * GIVEN : @p header_value parameter is null.
 * WHEN  : #mcl_http_response_get_header() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_header_005()
{
    // Variable declaration.
    mcl_http_response_t http_response;

    // Test.
    mcl_error_t code = mcl_http_response_get_header(&http_response, "dummy header name", MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_http_response_get_header() failed");
}
