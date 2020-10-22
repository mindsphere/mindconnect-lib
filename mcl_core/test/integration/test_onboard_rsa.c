/*!
 * @file     test_onboard_rsa.c
 * @brief    Integration tests for onboarding functionality with rsa security profile.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_core/mcl_core.h"

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
    "-----BEGIN CERTIFICATE-----\n"\
    "MIIDsjCCApqgAwIBAgIQbvQfnldQrKRBqEGPNinT2zANBgkqhkiG9w0BAQsFADBn\n"\
    "MSswKQYDVQQLDCJDcmVhdGVkIGJ5IGh0dHA6Ly93d3cuZmlkZGxlcjIuY29tMRUw\n"\
    "EwYDVQQKDAxET19OT1RfVFJVU1QxITAfBgNVBAMMGERPX05PVF9UUlVTVF9GaWRk\n"\
    "bGVyUm9vdDAeFw0xNzAxMzExNDQ0MDFaFw0yMzAxMzExNDQ0MDFaMGcxKzApBgNV\n"\
    "BAsMIkNyZWF0ZWQgYnkgaHR0cDovL3d3dy5maWRkbGVyMi5jb20xFTATBgNVBAoM\n"\
    "DERPX05PVF9UUlVTVDEhMB8GA1UEAwwYRE9fTk9UX1RSVVNUX0ZpZGRsZXJSb290\n"\
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAvWZEiI0vap/0NDtb46bK\n"\
    "PEfpXuOnbZ++KwVvm84GCfCflN4l8fZm5Mrd5rAf2M77Ql4N4y7S2TG7l2ZLjzT5\n"\
    "KxPPqgdxnf2ZV2lzsMw36FG2aI+Ub8/n4L0qupSlerWBtQzn+NYF0O1JXgdq2jd9\n"\
    "Dfebc3QjTrH2jJtnPF+gCfVi+gDSxUzE/8zEp9EgfqkkcDzeEWlSq2/nE8aZKx4W\n"\
    "sfVxch1wFYY90vgxa38bygKAC/p0JG3xiiWBCTTx2TJfw2Gf+WIRhC9GQmIuaxu6\n"\
    "YBNT4iw+GaH84KKAkPiQVb4e0lHfgBIMFZ9yfgVCwl6wlLQHZQDMRgS+4agyc9Ar\n"\
    "owIDAQABo1owWDATBgNVHSUEDDAKBggrBgEFBQcDATASBgNVHRMBAf8ECDAGAQH/\n"\
    "AgEAMB0GA1UdDgQWBBQQdVTbAUv/aM9L6neV01lcWSW88DAOBgNVHQ8BAf8EBAMC\n"\
    "AQYwDQYJKoZIhvcNAQELBQADggEBAFK5NEDKLxYvm8Ngmhjd1wuC/kH/4ahqP1BF\n"\
    "t9J3WMZqNwbukr5Oxkvk7DbUbkoj+HGqRX4pldClCPQ4qXdrWJm+KjGTwqYR0nMd\n"\
    "IrAeeWwENcZQC2Sf+JBtLPKY2WBR1CYpfCAjLb1/cER9WUOagIdycCHdxqOv6OHy\n"\
    "0iuaxkIULMrDwOxZIi5g0YHarXLXz70lKvJCq3CCMJ8hMNE/uoPnRzyCMg2lQXIc\n"\
    "dNP50xslM1iXr5rZ4iBlqgRWzA6sFPfOpU3xGppHqO01FoRqrqmcJ+I+X70c6rPB\n"\
    "t4Lsc4TJ0KsL9UboJP+Y4BX0oRDxcoP+ngxA8BhdFPqy0VLdHYw=\n"\
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
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *user_agent = "custom agent v1.0";
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
}

void tearDown(void)
{
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&configuration);
}

/**
* GIVEN : Agent is not onboarded. Security profile is RSA.
* WHEN  : Agent tries to onboard.
* THEN  : MCL_OK is returned indicating a successful onboard.
*/
void test_onboard_01(void)
{
    // Variable declaration.
    mcl_error_t code;
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%";

    // Set user agent parameter.
    code = mcl_core_configuration_set_parameter(configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Initialize an MCL Core Handle with the given configuration.
    code = mcl_core_initialize(configuration, &core);

    // Onboard only if initialization is successful.
    if (MCL_OK == code)
    {
        code = mcl_core_onboard(core);
    }

    TEST_ASSERT_MESSAGE(MCL_OK == code, "Onboard failed.");
}
