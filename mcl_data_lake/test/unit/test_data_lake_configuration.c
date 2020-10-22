/*!
 * @file     test_data_lake_configuration.c
 * @brief    This file implements all test cases of mcl_data_lake_configuration module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_data_lake/mcl_data_lake_configuration.h"
#include "data_lake_configuration.h"
#include "mock_mcl_core.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : No initial condition.
 * WHEN  : mcl_data_lake_configuration_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized mcl_data_lake_configuration_t struct.
 */
void test_initialize_001(void)
{
    // Variable declaration.
    mcl_data_lake_configuration_t *configuration = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_data_lake_configuration_initialize(&configuration);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for configuration.");
    TEST_ASSERT_NOT_NULL_MESSAGE(configuration, "Configuration is NULL.");
    
    // Clean up.
    mcl_data_lake_configuration_destroy(&configuration);
}

/**
 * GIVEN : Configuration parameter is NULL.
 * WHEN  : mcl_data_lake_configuration_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002(void)
{
    // Variable declaration.
    mcl_error_t code;

    // Test.
    code = mcl_data_lake_configuration_initialize(MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_lake_configuration_initialize() should have returned MCL_TRIGGERED_WITH_NULL.");
}

/**
 * GIVEN : Value parameter is NULL.
 * WHEN  : mcl_data_lake_configuration_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_001()
{
    // Variable declaration.
    mcl_data_lake_configuration_t configuration;

    // Test.
    mcl_error_t code = mcl_data_lake_configuration_set_parameter(&configuration, MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CORE, MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_lake_configuration_set_parameter() should have returned MCL_TRIGGERED_WITH_NULL.");
}

/**
 * GIVEN : Invalid parameter to set.
 * WHEN  : mcl_data_lake_configuration_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_002()
{
    // Variable declaration.
    mcl_size_t dummy_variable = 0;
    mcl_data_lake_configuration_t *configuration;

    // Initialize data lake configuration.
    mcl_error_t code = mcl_data_lake_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake configuration.");

    // Set parameter.
    code = mcl_data_lake_configuration_set_parameter(configuration, MCL_DATA_LAKE_CONFIGURATION_PARAMETER_END, &dummy_variable);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_data_lake_configuration_set_parameter() should have returned MCL_INVALID_PARAMETER.");

    // Clean up.
    mcl_data_lake_configuration_destroy(&configuration);
}

/**
 * GIVEN : Core parameter.
 * WHEN  : mcl_data_lake_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and core parameter of the data lake configuration is set.
 */
void test_set_parameter_003()
{
    // Variable declaration.
    mcl_core_t *core = (mcl_core_t *)0xABCD;
    mcl_data_lake_configuration_t *configuration;
    char *hostname = "hostname";

    // Initialize data lake configuration.
    mcl_error_t code = mcl_data_lake_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake configuration.");

    // Mock core.
    mcl_core_get_host_name_ExpectAnyArgsAndReturn(hostname);

    // Set parameter.
    code = mcl_data_lake_configuration_set_parameter(configuration, MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CORE, core);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(core, configuration->core, "Core parameter is wrong.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("hostname/api/datalake/v3/generateUploadObjectUrls", configuration->upload_url_generation_url, "Upload url is wrong.");

    // Clean up.
    mcl_data_lake_configuration_destroy(&configuration);
}

/**
 * GIVEN : Certificate parameter.
 * WHEN  : mcl_data_lake_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and certificate parameter of the data lake configuration is set.
 */
void test_set_parameter_004()
{
    mcl_data_lake_configuration_t *configuration;
    char *certificate = "This is a certificate.";

    // Initialize data lake configuration.
    mcl_error_t code = mcl_data_lake_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake configuration.");

    // Set parameter.
    code = mcl_data_lake_configuration_set_parameter(configuration, MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CERTIFICATE, certificate);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(certificate, configuration->certificate, "Certificate parameter is wrong.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MCL_FALSE, configuration->certificate_is_file, "Certificate is not file.");

    // Clean up.
    mcl_data_lake_configuration_destroy(&configuration);
}

/**
 * GIVEN : Certificate parameter as file.
 * WHEN  : mcl_data_lake_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and certificate parameter of the data lake configuration is set.
 */
void test_set_parameter_005()
{
    mcl_data_lake_configuration_t *configuration;
    char *certificate = "certificate.pem";

    // Initialize data lake configuration.
    mcl_error_t code = mcl_data_lake_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake configuration.");

    // Set parameter.
    code = mcl_data_lake_configuration_set_parameter(configuration, MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CERTIFICATE_FILE, certificate);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(certificate, configuration->certificate, "Certificate parameter is wrong.");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MCL_TRUE, configuration->certificate_is_file, "Certificate is file.");

    // Clean up.
    mcl_data_lake_configuration_destroy(&configuration);
}

/**
 * GIVEN : Valid data lake configuration.
 * WHEN  : data_lake_configuration_validate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_validate_001()
{
    // Variable declaration.
    mcl_core_t *core = (mcl_core_t *) 0xABCD;
    mcl_data_lake_configuration_t *configuration;
    char *hostname = "hostname";

    // Mock core.
    mcl_core_get_host_name_ExpectAnyArgsAndReturn(hostname);

    // Initialize data lake configuration.
    mcl_error_t code = mcl_data_lake_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake configuration.");

    // Set core parameter.
    code = mcl_data_lake_configuration_set_parameter(configuration, MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_configuration_set_parameter() failed.");

    // Test.
    code = data_lake_configuration_validate(configuration);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "data_lake_configuration_validate() failed.");

    // Clean up.
    mcl_data_lake_configuration_destroy(&configuration);
}

/**
 * GIVEN : Core parameter is NULL.
 * WHEN  : data_lake_configuration_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_validate_002()
{
    // Variable declaration.
    mcl_data_lake_configuration_t *configuration;

    // Initialize data lake configuration.
    mcl_error_t code = mcl_data_lake_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake configuration.");

    // Test.
    code = data_lake_configuration_validate(configuration);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "data_lake_configuration_validate() should have returned MCL_INVALID_PARAMETER.");

    // Clean up.
    mcl_data_lake_configuration_destroy(&configuration);
}
