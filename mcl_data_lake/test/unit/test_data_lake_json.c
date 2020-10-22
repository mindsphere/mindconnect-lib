/*!
 * @file     test_data_lake_json.c
 * @brief    This file implements all test cases of data_lake_json module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "data_lake_json.h"
#include "mcl_data_lake/mcl_data_lake_object.h"
#include "mcl_core/mcl_memory.h"
#include "data_lake_object.h"

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
 * GIVEN : Three objects with path.
 * WHEN  : data_lake_json_add_objects_to_request() is called.
 * THEN  : MCL_OK is returned.
 */
void test_add_objects_to_request_001(void)
{
    mcl_error_t code;
    char *json = MCL_NULL;

    // Data lake objects.
    data_lake_object_t object_1;
    data_lake_object_t object_2;
    data_lake_object_t object_3;
    data_lake_object_t *object_array[] = {&object_1,&object_2, &object_3};

    // Assign paths.
    object_1.path = TEST_PATH_1;
    object_2.path = TEST_PATH_2;
    object_3.path = TEST_PATH_3;

    // Expected json.
    char expected[] = "{\"paths\":[{\"path\":\""TEST_CLIENT_ID"/"TEST_PATH_1"\"},{\"path\":\""TEST_CLIENT_ID"/"TEST_PATH_2"\"},{\"path\":\""TEST_CLIENT_ID"/"TEST_PATH_3"\"}]}";

    // Test.
    code = data_lake_json_from_objects(object_array, sizeof(object_array) / sizeof(data_lake_object_t *), TEST_CLIENT_ID, MCL_NULL, &json);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "data_lake_json_add_objects_to_request() should have returned MCL_OK.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, json, "Json is not correct.");

    // Clean up.
    MCL_FREE(json);
}

/**
 * GIVEN : Three objects with path.
 * WHEN  : data_lake_json_add_objects_to_request() is called with subtenant id.
 * THEN  : MCL_OK is returned.
 */
void test_add_objects_to_request_002(void)
{
    mcl_error_t code;
    char *json = MCL_NULL;

    // Data lake objects.
    data_lake_object_t object_1;
    data_lake_object_t object_2;
    data_lake_object_t object_3;
    data_lake_object_t *object_array[] = { &object_1,&object_2, &object_3 };

    // Assign paths.
    object_1.path = TEST_PATH_1;
    object_2.path = TEST_PATH_2;
    object_3.path = TEST_PATH_3;

    // Expected json.
    char expected[] = "{\"paths\":[{\"path\":\""TEST_CLIENT_ID"/"TEST_PATH_1"\"},{\"path\":\""TEST_CLIENT_ID"/"TEST_PATH_2"\"},{\"path\":\""TEST_CLIENT_ID"/"TEST_PATH_3"\"}],\"subtenantId\":\""TEST_SUBTENANT_ID"\"}";

    // Test.
    code = data_lake_json_from_objects(object_array, sizeof(object_array) / sizeof(data_lake_object_t *), TEST_CLIENT_ID, TEST_SUBTENANT_ID, &json);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "data_lake_json_add_objects_to_request() should have returned MCL_OK.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, json, "Json is not correct.");

    // Clean up.
    MCL_FREE(json);
}

/**
 * GIVEN : Json string which contain three signed urls for three objects with non-NULL paths.
 * WHEN  : data_lake_json_match_signed_urls_with_objects() is called.
 * THEN  : MCL_OK is returned.
 */
void test_match_signed_urls_with_objects_001(void)
{
    mcl_error_t code;
    mcl_size_t client_id_length = sizeof(TEST_CLIENT_ID) - MCL_NULL_CHAR_SIZE;
    char response_payload[] = "{\"objectUrls\":[{\"signedUrl\":\"" TEST_SIGNED_URL_2 "\",\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_2 "\"}," \
        "{\"signedUrl\":\"" TEST_SIGNED_URL_3 "\",\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_3 "\"}," \
        "{\"signedUrl\":\"" TEST_SIGNED_URL_1 "\",\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_1 "\"}]}";

    // Data lake objects.
    data_lake_object_t object_1;
    data_lake_object_t object_2;
    data_lake_object_t object_3;
    data_lake_object_t *object_array[] = {&object_1, &object_2, &object_3};

    // Assign paths.
    object_1.path = TEST_PATH_1;
    object_2.path = TEST_PATH_2;
    object_3.path = TEST_PATH_3;

    object_1.signed_url = MCL_NULL;
    object_2.signed_url = MCL_NULL;
    object_3.signed_url = MCL_NULL;

    // Test.
    code = data_lake_json_match_signed_urls_with_objects(object_array, sizeof(object_array) / sizeof(data_lake_object_t *), response_payload, sizeof(response_payload) - 1, client_id_length);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should have been returned.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_SIGNED_URL_1, object_1.signed_url, "Signed url is not correct.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_SIGNED_URL_2, object_2.signed_url, "Signed url is not correct.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_SIGNED_URL_3, object_3.signed_url, "Signed url is not correct.");

    // Clean up.
    MCL_FREE(object_1.signed_url);
    MCL_FREE(object_2.signed_url);
    MCL_FREE(object_3.signed_url);
}

/**
 * GIVEN : Json string which contain two signed urls for three objects with non-NULL paths.
 * WHEN  : data_lake_json_match_signed_urls_with_objects() is called.
 * THEN  : MCL_DATA_LAKE_SIGNED_URL_GENERATION_FAIL is returned.
 */
void test_match_signed_urls_with_objects_002(void)
{
    mcl_error_t code;
    mcl_size_t client_id_length = sizeof(TEST_CLIENT_ID) - MCL_NULL_CHAR_SIZE;
    char response_payload[] = "{\"objectUrls\":[{\"signedUrl\":\"" TEST_SIGNED_URL_2 "\",\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_2 "\"}," \
        "{\"signedUrl\":\"" TEST_SIGNED_URL_1 "\",\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_1 "\"}]}";

    // Data lake objects.
    data_lake_object_t object_1;
    data_lake_object_t object_2;
    data_lake_object_t object_3;
    data_lake_object_t *object_array[] = {&object_1,&object_2, &object_3};

    // Assign paths.
    object_1.path = TEST_PATH_1;
    object_2.path = TEST_PATH_2;
    object_3.path = TEST_PATH_3;

    object_1.signed_url = MCL_NULL;
    object_2.signed_url = MCL_NULL;
    object_3.signed_url = MCL_NULL;

    // Test.
    code = data_lake_json_match_signed_urls_with_objects(object_array, sizeof(object_array) / sizeof(data_lake_object_t *), response_payload, sizeof(response_payload) - 1, client_id_length);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_DATA_LAKE_SIGNED_URL_GENERATION_FAIL, code, "MCL_DATA_LAKE_SIGNED_URL_GENERATION_FAIL should have been returned.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_SIGNED_URL_1, object_1.signed_url, "Signed url is not correct.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_SIGNED_URL_2, object_2.signed_url, "Signed url is not correct.");
    TEST_ASSERT_NULL_MESSAGE(object_3.signed_url, "Signed url for object 3 should be NULL.");

    // Clean up.
    MCL_FREE(object_1.signed_url);
    MCL_FREE(object_2.signed_url);
}

/**
 * GIVEN : Json string which contain two signed urls for two objects with non-NULL paths.
 * WHEN  : data_lake_json_match_signed_urls_with_objects() is called.
 * THEN  : MCL_OK is returned.
 */
void test_match_signed_urls_with_objects_003(void)
{
    mcl_error_t code;
    mcl_size_t client_id_length = sizeof(TEST_CLIENT_ID) - MCL_NULL_CHAR_SIZE;
    char response_payload[] = "{\"objectUrls\":[{\"signedUrl\":\"" TEST_SIGNED_URL_2 "\",\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_2 "\"}," \
        "{\"signedUrl\":\"" TEST_SIGNED_URL_1 "\",\"path\":\"" TEST_CLIENT_ID "/" TEST_PATH_1 "\"}]}";

    // Data lake objects.
    data_lake_object_t object_1;
    data_lake_object_t object_2;
    data_lake_object_t object_3;
    data_lake_object_t *object_array[] = {&object_1,&object_2, &object_3};

    // Assign paths.
    object_1.path = TEST_PATH_1;
    object_2.path = TEST_PATH_2;
    object_3.path = MCL_NULL;

    object_1.signed_url = MCL_NULL;
    object_2.signed_url = MCL_NULL;
    object_3.signed_url = MCL_NULL;

    // Test.
    code = data_lake_json_match_signed_urls_with_objects(object_array, sizeof(object_array) / sizeof(data_lake_object_t *), response_payload, sizeof(response_payload) - 1, client_id_length);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should have been returned.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_SIGNED_URL_1, object_1.signed_url, "Signed url is not correct.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(TEST_SIGNED_URL_2, object_2.signed_url, "Signed url is not correct.");
    TEST_ASSERT_NULL_MESSAGE(object_3.signed_url, "Signed url for object 3 should be NULL.");

    // Clean up.
    MCL_FREE(object_1.signed_url);
    MCL_FREE(object_2.signed_url);
}
