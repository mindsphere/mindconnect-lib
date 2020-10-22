/*!
 * @file     test_timeseries.c
 * @brief    This file implements all test cases of mcl_timeseries module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_connectivity/mcl_timeseries.h"
#include "timeseries.h"
#include "mcl_connectivity/mcl_timeseries_value_list.h"
#include "timeseries_value_list.h"
#include "mcl_connectivity/mcl_timeseries_value.h"
#include "timeseries_value.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_list.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Meta payload version is given as 1.0.
 * WHEN  : mcl_timeseries_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized timeseries data struct with meta payload version 1.0.
 */
void test_initialize_001(void)
{
    // Initialize variables.
    mcl_error_t code;
    mcl_timeseries_t *timeseries = MCL_NULL;
    E_MCL_TIMESERIES_VERSION version = MCL_TIMESERIES_VERSION_1_0;

    // Test.
    code = mcl_timeseries_initialize(version, &timeseries);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries.");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((mcl_uint32_t)version, timeseries->item_base.version, "Timeseries meta payload version is wrong");
    TEST_ASSERT_NOT_NULL_MESSAGE(timeseries, "Timeseries is Null.");
    TEST_ASSERT_NOT_NULL_MESSAGE(timeseries->payload, "Timeseries payload is Null.");

    // Clean up.
    mcl_timeseries_destroy(&timeseries);
}

/**
 * GIVEN : mcl_timeseries_t parameter is given as MCL_NULL.
 * WHEN  : mcl_timeseries_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002(void)
{
    // Initialize variables.
    mcl_error_t code;
    E_MCL_TIMESERIES_VERSION version = MCL_TIMESERIES_VERSION_1_0;

    // Test.
    code = mcl_timeseries_initialize(version, MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_timeseries_initialize() unexpected return error!");
}

/**
 * GIVEN : Meta payload version is given as undefined E_MCL_TIMESERIES_VERSION.
 * WHEN  : mcl_timeseries_initialize() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_initialize_003(void)
{
    // Initialize variables.
    mcl_error_t code;
    mcl_timeseries_t *timeseries = MCL_NULL;
    E_MCL_TIMESERIES_VERSION version_invalid = MCL_TIMESERIES_VERSION_END;

    // Test.
    code = mcl_timeseries_initialize(version_invalid, &timeseries);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_timeseries_initialize() unexpected return error!");
}

/**
 * GIVEN : mcl_timeseries_t parameter is given as MCL_NULL.
 * WHEN  : mcl_timeseries_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_001(void)
{
    // Initialize variable.
    mcl_error_t code;
    const char *configuration_id = "Configuration ID";

    // Test.
    code = mcl_timeseries_set_parameter(MCL_NULL, MCL_TIMESERIES_PARAMETER_CONFIGURATION_ID, configuration_id);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_timeseries_set_parameter() unexpected return error!");
}

/**
 * GIVEN : Value parameter is given as MCL_NULL.
 * WHEN  : mcl_timeseries_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_002(void)
{
    // Declarations.
    mcl_error_t code;
    mcl_timeseries_t timeseries;

    // Test.
    code = mcl_timeseries_set_parameter(&timeseries, MCL_TIMESERIES_PARAMETER_CONFIGURATION_ID, MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_timeseries_set_parameter() unexpected return error!");
}

/**
 * GIVEN : Initialized timeseries.
 * WHEN  : mcl_timeseries_set_parameter() is called.
 * THEN  : MCL_OK is returned and configuration id is set.
 */
void test_set_parameter_003(void)
{
    // Declarations.
    mcl_error_t code;
    mcl_timeseries_t *timeseries;

    // Initialize configuration id for test.
    const char configuration_id[] = "Configuration ID";

    // Initialize timeseries.
    code = mcl_timeseries_initialize(MCL_TIMESERIES_VERSION_1_0, &timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries.");

    // Test.
    code = mcl_timeseries_set_parameter(timeseries, MCL_TIMESERIES_PARAMETER_CONFIGURATION_ID, configuration_id);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_set_parameter() unexpected return error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(timeseries->payload->configuration_id, configuration_id, "Configuration ID could not be set.");

    // Clean up.
    mcl_timeseries_destroy(&timeseries);
}

/**
 * GIVEN : Initialized timeseries and invalid E_MCL_TIMESERIES_PARAMETER parameter are given.
 * WHEN  : mcl_timeseries_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_004(void)
{
    // Declarations.
    mcl_error_t code;
    mcl_timeseries_t *timeseries = MCL_NULL;

    // Initialize timeseries.
    code = mcl_timeseries_initialize(MCL_TIMESERIES_VERSION_1_0, &timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries.");

    // Test.
    code = mcl_timeseries_set_parameter(timeseries, MCL_TIMESERIES_PARAMETER_END, "dummy");

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_timeseries_set_parameter() unexpected return error!");

    // Clean up.
    mcl_timeseries_destroy(&timeseries);
}

/**
 * GIVEN : Timeseries parameter is NULL.
 * WHEN  : mcl_timeseries_add_value_list() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_timeseries_value_list_001(void)
{
    mcl_timeseries_value_list_t timeseries_value_list;
    mcl_error_t code = mcl_timeseries_add_value_list(MCL_NULL, &timeseries_value_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_timeseries_add_value_list() unexpected return error.");
}

/**
 * GIVEN : Timeseries value list is NULL.
 * WHEN  : mcl_timeseries_add_value_list() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_timeseries_value_list_002(void)
{
    mcl_timeseries_t timeseries;
    mcl_error_t code = mcl_timeseries_add_value_list(&timeseries, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_timeseries_add_value_list() unexpected return error.");
}

/**
 * GIVEN : Timeseries and timeseries value list.
 * WHEN  : mcl_timeseries_add_value_list() is called.
 * THEN  : MCL_OK is returned and timeseries value list is added to timeseries.
 */
void test_add_timeseries_value_list_003(void)
{
    // Initialize timeseries.
    mcl_timeseries_t *timeseries = MCL_NULL;
    mcl_error_t code = mcl_timeseries_initialize(MCL_TIMESERIES_VERSION_1_0, &timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries.");

    // Initialize timeseries value list.
    mcl_timeseries_value_list_t *timeseries_value_list = MCL_NULL;
    code = mcl_timeseries_value_list_initialize(&timeseries_value_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value list.");

    // Add timeseries value list to timeseries.
    code = mcl_timeseries_add_value_list(timeseries, timeseries_value_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding timeseries value list to timeseries failed.");
    TEST_ASSERT_EQUAL_PTR(timeseries_value_list, timeseries->payload->value_lists->head->data);

    // Clean up.
    mcl_timeseries_destroy(&timeseries);
}

/**
 * GIVEN : Mandatory timeseries parameters.
 * WHEN  : timeseries_validate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_timeseries_validate_001()
{
    // Initialize timeseries.
    mcl_timeseries_t *timeseries = MCL_NULL;
    mcl_error_t code = mcl_timeseries_initialize(MCL_TIMESERIES_VERSION_1_0, &timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries.");

    // Initialize timeseries value lists.
    mcl_timeseries_value_list_t *timeseries_value_list_1 = MCL_NULL;
    mcl_timeseries_value_list_t *timeseries_value_list_2 = MCL_NULL;
    code = mcl_timeseries_value_list_initialize(&timeseries_value_list_1);
    (MCL_OK == code) && (code = mcl_timeseries_value_list_initialize(&timeseries_value_list_2));
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

    // Add timeseries values to timeseries value lists.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list_1, timeseries_value_1);
    (MCL_OK == code) && (code = mcl_timeseries_value_list_add_value(timeseries_value_list_2, timeseries_value_2));
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding timeseries values to timeseries value lists failed.");

    // Set timestamp parameter.
    const char *timestamp_1 = "2018-04-26T08:06:25.317Z";
    const char *timestamp_2 = "2018-02-26T08:06:25.317Z";
    code = mcl_timeseries_value_list_set_parameter(timeseries_value_list_1, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, timestamp_1);
    (MCL_OK == code) && (code = mcl_timeseries_value_list_set_parameter(timeseries_value_list_2, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, timestamp_2));
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_list_set_parameter() failed.");

    // Add timeseries value lists to timeseries.
    code = mcl_timeseries_add_value_list(timeseries, timeseries_value_list_1);
    (MCL_OK == code) && (code = mcl_timeseries_add_value_list(timeseries, timeseries_value_list_2));
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding timeseries value lists to timeseries failed.");

    // Set configuration id.
    const char *configuration_id = "e3217e2b-7036-49f2-9814-4c38542cd781";
    code = mcl_timeseries_set_parameter(timeseries, MCL_TIMESERIES_PARAMETER_CONFIGURATION_ID, configuration_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_set_parameter() failed.");

    // Test.
    code = timeseries_validate(timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "timeseries_validate() failed.");

    // Clean up.
    mcl_timeseries_destroy(&timeseries);
}

/**
 * GIVEN : No timeseries value list is added to timeseries.
 * WHEN  : timeseries_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_timeseries_validate_002()
{
    // Initialize timeseries.
    mcl_timeseries_t *timeseries = MCL_NULL;
    mcl_error_t code = mcl_timeseries_initialize(MCL_TIMESERIES_VERSION_1_0, &timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries.");

    // Set configuration id.
    const char *configuration_id = "e3217e2b-7036-49f2-9814-4c38542cd781";
    code = mcl_timeseries_set_parameter(timeseries, MCL_TIMESERIES_PARAMETER_CONFIGURATION_ID, configuration_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_set_parameter() failed.");

    // Test.
    code = timeseries_validate(timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "timeseries_validate() function failed to detect invalid timeseries.");

    // Clean up.
    mcl_timeseries_destroy(&timeseries);
}

/**
 * GIVEN : Configuration id of the timeseries is not set.
 * WHEN  : timeseries_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_timeseries_validate_003()
{
    // Initialize timeseries.
    mcl_timeseries_t *timeseries = MCL_NULL;
    mcl_error_t code = mcl_timeseries_initialize(MCL_TIMESERIES_VERSION_1_0, &timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries.");

    // Test.
    code = timeseries_validate(timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "timeseries_validate() failed to detect invalid timeseries.");

    // Clean up.
    mcl_timeseries_destroy(&timeseries);
}
