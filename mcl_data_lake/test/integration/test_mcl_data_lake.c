/*!
 * @file     test_mcl_data_lake.c
 * @brief    Integration tests for data lake module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_data_lake/mcl_data_lake.h"
#include "mcl_core/mcl_core.h"
#include <string.h>

//J-
char *mock_server_certificate =
    "-----BEGIN CERTIFICATE-----\n"\
    "MIIDqDCCApCgAwIBAgIEo9uk7jANBgkqhkiG9w0BAQsFADBiMRswGQYDVQQDDBJ3\n"\
    "d3cubW9ja3NlcnZlci5jb20xEzARBgNVBAoMCk1vY2tTZXJ2ZXIxDzANBgNVBAcM\n"\
    "BkxvbmRvbjEQMA4GA1UECAwHRW5nbGFuZDELMAkGA1UEBhMCVUswIBcNMTUwMjEz\n"\
    "MTYzMDA2WhgPMjExNjAxMjAxNjMwMDZaMGIxGzAZBgNVBAMMEnd3dy5tb2Nrc2Vy\n"\
    "dmVyLmNvbTETMBEGA1UECgwKTW9ja1NlcnZlcjEPMA0GA1UEBwwGTG9uZG9uMRAw\n"\
    "DgYDVQQIDAdFbmdsYW5kMQswCQYDVQQGEwJVSzCCASIwDQYJKoZIhvcNAQEBBQAD\n"\
    "ggEPADCCAQoCggEBAIslQgdbmc1UDLjtCsFVeFEPHZWWQsIJkcLCT15QJbLnMPEm\n"\
    "pyiSo07DQ1xQk7WCBOWWBuhJt+MwEAK5QFR/MLf11xcUF26PkO0GO01lEPtS6M8n\n"\
    "pXpNCYKFF+gZ0q1hmxZ08ZQD+6LAPnLx5XUKk/XvjEpKRz01Sza4nUhgc4QDnN+B\n"\
    "CiamQ4fzhbwR6eTPm9Z266RYhK+oPBBN1AxE/+CwO0nV85n9EYimn5kfZqyideCQ\n"\
    "3ZbjPzXgfXctC9cWxKK4eEdfjMRnxxwAVVPCJU59sMv6JjhDV3YKWX4UMevsc6bf\n"\
    "Q/KOgVAoFJuFXbAiS+oE5/JOW0yU9fHngD7QWUMCAwEAAaNkMGIwHQYDVR0OBBYE\n"\
    "FKmWp1h2Zhsn4FP97H977H4izXGkMA8GA1UdEwEB/wQFMAMBAf8wCwYDVR0PBAQD\n"\
    "AgG2MCMGA1UdJQQcMBoGCCsGAQUFBwMBBggrBgEFBQcDAgYEVR0lADANBgkqhkiG\n"\
    "9w0BAQsFAAOCAQEAFW+ARa1RmSjbX82aj5+4hAVb3hiVcEVfDD/yfSwEYP93yNgV\n"\
    "u74z/a3eqrzQKWveKH+CTGaRij1WNAknswrBijSEppNKEebg/J/vpXNwaDNK49oi\n"\
    "8zbNAWPOB/mRK8ogY0qWXUHzt69r6Cm6Jid1TLKlpuJ60REZT8kfQlbXaNjy2g0N\n"\
    "W254k/MBugmBQcKEJ/nfiiZe/1zLkn2JLQAgFf5LIvhGNBS/t7Fz0QiKwyfhyjP3\n"\
    "t0YCdqheMQM8/rFjufCgJDu7IfieKguRoyRRbfSb6/gApANG3ARlLu6SoXK1TjfX\n"\
    "gJvTmKj67HcgyjQgsSTx7r1O/HgxF4+rhpOpxg==\n"\
    "-----END CERTIFICATE-----";
//J+

void setUp(void)
{
}

void tearDown(void)
{
}

// Initialize configuration.
static mcl_core_configuration_t *_initialize_configuration(const char *user_agent);

// This function creates a log file to use with callback.
static mcl_error_t _create_sensor_data(const char *file_content, const char *file_name);

// This callback function gets sensor data.
static mcl_size_t _get_sensor_data(void *buffer, mcl_size_t size, mcl_size_t count, void *file_descriptor);

/**
 * GIVEN : Data lake object.
 * WHEN  : mcl_data_lake_generate_upload_url() is called.
 * THEN  : MCL_OK is returned.
 */
void test_generate_upload_url_001(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_t *data_lake = MCL_NULL;
    mcl_data_lake_configuration_t *configuration = MCL_NULL;
    mcl_data_lake_object_t *object;
    const char *path = "sensor_data/door_sensor.log";
    mcl_core_t *core = NULL;
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
                             "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
                             "SERVICE=DATA_LAKE_GENERATE_UPLOAD_URLS;HTTP_STATUS=201;%";
    mcl_core_configuration_t *core_configuration = _initialize_configuration(user_agent);

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(core_configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize data lake configuration.
    code = mcl_data_lake_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for configuration.");

    // Set core parameter of data lake configuration.
    code = mcl_data_lake_configuration_set_parameter(configuration, MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_configuration_set_parameter() failed.");

    // Initialize data lake.
    code = mcl_data_lake_initialize(configuration, &data_lake);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake.");

    // Initialize data lake object.
    code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake object.");

    // Set path parameter of data lake object.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_PATH, path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");

    // Test.
    code = mcl_data_lake_generate_upload_url(data_lake, object);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should have been returned.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&core_configuration);
    mcl_data_lake_object_destroy(&object);
    mcl_data_lake_destroy(&data_lake);
    mcl_data_lake_configuration_destroy(&configuration);
}

/**
 * GIVEN : Data lake object.
 * WHEN  : mcl_data_lake_generate_upload_url() is called.
 * THEN  : MCL_BAD_REQUEST is returned.
 */
void test_generate_upload_url_002(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_t *data_lake = MCL_NULL;
    mcl_data_lake_configuration_t *configuration = MCL_NULL;
    mcl_data_lake_object_t *object;
    const char *path = "sensor_data/door_sensor.log";
    mcl_core_t *core = NULL;
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
                             "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
                             "SERVICE=DATA_LAKE_GENERATE_UPLOAD_URLS;HTTP_STATUS=400;%";
    mcl_core_configuration_t *core_configuration = _initialize_configuration(user_agent);

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(core_configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize data lake configuration.
    code = mcl_data_lake_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for configuration.");

    // Set core parameter of data lake configuration.
    code = mcl_data_lake_configuration_set_parameter(configuration, MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_configuration_set_parameter() failed.");

    // Initialize data lake.
    code = mcl_data_lake_initialize(configuration, &data_lake);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake.");

    // Initialize data lake object.
    code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake object.");

    // Set path parameter of data lake object.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_PATH, path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");

    // Test.
    code = mcl_data_lake_generate_upload_url(data_lake, object);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_BAD_REQUEST, code, "MCL_BAD_REQUEST should have been returned.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&core_configuration);
    mcl_data_lake_object_destroy(&object);
    mcl_data_lake_destroy(&data_lake);
    mcl_data_lake_configuration_destroy(&configuration);
}

/**
 * GIVEN : Multiple data lake objects in a list.
 * WHEN  : mcl_data_lake_generate_upload_urls() is called.
 * THEN  : MCL_OK is returned.
 */
void test_generate_upload_urls_001(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_t *data_lake = MCL_NULL;
    mcl_data_lake_configuration_t *configuration = MCL_NULL;
    mcl_list_t *object_list;
    mcl_size_t index;
    const char *paths[] = {"sensor_data/gyroscope.log", "sensor_data/accelerometer.log", "sensor_data/temperature.log"};
    mcl_core_t *core = NULL;
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=DATA_LAKE_GENERATE_UPLOAD_URLS;HTTP_STATUS=201;%";
    mcl_core_configuration_t *core_configuration = _initialize_configuration(user_agent);

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(core_configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize data lake configuration.
    code = mcl_data_lake_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for configuration.");

    // Set core parameter of data lake configuration.
    code = mcl_data_lake_configuration_set_parameter(configuration, MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_configuration_set_parameter() failed.");

    // Initialize data lake.
    code = mcl_data_lake_initialize(configuration, &data_lake);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake.");

    // Initialize object list.
    code = mcl_list_initialize(&object_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for object list.");

    for (index = 0; index < sizeof(paths) / sizeof(const char *); index++)
    {
        mcl_data_lake_object_t *object;

        // Initialize data lake object.
        code = mcl_data_lake_object_initialize(&object);
        TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake object.");

        // Set path parameter of data lake object.
        code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_PATH, paths[index]);
        TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");

        code = mcl_list_add(object_list, object);
        TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_list_add() failed.");
    }

    // Test.
    code = mcl_data_lake_generate_upload_urls(data_lake, object_list);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should have been returned.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&core_configuration);
    mcl_list_destroy_with_content(&object_list, (mcl_list_item_destroy_callback) mcl_data_lake_object_destroy);
    mcl_data_lake_destroy(&data_lake);
    mcl_data_lake_configuration_destroy(&configuration);
}

/**
 * GIVEN : Initialized object with valid signed url.
 * WHEN  : mcl_data_lake_upload() is called.
 * THEN  : MCL_OK is returned.
 */
void test_upload_001(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_t *data_lake = MCL_NULL;
    mcl_data_lake_configuration_t *configuration = MCL_NULL;
    mcl_data_lake_object_t *object;
    const char *path = "sensor_data/door_sensor.log";
    mcl_core_t *core = NULL;
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
                             "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
                             "SERVICE=DATA_LAKE_GENERATE_UPLOAD_URLS;HTTP_STATUS=201;%"\
                             "SERVICE=DATA_LAKE_UPLOAD;HTTP_STATUS=200;%";
    mcl_core_configuration_t *core_configuration = _initialize_configuration(user_agent);

    // Create a a log file to store sensor data for callback.
    char *file_name = "door_sensor.log";
    char *file_content = "Door is open - 21:30 pm";
    _create_sensor_data(file_content, file_name);
    FILE *file_descriptor = fopen(file_name, "r");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(core_configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize data lake configuration.
    code = mcl_data_lake_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for configuration.");

    // Set core parameter of data lake configuration.
    code = mcl_data_lake_configuration_set_parameter(configuration, MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_configuration_set_parameter() failed.");

    // Initialize data lake.
    code = mcl_data_lake_initialize(configuration, &data_lake);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake.");

    // Initialize data lake object.
    code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake object.");

    // Set path parameter of data lake object.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_PATH, path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");

    // Set upload callback parameter of data lake object.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_UPLOAD_CALLBACK, _get_sensor_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");

    // Set payload size.
    mcl_size_t object_size = strlen(file_content);
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_SIZE, &object_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");

    // Set user context parameter of data lake object.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_USER_CONTEXT, file_descriptor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");

    // Generate signed URL.
    code = mcl_data_lake_generate_upload_url(data_lake, object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_generate_upload_url() failed.");

    // Test.
    code = mcl_data_lake_upload(data_lake, object);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should have been returned.");

    // Clean up.
    fclose(file_descriptor);
    remove(file_name);
    mcl_core_destroy(&core);
    mcl_data_lake_configuration_destroy(&configuration);
    mcl_data_lake_object_destroy(&object);
    mcl_data_lake_destroy(&data_lake);
    mcl_core_configuration_destroy(&core_configuration);
}

/**
 * GIVEN : Initialized object with expired signed url.
 * WHEN  : mcl_data_lake_upload() is called.
 * THEN  : MCL_BAD_REQUEST is returned.
 */
void test_upload_002(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_data_lake_t *data_lake = MCL_NULL;
    mcl_data_lake_configuration_t *configuration = MCL_NULL;
    mcl_data_lake_object_t *object;
    const char *path = "sensor_data/door_sensor.log";
    mcl_core_t *core = NULL;
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
                             "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
                             "SERVICE=DATA_LAKE_GENERATE_UPLOAD_URLS;HTTP_STATUS=201;%"\
                             "SERVICE=DATA_LAKE_UPLOAD;HTTP_STATUS=400;%";
    mcl_core_configuration_t *core_configuration = _initialize_configuration(user_agent);

    // Create a a log file to store sensor data for callback.
    char *file_name = "door_sensor.log";
    char *file_content = "Door is open - 21:30 pm";
    _create_sensor_data(file_content, file_name);
    FILE *file_descriptor = fopen(file_name, "r");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(core_configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize data lake configuration.
    code = mcl_data_lake_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for configuration.");

    // Set core parameter of data lake configuration.
    code = mcl_data_lake_configuration_set_parameter(configuration, MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_configuration_set_parameter() failed.");

    // Initialize data lake.
    code = mcl_data_lake_initialize(configuration, &data_lake);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake.");

    // Initialize data lake object.
    code = mcl_data_lake_object_initialize(&object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data lake object.");

    // Set path parameter of data lake object.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_PATH, path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");

    // Set upload callback parameter of data lake object.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_UPLOAD_CALLBACK, _get_sensor_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");

    // Set payload size.
    mcl_size_t object_size = strlen(file_content);
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_SIZE, &object_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");

    // Set user context parameter of data lake object.
    code = mcl_data_lake_object_set_parameter(object, MCL_DATA_LAKE_OBJECT_PARAMETER_USER_CONTEXT, file_descriptor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_object_set_parameter() failed.");

    // Generate signed URL.
    code = mcl_data_lake_generate_upload_url(data_lake, object);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_lake_generate_upload_url() failed.");

    // Test.
    code = mcl_data_lake_upload(data_lake, object);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_BAD_REQUEST, code, "MCL_BAD_REQUEST should have been returned.");

    // Clean up.
    fclose(file_descriptor);
    remove(file_name);
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&core_configuration);
    mcl_data_lake_object_destroy(&object);
    mcl_data_lake_destroy(&data_lake);
    mcl_data_lake_configuration_destroy(&configuration);
}

static mcl_core_configuration_t *_initialize_configuration(const char *user_agent)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_t *core_configuration = MCL_NULL;
    mcl_core_configuration_initialize(&core_configuration);

    // Set mindsphere hostname parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    return core_configuration;
}

static mcl_size_t _get_sensor_data(void *buffer, mcl_size_t size, mcl_size_t count, void *file_descriptor)
{
    MCL_DEBUG_ENTRY("void *buffer = <%p>, mcl_size_t size = <%u>, mcl_size_t count = <%u>, void *file_descriptor = <%p>", buffer, size, count, file_descriptor);

    // Read from file.
    mcl_size_t actual_size_read = fread(buffer, size, count, (FILE *)file_descriptor);

    MCL_DEBUG_LEAVE("retVal = <%u>", actual_size_read);
    return actual_size_read;
}

static mcl_error_t _create_sensor_data(const char *file_content, const char *file_name)
{
    MCL_DEBUG_ENTRY("const char *file_content = <%p>, const char *file_name = <%p>", file_content, file_name);

    // Open file.
    FILE *file_descriptor = fopen(file_name, "w");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(MCL_NULL, file_descriptor, "File descriptor is Null.");

    // Write to file.
    mcl_error_t result = fputs(file_content, file_descriptor);
    TEST_ASSERT_MESSAGE(0 <= result, "File descriptor is Null.");

    // Close file.
    result = fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(0 == result, "File can not be closed.");

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}
