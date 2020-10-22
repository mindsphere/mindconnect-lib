/*!
 * @file     test_connectivity_processor.c
 * @brief    This file implements test cases for connectivity processor module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "connectivity_processor.h"
#include "connectivity_common.h"
#include "mcl_core/mcl_string_util.h"
#include "mcl_core/mcl_memory.h"
#include "mock_mcl_http_client.h"
#include "mock_mcl_http_request.h"
#include "mock_mcl_http_response.h"
#include "mock_multipart.h"
#include "mock_mcl_random.h"
#include "mock_mcl_event.h"
#include "mock_event.h"
#include "mock_mcl_file.h"
#include "mock_file.h"
#include "mock_mcl_custom_data.h"
#include "mock_custom_data.h"
#include "mock_mcl_store.h"
#include "mock_store.h"
#include "mock_mapping.h"
#include "mock_json.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : A single event to exchange.
 * WHEN  : connectivity_processor_exchange() is called.
 * THEN  : MCL_OK is returned.
 */
void test_exchange_001()
{
    // Declarations.
    mcl_error_t code;
    char access_token[] = "access_token";
    char exchange_url[] = "exchange_url";
    mcl_http_client_t http_client;

    // Initialize processor.
    connectivity_processor_t processor =
    {
        .access_token = access_token,
        .exchange_url = exchange_url,
        .http_client = &http_client,
        .max_http_payload_size = 16384
    };

    // Mock event.
    mcl_event_t *event = MCL_NULL;
    MCL_NEW(event);
    event->item_base.preamble = MCL_ITEM_PREAMBLE;
    event->item_base.type = MCL_ITEM_TYPE_EVENT;
    event->item_base.version = MCL_EVENT_VERSION_1_0;
    event->payload = MCL_NULL;
    event_validate_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http request initialization.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http request set parameter.
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock boundary generation.
    char *boundary = MCL_MALLOC(21);
    multipart_generate_boundary_ExpectAnyArgsAndReturn(MCL_OK);
    multipart_generate_boundary_ReturnThruPtr_boundary(&boundary);

    // Mock http header adding.
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock tuple adding to http request.
    multipart_get_overhead_size_IgnoreAndReturn(0);
    multipart_get_tuple_size_ExpectAnyArgsAndReturn(500);
    multipart_add_tuple_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock closing boundary adding to http request.
    multipart_close_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock correlation id header adding.
    mcl_random_generate_bytes_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_response_t *http_response = MCL_NULL;
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    // Test.
    code = connectivity_processor_exchange(&processor, (mcl_item_t *)event);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Exchange failed for single event.");

    // Clean up.
    MCL_FREE(http_response);
    MCL_FREE(event);
}

/**
 * GIVEN : A single file to exchange.
 * WHEN  : connectivity_processor_exchange() is called.
 * THEN  : MCL_OK is returned.
 */
void test_exchange_002()
{
    // Declarations.
    mcl_error_t code;
    char access_token[] = "access_token";
    char exchange_url[] = "exchange_url";
    mcl_http_client_t http_client;

    // Initialize processor.
    connectivity_processor_t processor =
    {
        .access_token = access_token,
        .exchange_url = exchange_url,
        .http_client = &http_client,
        .max_http_payload_size = 16384
    };

    // Mock file.
    file_payload_t file_payload =
    {
        .creation_date = "2018-10-02T08:00:00.000Z",
        .local_path = "local_path",
        .remote_name = "remote_name",
        .type = "file_type",
        .size = 0,
        .file_descriptor = MCL_NULL
    };

    file_t file =
    {
        .item_base =
        {
            .preamble = MCL_ITEM_PREAMBLE,
            .type = MCL_ITEM_TYPE_FILE,
            .version = MCL_FILE_VERSION_1_0
        },
        .payload = &file_payload
    };

    file_validate_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http request initialization.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http request set option.
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock boundary generation.
    char *boundary = MCL_MALLOC(21);
    multipart_generate_boundary_ExpectAnyArgsAndReturn(MCL_OK);
    multipart_generate_boundary_ReturnThruPtr_boundary(&boundary);

    // Mock http header adding.
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock tuple adding to http request.
    multipart_get_overhead_size_IgnoreAndReturn(0);
    multipart_get_tuple_size_ExpectAnyArgsAndReturn(500);
    multipart_add_tuple_with_callback_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock closing boundary adding to http request.
    multipart_close_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock correlation id header adding.
    mcl_random_generate_bytes_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_response_t *http_response = MCL_NULL;
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    // Test.
    code = connectivity_processor_exchange(&processor, (mcl_item_t *)&file);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Exchange failed for single file.");

    // Clean up.
    MCL_FREE(http_response);
}

/**
 * GIVEN : A custom data to exchange.
 * WHEN  : connectivity_processor_exchange() is called.
 * THEN  : MCL_OK is returned.
 */
void test_exchange_003()
{
    // Declarations.
    mcl_error_t code;
    char access_token[] = "access_token";
    char exchange_url[] = "exchange_url";
    mcl_http_client_t http_client;

    // Initialize processor.
    connectivity_processor_t processor =
    {
        .access_token = access_token,
        .exchange_url = exchange_url,
        .http_client = &http_client,
        .max_http_payload_size = 16384
    };

    // Mock custom data.
    custom_data_payload_t custom_data_payload =
    {
        .version = "1.2",
        .buffer = (const mcl_uint8_t*) 0xABCD,
        .content_type = "myContentType",
        .type = "CustomDataType",
        .size = 42,
        .details = MCL_NULL
    };

    custom_data_t custom_data =
    {
        .item_base =
        {
            .preamble = MCL_ITEM_PREAMBLE,
            .type = MCL_ITEM_TYPE_CUSTOM_DATA,
            .version = 0 
        },
        .payload = &custom_data_payload
    };

    custom_data_validate_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http request initialization.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http request set option.
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock boundary generation.
    char *boundary = MCL_MALLOC(21);
    multipart_generate_boundary_ExpectAnyArgsAndReturn(MCL_OK);
    multipart_generate_boundary_ReturnThruPtr_boundary(&boundary);

    // Mock http header adding.
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock tuple adding to http request.
    multipart_get_overhead_size_IgnoreAndReturn(0);
    multipart_get_tuple_size_ExpectAnyArgsAndReturn(500);
    multipart_add_tuple_with_callback_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock closing boundary adding to http request.
    multipart_close_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock correlation id header adding.
    mcl_random_generate_bytes_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_response_t *http_response = MCL_NULL;
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    // Test.
    code = connectivity_processor_exchange(&processor, (mcl_item_t *)&custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Exchange failed for single custom data.");

    // Clean up.
    MCL_FREE(http_response);
}

/**
 * GIVEN : A store to exchange in a single http request.
 * WHEN  : connectivity_processor_exchange() is called.
 * THEN  : MCL_OK is returned.
 */
void test_exchange_004()
{
    // Declarations.
    mcl_error_t code;
    char access_token[] = "access_token";
    char exchange_url[] = "exchange_url";
    mcl_http_client_t http_client;
    mcl_size_t buffer_size_after_custom_data = 1000;
    mcl_size_t buffer_size_after_file = 0;

    // Initialize processor.
    connectivity_processor_t processor =
    {
        .access_token = access_token,
        .exchange_url = exchange_url,
        .http_client = &http_client,
        .max_http_payload_size = 2000
    };

    // Mock custom data.
    custom_data_payload_t custom_data_payload =
    {
        .version = "1.2",
        .buffer = (const mcl_uint8_t*)0xABCD,
        .content_type = "myContentType",
        .type = "CustomDataType",
        .size = 42,
        .details = MCL_NULL
    };

    custom_data_t custom_data =
    {
        .item_base =
        {
            .preamble = MCL_ITEM_PREAMBLE,
            .type = MCL_ITEM_TYPE_CUSTOM_DATA,
            .version = 0
        },
        .payload = &custom_data_payload
    };

    // Mock file.
    file_payload_t file_payload =
    {
        .creation_date = "2018-10-02T08:00:00.000Z",
        .local_path = "local_path",
        .remote_name = "remote_name",
        .type = "file_type",
        .size = 0,
        .file_descriptor = MCL_NULL
    };

    file_t file =
    {
        .item_base =
        {
            .preamble = MCL_ITEM_PREAMBLE,
            .type = MCL_ITEM_TYPE_FILE,
            .version = MCL_FILE_VERSION_1_0
        },
        .payload = &file_payload
    };

    store_item_t custom_data_item =
    {
        .item = &custom_data,
        .status = STORE_ITEM_STATUS_READY,
        .id = 0
    };

    store_item_t file_item =
    {
        .item = &file,
        .status = STORE_ITEM_STATUS_READY,
        .id = 1
    };

    // Mock store
    mcl_list_t *store_item_list = MCL_NULL;
    mcl_list_initialize(&store_item_list);
    mcl_list_add(store_item_list, &custom_data_item);
    mcl_list_add(store_item_list, &file_item);

    store_t store =
    {
        .item_base =
        {
            .preamble = MCL_ITEM_PREAMBLE,
            .type = MCL_ITEM_TYPE_STORE
        },
        .item_list = store_item_list,
        .last_item_id = 2
    };

    // Mock http request initialization.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http request set option.
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock boundary generation.
    char *boundary = MCL_MALLOC(21);
    multipart_generate_boundary_ExpectAnyArgsAndReturn(MCL_OK);
    multipart_generate_boundary_ReturnThruPtr_boundary(&boundary);

    // Mock http header adding.
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock tuple adding to http request.
    multipart_get_overhead_size_IgnoreAndReturn(0);
    multipart_get_tuple_size_ExpectAnyArgsAndReturn(1000);
    multipart_get_tuple_size_ExpectAnyArgsAndReturn(1000);
    multipart_add_tuple_with_callback_ExpectAnyArgsAndReturn(MCL_OK);
    multipart_add_tuple_with_callback_ReturnThruPtr_buffer_size(&buffer_size_after_custom_data);
    multipart_add_tuple_with_callback_ExpectAnyArgsAndReturn(MCL_OK);
    multipart_add_tuple_with_callback_ReturnThruPtr_buffer_size(&buffer_size_after_file);

    // Mock closing boundary adding to http request.
    multipart_close_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock correlation id header adding.
    mcl_random_generate_bytes_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_response_t *http_response = MCL_NULL;
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();
    store_item_destroy_ExpectAnyArgs();
    store_item_destroy_ExpectAnyArgs();

    // Test.
    code = connectivity_processor_exchange(&processor, (mcl_item_t *)&store);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Exchange failed for the store.");

    // Clean up.
    MCL_FREE(http_response);
    mcl_list_destroy(&store_item_list);
}

/**
 * GIVEN : A store to exchange which needs two http requests.
 * WHEN  : connectivity_processor_exchange() is called.
 * THEN  : MCL_OK is returned.
 */
void test_exchange_005()
{
    // Declarations.
    mcl_error_t code;
    char access_token[] = "access_token";
    char exchange_url[] = "exchange_url";
    mcl_http_client_t http_client;
    mcl_size_t buffer_size_after_custom_data = 0;
    mcl_size_t buffer_size_after_file = 0;

    // Initialize processor.
    connectivity_processor_t processor =
    {
        .access_token = access_token,
        .exchange_url = exchange_url,
        .http_client = &http_client,
        .max_http_payload_size = 2000
    };

    // Mock custom data.
    custom_data_payload_t custom_data_payload =
    {
        .version = "1.2",
        .buffer = (const mcl_uint8_t*)0xABCD,
        .content_type = "myContentType",
        .type = "CustomDataType",
        .size = 42,
        .details = MCL_NULL
    };

    custom_data_t custom_data =
    {
        .item_base =
        {
            .preamble = MCL_ITEM_PREAMBLE,
            .type = MCL_ITEM_TYPE_CUSTOM_DATA,
            .version = 0
        },
        .payload = &custom_data_payload
    };

    // Mock file.
    file_payload_t file_payload =
    {
        .creation_date = "2018-10-02T08:00:00.000Z",
        .local_path = "local_path",
        .remote_name = "remote_name",
        .type = "file_type",
        .size = 0,
        .file_descriptor = MCL_NULL
    };

    file_t file =
    {
        .item_base =
        {
            .preamble = MCL_ITEM_PREAMBLE,
            .type = MCL_ITEM_TYPE_FILE,
            .version = MCL_FILE_VERSION_1_0
        },
        .payload = &file_payload
    };

    store_item_t custom_data_item =
    {
        .item = &custom_data,
        .status = STORE_ITEM_STATUS_READY,
        .id = 0
    };

    store_item_t file_item =
    {
        .item = &file,
        .status = STORE_ITEM_STATUS_READY,
        .id = 1
    };

    // Mock store
    mcl_list_t *store_item_list = MCL_NULL;
    mcl_list_initialize(&store_item_list);
    mcl_list_add(store_item_list, &custom_data_item);
    mcl_list_add(store_item_list, &file_item);

    store_t store =
    {
        .item_base =
        {
            .preamble = MCL_ITEM_PREAMBLE,
            .type = MCL_ITEM_TYPE_STORE
        },
        .item_list = store_item_list,
        .last_item_id = 2
    };

    // Mock http request initialization.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http request set option.
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock boundary generation.
    char *boundary = MCL_MALLOC(21);
    multipart_generate_boundary_ExpectAnyArgsAndReturn(MCL_OK);
    multipart_generate_boundary_ReturnThruPtr_boundary(&boundary);

    char *boundary_2 = MCL_MALLOC(21);
    multipart_generate_boundary_ExpectAnyArgsAndReturn(MCL_OK);
    multipart_generate_boundary_ReturnThruPtr_boundary(&boundary_2);

    // Mock http header adding.
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock tuple adding to http request.
    multipart_get_overhead_size_IgnoreAndReturn(0);
    multipart_get_tuple_size_ExpectAnyArgsAndReturn(1500);
    multipart_get_tuple_size_ExpectAnyArgsAndReturn(1500);
    multipart_add_tuple_with_callback_ExpectAnyArgsAndReturn(MCL_OK);
    multipart_add_tuple_with_callback_ReturnThruPtr_buffer_size(&buffer_size_after_custom_data);
    multipart_add_tuple_with_callback_ExpectAnyArgsAndReturn(MCL_OK);
    multipart_add_tuple_with_callback_ReturnThruPtr_buffer_size(&buffer_size_after_file);

    // Mock closing boundary adding to http request.
    multipart_close_ExpectAnyArgsAndReturn(MCL_OK);
    multipart_close_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock correlation id header adding.
    mcl_random_generate_bytes_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_random_generate_bytes_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_response_t *http_response = MCL_NULL;
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    mcl_http_response_t *http_response_2 = MCL_NULL;
    MCL_NEW(http_response_2);
    http_response_2->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response_2);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();
    mcl_http_response_destroy_Ignore();
    store_item_destroy_ExpectAnyArgs();
    store_item_destroy_ExpectAnyArgs();

    // Test.
    code = connectivity_processor_exchange(&processor, (mcl_item_t *)&store);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Exchange failed for the store.");

    // Clean up.
    MCL_FREE(http_response);
    MCL_FREE(http_response_2);
    mcl_list_destroy(&store_item_list);
}

/**
 * GIVEN : A store to exchange which has an item whose payload is too large to exchange.
 * WHEN  : connectivity_processor_exchange() is called.
 * THEN  : MCL_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE is returned.
 */
void test_exchange_006()
{
    // Declarations.
    mcl_error_t code;
    char access_token[] = "access_token";
    char exchange_url[] = "exchange_url";
    mcl_http_client_t http_client;
    mcl_size_t buffer_size_after_custom_data = 0;

    // Initialize processor.
    connectivity_processor_t processor =
    {
        .access_token = access_token,
        .exchange_url = exchange_url,
        .http_client = &http_client,
        .max_http_payload_size = 2000
    };

    // Mock custom data.
    custom_data_payload_t custom_data_payload =
    {
        .version = "1.2",
        .buffer = (const mcl_uint8_t*)0xABCD,
        .content_type = "myContentType",
        .type = "CustomDataType",
        .size = 42,
        .details = MCL_NULL
    };

    custom_data_t custom_data =
    {
        .item_base =
        {
            .preamble = MCL_ITEM_PREAMBLE,
            .type = MCL_ITEM_TYPE_CUSTOM_DATA,
            .version = 0
        },
        .payload = &custom_data_payload
    };

    // Mock file.
    file_payload_t file_payload =
    {
        .creation_date = "2018-10-02T08:00:00.000Z",
        .local_path = "local_path",
        .remote_name = "remote_name",
        .type = "file_type",
        .size = 0,
        .file_descriptor = MCL_NULL
    };

    file_t file =
    {
        .item_base =
        {
            .preamble = MCL_ITEM_PREAMBLE,
            .type = MCL_ITEM_TYPE_FILE,
            .version = MCL_FILE_VERSION_1_0
        },
        .payload = &file_payload
    };

    store_item_t custom_data_item =
    {
        .item = &custom_data,
        .status = STORE_ITEM_STATUS_READY,
        .id = 0
    };

    store_item_t file_item =
    {
        .item = &file,
        .status = STORE_ITEM_STATUS_READY,
        .id = 1
    };

    // Mock store
    mcl_list_t *store_item_list = MCL_NULL;
    mcl_list_initialize(&store_item_list);
    mcl_list_add(store_item_list, &custom_data_item);
    mcl_list_add(store_item_list, &file_item);

    store_t store =
    {
        .item_base =
        {
            .preamble = MCL_ITEM_PREAMBLE,
            .type = MCL_ITEM_TYPE_STORE
        },
        .item_list = store_item_list,
        .last_item_id = 2
    };

    // Mock http request initialization.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http request set option.
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock boundary generation.
    char *boundary = MCL_MALLOC(21);
    multipart_generate_boundary_ExpectAnyArgsAndReturn(MCL_OK);
    multipart_generate_boundary_ReturnThruPtr_boundary(&boundary);

    // Mock http header adding.
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock tuple adding to http request.
    multipart_get_overhead_size_IgnoreAndReturn(0);
    multipart_get_tuple_size_ExpectAnyArgsAndReturn(1500);
    multipart_get_tuple_size_ExpectAnyArgsAndReturn(2500);
    multipart_add_tuple_with_callback_ExpectAnyArgsAndReturn(MCL_OK);
    multipart_add_tuple_with_callback_ReturnThruPtr_buffer_size(&buffer_size_after_custom_data);

    // Mock closing boundary adding to http request.
    multipart_close_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock correlation id header adding.
    mcl_random_generate_bytes_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_response_t *http_response = MCL_NULL;
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();
    store_item_destroy_ExpectAnyArgs();

    // Test.
    code = connectivity_processor_exchange(&processor, (mcl_item_t *)&store);
    TEST_ASSERT_MESSAGE(MCL_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE == code, "Exchange must fail for file in the store.");

    // Clean up.
    MCL_FREE(http_response);
    mcl_list_destroy(&store_item_list);
}

/**
 * GIVEN : A valid mapping to be created.
 * WHEN  : connectivity_processor_create_mapping() is called.
 * THEN  : MCL_OK is returned.
 */
void test_create_mapping_001()
{
    mcl_error_t code;
    mapping_t mapping = {"data_point_id", "entity_id", "property_set_name", "property_name", MCL_TRUE};
    char access_token[] = "access_token";
    char exchange_url[] = "exchange_url";
    const char agent_id[] = "agent_id";
    mcl_http_client_t http_client;

    // Initialize processor.
    connectivity_processor_t processor =
    {
        .agent_id = agent_id,
        .access_token = access_token,
        .exchange_url = exchange_url,
        .http_client = &http_client,
        .max_http_payload_size = 16384
    };

    // Mock mapping validate.
    mapping_validate_ExpectAndReturn(&mapping, MCL_OK);

    // Mock http request initialization.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http request set parameter.
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http header adding.
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock correlation id header adding.
    mcl_random_generate_bytes_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_response_t *http_response = MCL_NULL;
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_CREATED;
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    // Test.
    code = connectivity_processor_create_mapping(&processor, &mapping);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Creating a mapping failed.");

    // Clean up.
    MCL_FREE(http_response);
}

/**
 * GIVEN : An invalid mapping to be created.
 * WHEN  : connectivity_processor_create_mapping() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_create_mapping_002()
{
    mcl_error_t code;
    mapping_t mapping = { MCL_NULL, "entity_id", "property_set_name", "property_name", MCL_TRUE };
    char access_token[] = "access_token";
    char exchange_url[] = "exchange_url";
    const char agent_id[] = "agent_id";
    mcl_http_client_t http_client;

    // Initialize processor.
    connectivity_processor_t processor =
    {
        .agent_id = agent_id,
        .access_token = access_token,
        .exchange_url = exchange_url,
        .http_client = &http_client,
        .max_http_payload_size = 16384
    };

    // Mock mapping validate.
    mapping_validate_ExpectAndReturn(&mapping, MCL_INVALID_PARAMETER);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    // Test.
    code = connectivity_processor_create_mapping(&processor, &mapping);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "MCL_INVALID_PARAMETER should have been returned.");
}

/**
 * GIVEN : Data source configuration which was uploaded to MindSphere.
 * WHEN  : connectivity_processor_get_data_source_configuration() is called.
 * THEN  : MCL_OK is returned.
 */
void test_get_data_source_configuration_001()
{
    mcl_error_t code;
    char access_token[] = "access_token";
    char exchange_url[] = "exchange_url";
    const char agent_id[] = "agent_id";
    const char hostname[] = "hostname";
    mcl_http_client_t http_client;
    data_source_configuration_t *dsc;

    // Initialize processor.
    connectivity_processor_t processor =
    {
        .agent_id = agent_id,
        .access_token = access_token,
        .exchange_url = exchange_url,
        .http_client = &http_client,
        .hostname = hostname,
        .max_http_payload_size = 16384
    };

    // Mock http request initialization.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http request set parameter.
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http header adding.
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_response_t *http_response = MCL_NULL;
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    // Mock json.
    json_parse_item_ExpectAnyArgsAndReturn(MCL_OK);

    // Test.
    code = connectivity_processor_get_data_source_configuration(&processor, &dsc);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Getting data source configuration failed.");

    // Clean up.
    MCL_FREE(http_response);
}

/**
 * GIVEN : Data source configuration does not exist.
 * WHEN  : connectivity_processor_get_data_source_configuration() is called.
 * THEN  : MCL_NOT_FOUND is returned.
 */
void test_get_data_source_configuration_002()
{
    mcl_error_t code;
    char access_token[] = "access_token";
    char exchange_url[] = "exchange_url";
    const char agent_id[] = "agent_id";
    const char hostname[] = "hostname";
    mcl_http_client_t http_client;
    data_source_configuration_t *dsc;

    // Initialize processor.
    connectivity_processor_t processor =
    {
        .agent_id = agent_id,
        .access_token = access_token,
        .exchange_url = exchange_url,
        .http_client = &http_client,
        .hostname = hostname,
        .max_http_payload_size = 16384
    };

    // Mock http request initialization.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http request set parameter.
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_set_parameter_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http header adding.
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_add_header_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_response_t *http_response = MCL_NULL;
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_NOT_FOUND;
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    // Test.
    code = connectivity_processor_get_data_source_configuration(&processor, &dsc);
    TEST_ASSERT_MESSAGE(MCL_NOT_FOUND == code, "MCL_NOT_FOUND should have been returned.");

    // Clean up.
    MCL_FREE(http_response);
}
