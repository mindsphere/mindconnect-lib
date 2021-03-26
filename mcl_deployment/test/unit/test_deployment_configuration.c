/*!
 * @file     test_deployment_configuration.c
 * @brief    This file implements all test cases of deployment_configuration module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_deployment/mcl_deployment_configuration.h"
#include "deployment_configuration.h"
#include "mock_mcl_core.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Null configuration handle address.
 * WHEN  : mcl_deployment_configuration_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_001(void)
{
    // Test.
    mcl_error_t code = mcl_deployment_configuration_initialize(MCL_NULL);

    // Check results.
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);
}

/**
 * GIVEN : No initial condition.
 * WHEN  : mcl_deployment_configuration_initialize() is called.
 * THEN  : MCL_OK is returned and memory is allocated for configuration data struct.
 */
void test_initialize_002(void)
{
    mcl_deployment_configuration_t *configuration = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_deployment_configuration_initialize(&configuration);

    // Check results.
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_NOT_NULL(configuration);
    TEST_ASSERT_NULL(configuration->core);
    TEST_ASSERT_NULL(configuration->workflow_instances_url);

    // Clean up.
    mcl_deployment_configuration_destroy(&configuration);
}

/**
 * GIVEN : Initialized configuration and core parameter.
 * WHEN  : mcl_deployment_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and the parameter is set.
 */
void test_set_parameter_001(void)
{
    mcl_deployment_configuration_t *configuration = MCL_NULL;
    
    mcl_error_t code = mcl_deployment_configuration_initialize(&configuration);
    TEST_ASSERT_NOT_NULL(configuration);

    // Mock access to Core.
    mcl_core_t *core = (mcl_core_t*) 0xABCD;
    char *hostname = "hostname";
    mcl_core_get_host_name_ExpectAnyArgsAndReturn(hostname);

    // Test.
    code = mcl_deployment_configuration_set_parameter(configuration, MCL_DEPLOYMENT_CONFIGURATION_PARAMETER_CORE, core);

    // Check results.
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_PTR(core, configuration->core);
    TEST_ASSERT_EQUAL_STRING("hostname/api/deploymentworkflow/v3/instances", configuration->workflow_instances_url);

    // Clean up.
    mcl_deployment_configuration_destroy(&configuration);
}

/**
 * GIVEN : Initialized configuration with all mandatory parameters set.
 * WHEN  : deployment_configuration_validate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_validate_001(void)
{
    mcl_deployment_configuration_t *configuration = MCL_NULL;

    mcl_error_t code = mcl_deployment_configuration_initialize(&configuration);
    TEST_ASSERT_NOT_NULL(configuration);

    // Mock access to Core.
    mcl_core_t *core = (mcl_core_t*) 0xABCD;
    char *hostname = "hostname";
    mcl_core_get_host_name_ExpectAnyArgsAndReturn(hostname);

    code = mcl_deployment_configuration_set_parameter(configuration, MCL_DEPLOYMENT_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Test.
    code = deployment_configuration_validate(configuration);

    // Check result.
    TEST_ASSERT_EQUAL(MCL_OK, code);
    
    // Clean up.
    mcl_deployment_configuration_destroy(&configuration);
}
