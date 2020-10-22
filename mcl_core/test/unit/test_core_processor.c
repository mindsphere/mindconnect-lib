/*!
 * @file     test_core_processor.c
 * @brief    This file contains test case functions to test core_processor module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "core_processor.h"
#include "core_common.h"
#include "random.h"
#include "definitions.h"
#include "memory.h"
#include "string_util.h"
#include "json_util.h"
#include "http_definitions.h"
#include "mcl_core/mcl_log_util.h"
#include "mcl_core/mcl_string_util.h"
#include "mock_mcl_http_client.h"
#include "mock_security_handler.h"
#include "mock_security.h"
#include "mock_http_request.h"
#include "mock_mcl_http_request.h"
#include "mock_mcl_http_response.h"
#include "mock_mcl_file_util.h"
#include "mock_jwt.h"
#include "mock_time_util.h"
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

mcl_uint8_t update_security_state = 0;

static mcl_error_t _custom_load_credentials_shared_secret_info_func(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri);
static mcl_error_t _custom_save_credentials_shared_secret_info_func(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri);
static mcl_error_t _custom_load_credentials_rsa_info_func(char **client_id, char **public_key, char **private_key, char **registration_access_token, char **registration_uri);
static mcl_error_t _custom_save_credentials_rsa_info_func(char **client_id, char **public_key, char **private_key, char **registration_access_token, char **registration_uri);
static mcl_error_t load_for_update_security_test(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri);
static mcl_error_t save_for_update_security_test(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri);
static mcl_error_t load_for_update_security_test_rsa(char **client_id, char **public_key, char **private_key, char **registration_access_token, char **registration_uri);
static mcl_error_t save_for_update_security_test_rsa(const char *client_id, const char *public_key, const char *private_key, const char *registration_access_token, const char *registration_uri);

static mcl_error_t _custom_save_credentials_shared_secret_info_fail_func(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri);

static void _create_configuration(E_MCL_SECURITY_PROFILE security_profile, char *initial_access_token, core_configuration_t **configuration);
static void _create_security_handler(security_handler_t **security_handler);

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Initial access token is null and callback functions are not provided.
 * WHEN  : core_processor_initialize() is called.
 * THEN  : MCL_NO_ACCESS_TOKEN_PROVIDED is returned.
 */
void test_initialize_001(void)
{
    // Mock configuration.
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_SHARED_SECRET, MCL_NULL, &configuration);

    security_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_client_initialize_IgnoreAndReturn(MCL_OK);
    security_handler_initialize_IgnoreAndReturn(MCL_OK);

    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();

    // Initialize core processor.
    core_processor_t *core_processor = MCL_NULL;
    mcl_error_t code = core_processor_initialize(configuration, &core_processor);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_NO_ACCESS_TOKEN_PROVIDED, code, "core_processor_initialize() does not return MCL_NO_ACCESS_TOKEN_PROVIDED.");
    TEST_ASSERT_NULL_MESSAGE(core_processor, "Core processor is not null after initialization.");

    // Clean up.
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
}

/**
 * GIVEN : Callback functions are not provided but valid initial access token is provided for SHARED_SECRET security profile.
 * WHEN  : core_processor_initialize() is called.
 * THEN  : MCL_OK is returned.
 */
void test_initialize_002(void)
{
    // Mock configuration.
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_SHARED_SECRET, "InitialAccessToken", &configuration);

    security_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_client_initialize_IgnoreAndReturn(MCL_OK);
    security_handler_initialize_IgnoreAndReturn(MCL_OK);

    // Initialize core processor.
    core_processor_t *core_processor = MCL_NULL;
    mcl_error_t code = core_processor_initialize(configuration, &core_processor);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "core_processor_initialize() does not return MCL_OK.");
    TEST_ASSERT_NOT_NULL_MESSAGE(core_processor, "Core processor is null after initialization.");

    // Clean up.
    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    core_processor_destroy(&core_processor);
    MCL_FREE(configuration->initial_access_token);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
}

/**
 * GIVEN : Callback functions are not provided but valid initial access token is provided for RSA security profile.
 * WHEN  : core_processor_initialize() is called.
 * THEN  : MCL_OK is returned.
 */
void test_initialize_003(void)
{
    // Mock configuration.
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_RSA_3072, "InitialAccessToken", &configuration);

    security_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_client_initialize_IgnoreAndReturn(MCL_OK);
    security_handler_initialize_IgnoreAndReturn(MCL_OK);
    security_handler_generate_rsa_key_IgnoreAndReturn(MCL_OK);

    // Initialize core processor.
    core_processor_t *core_processor = MCL_NULL;
    mcl_error_t code = core_processor_initialize(configuration, &core_processor);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "core_processor_initialize() does not return MCL_OK.");
    TEST_ASSERT_NOT_NULL_MESSAGE(core_processor, "Core processor is null after initialization.");

    // Clean up.
    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    core_processor_destroy(&core_processor);
    MCL_FREE(configuration->initial_access_token);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
}

/**
 * GIVEN : Callback functions for loading and saving credentials are provided for SHARED_SECRET security profile, initial access token is null.
 * WHEN  : core_processor_initialize() is called.
 * THEN  : MCL_OK is returned.
 */
void test_initialize_004(void)
{
    // Mock configuration.
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_SHARED_SECRET, MCL_NULL, &configuration);
    configuration->credentials_load_callback.shared_secret = (mcl_credentials_load_shared_secret_callback_t) _custom_load_credentials_shared_secret_info_func;
    configuration->credentials_save_callback.shared_secret = (mcl_credentials_save_shared_secret_callback_t) _custom_save_credentials_shared_secret_info_func;

    security_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_client_initialize_IgnoreAndReturn(MCL_OK);

    // Mock security handler.
    security_handler_t *security_handler = NULL;
    MCL_NEW(security_handler);
    security_handler->client_secret = MCL_NULL;

    security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();

    // Mock file util.
    mcl_file_util_fopen_IgnoreAndReturn(MCL_OK);
    mcl_file_util_fclose_IgnoreAndReturn(MCL_OK);

    core_processor_t *core_processor = MCL_NULL;
    mcl_error_t code = core_processor_initialize(configuration, &core_processor);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "core_processor_initialize() does not return MCL_OK.");
    TEST_ASSERT_NOT_NULL_MESSAGE(core_processor, "Core processor is null after initialization.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("client_id:0123456789", core_processor->security_handler->client_id, "Client id is not loaded correctly");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("client_secret:123456789", core_processor->security_handler->client_secret, "Client secret is not loaded correctly");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("rat:123456789", core_processor->security_handler->registration_access_token, "Rat is not loaded correctly");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("uri:southgate.eu1.mindsphere.io", core_processor->security_handler->registration_uri, "Uri is not loaded correctly");

    // Clean up.
    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    core_processor_destroy(&core_processor);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
    MCL_FREE(security_handler->client_id);
    MCL_FREE(security_handler->client_secret);
    MCL_FREE(security_handler->registration_access_token);
    MCL_FREE(security_handler->registration_uri);
    MCL_FREE(security_handler);
}

/**
 * GIVEN : Callback functions for loading and saving credentials are provided for RSA security profile, initial access token is null.
 * WHEN  : core_processor_initialize() is called.
 * THEN  : MCL_OK is returned.
 */
void test_initialize_005(void)
{
    // Mock configuration.
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_RSA_3072, MCL_NULL, &configuration);
    configuration->credentials_load_callback.rsa = (mcl_credentials_load_rsa_callback_t) _custom_load_credentials_rsa_info_func;
    configuration->credentials_save_callback.rsa = (mcl_credentials_save_rsa_callback_t) _custom_save_credentials_rsa_info_func;

    security_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_client_initialize_IgnoreAndReturn(MCL_OK);
    security_handler_generate_rsa_key_IgnoreAndReturn(MCL_OK);

    // Mock security handler.
    security_handler_t *security_handler = NULL;
    MCL_NEW(security_handler);
    security_handler->client_secret = MCL_NULL;

    security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();

    // Mock file util.
    mcl_file_util_fopen_IgnoreAndReturn(MCL_OK);
    mcl_file_util_fclose_IgnoreAndReturn(MCL_OK);

    core_processor_t *core_processor = MCL_NULL;
    mcl_error_t code = core_processor_initialize(configuration, &core_processor);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "core_processor_initialize() does not return MCL_OK.");
    TEST_ASSERT_NOT_NULL_MESSAGE(core_processor, "Core processor is null after initialization.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(core_processor->security_handler->client_id, "client_id:10", "Client id is not loaded correctly");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(core_processor->security_handler->rsa.public_key, "public_key:123456789", "Public key is not loaded correctly");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(core_processor->security_handler->rsa.private_key, "private_key:123456789", "Private key is not loaded correctly");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(core_processor->security_handler->registration_access_token, "rat:123456789", "Rat is not loaded correctly");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(core_processor->security_handler->registration_uri, "uri:southgate.eu1.mindsphere.io", "Uri is not loaded correctly");

    // Clean up.
    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    core_processor_destroy(&core_processor);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
    MCL_FREE(security_handler->client_id);
    MCL_FREE(security_handler->client_secret);
    MCL_FREE(security_handler->registration_access_token);
    MCL_FREE(security_handler->registration_uri);
    MCL_FREE(security_handler->rsa.public_key);
    MCL_FREE(security_handler->rsa.private_key);
    MCL_FREE(security_handler);
}

/**
 * GIVEN : Initialized core processor.
 * WHEN  : core_processor_destroy() is called.
 * THEN  : Core processor is destroyed and set to null.
 */
void test_destroy_001(void)
{
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_SHARED_SECRET, "InitialAccessToken", &configuration);

    security_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_client_initialize_IgnoreAndReturn(MCL_OK);
    security_handler_initialize_IgnoreAndReturn(MCL_OK);

    core_processor_t *core_processor = MCL_NULL;
    core_processor_initialize(configuration, &core_processor);
    TEST_ASSERT_NOT_NULL_MESSAGE(core_processor, "Core processor is null after initialization.");

    // Clean up.
    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    core_processor_destroy(&core_processor);
    MCL_FREE(configuration->initial_access_token);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);

    TEST_ASSERT_NULL_MESSAGE(core_processor, "Core processor is not null after it is destroyed.");
}

/**
 * GIVEN : An agent not onboarded, security profile is MCL_SECURITY_SHARED_SECRET.
 * WHEN  : core_processor_register() is called.
 * THEN  : MCL_OK is returned.
 */
void test_register_001(void)
{
    // Mock configuration.
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_SHARED_SECRET, "InitialAccessToken", &configuration);

    security_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_client_initialize_IgnoreAndReturn(MCL_OK);

    // Mock security handler.
    security_handler_t *security_handler = NULL;
    _create_security_handler(&security_handler);

    security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

    // Initialize core processor.
    core_processor_t *core_processor = MCL_NULL;
    core_processor_initialize(configuration, &core_processor);

    // Mock http request.
    mcl_http_request_t *http_request = NULL;
    MCL_NEW(http_request);
    http_request->header = MCL_NULL;
    http_request->payload_size = 2;
    http_request->payload = MCL_MALLOC(http_request->payload_size);
    http_request->uri = MCL_NULL;
    http_request->stream_data = MCL_NULL;

    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_initialize_ReturnThruPtr_http_request(&http_request);
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);
    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);

    // Correlation-ID.
    security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);

    // Mock http response.
    mcl_http_response_t *http_response;
    char http_response_string[] = "{\"client_id\":\"zxc\", \"client_secret\":\"dummy_secret\", \"registration_access_token\":\"123\", \"registration_client_uri\":\"dummy_host/register\"}";
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_CREATED;
    http_response->payload_size = sizeof(http_response_string) - 1;
    http_response->payload = MCL_MALLOC(http_response->payload_size);
    mcl_string_util_memcpy(http_response->payload, http_response_string, sizeof(http_response_string) - 1);

    // Mock http client.
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    mcl_error_t code = core_processor_register(core_processor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "core_processor_register failed.");

    // Clean up.
    MCL_FREE(http_response->payload);
    MCL_FREE(http_response);
    MCL_FREE(http_request->payload);
    MCL_FREE(http_request);
    MCL_FREE(security_handler->client_id);
    MCL_FREE(security_handler->registration_access_token);
    MCL_FREE(security_handler->client_secret);
    MCL_FREE(security_handler->registration_uri);
    MCL_FREE(security_handler);
    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    core_processor_destroy(&core_processor);
    MCL_FREE(configuration->initial_access_token);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
}

/**
 * GIVEN : An agent already onboarded, security profile is MCL_SECURITY_SHARED_SECRET.
 * WHEN  : core_processor_register() is called.
 * THEN  : MCL_OK is returned.
 */
void test_register_002(void)
{
    // Mock configuration.
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_SHARED_SECRET, "InitialAccessToken", &configuration);

    security_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_client_initialize_IgnoreAndReturn(MCL_OK);

    // Mock security handler.
    security_handler_t *security_handler = NULL;
    _create_security_handler(&security_handler);

    // It is already onboarded and it has RAT.
    security_handler->registration_access_token = string_util_strdup("registration_access_token");
    security_handler->client_id = string_util_strdup("client_id");
    security_handler->registration_uri = string_util_strdup("registration_uri");

    security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

    // Initialize core processor.
    core_processor_t *core_processor = MCL_NULL;
    core_processor_initialize(configuration, &core_processor);

    // Mock http request.
    mcl_http_request_t *http_request = NULL;
    MCL_NEW(http_request);
    http_request->header = MCL_NULL;
    http_request->payload_size = 100;
    http_request->payload = MCL_MALLOC(http_request->payload_size);
    http_request->uri = MCL_NULL;
    http_request->stream_data = MCL_NULL;

    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_initialize_ReturnThruPtr_http_request(&http_request);
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);
    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);

    // Correlation-ID.
    security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);

    // Mock http response.
    mcl_http_response_t *http_response;
    char http_response_string[] = "{\"client_id\":\"zxc\", \"client_secret\":\"dummy_secret\", \"registration_access_token\":\"123\", \"registration_client_uri\":\"dummy_host/register\"}";
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    http_response->payload_size = sizeof(http_response_string) - 1;
    http_response->payload = MCL_MALLOC(http_response->payload_size);
    mcl_string_util_memcpy(http_response->payload, http_response_string, sizeof(http_response_string) - 1);

    // Mock http client.
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    mcl_error_t code = core_processor_register(core_processor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "core_processor_register failed.");

    // Clean up.
    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    core_processor_destroy(&core_processor);
    MCL_FREE(http_request->payload);
    MCL_FREE(http_request);
    MCL_FREE(http_response->payload);
    MCL_FREE(http_response);
    MCL_FREE(configuration->initial_access_token);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
    MCL_FREE(security_handler->client_secret);
    MCL_FREE(security_handler->registration_access_token);
    MCL_FREE(security_handler->client_id);
    MCL_FREE(security_handler->registration_uri);
    MCL_FREE(security_handler);
}

/**
 * GIVEN : An agent not onboarded, security profile is MCL_SECURITY_RSA_3072.
 * WHEN  : core_processor_register() is called.
 * THEN  : MCL_OK is returned.
 */
void test_register_003(void)
{
    // Mock configuration.
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_RSA_3072, "InitialAccessToken", &configuration);

    security_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_client_initialize_IgnoreAndReturn(MCL_OK);

    // Mock security handler.
    security_handler_t *security_handler = NULL;
    _create_security_handler(&security_handler);

    security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);
    security_handler_generate_rsa_key_IgnoreAndReturn(MCL_OK);

    // Initialize core processor.
    core_processor_t *core_processor = MCL_NULL;
    core_processor_initialize(configuration, &core_processor);

    // Mock http request.
    mcl_http_request_t *http_request = NULL;
    MCL_NEW(http_request);
    http_request->header = MCL_NULL;
    http_request->payload_size = 100;
    http_request->payload = MCL_MALLOC(http_request->payload_size);
    http_request->uri = MCL_NULL;
    http_request->stream_data = MCL_NULL;

    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_initialize_ReturnThruPtr_http_request(&http_request);
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);
    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);

    // Correlation-ID.
    security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    char *modulus = MCL_MALLOC(2);
    char *public_exponent = MCL_MALLOC(2);
    string_util_memcpy(modulus, "3", 2);
    string_util_memcpy(public_exponent, "5", 2);
    security_rsa_get_modulus_and_exponent_ExpectAnyArgsAndReturn(MCL_OK);
    security_rsa_get_modulus_and_exponent_ReturnThruPtr_exponent(&public_exponent);
    security_rsa_get_modulus_and_exponent_ReturnThruPtr_modulus(&modulus);
    security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);

    // Mock http response.
    mcl_http_response_t *http_response;
    char http_response_string[] = "{\"client_id\":\"zxc\", \"client_secret\":\"dummy_secret\", \"registration_access_token\":\"123\", \"registration_client_uri\":\"dummy_host/register\"}";
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_CREATED;
    http_response->payload_size = sizeof(http_response_string) - 1;
    http_response->payload = MCL_MALLOC(http_response->payload_size);
    mcl_string_util_memcpy(http_response->payload, http_response_string, sizeof(http_response_string) - 1);

    // Mock http client.
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    mcl_error_t code = core_processor_register(core_processor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "core_processor_register failed.");

    // Clean up.
    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    core_processor_destroy(&core_processor);
    MCL_FREE(http_response->payload);
    MCL_FREE(http_response);
    MCL_FREE(http_request->payload);
    MCL_FREE(http_request);
    MCL_FREE(security_handler->registration_access_token);
    MCL_FREE(security_handler->client_id);
    MCL_FREE(security_handler->registration_uri);
    MCL_FREE(security_handler);
    MCL_FREE(configuration->initial_access_token);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
}

/**
 * GIVEN : An agent already onboarded, security profile is MCL_SECURITY_RSA_3072.
 * WHEN  : core_processor_register() is called.
 * THEN  : MCL_OK is returned.
 */
void test_register_004(void)
{
    // Mock configuration.
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_RSA_3072, "InitialAccessToken", &configuration);

    security_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_client_initialize_IgnoreAndReturn(MCL_OK);

    // Mock security handler.
    security_handler_t *security_handler = NULL;
    _create_security_handler(&security_handler);

    // It is already onboarded and it has RAT.
    security_handler->registration_access_token = string_util_strdup("registration_access_token");
    security_handler->client_id = string_util_strdup("client_id");
    security_handler->registration_uri = string_util_strdup("registration_uri");

    security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);
    security_handler_generate_rsa_key_IgnoreAndReturn(MCL_OK);

    // Initialize core processor.
    core_processor_t *core_processor = MCL_NULL;
    core_processor_initialize(configuration, &core_processor);

    char *modulus = MCL_MALLOC(2);
    char *public_exponent = MCL_MALLOC(2);
    string_util_memcpy(modulus, "3", 2);
    string_util_memcpy(public_exponent, "5", 2);
    security_rsa_get_modulus_and_exponent_ExpectAnyArgsAndReturn(MCL_OK);
    security_rsa_get_modulus_and_exponent_ReturnThruPtr_exponent(&public_exponent);
    security_rsa_get_modulus_and_exponent_ReturnThruPtr_modulus(&modulus);
    security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    // Mock http request.
    mcl_http_request_t *http_request = NULL;
    MCL_NEW(http_request);
    http_request->header = MCL_NULL;
    http_request->payload_size = 100;
    http_request->payload = MCL_MALLOC(http_request->payload_size);
    http_request->uri = MCL_NULL;
    http_request->stream_data = MCL_NULL;

    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_initialize_ReturnThruPtr_http_request(&http_request);
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);
    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);

    // Correlation-ID.
    security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);

    // Mock http response.
    mcl_http_response_t *http_response;

    MCL_NEW(http_response);
    char http_response_string[] = "{\"client_id\":\"zxc\", \"client_secret\":\"dummy_secret\", \"registration_access_token\":\"123\", \"registration_client_uri\":\"dummy_host/register\"}";
    http_response->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    http_response->payload_size = sizeof(http_response_string) - 1;
    http_response->payload = MCL_MALLOC(http_response->payload_size);
    mcl_string_util_memcpy(http_response->payload, http_response_string, sizeof(http_response_string) - 1);

    // Mock http client.
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    mcl_error_t code = core_processor_register(core_processor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "core_processor_register failed.");

    // Clean up.
    MCL_FREE(security_handler->registration_access_token);
    MCL_FREE(security_handler->client_id);
    MCL_FREE(security_handler->registration_uri);
    MCL_FREE(security_handler);
    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    core_processor_destroy(&core_processor);
    MCL_FREE(http_request->payload);
    MCL_FREE(http_request);
    MCL_FREE(http_response->payload);
    MCL_FREE(http_response);
    MCL_FREE(configuration->initial_access_token);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
}

/**
 * GIVEN : Registration Access Token is present, security profile is MCL_SECURITY_SHARED_SECRET and there is not enough memory.
 * WHEN  : core_processor_register() is called.
 * THEN  : MCL_FAIL is returned.
 */
void test_register_005(void)
{
    // Mock configuration.
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_SHARED_SECRET, "InitialAccessToken", &configuration);

    // Mock security handler.
    security_handler_t *security_handler = NULL;
    _create_security_handler(&security_handler);

    // It is already onboarded and it has RAT.
    security_handler->registration_access_token = string_util_strdup("registration_access_token");
    security_handler->client_id = string_util_strdup("client_id");
    security_handler->registration_uri= string_util_strdup("registration_uri");

    // Initialize core processor.
    core_processor_t *core_processor = MCL_NULL;
    MCL_NEW(core_processor);
    core_processor->configuration = configuration;
    core_processor->security_handler = security_handler;

    mcl_http_request_initialize_IgnoreAndReturn(MCL_FAIL);
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    mcl_error_t result = core_processor_register(core_processor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_FAIL, result, "core_processor_register failed.");

    // Clean up.
    MCL_FREE(configuration->initial_access_token);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
    MCL_FREE((security_handler->registration_access_token));
    MCL_FREE((security_handler->client_id));
    MCL_FREE(security_handler->client_secret);
    MCL_FREE(security_handler->registration_uri);
    free(security_handler);
    free(core_processor);
}

/**
 * GIVEN : An agent already onboarded, security profile is MCL_SECURITY_SHARED_SECRET and credentials could not be saved by callback function.
 * WHEN  : core_processor_register() is called.
 * THEN  : MCL_CREDENTIALS_NOT_SAVED is returned.
 */
void test_register_006(void)
{
    // Mock configuration.
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_SHARED_SECRET, "InitialAccessToken", &configuration);
    configuration->credentials_load_callback.shared_secret = (mcl_credentials_load_shared_secret_callback_t) _custom_load_credentials_shared_secret_info_func;
    configuration->credentials_save_callback.shared_secret = (mcl_credentials_save_shared_secret_callback_t) _custom_save_credentials_shared_secret_info_fail_func;

    security_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_client_initialize_IgnoreAndReturn(MCL_OK);

    // Mock security handler.
    security_handler_t *security_handler = NULL;
    _create_security_handler(&security_handler);

    security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

    // Mock file util.
    mcl_file_util_fopen_IgnoreAndReturn(MCL_OK);
    mcl_file_util_fclose_IgnoreAndReturn(MCL_OK);

    // Initialize core processor.
    core_processor_t *core_processor = MCL_NULL;
    core_processor_initialize(configuration, &core_processor);

    // Mock http request.
    mcl_http_request_t *http_request = NULL;
    MCL_NEW(http_request);
    http_request->header = MCL_NULL;
    http_request->payload_size = 100;
    http_request->payload = MCL_MALLOC(http_request->payload_size);
    http_request->uri = MCL_NULL;
    http_request->stream_data = MCL_NULL;

    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_initialize_ReturnThruPtr_http_request(&http_request);
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);
    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);

    // Correlation-ID.
    security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);

    // Mock http response.
    mcl_http_response_t *http_response;
    char http_response_string[] = "{\"client_id\":\"zxc\", \"client_secret\":\"dummy_secret\", \"registration_access_token\":\"123\", \"registration_client_uri\":\"dummy_host/register\"}";
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    http_response->payload_size = sizeof(http_response_string) - 1;
    http_response->payload = MCL_MALLOC(http_response->payload_size);
    mcl_string_util_memcpy(http_response->payload, http_response_string, sizeof(http_response_string) - 1);

    // Mock http client.
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    mcl_error_t code = core_processor_register(core_processor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_CREDENTIALS_NOT_SAVED, code, "core_processor_register failed.");

    // Clean up.
    MCL_FREE(security_handler->client_secret);
    MCL_FREE(security_handler->registration_access_token);
    MCL_FREE(security_handler->client_id);
    MCL_FREE(security_handler->registration_uri);
    MCL_FREE(security_handler);
    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    core_processor_destroy(&core_processor);
    MCL_FREE(http_request->payload);
    MCL_FREE(http_request);
    MCL_FREE(http_response->payload);
    MCL_FREE(http_response);
    MCL_FREE(configuration->initial_access_token);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
}

/**
 * GIVEN : Core processor is initialized using shared secret security profile.
 * WHEN  : core_processor_update_credentials() is called.
 * THEN  : MCL_OK is returned.
 */
void test_update_credentials_001(void)
{
    // Mock configuration.
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_SHARED_SECRET, "InitialAccessToken", &configuration);
    configuration->credentials_load_callback.shared_secret = load_for_update_security_test;
    configuration->credentials_save_callback.shared_secret = save_for_update_security_test;

    security_handler_t *security_handler = MCL_NULL;
    MCL_NEW(security_handler);
    memset(security_handler, 0, sizeof(security_handler_t));

    security_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_client_initialize_IgnoreAndReturn(MCL_OK);
    security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

    update_security_state = 0;

    core_processor_t *core_processor = MCL_NULL;
    mcl_error_t result = core_processor_initialize(configuration, &core_processor);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, result, "core_processor_initialize() does not return MCL_OK.");
    TEST_ASSERT_NOT_NULL_MESSAGE(core_processor, "Core processor is null after initialization.");

    update_security_state = 1;

    result = core_processor_update_credentials(core_processor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, result, "core_processor_update_credentials() does not return MCL_OK.");

    // Clean up.
    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    core_processor_destroy(&core_processor);
    MCL_FREE(configuration->initial_access_token);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
    MCL_FREE(security_handler->client_id);
    MCL_FREE(security_handler->client_secret);
    MCL_FREE(security_handler->registration_access_token);
    MCL_FREE(security_handler->registration_uri);
    MCL_FREE(security_handler);
}

/**
 * GIVEN : Core processor is initialized using RSA security profile.
 * WHEN  : core_processor_update_credentials() is called.
 * THEN  : MCL_OK is returned.
 */
void test_update_credentials_002(void)
{
    // Mock configuration.
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_RSA_3072, "InitialAccessToken", &configuration);
    configuration->credentials_load_callback.rsa = load_for_update_security_test_rsa;
    configuration->credentials_save_callback.rsa = save_for_update_security_test_rsa;

    security_handler_t *security_handler = MCL_NULL;
    MCL_NEW(security_handler);
    memset(security_handler, 0, sizeof(security_handler_t));

    security_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_client_initialize_IgnoreAndReturn(MCL_OK);
    security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

    update_security_state = 0;

    core_processor_t *core_processor = MCL_NULL;
    mcl_error_t result = core_processor_initialize(configuration, &core_processor);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, result, "core_processor_initialize() does not return MCL_OK.");
    TEST_ASSERT_NOT_NULL_MESSAGE(core_processor, "Core processor is null after initialization.");

    update_security_state = 1;

    result = core_processor_update_credentials(core_processor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, result, "core_processor_update_credentials() does not return MCL_OK.");

    // Clean up.
    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    core_processor_destroy(&core_processor);
    MCL_FREE(configuration->initial_access_token);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
    MCL_FREE(security_handler->client_id);
    MCL_FREE(security_handler->rsa.public_key);
    MCL_FREE(security_handler->rsa.private_key);
    MCL_FREE(security_handler->registration_access_token);
    MCL_FREE(security_handler->registration_uri);
    MCL_FREE(security_handler);
}

/**
 * GIVEN : Core processor is initialized using RSA security profile.
 * WHEN  : core_processor_update_credentials() is called when it is up to date.
 * THEN  : MCL_CREDENTIALS_UP_TO_DATE is returned.
 */
void test_update_credentials_003(void)
{
    // Mock configuration.
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_RSA_3072, "InitialAccessToken", &configuration);
    configuration->credentials_load_callback.rsa = load_for_update_security_test_rsa;
    configuration->credentials_save_callback.rsa = save_for_update_security_test_rsa;

    security_handler_t *security_handler = MCL_NULL;
    MCL_NEW(security_handler);
    memset(security_handler, 0, sizeof(security_handler_t));

    security_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_client_initialize_IgnoreAndReturn(MCL_OK);
    security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

    update_security_state = 0;

    core_processor_t *core_processor = MCL_NULL;
    mcl_error_t result = core_processor_initialize(configuration, &core_processor);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, result, "core_processor_initialize() does not return MCL_OK.");
    TEST_ASSERT_NOT_NULL_MESSAGE(core_processor, "Core processor is null after initialization.");

    result = core_processor_update_credentials(core_processor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_CREDENTIALS_UP_TO_DATE, result, "core_processor_update_credentials() does not return MCL_CREDENTIALS_UP_TO_DATE.");

    // Clean up.
    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    core_processor_destroy(&core_processor);
    MCL_FREE(configuration->initial_access_token);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
    MCL_FREE(security_handler->client_id);
    MCL_FREE(security_handler->rsa.public_key);
    MCL_FREE(security_handler->rsa.private_key);
    MCL_FREE(security_handler->registration_access_token);
    MCL_FREE(security_handler->registration_uri);
    MCL_FREE(security_handler);
}

/**
 * GIVEN : Core processor is initialized.
 * WHEN  : core_processor_get_access_token() is called.
 * THEN  : MCL_OK is returned and access token is received.
 */
void test_get_access_token_001(void)
{
    // Mock configuration.
    core_configuration_t *configuration = MCL_NULL;
    _create_configuration(MCL_SECURITY_SHARED_SECRET, "InitialAccessToken", &configuration);

    security_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_client_initialize_IgnoreAndReturn(MCL_OK);

    // Mock security handler.
    security_handler_t *security_handler = NULL;
    _create_security_handler(&security_handler);

    security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

    // Initialize core processor.
    core_processor_t *core_processor = MCL_NULL;
    core_processor_initialize(configuration, &core_processor);

    // Mock jwt.
    jwt_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    jwt_destroy_Ignore();

    char *jwt_string = string_util_strdup("jwt_string");
    jwt_get_token_ExpectAnyArgsAndReturn(jwt_string);

    // Mock http request.
    mcl_http_request_t *http_request = NULL;
    MCL_NEW(http_request);
    http_request->header = MCL_NULL;
    http_request->payload_size = 500;
    http_request->payload = MCL_MALLOC(http_request->payload_size);
    http_request->uri = MCL_NULL;
    http_request->stream_data = MCL_NULL;

    mcl_http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_request_initialize_ReturnThruPtr_http_request(&http_request);
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);
    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);

    // Correlation-ID.
    security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);

    // Mock http response.
    mcl_http_response_t *http_response;
    char http_response_string[] = "{\"access_token\":\"eyJraWQiOi...\",\"token_type\":\"Bearer\",\"expires_in\":3600,\"scope\":[\"mdsp:core:DefaultAgent\"],\"jti\":\"43bac...\"}";
    MCL_NEW(http_response);
    http_response->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    http_response->payload_size = sizeof(http_response_string) - 1;
    http_response->payload = MCL_MALLOC(http_response->payload_size);
    mcl_string_util_memcpy(http_response->payload, http_response_string, sizeof(http_response_string) - 1);

    // Mock http client.
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    mcl_http_response_get_header_IgnoreAndReturn(MCL_OK);

    // Mock destroy functions.
    mcl_http_request_destroy_Ignore();
    mcl_http_response_destroy_Ignore();

    mcl_error_t code = core_processor_get_access_token(core_processor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "core_processor_get_access_token() does not return MCL_OK.");

    TEST_ASSERT_EQUAL_STRING_MESSAGE("eyJraWQiOi...", core_processor->security_handler->access_token, "Access token is not correct.");

    // Clean up.
    mcl_http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    core_processor_destroy(&core_processor);
    MCL_FREE(http_request->payload);
    MCL_FREE(http_request);
    MCL_FREE(http_response->payload);
    MCL_FREE(http_response);
    MCL_FREE(security_handler->access_token);
    MCL_FREE(security_handler->last_token_time);
    MCL_FREE(security_handler);
    MCL_FREE(configuration->initial_access_token);
    MCL_FREE(configuration->token_endpoint);
    MCL_FREE(configuration->mindsphere_hostname);
    MCL_FREE(configuration);
}

static mcl_error_t _custom_load_credentials_shared_secret_info_func(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri)
{
    char client_id_test[] = "client_id:0123456789";
    *client_id = MCL_MALLOC(string_util_strlen(client_id_test) + 1);
    string_util_strncpy(*client_id, client_id_test, string_util_strlen(client_id_test));
    (*client_id)[20] = MCL_NULL_CHAR;

    char *client_secret_test = "client_secret:123456789";
    *client_secret = MCL_MALLOC(string_util_strlen(client_secret_test) + 1);
    string_util_strncpy(*client_secret, client_secret_test, string_util_strlen(client_secret_test));
    (*client_secret)[23] = MCL_NULL_CHAR;

    char *rat_test = "rat:123456789";
    *registration_access_token = MCL_MALLOC(string_util_strlen(rat_test) + 1);
    string_util_strncpy(*registration_access_token, rat_test, string_util_strlen(rat_test));
    (*registration_access_token)[13] = MCL_NULL_CHAR;

    char *registration_uri_test = "uri:southgate.eu1.mindsphere.io";
    *registration_uri = MCL_MALLOC(string_util_strlen(registration_uri_test) + 1);
    string_util_strncpy(*registration_uri, registration_uri_test, string_util_strlen(registration_uri_test));
    (*registration_uri)[31] = MCL_NULL_CHAR;

    return MCL_OK;
}

static mcl_error_t _custom_save_credentials_shared_secret_info_func(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri)
{
    return MCL_OK;
}

static mcl_error_t _custom_load_credentials_rsa_info_func(char **client_id, char **public_key, char **private_key, char **registration_access_token, char **registration_uri)
{
    char *client_id_test = "client_id:10";
    *client_id = MCL_MALLOC(string_util_strlen(client_id_test) + 1);
    string_util_strncpy(*client_id, client_id_test, string_util_strlen(client_id_test));
    (*client_id)[12] = MCL_NULL_CHAR;

    char *public_key_test = "public_key:123456789";
    *public_key = MCL_MALLOC(string_util_strlen(public_key_test) + 1);
    string_util_strncpy(*public_key, public_key_test, string_util_strlen(public_key_test));
    (*public_key)[20] = MCL_NULL_CHAR;

    char *private_key_test = "private_key:123456789";
    *private_key = MCL_MALLOC(string_util_strlen(private_key_test) + 1);
    string_util_strncpy(*private_key, private_key_test, string_util_strlen(private_key_test));
    (*private_key)[21] = MCL_NULL_CHAR;

    char *rat_test = "rat:123456789";
    *registration_access_token = MCL_MALLOC(string_util_strlen(rat_test) + 1);
    string_util_strncpy(*registration_access_token, rat_test, string_util_strlen(rat_test));
    (*registration_access_token)[13] = MCL_NULL_CHAR;

    char *registration_uri_test = "uri:southgate.eu1.mindsphere.io";
    *registration_uri = MCL_MALLOC(string_util_strlen(registration_uri_test) + 1);
    string_util_strncpy(*registration_uri, registration_uri_test, string_util_strlen(registration_uri_test));
    (*registration_uri)[31] = MCL_NULL_CHAR;

    return MCL_OK;
}

static mcl_error_t _custom_save_credentials_rsa_info_func(char **client_id, char **public_key, char **private_key, char **registration_access_token, char **registration_uri)
{
    return MCL_OK;
}

static mcl_error_t _custom_save_credentials_shared_secret_info_fail_func(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri)
{
    return MCL_CREDENTIALS_NOT_SAVED;
}

static mcl_error_t load_for_update_security_test(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri)
{
    *client_id = calloc(5, 1);
    snprintf(*client_id, 5, "1234");

    *registration_access_token = calloc(5, 1);
    snprintf(*registration_access_token, 5, "1234");

    *registration_uri = calloc(5, 1);
    snprintf(*registration_uri, 5, "1234");

    *client_secret = calloc(5, 1);

    if (0 == update_security_state)
    {
        snprintf(*client_secret, 5, "1234");
    }
    else
    {
        snprintf(*client_secret, 5, "abcd");
    }

    return MCL_OK;
}

static mcl_error_t save_for_update_security_test(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri)
{
    return MCL_OK;
}

static mcl_error_t load_for_update_security_test_rsa(char **client_id, char **public_key, char **private_key, char **registration_access_token, char **registration_uri)
{
    *client_id = calloc(5, 1);
    snprintf(*client_id, 5, "1234");

    *registration_access_token = calloc(5, 1);
    snprintf(*registration_access_token, 5, "1234");

    *registration_uri = calloc(5, 1);
    snprintf(*registration_uri, 5, "1234");

    *public_key = calloc(5, 1);
    *private_key = calloc(5, 1);

    if (0 == update_security_state)
    {
        snprintf(*public_key, 5, "1234");
        snprintf(*private_key, 5, "5678");
    }
    else
    {
        snprintf(*public_key, 5, "abcd");
        snprintf(*private_key, 5, "efgh");
    }

    return MCL_OK;
}

static mcl_error_t save_for_update_security_test_rsa(const char *client_id, const char *public_key, const char *private_key, const char *registration_access_token, const char *registration_uri)
{
    return MCL_OK;
}

static void _create_configuration(E_MCL_SECURITY_PROFILE security_profile, char *initial_access_token, core_configuration_t **configuration)
{
    MCL_NEW(*configuration);
    (*configuration)->mindsphere_hostname = string_util_strdup("https://brsm-MindConnectCom.cfapps.industrycloud-staging.siemens.com");
    (*configuration)->security_profile = security_profile;

    if (MCL_NULL != initial_access_token)
    {
        (*configuration)->initial_access_token = string_util_strdup("initial_access_token");
    }
    else
    {
        (*configuration)->initial_access_token = MCL_NULL;
    }

    (*configuration)->mindsphere_port = 0;
    (*configuration)->mindsphere_certificate = MCL_NULL;
    (*configuration)->proxy_hostname = MCL_NULL;
    (*configuration)->proxy_port = 0;
    (*configuration)->proxy_type = MCL_PROXY_UNKNOWN;
    (*configuration)->proxy_username = MCL_NULL;
    (*configuration)->proxy_password = MCL_NULL;
    (*configuration)->proxy_domain = MCL_NULL;
    (*configuration)->http_request_timeout = DEFAULT_HTTP_REQUEST_TIMEOUT;
    (*configuration)->user_agent = MCL_NULL;
    (*configuration)->tenant = MCL_NULL;
    (*configuration)->credentials_load_callback.rsa = MCL_NULL;
    (*configuration)->credentials_save_callback.rsa = MCL_NULL;
    (*configuration)->critical_section_enter_callback = MCL_NULL;
    (*configuration)->critical_section_leave_callback = MCL_NULL;
    (*configuration)->register_endpoint = MCL_NULL;
    (*configuration)->token_endpoint = MCL_NULL;
}

static void _create_security_handler(security_handler_t **security_handler)
{
    MCL_NEW(*security_handler);
    (*security_handler)->rsa.private_key = MCL_NULL;
    (*security_handler)->rsa.public_key = MCL_NULL;
    (*security_handler)->access_token = MCL_NULL;
    (*security_handler)->registration_access_token = MCL_NULL;
    (*security_handler)->client_secret = MCL_NULL;
    (*security_handler)->client_id = MCL_NULL;
    (*security_handler)->registration_uri = MCL_NULL;
    (*security_handler)->last_token_time = MCL_NULL;
}
