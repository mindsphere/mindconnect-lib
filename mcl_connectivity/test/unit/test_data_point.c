/*!
 * @file     test_data_point.c
 * @brief    This file implements all test cases of mcl_data_point module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_connectivity/mcl_data_point.h"
#include "data_point.h"
#include "mcl_core/mcl_log_util.h"
#include "mcl_core/mcl_json_util.h"
#include "mcl_core/mcl_memory.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : No initial condition.
 * WHEN  : mcl_data_point_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized data point struct.
 */
void test_initialize_001(void)
{
    mcl_data_point_t *data_point = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_data_point_initialize(&data_point);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data point");
    TEST_ASSERT_NOT_NULL_MESSAGE(data_point, "data point is NULL");

    // Clean up.
    mcl_data_point_destroy(&data_point);
}

/**
 * GIVEN : mcl_data_point parameter is given as MCL_NULL.
 * WHEN  : mcl_data_point_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002(void)
{
    mcl_error_t code;

    // Test.
    code = mcl_data_point_initialize(MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_point_initialize() unexpected return error.");
}

/**
 * GIVEN : data_point parameter is NULL.
 * WHEN  : mcl_data_point_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_001()
{
    const char *dummy_value = "dummy";
    mcl_error_t code = mcl_data_point_set_parameter(MCL_NULL, MCL_DATA_POINT_PARAMETER_NAME, dummy_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_point_set_parameter() failed when data point is NULL.");
}

/**
 * GIVEN : value parameter is NULL.
 * WHEN  : mcl_data_point_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_002()
{
    mcl_data_point_t data_point;
    mcl_error_t code = mcl_data_point_set_parameter(&data_point, MCL_DATA_POINT_PARAMETER_NAME, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_point_set_parameter() failed when value is NULL.");
}

/**
 * GIVEN : Invalid parameter.
 * WHEN  : mcl_data_point_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_003()
{
    mcl_data_point_t data_point;
    const char *dummy_value = "dummy";
    mcl_error_t code = mcl_data_point_set_parameter(&data_point, MCL_DATA_POINT_PARAMETER_END, dummy_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_data_point_set_parameter() failed when parameter is invalid.");
}

/**
 * GIVEN : Id parameter.
 * WHEN  : mcl_data_point_set_parameter() is called.
 * THEN  : MCL_OK is returned and new id parameter of the data point is set.
 */
void test_set_parameter_004()
{
    // Initialize data point.
    mcl_data_point_t *data_point;
    mcl_error_t code = mcl_data_point_initialize(&data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data point");

    // Set parameter.
    const char *id = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
    code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_ID, id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(id, data_point->id, "Id parameter is wrong.");

    // Clean up.
    mcl_data_point_destroy(&data_point);
}

/**
 * GIVEN : Name parameter.
 * WHEN  : mcl_data_point_set_parameter() is called.
 * THEN  : MCL_OK is returned and new name parameter of the data point is set.
 */
void test_set_parameter_005()
{
    // Initialize data point.
    mcl_data_point_t *data_point;
    mcl_error_t code = mcl_data_point_initialize(&data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data point");

    // Set parameter.
    const char *name = "Speed";
    code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_NAME, name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(name, data_point->name, "Name parameter is wrong.");

    // Clean up.
    mcl_data_point_destroy(&data_point);
}

/**
 * GIVEN : Description parameter.
 * WHEN  : mcl_data_point_set_parameter() is called.
 * THEN  : MCL_OK is returned and new description parameter of the data point is set.
 */
void test_set_parameter_006()
{
    // Initialize data point.
    mcl_data_point_t *data_point;
    mcl_error_t code = mcl_data_point_initialize(&data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data point");

    // Set parameter.
    const char *description = "Test Rack";
    code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_DESCRIPTION, description);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(description, data_point->description, "Description parameter is wrong.");

    // Clean up.
    mcl_data_point_destroy(&data_point);
}

/**
 * GIVEN : Type parameter.
 * WHEN  : mcl_data_point_set_parameter() is called.
 * THEN  : MCL_OK is returned and new type parameter of the data point is set.
 */
void test_set_parameter_007()
{
    // Initialize data point.
    mcl_data_point_t *data_point;
    mcl_error_t code = mcl_data_point_initialize(&data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data point");

    // Set parameter.
    const char *type = "uint";
    code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_TYPE, type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(type, data_point->type, "Type parameter is wrong.");

    // Clean up.
    mcl_data_point_destroy(&data_point);
}

/**
 * GIVEN : Unit parameter.
 * WHEN  : mcl_data_point_set_parameter() is called.
 * THEN  : MCL_OK is returned and new unit parameter of the data point is set.
 */
void test_set_parameter_008()
{
    // Initialize data point.
    mcl_data_point_t *data_point;
    mcl_error_t code = mcl_data_point_initialize(&data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data point");

    // Set parameter.
    const char *unit = "rpm";
    code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_UNIT, unit);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(unit, data_point->unit, "Unit parameter is wrong.");

    // Clean up.
    mcl_data_point_destroy(&data_point);
}

/**
 * GIVEN : Custom data parameter.
 * WHEN  : mcl_data_point_set_parameter() is called.
 * THEN  : MCL_OK is returned and new custom data parameter of the data point is set.
 */
void test_set_parameter_009()
{
    // Initialize data point.
    mcl_data_point_t *data_point;
    mcl_error_t code = mcl_data_point_initialize(&data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data point");

    // Create custom data as json.
    mcl_json_t *custom_data = MCL_NULL;
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for custom_data.");

    code = mcl_json_util_add_string(custom_data, "motor status", "need maintenance");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "String could not be added to the json object.");

    // Set parameter.
    code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_CUSTOM_DATA, custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");

    // Check custom data.
    char *actual_custom_data = MCL_NULL;
    code = mcl_json_util_to_string(data_point->custom_data, &actual_custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Json could not be converted to string.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("{\"motor status\":\"need maintenance\"}", actual_custom_data, "Custom data parameter could not be set.");

    // Clean up.
    mcl_data_point_destroy(&data_point);
    mcl_json_util_destroy(&custom_data);
    MCL_FREE(actual_custom_data);
}

/**
 * GIVEN : Mandatory data point parameters.
 * WHEN  : data_point_validate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_data_point_validate_001()
{
    // Initialize data point.
    mcl_data_point_t *data_point;
    mcl_error_t code = mcl_data_point_initialize(&data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data point");

    // Set id parameter.
    const char *id = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
    code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_ID, id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");

    // Set name parameter.
    const char *name = "Speed";
    code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_NAME, name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");

    // Set type parameter.
    const char *type = "uint";
    code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_TYPE, type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");

    // Set unit parameter.
    const char *unit = "rpm";
    code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_UNIT, unit);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");

    // Test.
    code = data_point_validate(data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "data_point_validate() failed.");

    // Clean up.
    mcl_data_point_destroy(&data_point);
}

/**
 * GIVEN : Mandatory data point parameters are not set.
 * WHEN  : data_point_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_data_point_validate_002()
{
    // Initialize data point.
    mcl_data_point_t *data_point;
    mcl_error_t code = mcl_data_point_initialize(&data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data point");

    // Test.
    code = data_point_validate(data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "data_point_validate() failed when mandatory parameters are NULL.");

    // Clean up.
    mcl_data_point_destroy(&data_point);
}
