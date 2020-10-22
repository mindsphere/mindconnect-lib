/*!
 * @file     test_connectivity_configuration.c
 * @brief    This file implements all test cases of mcl_connectivity_configuration module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_connectivity/mcl_connectivity_configuration.h"
#include "connectivity_configuration.h"
#include "mock_mcl_core.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : No initial condition.
 * WHEN  : mcl_connectivity_configuration_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized mcl_connectivity_configuration_t struct.
 */
void test_initialize_001(void)
{
    mcl_connectivity_configuration_t *configuration = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_connectivity_configuration_initialize(&configuration);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for configuration");
    TEST_ASSERT_NOT_NULL_MESSAGE(configuration, "Configuration is NULL");

    // Clean up.
    mcl_connectivity_configuration_destroy(&configuration);
}

/**
 * GIVEN : Configuration parameter is given as MCL_NULL.
 * WHEN  : mcl_connectivity_configuration_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002(void)
{
    mcl_error_t code;

    // Test.
    code = mcl_connectivity_configuration_initialize(MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_connectivity_configuration_initialize() should have returned MCL_TRIGGERED_WITH_NULL.");
}

/**
 * GIVEN : Configuration parameter is NULL.
 * WHEN  : mcl_connectivity_configuration_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_001()
{
    mcl_size_t max_http_payload_size = MCL_CONNECTIVITY_MAX_HTTP_PAYLOAD_SIZE;

    // Test.
    mcl_error_t code = mcl_connectivity_configuration_set_parameter(MCL_NULL, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_MAX_HTTP_PAYLOAD_SIZE, &max_http_payload_size);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_connectivity_configuration_set_parameter() should have returned MCL_TRIGGERED_WITH_NULL.");
}

/**
 * GIVEN : Value parameter is NULL.
 * WHEN  : mcl_connectivity_configuration_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_002()
{
    mcl_connectivity_configuration_t configuration;

    // Test.
    mcl_error_t code = mcl_connectivity_configuration_set_parameter(&configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_connectivity_configuration_set_parameter() should have returned MCL_TRIGGERED_WITH_NULL.");
}

/**
 * GIVEN : Invalid max HTTP payload size parameter.
 * WHEN  : mcl_connectivity_configuration_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_003()
{
    mcl_connectivity_configuration_t configuration;
    mcl_size_t max_http_payload_size = MCL_CONNECTIVITY_MAX_HTTP_PAYLOAD_SIZE + 1;

    // Test.
    mcl_error_t code = mcl_connectivity_configuration_set_parameter(&configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_MAX_HTTP_PAYLOAD_SIZE, &max_http_payload_size);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_connectivity_configuration_set_parameter() should have returned MCL_INVALID_PARAMETER.");
}

/**
 * GIVEN : Core parameter.
 * WHEN  : mcl_connectivity_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and core parameter of the connectivity configuration is set.
 */
void test_set_parameter_004()
{
    mcl_core_t *core = (mcl_core_t *) 0xABCD;
    mcl_connectivity_configuration_t *configuration;
    char *hostname = "hostname";

    // Initialize connectivity configuration.
    mcl_error_t code = mcl_connectivity_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for connectivity configuration.");

    // Mock core.
    mcl_core_get_host_name_ExpectAnyArgsAndReturn(hostname);

    // Set parameter.
    code = mcl_connectivity_configuration_set_parameter(configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_connectivity_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(core, configuration->core, "Core parameter is wrong.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("hostname/api/mindconnect/v3/exchange", configuration->exchange_url, "Core parameter is wrong.");

    // Clean up.
    mcl_connectivity_configuration_destroy(&configuration);
}

/**
 * GIVEN : Valid max HTTP payload size parameter.
 * WHEN  : mcl_connectivity_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and max HTTP payload size parameter of the connectivity configuration is set.
 */
void test_set_parameter_005()
{
    mcl_size_t max_http_payload_size = MCL_CONNECTIVITY_MAX_HTTP_PAYLOAD_SIZE / 2;
    mcl_connectivity_configuration_t *configuration;

    // Initialize connectivity configuration.
    mcl_error_t code = mcl_connectivity_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for connectivity configuration.");

    // Set parameter.
    code = mcl_connectivity_configuration_set_parameter(configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_MAX_HTTP_PAYLOAD_SIZE, &max_http_payload_size);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_connectivity_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_CONNECTIVITY_MAX_HTTP_PAYLOAD_SIZE / 2, configuration->max_http_payload_size, "Core parameter is wrong.");

    // Clean up.
    mcl_connectivity_configuration_destroy(&configuration);
}

/**
 * GIVEN : Invalid parameter to set.
 * WHEN  : mcl_connectivity_configuration_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_006()
{
    mcl_size_t max_http_payload_size = MCL_CONNECTIVITY_MAX_HTTP_PAYLOAD_SIZE / 2;
    mcl_connectivity_configuration_t *configuration;

    // Initialize connectivity configuration.
    mcl_error_t code = mcl_connectivity_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for connectivity configuration.");

    // Set parameter.
    code = mcl_connectivity_configuration_set_parameter(configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_END, &max_http_payload_size);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_connectivity_configuration_set_parameter() should have returned MCL_INVALID_PARAMETER.");

    // Clean up.
    mcl_connectivity_configuration_destroy(&configuration);
}

/**
 * GIVEN : Valid connectivity configuration.
 * WHEN  : connectivity_configuration_validate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_validate_001()
{
    mcl_size_t max_http_payload_size = MCL_CONNECTIVITY_MAX_HTTP_PAYLOAD_SIZE / 2;
    mcl_core_t *core = (mcl_core_t *)0xABCD;
    mcl_connectivity_configuration_t *configuration;
    char *hostname = "hostname";

    // Mock core.
    mcl_core_get_host_name_ExpectAnyArgsAndReturn(hostname);

    // Initialize connectivity configuration.
    mcl_error_t code = mcl_connectivity_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for connectivity configuration.");

    // Set parameter.
    code = mcl_connectivity_configuration_set_parameter(configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_MAX_HTTP_PAYLOAD_SIZE, &max_http_payload_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_connectivity_configuration_set_parameter() failed.");

    // Set parameter.
    code = mcl_connectivity_configuration_set_parameter(configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_connectivity_configuration_set_parameter() failed.");

    // Check result.
    code = connectivity_configuration_validate(configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "connectivity_configuration_validate() failed.");

    // Clean up.
    mcl_connectivity_configuration_destroy(&configuration);
}

/**
 * GIVEN : Core parameter is NULL.
 * WHEN  : connectivity_configuration_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_validate_002()
{
    mcl_connectivity_configuration_t *configuration;

    // Initialize connectivity configuration.
    mcl_error_t code = mcl_connectivity_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for connectivity configuration.");

    // Check result.
    code = connectivity_configuration_validate(configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "connectivity_configuration_validate() should have returned MCL_INVALID_PARAMETER.");

    // Clean up.
    mcl_connectivity_configuration_destroy(&configuration);
}
