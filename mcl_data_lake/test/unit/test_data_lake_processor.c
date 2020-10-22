/*!
 * @file     test_data_lake_processor.c
 * @brief    This file implements all test cases of data_lake_processor module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "data_lake_processor.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"
#include "mock_mcl_http_client.h"
#include "mock_mcl_http_request.h"
#include "mock_mcl_http_response.h"
#include "mock_data_lake_object.h"
#include "mock_data_lake_json.h"

void setUp(void)
{
}

void tearDown(void)
{
}

#define TEST_PATH_1         "<Path to upload 1>"
#define TEST_PATH_2         "<Path to upload 2>"
#define TEST_PATH_3         "<Path to upload 3>"
#define TEST_SIGNED_URL_1   "https://signed_1.url"
#define TEST_SIGNED_URL_2   "https://signed_2.url"
#define TEST_SIGNED_URL_3   "https://signed_3.url"
#define TEST_CLIENT_ID      "<Client ID>"
#define TEST_SUBTENANT_ID   "<Subtenant ID>"

/**
 * GIVEN : Object with path.
 * WHEN  : data_lake_processor_generate_upload_url is called.
 * THEN  : Everything goes fine and MCL_OK is returned.
 */
void test_generate_upload_url_001(void)
{
    // Initialize test variables.
    mcl_error_t code;
    data_lake_processor_t processor;
    data_lake_object_t dummy_object;
    mcl_http_client_t dummy_client;
    mcl_http_request_t dummy_request;
    mcl_http_response_t dummy_response;

    mcl_http_request_t *ptr_dummy_request = &dummy_request;
    mcl_http_response_t *ptr_dummy_response = &dummy_response;

    char upload_url_generation_url[] = "The URL";
    char json_for_body[] = "{\"paths\":[{\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_1 "\"}]}";
    char response_payload[] = "{\"objectUrls\":[{\"signedUrl\":\"" TEST_SIGNED_URL_1 "\",\"objectPath\":\"" TEST_CLIENT_ID "/" TEST_PATH_1 "\"}]}";

    char *allocated_body = MCL_MALLOC(sizeof(json_for_body));
    mcl_string_util_memcpy(allocated_body, json_for_body, sizeof(json_for_body));

    // Data lake object.
    dummy_object.path = TEST_PATH_1;
    dummy_object.signed_url = MCL_NULL;

    // Data lake processor.
    processor.access_token = "<Access Token>";
    processor.http_client = &dummy_client;
    processor.upload_url_generation_url = upload_url_generation_url;
    processor.client_id = TEST_CLIENT_ID;

    // HTTP response
    dummy_response.payload = response_payload;
    dummy_response.payload_size = sizeof(response_payload) - 1;
    dummy_response.status_code = MCL_HTTP_STATUS_CODE_CREATED;

    // Processor should initialize a http request.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_initialize_ReturnThruPtr_http_request(&ptr_dummy_request);

    // There will be two headers.
    mcl_http_request_add_header_ExpectAndReturn(ptr_dummy_request, "Content-Type", "application/json", MCL_OK);
    mcl_http_request_add_header_ExpectAndReturn(ptr_dummy_request, "Authorization", "Bearer <Access Token>", MCL_OK);

    // Http request parameters will be set.
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_client_send_ExpectAndReturn(&dummy_client, ptr_dummy_request, MCL_NULL, MCL_OK);
    mcl_http_client_send_IgnoreArg_http_response();
    mcl_http_client_send_ReturnThruPtr_http_response(&ptr_dummy_response);

    // HTTP request and response should be destroyed.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    // Mock data lake json.
    data_lake_json_from_objects_ExpectAnyArgsAndReturn(MCL_OK);
    data_lake_json_from_objects_ReturnThruPtr_json(&allocated_body);
    data_lake_json_match_signed_urls_with_objects_ExpectAnyArgsAndReturn(MCL_OK);

    // Test.
    code = data_lake_processor_generate_upload_url(&processor, &dummy_object, MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should have been returned.");
}

/**
 * GIVEN : Server responds with 400.
 * WHEN  : data_lake_processor_generate_upload_url is called.
 * THEN  : MCL_BAD_REQUEST is returned.
 * THEN  : MCL_FAIL is returned.
 */
void test_generate_upload_url_002(void)
{
    // Initialize test variables.
    mcl_error_t code;
    data_lake_processor_t processor;
    data_lake_object_t dummy_object;
    mcl_http_client_t dummy_client;
    mcl_http_request_t dummy_request;
    mcl_http_response_t dummy_response;

    mcl_http_request_t *ptr_dummy_request = &dummy_request;
    mcl_http_response_t *ptr_dummy_response = &dummy_response;

    char upload_url_generation_url[] = "The URL";
    char json_for_body[] = "{\"paths\":[{\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_1 "\"}]}";
    char response_payload[] = "{\"objectUrls\":[{\"signedUrl\":\"" TEST_SIGNED_URL_1 "\",\"objectPath\":\"" TEST_CLIENT_ID "/" TEST_PATH_1 "\"}]}";

    char *allocated_body = MCL_MALLOC(sizeof(json_for_body));
    mcl_string_util_memcpy(allocated_body, json_for_body, sizeof(json_for_body));

    // Data lake object.
    dummy_object.path = TEST_PATH_1;
    dummy_object.signed_url = MCL_NULL;

    // Data lake processor.
    processor.access_token = "<Access Token>";
    processor.http_client = &dummy_client;
    processor.upload_url_generation_url = upload_url_generation_url;
    processor.client_id = TEST_CLIENT_ID;

    // HTTP response
    dummy_response.payload = response_payload;
    dummy_response.payload_size = sizeof(response_payload) - 1;
    dummy_response.status_code = MCL_HTTP_STATUS_CODE_BAD_REQUEST;

    // Processor should initialize a http request.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_initialize_ReturnThruPtr_http_request(&ptr_dummy_request);

    // There will be two headers.
    mcl_http_request_add_header_ExpectAndReturn(ptr_dummy_request, "Content-Type", "application/json", MCL_OK);
    mcl_http_request_add_header_ExpectAndReturn(ptr_dummy_request, "Authorization", "Bearer <Access Token>", MCL_OK);

    // Http request parameters will be set.
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_client_send_ExpectAndReturn(&dummy_client, ptr_dummy_request, MCL_NULL, MCL_OK);
    mcl_http_client_send_IgnoreArg_http_response();
    mcl_http_client_send_ReturnThruPtr_http_response(&ptr_dummy_response);

    // HTTP request and response should be destroyed.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    // Mock data lake json.
    data_lake_json_from_objects_ExpectAnyArgsAndReturn(MCL_OK);
    data_lake_json_from_objects_ReturnThruPtr_json(&allocated_body);

    // Test.
    code = data_lake_processor_generate_upload_url(&processor, &dummy_object, MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_BAD_REQUEST, code, "MCL_BAD_REQUEST should have been returned.");
    TEST_ASSERT_NULL_MESSAGE(dummy_object.signed_url, "Signed url should be NULL.")
}

/**
 * GIVEN : Object with path.
 * WHEN  : data_lake_processor_generate_upload_url is called with non-NULL subtenant_id parameter.
 * THEN  : Everything goes fine and MCL_OK is returned.
 */
void test_generate_upload_url_003(void)
{
    // Initialize test variables.
    mcl_error_t code;
    data_lake_processor_t processor;
    data_lake_object_t dummy_object;
    mcl_http_client_t dummy_client;
    mcl_http_request_t dummy_request;
    mcl_http_response_t dummy_response;

    mcl_http_request_t *ptr_dummy_request = &dummy_request;
    mcl_http_response_t *ptr_dummy_response = &dummy_response;

    char upload_url_generation_url[] = "The URL";
    char json_for_body[] = "{\"paths\":[{\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_1 "\"}]}";
    char response_payload[] = "{\"objectUrls\":[{\"signedUrl\":\"" TEST_SIGNED_URL_1 "\",\"objectPath\":\"" TEST_CLIENT_ID "/" TEST_PATH_1 "\"}],\"subtenantId\":\"" TEST_SUBTENANT_ID "\"}";

    char *allocated_body = MCL_MALLOC(sizeof(json_for_body));
    mcl_string_util_memcpy(allocated_body, json_for_body, sizeof(json_for_body));

    // Data lake object.
    dummy_object.path = TEST_PATH_1;
    dummy_object.signed_url = MCL_NULL;

    // Data lake processor.
    processor.access_token = "<Access Token>";
    processor.http_client = &dummy_client;
    processor.upload_url_generation_url = upload_url_generation_url;
    processor.client_id = TEST_CLIENT_ID;

    // HTTP response
    dummy_response.payload = response_payload;
    dummy_response.payload_size = sizeof(response_payload) - 1;
    dummy_response.status_code = MCL_HTTP_STATUS_CODE_CREATED;

    // Processor should initialize a http request.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_initialize_ReturnThruPtr_http_request(&ptr_dummy_request);

    // There will be two headers.
    mcl_http_request_add_header_ExpectAndReturn(ptr_dummy_request, "Content-Type", "application/json", MCL_OK);
    mcl_http_request_add_header_ExpectAndReturn(ptr_dummy_request, "Authorization", "Bearer <Access Token>", MCL_OK);

    // Http request parameters will be set.
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_client_send_ExpectAndReturn(&dummy_client, ptr_dummy_request, MCL_NULL, MCL_OK);
    mcl_http_client_send_IgnoreArg_http_response();
    mcl_http_client_send_ReturnThruPtr_http_response(&ptr_dummy_response);

    // HTTP request and response should be destroyed.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    // Mock data lake json.
    data_lake_json_from_objects_ExpectAndReturn(MCL_NULL, 1, processor.client_id, TEST_SUBTENANT_ID, MCL_NULL, MCL_OK);
    data_lake_json_from_objects_IgnoreArg_object_array();
    data_lake_json_from_objects_IgnoreArg_json();
    data_lake_json_from_objects_ReturnThruPtr_json(&allocated_body);
    data_lake_json_match_signed_urls_with_objects_ExpectAnyArgsAndReturn(MCL_OK);

    // Test.
    code = data_lake_processor_generate_upload_url(&processor, &dummy_object, TEST_SUBTENANT_ID);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should have been returned.");
}

/**
 * GIVEN : Objects with path.
 * WHEN  : data_lake_processor_generate_upload_urls is called.
 * THEN  : Everything goes fine and MCL_OK is returned.
 */
void test_generate_upload_urls_001(void)
{
    // Initialize test variables.
    mcl_error_t code;
    data_lake_processor_t processor;
    mcl_http_client_t dummy_client;
    mcl_http_request_t dummy_request;
    mcl_http_response_t dummy_response;
    mcl_list_t *object_list;

    // Multiple objects.
    data_lake_object_t dummy_object_1;
    data_lake_object_t dummy_object_2;
    data_lake_object_t dummy_object_3;

    mcl_http_request_t *ptr_dummy_request = &dummy_request;
    mcl_http_response_t *ptr_dummy_response = &dummy_response;

    char upload_url_generation_url[] = "The URL";
    char json_for_body[] = "{\"paths\":[{\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_1 "\"},{\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_2 "\"},{\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_3 "\"}]}";

    char response_payload[] = "{\"objectUrls\":[{\"signedUrl\":\"" TEST_SIGNED_URL_2 "\",\"objectPath\":\"" TEST_CLIENT_ID "/" TEST_PATH_2 "\"}," \
                                "{\"signedUrl\":\"" TEST_SIGNED_URL_3 "\",\"objectPath\":\"" TEST_CLIENT_ID "/" TEST_PATH_3 "\"}," \
                                "{\"signedUrl\":\"" TEST_SIGNED_URL_1 "\",\"objectPath\":\"" TEST_CLIENT_ID "/" TEST_PATH_1 "\"}]}";

    char *allocated_body = MCL_MALLOC(sizeof(json_for_body));
    mcl_string_util_memcpy(allocated_body, json_for_body, sizeof(json_for_body));

    // Data lake objects.
    dummy_object_1.path = TEST_PATH_1;
    dummy_object_1.signed_url = MCL_NULL;

    dummy_object_2.path = TEST_PATH_2;
    dummy_object_2.signed_url = MCL_NULL;

    dummy_object_3.path = TEST_PATH_3;
    dummy_object_3.signed_url = MCL_NULL;

    // Put objects in a list.
    mcl_list_initialize(&object_list);
    mcl_list_add(object_list, &dummy_object_1);
    mcl_list_add(object_list, &dummy_object_2);
    mcl_list_add(object_list, &dummy_object_3);

    // Data lake processor.
    processor.access_token = "<Access Token>";
    processor.http_client = &dummy_client;
    processor.upload_url_generation_url = upload_url_generation_url;
    processor.client_id = TEST_CLIENT_ID;

    // HTTP response
    dummy_response.payload = response_payload;
    dummy_response.payload_size = sizeof(response_payload) - 1;
    dummy_response.status_code = MCL_HTTP_STATUS_CODE_CREATED;

    // Processor should initialize a http request.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_initialize_ReturnThruPtr_http_request(&ptr_dummy_request);

    // There will be two headers.
    mcl_http_request_add_header_ExpectAndReturn(ptr_dummy_request, "Content-Type", "application/json", MCL_OK);
    mcl_http_request_add_header_ExpectAndReturn(ptr_dummy_request, "Authorization", "Bearer <Access Token>", MCL_OK);

    // Http request parameters will be set.
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_client_send_ExpectAndReturn(&dummy_client, ptr_dummy_request, MCL_NULL, MCL_OK);
    mcl_http_client_send_IgnoreArg_http_response();
    mcl_http_client_send_ReturnThruPtr_http_response(&ptr_dummy_response);

    // HTTP request and response should be destroyed.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    // Mock data lake json.
    data_lake_json_from_objects_ExpectAnyArgsAndReturn(MCL_OK);
    data_lake_json_from_objects_ReturnThruPtr_json(&allocated_body);
    data_lake_json_match_signed_urls_with_objects_ExpectAnyArgsAndReturn(MCL_OK);

    // Test.
    code = data_lake_processor_generate_upload_urls(&processor, object_list, MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should have been returned.");

    // Clean up.
    mcl_list_destroy(&object_list);
}

/**
 * GIVEN : Objects with path.
 * WHEN  : data_lake_processor_generate_upload_urls is called.
 * THEN  : Signed url cannot be retrieved for an object and MCL_DATA_LAKE_SIGNED_URL_GENERATION_FAIL is returned.
 */
void test_generate_upload_urls_002(void)
{
    // Initialize test variables.
    mcl_error_t code;
    data_lake_processor_t processor;
    mcl_http_client_t dummy_client;
    mcl_http_request_t dummy_request;
    mcl_http_response_t dummy_response;
    mcl_list_t *object_list;

    // Multiple objects.
    data_lake_object_t dummy_object_1;
    data_lake_object_t dummy_object_2;
    data_lake_object_t dummy_object_3;

    mcl_http_request_t *ptr_dummy_request = &dummy_request;
    mcl_http_response_t *ptr_dummy_response = &dummy_response;

    char upload_url_generation_url[] = "The URL";
    char json_for_body[] = "{\"paths\":[{\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_1 "\"},{\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_2 "\",{\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_3 "\"}]}";

    char response_payload[] = "{\"objectUrls\":[{\"signedUrl\":\"" TEST_SIGNED_URL_2 "\",\"objectPath\":\"" TEST_CLIENT_ID "/" TEST_PATH_2 "\"}," \
        "{\"signedUrl\":\"" TEST_SIGNED_URL_1 "\",\"objectPath\":\"" TEST_CLIENT_ID "/" TEST_PATH_1 "\"}]}";

    char *allocated_body = MCL_MALLOC(sizeof(json_for_body));
    mcl_string_util_memcpy(allocated_body, json_for_body, sizeof(json_for_body));

    // Data lake objects.
    dummy_object_1.path = TEST_PATH_1;
    dummy_object_1.signed_url = MCL_NULL;

    dummy_object_2.path = TEST_PATH_2;
    dummy_object_2.signed_url = MCL_NULL;

    dummy_object_3.path = TEST_PATH_3;
    dummy_object_3.signed_url = MCL_NULL;

    // Put objects in a list.
    mcl_list_initialize(&object_list);
    mcl_list_add(object_list, &dummy_object_1);
    mcl_list_add(object_list, &dummy_object_2);
    mcl_list_add(object_list, &dummy_object_3);

    // Data lake processor.
    processor.access_token = "<Access Token>";
    processor.http_client = &dummy_client;
    processor.upload_url_generation_url = upload_url_generation_url;
    processor.client_id = TEST_CLIENT_ID;

    // HTTP response
    dummy_response.payload = response_payload;
    dummy_response.payload_size = sizeof(response_payload) - 1;
    dummy_response.status_code = MCL_HTTP_STATUS_CODE_CREATED;

    // Processor should initialize a http request.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_initialize_ReturnThruPtr_http_request(&ptr_dummy_request);

    // There will be two headers.
    mcl_http_request_add_header_ExpectAndReturn(ptr_dummy_request, "Content-Type", "application/json", MCL_OK);
    mcl_http_request_add_header_ExpectAndReturn(ptr_dummy_request, "Authorization", "Bearer <Access Token>", MCL_OK);

    // Http request parameters will be set.
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_client_send_ExpectAndReturn(&dummy_client, ptr_dummy_request, MCL_NULL, MCL_OK);
    mcl_http_client_send_IgnoreArg_http_response();
    mcl_http_client_send_ReturnThruPtr_http_response(&ptr_dummy_response);

    // HTTP request and response should be destroyed.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    // Mock data lake json.
    data_lake_json_from_objects_ExpectAnyArgsAndReturn(MCL_OK);
    data_lake_json_from_objects_ReturnThruPtr_json(&allocated_body);
    data_lake_json_match_signed_urls_with_objects_ExpectAnyArgsAndReturn(MCL_DATA_LAKE_SIGNED_URL_GENERATION_FAIL);

    // Test.
    code = data_lake_processor_generate_upload_urls(&processor, object_list, MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_DATA_LAKE_SIGNED_URL_GENERATION_FAIL, code, "MCL_DATA_LAKE_SIGNED_URL_GENERATION_FAIL should have been returned.");

    // Clean up.
    mcl_list_destroy(&object_list);
}

/**
 * GIVEN : Processor and object.
 * WHEN  : data_lake_processor_upload() is called.
 * THEN  : MCL_OK is returned.
 */
void test_upload_001(void)
{
    // Initialize test variables.
    mcl_error_t code;
    data_lake_processor_t processor;
    data_lake_object_t object;
    mcl_http_client_t http_client;
    
    // Data lake processor.
    processor.access_token = "<Access Token>";
    processor.http_client = &http_client;
    
    void *dummy_user_context = (void *)0xC0DE;
    mcl_data_lake_upload_callback dummy_callback = (mcl_data_lake_upload_callback)0xDEADCA11;

    // Data lake object.
    object.path = "<Path to upload>";
    object.signed_url = "https://signed.url";
    object.upload_callback = dummy_callback;
    object.user_context = dummy_user_context;

    // Mock data lake object validate.
    data_lake_object_validate_IgnoreAndReturn(MCL_OK);

    // Mock http request initialization.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http request set parameter.
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_response_t *http_response = MCL_NULL;
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    http_response->payload = NULL;
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);
  
    // HTTP request and response should be destroyed.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    // Test.
    code = data_lake_processor_upload(&processor, &object);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should have been returned.");

    // Clean up.
    MCL_FREE(http_response);
}

/**
 * GIVEN : Server response with 400.
 * WHEN  : data_lake_processor_upload() is called.
 * THEN  : MCL_BAD_REQUEST is returned.
 */
void test_upload_002(void)
{
    // Initialize test variables.
    mcl_error_t code;
    data_lake_processor_t processor;
    data_lake_object_t object;
    mcl_http_client_t http_client;

    // Data lake processor.
    processor.access_token = "<Access Token>";
    processor.http_client = &http_client;

    void *dummy_user_context = (void *)0xC0DE;
    mcl_data_lake_upload_callback dummy_callback = (mcl_data_lake_upload_callback)0xDEADCA11;

    // Data lake object.
    object.path = "<Path to upload>";
    object.signed_url = "https://signed.url";
    object.upload_callback = dummy_callback;
    object.user_context = dummy_user_context;

    // Mock data lake object validate.
    data_lake_object_validate_IgnoreAndReturn(MCL_OK);

    // Mock http request initialization.
    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http request set parameter.
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);

    // Mock http client.
    mcl_http_response_t *http_response = MCL_NULL;
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_BAD_REQUEST;
    http_response->payload = NULL;
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // HTTP request and response should be destroyed.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    // Test.
    code = data_lake_processor_upload(&processor, &object);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_BAD_REQUEST, code, "MCL_OK should have been returned.");

    // Clean up.
    MCL_FREE(http_response);
}
