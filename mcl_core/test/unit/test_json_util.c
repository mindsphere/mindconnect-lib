/*!
 * @file     test_json_util.c
 * @brief    This file implements all test cases of json_util module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_core/mcl_json_util.h"
#include "json_util.h"
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
 * GIVEN : Uninitialized root.
 * WHEN  : mcl_json_util_initialize() is called.
 * THEN  : MCL_OK is returned.
 */
void test_initialize_001(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_error_t code = mcl_json_util_initialize(MCL_JSON_OBJECT, &root);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_initialize() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    // Test.
    char *expected_json_root = "{}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    mcl_json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : mcl_json_util_initialize() is called.
 * THEN  : MCL_OK is returned.
 */
void test_initialize_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_error_t code = mcl_json_util_initialize(MCL_JSON_ARRAY, &root);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_initialize() failed.");

    // Convert root json array to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    // Test.
    char *expected_json_root = "[]";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    mcl_json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : Root parameter is MCL_NULL.
 * WHEN  : mcl_json_util_start_array() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_start_array_001(void)
{
    // Start array in root.
    char *array_name = "test array name";
    mcl_json_t *json_array = MCL_NULL;
    mcl_error_t code = mcl_json_util_start_array(MCL_NULL, array_name, &json_array);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Array name parameter is MCL_NULL.
 * WHEN  : mcl_json_util_start_array() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is reurned.
 */
void test_start_array_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Start array in root.
    mcl_json_t *json_array = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_json_util_start_array(root, MCL_NULL, &json_array);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for array_name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : mcl_json_util_start_array() is called.
 * THEN  : MCL_OK is returned and array object is added to root.
 */
void test_start_array_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Start array in root.
    char *array_name = "test array name";
    mcl_json_t *json_array = MCL_NULL;
    mcl_error_t code = mcl_json_util_start_array(root, array_name, &json_array);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_start_array() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    // Test.
    char *expected_json_root = "{\"test array name\":[]}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    mcl_json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : Array parameter is MCL_NULL.
 * WHEN  : mcl_json_util_get_array_item() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_array_item_001(void)
{
    // Variable declaration.
    mcl_json_t *array = MCL_NULL;
    int index = 1;
    mcl_json_t *item = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_json_util_get_array_item(array, index, &item);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for array = NULL.");
}

/**
 * GIVEN : Item parameter is MCL_NULL.
 * WHEN  : mcl_json_util_get_array_item() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_array_item_002(void)
{
    mcl_json_t *array = (mcl_json_t *) 0xC0DE;
    int index = 1;

    // Test.
    mcl_error_t code = mcl_json_util_get_array_item(&array, index, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for item = NULL.");
}

/**
 * GIVEN : Initialized array with two items.
 * WHEN  : mcl_json_util_get_array_item() is called for each item.
 * THEN  : MCL_OK is returned.
 */
void test_get_array_item_003(void)
{
    // Initialize array.
    mcl_json_t *array = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &array);

    // Add first item.
    char *item_value_1 = "Item Value 1";
    mcl_json_util_add_string(array, MCL_NULL, item_value_1);

    // Add second item.
    char *item_value_2 = "Item Value 2";
    mcl_json_util_add_string(array, MCL_NULL, item_value_2);

    int index = 0;
    mcl_json_t *item_1 = MCL_NULL;
    mcl_json_t *item_2 = MCL_NULL;

    mcl_error_t code = mcl_json_util_get_array_item(array, index, &item_1);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_get_array_item() failed for first item.");

    char *item_value_actual_1 = MCL_NULL;
    code = mcl_json_util_get_string(item_1, &item_value_actual_1);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_get_string() failed for first item.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(item_value_1, item_value_actual_1, "Value mismatch for first item.");

    index++;

    code = mcl_json_util_get_array_item(array, index, &item_2);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_get_array_item() failed for second item.");

    char *item_value_actual_2 = MCL_NULL;
    code = mcl_json_util_get_string(item_2, &item_value_actual_2);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_get_string() failed for second item.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(item_value_2, item_value_actual_2, "Value mismatch for second item.");

    // Clean up.
    MCL_FREE(item_value_actual_1);
    MCL_FREE(item_value_actual_2);
    mcl_json_util_destroy(&array);
}

/**
 * GIVEN : Initialized json array with no items.
 * WHEN  : mcl_json_util_get_array_item() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_get_array_item_004(void)
{
    // Initialize array.
    mcl_json_t *array = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &array);

    int index = 0;
    mcl_json_t *item_1 = MCL_NULL;
    mcl_error_t code = mcl_json_util_get_array_item(array, index, &item_1);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "mcl_json_util_get_array_item() failed.");
    TEST_ASSERT_NULL_MESSAGE(item_1, "mcl_json_util_get_array_item() failed to return null item.");

    // Clean up.
    mcl_json_util_destroy(&array);
}

/**
 * GIVEN : Initialized json array and negative index for the item to retrieve.
 * WHEN  : mcl_json_util_get_array_item() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_get_array_item_005(void)
{
    // Initialize array.
    mcl_json_t *array = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &array);

    int index = -1;
    mcl_json_t *item_1 = MCL_NULL;
    mcl_error_t code = mcl_json_util_get_array_item(array, index, &item_1);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "mcl_json_util_get_array_item() failed to detect negative index.");
    TEST_ASSERT_NULL_MESSAGE(item_1, "mcl_json_util_get_array_item() failed to return null item.");

    // Clean up.
    mcl_json_util_destroy(&array);
}

/**
 * GIVEN : Initialized json array with single item.
 * WHEN  : mcl_json_util_get_array_item() is called for third item.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_get_array_item_006(void)
{
    // Initialize array.
    mcl_json_t *array = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &array);

    // Add single item.
    char *item_value_1 = "Item Value 1";
    mcl_json_util_add_string(array, MCL_NULL, item_value_1);

    int index = 2;
    mcl_json_t *item_3 = MCL_NULL;

    // Get third item.
    mcl_error_t code = mcl_json_util_get_array_item(array, index, &item_3);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "mcl_json_util_get_array_item() failed to detect index range overflow.");
    TEST_ASSERT_NULL_MESSAGE(item_3, "mcl_json_util_get_array_item() failed to return null item.");

    // Clean up.
    mcl_json_util_destroy(&array);
}

/**
 * GIVEN : Initialized json object (not array).
 * WHEN  : mcl_json_util_get_array_item() is called.
 * THEN  : MCL_JSON_TYPE_MISMATCH is returned.
 */
void test_get_array_item_007(void)
{
    // Initialize object (not array).
    mcl_json_t *array = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &array);

    int index = 1;
    mcl_json_t *item = MCL_NULL;
    mcl_error_t code = mcl_json_util_get_array_item(array, index, &item);
    TEST_ASSERT_MESSAGE(MCL_JSON_TYPE_MISMATCH == code, "Type mismatch not detected.");

    mcl_json_util_destroy(&array);
}

/**
 * GIVEN : Array size parameter is MCL_NULL.
 * WHEN  : mcl_json_util_get_array_size() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_array_size_001(void)
{
    mcl_json_t *array = MCL_NULL;
    mcl_size_t array_size;

    // Test.
    mcl_error_t code = mcl_json_util_get_array_size(array, &array_size);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for array = NULL.");
}

/**
 * GIVEN : Array parameter is MCL_NULL.
 * WHEN  : mcl_json_util_get_array_size() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_array_size_002(void)
{
    mcl_json_t *array = (mcl_json_t *) 0xC0DE;
    mcl_size_t *array_size = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_json_util_get_array_size(&array, array_size);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for size = NULL.");
}

/**
 * GIVEN : Initialized array and size.
 * WHEN  : mcl_json_util_get_array_size() is called.
 * THEN  : MCL_OK is returned.
 */
void test_get_array_size_003(void)
{
    // Initialize array.
    mcl_json_t *array = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &array);

    // Add first item.
    char *first_array_value = "1. array_item_value";
    mcl_json_util_add_string(array, MCL_NULL, first_array_value);

    // Add second item.
    char *second_array_value = "2. array_item_value";
    mcl_json_util_add_string(array, MCL_NULL, second_array_value);

    mcl_size_t array_size;
    mcl_error_t code = mcl_json_util_get_array_size(array, &array_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "It should have returned MCL_OK.");

    // Check array size (it must be 2).
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, array_size, "Array size is wrong.");

    // Clean up.
    mcl_json_util_destroy(&array);
}

/**
 * GIVEN : Initialized json object (not array).
 * WHEN  : mcl_json_util_get_array_size() is called.
 * THEN  : MCL_OK is returned.
 */
void test_get_array_size_004(void)
{
    // Initialize json object (not array).
    mcl_json_t *array = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &array);

    // Add first item.
    char *first_array_value = "1. array_item_value";
    mcl_json_util_add_string(array, MCL_NULL, first_array_value);

    // Add second item.
    char *second_array_value = "2. array_item_value";
    mcl_json_util_add_string(array, MCL_NULL, second_array_value);

    mcl_size_t array_size;
    mcl_error_t code = mcl_json_util_get_array_size(array, &array_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_JSON_TYPE_MISMATCH, code, "Type mismatch not detected.");

    // Clean up.
    mcl_json_util_destroy(&array);
}

/**
 * GIVEN : Root parameter is MCL_NULL.
 * WHEN  : mcl_json_util_start_object() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_start_object_001(void)
{
    // Start object in root.
    char *object_name = "test object name";
    mcl_json_t *json_object = MCL_NULL;
    mcl_error_t code = mcl_json_util_start_object(MCL_NULL, object_name, &json_object);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Object name parameter is MCL_NULL.
 * WHEN  : mcl_json_util_start_object() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_start_object_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Start object in root.
    mcl_json_t *json_object = MCL_NULL;
    mcl_error_t code = mcl_json_util_start_object(root, MCL_NULL, &json_object);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for object_name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : mcl_json_util_start_object() is called.
 * THEN  : MCL_OK is returned.
 */
void test_start_object_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Start object in root.
    char *object_name = "test object name";
    mcl_json_t *json_object = MCL_NULL;
    mcl_error_t code = mcl_json_util_start_object(root, object_name, &json_object);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_start_object() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    // Test.
    char *expected_json_root = "{\"test object name\":{}}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    mcl_json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : mcl_json_util_add_string() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_string_001(void)
{
    char *name = "test_name";
    char *value = "test_value";

    // Test.
    mcl_error_t code = mcl_json_util_add_string(MCL_NULL, name, value);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Initialized root whose type is object and object name parameter is MCL_NULL.
 * WHEN  : mcl_json_util_add_string()is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_add_string_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *value = "test_value";

    // Test.
    mcl_error_t code = mcl_json_util_add_string(root, MCL_NULL, value);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER when root is object and name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Object value parameter is MCL_NULL.
 * WHEN  : mcl_json_util_add_string() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_string_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *name = "test_name";

    // Test.
    mcl_error_t code = mcl_json_util_add_string(root, name, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for value = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root whose type is object and object name is valid.
 * WHEN  : mcl_json_util_add_string() is called.
 * THEN  : MCL_OK is returned.
 */
void test_add_string_004(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *name = "test_name";
    char *value = "test_value";

    mcl_error_t code = mcl_json_util_add_string(root, name, value);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_string() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test_name\":\"test_value\"}";

    // Test.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    mcl_json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : Initialized root whose type is array and object name parameter is not MCL_NULL.
 * WHEN  : mcl_json_util_add_string() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_add_string_005(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    char *name = "test_name";
    char *value = "test_value";

    // Test.
    mcl_error_t code = mcl_json_util_add_string(root, name, value);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER when root is array and name is not NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root whose type is array and object name is MCL_NULL.
 * WHEN  : mcl_json_util_add_string() is called.
 * THEN  : MCL_OK is returned.
 */
void test_add_string_006(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    char *value_1 = "test_value_1";
    char *value_2 = "test_value_2";

    mcl_error_t code = mcl_json_util_add_string(root, MCL_NULL, value_1);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_string() failed.");

    code = mcl_json_util_add_string(root, MCL_NULL, value_2);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_string() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    // Test.
    char *expected_json_root = "[\"test_value_1\",\"test_value_2\"]";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    mcl_json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : Root parameter is MCL_NULL.
 * WHEN  : mcl_json_util_add_uint() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_uint_001(void)
{
    char *object_name = "test object name";
    mcl_size_t number = 2;

    // Test.
    mcl_error_t code = mcl_json_util_add_uint(MCL_NULL, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Initialized root whose type is object and object name parameter is MCL_NULL.
 * WHEN  : mcl_json_util_add_uint() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_add_uint_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    mcl_size_t number = 2;

    // Test.
    mcl_error_t code = mcl_json_util_add_uint(root, MCL_NULL, number);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root whose type is object and object name parameter is valid.
 * WHEN  : mcl_json_util_add_uint() is called.
 * THEN  : MCL_OK is returned.
 */
void test_add_uint_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *object_name = "test object name";
    mcl_size_t number = 2;

    mcl_error_t code = mcl_json_util_add_uint(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding integer to root failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    // Test.
    char *expected_json_root = "{\"test object name\":2}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root whose type is array and object name is not MCL_NULL.
 * WHEN  : mcl_json_util_add_uint() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_add_uint_004(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    char *object_name = "test object name";
    mcl_size_t number = 2;

    // Test.
    mcl_error_t code = mcl_json_util_add_uint(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root whose type is array and object name is MCL_NULL.
 * WHEN  : mcl_json_util_add_uint() is called.
 * THEN  : MCL_OK is returned.
 */
void test_add_uint_005(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    mcl_size_t number_1 = 1;
    mcl_size_t number_2 = 2;

    mcl_error_t code = mcl_json_util_add_uint(root, MCL_NULL, number_1);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding 1. integer to root failed.");

    code = mcl_json_util_add_uint(root, MCL_NULL, number_2);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding 2. integer to root failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    // Test.
    char *expected_json_root = "[1,2]";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : mcl_json_util_add_double() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_double_001(void)
{
    char *object_name = "test object name";
    double number = 2.3;

    // Test.
    mcl_error_t code = mcl_json_util_add_double(MCL_NULL, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Initialized root whose type is object and object name is MCL_NULL.
 * WHEN  : mcl_json_util_add_double() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_add_double_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    double number = 2.3;

    // Test.
    mcl_error_t code = mcl_json_util_add_double(root, MCL_NULL, number);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root whose type is object and object name is valid.
 * WHEN  : mcl_json_util_add_double() is called.
 * THEN  : MCL_OK is returned.
 */
void test_add_double_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *object_name = "test object name";
    double number = 2.3;

    mcl_error_t code = mcl_json_util_add_double(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding double to root failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test object name\":2.3}";

    // Test.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root whose type is array and object name is not MCL_NULL.
 * WHEN  : mcl_json_util_add_double() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_add_double_004(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    char *object_name = "test object name";
    double number = 2.3;

    // Test.
    mcl_error_t code = mcl_json_util_add_double(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name is not NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root whose type is array and object name is MCL_NULL.
 * WHEN  : mcl_json_util_add_double() is called.
 * THEN  : MCL_OK is returned.
 */
void test_add_double_005(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    double number_1 = 2.3;
    double number_2 = 4.3;

    mcl_error_t code = mcl_json_util_add_double(root, MCL_NULL, number_1);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding double to root failed.");

    code = mcl_json_util_add_double(root, MCL_NULL, number_2);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding double to root failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "[2.3,4.3]";

    // Test.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : mcl_json_util_add_bool() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_bool_001(void)
{
    char *object_name = "test object name";
    mcl_bool_t bool_value = MCL_TRUE;

    // Test.
    mcl_error_t code = mcl_json_util_add_bool(MCL_NULL, object_name, bool_value);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Initialized root whose type is object and object name is MCL_NULL.
 * WHEN  : mcl_json_util_add_bool() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_add_bool_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    mcl_bool_t bool_value = MCL_TRUE;

    // Test.
    mcl_error_t code = mcl_json_util_add_bool(root, MCL_NULL, bool_value);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name is NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root whose type is object and object name is valid.
 * WHEN  : mcl_json_util_add_bool() is called.
 * THEN  : MCL_OK is returned.
 */
void test_add_bool_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *object_name = "test object name";
    mcl_bool_t bool_value = MCL_TRUE;

    mcl_error_t code = mcl_json_util_add_bool(root, object_name, bool_value);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_bool() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    // Test.
    char *expected_json_root = "{\"test object name\":true}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root whose type is array and object name is not MCL_NULL.
 * WHEN  : mcl_json_util_add_bool() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_add_bool_004(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    char *object_name = "test object name";
    mcl_bool_t bool_value = MCL_TRUE;

    // Test.
    mcl_error_t code = mcl_json_util_add_bool(root, object_name, bool_value);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name is not NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root whose type is array and object name parameters are MCL_NULL.
 * WHEN  : mcl_json_util_add_bool() is called.
 * THEN  : MCL_OK is returned.
 */
void test_add_bool_005(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    mcl_bool_t bool_value_1 = MCL_TRUE;
    mcl_bool_t bool_value_2 = MCL_FALSE;


    mcl_error_t code = mcl_json_util_add_bool(root, MCL_NULL, bool_value_1);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_bool() failed.");

    code = mcl_json_util_add_bool(root, MCL_NULL, bool_value_2);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_bool() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    // Test.
    char *expected_json_root = "[true,false]";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : mcl_json_util_add_null() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_null_001(void)
{
    char *object_name = "test object name";

    // Test.
    mcl_error_t code = mcl_json_util_add_null(MCL_NULL, object_name);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Initialized root whose type is object and object name is MCL_NULL.
 * WHEN  : mcl_json_util_add_null() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_add_null_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Test.
    mcl_error_t code = mcl_json_util_add_null(root, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name is NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root whose type is object and object name is valid.
 * WHEN  : mcl_json_util_add_null() is called.
 * THEN  : MCL_OK is returned.
 */
void test_add_null_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *object_name = "test object name";

    mcl_error_t code = mcl_json_util_add_null(root, object_name);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_null() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    // Test.
    char *expected_json_root = "{\"test object name\":null}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root whose type is array and object name is not MCL_NULL.
 * WHEN  : mcl_json_util_add_null() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_add_null_004(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    char *object_name = "test object name";

    // Test.
    mcl_error_t code = mcl_json_util_add_null(root, object_name);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name is not NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root whose type is array and object name is MCL_NULL.
 * WHEN  : mcl_json_util_add_null() is called.
 * THEN  : MCL_OK is returned.
 */
void test_add_null_005(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    mcl_error_t code = mcl_json_util_add_null(root, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_null() failed.");

    code = mcl_json_util_add_null(root, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_null() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    // Test.
    char *expected_json_root = "[null,null]";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : mcl_json_util_add_object() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_object_001(void)
{
    // Initialize object to add to root.
    mcl_json_t *object = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &object);

    char *object_name = "test object name";

    // Test.
    mcl_error_t code = mcl_json_util_add_object(MCL_NULL, object_name, object);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");

    // Clean up.
    mcl_json_util_destroy(&object);
}

/**
 * GIVEN : Initialized root and object name is MCL_NULL.
 * WHEN  : mcl_json_util_add_object() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_object_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Initialize object to add to root.
    mcl_json_t *object = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &object);

    // Test.
    mcl_error_t code = mcl_json_util_add_object(root, MCL_NULL, object);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for object_name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
    mcl_json_util_destroy(&object);
}

/**
 * GIVEN : Initialized root and object is MCL_NULL.
 * WHEN  : mcl_json_util_add_object() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_object_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *object_name = "test object name";

    // Test.
    mcl_error_t code = mcl_json_util_add_object(root, object_name, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for object = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root and all parameters are valid.
 * WHEN  : mcl_json_util_add_object() is called.
 * THEN  : MCL_OK is returned.
 */
void test_add_object_004(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Initialize object to add to root.
    mcl_json_t *object = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &object);

    char *object_name = "test object name";

    mcl_error_t code = mcl_json_util_add_object(root, object_name, object);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_object() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    // Test.
    char *expected_json_root = "{\"test object name\":{}}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : mcl_json_util_add_item_to_array() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_item_to_array_001(void)
{
    // Initialize object to add to root.
    mcl_json_t *object = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &object);

    // Test.
    mcl_error_t code = mcl_json_util_add_item_to_array(MCL_NULL, object);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_json_util_add_item_to_array() should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");

    // Clean up.
    mcl_json_util_destroy(&object);
}

/**
 * GIVEN : Uninitialized root and object is MCL_NULL.
 * WHEN  : mcl_json_util_add_item_to_array() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_item_to_array_002(void)
{
    // Initialize array.
    mcl_json_t *array = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &array);

    // Test.
    mcl_error_t code = mcl_json_util_add_item_to_array(array, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_json_util_add_item_to_array() should have returned MCL_TRIGGERED_WITH_NULL for object = NULL.");

    // Clean up.
    mcl_json_util_destroy(&array);
}

/**
 * GIVEN : Initialized root whose type is array and objects are valid.
 * WHEN  : mcl_json_util_add_item_to_array() is called.
 * THEN  : MCL_OK is returned.
 */
void test_add_item_to_array_003(void)
{
    // Initialize object_1.
    mcl_json_t *object_1 = MCL_NULL;
    json_util_initialize(MCL_JSON_OBJECT, &object_1);

    // Initialize object_2.
    mcl_json_t *object_2 = MCL_NULL;
    json_util_initialize(MCL_JSON_OBJECT, &object_2);

    // Initialize array.
    mcl_json_t *array = MCL_NULL;
    json_util_initialize(MCL_JSON_ARRAY, &array);

    // Add string.
    json_util_add_string(object_1, "name_1", "value_1");
    json_util_add_string(object_2, "name_2", "value_2");

    mcl_error_t code = mcl_json_util_add_item_to_array(array, object_1);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_item_to_array() failed.");

    code = mcl_json_util_add_item_to_array(array, object_2);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_item_to_array() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    json_util_to_string(array, &json_root);

    // Test.
    char *expected_json_root = "[{\"name_1\":\"value_1\"},{\"name_2\":\"value_2\"}]";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    json_util_destroy(&array);
}

/**
 * GIVEN : Uninitialized json parent.
 * WHEN  : mcl_json_util_get_object_item() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_object_item_001(void)
{
    char *object_name = "test_name";

    // Test.
    mcl_json_t *json_child = MCL_NULL;
    mcl_error_t code = mcl_json_util_get_object_item(MCL_NULL, object_name, &json_child);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for json_parent = NULL.");
}

/**
 * GIVEN : Initialized root and child name is MCL_NULL.
 * WHEN  : mcl_json_util_get_object_item() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_object_item_002(void)
{
    // Initialize json_parent.
    mcl_json_t *json_parent = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json_parent);

    // Test.
    mcl_json_t *json_child = MCL_NULL;
    mcl_error_t code = mcl_json_util_get_object_item(json_parent, MCL_NULL, &json_child);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for child_name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&json_parent);
}

/**
 * GIVEN : Initialized root and json child is MCL_NULL.
 * WHEN  : mcl_json_util_get_object_item() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_object_item_003(void)
{
    // Initialize json_parent.
    mcl_json_t *json_parent = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json_parent);

    // Add string object to json parent.
    char *object_name = "test_name";

    // Test.
    mcl_error_t code = mcl_json_util_get_object_item(json_parent, object_name, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for json_child = NULL.");

    // Clean up.
    mcl_json_util_destroy(&json_parent);
}

/**
 * GIVEN : Initialized root and other parameters are valid.
 * WHEN  : mcl_json_util_get_object_item() is called.
 * THEN  : MCL_OK is returned.
 */
void test_get_object_item_004(void)
{
    // Initialize json_parent.
    mcl_json_t *json_parent = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json_parent);

    // Add string object to json parent.
    char *object_name = "test_name";
    mcl_json_util_add_string(json_parent, object_name, "test_value");

    mcl_json_t *json_child = MCL_NULL;
    mcl_error_t code = mcl_json_util_get_object_item(json_parent, object_name, &json_child);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_get_object_item() failed.");

    // Convert root json object to string.
    char *json_child_string = MCL_NULL;
    mcl_json_util_to_string(json_child, &json_child_string);

    // Test.
    char *expected_json_root = "\"test_value\"";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_child_string, "json_child_string fail.");

    // Clean up.
    MCL_FREE(json_child_string);
    mcl_json_util_destroy(&json_parent);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : mcl_json_util_has_child() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_has_child_001(void)
{
    mcl_bool_t result = MCL_FALSE;

    // Test.
    mcl_error_t code = mcl_json_util_has_child(MCL_NULL, &result);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Initialized root and result parameter is MCL_NULL.
 * WHEN  : mcl_json_util_has_child() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_has_child_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Test.
    mcl_error_t code = mcl_json_util_has_child(root, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for result = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root with an object.
 * WHEN  : mcl_json_util_has_child() is called.
 * THEN  : MCL_OK is returned and value of result parameter is MCL_TRUE.
 */
void test_has_child_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Add string object to root.
    char *object_name = "test_name";
    mcl_json_util_add_string(root, object_name, "test_value");

    mcl_bool_t result = MCL_FALSE;

    // Test.
    mcl_error_t code = mcl_json_util_has_child(root, &result);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_has_child() failed.");
    TEST_ASSERT_MESSAGE(MCL_TRUE == result, "Result is wrong.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root without an object.
 * WHEN  : mcl_json_util_has_child() is called.
 * THEN  : MCL_OK is returned and value of result parameter is MCL_FALSE.
 */
void test_has_child_004(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    mcl_bool_t result = MCL_FALSE;

    // Test.
    mcl_error_t code = mcl_json_util_has_child(root, &result);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_has_child() failed.");
    TEST_ASSERT_MESSAGE(MCL_FALSE == result, "Result is wrong.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : mcl_json_util_get_number_value() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_number_value_001(void)
{
    // Get the number value.
    mcl_int32_t number_value_to_be_received = 0;
    mcl_error_t code = mcl_json_util_get_number_value(MCL_NULL, &number_value_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for json_item = NULL.");
}

/**
 * GIVEN : Number value is MCL_NULL.
 * WHEN  : mcl_json_util_get_number_value() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_number_value_002(void)
{
    mcl_json_t *json= (mcl_json_t *) 0xC0DE;

    // Get the number value.
    mcl_error_t code = mcl_json_util_get_number_value(json, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for number_value = NULL.");
}

/**
 * GIVEN : Initialized root with object.
 * WHEN  : mcl_json_util_get_number_value() is called.
 * THEN  : MCL_OK is returned.
 */
void test_get_number_value_003(void)
{
    // Initialize json.
    mcl_json_t *json = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json);

    // Add number value to json.
    char *object_name = "number value";
    mcl_size_t number_value_to_be_added = 5;
    mcl_json_util_add_uint(json, object_name, number_value_to_be_added);

    // Get the number added child object of json.
    mcl_json_t *json_child = MCL_NULL;
    mcl_json_util_get_object_item(json, object_name, &json_child);

    // Get the number value.
    mcl_int32_t number_value_to_be_received = 0;
    mcl_error_t code = mcl_json_util_get_number_value(json_child, &number_value_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_get_number_value() failed.");

    // Test.
    TEST_ASSERT_EQUAL_INT_MESSAGE(number_value_to_be_added, number_value_to_be_received, "Wrong number value received.");

    // Clean up.
    mcl_json_util_destroy(&json);
}

/**
 * GIVEN : Initialized json object with string value.
 * WHEN  : mcl_json_util_get_number_value() is called.
 * THEN  : MCL_JSON_TYPE_MISMATCH is returned.
 */
void test_get_number_value_004(void)
{
    // Initialize root json object.
    mcl_json_t *json = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json);

    // Add object with string value to root.
    char *object_name = "string object";
    const char *string_value = "string value";
    mcl_json_util_add_string(json, object_name, string_value);

    // Get the child object from root.
    mcl_json_t *json_child = MCL_NULL;
    mcl_json_util_get_object_item(json, object_name, &json_child);

    // Try to get integer value from child json object.
    mcl_int32_t number_value_to_be_received = 0;
    mcl_error_t code = mcl_json_util_get_number_value(json_child, &number_value_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_JSON_TYPE_MISMATCH == code, "Type mismatch is not detected.");

    // Clean up.
    mcl_json_util_destroy(&json);
}

/**
 * GIVEN : Initialized json object with double value.
 * WHEN  : mcl_json_util_get_number_value() is called.
 * THEN  : MCL_OK is returned and the returned value is truncated version of double.
 */
void test_get_number_value_005(void)
{
    // Initialize root json object.
    mcl_json_t *json = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json);

    // Add object with string value to root.
    char *object_name = "double object";
    double double_value = 3.4;
    mcl_json_util_add_double(json, object_name, double_value);

    // Get the child object from root.
    mcl_json_t *json_child = MCL_NULL;
    mcl_json_util_get_object_item(json, object_name, &json_child);

    // Try to get integer value from child json object.
    mcl_int32_t integer_value_to_be_received = 0;
    mcl_error_t code = mcl_json_util_get_number_value(json_child, &integer_value_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_get_number_value() failed.");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(3, integer_value_to_be_received, "Value returned is not equal to expected value.");

    // Clean up.
    mcl_json_util_destroy(&json);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : mcl_json_util_get_double_value() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_double_value_001(void)
{
    // Get the double value.
    double double_value_to_be_received = 0;
    mcl_error_t code = mcl_json_util_get_double_value(MCL_NULL, &double_value_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for json_item = NULL.");
}

/**
 * GIVEN : Double value is MCL_NULL.
 * WHEN  : mcl_json_util_get_double_value() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_double_value_002(void)
{
    mcl_json_t *json = (mcl_json_t *) 0xC0DE;

    // Get the double value.
    mcl_error_t code = mcl_json_util_get_double_value(json, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for double_value = NULL.");
}

/**
 * GIVEN : Initialized root with object.
 * WHEN  : mcl_json_util_get_double_value() is called.
 * THEN  : MCL_OK is returned.
 */
void test_get_double_value_003(void)
{
    // Initialize json.
    mcl_json_t *json = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json);

    // Add double value to json.
    char *object_name = "double value";
    double double_value_to_be_added = 3.1;
    mcl_json_util_add_double(json, object_name, double_value_to_be_added);

    // Get the double value added child object of json.
    mcl_json_t *json_child = MCL_NULL;
    mcl_json_util_get_object_item(json, object_name, &json_child);

    // Get the double value.
    double double_value_to_be_received = 0;
    mcl_error_t code = mcl_json_util_get_double_value(json_child, &double_value_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_get_double_value() failed.");

    // Test.
    TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(double_value_to_be_added, double_value_to_be_received, "Wrong double value received.");

    // Clean up.
    mcl_json_util_destroy(&json);
}

/**
 * GIVEN : Initialized json object with string value.
 * WHEN  : mcl_json_util_get_double_value() is called.
 * THEN  : MCL_JSON_TYPE_MISMATCH is returned.
 */
void test_get_double_value_004(void)
{
    // Initialize json.
    mcl_json_t *json = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json);

    // Add json object with string value to root json.
    char* object_name = "string object";
    const char *string_value_to_be_added = "string value";
    mcl_json_util_add_string(json, object_name, string_value_to_be_added);

    // Get the json object from root json object.
    mcl_json_t *json_child = MCL_NULL;
    mcl_json_util_get_object_item(json, object_name, &json_child);

    // Try to get the value as double.
    double double_value_to_be_received = 0;
    mcl_error_t code = mcl_json_util_get_double_value(json_child, &double_value_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_JSON_TYPE_MISMATCH == code, "Type mismatch is not detected.");

    // Clean up.
    mcl_json_util_destroy(&json);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : mcl_json_util_get_bool_value() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_bool_value_001(void)
{
    mcl_bool_t bool_value;

    // Get the bool value.
    mcl_error_t code = mcl_json_util_get_bool_value(MCL_NULL, &bool_value);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for json = NULL.");
}

/**
 * GIVEN : Bool value is MCL_NULL.
 * WHEN  : mcl_json_util_get_bool_value() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_bool_value_002(void)
{
    mcl_json_t *json = (mcl_json_t *) 0xC0DE;

    // Get the bool value.
    mcl_error_t code = mcl_json_util_get_bool_value(json, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for bool_value = NULL.");
}

/**
 * GIVEN : Initialized json object with boolean true value.
 * WHEN  : mcl_json_util_get_bool_value() is called.
 * THEN  : MCL_OK is returned.
 */
void test_get_bool_value_003(void)
{
    // Initialize json.
    mcl_json_t *json = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json);

    // Add bool value to json.
    const char *object_name = "is motor enabled";
    mcl_bool_t motor_status = MCL_TRUE;
    mcl_json_util_add_bool(json, object_name, motor_status);

    // Get the bool value added child object of json.
    mcl_json_t *json_child = MCL_NULL;
    mcl_json_util_get_object_item(json, object_name, &json_child);

    // Get the bool value.
    mcl_bool_t motor_status_actual = MCL_FALSE;
    mcl_error_t code = mcl_json_util_get_bool_value(json_child, &motor_status_actual);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_json_util_get_bool_value() failed.");

    // Test.
    TEST_ASSERT_EQUAL_INT_MESSAGE((int) motor_status, motor_status_actual, "Wrong bool value received.");

    // Clean up.
    mcl_json_util_destroy(&json);
}

/**
 * GIVEN : Initialized json object with boolean false value.
 * WHEN  : mcl_json_util_get_bool_value() is called.
 * THEN  : MCL_OK is returned.
 */
void test_get_bool_value_004(void)
{
    // Initialize json.
    mcl_json_t *json = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json);

    // Add bool value to json.
    const char *object_name = "is motor enabled";
    mcl_bool_t motor_status = MCL_FALSE;
    mcl_json_util_add_bool(json, object_name, motor_status);

    // Get json object.
    mcl_json_t *json_child = MCL_NULL;
    mcl_json_util_get_object_item(json, object_name, &json_child);

    // Get the bool value.
    mcl_bool_t motor_status_actual = MCL_TRUE;
    mcl_error_t code = mcl_json_util_get_bool_value(json_child, &motor_status_actual);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_json_util_get_bool_value() failed.");
    TEST_ASSERT_EQUAL_INT_MESSAGE((int) motor_status, motor_status_actual, "Wrong bool value received.");

    // Clean up.
    mcl_json_util_destroy(&json);
}

/**
 * GIVEN : Initialized json object with double value.
 * WHEN  : mcl_json_util_get_bool_value() is called.
 * THEN  : MCL_JSON_TYPE_MISMATCH is returned.
 */
void test_get_bool_value_005(void)
{
    // Initialize json.
    mcl_json_t *json = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json);

    // Add double value to json.
    const char *object_name = "double value";
    double value = 3.4;
    mcl_json_util_add_double(json, object_name, value);

    // Get json object.
    mcl_json_t *json_child = MCL_NULL;
    mcl_json_util_get_object_item(json, object_name, &json_child);

    // Try to get the value of the json object as boolean.
    mcl_bool_t motor_status_actual;
    mcl_error_t code = mcl_json_util_get_bool_value(json_child, &motor_status_actual);
    TEST_ASSERT_MESSAGE(MCL_JSON_TYPE_MISMATCH == code, "Type mismatch is not detected.");

    // Clean up.
    mcl_json_util_destroy(&json);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : mcl_json_util_get_string() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_string_001(void)
{
    char *string_to_be_received = MCL_NULL;

    // Get the string.
    mcl_error_t code = mcl_json_util_get_string(MCL_NULL, &string_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for json_item = NULL.");
}

/**
 * GIVEN : String value is MCL_NULL.
 * WHEN  : mcl_json_util_get_string() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_get_string_002(void)
{
    mcl_json_t *json = (mcl_json_t *) 0xC0DE;

    // Get the string.
    mcl_error_t code = mcl_json_util_get_string(json, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for string_value = NULL.");
}

/**
 * GIVEN : Initialized root with object.
 * WHEN  : mcl_json_util_get_string() is called.
 * THEN  : MCL_OK is returned.
 */
void test_get_string_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Add string to root.
    char *name = "test_name";
    char *string_to_be_added = "test_value";

    mcl_error_t code = mcl_json_util_add_string(root, name, string_to_be_added);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_string() failed.");

    // Get the string added child object of json.
    mcl_json_t *json_child = MCL_NULL;
    mcl_json_util_get_object_item(root, name, &json_child);

    char *string_to_be_received = MCL_NULL;

    // Get the string.
    code = mcl_json_util_get_string(json_child, &string_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_get_string() failed.");

    // Test.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(string_to_be_added, string_to_be_received, "Wrong string received.");

    // Clean up.
    MCL_FREE(string_to_be_received);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized json object with double value.
 * WHEN  : mcl_json_util_get_string() is called.
 * THEN  : MCL_JSON_TYPE_MISMATCH is returned.
 */
void test_get_string_004(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Add double value to json.
    const char *object_name = "double value";
    double value = 3.4;
    mcl_json_util_add_double(root, object_name, value);

    // Get json object.
    mcl_json_t *json_child = MCL_NULL;
    mcl_json_util_get_object_item(root, object_name, &json_child);

    // Try to get the value as string.
    char *string_to_be_received = MCL_NULL;
    mcl_error_t code = mcl_json_util_get_string(json_child, &string_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_JSON_TYPE_MISMATCH == code, "Type mismatch is not detected.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : mcl_json_util_to_string() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_to_string_001(void)
{
    char *json_string = MCL_NULL;

    mcl_error_t code = mcl_json_util_to_string(MCL_NULL, &json_string);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Json string is MCL_NULL.
 * WHEN  : mcl_json_util_to_string() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_to_string_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Test.
    mcl_error_t code = mcl_json_util_to_string(root, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for json_string = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root with object.
 * WHEN  : mcl_json_util_to_string() is called.
 * THEN  : MCL_OK is returned.
 */
void test_to_string_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Add string to root.
    char *name = "test_name";
    char *value = "test_value";

    mcl_error_t code = mcl_json_util_add_string(root, name, value);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_string() failed.");

    // Add number value to json.
    char *object_name = "test_number";
    mcl_size_t number = 2;

    code = mcl_json_util_add_uint(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding integer to root failed.");

    // Test.
    char *json_string = MCL_NULL;
    code = mcl_json_util_to_string(root, &json_string);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_to_string() failed.");

    char *expected_json_root = "{\"test_name\":\"test_value\",\"test_number\":2}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_string, "Wrong string received.");

    // Clean up.
    MCL_FREE(json_string);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Json string is MCL_NULL.
 * WHEN  : mcl_json_util_parse() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_parse_001(void)
{
    mcl_json_t *root = MCL_NULL;
    mcl_error_t code = mcl_json_util_parse(MCL_NULL, 0, &root);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for json_string = NULL.");
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : mcl_json_util_parse() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_parse_002(void)
{
    char *json_to_be_parsed = "{\"type\":\"item\"}";

    mcl_error_t code = mcl_json_util_parse(json_to_be_parsed, 0, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Initialized root and json string.
 * WHEN  : mcl_json_util_parse() is called.
 * THEN  : MCL_OK is returned.
 */
void test_parse_003(void)
{
    char *json_to_be_parsed =
        "{\"type\":\"item\",\"version\":\"1.0\",\"payload\":{\"type\":\"standardTimeSeries\",\"version\":\"1.0\",\"details\":{\"configurationId\":\"e3217e2b-7036-49f2-9814-4c38542cd781\"}}}";

    mcl_json_t *root = MCL_NULL;
    mcl_error_t code = mcl_json_util_parse(json_to_be_parsed, 0, &root);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_parse() failed.");

    // Get payload object.
    mcl_json_t *json_child_1 = MCL_NULL;
    mcl_json_util_get_object_item(root, "payload", &json_child_1);

    // Get type object of payload.
    mcl_json_t *json_child_2 = MCL_NULL;
    mcl_json_util_get_object_item(json_child_1, "type", &json_child_2);

    // json_child_2 => "type\":"standardTimeSeries"
    char *string_to_be_received = MCL_NULL;
    mcl_json_util_get_string(json_child_2, &string_to_be_received);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("standardTimeSeries", string_to_be_received, "Received json string fail.");

    // Clean up.
    MCL_FREE(string_to_be_received);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Source json is MCL_NULL.
 * WHEN  : mcl_json_util_duplicate() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_duplicate_001(void)
{
    mcl_json_t *duplicated_json = MCL_NULL;

    mcl_error_t code = mcl_json_util_duplicate(MCL_NULL, &duplicated_json);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "It should have returned MCL_TRIGGERED_WITH_NULL for source_json = NULL.");
}

/**
 * GIVEN : Duplicated json is MCL_NULL.
 * WHEN  : mcl_json_util_duplicate() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_duplicate_002(void)
{
    const mcl_json_t *source_json = (mcl_json_t *) 0xC0DE;

    mcl_error_t code = mcl_json_util_duplicate(source_json, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "It should have returned MCL_TRIGGERED_WITH_NULL for duplicated_json = NULL.");
}

/**
 * GIVEN : Initialized source json and duplicated json is valid.
 * WHEN  : mcl_json_util_duplicate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_duplicate_003(void)
{
    // Get a json source to be duplicated.
    mcl_json_t *root = MCL_NULL;
    char *json_to_be_parsed = "{\"type\":\"item\",\"version\":\"1.0\"}";
    mcl_json_util_parse(json_to_be_parsed, 0, &root);

    mcl_json_t *duplicated_json = MCL_NULL;

    mcl_error_t code = mcl_json_util_duplicate(root, &duplicated_json);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_json_util_duplicate() failed.");

    // Version object.
    mcl_json_t *json_version = MCL_NULL;
    mcl_json_util_get_object_item(root, "version", &json_version);

    // json_version => "version\":"1.0"
    char *string_to_be_received = MCL_NULL;
    mcl_json_util_get_string(json_version, &string_to_be_received);

    // Check a field of duplicated json.
    TEST_ASSERT_EQUAL_STRING_MESSAGE("1.0", string_to_be_received, "Received json string fail.");

    // Clean up.
    MCL_FREE(string_to_be_received);
    mcl_json_util_destroy(&root);
    mcl_json_util_destroy(&duplicated_json);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : mcl_json_util_destroy() is called.
 * THEN  : Root is destroyed and set to NULL.
 */
void test_destroy_001(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Add string to root.
    char *name = "test_name";
    char *value = "test_value";
    mcl_json_util_add_string(root, name, value);

    // Test.
    mcl_json_util_destroy(&root);

    // Check root. It should be NULL.
    TEST_ASSERT_NULL_MESSAGE(root, "root should have been NULL.")
}
