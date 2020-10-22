/*!
 * @file     test_timeseries_value_list.c
 * @brief    This file implements all test cases of mcl_timeseries_value_list module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_connectivity/mcl_timeseries_value_list.h"
#include "timeseries_value_list.h"
#include "mcl_connectivity/mcl_timeseries_value.h"
#include "timeseries_value.h"
#include "mcl_core/mcl_memory.h"
#include <time.h>

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : No initial condition.
 * WHEN  : mcl_timeseries_value_list_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized timeseries value list struct.
 */
void test_initialize_001(void)
{
    mcl_timeseries_value_list_t *timeseries_value_list = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_timeseries_value_list_initialize(&timeseries_value_list);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value list.");
    TEST_ASSERT_NOT_NULL_MESSAGE(timeseries_value_list, "Timeseries value list is NULL.");

    // Clean up.
    mcl_timeseries_value_list_destroy(&timeseries_value_list);
}

/**
 * GIVEN : mcl_timeseries_value_list parameter is given as MCL_NULL.
 * WHEN  : mcl_timeseries_value_list_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002(void)
{
    mcl_error_t code;

    // Test.
    code = mcl_timeseries_value_list_initialize(MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_timeseries_value_list_initialize() unexpected return error.");
}

/**
 * GIVEN : Timeseries value list parameter is NULL.
 * WHEN  : mcl_timeseries_value_list_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_001()
{
    const char *dummy_value = "dummy";
    mcl_error_t code = mcl_timeseries_value_list_set_parameter(MCL_NULL, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, dummy_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_timeseries_value_list_set_parameter() failed when timeseries value list is NULL.");
}

/**
 * GIVEN : Value parameter is NULL.
 * WHEN  : mcl_timeseries_value_list_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_002()
{
    mcl_timeseries_value_list_t timeseries_value_list;
    mcl_error_t code = mcl_timeseries_value_list_set_parameter(&timeseries_value_list, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_timeseries_value_list_set_parameter() failed when value is NULL.");
}

/**
 * GIVEN : Invalid parameter.
 * WHEN  : mcl_timeseries_value_list_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_003()
{
    mcl_timeseries_value_list_t timeseries_value_list;
    const char *dummy_value = "dummy";
    mcl_error_t code = mcl_timeseries_value_list_set_parameter(&timeseries_value_list, MCL_TIMESERIES_VALUE_LIST_PARAMETER_END, dummy_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_timeseries_value_list_set_parameter() failed when parameter is invalid.");
}

/**
 * GIVEN : Timestamp parameter.
 * WHEN  : mcl_timeseries_value_list_set_parameter() is called.
 * THEN  : MCL_OK is returned and new timestamp parameter of the timeseries value list is set.
 */
void test_set_parameter_004()
{
    // Initialize timeseries value list.
    mcl_timeseries_value_list_t *timeseries_value_list;
    mcl_error_t code = mcl_timeseries_value_list_initialize(&timeseries_value_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value list.");

    // Set parameter.
    const char *timestamp = "2016-04-26T08:06:25.317Z";
    code = mcl_timeseries_value_list_set_parameter(timeseries_value_list, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, timestamp);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_list_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(timestamp, timeseries_value_list->timestamp, "Timestamp parameter is wrong.");

    // Clean up.
    mcl_timeseries_value_list_destroy(&timeseries_value_list);
}

/**
 * GIVEN : Invalid timestamp parameter.
 * WHEN  : mcl_timeseries_value_list_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_005()
{
    // Initialize timeseries value list.
    mcl_timeseries_value_list_t *timeseries_value_list;
    mcl_error_t code = mcl_timeseries_value_list_initialize(&timeseries_value_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value list.");

    // Set parameter.
    const char *timestamp = "2016-04-26T08:06:25.317L";
    code = mcl_timeseries_value_list_set_parameter(timeseries_value_list, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, timestamp);

    // Test.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_timeseries_value_list_set_parameter() failed when parameter is invalid.");

    // Clean up.
    mcl_timeseries_value_list_destroy(&timeseries_value_list);
}

/**
 * GIVEN : Timestamp value as time_t.
 * WHEN  : mcl_timeseries_value_list_set_parameter() is called.
 * THEN  : MCL_OK is returned.
 */
void test_set_parameter_006()
{
    // Initialize timeseries value list.
    mcl_timeseries_value_list_t *timeseries_value_list;
    mcl_error_t code = mcl_timeseries_value_list_initialize(&timeseries_value_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value list.");

    // Epoch for 2019-03-06T08:00:00.000Z.
    time_t test_time = 1551859200;
    code = mcl_timeseries_value_list_set_parameter(timeseries_value_list, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_EPOCH, &test_time);

    // Test.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_list_set_parameter() should have returned MCL_OK.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2019-03-06T08:00:00.000Z", timeseries_value_list->timestamp, "Timestamp is wrong!");

    // Clean up.
    mcl_timeseries_value_list_destroy(&timeseries_value_list);
}

/**
 * GIVEN : Mandatory timeseries value list parameters.
 * WHEN  : timeseries_value_list_validate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_timeseries_value_list_validate_001()
{
    // Initialize timeseries value list.
    mcl_timeseries_value_list_t *timeseries_value_list = MCL_NULL;
    mcl_error_t code = mcl_timeseries_value_list_initialize(&timeseries_value_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value list.");

    // Initialize timeseries values.
    mcl_timeseries_value_t *timeseries_value_1;
    mcl_timeseries_value_t *timeseries_value_2;
    code = mcl_timeseries_value_initialize(&timeseries_value_1);
    (MCL_OK == code) && (code = mcl_timeseries_value_initialize(&timeseries_value_2));
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries values.");

    // Set data point id parameter.
    const char *data_point_id_1 = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
    const char *data_point_id_2 = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1e";
    code = mcl_timeseries_value_set_parameter(timeseries_value_1, MCL_TIMESERIES_VALUE_PARAMETER_DATA_POINT_ID, data_point_id_1);
    (MCL_OK == code) && (code = mcl_timeseries_value_set_parameter(timeseries_value_2, MCL_TIMESERIES_VALUE_PARAMETER_DATA_POINT_ID, data_point_id_2));
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_set_parameter() failed.");

    // Set value parameter.
    const char *value_1 = "45";
    const char *value_2 = "23";
    code = mcl_timeseries_value_set_parameter(timeseries_value_1, MCL_TIMESERIES_VALUE_PARAMETER_VALUE, value_1);
    (MCL_OK == code) && (code = mcl_timeseries_value_set_parameter(timeseries_value_2, MCL_TIMESERIES_VALUE_PARAMETER_VALUE, value_2));
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_set_parameter() failed.");

    // Set quality code parameter.
    const char *quality_code_1 = "00000000";
    const char *quality_code_2 = "00000000";
    code = mcl_timeseries_value_set_parameter(timeseries_value_1, MCL_TIMESERIES_VALUE_PARAMETER_QUALITY_CODE, quality_code_1);
    code = mcl_timeseries_value_set_parameter(timeseries_value_2, MCL_TIMESERIES_VALUE_PARAMETER_QUALITY_CODE, quality_code_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_set_parameter() failed.");

    // Add timeseries values to timeseries value list.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list, timeseries_value_1);
    (MCL_OK == code) && (code = mcl_timeseries_value_list_add_value(timeseries_value_list, timeseries_value_2));
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding values to timeseries value list failed.");

    // Set timestamp parameter.
    const char *timestamp = "2018-04-26T08:06:25.317Z";
    code = mcl_timeseries_value_list_set_parameter(timeseries_value_list, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, timestamp);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_list_set_parameter() failed.");

    // Test.
    code = timeseries_value_list_validate(timeseries_value_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "timeseries_value_list_validate() failed.");

    // Clean up.
    mcl_timeseries_value_list_destroy(&timeseries_value_list);
}

/**
 * GIVEN : No values are added to timeseries value list.
 * WHEN  : timeseries_value_list_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_timeseries_value_list_validate_002()
{
    // Initialize timeseries value list.
    mcl_timeseries_value_list_t *timeseries_value_list;
    mcl_error_t code = mcl_timeseries_value_list_initialize(&timeseries_value_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value list.");

    // Set timestamp parameter.
    const char *timestamp = "2018-04-26T08:06:25.317Z";
    code = mcl_timeseries_value_list_set_parameter(timeseries_value_list, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, timestamp);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_list_set_parameter() failed.");

    // Test.
    code = timeseries_value_list_validate(timeseries_value_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "timeseries_value_list_validate() function failed to detect invalid timeseries value list.");

    // Clean up.
    mcl_timeseries_value_list_destroy(&timeseries_value_list);
}

/**
 * GIVEN : Timestamp of the timeseries value list is not set.
 * WHEN  : timeseries_value_list_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_timeseries_value_list_validate_003()
{
    // Initialize timeseries value list.
    mcl_timeseries_value_list_t *timeseries_value_list;
    mcl_error_t code = mcl_timeseries_value_list_initialize(&timeseries_value_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value list.");

    // Test.
    code = timeseries_value_list_validate(timeseries_value_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "timeseries_value_list_validate() failed to detect invalid timeseries value list.");

    // Clean up.
    mcl_timeseries_value_list_destroy(&timeseries_value_list);
}

/**
 * GIVEN : Timeseries value list is NULL.
 * WHEN  : mcl_timeseries_value_list_add_value() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_timeseries_value_001(void)
{
    mcl_timeseries_value_t timeseries_value;
    mcl_error_t code = mcl_timeseries_value_list_add_value(MCL_NULL, &timeseries_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_timeseries_value_list_add_value() unexpected return error.");
}

/**
 * GIVEN : Timeseries value is NULL.
 * WHEN  : mcl_timeseries_value_list_add_value() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_timeseries_value_002(void)
{
    mcl_timeseries_value_list_t timeseries_value_list;
    mcl_error_t code = mcl_timeseries_value_list_add_value(&timeseries_value_list, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_timeseries_value_list_add_value() unexpected return error.");
}

/**
 * GIVEN : Timeseries value list and timeseries value.
 * WHEN  : mcl_timeseries_value_list_add_value() is called.
 * THEN  : MCL_OK is returned and timeseries value is added to timeseries value list.
 */
void test_add_timeseries_value_003(void)
{
    // Initialize timeseries value list.
    mcl_timeseries_value_list_t *timeseries_value_list = MCL_NULL;
    mcl_error_t code = mcl_timeseries_value_list_initialize(&timeseries_value_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value list.");

    // Initialize timeseries value.
    mcl_timeseries_value_t *timeseries_value;
    code = mcl_timeseries_value_initialize(&timeseries_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value.");

    // Add timeseries value to timeseries_value list.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list, timeseries_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding timeseries value to timeseries value list failed.");
    TEST_ASSERT_EQUAL_PTR(timeseries_value, timeseries_value_list->values->head->data);

    // Clean up.
    mcl_timeseries_value_list_destroy(&timeseries_value_list);
}
