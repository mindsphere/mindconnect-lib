/*!
 * @file     test_data_lake.c
 * @brief    This file implements all test cases of mcl_data_lake module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_data_lake/mcl_data_lake.h"
#include "data_lake.h"
#include "mcl_data_lake/mcl_data_lake_configuration.h"
#include "data_lake_configuration.h"
#include "mock_data_lake_processor.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"
#include "mock_mcl_core.h"
#include "mock_mcl_http_client.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Configuration parameter is null.
 * WHEN  : mcl_data_lake_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_001(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_t *data_lake = MCL_NULL;

    // Test.
    code = mcl_data_lake_initialize(MCL_NULL, &data_lake);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_lake_initialize() should have returned MCL_TRIGGERED_WITH_NULL.");
}

/**
 * GIVEN : Data lake parameter is null.
 * WHEN  : mcl_data_lake_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_configuration_t configuration;

    // Test.
    code = mcl_data_lake_initialize(&configuration, MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_data_lake_initialize() should have returned MCL_TRIGGERED_WITH_NULL.");
}

/**
 * GIVEN : Initialized data lake configuration.
 * WHEN  : mcl_data_lake_initialize() is called.
 * THEN  : MCL_OK is returned.
 */
void test_initialize_003(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_configuration_t *configuration = MCL_NULL;
    mcl_core_t *core = (mcl_core_t *)0xABCD;
    mcl_data_lake_t *data_lake = MCL_NULL;
    char *hostname = "hostname";
    const char *client_id = "client_id";
    mcl_http_client_t http_client;

    // Initialize configuration.
    code = mcl_data_lake_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for configuration.");

    // Mock core functions.
    mcl_core_get_http_client_IgnoreAndReturn(&http_client);
    mcl_core_get_host_name_ExpectAnyArgsAndReturn(hostname);
    mcl_core_get_client_id_ExpectAnyArgsAndReturn(client_id);

    // Set parameter.
    code = mcl_data_lake_configuration_set_parameter(configuration, MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_configuration_set_parameter() failed.");

    // Test.
    code = mcl_data_lake_initialize(configuration, &data_lake);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake.");
    TEST_ASSERT_EQUAL_MESSAGE(configuration, data_lake->configuration, "Data lake configuration couldn't be set.");
    TEST_ASSERT_NULL_MESSAGE(data_lake->processor.access_token, "Access token must be initially NULL.");
    TEST_ASSERT_EQUAL_MESSAGE(client_id, data_lake->processor.client_id, "Client id couldn't be set.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&http_client, data_lake->processor.http_client, "Http client is not correct.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(configuration->upload_url_generation_url, data_lake->processor.upload_url_generation_url, "Upload url is not correct.");

    // Clean up.
    mcl_data_lake_destroy(&data_lake);
    mcl_data_lake_configuration_destroy(&configuration);
}

/**
 * GIVEN : Core parameter of data lake configuration is not set.
 * WHEN  : mcl_data_lake_initialize() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_initialize_004(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_configuration_t *configuration = MCL_NULL;
    mcl_data_lake_t *data_lake = MCL_NULL;

    // Initialize configuration.
    code = mcl_data_lake_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for configuration.");

    // Test.
    code = mcl_data_lake_initialize(configuration, &data_lake);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "MCL_INVALID_PARAMETER should have been returned.");

    // Clean up.
    mcl_data_lake_configuration_destroy(&configuration);
}

/**
 * GIVEN : Initialized data lake and data lake object.
 * WHEN  : mcl_data_lake_generate_upload_url() is called.
 * THEN  : MCL_OK is returned.
 */
void test_generate_upload_url_001(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_object_t object;
    mcl_data_lake_t data_lake;
    mcl_data_lake_configuration_t configuration;
    mcl_core_t dummy_core;
    mcl_http_client_t dummy_http_client;
    char *access_token = "<Access Token>";

    // Configuration.
    configuration.core = &dummy_core;
    configuration.upload_url_generation_url = "<upload_url_generation_url>";

    // Data lake.
    data_lake.processor.http_client = &dummy_http_client;
    data_lake.processor.upload_url_generation_url = "<upload_url_generation_url>";
    data_lake.processor.access_token = MCL_NULL;
    data_lake.configuration = &configuration;

    // Mock core.
    mcl_core_get_last_access_token_ExpectAndReturn(configuration.core, &data_lake.processor.access_token, MCL_OK);
    mcl_core_get_last_access_token_ReturnThruPtr_token(&access_token);

    // Mock data_lake_processor.
    data_lake_processor_generate_upload_url_ExpectAndReturn(&data_lake.processor, &object, MCL_NULL, MCL_OK);

    // Test.
    code = mcl_data_lake_generate_upload_url(&data_lake, &object);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should have been returned.");
}

/**
 * GIVEN : One of the parameters is NULL.
 * WHEN  : mcl_data_lake_generate_upload_url() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_generate_upload_url_002(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_object_t object;
    mcl_data_lake_t data_lake;

    // Test first parameter.
    code = mcl_data_lake_generate_upload_url(MCL_NULL, &object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "MCL_TRIGGERED_WITH_NULL should have been returned.");

    // Test second parameter.
    code = mcl_data_lake_generate_upload_url(&data_lake, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "MCL_TRIGGERED_WITH_NULL should have been returned.");
}

/**
 * GIVEN : Initialized data lake and object list.
 * WHEN  : mcl_data_lake_generate_upload_urls() is called.
 * THEN  : MCL_OK is returned.
 */
void test_generate_upload_urls_001(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_list_t list;
    mcl_data_lake_t data_lake;
    mcl_data_lake_configuration_t configuration;
    mcl_core_t dummy_core;
    mcl_http_client_t dummy_http_client;
    char *access_token = "<Access Token>";

    // Configuration.
    configuration.core = &dummy_core;
    configuration.upload_url_generation_url = "<upload_url_generation_url>";

    // Data lake.
    data_lake.processor.http_client = &dummy_http_client;
    data_lake.processor.upload_url_generation_url = "<upload_url_generation_url>";
    data_lake.processor.access_token = MCL_NULL;
    data_lake.configuration = &configuration;

    // Mock core.
    mcl_core_get_last_access_token_ExpectAndReturn(configuration.core, &data_lake.processor.access_token, MCL_OK);
    mcl_core_get_last_access_token_ReturnThruPtr_token(&access_token);

    // Mock data_lake_processor.
    data_lake_processor_generate_upload_urls_ExpectAndReturn(&data_lake.processor, &list, MCL_NULL, MCL_OK);

    // Test.
    code = mcl_data_lake_generate_upload_urls(&data_lake, &list);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should have been returned.");
}

/**
 * GIVEN : One of the parameters is NULL.
 * WHEN  : mcl_data_lake_generate_upload_urls() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_generate_upload_urls_002(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_list_t list;
    mcl_data_lake_t data_lake;

    // Test first parameter.
    code = mcl_data_lake_generate_upload_urls(MCL_NULL, &list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "MCL_TRIGGERED_WITH_NULL should have been returned.");

    // Test second parameter.
    code = mcl_data_lake_generate_upload_url(&data_lake, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "MCL_TRIGGERED_WITH_NULL should have been returned.");
}

/**
 * GIVEN : Initialized data lake and data lake object.
 * WHEN  : mcl_data_lake_generate_upload_url_for_subtenant() is called.
 * THEN  : MCL_OK is returned.
 */
void test_generate_upload_url_for_subtenant_001(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_object_t object;
    mcl_data_lake_t data_lake;
    mcl_data_lake_configuration_t configuration;
    mcl_core_t dummy_core;
    mcl_http_client_t dummy_http_client;
    char *access_token = "<Access Token>";
    const char *subtenant_id = "<Subtenant ID>";

    // Configuration.
    configuration.core = &dummy_core;
    configuration.upload_url_generation_url = "<upload_url_generation_url>";

    // Data lake.
    data_lake.processor.http_client = &dummy_http_client;
    data_lake.processor.upload_url_generation_url = "<upload_url_generation_url>";
    data_lake.processor.access_token = MCL_NULL;
    data_lake.configuration = &configuration;

    // Mock core.
    mcl_core_get_last_access_token_ExpectAndReturn(configuration.core, &data_lake.processor.access_token, MCL_OK);
    mcl_core_get_last_access_token_ReturnThruPtr_token(&access_token);

    // Mock data_lake_processor.
    data_lake_processor_generate_upload_url_ExpectAndReturn(&data_lake.processor, &object, subtenant_id, MCL_OK);

    // Test.
    code = mcl_data_lake_generate_upload_url_for_subtenant(&data_lake, &object, subtenant_id);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should have been returned.");
}

/**
 * GIVEN : Initialized data lake and object list.
 * WHEN  : mcl_data_lake_generate_upload_urls_for_subtenant() is called.
 * THEN  : MCL_OK is returned.
 */
void test_generate_upload_urls_for_subtenant_001(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_list_t list;
    mcl_data_lake_t data_lake;
    mcl_data_lake_configuration_t configuration;
    mcl_core_t dummy_core;
    mcl_http_client_t dummy_http_client;
    char *access_token = "<Access Token>";
    const char *subtenant_id = "<Subtenant ID>";

    // Configuration.
    configuration.core = &dummy_core;
    configuration.upload_url_generation_url = "<upload_url_generation_url>";

    // Data lake.
    data_lake.processor.http_client = &dummy_http_client;
    data_lake.processor.upload_url_generation_url = "<upload_url_generation_url>";
    data_lake.processor.access_token = MCL_NULL;
    data_lake.configuration = &configuration;

    // Mock core.
    mcl_core_get_last_access_token_ExpectAndReturn(configuration.core, &data_lake.processor.access_token, MCL_OK);
    mcl_core_get_last_access_token_ReturnThruPtr_token(&access_token);

    // Mock data_lake_processor.
    data_lake_processor_generate_upload_urls_ExpectAndReturn(&data_lake.processor, &list, subtenant_id, MCL_OK);

    // Test.
    code = mcl_data_lake_generate_upload_urls_for_subtenant(&data_lake, &list, subtenant_id);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should have been returned.");
}

/**
 * GIVEN : Initialized data lake and data lake object.
 * WHEN  : mcl_data_lake_upload() is called.
 * THEN  : MCL_OK is returned.
 */
void test_upload_001(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_object_t object;
    mcl_data_lake_t data_lake;
    mcl_http_client_t dummy_http_client;

    // Data lake.
    data_lake.processor.http_client = &dummy_http_client;
    data_lake.processor.upload_url_generation_url = "<upload_url_generation_url>";
    data_lake.processor.access_token = MCL_NULL;

    // Mock data_lake_processor.
    data_lake_processor_upload_ExpectAndReturn(&data_lake.processor, &object, MCL_OK);

    // Test.
    code = mcl_data_lake_upload(&data_lake, &object);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should have been returned.");
}

/**
 * GIVEN : One of the parameters is NULL.
 * WHEN  : mcl_data_lake_upload() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_upload_002(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_object_t object;
    mcl_data_lake_t data_lake;

    // Test first parameter.
    code = mcl_data_lake_upload(MCL_NULL, &object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "MCL_TRIGGERED_WITH_NULL should have been returned.");

    // Test second parameter.
    code = mcl_data_lake_upload(&data_lake, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "MCL_TRIGGERED_WITH_NULL should have been returned.");
}
