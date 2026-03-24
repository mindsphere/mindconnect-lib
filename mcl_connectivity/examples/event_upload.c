/*!
 * @file     event_upload.c
 * @brief    Example custom agent code which uploads event.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include <stdio.h>
#include "mcl_connectivity/mcl_connectivity.h"
#include "callbacks.h"

const char my_initial_access_token[] = "<Initial Access Token>";

const char *mindsphere_certificate = "-----BEGIN CERTIFICATE-----\n"
"MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n"
"MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n"
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n"
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n"
"2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n"
"1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n"
"q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n"
"tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n"
"vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n"
"BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n"
"5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n"
"1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n"
"NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n"
"Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n"
"8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n"
"pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n"
"MrY=\n"
"-----END CERTIFICATE-----\n"
"-----BEGIN CERTIFICATE-----\n"\
"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\n"\
"-----END CERTIFICATE-----";

int main(void)
{
    mcl_error_t code;
    mcl_core_t *core = NULL;
    mcl_connectivity_t *connectivity = NULL;

    // Core configuration and related parameters.
    mcl_core_configuration_t *core_configuration = NULL;
    mcl_uint16_t mindsphere_port = 443;
    mcl_uint16_t proxy_port = 9400;
    E_MCL_PROXY mcl_proxy = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_SHARED_SECRET;

    // Connectivity configuration and related parameter.
    mcl_connectivity_configuration_t *connectivity_configuration = NULL;
    mcl_size_t maximum_payload_size = 65536;

    // Event to exchange.
    mcl_event_t *my_event = NULL;

    // Set log level. Note that default log level is the level set during build.
    code = mcl_log_util_set_output_level(MCL_LOG_LEVEL_INFO);
    printf("Setting log output level : %s.\n", MCL_CORE_CODE_TO_STRING(code));

    // Initializing core configuration.
    if (MCL_OK == code)
    {
        code = mcl_core_configuration_initialize(&core_configuration);
        printf("Core configuration initialization : %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    if (MCL_OK == code)
    {
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, my_initial_access_token);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, "https://api.eu1.siemens.app");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mindsphere_certificate);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, "194.138.0.5");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &mcl_proxy);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, "custom agent v1.0");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, "mclibiot");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_LOAD_CALLBACK, custom_load_function_shared_secret);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_SAVE_CALLBACK, custom_save_function_shared_secret);

        // Initialize mcl core with the core configuration.
        code = mcl_core_initialize(core_configuration, &core);
        printf("MCL core initialization : %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    // If mcl core is successfully initialized, try to onboard.
    if (MCL_OK == code)
    {
        code = mcl_core_onboard(core);
        printf("Onboarding : %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    // If onboarding is successful or the agent is already onboarded, get access token.
    if ((MCL_OK == code) || (MCL_ALREADY_ONBOARDED == code))
    {
        code = mcl_core_get_access_token(core);
        printf("Getting access token : %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    // Initialize connectivity configuration.
    if (MCL_OK == code)
    {
        code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    }

    // Set core for connectivity configuration.
    if (MCL_OK == code)
    {
        code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    }

    // Set max HTTP payload size for connectivity configuration.
    if (MCL_OK == code)
    {
        code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_MAX_HTTP_PAYLOAD_SIZE, &maximum_payload_size);
    }

    // Initialize mcl connectivity.
    if (MCL_OK == code)
    {
        code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    }

    // Create event.
    if (MCL_OK == code)
    {
        E_MCL_EVENT_SEVERITY my_event_severity = MCL_EVENT_SEVERITY_ERROR;

        // Initializing an event.
        code = mcl_event_initialize(MCL_EVENT_VERSION_1_0, &my_event);
        printf("Event initialization : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));

        // Setting severity.
        if (MCL_OK == code)
        {
            code = mcl_event_set_parameter(my_event, MCL_EVENT_PARAMETER_SEVERITY, &my_event_severity);
            printf("|-->Setting severity of event : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
        }

        // Setting timestamp.
        if (MCL_OK == code)
        {
            code = mcl_event_set_parameter(my_event, MCL_EVENT_PARAMETER_TIMESTAMP_ISO8601, "2018-08-08T01:23:45.678Z");
            printf("|-->Setting timestamp of event : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
        }

        // Setting version.
        if (MCL_OK == code)
        {
            code = mcl_event_set_parameter(my_event, MCL_EVENT_PARAMETER_VERSION, "1.0");
            printf("|-->Setting version of event : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
        }

        // Setting type.
        if (MCL_OK == code)
        {
            code = mcl_event_set_parameter(my_event, MCL_EVENT_PARAMETER_TYPE, "MindSphereStandardEvent");
            printf("|-->Setting type of event : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
        }

        // Setting description.
        if (MCL_OK == code)
        {
            code = mcl_event_set_parameter(my_event, MCL_EVENT_PARAMETER_DESCRIPTION, "Your event description.");
            printf("|-->Setting description of event : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
        }
    }

    // Exchange event.
    if (MCL_OK == code)
    {
        code = mcl_connectivity_exchange(connectivity, my_event);
        printf("Uploading item : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
    }

    // Destroy the core configuration, event, core and connectivity.
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&core_configuration);
    mcl_event_destroy(&my_event);

    if (MCL_OK == code)
    {
        return 0;
    }

    return -1;
}
