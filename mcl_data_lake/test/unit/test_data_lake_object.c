/*!
 * @file     test_data_lake_object.c
 * @brief    This file implements all test cases of mcl_data_lake_object module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_data_lake/mcl_data_lake_object.h"
#include "data_lake_object.h"

static void _dummy_upload_callback();

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : No initial condition.
 * WHEN  : mcl_data_lake_object_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized mcl_data_lake_object_t struct.
 */
void test_initialize_001(void)
{
    // Variable declaration.
    mcl_data_lake_object_t *object = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_data_lake_object_initialize(&object);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for object.");
    TEST_ASSERT_NOT_NULL_MESSAGE(object, "Object should have not been NULL.");
    TEST_ASSERT_NULL_MESSAGE(object->path, "Path should have been NULL.");
    TEST_ASSERT_NULL_MESSAGE(object->signed_url, "Signed url should have been NULL.");
    TEST_ASSERT_NULL_MESSAGE(object->user_context, "User context should have been NULL.");
    TEST_ASSERT_NULL_MESSAGE(object->upload_callback, "Upload callback should have been NULL.");
    TEST_ASSERT_EQUAL_MESSAGE(0, object->size, "Object size should have been 0.");

    // Clean up.
    mcl_data_lake_object_destroy(&object);
}

/**
 * GIVEN : Object parameter is NULL.
 * WHEN  : mcl_data_lake_object_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002(void)
{
    // Variable declaration.
    mcl_error_t code;

    // Test.
    code = mcl_data_lake_object_initialize(MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_lake_object_initialize() should have returned MCL_TRIGGERED_WITH_NULL.");
}

/**
 * GIVEN : Object parameter is NULL.
 * WHEN  : mcl_data_lake_object_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_001(void)
{
    // Variable declaration.
    const char *dummy_path = "/dummy_path";

    // Test.
    mcl_error_t code = mcl_data_lake_object_set_parameter(MCL_NULL, MCL_DATA_LAKE_OBJECT_PARAMETER_PATH, dummy_path);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_lake_object_set_parameter() should have returned MCL_TRIGGERED_WITH_NULL.");
}

/**
 * GIVEN : Value parameter is NULL.
 * WHEN  : mcl_data_lake_object_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_002(void)
{
    // Variable declaration.
    mcl_data_lake_object_t *object;

    // Initialize data lake object.
    mcl_error_t code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake object.");

    // Test.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_PATH, MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_lake_object_set_parameter() should have returned MCL_TRIGGERED_WITH_NULL.");

    // Clean up.
    mcl_data_lake_object_destroy(&object);
}

/**
 * GIVEN : Invalid parameter to set.
 * WHEN  : mcl_data_lake_object_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_003()
{
    // Variable declaration.
    mcl_data_lake_object_t *object;
    char *dummy_path = "dummy_path";

    // Initialize data lake object.
    mcl_error_t code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake object.");

    // Set parameter.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_END, dummy_path);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_data_lake_object_set_parameter() should have returned MCL_INVALID_PARAMETER.");

    // Clean up.
    mcl_data_lake_object_destroy(&object);
}

/**
 * GIVEN : Path parameter to set.
 * WHEN  : mcl_data_lake_object_set_parameter() is called.
 * THEN  : MCL_OK is returned and path parameter of the data lake object is set.
 */
void test_set_parameter_004()
{
    // Initialize data lake object.
    mcl_data_lake_object_t *object;
    mcl_error_t code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake object.");

    // Test.
    const char *dummy_path = "dummy_path";
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_PATH, dummy_path);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(dummy_path, object->path, "Path parameter is wrong.");

    // Clean up.
    mcl_data_lake_object_destroy(&object);
}

/**
 * GIVEN : Upload callback parameter to set.
 * WHEN  : mcl_data_lake_object_set_parameter() is called.
 * THEN  : MCL_OK is returned and upload callback parameter of the data lake object is set.
 */
void test_set_parameter_005()
{
    // Initialize data lake object.
    mcl_data_lake_object_t *object;
    mcl_error_t code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake object.");

    // Test.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_UPLOAD_CALLBACK, _dummy_upload_callback);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(_dummy_upload_callback, object->upload_callback, "Upload callback parameter is wrong.");

    // Clean up.
    mcl_data_lake_object_destroy(&object);
}

/**
 * GIVEN : User context parameter to set.
 * WHEN  : mcl_data_lake_object_set_parameter() is called.
 * THEN  : MCL_OK is returned and user context parameter of the data lake object is set.
 */
void test_set_parameter_006()
{
    // Initialize data lake object.
    mcl_data_lake_object_t *object;
    mcl_error_t code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake object.");

    // Test.
    void *dummy_context = (void *) 0xABCD;
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_USER_CONTEXT, dummy_context);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(dummy_context, object->user_context, "User context parameter is wrong.");

    // Clean up.
    mcl_data_lake_object_destroy(&object);
}

/**
 * GIVEN : Payload size parameter to set.
 * WHEN  : mcl_data_lake_object_set_parameter() is called.
 * THEN  : MCL_OK is returned and size parameter of the data lake object is set.
 */
void test_set_parameter_007()
{
    // Initialize data lake object.
    mcl_data_lake_object_t *object;
    mcl_error_t code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake object.");

    // Test.
    mcl_size_t object_size = 0xABCD;
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_SIZE, &object_size);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");
    TEST_ASSERT_EQUAL_MESSAGE(object_size, object->size, "Object size is wrong.");

    // Clean up.
    mcl_data_lake_object_destroy(&object);
}

/**
 * GIVEN : Signed url parameter to set.
 * WHEN  : data_lake_object_set_internal_parameter() is called.
 * THEN  : MCL_OK is returned and signed url parameter of the data lake object is set.
 */
void test_set_internal_parameter_001()
{
    // Initialize data lake object.
    mcl_data_lake_object_t *object;
    mcl_error_t code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake object.");

    // Test.
    const char *dummy_signed_url = "dummy_signed_url";
    code = data_lake_object_set_internal_parameter(object, DATA_LAKE_OBJECT_INTERNAL_PARAMETER_SIGNED_URL, dummy_signed_url);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(dummy_signed_url, object->signed_url, "Signed url parameter is wrong.");

    // Clean up.
    mcl_data_lake_object_destroy(&object);
}

/**
 * GIVEN : Invalid parameter to set.
 * WHEN  : data_lake_object_set_internal_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_internal_parameter_002()
{
    // Initialize data lake object.
    mcl_data_lake_object_t *object;
    mcl_error_t code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake object.");

    // Test.
    const char *dummy_signed_url = "dummy_signed_url";
    code = data_lake_object_set_internal_parameter(object, DATA_LAKE_OBJECT_INTERNAL_PARAMETER_END, dummy_signed_url);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "data_lake_object_set_internal_parameter() should have returned MCL_INVALID_PARAMETER.");

    // Clean up.
    mcl_data_lake_object_destroy(&object);
}

static void _dummy_upload_callback()
{

}

/**
 * GIVEN : Mandatory (all) data lake object parameters.
 * WHEN  : data_lake_object_validate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_validate_001()
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_object_t *object = MCL_NULL;
    const char *dummy_signed_url = "dummy_signed_url";
    char *dummy_path = "dummy_path";
    mcl_size_t object_size = 0xABCD;

    // Initialize data lake object.
    code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for object.");

    // Set upload callback parameter.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_UPLOAD_CALLBACK, _dummy_upload_callback);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() should have returned MCL_OK for upload callback.");

    // Set path.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_PATH, dummy_path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() should have returned MCL_OK for path.");

    // Set signed url.
    code = data_lake_object_set_internal_parameter(object, DATA_LAKE_OBJECT_INTERNAL_PARAMETER_SIGNED_URL, dummy_signed_url);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "data_lake_object_set_internal_parameter() should have returned MCL_OK for signed url.");

    // Set payload size.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_SIZE, &object_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() should have returned MCL_OK for object size.");

    // Test.
    code = data_lake_object_validate(object);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "data_lake_object_validate() failed.");

    // Clean up.
    mcl_data_lake_object_destroy(&object);
}

/**
 * GIVEN : Upload callback parameter of data lake object is not set.
 * WHEN  : data_lake_object_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_validate_002()
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_object_t *object = MCL_NULL;
    const char *dummy_signed_url = "dummy_signed_url";
    char *dummy_path = "dummy_path";
    mcl_size_t object_size = 0xABCD;

    // Initialize data lake object.
    code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for object.");

    // Set path.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_PATH, dummy_path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() should have returned MCL_OK for path.");

    // Set signed url.
    code = data_lake_object_set_internal_parameter(object, DATA_LAKE_OBJECT_INTERNAL_PARAMETER_SIGNED_URL, dummy_signed_url);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "data_lake_object_set_internal_parameter() should have returned MCL_OK for signed url.");

    // Set payload size.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_SIZE, &object_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() should have returned MCL_OK for object size.");

    // Test.
    code = data_lake_object_validate(object);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "data_lake_object_validate() should have returned MCL_INVALID_PARAMETER.");

    // Clean up.
    mcl_data_lake_object_destroy(&object);
}

/**
 * GIVEN : Path parameter of data lake object is not set.
 * WHEN  : data_lake_object_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_validate_003()
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_object_t *object = MCL_NULL;
    const char *dummy_signed_url = "dummy_signed_url";
    mcl_size_t object_size = 0xABCD;

    // Initialize data lake object.
    code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for object.");

    // Set upload callback parameter.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_UPLOAD_CALLBACK, _dummy_upload_callback);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() should have returned MCL_OK for upload callback.");

    // Set signed url.
    code = data_lake_object_set_internal_parameter(object, DATA_LAKE_OBJECT_INTERNAL_PARAMETER_SIGNED_URL, dummy_signed_url);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "data_lake_object_set_internal_parameter() should have returned MCL_OK for signed url.");

    // Set payload size.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_SIZE, &object_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() should have returned MCL_OK for object size.");

    // Test.
    code = data_lake_object_validate(object);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "data_lake_object_validate() should have returned MCL_INVALID_PARAMETER.");

    // Clean up.
    mcl_data_lake_object_destroy(&object);
}

/**
 * GIVEN : Signed URL parameter of data lake object is not set.
 * WHEN  : data_lake_object_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_validate_004()
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_object_t *object = MCL_NULL;
    char *dummy_path = "dummy_path";
    mcl_size_t object_size = 0xABCD;

    // Initialize data lake object.
    code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for object.");

    // Set upload callback parameter.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_UPLOAD_CALLBACK, _dummy_upload_callback);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() should have returned MCL_OK for upload callback.");

    // Set path.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_PATH, dummy_path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() should have returned MCL_OK for path.");

    // Set payload size.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_SIZE, &object_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() should have returned MCL_OK for object size.");

    // Test.
    code = data_lake_object_validate(object);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "data_lake_object_validate() should have returned MCL_INVALID_PARAMETER.");

    // Clean up.
    mcl_data_lake_object_destroy(&object);
}

/**
 * GIVEN : Payload size parameter of data lake object is not set.
 * WHEN  : data_lake_object_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_validate_005()
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_object_t *object = MCL_NULL;
    const char *dummy_signed_url = "dummy_signed_url";
    char *dummy_path = "dummy_path";

    // Initialize data lake object.
    code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for object.");

    // Set upload callback parameter.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_UPLOAD_CALLBACK, _dummy_upload_callback);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() should have returned MCL_OK for upload callback.");

    // Set path.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_PATH, dummy_path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() should have returned MCL_OK for path.");

    // Set signed url.
    code = data_lake_object_set_internal_parameter(object, DATA_LAKE_OBJECT_INTERNAL_PARAMETER_SIGNED_URL, dummy_signed_url);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "data_lake_object_set_internal_parameter() should have returned MCL_OK for signed url.");

    // Test.
    code = data_lake_object_validate(object);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "data_lake_object_validate() should have returned MCL_INVALID_PARAMETER.");

    // Clean up.
    mcl_data_lake_object_destroy(&object);
}
