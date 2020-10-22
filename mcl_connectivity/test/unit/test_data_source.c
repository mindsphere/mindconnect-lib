/*!
 * @file     test_data_source.c
 * @brief    This file implements all test cases of mcl_data_source module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_connectivity/mcl_data_source.h"
#include "data_source.h"
#include "mcl_connectivity/mcl_data_point.h"
#include "data_point.h"
#include "mcl_core/mcl_log_util.h"
#include "mcl_core/mcl_list.h"
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
 * WHEN  : mcl_data_source_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized data source struct.
 */
void test_initialize_001(void)
{
    mcl_data_source_t *data_source = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_data_source_initialize(&data_source);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source");
    TEST_ASSERT_NOT_NULL_MESSAGE(data_source, "data source is NULL");

    // Clean up.
    mcl_data_source_destroy(&data_source);
}

/**
 * GIVEN : mcl_data_source parameter is given as MCL_NULL.
 * WHEN  : mcl_data_source_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002(void)
{
    mcl_error_t code;

    // Test.
    code = mcl_data_source_initialize(MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_source_initialize() unexpected return error.");
}

/**
 * GIVEN : Data source is NULL.
 * WHEN  : mcl_data_source_add_data_point() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_data_point_001(void)
{
    mcl_data_point_t data_point;
    mcl_error_t code = mcl_data_source_add_data_point(MCL_NULL, &data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_source_add_data_point() unexpected return error.");
}

/**
 * GIVEN : Data point is NULL.
 * WHEN  : mcl_data_source_add_data_point() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_data_point_002(void)
{
    mcl_data_source_t data_source;
    mcl_error_t code = mcl_data_source_add_data_point(&data_source, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_source_add_data_point() unexpected return error.");
}

/**
 * GIVEN : Data source and data point.
 * WHEN  : mcl_data_source_add_data_point() is called.
 * THEN  : MCL_OK is returned and data point is added to data source.
 */
void test_add_data_point_003(void)
{
    // Initialize data source.
    mcl_data_source_t *data_source = MCL_NULL;
    mcl_error_t code = mcl_data_source_initialize(&data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source");

    // Initialize data point.
    mcl_data_point_t *data_point;
    code = mcl_data_point_initialize(&data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data point");

    // Add data point to data source.
    code = mcl_data_source_add_data_point(data_source, data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding data point to data source failed.");
    TEST_ASSERT_EQUAL_PTR(data_point, data_source->data_points->head->data);

    // Clean up.
    mcl_data_source_destroy(&data_source);
}

/**
 * GIVEN : Data source parameter is NULL.
 * WHEN  : mcl_data_source_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_001()
{
    const char *dummy_value = "dummy";
    mcl_error_t code = mcl_data_source_set_parameter(MCL_NULL, MCL_DATA_SOURCE_PARAMETER_NAME, dummy_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_source_set_parameter() failed when data source is NULL.");
}

/**
 * GIVEN : Value parameter is NULL.
 * WHEN  : mcl_data_source_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_002()
{
    mcl_data_source_t data_source;
    mcl_error_t code = mcl_data_source_set_parameter(&data_source, MCL_DATA_SOURCE_PARAMETER_NAME, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_source_set_parameter() failed when value is NULL.");
}

/**
 * GIVEN : Invalid parameter.
 * WHEN  : mcl_data_source_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_003()
{
    mcl_data_source_t data_source;
    const char *dummy_value = "dummy";
    mcl_error_t code = mcl_data_source_set_parameter(&data_source, 10, dummy_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_data_source_set_parameter() failed when parameter is invalid.");
}

/**
 * GIVEN : Name parameter.
 * WHEN  : mcl_data_source_set_parameter() is called.
 * THEN  : MCL_OK is returned and new name parameter of the data source is set.
 */
void test_set_parameter_004()
{
    // Initialize data source.
    mcl_data_source_t *data_source;
    mcl_error_t code = mcl_data_source_initialize(&data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source");

    // Set parameter.
    const char *name = "Main motor";
    code = mcl_data_source_set_parameter(data_source, MCL_DATA_SOURCE_PARAMETER_NAME, name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_source_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(name, data_source->name, "Name parameter is wrong.");

    // Clean up.
    mcl_data_source_destroy(&data_source);
}

/**
 * GIVEN : Description parameter.
 * WHEN  : mcl_data_source_set_parameter() is called.
 * THEN  : MCL_OK is returned and new description parameter of the data source is set.
 */
void test_set_parameter_005()
{
    // Initialize data source.
    mcl_data_source_t *data_source;
    mcl_error_t code = mcl_data_source_initialize(&data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source");

    // Set parameter.
    const char *description = "Test Rack";
    code = mcl_data_source_set_parameter(data_source, MCL_DATA_SOURCE_PARAMETER_DESCRIPTION, description);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_source_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(description, data_source->description, "Description parameter is wrong.");

    // Clean up.
    mcl_data_source_destroy(&data_source);
}

/**
 * GIVEN : Custom data parameter.
 * WHEN  : mcl_data_source_set_parameter() is called.
 * THEN  : MCL_OK is returned and new custom data parameter of the data source is set.
 */
void test_set_parameter_006()
{
    // Initialize data source.
    mcl_data_source_t *data_source;
    mcl_error_t code = mcl_data_source_initialize(&data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source");

    // Create custom data as json.
    mcl_json_t *custom_data = MCL_NULL;
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for custom_data.");

    code = mcl_json_util_add_string(custom_data, "motor efficiency", "good");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "String could not be added to the json object.");

    // Set parameter.
    code = mcl_data_source_set_parameter(data_source, MCL_DATA_SOURCE_PARAMETER_CUSTOM_DATA, custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_source_set_parameter() failed.");

    // Check custom data.
    char *actual_custom_data = MCL_NULL;
    code = mcl_json_util_to_string(data_source->custom_data, &actual_custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Json could not be converted to string.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("{\"motor efficiency\":\"good\"}", actual_custom_data, "Custom data parameter could not be set.");

    // Clean up.
    mcl_data_source_destroy(&data_source);
    mcl_json_util_destroy(&custom_data);
    MCL_FREE(actual_custom_data);
}

/**
 * GIVEN : Mandatory data source parameters.
 * WHEN  : data_source_validate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_data_source_validate_001()
{
    // Initialize data source.
    mcl_data_source_t *data_source = MCL_NULL;
    mcl_error_t code = mcl_data_source_initialize(&data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source");

    // Initialize data points.
    mcl_data_point_t *data_point_1;
    mcl_data_point_t *data_point_2;
    code = mcl_data_point_initialize(&data_point_1);
    (MCL_OK == code) && (code = mcl_data_point_initialize(&data_point_2));
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data points");

    // Set id parameter.
    const char *id_1 = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
    const char *id_2 = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1e";
    code = mcl_data_point_set_parameter(data_point_1, MCL_DATA_POINT_PARAMETER_ID, id_1);
    (MCL_OK == code) && (code = mcl_data_point_set_parameter(data_point_2, MCL_DATA_POINT_PARAMETER_ID, id_2));
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");

    // Set name parameter.
    const char *name_1 = "Speed_1";
    const char *name_2 = "Speed_2";
    code = mcl_data_point_set_parameter(data_point_1, MCL_DATA_POINT_PARAMETER_NAME, name_1);
    (MCL_OK == code) && (code = mcl_data_point_set_parameter(data_point_2, MCL_DATA_POINT_PARAMETER_NAME, name_2));
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");

    // Set type parameter.
    const char *type_1 = "uint_1";
    const char *type_2 = "uint_2";
    code = mcl_data_point_set_parameter(data_point_1, MCL_DATA_POINT_PARAMETER_TYPE, type_1);
    code = mcl_data_point_set_parameter(data_point_2, MCL_DATA_POINT_PARAMETER_TYPE, type_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");

    // Set unit parameter.
    const char *unit_1 = "rpm_1";
    const char *unit_2 = "rpm_2";
    code = mcl_data_point_set_parameter(data_point_1, MCL_DATA_POINT_PARAMETER_UNIT, unit_1);
    code = mcl_data_point_set_parameter(data_point_2, MCL_DATA_POINT_PARAMETER_UNIT, unit_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");

    // Add data points to data source.
    code = mcl_data_source_add_data_point(data_source, data_point_1);
    (MCL_OK == code) && (code = mcl_data_source_add_data_point(data_source, data_point_2));
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding data points to data source failed.");

    // Set name parameter.
    const char *name = "Main motor";
    code = mcl_data_source_set_parameter(data_source, MCL_DATA_SOURCE_PARAMETER_NAME, name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_source_set_parameter() failed.");

    // Test.
    code = data_source_validate(data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "data_source_validate() failed.");

    // Clean up.
    mcl_data_source_destroy(&data_source);
}

/**
 * GIVEN : No data points are added to data source.
 * WHEN  : data_source_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_data_source_validate_002()
{
    // Initialize data source.
    mcl_data_source_t *data_source;
    mcl_error_t code = mcl_data_source_initialize(&data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source");

    // Set name parameter.
    const char *name = "Main motor";
    code = mcl_data_source_set_parameter(data_source, MCL_DATA_SOURCE_PARAMETER_NAME, name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_source_set_parameter() failed.");

    // Test.
    code = data_source_validate(data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "data_source_validate() function failed to detect invalid data source.");

    // Clean up.
    mcl_data_source_destroy(&data_source);
}

/**
 * GIVEN : Name of the data source is not set.
 * WHEN  : data_source_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_data_source_validate_003()
{
    // Initialize data source.
    mcl_data_source_t *data_source;
    mcl_error_t code = mcl_data_source_initialize(&data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source");

    // Test.
    code = data_source_validate(data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "data_source_validate() failed to detect invalid data source.");

    // Clean up.
    mcl_data_source_destroy(&data_source);
}
