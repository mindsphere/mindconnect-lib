/*!
 * @file     test_multipart.c
 * @brief    This file implements test cases of multipart module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "multipart.h"
#include "mock_json.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"
#include "mock_mcl_random.h"
#include "mock_mcl_http_request.h"
#include <string.h>

void setUp(void)
{

}

void tearDown(void)
{

}

static const char boundary_sign[] = "--";
static const char content_type[] = "Content-Type: ";
static const char boundary_key[] = ";boundary=";
static const char endline[] = "\r\n\r\n";
static const char content_type_multipart_related[] = "multipart/related";
static const char content_type_meta_json[] = "application/vnd.siemens.mindsphere.meta+json";
static const char content_type_application_json[] = "application/json";

#define BOUNDARY_SIGN_LENGTH        (sizeof(boundary_sign) - 1)
#define CONTENT_TYPE_LENGTH         (sizeof(content_type) - 1)
#define BOUNDARY_KEY_LENGTH         (sizeof(boundary_key) - 1)
#define DOUBLE_ENDLINE_LENGTH       (sizeof(endline) - 1)
#define ENDLINE_LENGTH              (DOUBLE_ENDLINE_LENGTH / 2)
#define BOUNDARY_LENGTH 20
#define CONTENT_TYPE_MULTIPART_RELATED_LENGTH       (sizeof(content_type_multipart_related) - 1)
#define CONTENT_TYPE_META_JSON_LENGTH               (sizeof(content_type_meta_json) - 1)
#define CONTENT_TYPE_APPLICATION_JSON_LENGTH        (sizeof(content_type_application_json) - 1)

#define MULTIPART_BASE_SIZE (5 * BOUNDARY_SIGN_LENGTH + 5 * BOUNDARY_LENGTH + 3 * CONTENT_TYPE_LENGTH + CONTENT_TYPE_MULTIPART_RELATED_LENGTH \
                             + BOUNDARY_KEY_LENGTH + CONTENT_TYPE_META_JSON_LENGTH + 12 * ENDLINE_LENGTH)

#define MULTIPART_CLOSE_LENGTH (BOUNDARY_LENGTH + 2 * BOUNDARY_SIGN_LENGTH)

static mcl_size_t dummy_callback(char *buffer, mcl_size_t size, mcl_size_t count, void *user_context);

typedef struct dummy_user_context_t
{
    char *payload;
    mcl_size_t payload_length;
    mcl_size_t offset;
} dummy_user_context_t;

/**
 * GIVEN : An item which will be added as a tuple.
 * WHEN  : multipart_add_tuple is called with the item.
 * THEN  : MCL_OK is returned.
 */
void test_add_tuple_001(void)
{
    // Initializations.
    mcl_error_t code;
    char *dummy_item = "Think me as an item!";
    char *dummy_boundary = "_BOUNDARY__BOUNDARY_";

    // Item meta size and item payload size are equal to 1.
    char *item_meta = MCL_MALLOC(2);
    char *item_payload = MCL_MALLOC(2);
    char tuple_buffer[MULTIPART_BASE_SIZE + CONTENT_TYPE_APPLICATION_JSON_LENGTH + 2];
    mcl_size_t buffer_size = MULTIPART_BASE_SIZE + CONTENT_TYPE_APPLICATION_JSON_LENGTH + 2;

    item_meta[0] = 'M';
    item_meta[1] = '\0';

    item_payload[0] = 'P';
    item_payload[1] = '\0';

    // Generate sub boundary.
    mcl_random_generate_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // Item meta.
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);
    json_from_item_meta_ReturnThruPtr_json_string(&item_meta);

    // Item payload.    
    json_from_item_payload_ExpectAnyArgsAndReturn(MCL_OK);
    json_from_item_payload_ReturnThruPtr_json_string(&item_payload);
    
    // Test.
    code = multipart_add_tuple(tuple_buffer, &buffer_size, dummy_item, dummy_boundary);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "multipart_add_tuple() did not return MCL_OK.");
}

/**
 * GIVEN : An item which will be added as a tuple.
 * WHEN  : There is not enough space in the buffer.
 * THEN  : MCL_FAIL is returned.
 */
void test_add_tuple_002(void)
{
    // Initializations.
    mcl_error_t code;
    char *dummy_item = "Think me as an item!";
    char *dummy_boundary = "boundary";
    char tuple_buffer[BOUNDARY_LENGTH];
    mcl_size_t buffer_size = BOUNDARY_LENGTH;

    // Generate sub boundary.
    mcl_random_generate_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // Test.
    code = multipart_add_tuple(tuple_buffer, &buffer_size, dummy_item, dummy_boundary);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_FAIL, code, "multipart_add_tuple() did not return MCL_HTTP_REQUEST_NO_MORE_SPACE.");
}

/**
 * GIVEN : An item which will be added as a tuple with callback.
 * WHEN  : multipart_add_tuple_with_callback is called with the item.
 * THEN  : MCL_OK is returned.
 */
void test_add_tuple_with_callback_001(void)
{
    // Initializations.
    mcl_error_t code;
    char dummy_item[] = "Think me as an item!";
    char dummy_boundary[] = "_BOUNDARY__BOUNDARY_";
    char dummy_content_type[] = "Dummy content type";
    char dummy_payload[] = "Dummy payload";

    dummy_user_context_t dummy_user_context =
    {
        .payload = dummy_payload,
        .payload_length = sizeof(dummy_payload) - MCL_NULL_CHAR_SIZE,
        .offset = 0
    };

    char *item_meta = MCL_MALLOC(2);

    item_meta[0] = 'M';
    item_meta[1] = '\0';

    mcl_size_t buffer_size = MULTIPART_BASE_SIZE + sizeof(dummy_payload) + sizeof(dummy_content_type) + 1;
    char *tuple_buffer = MCL_MALLOC(buffer_size);

    // Prepare HTTP request payload.
    // Generate sub boundary.
    mcl_random_generate_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // Item meta.
    json_from_item_meta_ExpectAnyArgsAndReturn(MCL_OK);
    json_from_item_meta_ReturnThruPtr_json_string(&item_meta);

    // Test.
    code = multipart_add_tuple_with_callback(tuple_buffer, &buffer_size, dummy_item, dummy_boundary, dummy_content_type, dummy_callback, &dummy_user_context);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "multipart_add_tuple_with_callback() did not return MCL_OK.");

    // Clean up.
    MCL_FREE(tuple_buffer);
}

/**
 * GIVEN : Initialized item.
 * WHEN  : multipart_get_tuple_size() is called.
 * THEN  : Tuple size of the item is returned.
 */
void test_get_tuple_size_001(void)
{
    mcl_size_t tuple_size;
    char dummy_item[] = "Think me as an item!";
    char multipart_base[] = "--01234567890123456789\r\n" \
        "Content-Type: multipart/related;boundary=98765432109876543210\r\n" \
        "\r\n" \
        "--98765432109876543210\r\n" \
        "Content-Type: application/vnd.siemens.mindsphere.meta+json\r\n" \
        "\r\n" \
        /* Item meta */ "\r\n" \
        "--98765432109876543210\r\n" \
        "Content-Type: " /* Item payload content type */ "\r\n" \
        "\r\n" \
        /* Item payload */ "\r\n" \
        "--98765432109876543210--\r\n";

    mcl_size_t multipart_base_size = sizeof(multipart_base) - MCL_NULL_CHAR_SIZE;

    // Assign a dummy item size.
    mcl_size_t item_size = 123;

    // Mock json get item size.
    json_get_item_size_ExpectAndReturn(dummy_item, item_size);

    // Test.
    tuple_size = multipart_get_tuple_size(dummy_item, content_type_application_json);

    // Check.
    TEST_ASSERT_EQUAL_MESSAGE(multipart_base_size + item_size + CONTENT_TYPE_APPLICATION_JSON_LENGTH, tuple_size, "Tuple size is not calculated correctly.");
}

/**
 * GIVEN : Initialized http request and boundary.
 * WHEN  : multipart_close() is called.
 * THEN  : MCL_OK is returned.
 */
void test_close_001(void)
{
    // Example boundary. Length = 20.
    const char *boundary = "effd351cf0bd99bcc16d";

    char tuple_buffer[MULTIPART_CLOSE_LENGTH];
    mcl_size_t buffer_size = MULTIPART_CLOSE_LENGTH;

    // Close multipart.
    mcl_error_t code = multipart_close(tuple_buffer, &buffer_size, boundary);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "multipart_close() didn't return MCL_OK.");
}

/**
 * GIVEN : Nothing specific.
 * WHEN  : multipart_generate_boundary() is called.
 * THEN  : MCL_OK is returned.
 */
void test_generate_boundary_001(void)
{
    char *boundary = MCL_NULL;

    // Mock random number generation.
    mcl_random_generate_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // Test.
    mcl_error_t code = multipart_generate_boundary(&boundary);

    // Check test results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Boundary can not be generated.");
    TEST_ASSERT_NOT_NULL_MESSAGE(boundary, "Boundary is NULL.");

    // Clean up.
    MCL_FREE(boundary);
}

static mcl_size_t dummy_callback(char *buffer, mcl_size_t size, mcl_size_t count, void *user_context)
{
    mcl_size_t buffer_size = size * count;
    mcl_size_t copy_size;
    dummy_user_context_t *dummy_user_context = (dummy_user_context_t *) user_context;

    copy_size = dummy_user_context->payload_length == dummy_user_context->offset;

    if (0 != copy_size)
    {
        copy_size = copy_size > buffer_size ? buffer_size : copy_size;
        memcpy(buffer, dummy_user_context->payload + dummy_user_context->offset, copy_size);
        dummy_user_context->offset += copy_size;
    }

    return copy_size;
}
