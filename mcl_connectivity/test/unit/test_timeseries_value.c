/*!
 * @file     test_timeseries_value.c
 * @brief    This file implements all test cases of mcl_timeseries_value module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_connectivity/mcl_timeseries_value.h"
#include "timeseries_value.h"
#include "mcl_core/mcl_memory.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : No initial condition.
 * WHEN  : mcl_timeseries_value_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized timeseries value struct.
 */
void test_initialize_001(void)
{
    mcl_timeseries_value_t *timeseries_value = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_timeseries_value_initialize(&timeseries_value);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value.");
    TEST_ASSERT_NOT_NULL_MESSAGE(timeseries_value, "Timeseries value is NULL.");

    // Clean up.
    mcl_timeseries_value_destroy(&timeseries_value);
}

/**
 * GIVEN : mcl_timeseries_value parameter is given as MCL_NULL.
 * WHEN  : mcl_timeseries_value_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002(void)
{
    // Test.
    mcl_error_t code = mcl_timeseries_value_initialize(MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_timeseries_value_initialize() unexpected return error.");
}

/**
 * GIVEN : timeseries_value parameter is NULL.
 * WHEN  : mcl_timeseries_value_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_001()
{
    const char *dummy_value = "dummy";
    mcl_error_t code = mcl_timeseries_value_set_parameter(MCL_NULL, MCL_TIMESERIES_VALUE_PARAMETER_DATA_POINT_ID, dummy_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_timeseries_value_set_parameter() failed when timeseries value is NULL.");
}

/**
 * GIVEN : value parameter is NULL.
 * WHEN  : mcl_timeseries_value_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_002()
{
    mcl_timeseries_value_t timeseries_value;
    mcl_error_t code = mcl_timeseries_value_set_parameter(&timeseries_value, MCL_TIMESERIES_VALUE_PARAMETER_DATA_POINT_ID, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_timeseries_value_set_parameter() failed when value is NULL.");
}

/**
 * GIVEN : Invalid parameter.
 * WHEN  : mcl_timeseries_value_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_003()
{
    mcl_timeseries_value_t timeseries_value;
    const char *dummy_value = "dummy";
    mcl_error_t code = mcl_timeseries_value_set_parameter(&timeseries_value, MCL_TIMESERIES_VALUE_PARAMETER_END, dummy_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_timeseries_value_set_parameter() failed when parameter is invalid.");
}

/**
 * GIVEN : Data point id parameter.
 * WHEN  : mcl_timeseries_value_set_parameter() is called.
 * THEN  : MCL_OK is returned and new data point id parameter of the timeseries value is set.
 */
void test_set_parameter_004()
{
    // Initialize timeseries value.
    mcl_timeseries_value_t *timeseries_value;
    mcl_error_t code = mcl_timeseries_value_initialize(&timeseries_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value.");

    // Set parameter.
    const char *data_point_id = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
    code = mcl_timeseries_value_set_parameter(timeseries_value, MCL_TIMESERIES_VALUE_PARAMETER_DATA_POINT_ID, data_point_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data_point_id, timeseries_value->data_point_id, "Data point id parameter is wrong.");

    // Clean up.
    mcl_timeseries_value_destroy(&timeseries_value);
}

/**
 * GIVEN : Value parameter.
 * WHEN  : mcl_timeseries_value_set_parameter() is called.
 * THEN  : MCL_OK is returned and new value parameter of the timeseries value is set.
 */
void test_set_parameter_005()
{
    // Initialize timeseries value.
    mcl_timeseries_value_t *timeseries_value;
    mcl_error_t code = mcl_timeseries_value_initialize(&timeseries_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value.");

    // Set parameter.
    const char *value = "45";
    code = mcl_timeseries_value_set_parameter(timeseries_value, MCL_TIMESERIES_VALUE_PARAMETER_VALUE, value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(value, timeseries_value->value, "Value parameter is wrong.");

    // Clean up.
    mcl_timeseries_value_destroy(&timeseries_value);
}

/**
 * GIVEN : Quality code parameter.
 * WHEN  : mcl_timeseries_value_set_parameter() is called.
 * THEN  : MCL_OK is returned and new quality code parameter of the timeseries value is set.
 */
void test_set_parameter_006()
{
    // Initialize timeseries value.
    mcl_timeseries_value_t *timeseries_value;
    mcl_error_t code = mcl_timeseries_value_initialize(&timeseries_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value.");

    // Set parameter.
    const char *quality_code = "00000000";
    code = mcl_timeseries_value_set_parameter(timeseries_value, MCL_TIMESERIES_VALUE_PARAMETER_QUALITY_CODE, quality_code);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(quality_code, timeseries_value->quality_code, "Quality code parameter is wrong.");

    // Clean up.
    mcl_timeseries_value_destroy(&timeseries_value);
}

/**
 * GIVEN : Mandatory timeseries value parameters.
 * WHEN  : timeseries_value_validate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_timeseries_value_validate_001()
{
    // Initialize timeseries value.
    mcl_timeseries_value_t *timeseries_value;
    mcl_error_t code = mcl_timeseries_value_initialize(&timeseries_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value.");

    // Set data point id parameter.
    const char *data_point_id = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
    code = mcl_timeseries_value_set_parameter(timeseries_value, MCL_TIMESERIES_VALUE_PARAMETER_DATA_POINT_ID, data_point_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_set_parameter() failed.");

    // Set value parameter.
    const char *value = "23";
    code = mcl_timeseries_value_set_parameter(timeseries_value, MCL_TIMESERIES_VALUE_PARAMETER_VALUE, value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_set_parameter() failed.");

    // Set quality code parameter.
    const char *quality_code = "00000000";
    code = mcl_timeseries_value_set_parameter(timeseries_value, MCL_TIMESERIES_VALUE_PARAMETER_QUALITY_CODE, quality_code);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_set_parameter() failed.");

    // Test.
    code = timeseries_value_validate(timeseries_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "timeseries_value_validate() failed.");

    // Clean up.
    mcl_timeseries_value_destroy(&timeseries_value);
}

/**
 * GIVEN : Mandatory timeseries value parameters are not set.
 * WHEN  : timeseries_value_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_timeseries_value_validate_002()
{
    // Initialize timeseries value.
    mcl_timeseries_value_t *timeseries_value;
    mcl_error_t code = mcl_timeseries_value_initialize(&timeseries_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value.");

    // Test.
    code = timeseries_value_validate(timeseries_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "timeseries_value_validate() failed when mandatory parameters are NULL.");

    // Clean up.
    mcl_timeseries_value_destroy(&timeseries_value);
}
