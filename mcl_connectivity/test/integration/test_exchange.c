/*!
 * @file     test_exchange.c
 * @brief    Integration tests for exchange endpoint.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_connectivity/mcl_connectivity.h"
#include "mcl_core/mcl_file_util.h"
#include "mcl_core/mcl_random.h"
#include <stdlib.h>
#include <string.h>

mcl_core_configuration_t *configuration;
mcl_core_t *core = NULL;

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

char *fiddler_certificate =
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIDsjCCApqgAwIBAgIQLTASlotOm4RAdtLOR6e3wjANBgkqhkiG9w0BAQsFADBn\n" \
    "MSswKQYDVQQLDCJDcmVhdGVkIGJ5IGh0dHA6Ly93d3cuZmlkZGxlcjIuY29tMRUw\n" \
    "EwYDVQQKDAxET19OT1RfVFJVU1QxITAfBgNVBAMMGERPX05PVF9UUlVTVF9GaWRk\n" \
    "bGVyUm9vdDAeFw0xNzA1MjExMDA1MDFaFw0yMzA1MjExMDA1MDFaMGcxKzApBgNV\n" \
    "BAsMIkNyZWF0ZWQgYnkgaHR0cDovL3d3dy5maWRkbGVyMi5jb20xFTATBgNVBAoM\n" \
    "DERPX05PVF9UUlVTVDEhMB8GA1UEAwwYRE9fTk9UX1RSVVNUX0ZpZGRsZXJSb290\n" \
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA2A7TzKyamp04MI0dRVTu\n" \
    "qJoKrRxs0+Uakbn/S0ZTb2fb9KAaEHxyAwhK6dp9v98gsbG/pDgeMj2W/+Wev66N\n" \
    "QUcYBAA5v/pqbYcydJmrcQ49mE42FUPzQh3GM7e9AgbC8Eem1GMH7GOYmsFUywVg\n" \
    "jLOoXrDbFa5ZMdmfa5coUJxMM5S7poBwmd9tGd43PVOpHZTlbCv524wBZNyXQ9Kr\n" \
    "WsSz2fdayndnn5jpG413ZReMpqQgL472uv9FK+v3clGHaWhRWJoiMUMsKgoEHean\n" \
    "7guTLdLKLIpl7kd3pRRd7xiC/+DpeVyk0d+PckSHop3vyoJ27tyFDe3hvUjlSYTJ\n" \
    "KQIDAQABo1owWDATBgNVHSUEDDAKBggrBgEFBQcDATASBgNVHRMBAf8ECDAGAQH/\n" \
    "AgEAMB0GA1UdDgQWBBS9CSk2gJbHnbvTNurHcij4IfezIDAOBgNVHQ8BAf8EBAMC\n" \
    "AQYwDQYJKoZIhvcNAQELBQADggEBACx3yHz9G/dXL8CD3iLlo1BAADOtdu9U88/L\n" \
    "ZOoi3MAD5vVMceoZJHHnri4KYjth9xmSd4RXxM8TUvFE4tR9rr/hsKcIx5NsvM1U\n" \
    "B/7WWHQBqIstVEobF1fTNqIEl+RO4YzWnPuQ6eBRP+Z9TP9XiFxs01ZxVim3jo3p\n" \
    "0aLPYfvWtFnVpzaI5gkX7mt/rkX3qgm2Ea8seH5z07DChZlVabLzZdyVAvi9dLYr\n" \
    "+3qP7IqBn0Xq3T3VgmS30h5jq5MPXPb6hmfMtVZSwbiMeX52DWwzUHk5Z2ru4z2J\n" \
    "hebTf5rqG3czrtQRjUHaaYXtZO2eDzs+DRa4A6HC0MzZ4HqXTdw=\n" \
    "-----END CERTIFICATE-----";

char *improper_certificate =
    "-----BEGIN CERTIFICATE-----\n"\
    "InvalidCertificateIQcgl5JcHmAqZNIheZNjIxcDANBgkqhkiG9w0BAQsFADBn\n"\
    "MSswKQYDVQQLDCJDcmVhdGVkIGJ5IGh0dHA6Ly93d3cuZmlkZGxlcjIuY29tMRUw\n"\
    "EwYDVQQKDAxET19OT1RfVFJVU1QxITAfBgNVBAMMGERPX05PVF9UUlVTVF9GaWRk\n"\
    "bGVyUm9vdDAeFw0xNTEwMjQxNzExNDBaFw0yMTEwMjMxNzExNDBaMGcxKzApBgNV\n"\
    "BAsMIkNyZWF0ZWQgYnkgaHR0cDovL3d3dy5maWRkbGVyMi5jb20xFTATBgNVBAoM\n"\
    "DERPX05PVF9UUlVTVDEhMB8GA1UEAwwYRE9fTk9UX1RSVVNUX0ZpZGRsZXJSb290\n"\
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAtFdQpTZJ5r4sNmijBvJ5\n"\
    "ej3U0FusUNYhoqJyoB1nBivuKhet5VWgbULY+NohVMJp+K22/FtatwUwvToR36vh\n"\
    "ajpThc4ebHbdxmjisNJLIhMAEygt6C35bXFGhVHsbV8i7PWm3aSVjHUHUJq4gYkt\n"\
    "5Q9yi2RTjN+tYlZ+hHT98rBncdSwsGxr2jblaYKwFP4BGPBN08Wsoc4EpCtwgqKx\n"\
    "X1PoGyQQi8lXE1nVp8TczGKPeJ/0OpwPqg0D13iLux5xtKbDevmhuH3IOArrRG1H\n"\
    "iXQv+njVZBtg4ygj09MBXC6CbwzHwsklJu8DGxv45YZXWat4VG4PLSFUIMaafPAu\n"\
    "0QIDAQABo1owWDATBgNVHSUEDDAKBggrBgEFBQcDATASBgNVHRMBAf8ECDAGAQH/\n"\
    "AgEAMB0GA1UdDgQWBBTVSkiB4PW0HvNgDGQVT1B2oAz3NDAOBgNVHQ8BAf8EBAMC\n"\
    "AQYwDQYJKoZIhvcNAQELBQADggEBAJISTlO5o16MPJ6QOahXxYHo+spu3YB4rpJA\n"\
    "HwTf0ys/XnkOqWIvEkEViTkphYFtZ2fNH5pT3czYWPMJuh2SB3sEF2N27h9i7YP4\n"\
    "Rb0adptc+DkPDXAwC+CA+6whVHSaAasOXeIWq4CF1zPmPF9yTrGQZNldnT4P4PGP\n"\
    "d+Wt67LJLWJfwAW5oVTZlwS/JKZTclKhDYJTUek4srk1J5f/yT681HsUvHp08AvJ\n"\
    "AfAjUPm2CxFAwGwuFENo7QxE1m3ymBYTZVkbaS3XuGxYSjExdVatGekivj9Rvu4I\n"\
    "aWv490DonBQ63t+Sa9VoG9VDV43XU1xuH3AGTts+GMD17Gf/gwg=\n"\
    "-----END CERTIFICATE-----";
//J+

void setUp(void)
{
}

void tearDown(void)
{
}

// Creates custom data as json.
static mcl_json_t *_create_custom_data(const char *name, const char *value);

// Creates data point.
static mcl_error_t _create_data_point(const char *name, const char *description, const char *type, const char *unit, mcl_json_t *custom_data, mcl_data_point_t **data_point);

// Creates data source.
static mcl_error_t _create_data_source(const char *name, const char *description, mcl_json_t *custom_data, mcl_data_source_t **data_source);

// Creates data source configuration.
static mcl_error_t _create_data_source_configuration(mcl_data_source_configuration_t **data_source_configuration);

// Creates event.
static mcl_error_t _create_event(mcl_event_t **event);

// Creates timeseries value.
static mcl_error_t _create_time_series_value(const char *data_point_id, const char *value, const char *quality_code, mcl_timeseries_value_t **timeseries_value);

// Creates file.
static mcl_error_t _create_file(const char *file_content, const char *file_name);

/**
* GIVEN : Onboarded agent and single event to upload.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_OK is returned and event is uploaded as single item.
*/
void test_exchange_01(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Create event.
    mcl_event_t *event = NULL;
    code = _create_event(&event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Event couldn't be created.");

    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange event.
    code = mcl_connectivity_exchange(connectivity, event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_event_destroy(&event);
}

/**
* GIVEN : Onboarded agent and data source configuration with both mandatory and optional fields.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_OK is returned and data source configuration is uploaded as single item.
*/
void test_exchange_02(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Create data source configuration.
    mcl_data_source_configuration_t *data_source_configuration = NULL;
    code = _create_data_source_configuration(&data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source configuration.");

    // Create first data source. First loop.
    mcl_data_source_t *data_source_1 = NULL;
    const char *data_source_name_1 = "Main motor";
    const char *data_source_description_1 = "Test Rack";
    mcl_json_t *data_source_custom_data_1 = _create_custom_data("motor efficiency", "good");
    code = _create_data_source(data_source_name_1, data_source_description_1, data_source_custom_data_1, &data_source_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for first data source.");

    // Create first data point.
    mcl_data_point_t *data_point_1 = NULL;
    const char *data_point_name_1 = "Speed";
    const char *data_point_description_1 = "Test Rack";
    const char *data_point_type_1 = "uint";
    const char *data_point_unit_1 = "rpm";
    mcl_json_t *data_point_custom_data_1 = _create_custom_data("motor status", "need maintenance");
    code = _create_data_point(data_point_name_1, data_point_description_1, data_point_type_1, data_point_unit_1, data_point_custom_data_1, &data_point_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for first data point.");

    // Add first data point to data source.
    code = mcl_data_source_add_data_point(data_source_1, data_point_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding first data point to first data source failed.");

    // Create second data point.
    mcl_data_point_t *data_point_2 = NULL;
    const char *data_point_name_2 = "Temperature";
    const char *data_point_description_2 = "Test Fan";
    const char *data_point_type_2 = "uint";
    const char *data_point_unit_2 = "Celcius";
    mcl_json_t *data_point_custom_data_2 = _create_custom_data("motor temperature", "normal");
    code = _create_data_point(data_point_name_2, data_point_description_2, data_point_type_2, data_point_unit_2, data_point_custom_data_2, &data_point_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for second data point.");

    // Add second data point to first data source.
    code = mcl_data_source_add_data_point(data_source_1, data_point_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding second data point to first data source failed.");

    // Add first data source to data source configuration.
    code = mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding first data source to data source configuration failed.");

    // Create second data source. Second loop.
    mcl_data_source_t *data_source_2 = NULL;
    const char *data_source_name_2 = "Second motor";
    const char *data_source_description_2 = "Test Rack";
    mcl_json_t *data_source_custom_data_2 = _create_custom_data("motor efficiency", "good");
    code = _create_data_source(data_source_name_2, data_source_description_2, data_source_custom_data_2, &data_source_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for second data source");

    // Create third data point.
    mcl_data_point_t *data_point_3 = NULL;
    const char *data_point_name_3 = "Voltage";
    const char *data_point_description_3 = "Test Rack";
    const char *data_point_type_3 = "uint";
    const char *data_point_unit_3 = "Volt";
    mcl_json_t *data_point_custom_data_3 = _create_custom_data("motor voltage", "normal value");
    code = _create_data_point(data_point_name_3, data_point_description_3, data_point_type_3, data_point_unit_3, data_point_custom_data_3, &data_point_3);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for third data point.");

    // Add third data point to second data source.
    code = mcl_data_source_add_data_point(data_source_2, data_point_3);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding third data point to second data source failed.");

    // Create fourth data point.
    mcl_data_point_t *data_point_4 = NULL;
    const char *data_point_name_4 = "Ampere";
    const char *data_point_description_4 = "Test Fan";
    const char *data_point_type_4 = "uint";
    const char *data_point_unit_4 = "Amp";
    mcl_json_t *data_point_custom_data_4 = _create_custom_data("motor current", "low");
    code = _create_data_point(data_point_name_4, data_point_description_4, data_point_type_4, data_point_unit_4, data_point_custom_data_4, &data_point_4);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for fourth data point.");

    // Add fourth data point to second data source.
    code = mcl_data_source_add_data_point(data_source_2, data_point_4);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding fourth data point to second data source failed.");

    // Add second data source to data source configuration.
    code = mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding second data_source to data source configuration failed.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange data source configuration.
    code = mcl_connectivity_exchange(connectivity, data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_data_source_configuration_destroy(&data_source_configuration);
    mcl_json_util_destroy(&data_source_custom_data_1);
    mcl_json_util_destroy(&data_source_custom_data_2);
    mcl_json_util_destroy(&data_point_custom_data_1);
    mcl_json_util_destroy(&data_point_custom_data_2);
    mcl_json_util_destroy(&data_point_custom_data_3);
    mcl_json_util_destroy(&data_point_custom_data_4);
}

/**
* GIVEN : Onboarded agent and data source configuration with only mandatory fields.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_OK is returned and data source configuration is uploaded as single item.
*/
void test_exchange_03(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Create data source configuration.
    mcl_data_source_configuration_t *data_source_configuration = NULL;
    code = _create_data_source_configuration(&data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source configuration.");

    // Create data source.
    mcl_data_source_t *data_source = NULL;
    const char *data_source_name = "Main motor";
    const char *data_source_description = NULL;
    mcl_json_t *data_source_custom_data = NULL;
    code = _create_data_source(data_source_name, data_source_description, data_source_custom_data, &data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source.");

    // Create data point.
    mcl_data_point_t *data_point = NULL;
    const char *data_point_name = "Speed";
    const char *data_point_description = NULL;
    const char *data_point_type = "uint";
    const char *data_point_unit = "rpm";
    mcl_json_t *data_point_custom_data = NULL;
    code = _create_data_point(data_point_name, data_point_description, data_point_type, data_point_unit, data_point_custom_data, &data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data point.");

    // Add data point to data source.
    code = mcl_data_source_add_data_point(data_source, data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding data point to data source failed.");

    // Add data source to data source configuration.
    code = mcl_data_source_configuration_add_data_source(data_source_configuration, data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding data source to data source configuration failed.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange data source configuration.
    code = mcl_connectivity_exchange(connectivity,  data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_data_source_configuration_destroy(&data_source_configuration);
}

/**
* GIVEN : Onboarded agent and data source configuration without mandatory fields.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_INVALID_PARAMETER is returned.
*/
void test_exchange_04(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Create data source configuration.
    mcl_data_source_configuration_t *data_source_configuration = NULL;
    code = _create_data_source_configuration(&data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source configuration.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange data source configuration.
    code = mcl_connectivity_exchange(connectivity, data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_data_source_configuration_destroy(&data_source_configuration);
}

/**
* GIVEN : Onboarded agent and timeseries with all mandatory fields.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_OK is returned and timeseries is uploaded as single item.
*/
void test_exchange_05(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Data point ids comes from data source configuration upload.
    const char *data_point_id_1 = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
    const char *data_point_id_2 = "a90ab7ca-fd5d-11e5-8000-001b1bc14a7b";

    // Timestamps of datapoints.
    const char *timestamp_1 = "2016-04-26T08:06:25.317Z";
    const char *timestamp_2 = "2016-04-26T08:06:27.317Z";

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Create timeseries.
    mcl_timeseries_t *timeseries;
    code = mcl_timeseries_initialize(MCL_TIMESERIES_VERSION_1_0, &timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries.");

    // Set parameter configuration id.
    code = mcl_timeseries_set_parameter(timeseries, MCL_TIMESERIES_PARAMETER_CONFIGURATION_ID, "e3217e2b-7036-49f2-9814-4c38542cd781");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_set_parameter() failed.");

    // Initialize first timeseries value list.
    mcl_timeseries_value_list_t *timeseries_value_list_1;
    code = mcl_timeseries_value_list_initialize(&timeseries_value_list_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for first timeseries value list.");

    // Set parameter timestamp.
    code = mcl_timeseries_value_list_set_parameter(timeseries_value_list_1, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, timestamp_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_list_set_parameter() failed.");

    // Create first timeseries value.
    mcl_timeseries_value_t *timeseries_value_1;
    const char *value_1 = "96";
    const char *quality_code_1 = "00A60000";
    code = _create_time_series_value(data_point_id_1, value_1, quality_code_1, &timeseries_value_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for first timeseries value.");

    // Add first timeseries value to first timeseries value list.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list_1, timeseries_value_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding first timeseries value to first timeseries value list failed.");

    // Create second timeseries value.
    mcl_timeseries_value_t *timeseries_value_2;
    const char *value_2 = "48";
    const char *quality_code_2 = "00B60006";
    code = _create_time_series_value(data_point_id_2, value_2, quality_code_2, &timeseries_value_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for second timeseries value.");

    // Add second timeseries value to first timeseries value list.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list_1, timeseries_value_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding first timeseries value to first timeseries value list failed.");

    // Add first timeseries value list to timeseries.
    code = mcl_timeseries_add_value_list(timeseries, timeseries_value_list_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding first timeseries value list to timeseries failed.");

    // Initialize second timeseries value list. Second loop.
    mcl_timeseries_value_list_t *timeseries_value_list_2;
    code = mcl_timeseries_value_list_initialize(&timeseries_value_list_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for second timeseries value list.");

    // Set parameter timestamp.
    code = mcl_timeseries_value_list_set_parameter(timeseries_value_list_2, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, timestamp_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_list_set_parameter() failed.");

    // Create third timeseries value.
    mcl_timeseries_value_t *timeseries_value_3;
    const char *value_3 = "96";
    const char *quality_code_3 = "00A60000";
    code = _create_time_series_value(data_point_id_1, value_3, quality_code_3, &timeseries_value_3);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for third timeseries value.");

    // Add third timeseries value to second timeseries value list.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list_2, timeseries_value_3);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding third timeseries value to second timeseries value list failed.");

    // Create fourth timeseries value.
    mcl_timeseries_value_t *timeseries_value_4;
    const char *value_4 = "24";
    const char *quality_code_4 = "00D40006";
    code = _create_time_series_value(data_point_id_2, value_4, quality_code_4, &timeseries_value_4);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for fourth timeseries value.");

    // Add fourth timeseries value to second timeseries value list.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list_2, timeseries_value_4);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding fourth timeseries value to second timeseries value list failed.");

    // Add second timeseries value list to timeseries.
    code = mcl_timeseries_add_value_list(timeseries, timeseries_value_list_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding second timeseries value list to timeseries failed.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange timeseries.
    code = mcl_connectivity_exchange(connectivity, timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_timeseries_destroy(&timeseries);
}

/**
* GIVEN : Onboarded agent and timeseries without a mandatory field which is timestamp.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_INVALID_PARAMETER is returned.
*/
void test_exchange_06(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Create timeseries.
    mcl_timeseries_t *timeseries;
    code = mcl_timeseries_initialize(MCL_TIMESERIES_VERSION_1_0, &timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries.");

    // Set parameter configuration id.
    code = mcl_timeseries_set_parameter(timeseries, MCL_TIMESERIES_PARAMETER_CONFIGURATION_ID, "e3217e2b-7036-49f2-9814-4c38542cd781");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_set_parameter() failed.");

    // Initialize timeseries value list.
    mcl_timeseries_value_list_t *timeseries_value_list;
    code = mcl_timeseries_value_list_initialize(&timeseries_value_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value list.");

    // Create a timeseries value.
    mcl_timeseries_value_t *timeseries_value;
    const char *data_point_id = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
    const char *value = "96";
    const char *quality_code = "00A60000";
    code = _create_time_series_value(data_point_id, value, quality_code, &timeseries_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value.");

    // Add timeseries value to timeseries value list.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list, timeseries_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding timeseries value to timeseries value list failed.");

    // Add timeseries value list to timeseries.
    code = mcl_timeseries_add_value_list(timeseries, timeseries_value_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding timeseries value list to timeseries failed.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange timeseries.
    code = mcl_connectivity_exchange(connectivity, timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_timeseries_destroy(&timeseries);
}

/**
* GIVEN : Onboarded agent and file with both mandatory and optional fields.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_OK is returned and file is uploaded as single item.
*/
void test_exchange_07(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Create a file containing data to be uploaded.
    const char *file_data = "0123456789";
    const char *remote_file_name = "data_file.txt";
    code = _create_file(file_data, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "File could not be created.");

    // Initialize file.
    mcl_file_t *file = MCL_NULL;
    code = mcl_file_initialize(MCL_FILE_VERSION_1_0, &file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for file.");

    // Set file type.
    const char *file_type = "text";
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_TYPE, file_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file type.");

    // Set file remote name.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_REMOTE_NAME, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file name.");

    // Set file local path.
    const char *file_local_path = "data_file.txt";
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_LOCAL_PATH, file_local_path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file local path.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange file.
    code = mcl_connectivity_exchange(connectivity, file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_file_destroy(&file);
    remove(remote_file_name);
}

/**
* GIVEN : Onboarded agent and file with only mandatory fields.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_OK is returned and file is uploaded as single item.
*/
void test_exchange_08(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Create a file containing data to be uploaded.
    const char *file_data = "0123456789";
    const char *remote_file_name = "data_file.txt";
    code = _create_file(file_data, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "File could not be created.");

    // Initialize file.
    mcl_file_t *file = MCL_NULL;
    code = mcl_file_initialize(MCL_FILE_VERSION_1_0, &file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for file.");

    // Set file remote name.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_REMOTE_NAME, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file name.");

    // Set file local path.
    const char *file_local_path = "data_file.txt";
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_LOCAL_PATH, file_local_path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file local path.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange file.
    code = mcl_connectivity_exchange(connectivity, file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_file_destroy(&file);
    remove(remote_file_name);
}

/**
* GIVEN : Onboarded agent and file without a mandatory field which is remote file name.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_INVALID_PARAMETER is returned.
*/
void test_exchange_09(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Create a file containing data to be uploaded.
    const char *file_data = "0123456789";
    const char *remote_file_name = "data_file.txt";
    code = _create_file(file_data, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "File could not be created.");

    // Initialize file.
    mcl_file_t *file = MCL_NULL;
    code = mcl_file_initialize(MCL_FILE_VERSION_1_0, &file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for file.");

    // Set file type.
    const char *file_type = "text";
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_TYPE, file_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file type.");

    // Set file local path.
    const char *file_local_path = "data_file.txt";
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_LOCAL_PATH, file_local_path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file local path.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange file.
    code = mcl_connectivity_exchange(connectivity, file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "MCL_INVALID_PARAMETER should have been returned.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_file_destroy(&file);
    remove(remote_file_name);
}

/**
* GIVEN : Onboarded agent and file without a mandatory field which is local path.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_INVALID_PARAMETER is returned.
*/
void test_exchange_10(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Create a file containing data to be uploaded.
    const char *file_data = "0123456789";
    const char *remote_file_name = "data_file.txt";
    code = _create_file(file_data, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "File could not be created.");

    // Initialize file.
    mcl_file_t *file = MCL_NULL;
    code = mcl_file_initialize(MCL_FILE_VERSION_1_0, &file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for file.");

    // Set file type.
    const char *file_type = "text";
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_TYPE, file_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file type.");

    // Set file remote name.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_REMOTE_NAME, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file name.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange file.
    code = mcl_connectivity_exchange(connectivity, file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "MCL_INVALID_PARAMETER should have been returned.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_file_destroy(&file);
    remove(remote_file_name);
}

/**
* GIVEN : Onboarded agent and file which is larger than maximum http payload size.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE is returned.
*/
void test_exchange_11(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    mcl_size_t max_http_payload_size = 10 * 1024 * 1024;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Create a file containing data to be uploaded.
    char *too_large_buffer = calloc(MCL_MAXIMUM_HTTP_PAYLOAD_SIZE + 1, 1);
    memset(too_large_buffer, 'B', MCL_MAXIMUM_HTTP_PAYLOAD_SIZE);

    const char *remote_file_name = "data_file.txt";
    code = _create_file(too_large_buffer, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "File could not be created.");

    // Initialize file.
    mcl_file_t *file = MCL_NULL;
    code = mcl_file_initialize(MCL_FILE_VERSION_1_0, &file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for file.");

    // Set file type.
    const char *file_type = "text";
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_TYPE, file_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file type.");

    // Set file remote name.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_REMOTE_NAME, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file name.");

    // Set file local path.
    const char *file_local_path = "data_file.txt";
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_LOCAL_PATH, file_local_path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file local path.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange file.
    code = mcl_connectivity_exchange(connectivity, file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE, code, "MCL_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE should have been returned.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_file_destroy(&file);
    remove(remote_file_name);
    free(too_large_buffer);
}

/**
* GIVEN : Onboarded agent and custom data with both mandatory and optional fields.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_OK is returned and custom data is uploaded as single item.
*/
void test_exchange_12(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;
    mcl_custom_data_t *custom_data = MCL_NULL;
    const char *version = "version 3.0.1";
    const char *custom_data_payload_type = "anyTypeSpecifiedByAgent";
    const char *custom_data_payload_content_type = "custom_payload_content_type1";
    const char *custom_data_payload_buffer = "buffer-1";
    mcl_size_t buffer_size = 8;
    mcl_json_t *custom_data_payload_details = MCL_NULL;
    const char *custom_data_payload_details_name = "Motor-1";
    const char *custom_data_payload_details_value = "Out of service";

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize custom_data.
    code = mcl_custom_data_initialize(version, &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for custom_data.");

    // Set custom_data type parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_TYPE, custom_data_payload_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Setting payload type failed.");

    // Set custom_data content type parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_CONTENT_TYPE, custom_data_payload_content_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Setting content type failed.");

    // Set custom_data buffer parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_BUFFER, custom_data_payload_buffer);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Setting payload buffer failed.");

    // Set custom_data size parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_BUFFER_SIZE, &buffer_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Payload size parameter could not be set.");

    // Initialize custom_data details parameter.
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, &custom_data_payload_details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for json.");

    // Add string to custom_data details parameter.
    code = mcl_json_util_add_string(custom_data_payload_details, custom_data_payload_details_name, custom_data_payload_details_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "String could not be added to the json object.");

    // Set custom_data details parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_DETAILS, custom_data_payload_details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Setting details failed.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange custom data.
    code = mcl_connectivity_exchange(connectivity, custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_custom_data_destroy(&custom_data);
    mcl_json_util_destroy(&custom_data_payload_details);
}

/**
* GIVEN : Onboarded agent and custom data with only mandatory fields.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_OK is returned and custom data is uploaded as single item.
*/
void test_exchange_13(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;
    mcl_custom_data_t *custom_data = MCL_NULL;
    const char *version = "version 3.0.1";
    const char *custom_data_payload_type = "anyTypeSpecifiedByAgent";
    const char *custom_data_payload_content_type = "custom_payload_content_type1";
    const char *custom_data_payload_buffer = "buffer-1";
    mcl_size_t buffer_size = 8;
    const char *custom_data_payload_details_name = "Motor-1";
    const char *custom_data_payload_details_value = "Out of service";

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize custom_data.
    code = mcl_custom_data_initialize(version, &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for custom_data.");

    // Set custom_data type parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_TYPE, custom_data_payload_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Setting payload type failed.");

    // Set custom_data content type parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_CONTENT_TYPE, custom_data_payload_content_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Setting content type failed.");

    // Set custom_data buffer parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_BUFFER, custom_data_payload_buffer);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Setting payload buffer failed.");

    // Set custom_data size parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_BUFFER_SIZE, &buffer_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Payload size parameter could not be set.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange custom data.
    code = mcl_connectivity_exchange(connectivity, custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_custom_data_destroy(&custom_data);
}

/**
* GIVEN : Onboarded agent and custom data without a mandatory field which is content type.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_INVALID_PARAMETER is returned.
*/
void test_exchange_14(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;
    mcl_custom_data_t *custom_data = MCL_NULL;
    const char *version = "version 3.0.1";
    const char *custom_data_payload_type = "anyTypeSpecifiedByAgent";
    const char *custom_data_payload_buffer = "buffer-1";
    mcl_size_t buffer_size = 8;
    const char *custom_data_payload_details_name = "Motor-1";
    const char *custom_data_payload_details_value = "Out of service";

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize custom_data.
    code = mcl_custom_data_initialize(version, &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for custom_data.");

    // Set custom_data type parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_TYPE, custom_data_payload_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Setting payload type failed.");

    // Set custom_data buffer parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_BUFFER, custom_data_payload_buffer);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Setting payload buffer failed.");

    // Set custom_data size parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_BUFFER_SIZE, &buffer_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Payload size parameter could not be set.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange custom data.
    code = mcl_connectivity_exchange(connectivity, custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_custom_data_destroy(&custom_data);
}

/**
* GIVEN : Onboarded agent and store with event.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_OK is returned.
*/
void test_exchange_15(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Create event.
    mcl_event_t *event = NULL;
    code = _create_event(&event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Event couldn't be created.");

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(&store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for store.");

    // Add event to store.
    code = mcl_store_add(store, event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding event to store failed.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange store.
    code = mcl_connectivity_exchange(connectivity, store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_store_destroy(&store);
}

/**
* GIVEN : Onboarded agent and store with data source configuration.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_OK is returned.
*/
void test_exchange_16(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Create data source configuration.
    mcl_data_source_configuration_t *data_source_configuration = NULL;
    code = _create_data_source_configuration(&data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source configuration.");

    // Create first data source. First loop.
    mcl_data_source_t *data_source_1 = NULL;
    const char *data_source_name_1 = "Main motor";
    const char *data_source_description_1 = "Test Rack";
    mcl_json_t *data_source_custom_data_1 = _create_custom_data("motor efficiency", "good");
    code = _create_data_source(data_source_name_1, data_source_description_1, data_source_custom_data_1, &data_source_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for first data source.");

    // Create first data point.
    mcl_data_point_t *data_point_1 = NULL;
    const char *data_point_name_1 = "Speed";
    const char *data_point_description_1 = "Test Rack";
    const char *data_point_type_1 = "uint";
    const char *data_point_unit_1 = "rpm";
    mcl_json_t *data_point_custom_data_1 = _create_custom_data("motor status", "need maintenance");
    code = _create_data_point(data_point_name_1, data_point_description_1, data_point_type_1, data_point_unit_1, data_point_custom_data_1, &data_point_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for first data point.");

    // Add first data point to data source.
    code = mcl_data_source_add_data_point(data_source_1, data_point_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding first data point to first data source failed.");

    // Create second data point.
    mcl_data_point_t *data_point_2 = NULL;
    const char *data_point_name_2 = "Temperature";
    const char *data_point_description_2 = "Test Fan";
    const char *data_point_type_2 = "uint";
    const char *data_point_unit_2 = "Celcius";
    mcl_json_t *data_point_custom_data_2 = _create_custom_data("motor temperature", "normal");
    code = _create_data_point(data_point_name_2, data_point_description_2, data_point_type_2, data_point_unit_2, data_point_custom_data_2, &data_point_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for second data point.");

    // Add second data point to first data source.
    code = mcl_data_source_add_data_point(data_source_1, data_point_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding second data point to first data source failed.");

    // Add first data source to data source configuration.
    code = mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding first data source to data source configuration failed.");

    // Create second data source. Second loop.
    mcl_data_source_t *data_source_2 = NULL;
    const char *data_source_name_2 = "Second motor";
    const char *data_source_description_2 = "Test Rack";
    mcl_json_t *data_source_custom_data_2 = _create_custom_data("motor efficiency", "good");
    code = _create_data_source(data_source_name_2, data_source_description_2, data_source_custom_data_2, &data_source_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for second data source");

    // Create third data point.
    mcl_data_point_t *data_point_3 = NULL;
    const char *data_point_name_3 = "Voltage";
    const char *data_point_description_3 = "Test Rack";
    const char *data_point_type_3 = "uint";
    const char *data_point_unit_3 = "Volt";
    mcl_json_t *data_point_custom_data_3 = _create_custom_data("motor voltage", "normal value");
    code = _create_data_point(data_point_name_3, data_point_description_3, data_point_type_3, data_point_unit_3, data_point_custom_data_3, &data_point_3);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for third data point.");

    // Add third data point to second data source.
    code = mcl_data_source_add_data_point(data_source_2, data_point_3);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding third data point to second data source failed.");

    // Create fourth data point.
    mcl_data_point_t *data_point_4 = NULL;
    const char *data_point_name_4 = "Ampere";
    const char *data_point_description_4 = "Test Fan";
    const char *data_point_type_4 = "uint";
    const char *data_point_unit_4 = "Amp";
    mcl_json_t *data_point_custom_data_4 = _create_custom_data("motor current", "low");
    code = _create_data_point(data_point_name_4, data_point_description_4, data_point_type_4, data_point_unit_4, data_point_custom_data_4, &data_point_4);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for fourth data point.");

    // Add fourth data point to second data source.
    code = mcl_data_source_add_data_point(data_source_2, data_point_4);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding fourth data point to second data source failed.");

    // Add second data source to data source configuration.
    code = mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding second data_source to data source configuration failed.");

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(&store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for store.");

    // Add data source configuration to store.
    code = mcl_store_add(store, data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding data source configuration to store failed.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange store.
    code = mcl_connectivity_exchange(connectivity, store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_store_destroy(&store);
    mcl_json_util_destroy(&data_source_custom_data_1);
    mcl_json_util_destroy(&data_point_custom_data_1);
    mcl_json_util_destroy(&data_point_custom_data_2);
    mcl_json_util_destroy(&data_source_custom_data_2);
    mcl_json_util_destroy(&data_point_custom_data_3);
    mcl_json_util_destroy(&data_point_custom_data_4);
}

/**
* GIVEN : Onboarded agent and store with timeseries.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_OK is returned.
*/
void test_exchange_17(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Data point ids comes from data source configuration upload.
    const char *data_point_id_1 = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
    const char *data_point_id_2 = "a90ab7ca-fd5d-11e5-8000-001b1bc14a7b";

    // Timestamps of datapoints.
    const char *timestamp_1 = "2016-04-26T08:06:25.317Z";
    const char *timestamp_2 = "2016-04-26T08:06:27.317Z";

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Create timeseries.
    mcl_timeseries_t *timeseries;
    code = mcl_timeseries_initialize(MCL_TIMESERIES_VERSION_1_0, &timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries.");

    // Set parameter configuration id.
    code = mcl_timeseries_set_parameter(timeseries, MCL_TIMESERIES_PARAMETER_CONFIGURATION_ID, "e3217e2b-7036-49f2-9814-4c38542cd781");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_set_parameter() failed.");

    // Initialize first timeseries value list.
    mcl_timeseries_value_list_t *timeseries_value_list_1;
    code = mcl_timeseries_value_list_initialize(&timeseries_value_list_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for first timeseries value list.");

    // Set parameter timestamp.
    code = mcl_timeseries_value_list_set_parameter(timeseries_value_list_1, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, timestamp_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_list_set_parameter() failed.");

    // Create first timeseries value.
    mcl_timeseries_value_t *timeseries_value_1;
    const char *value_1 = "96";
    const char *quality_code_1 = "00A60000";
    code = _create_time_series_value(data_point_id_1, value_1, quality_code_1, &timeseries_value_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for first timeseries value.");

    // Add first timeseries value to first timeseries value list.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list_1, timeseries_value_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding first timeseries value to first timeseries value list failed.");

    // Create second timeseries value.
    mcl_timeseries_value_t *timeseries_value_2;
    const char *value_2 = "48";
    const char *quality_code_2 = "00B60006";
    code = _create_time_series_value(data_point_id_2, value_2, quality_code_2, &timeseries_value_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for second timeseries value.");

    // Add second timeseries value to first timeseries value list.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list_1, timeseries_value_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding first timeseries value to first timeseries value list failed.");

    // Add first timeseries value list to timeseries.
    code = mcl_timeseries_add_value_list(timeseries, timeseries_value_list_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding first timeseries value list to timeseries failed.");

    // Initialize second timeseries value list. Second loop.
    mcl_timeseries_value_list_t *timeseries_value_list_2;
    code = mcl_timeseries_value_list_initialize(&timeseries_value_list_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for second timeseries value list.");

    // Set parameter timestamp.
    code = mcl_timeseries_value_list_set_parameter(timeseries_value_list_2, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, timestamp_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_list_set_parameter() failed.");

    // Create third timeseries value.
    mcl_timeseries_value_t *timeseries_value_3;
    const char *value_3 = "96";
    const char *quality_code_3 = "00A60000";
    code = _create_time_series_value(data_point_id_1, value_3, quality_code_3, &timeseries_value_3);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for third timeseries value.");

    // Add third timeseries value to second timeseries value list.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list_2, timeseries_value_3);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding third timeseries value to second timeseries value list failed.");

    // Create fourth timeseries value.
    mcl_timeseries_value_t *timeseries_value_4;
    const char *value_4 = "24";
    const char *quality_code_4 = "00D40006";
    code = _create_time_series_value(data_point_id_2, value_4, quality_code_4, &timeseries_value_4);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for fourth timeseries value.");

    // Add fourth timeseries value to second timeseries value list.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list_2, timeseries_value_4);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding fourth timeseries value to second timeseries value list failed.");

    // Add second timeseries value list to timeseries.
    code = mcl_timeseries_add_value_list(timeseries, timeseries_value_list_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding second timeseries value list to timeseries failed.");

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(&store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for store.");

    // Add timeseries to store.
    code = mcl_store_add(store, timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding event to store failed.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange store.
    code = mcl_connectivity_exchange(connectivity, store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_store_destroy(&store);
}

/**
* GIVEN : Onboarded agent and store with file.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_OK is returned.
*/
void test_exchange_18(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Create a file containing data to be uploaded.
    const char *file_data = "0123456789";
    const char *remote_file_name = "data_file.txt";
    code = _create_file(file_data, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "File could not be created.");

    // Initialize file.
    mcl_file_t *file = MCL_NULL;
    code = mcl_file_initialize(MCL_FILE_VERSION_1_0, &file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for file.");

    // Set file type.
    const char *file_type = "text";
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_TYPE, file_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file type.");

    // Set file remote name.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_REMOTE_NAME, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file name.");

    // Set file local path.
    const char *file_local_path = "data_file.txt";
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_LOCAL_PATH, file_local_path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file local path.");

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(&store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for store.");

    // Add file to store.
    code = mcl_store_add(store, file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding event to store failed.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange store.
    code = mcl_connectivity_exchange(connectivity, store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_store_destroy(&store);
}

/**
* GIVEN : Onboarded agent and store with custom data.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_OK is returned.
*/
void test_exchange_19(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;
    mcl_custom_data_t *custom_data = MCL_NULL;
    const char *version = "version 3.0.1";
    const char *custom_data_payload_type = "anyTypeSpecifiedByAgent";
    const char *custom_data_payload_content_type = "custom_payload_content_type1";
    const char *custom_data_payload_buffer = "buffer-1";
    mcl_size_t buffer_size = 8;
    const char *custom_data_payload_details_name = "Motor-1";
    const char *custom_data_payload_details_value = "Out of service";

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize custom_data.
    code = mcl_custom_data_initialize(version, &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for custom_data.");

    // Set custom_data type parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_TYPE, custom_data_payload_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Setting payload type failed.");

    // Set custom_data content type parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_CONTENT_TYPE, custom_data_payload_content_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Setting content type failed.");

    // Set custom_data buffer parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_BUFFER, custom_data_payload_buffer);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Setting payload buffer failed.");

    // Set custom_data size parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_BUFFER_SIZE, &buffer_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Payload size parameter could not be set.");

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(&store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for store.");

    // Add custom data to store.
    code = mcl_store_add(store, custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding event to store failed.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange store.
    code = mcl_connectivity_exchange(connectivity, store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_store_destroy(&store);
}

/**
* GIVEN : Onboarded agent and store with file and event items which have a total size bigger than max http payload size.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_OK is returned.
*/
void test_exchange_20(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    mcl_size_t max_http_payload_size = 10 * 1024 * 1024;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(&store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for store.");

    // Create a file containing data to be uploaded.
    const char *file_local_path_1 = "data_file_1.txt";
    char *too_large_buffer = calloc(MCL_MAXIMUM_HTTP_PAYLOAD_SIZE / 2 + 1, 1);
    memset(too_large_buffer, 'A', MCL_MAXIMUM_HTTP_PAYLOAD_SIZE / 2);
    code = _create_file(too_large_buffer, file_local_path_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "File could not be created.");

    // Initialize file.
    mcl_file_t *file_1 = MCL_NULL;
    code = mcl_file_initialize(MCL_FILE_VERSION_1_0, &file_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for file.");

    // Set file type.
    const char *file_type_1 = "text";
    code = mcl_file_set_parameter(file_1, MCL_FILE_PARAMETER_TYPE, file_type_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file type.");

    // Set file remote name.
    const char *remote_file_name_1 = "remote_data_file_1.txt";
    code = mcl_file_set_parameter(file_1, MCL_FILE_PARAMETER_REMOTE_NAME, remote_file_name_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file name.");

    // Set file local path.
    code = mcl_file_set_parameter(file_1, MCL_FILE_PARAMETER_LOCAL_PATH, file_local_path_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file local path.");

    // Add file to store.
    code = mcl_store_add(store, file_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding event to store failed.");

    // Create a file containing data to be uploaded.
    const char *file_local_path_2 = "data_file_2.txt";
    memset(too_large_buffer, 'B', MCL_MAXIMUM_HTTP_PAYLOAD_SIZE / 2);
    code = _create_file(too_large_buffer, file_local_path_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "File could not be created.");

    // Initialize file.
    mcl_file_t *file_2 = MCL_NULL;
    code = mcl_file_initialize(MCL_FILE_VERSION_1_0, &file_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for file.");

    // Set file type.
    const char *file_type_2 = "text";
    code = mcl_file_set_parameter(file_2, MCL_FILE_PARAMETER_TYPE, file_type_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file type.");

    // Set file remote name.
    const char *remote_file_name_2 = "remote_data_file_2.txt";
    code = mcl_file_set_parameter(file_2, MCL_FILE_PARAMETER_REMOTE_NAME, remote_file_name_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file name.");

    // Set file local path.
    code = mcl_file_set_parameter(file_2, MCL_FILE_PARAMETER_LOCAL_PATH, file_local_path_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file local path.");

    // Add file to store.
    code = mcl_store_add(store, file_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding event to store failed.");

    // Create event.
    mcl_event_t *event = NULL;
    code = _create_event(&event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Event couldn't be created.");

    // Add event to store.
    code = mcl_store_add(store, event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding event to store failed.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Set max HTTP payload size parameter.
    mcl_size_t max_http_size = MCL_CONNECTIVITY_MAX_HTTP_PAYLOAD_SIZE;
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_MAX_HTTP_PAYLOAD_SIZE, &max_http_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration maximum http payload size couldn't be set.");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange store.
    code = mcl_connectivity_exchange(connectivity, store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_store_destroy(&store);
    remove(file_local_path_1);
    remove(file_local_path_2);
    free(too_large_buffer);
}

/**
* GIVEN : Onboarded agent and store with file which has a size bigger than maximum http payload size, event and timeseries items.
* WHEN  : mcl_connectivity_exchange() is called.
* THEN  : MCL_STORE_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE is returned.
*/
void test_exchange_21(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    mcl_size_t max_http_payload_size = 10 * 1024 * 1024;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Data point ids comes from data source configuration upload.
    const char *data_point_id_1 = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
    const char *data_point_id_2 = "a90ab7ca-fd5d-11e5-8000-001b1bc14a7b";

    // Timestamps of datapoints.
    const char *timestamp_1 = "2016-04-26T08:06:25.317Z";
    const char *timestamp_2 = "2016-04-26T08:06:27.317Z";

    // Initialize core configuration.
    mcl_core_configuration_initialize(&configuration);

    // Set mindsphere hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(&store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for store.");

    // Create timeseries.
    mcl_timeseries_t *timeseries;
    code = mcl_timeseries_initialize(MCL_TIMESERIES_VERSION_1_0, &timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries.");

    // Set parameter configuration id.
    code = mcl_timeseries_set_parameter(timeseries, MCL_TIMESERIES_PARAMETER_CONFIGURATION_ID, "e3217e2b-7036-49f2-9814-4c38542cd781");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_set_parameter() failed.");

    // Initialize first timeseries value list.
    mcl_timeseries_value_list_t *timeseries_value_list_1;
    code = mcl_timeseries_value_list_initialize(&timeseries_value_list_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for first timeseries value list.");

    // Set parameter timestamp.
    code = mcl_timeseries_value_list_set_parameter(timeseries_value_list_1, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, timestamp_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_list_set_parameter() failed.");

    // Create first timeseries value.
    mcl_timeseries_value_t *timeseries_value_1;
    const char *value_1 = "96";
    const char *quality_code_1 = "00A60000";
    code = _create_time_series_value(data_point_id_1, value_1, quality_code_1, &timeseries_value_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for first timeseries value.");

    // Add first timeseries value to first timeseries value list.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list_1, timeseries_value_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding first timeseries value to first timeseries value list failed.");

    // Create second timeseries value.
    mcl_timeseries_value_t *timeseries_value_2;
    const char *value_2 = "48";
    const char *quality_code_2 = "00B60006";
    code = _create_time_series_value(data_point_id_2, value_2, quality_code_2, &timeseries_value_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for second timeseries value.");

    // Add second timeseries value to first timeseries value list.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list_1, timeseries_value_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding first timeseries value to first timeseries value list failed.");

    // Add first timeseries value list to timeseries.
    code = mcl_timeseries_add_value_list(timeseries, timeseries_value_list_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding first timeseries value list to timeseries failed.");

    // Initialize second timeseries value list. Second loop.
    mcl_timeseries_value_list_t *timeseries_value_list_2;
    code = mcl_timeseries_value_list_initialize(&timeseries_value_list_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for second timeseries value list.");

    // Set parameter timestamp.
    code = mcl_timeseries_value_list_set_parameter(timeseries_value_list_2, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, timestamp_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_list_set_parameter() failed.");

    // Create third timeseries value.
    mcl_timeseries_value_t *timeseries_value_3;
    const char *value_3 = "96";
    const char *quality_code_3 = "00A60000";
    code = _create_time_series_value(data_point_id_1, value_3, quality_code_3, &timeseries_value_3);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for third timeseries value.");

    // Add third timeseries value to second timeseries value list.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list_2, timeseries_value_3);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding third timeseries value to second timeseries value list failed.");

    // Create fourth timeseries value.
    mcl_timeseries_value_t *timeseries_value_4;
    const char *value_4 = "24";
    const char *quality_code_4 = "00D40006";
    code = _create_time_series_value(data_point_id_2, value_4, quality_code_4, &timeseries_value_4);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for fourth timeseries value.");

    // Add fourth timeseries value to second timeseries value list.
    code = mcl_timeseries_value_list_add_value(timeseries_value_list_2, timeseries_value_4);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding fourth timeseries value to second timeseries value list failed.");

    // Add second timeseries value list to timeseries.
    code = mcl_timeseries_add_value_list(timeseries, timeseries_value_list_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding second timeseries value list to timeseries failed.");

    // Add timeseries to store.
    code = mcl_store_add(store, timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding event to store failed.");

    // Create a file containing data to be uploaded.
    const char *file_local_path = "data_file.txt";
    char *too_large_buffer = calloc(MCL_MAXIMUM_HTTP_PAYLOAD_SIZE + 1, 1);
    memset(too_large_buffer, 'A', MCL_MAXIMUM_HTTP_PAYLOAD_SIZE);
    code = _create_file(too_large_buffer, file_local_path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "File could not be created.");

    // Initialize file.
    mcl_file_t *file = MCL_NULL;
    code = mcl_file_initialize(MCL_FILE_VERSION_1_0, &file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for file.");

    // Set file type.
    const char *file_type = "text";
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_TYPE, file_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file type.");

    // Set file remote name.
    const char *remote_file_name = "remote_data_file.txt";
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_REMOTE_NAME, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file name.");

    // Set file local path.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_LOCAL_PATH, file_local_path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file local path.");

    // Add file to store.
    code = mcl_store_add(store, file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding event to store failed.");

    // Create event.
    mcl_event_t *event = NULL;
    code = _create_event(&event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Event couldn't be created.");

    // Add event to store.
    code = mcl_store_add(store, event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding event to store failed.");

    // Initialize connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration;
    code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration couldn't be created.");

    // Set core parameter.
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration core parameter couldn't be set.");

    // Set max HTTP payload size parameter.
    mcl_size_t max_http_size = MCL_CONNECTIVITY_MAX_HTTP_PAYLOAD_SIZE;
    code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_MAX_HTTP_PAYLOAD_SIZE, &max_http_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity configuration maximum http payload size couldn't be set");

    // Initialize connectivity.
    mcl_connectivity_t *connectivity = NULL;
    code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Connectivity initialization failed.");

    // Exchange store.
    code = mcl_connectivity_exchange(connectivity, store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE, code, "Exchange failed.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_store_destroy(&store);
    remove(file_local_path);
    free(too_large_buffer);
}

static mcl_json_t *_create_custom_data(const char *name, const char *value)
{
    // Create custom data as json.
    mcl_json_t *custom_data = NULL;
    mcl_error_t code = mcl_json_util_initialize(MCL_JSON_OBJECT, &custom_data);

    // Add string to custom data.
    (MCL_OK == code) && (code = mcl_json_util_add_string(custom_data, name, value));

    // If something is wrong, destroy custom data.
    if (MCL_OK != code)
    {
        mcl_json_util_destroy(&custom_data);
    }

    return custom_data;
}

static mcl_error_t _create_data_point(const char *name, const char *description, const char *type, const char *unit, mcl_json_t *custom_data, mcl_data_point_t **data_point)
{
    // Create data point.
    mcl_error_t code = mcl_data_point_initialize(data_point);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data point");

    // Generate id of data point.
    char *id = NULL;
    code = mcl_random_generate_guid(&id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Generating id failed.");

    // Set id parameter.
    code = mcl_data_point_set_parameter(*data_point, MCL_DATA_POINT_PARAMETER_ID, id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");

    // Destroy id.
    free(id);

    // Set name parameter.
    code = mcl_data_point_set_parameter(*data_point, MCL_DATA_POINT_PARAMETER_NAME, name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");

    if (NULL != description)
    {
        // Set description parameter.
        code = mcl_data_point_set_parameter(*data_point, MCL_DATA_POINT_PARAMETER_DESCRIPTION, description);
        TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");
    }

    // Set type parameter.
    code = mcl_data_point_set_parameter(*data_point, MCL_DATA_POINT_PARAMETER_TYPE, type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");

    // Set unit parameter.
    code = mcl_data_point_set_parameter(*data_point, MCL_DATA_POINT_PARAMETER_UNIT, unit);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");

    if (NULL != custom_data)
    {
        // Set custom_data parameter.
        code = mcl_data_point_set_parameter(*data_point, MCL_DATA_POINT_PARAMETER_CUSTOM_DATA, custom_data);
        TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_point_set_parameter() failed.");
    }

    return code;
}

static mcl_error_t _create_data_source(const char *name, const char *description, mcl_json_t *custom_data, mcl_data_source_t **data_source)
{
    // Create data source.
    mcl_error_t code = mcl_data_source_initialize(data_source);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source");

    // Set name parameter.
    code = mcl_data_source_set_parameter(*data_source, MCL_DATA_SOURCE_PARAMETER_NAME, name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_source_set_parameter() failed for name.");

    if (NULL != description)
    {
        // Set description parameter.
        code = mcl_data_source_set_parameter(*data_source, MCL_DATA_SOURCE_PARAMETER_DESCRIPTION, description);
        TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_source_set_parameter() failed for description.");
    }

    if (NULL != custom_data)
    {
        // Set custom_data parameter.
        code = mcl_data_source_set_parameter(*data_source, MCL_DATA_SOURCE_PARAMETER_CUSTOM_DATA, custom_data);
        TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_source_set_parameter() failed for custom data.");
    }

    return code;
}

static mcl_error_t _create_data_source_configuration(mcl_data_source_configuration_t **data_source_configuration)
{
    // Create data source configuration.
    mcl_error_t code = mcl_data_source_configuration_initialize(MCL_DATA_SOURCE_CONFIGURATION_1_0, data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for data source configuration");

    // Generate configuration id of data source configuration.
    char *configuration_id = NULL;
    code = mcl_random_generate_guid(&configuration_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Generating configuration id failed.");

    // Set configuration id of data source configuration.
    code = mcl_data_source_configuration_set_parameter(*data_source_configuration, MCL_DATA_SOURCE_CONFIGURATION_PARAMETER_ID, configuration_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_data_source_configuration_set_parameter() failed.");

    // Destroy configuration id.
    free(configuration_id);

    return code;
}

static mcl_error_t _create_event(mcl_event_t **event)
{
    mcl_error_t code = mcl_event_initialize(MCL_EVENT_VERSION_1_0, event);

    (MCL_OK == code) && (code = mcl_event_set_parameter(*event, MCL_EVENT_PARAMETER_TYPE, "thresholdViolation"));

    (MCL_OK == code) && (code = mcl_event_set_parameter(*event, MCL_EVENT_PARAMETER_VERSION, "1.0.1"));

    E_MCL_EVENT_SEVERITY severity = MCL_EVENT_SEVERITY_ERROR;
    (MCL_OK == code) && (code = mcl_event_set_parameter(*event, MCL_EVENT_PARAMETER_SEVERITY, &severity));

    (MCL_OK == code) && (code = mcl_event_set_parameter(*event, MCL_EVENT_PARAMETER_TIMESTAMP_ISO8601, "2018-04-26T08:06:25.317Z"));

    (MCL_OK == code) && (code = mcl_event_set_parameter(*event, MCL_EVENT_PARAMETER_DESCRIPTION, "Best event ever"));

    (MCL_OK == code) && (code = mcl_event_set_parameter(*event, MCL_EVENT_PARAMETER_CORRELATION_ID, "6ba7b810-9dad-11d1-80b4-00c04fd430c8"));

    mcl_json_t *event_payload_details = NULL;
    (MCL_OK == code) && (code = mcl_json_util_initialize(MCL_JSON_OBJECT, &event_payload_details));

    (MCL_OK == code) && (code = mcl_json_util_add_string(event_payload_details, "message1", "Critical level"));

    (MCL_OK == code) && (code = mcl_event_set_parameter(*event, MCL_EVENT_PARAMETER_DETAILS, event_payload_details));

    if (MCL_OK != code)
    {
        mcl_event_destroy(event);
    }

    mcl_json_util_destroy(&event_payload_details);

    return code;
}

static mcl_error_t _create_time_series_value(const char *data_point_id, const char *value, const char *quality_code, mcl_timeseries_value_t **timeseries_value)
{
    // Initialize timeseries value.
    mcl_error_t code = mcl_timeseries_value_initialize(timeseries_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for timeseries value.");

    // Set parameter data point id.
    code = mcl_timeseries_value_set_parameter(*timeseries_value, MCL_TIMESERIES_VALUE_PARAMETER_DATA_POINT_ID, data_point_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_set_parameter() failed.");

    // Set parameter value.
    code = mcl_timeseries_value_set_parameter(*timeseries_value, MCL_TIMESERIES_VALUE_PARAMETER_VALUE, value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_set_parameter() failed.");

    // Set parameter quality code.
    code = mcl_timeseries_value_set_parameter(*timeseries_value, MCL_TIMESERIES_VALUE_PARAMETER_QUALITY_CODE, quality_code);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_timeseries_value_set_parameter() failed.");

    return code;
}

static mcl_error_t _create_file(const char *file_content, const char *file_name)
{
    MCL_DEBUG_ENTRY("const char *file_content = <%p>, const char *file_name = <%p>", file_content, file_name);

    void *file_descriptor = MCL_NULL;
    mcl_error_t code = mcl_file_util_fopen(file_name, "w", &file_descriptor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "File couldn't be opened.");

    // Write data to the file.
    code = mcl_file_util_fputs(file_content, file_descriptor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Data couldn't be written to file.");

    // Close the file.
    mcl_file_util_fclose(file_descriptor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "File can not be closed.");

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}
