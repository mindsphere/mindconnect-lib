/*!
 * @file     test_http_request.c
 * @brief    This file implements test cases for http request module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "http_request.h"
#include "file_util.h"
#include "definitions.h"
#include "string_util.h"
#include "list.h"
#include "memory.h"
#include "mcl_core/mcl_http_request.h"
#include "mcl_core/mcl_file_util.h"
#include "mcl_core/mcl_core_common.h"
#include "mcl_core/mcl_log_util.h"

static mcl_size_t dummy_callback(void *buffer, mcl_size_t size, mcl_size_t count, void *file_descriptor);

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : http_request parameter is null.
 * WHEN  : #mcl_http_request_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_001(void)
{
    mcl_error_t code;
    code = mcl_http_request_initialize(MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "MCL_TRIGGERED_WITH_NULL should have been returned.");
}

/**
 * GIVEN : Valid http_request parameter.
 * WHEN  : #mcl_http_request_initialize() is called.
 * THEN  : MCL_OK is returned.
 */
void test_initialize_002(void)
{
    mcl_error_t code;
    mcl_http_request_t *http_request = MCL_NULL;

    // Initialize http_request object.
    code = mcl_http_request_initialize(&http_request);

    // Test.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_http_request_initialize() failed.");
    TEST_ASSERT_NOT_NULL_MESSAGE(http_request, "Http request should not be NULL.");
    TEST_ASSERT_EQUAL_MESSAGE(0, http_request->payload_size, "Payload size should be 0.");
    TEST_ASSERT_NULL_MESSAGE(http_request->uri, "Http request URI should be NULL.");
    TEST_ASSERT_NOT_NULL_MESSAGE(http_request->header, "Header list should not be NULL.");
    TEST_ASSERT_NULL_MESSAGE(http_request->stream_data, "Stream data should be NULL.");
    TEST_ASSERT_NULL_MESSAGE(http_request->stream_callback, "Stream callback should be NULL.");

    // Destroy http_request object.
    mcl_http_request_destroy(&http_request);
}

/**
 * GIVEN : @p http_request parameter is null.
 * WHEN  : #mcl_http_request_add_header() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_header_001(void)
{
    mcl_error_t code;
    char *header_name = "Content Type";
    char *header_value = "multipart/mixed;boundary=gc0p4Jq0M2Yt08jU534c0p";

    code = mcl_http_request_add_header(MCL_NULL, header_name, header_value);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "MCL_TRIGGERED_WITH_NULL should have been returned.");
}

/**
 * GIVEN : @p header_name parameter is null.
 * WHEN  : #mcl_http_request_add_header() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_header_002(void)
{
    mcl_error_t code;
    char *header_value = "multipart/mixed;boundary=gc0p4Jq0M2Yt08jU534c0p";
    mcl_http_request_t *http_request = MCL_NULL;

    // Initialize http_request object.
    mcl_http_request_initialize(&http_request);

    // Test.
    code = mcl_http_request_add_header(http_request, MCL_NULL, header_value);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "MCL_TRIGGERED_WITH_NULL should have been returned.");

    // Destroy http_request object.
    mcl_http_request_destroy(&http_request);
}

/**
 * GIVEN : @p header_value parameter is null.
 * WHEN  : #mcl_http_request_add_header() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_header_003(void)
{
    mcl_error_t code;
    char *header_name = "Content Type";
    mcl_http_request_t *http_request = MCL_NULL;

    // Initialize http_request object.
    mcl_http_request_initialize(&http_request);

    // Test.
    code = mcl_http_request_add_header(http_request, header_name, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "MCL_TRIGGERED_WITH_NULL should have been returned.");

    // Destroy http_request object.
    mcl_http_request_destroy(&http_request);
}

/**
 * GIVEN : Initialized @p http_request, valid @p header_name and valid @p header_value.
 * WHEN  : #mcl_http_request_add_header() is called two times.
 * THEN  : MCL_OK is returned and the header lines are added to the @p http_request object.
 */
void test_add_header_004(void)
{
    mcl_error_t code;
    mcl_http_request_t *http_request = MCL_NULL;

    // Initialize http_request object.
    mcl_http_request_initialize(&http_request);

    // Add first header.
    code = mcl_http_request_add_header(http_request, "Content Type", "multipart/mixed;boundary=gc0p4Jq0M2Yt08jU534c0p");
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_http_request_add_header() failed at first call.");

    // Add second header.
    code = mcl_http_request_add_header(http_request, "dummy header name", "dummy header value");
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_http_request_add_header() failed at second call.");

    mcl_list_reset(http_request->header);
    mcl_list_node_t *first_header_node = MCL_NULL;
    mcl_list_next(http_request->header, &first_header_node);

    // Variable declaration.
    char *first_header_line = (char *)first_header_node->data;
    char *second_header_line = (char *)first_header_node->next->data;

    // Test.
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Content Type: multipart/mixed;boundary=gc0p4Jq0M2Yt08jU534c0p", first_header_line,
                                     "Header line can not be added to string array of header lines.");

    TEST_ASSERT_EQUAL_STRING_MESSAGE("dummy header name: dummy header value", second_header_line,
                                     "Header line can not be added to string array of header lines.");

    // Destroy http_request object.
    mcl_http_request_destroy(&http_request);
}

/**
 * GIVEN : @p http_request parameter is null.
 * WHEN  : #mcl_http_request_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_001(void)
{
    mcl_error_t code;
    mcl_size_t body_size = 100;
    code = mcl_http_request_set_parameter(MCL_NULL, MCL_HTTP_REQUEST_PARAMETER_BODY_SIZE, &body_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "MCL_TRIGGERED_WITH_NULL should have been returned.");
}

/**
* GIVEN : @p value parameter is null.
* WHEN  : #mcl_http_request_set_parameter() is called.
* THEN  : MCL_TRIGGERED_WITH_NULL is returned.
*/
void test_set_parameter_002(void)
{
    mcl_error_t code;
    mcl_http_request_t *http_request = MCL_NULL;

    // Initialize http_request object.
    mcl_http_request_initialize(&http_request);

    // Test.
    code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_BODY, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "MCL_TRIGGERED_WITH_NULL should have been returned.");

    // Destroy http_request object.
    mcl_http_request_destroy(&http_request);
}

/**
 * GIVEN : @p http_request which its members set to default values.
 * WHEN  : #mcl_http_request_set_parameter() is called with the related option and value.
 * THEN  : New value of max_payload_size is set and MCL_OK is returned.
 */
void test_set_parameter_003(void)
{
    mcl_error_t code;
    mcl_http_request_t *http_request = MCL_NULL;

    // Initialize http_request object.
    mcl_http_request_initialize(&http_request);

    // Set payload size.
    const mcl_size_t payload_size = 100;
    code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_BODY_SIZE, &payload_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_http_request_set_parameter() failed.");

    // Test.
    TEST_ASSERT_EQUAL_MESSAGE(payload_size, http_request->payload_size, "Payload size could not be set.");

    // Destroy http_request object.
    mcl_http_request_destroy(&http_request);
}

/**
 * GIVEN : @p http_request which its members set to default values.
 * WHEN  : #mcl_http_request_set_parameter() is called with the related option and value.
 * THEN  : New value of resize_enabled is set and MCL_OK is returned.
 */
void test_set_parameter_004(void)
{
    mcl_error_t code;
    mcl_http_request_t *http_request = MCL_NULL;

    // Initialize http_request object.
    mcl_http_request_initialize(&http_request);

    char body[] = "The Body";
    code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_BODY, body);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_http_request_set_parameter() failed.");

    // Test.
    TEST_ASSERT_EQUAL_PTR_MESSAGE(body, http_request->payload, "Body could not be set.");

    // Destroy http_request object.
    mcl_http_request_destroy(&http_request);
}

/**
 * GIVEN : @p http_request which its members set to default values and invalid parameter as @p option.
 * WHEN  : #mcl_http_request_set_parameter() is called with invalid option.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_005(void)
{
    mcl_error_t code;
    mcl_http_request_t *http_request = MCL_NULL;

    // Initialize http_request object.
    mcl_http_request_initialize(&http_request);

    // Set value with invalid option parameter.
    mcl_bool_t dummy = MCL_TRUE;
    code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_END, &dummy);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_http_request_set_parameter() failed.");

    // Destroy http_request object.
    mcl_http_request_destroy(&http_request);
}

/**
 * GIVEN : @p http_request which its members set to default values.
 * WHEN  : #mcl_http_request_set_parameter() is called with the related option and value.
 * THEN  : New value of stream_callback is set and MCL_OK is returned.
 */
void test_set_parameter_006(void)
{
    mcl_error_t code;
    mcl_http_request_t *http_request = MCL_NULL;

    // Initialize http_request object.
    mcl_http_request_initialize(&http_request);

    // Set stream callback.
    code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_STREAM_CALLBACK, &dummy_callback);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_http_request_set_parameter() failed.");

    // Test.
    TEST_ASSERT_EQUAL_MESSAGE(dummy_callback, http_request->stream_callback, "Stream_callback could not be set.");

    // Destroy http_request object.
    mcl_http_request_destroy(&http_request);
}

/**
 * GIVEN : @p http_request which its members set to default values.
 * WHEN  : #mcl_http_request_set_parameter() is called with the related option and value.
 * THEN  : New value of stream_data is set and MCL_OK is returned.
 */
void test_set_parameter_007(void)
{
    mcl_error_t code;
    mcl_http_request_t *http_request = MCL_NULL;

    // Initialize http_request object.
    mcl_http_request_initialize(&http_request);

    // Set stream data.
    mcl_size_t stream_data[1];
    code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_STREAM_DATA, stream_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_http_request_set_parameter() failed.");

    // Test.
    TEST_ASSERT_EQUAL_MESSAGE(stream_data, http_request->stream_data, "Stream_data could not be set.");

    // Destroy http_request object.
    mcl_http_request_destroy(&http_request);
}

static mcl_size_t dummy_callback(void *buffer, mcl_size_t size, mcl_size_t count, void *file_descriptor)
{
    (void) buffer;
    (void) size;
    (void) count;
    (void) file_descriptor;

    return 0;
}