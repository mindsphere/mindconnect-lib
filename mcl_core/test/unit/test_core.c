/*!
 * @file     test_core.c
 * @brief    This file contains test case functions to test core module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "core.h"
#include "memory.h"
#include "string_util.h"
#include "list.h"
#include "mock_core_processor.h"
#include "mcl_core/mcl_core.h"
#include "mock_mcl_core_configuration.h"
#include "mock_core_configuration.h"
#include "mcl_core/mcl_http_client.h"
#include "mcl_core/mcl_list.h"
#include "mcl_core/mcl_log_util.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Configuration parameter is null.
 * WHEN  : mcl_core_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_001(void)
{
    // Test.
    mcl_core_t *core = MCL_NULL;
    mcl_error_t code = mcl_core_initialize(MCL_NULL, &core);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_core_initialize() unexpected return error.");
    TEST_ASSERT_NULL_MESSAGE(core, "Core is not NULL.");
}

/**
 * GIVEN : Core parameter is null.
 * WHEN  : mcl_core_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002(void)
{
    // Test.
    mcl_core_configuration_t configuration;
    mcl_error_t code = mcl_core_initialize(&configuration, MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_core_initialize() unexpected return error.");
}

/**
 * GIVEN : Initialized core configuration.
 * WHEN  : mcl_core_initialize() is called.
 * THEN  : MCL_OK is returned.
 */
void test_initialize_003(void)
{
    // Mock.
    mcl_core_configuration_t configuration;
    core_configuration_validate_IgnoreAndReturn(MCL_OK);
    core_processor_initialize_IgnoreAndReturn(MCL_OK);
    core_processor_destroy_Ignore();
    core_configuration_log_Ignore();

    // Test.
    mcl_core_t *core = MCL_NULL;
    mcl_error_t code = mcl_core_initialize(&configuration, &core);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_initialize() unexpected return error.");
    TEST_ASSERT_EQUAL_MESSAGE(&configuration, core->configuration, "Core configuration couldn't be set.");

    // Clean up.
    mcl_core_destroy(&core);
}

/**
 * GIVEN : Invalid core configuration.
 * WHEN  : mcl_core_initialize() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_initialize_004(void)
{
    // Mock.
    mcl_core_configuration_t configuration;
    core_configuration_validate_IgnoreAndReturn(MCL_INVALID_PARAMETER);
    core_processor_destroy_Ignore();

    // Test.
    mcl_core_t *core = MCL_NULL;
    mcl_error_t code = mcl_core_initialize(&configuration, &core);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_core_initialize() unexpected return error.");
}

/**
 * GIVEN : Null #mcl_core_t object.
 * WHEN  : mcl_core_onboard() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_onboard_001(void)
{
    // Test.
    mcl_error_t code = mcl_core_onboard(MCL_NULL);

    // Check test result.
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_core_onboard() unexpected return error!");
}

/**
 * GIVEN : MCL handle is successfully initialized.
 * WHEN  : mcl_core_onboard() is called.
 * THEN  : MCL_OK is returned.
 */
void test_onboard_002(void)
{
    // Variable declaration.
    mcl_core_t *core = MCL_NULL;
    mcl_core_configuration_t configuration =
    {
        .critical_section_enter_callback = MCL_NULL,
        .critical_section_leave_callback = MCL_NULL
    };

    // Mock core configuration functions.
    core_configuration_validate_IgnoreAndReturn(MCL_OK);
    core_configuration_log_Ignore();

    // Mock core_processor_initialize and make sure registration access token is NULL.
    core_processor_t *core_processor;
    MCL_NEW(core_processor);
    MCL_NEW(core_processor->security_handler);
    core_processor->security_handler->registration_access_token = MCL_NULL;
    core_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    core_processor_initialize_ReturnThruPtr_core_processor(&core_processor);
    
    // Initialize mcl_core.
    mcl_error_t code = mcl_core_initialize(&configuration, &core);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_core_initialize() unexpected return error!");

    // Mock core_processor_register.
    core_processor_register_IgnoreAndReturn(MCL_OK);

    // Test.
    code = mcl_core_onboard(core);

    // Check test result.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_core_onboard() unexpected return error!");

    // Clean up.
    MCL_FREE(core_processor->security_handler);
    MCL_FREE(core_processor);

    core_processor_destroy_Ignore();
    mcl_core_destroy(&core);
}

/**
 * GIVEN : MCL handle is successfully initialized.
 * WHEN  : mcl_core_onboard() is called when the agent is already onboarded.
 * THEN  : MCL_ALREADY_ONBOARDED is returned.
 */
void test_onboard_004(void)
{
    // Variable declaration.
    mcl_core_t *core = MCL_NULL;
    mcl_core_configuration_t configuration =
    {
        .critical_section_enter_callback = MCL_NULL,
        .critical_section_leave_callback = MCL_NULL
    };

    // Mock core configuration functions.
    core_configuration_validate_IgnoreAndReturn(MCL_OK);
    core_configuration_log_Ignore();

    // Mock core_processor_initialize with core_processor->security_handler->registration_access_token not null.
    core_processor_t *core_processor;
    MCL_NEW(core_processor);
    MCL_NEW(core_processor->security_handler);
    MCL_NEW(core_processor->security_handler->registration_access_token);
    core_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    core_processor_initialize_ReturnThruPtr_core_processor(&core_processor);

    // Initialize mcl_core.
    mcl_error_t code = mcl_core_initialize(&configuration, &core);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_core_initialize() unexpected return error!");

    // Test.
    code = mcl_core_onboard(core);

    // Check test result.
    TEST_ASSERT_MESSAGE(MCL_ALREADY_ONBOARDED == code, "Return code should be MCL_ALREADY_ONBOARDED.");

    // Clean up.
    MCL_FREE(core_processor->security_handler->registration_access_token);
    MCL_FREE(core_processor->security_handler);
    MCL_FREE(core_processor);

    core_processor_destroy_Ignore();
    mcl_core_destroy(&core);
}

/**
 * GIVEN : A valid access token in security handler.
 * WHEN  : mcl_core_get_last_access_token() is called.
 * THEN  : MCL_OK and access token are returned.
 */
void test_get_last_access_token_001()
{
    // Variable declaration.
    core_t core;
    core_processor_t processor;
    security_handler_t security_handler;
    char *dummy_token = "dummy_access_token";
    security_handler.access_token = string_util_strdup(dummy_token);

    core.core_processor = &processor;
    core.core_processor->security_handler = &security_handler;

    // To simulate core is onboarded.
    security_handler.registration_access_token = string_util_strdup("dummy_registration_access_token");

    // Test.
    char *token = MCL_NULL;
    mcl_error_t code = mcl_core_get_last_access_token(&core, &token);

    // Check test result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_get_last_access_token() unexpected return error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(dummy_token, token, "Token value is not equal to the expected one.");

    // Clean up.
    MCL_FREE(security_handler.registration_access_token);
    MCL_FREE(security_handler.access_token);
    MCL_FREE(token);
}

/**
 * GIVEN : Core is initialized and onboarded.
 * WHEN  : mcl_core_get_access_token() is called.
 * THEN  : MCL_OK and access token are returned.
 */
void test_get_access_token_001()
{
    // Variable declaration.
    char *dummy_token = "dummy_access_token";

    mcl_core_t *core = MCL_NULL;
    mcl_core_configuration_t configuration =
    {
        .critical_section_enter_callback = MCL_NULL,
        .critical_section_leave_callback = MCL_NULL
    };

    // Mock core configuration functions.
    core_configuration_validate_IgnoreAndReturn(MCL_OK);
    core_configuration_log_Ignore();

    // Mock core_processor_initialize and make sure registration access token is set.
    core_processor_t *core_processor;
    MCL_NEW(core_processor);
    MCL_NEW(core_processor->security_handler);
    core_processor->security_handler->registration_access_token = MCL_NULL;
    core_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    core_processor_initialize_ReturnThruPtr_core_processor(&core_processor);

    // Initialize mcl_core.
    mcl_error_t code = mcl_core_initialize(&configuration, &core);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_core_initialize() unexpected return error!");

    // Mock core_processor_register.
    core_processor_register_IgnoreAndReturn(MCL_OK);

    // Mock core_processor_get_access_token.
    core_processor_get_access_token_IgnoreAndReturn(MCL_OK);

    // Onboarding.
    code = mcl_core_onboard(core);

    TEST_ASSERT_EQUAL(MCL_OK, code);

    core_processor->security_handler->registration_access_token = string_util_strdup(dummy_token);

    // Test.
    code = mcl_core_get_access_token(core);

    // Check test result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_get_access_token() unexpected return error!");

    // Clean up.
    MCL_FREE(core_processor->security_handler->registration_access_token);
    MCL_FREE(core_processor->security_handler);
    MCL_FREE(core_processor);

    core_processor_destroy_Ignore();
    mcl_core_destroy(&core);
}

/**
 * GIVEN : A valid access token time in security handler.
 * WHEN  : mcl_core_get_last_token_time() is called.
 * THEN  : MCL_OK and token time are returned.
 */
void test_get_last_token_time_001()
{
    // Variable declaration.
    core_t core;
    core_processor_t processor;
    security_handler_t security_handler;
    char *dummy_token_time = "dummy_token_time";

    security_handler.last_token_time = string_util_strdup(dummy_token_time);

    core.core_processor = &processor;
    core.core_processor->security_handler = &security_handler;

    // Test.
    char *token_time = MCL_NULL;
    mcl_error_t code = mcl_core_get_last_token_time(&core, &token_time);

    // Check test result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_get_last_token_time() unexpected return error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(dummy_token_time, token_time, "Token time is not equal to the expected one.");

    // Clean up.
    MCL_FREE(security_handler.last_token_time);
    MCL_FREE(token_time);
}

/**
 * GIVEN : Core is initialized and onboarded.
 * WHEN  : mcl_core_rotate_key() is called.
 * THEN  : MCL_OK is returned.
 */
void test_rotate_key_001()
{
    // Variable declaration.
    mcl_core_t *core = MCL_NULL;
    mcl_core_configuration_t configuration =
    {
        .critical_section_enter_callback = MCL_NULL,
        .critical_section_leave_callback = MCL_NULL
    };

    // Mock core configuration functions.
    core_configuration_validate_IgnoreAndReturn(MCL_OK);
    core_configuration_log_Ignore();

    // Mock core_processor_initialize and make sure registration access token is NULL.
    core_processor_t *core_processor;
    MCL_NEW(core_processor);
    MCL_NEW(core_processor->security_handler);
    core_processor->security_handler->registration_access_token = MCL_NULL;
    core_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    core_processor_initialize_ReturnThruPtr_core_processor(&core_processor);

    // Initialize mcl_core.
    mcl_error_t code =mcl_core_initialize(&configuration, &core);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_core_initialize() unexpected return error!");

    // Mock core_processor_register.
    core_processor_register_IgnoreAndReturn(MCL_OK);

    // Onboarding.
    code = mcl_core_onboard(core);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_core_onboard() unexpected return error!");

    core->core_processor->security_handler->registration_access_token = "rat";

    // Mock core_processor_register.
    core_processor_register_IgnoreAndReturn(MCL_OK);

    // Test.
    code = mcl_core_rotate_key(core);

    // Check test result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_rotate_key() unexpected return error!");

    // Clean up.
    MCL_FREE(core_processor->security_handler);
    MCL_FREE(core_processor);

    core_processor_destroy_Ignore();
    mcl_core_destroy(&core);
}

/**
 * GIVEN : Core is initialized and onboarded.
 * WHEN  : mcl_core_update_credentials() is called.
 * THEN  : MCL_OK is returned.
 */
void test_update_credentials_001()
{
    // Variable declaration.
    char *dummy_token = "dummy_access_token";

    mcl_core_t *core = MCL_NULL;
    mcl_core_configuration_t configuration =
    {
        .critical_section_enter_callback = MCL_NULL,
        .critical_section_leave_callback = MCL_NULL
    };

    // Mock core configuration functions.
    core_configuration_validate_IgnoreAndReturn(MCL_OK);
    core_configuration_log_Ignore();

    // Mock core_processor_initialize and make sure registration access token is set.
    core_processor_t *core_processor;
    MCL_NEW(core_processor);
    MCL_NEW(core_processor->security_handler);
    core_processor->security_handler->registration_access_token = MCL_NULL;
    core_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    core_processor_initialize_ReturnThruPtr_core_processor(&core_processor);

    // Initialize mcl_core.
    mcl_error_t code = mcl_core_initialize(&configuration, &core);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_core_initialize() unexpected return error!");

    // Mock core_processor_register.
    core_processor_register_IgnoreAndReturn(MCL_OK);

    // Mock core_processor_update_credentials.
    core_processor_update_credentials_IgnoreAndReturn(MCL_OK);

    // Onboarding.
    code = mcl_core_onboard(core);

    TEST_ASSERT_EQUAL(MCL_OK, code);

    core_processor->security_handler->registration_access_token = string_util_strdup(dummy_token);

    // Test.
    code = mcl_core_update_credentials(core);

    // Check test result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_update_credentials() unexpected return error!");

    // Clean up.
    MCL_FREE((core_processor->security_handler->registration_access_token));
    MCL_FREE(core_processor->security_handler);
    MCL_FREE(core_processor);

    core_processor_destroy_Ignore();
    mcl_core_destroy(&core);
}

/**
 * GIVEN : Core is initialized and onboarded.
 * WHEN  : mcl_core_update_credentials() is called when it is up to date.
 * THEN  : MCL_CREDENTIALS_UP_TO_DATE is returned.
 */
void test_update_credentials_002()
{
    // Variable declaration.
    char *dummy_token = "dummy_access_token";

    mcl_core_t *core = MCL_NULL;
    mcl_core_configuration_t configuration =
    {
        .critical_section_enter_callback = MCL_NULL,
        .critical_section_leave_callback = MCL_NULL
    };

    // Mock core configuration functions.
    core_configuration_validate_IgnoreAndReturn(MCL_OK);
    core_configuration_log_Ignore();

    // Mock core_processor_initialize and make sure registration access token is set.
    core_processor_t *core_processor;
    MCL_NEW(core_processor);
    MCL_NEW(core_processor->security_handler);
    core_processor->security_handler->registration_access_token = MCL_NULL;
    core_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    core_processor_initialize_ReturnThruPtr_core_processor(&core_processor);

    // Initialize mcl_core.
    mcl_error_t code = mcl_core_initialize(&configuration, &core);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_core_initialize() unexpected return error!");

    // Mock core_processor_register.
    core_processor_register_IgnoreAndReturn(MCL_OK);

    // Mock core_processor_update_credentials.
    core_processor_update_credentials_IgnoreAndReturn(MCL_CREDENTIALS_UP_TO_DATE);

    // Onboarding.
    code = mcl_core_onboard(core);

    TEST_ASSERT_EQUAL(MCL_OK, code);

    core_processor->security_handler->registration_access_token = string_util_strdup(dummy_token);

    // Test.
    code = mcl_core_update_credentials(core);

    // Check test result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_CREDENTIALS_UP_TO_DATE, code, "mcl_core_update_credentials() should have returned MCL_CREDENTIALS_UP_TO_DATE.");

    // Clean up.
    MCL_FREE(core_processor->security_handler->registration_access_token);
    MCL_FREE(core_processor->security_handler);
    MCL_FREE(core_processor);

    core_processor_destroy_Ignore();
    mcl_core_destroy(&core);
}

/**
 * GIVEN : Core is initialized.
 * WHEN  : mcl_core_get_http_client() is called.
 * THEN  : http_client address is returned.
 */
void test_get_http_client_001()
{
    // Variable declaration.
    mcl_http_client_t *test_pointer = (mcl_http_client_t*) 0xC0DEC0DE;

    mcl_core_t *core = MCL_NULL;
    mcl_core_configuration_t configuration;

    // Mock core configuration functions.
    core_configuration_validate_IgnoreAndReturn(MCL_OK);
    core_configuration_log_Ignore();

    // Mock core_processor_initialize.
    core_processor_t *core_processor;
    MCL_NEW(core_processor);
    MCL_NEW(core_processor->security_handler);
    core_processor->http_client = test_pointer;
    core_processor->security_handler->registration_access_token = MCL_NULL;
    core_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    core_processor_initialize_ReturnThruPtr_core_processor(&core_processor);
    core_processor_destroy_Ignore();

    // Initialize mcl_core.
    mcl_error_t code = mcl_core_initialize(&configuration, &core);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_core_initialize() unexpected return error!");

    // Test.
    mcl_http_client_t *http_client = mcl_core_get_http_client(core);

    // Check test result.
    TEST_ASSERT_EQUAL_PTR_MESSAGE(http_client, test_pointer, "http_client is different from the expected one.");

    // Clean up.
    MCL_FREE(core_processor->security_handler);
    MCL_FREE(core_processor);

    core_processor_destroy_Ignore();
    mcl_core_destroy(&core);
}

/**
 * GIVEN : An initialized #mcl_core_t handle.
 * WHEN  : mcl_core_get_host_name() is called.
 * THEN  : Host name is returned.
 */
void test_get_host_name_001()
{
    // Variable declaration.
    mcl_core_t *core = MCL_NULL;
    mcl_core_configuration_t configuration =
    {
        .mindsphere_hostname = "test_host_name"
    };

    // Mock core configuration functions.
    core_configuration_validate_IgnoreAndReturn(MCL_OK);
    core_configuration_log_Ignore();

    // Initialize core object.
    core_processor_initialize_IgnoreAndReturn(MCL_OK);
    mcl_error_t code = mcl_core_initialize(&configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_initialize() failed!");

    // Test.
    const char *host_name = mcl_core_get_host_name(core);

    // Check test result.
    TEST_ASSERT_EQUAL_STRING_MESSAGE((&configuration)->mindsphere_hostname, host_name, "Host name is not equal to the expected one.");

    // Clean up.
    core_processor_destroy_Ignore();
    mcl_core_destroy(&core);
}

/**
 * GIVEN : An initialized #mcl_core_t handle.
 * WHEN  : mcl_core_get_client_id() is called.
 * THEN  : Client id is returned.
 */
void test_get_client_id_001()
{
    // Initialization for client id.
    core_t core;
    core_processor_t processor;
    security_handler_t security_handler;
    char *dummy_client_id = "dummy_client_id";
    security_handler.client_id = string_util_strdup(dummy_client_id);

    core.core_processor = &processor;
    core.core_processor->security_handler = &security_handler;
    security_handler.registration_access_token = "rat";

    // Test.
    const char *client_id = mcl_core_get_client_id(&core);

    // Check test result.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(dummy_client_id, client_id, "Client id is not equal to the expected one.");

    // Clean up.
    MCL_FREE((security_handler.client_id));
}
