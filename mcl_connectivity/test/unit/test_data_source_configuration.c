/*!
 * @file     test_data_source_configuration.c
 * @brief    This file implements all test cases of mcl_data_source_configuration module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_connectivity/mcl_data_source_configuration.h"
#include "data_source_configuration.h"
#include "mcl_connectivity/mcl_data_source.h"
#include "mcl_connectivity/mcl_data_point.h"
#include "data_source.h"
#include "data_point.h"
#include "mcl_core/mcl_log_util.h"
#include "mcl_core/mcl_memory.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : No initial condition.
 * WHEN  : mcl_data_source_configuration_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized data source configuration struct.
 */
void test_initialize_001(void)
{
    mcl_data_source_configuration_t *data_source_configuration = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_data_source_configuration_initialize(MCL_DATA_SOURCE_CONFIGURATION_1_0, &data_source_configuration);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source configuration");
    TEST_ASSERT_NOT_NULL_MESSAGE(data_source_configuration, "data source configuration is NULL");

    // Clean up.
    mcl_data_source_configuration_destroy(&data_source_configuration);
}

/**
 * GIVEN : Invalid version parameter for initial condition.
 * WHEN  : mcl_data_source_configuration_initialize() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned and it gives the initialized data source configuration struct.
 */
void test_initialize_002(void)
{
    mcl_data_source_configuration_t *data_source_configuration = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_data_source_configuration_initialize(MCL_DATA_SOURCE_CONFIGURATION_END, &data_source_configuration);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "Initialization failed for data source configuration");
}

/**
 * GIVEN : mcl_data_source_configuration parameter is given as MCL_NULL.
 * WHEN  : mcl_data_source_configuration_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_003(void)
{
    // Test.
    mcl_error_t code = mcl_data_source_configuration_initialize(MCL_DATA_SOURCE_CONFIGURATION_1_0, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_source_configuration_initialize() unexpected return error.");
}

/**
 * GIVEN : Data source configuration parameter is NULL.
 * WHEN  : mcl_data_source_configuration_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_001()
{
    const char *dummy_value = "dummy";
    mcl_error_t code = mcl_data_source_configuration_set_parameter(MCL_NULL, MCL_DATA_SOURCE_CONFIGURATION_PARAMETER_ID, dummy_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_source_configuration_set_parameter() failed when data source configuration is NULL.");
}

/**
 * GIVEN : Value parameter is NULL.
 * WHEN  : mcl_data_source_configuration_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_002()
{
    mcl_data_source_configuration_t data_source_configuration;
    mcl_error_t code = mcl_data_source_configuration_set_parameter(&data_source_configuration, MCL_DATA_SOURCE_CONFIGURATION_PARAMETER_ID, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_source_configuration_set_parameter() failed when value is NULL.");
}

/**
 * GIVEN : Invalid parameter.
 * WHEN  : mcl_data_source_configuration_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_003()
{
    mcl_data_source_configuration_t data_source_configuration;
    const char *dummy_value = "dummy";
    mcl_error_t code = mcl_data_source_configuration_set_parameter(&data_source_configuration, 10, dummy_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_data_source_configuration_set_parameter() failed when parameter is invalid.");
}

/**
 * GIVEN : Configuration id parameter.
 * WHEN  : mcl_data_source_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new configuration id parameter of the data source configuration is set.
 */
void test_set_parameter_004()
{
    // Initialize data source configuration.
    mcl_data_source_configuration_t *data_source_configuration;
    mcl_error_t code = mcl_data_source_configuration_initialize(MCL_DATA_SOURCE_CONFIGURATION_1_0, &data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source configuration.");

    // Set parameter.
    const char *configuration_id = "e3217e2b-7036-49f2-9814-4c38542cd781";
    code = mcl_data_source_configuration_set_parameter(data_source_configuration, MCL_DATA_SOURCE_CONFIGURATION_PARAMETER_ID, configuration_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_source_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(configuration_id, data_source_configuration->payload->configuration_id, "Configuration id parameter is wrong.");

    // Clean up.
    mcl_data_source_configuration_destroy(&data_source_configuration);
}

/**
 * GIVEN : Data source configuration is NULL.
 * WHEN  : mcl_data_source_configuration_add_data_source() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_data_source_001(void)
{
    mcl_data_source_t data_source;
    mcl_error_t code = mcl_data_source_configuration_add_data_source(MCL_NULL, &data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_source_configuration_add_data_source() unexpected return error.");
}

/**
 * GIVEN : Data source is NULL.
 * WHEN  : mcl_data_source_configuration_add_data_source() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_add_data_source_002(void)
{
    mcl_data_source_configuration_t data_source_configuration;
    mcl_error_t code = mcl_data_source_configuration_add_data_source(&data_source_configuration, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_source_configuration_add_data_source() unexpected return error.");
}

/**
 * GIVEN : Data source configuration and data source.
 * WHEN  : mcl_data_source_configuration_add_data_source() is called.
 * THEN  : MCL_OK is returned and data source is added to data source configuration.
 */
void test_add_data_source_003(void)
{
    // Initialize data source configuration.
    mcl_data_source_configuration_t *data_source_configuration = MCL_NULL;
    mcl_error_t code = mcl_data_source_configuration_initialize(MCL_DATA_SOURCE_CONFIGURATION_1_0, &data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source configuration.");

    // Initialize data source.
    mcl_data_source_t *data_source;
    code = mcl_data_source_initialize(&data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source");

    // Add data source to data source configuration.
    code = mcl_data_source_configuration_add_data_source(data_source_configuration, data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding data_source to data source configuration failed.");
    TEST_ASSERT_EQUAL_PTR(data_source, data_source_configuration->payload->data_sources->head->data);

    // Clean up.
    mcl_data_source_configuration_destroy(&data_source_configuration);
}

/**
 * GIVEN : Mandatory data source parameters.
 * WHEN  : data_source_configuration_validate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_data_source_configuration_validate_001()
{
    // Initialize data source configuration.
    mcl_data_source_configuration_t *data_source_configuration = MCL_NULL;
    mcl_error_t code = mcl_data_source_configuration_initialize(MCL_DATA_SOURCE_CONFIGURATION_1_0, &data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source configuration");

    // Initialize data sources.
    mcl_data_source_t *data_source_1 = MCL_NULL;
    mcl_data_source_t *data_source_2 = MCL_NULL;
    code = mcl_data_source_initialize(&data_source_1);
    (MCL_OK == code) && (code = mcl_data_source_initialize(&data_source_2));
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

    // Add data point to data sources.
    code = mcl_data_source_add_data_point(data_source_1, data_point_1);
    (MCL_OK == code) && (code = mcl_data_source_add_data_point(data_source_2, data_point_2));
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding data points to data sources failed.");

    // Set name parameter.
    const char *source_name_1 = "Primary motor";
    const char *source_name_2 = "Secondary motor";
    code = mcl_data_source_set_parameter(data_source_1, MCL_DATA_SOURCE_PARAMETER_NAME, source_name_1);
    (MCL_OK == code) && (code = mcl_data_source_set_parameter(data_source_2, MCL_DATA_SOURCE_PARAMETER_NAME, source_name_2));
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_source_set_parameter() failed.");

    // Add data sources to data source configuration.
    code = mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_1);
    (MCL_OK == code) && (code = mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_2));
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding data sources to data source configuration failed.");

    // Set configuration id.
    const char *configuration_id = "e3217e2b-7036-49f2-9814-4c38542cd781";
    code = mcl_data_source_configuration_set_parameter(data_source_configuration, MCL_DATA_SOURCE_CONFIGURATION_PARAMETER_ID, configuration_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_source_configuration_set_parameter() failed.");

    // Test.
    code = data_source_configuration_validate(data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "data_source_configuration_validate() failed.");

    // Clean up.
    mcl_data_source_configuration_destroy(&data_source_configuration);
}

/**
 * GIVEN : No data sources are added to data source configuration.
 * WHEN  : data_source_configuration_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_data_source_configuration_validate_002()
{
    // Initialize data source configuration.
    mcl_data_source_configuration_t *data_source_configuration = MCL_NULL;
    mcl_error_t code = mcl_data_source_configuration_initialize(MCL_DATA_SOURCE_CONFIGURATION_1_0, &data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source configuration");

    // Set configuration id.
    const char *configuration_id = "e3217e2b-7036-49f2-9814-4c38542cd781";
    code = mcl_data_source_configuration_set_parameter(data_source_configuration, MCL_DATA_SOURCE_CONFIGURATION_PARAMETER_ID, configuration_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_source_configuration_set_parameter() failed.");

    // Test.
    code = data_source_configuration_validate(data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "data_source_configuration_validate() function failed to detect invalid data source configuration.");

    // Clean up.
    mcl_data_source_configuration_destroy(&data_source_configuration);
}

/**
 * GIVEN : Configuration id of the data source configuration is not set.
 * WHEN  : data_source_configuration_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_data_source_configuration_validate_003()
{
    // Initialize data source configuration.
    mcl_data_source_configuration_t *data_source_configuration = MCL_NULL;
    mcl_error_t code = mcl_data_source_configuration_initialize(MCL_DATA_SOURCE_CONFIGURATION_1_0, &data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source configuration");

    // Test.
    code = data_source_configuration_validate(data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "data_source_configuration_validate() failed to detect invalid data source configuration.");

    // Clean up.
    mcl_data_source_configuration_destroy(&data_source_configuration);
}
