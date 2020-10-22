/*!
 * @file     test_configuration.c
 * @brief    This file contains test case functions to test configuration module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "core_configuration.h"
#include "definitions.h"
#include "memory.h"
#include "mcl_core/mcl_core_configuration.h"
#include "mcl_core/mcl_log_util.h"
#include "mcl_core/mcl_string_util.h"
#include "string_util.h"

static void _dummy_credentials_load_callback();
static void _dummy_credentials_save_callback();
static void _dummy_critical_section_enter_callback();
static void _dummy_critical_section_leave_callback();

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : No initial condition.
 * WHEN  : mcl_core_configuration_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized configuration struct.
 */
void test_initialize_001(void)
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for configuration.");
    TEST_ASSERT_NOT_NULL_MESSAGE(configuration, "Configuration is null.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Configuration parameter is null.
 * WHEN  : mcl_core_configuration_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002(void)
{
    // Test.
    mcl_error_t code = mcl_core_configuration_initialize(MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_core_configuration_initialize() unexpected return error.");
}

/**
 * GIVEN : Core configuration parameter is NULL.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_001()
{
    // Variable declaration.
    const char *dummy_value = "dummy";

    // Test.
    mcl_error_t code = mcl_core_configuration_set_parameter(MCL_NULL, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, dummy_value);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_core_configuration_set_parameter() failed when core configuration is NULL.");
}

/**
 * GIVEN : Value parameter is NULL.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_002()
{
    // Variable declaration.
    mcl_core_configuration_t configuration;

    // Test.
    mcl_error_t code = mcl_core_configuration_set_parameter(&configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_core_configuration_set_parameter() failed when value is NULL.");
}

/**
 * GIVEN : Invalid parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_003()
{
    // Variable declaration.
    mcl_core_configuration_t configuration;
    const char *dummy_value = "dummy";

    // Test.
    mcl_error_t code = mcl_core_configuration_set_parameter(&configuration, MCL_CORE_CONFIGURATION_PARAMETER_END, dummy_value);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_core_configuration_set_parameter() failed when parameter is invalid.");
}

/**
 * GIVEN : Mindsphere hostname parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new mindsphere hostname parameter of the core configuration is set.
 */
void test_set_parameter_004()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *hostname = "https://localhost";

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, hostname);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(hostname, configuration->mindsphere_hostname, "Mindsphere hostname parameter is wrong.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Mindsphere port parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new mindsphere port parameter of the core configuration is set.
 */
void test_set_parameter_005()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    mcl_uint16_t port = 1080;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &port);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(port, configuration->mindsphere_port, "Mindsphere port parameter is wrong.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Mindsphere certificate parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new mindsphere certificate parameter of the core configuration is set.
 */
void test_set_parameter_006()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *certificate = "mindsphere_certificate";

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, certificate);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(certificate, configuration->mindsphere_certificate, "Mindsphere certificate parameter is wrong.");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_FALSE, configuration->certificate_is_file, "Certificate is not given as file.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Proxy type parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new proxy type parameter of the core configuration is set.
 */
void test_set_parameter_007()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP_1_0;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_MESSAGE(proxy_type, configuration->proxy_type, "Proxy type parameter is wrong.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Proxy type parameter is invalid.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_008()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    E_MCL_PROXY proxy_type = MCL_PROXY_END;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_core_configuration_set_parameter() unexpected return code.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Proxy hostname parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new proxy hostname parameter of the core configuration is set.
 */
void test_set_parameter_009()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *proxy_hostname = "http://127.0.0.1";

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(proxy_hostname, configuration->proxy_hostname, "Proxy hostname parameter is wrong.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Proxy port parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new proxy port parameter of the core configuration is set.
 */
void test_set_parameter_010()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    mcl_uint16_t proxy_port = 8888;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(proxy_port, configuration->proxy_port, "Proxy port parameter is wrong.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Proxy username parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new proxy username parameter of the core configuration is set.
 */
void test_set_parameter_011()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *proxy_username = "proxy_username";

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_USER, proxy_username);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(proxy_username, configuration->proxy_username, "Proxy username parameter is wrong.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Proxy password parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new proxy password parameter of the core configuration is set.
 */
void test_set_parameter_012()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *proxy_password = "proxy_password";

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PASS, proxy_password);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(proxy_password, configuration->proxy_password, "Proxy password parameter is wrong.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Proxy domain parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new proxy domain parameter of the core configuration is set.
 */
void test_set_parameter_013()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *proxy_domain = "proxy_domain";

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_DOMAIN, proxy_domain);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(proxy_domain, configuration->proxy_domain, "Proxy domain parameter is wrong.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Security profile parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new security profile parameter of the core configuration is set.
 */
void test_set_parameter_014()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_MESSAGE(security_profile, configuration->security_profile, "Security profile parameter is wrong.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Security profile parameter is invalid.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_015()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_PROFILE_END;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_core_configuration_set_parameter() unexpected return code.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : HTTP request timeout parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new HTTP request timeout parameter of the core configuration is set.
 */
void test_set_parameter_016()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    mcl_size_t timeout_value = 250;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_HTTP_REQUEST_TIMEOUT, &timeout_value);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(timeout_value, configuration->http_request_timeout, "HTTP request timeout parameter is wrong.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : User agent parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new user agent parameter of the core configuration is set.
 */
void test_set_parameter_017()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *user_agent = "custom agent v1.0";

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("MCL/" MCL_VERSION_STRING " (custom agent v1.0)", configuration->user_agent, "User agent  parameter is wrong.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Tenant parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new tenant parameter of the core configuration is set.
 */
void test_set_parameter_018()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *tenant = "test_tenant";

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(tenant, configuration->tenant, "Tenant parameter is wrong.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Initial access token parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new initial access token parameter of the core configuration is set.
 */
void test_set_parameter_019()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *iat = "test_iat";

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(iat, configuration->initial_access_token, "Initial access token parameter is wrong.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Loading credentials callback parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new loading credentials callback parameter of the core configuration is set.
 */
void test_set_parameter_020()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_LOAD_CALLBACK, _dummy_credentials_load_callback);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(_dummy_credentials_load_callback, configuration->credentials_load_callback.rsa, "Value and loading credentials callback parameter have different addresses.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Saving credentials callback parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new saving credentials callback parameter of the core configuration is set.
 */
void test_set_parameter_021()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_SAVE_CALLBACK, _dummy_credentials_save_callback);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(_dummy_credentials_save_callback, configuration->credentials_save_callback.rsa, "Value and saving credentials callback parameter have different addresses.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Entering critical section callback parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new entering critical section callback parameter of the core configuration is set.
 */
void test_set_parameter_022()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CRITICAL_SECTION_ENTER_CALLBACK, _dummy_critical_section_enter_callback);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(_dummy_critical_section_enter_callback, configuration->critical_section_enter_callback, "Value and entering critical section callback parameter have different addresses.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Leaving critical section callback parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new leaving critical section callback parameter of the core configuration is set.
 */
void test_set_parameter_023()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CRITICAL_SECTION_LEAVE_CALLBACK, _dummy_critical_section_leave_callback);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(_dummy_critical_section_leave_callback, configuration->critical_section_leave_callback, "Value and leaving critical section callback parameter have different addresses.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Mindsphere certificate file parameter.
 * WHEN  : mcl_core_configuration_set_parameter() is called.
 * THEN  : MCL_OK is returned and new mindsphere certificate file parameter of the core configuration is set.
 */
void test_set_parameter_024()
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *certificate_file = "mindsphere_certificate.pem";

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Test.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE_FILE, certificate_file);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_core_configuration_set_parameter() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(certificate_file, configuration->mindsphere_certificate, "Mindsphere certificate parameter is wrong.");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRUE, configuration->certificate_is_file, "Certificate is given as file.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Mandatory core configuration parameters.
 * WHEN  : core_configuration_validate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_core_configuration_validate_001(void)
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "custom agent v1.0";
    const char *tenant = "br-smk1";
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Test.
    code = core_configuration_validate(configuration);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "core_configuration_validate() failed.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Mindsphere hostname parameter is not set.
 * WHEN  : core_configuration_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_core_configuration_validate_002(void)
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *user_agent = "custom agent v1.0";
    const char *tenant = "br-smk1";
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Test.
    code = core_configuration_validate(configuration);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "core_configuration_validate() failed.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Mindsphere hostname parameter is longer than MAXIMUM_HOST_NAME_LENGTH.
 * WHEN  : core_configuration_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_core_configuration_validate_003(void)
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *user_agent = "custom agent v1.0";
    const char *tenant = "br-smk1";
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;
    char mindsphere_hostname[MAXIMUM_HOST_NAME_LENGTH + 2];
    string_util_memset(mindsphere_hostname, 'a', sizeof(mindsphere_hostname));
    mindsphere_hostname[sizeof(mindsphere_hostname) - 1] = MCL_NULL_CHAR;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Test.
    code = core_configuration_validate(configuration);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "core_configuration_validate() failed.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Proxy hostname parameter is longer than MAXIMUM_HOST_NAME_LENGTH.
 * WHEN  : core_configuration_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_core_configuration_validate_004(void)
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "custom agent v1.0";
    const char *tenant = "br-smk1";
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;
    char proxy_hostname[MAXIMUM_HOST_NAME_LENGTH + 2];
    string_util_memset(proxy_hostname, 'a', sizeof(proxy_hostname));
    proxy_hostname[sizeof(proxy_hostname) - 1] = MCL_NULL_CHAR;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Test.
    code = core_configuration_validate(configuration);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "core_configuration_validate() failed.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Proxy hostname is set and proxy username parameter is longer than MAXIMUM_PROXY_USER_NAME_LENGTH.
 * WHEN  : core_configuration_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_core_configuration_validate_005(void)
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "custom agent v1.0";
    const char *tenant = "br-smk1";
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *proxy_username = "test_long_proxy_username123456789";

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy username parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_USER, proxy_username);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy username parameter.");

    // Test.
    code = core_configuration_validate(configuration);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "core_configuration_validate() failed.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Proxy username is set but proxy password parameter is not set.
 * WHEN  : core_configuration_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_core_configuration_validate_006(void)
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "custom agent v1.0";
    const char *tenant = "br-smk1";
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *proxy_username = "proxy_username";

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy username parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_USER, proxy_username);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy username parameter.");

    // Test.
    code = core_configuration_validate(configuration);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "core_configuration_validate() failed.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Proxy password parameter is longer than MAXIMUM_PROXY_PASSWORD_LENGTH.
 * WHEN  : core_configuration_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_core_configuration_validate_007(void)
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "custom agent v1.0";
    const char *tenant = "br-smk1";
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *proxy_username = "proxy_username";
    const char *proxy_password = "test_long_proxy_password123456789";

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy username parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_USER, proxy_username);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy username parameter.");

    // Set proxy password parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PASS, proxy_password);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy password parameter.");

    // Test.
    code = core_configuration_validate(configuration);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "core_configuration_validate() failed.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Proxy doamin parameter is longer than MAXIMUM_PROXY_DOMAIN_LENGTH.
 * WHEN  : core_configuration_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_core_configuration_validate_008(void)
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "custom agent v1.0";
    const char *tenant = "br-smk1";
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *proxy_username = "proxy_username";
    const char *proxy_password = "proxy_password";

    char proxy_domain[MAXIMUM_PROXY_DOMAIN_LENGTH + 2];
    string_util_memset(proxy_domain, 'a', sizeof(proxy_domain));
    proxy_domain[sizeof(proxy_domain) - 1] = MCL_NULL_CHAR;
    
    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy username parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_USER, proxy_username);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy username parameter.");

    // Set proxy password parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PASS, proxy_password);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy password parameter.");

    // Set proxy domain parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_DOMAIN, proxy_domain);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy domain parameter.");

    // Test.
    code = core_configuration_validate(configuration);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "core_configuration_validate() failed.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : User agent parameter is not set.
 * WHEN  : core_configuration_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_core_configuration_validate_009(void)
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *mindsphere_hostname = "https://localhost";
    const char *tenant = "br-smk1";
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_SHARED_SECRET;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Test.
    code = core_configuration_validate(configuration);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "core_configuration_validate() failed.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : User agent parameter is longer than MAXIMUM_USER_AGENT_LENGTH.
 * WHEN  : core_configuration_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_core_configuration_validate_010(void)
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *mindsphere_hostname = "https://localhost";
    char user_agent[502];
    string_util_memset(user_agent, 'a', sizeof(user_agent));
    user_agent[sizeof(user_agent) - 1] = MCL_NULL_CHAR;
    const char *tenant = "br-smk1";
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Test.
    code = core_configuration_validate(configuration);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "core_configuration_validate() failed.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Tenant parameter is not set.
 * WHEN  : core_configuration_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_core_configuration_validate_011(void)
{
    // Variable declaration.
    mcl_core_configuration_t *configuration = NULL;
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "user agent";
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for core configuration.");

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Test.
    code = core_configuration_validate(configuration);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "core_configuration_validate() failed.");

    // Clean up.
    mcl_core_configuration_destroy(&configuration);
}

/**
 * GIVEN : Initialized configuration object.
 * WHEN  : User requests to destroy configuration object.
 * THEN  : Configuration object is null.
 */
void test_destroy_001(void)
{
    // Initialize configuration.
    mcl_core_configuration_t *configuration = MCL_NULL;
    mcl_error_t code = mcl_core_configuration_initialize(&configuration);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_core_configuration_initialize() failed.");
    TEST_ASSERT_NOT_NULL_MESSAGE(configuration, "Configuration is null.");

    // Test.
    mcl_core_configuration_destroy(&configuration);
    TEST_ASSERT_MESSAGE(MCL_NULL == configuration, "Configuration is not NULL after destroy.");
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_core_configuration_destroy() failed.");
}

static void _dummy_credentials_load_callback()
{
}

static void _dummy_credentials_save_callback()
{
}

static void _dummy_critical_section_enter_callback()
{
}

static void _dummy_critical_section_leave_callback()
{
}
