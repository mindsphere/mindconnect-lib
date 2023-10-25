/*!
 * @file     onboard.c
 * @brief    Example custom agent code which onboards and gets access token.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mcl_core/mcl_core.h"
#include "callbacks.h"

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
"-----END CERTIFICATE-----";

// Buffer size for initial access token.
#define IAT_BUFFER_SIZE 2048

#define MINDSPHERE_HOSTNAME     "https://southgate.eu1.mindsphere.io"
#define MINDSPHERE_TENANT       "mclibiot"
#define USE_HTTP_PROXY          1
#define USE_SHARED_SECRET       0

#if USE_HTTP_PROXY
#define HTTP_PROXY_HOSTNAME     "194.138.0.5"
#define HTTP_PROXY_PORT         9400
#endif

// Get string from terminal to buffer.
mcl_size_t get_string_to_buffer(char *buffer, mcl_size_t buffer_size);

int main(void)
{
    printf("***** CUSTOM AGENT using MCL *****\n");

    mcl_error_t code;
    mcl_core_configuration_t *core_configuration = NULL;
    mcl_core_t *core = NULL;
    char *initial_access_token = NULL;
    mcl_uint16_t mindsphere_port = 443;
#if USE_HTTP_PROXY
    mcl_uint16_t proxy_port = HTTP_PROXY_PORT;
    E_MCL_PROXY mcl_proxy = MCL_PROXY_HTTP;
#endif

#if USE_SHARED_SECRET
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_SHARED_SECRET;
#else
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;
#endif

    // Initialize core configuration.
    code = mcl_core_configuration_initialize(&core_configuration);
    printf("Initializing core configuration: %s.\n", MCL_CORE_CODE_TO_STRING(code));

    if (MCL_OK == code)
    {
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, MINDSPHERE_HOSTNAME);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mindsphere_certificate);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, "custom agent v1.0");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, MINDSPHERE_TENANT);

#if USE_HTTP_PROXY
        // Set proxy.
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, HTTP_PROXY_HOSTNAME);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &mcl_proxy);
#endif

        // Set security profile and callback functions.
#if USE_SHARED_SECRET
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_LOAD_CALLBACK, custom_load_function_shared_secret);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_SAVE_CALLBACK, custom_save_function_shared_secret);
#else
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_LOAD_CALLBACK, custom_load_function_rsa);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_SAVE_CALLBACK, custom_save_function_rsa);
#endif

        // Initialize mcl core with the core configuration.
        code = mcl_core_initialize(core_configuration, &core);
        printf("Initializing mcl core: %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    // If no credentials are provided with load function, then request initial access token from user.
    if (MCL_NO_ACCESS_TOKEN_PROVIDED == code)
    {
        initial_access_token = malloc(IAT_BUFFER_SIZE);

        if (NULL == initial_access_token)
        {
            code = MCL_OUT_OF_MEMORY;
            printf("Cannot allocate buffer for initial access token!");
        }
        else
        {
            mcl_size_t string_length = 0;

            while (0 == string_length)
            {
                printf("Give me an initial access token.\n");
                string_length = get_string_to_buffer(initial_access_token, IAT_BUFFER_SIZE);
            }

            mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, initial_access_token);
            code = mcl_core_initialize(core_configuration, &core);
            printf("Initialize core with initial access token: %s.\n", MCL_CORE_CODE_TO_STRING(code));
            free(initial_access_token);
        }
    }

    // If mcl core is successfully initialized, try to onboard.
    if (MCL_OK == code)
    {
        code = mcl_core_onboard(core);
        printf("Onboarding: %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    if ((MCL_OK == code) || (MCL_ALREADY_ONBOARDED == code))
    {
        char selection = '\0';
        
        while ('e' != selection)
        {
            if ('\r' != selection && '\n' != selection)
            {
                printf("Press 'g' to try to get access token and print it, press 'e' to exit.\n");
            }

            selection = (char) getchar();

            if ('g' == selection)
            {
                code = mcl_core_get_access_token(core);
                printf("Getting access token: %s.\n", MCL_CORE_CODE_TO_STRING(code));

                if (MCL_BAD_REQUEST == code)
                {
                    // Key rotation is needed.
                    code = mcl_core_rotate_key(core);
                    printf("Rotating key: %s.\n", MCL_CORE_CODE_TO_STRING(code));

                    if (MCL_OK == code)
                    {
                        code = mcl_core_get_access_token(core);
                        printf("Getting access token after key rotation: %s.\n", MCL_CORE_CODE_TO_STRING(code));
                    }
                }

                if (MCL_OK == code)
                {
                    char *access_token = NULL;
                    code = mcl_core_get_last_access_token(core, &access_token);

                    if (MCL_OK == code)
                    {
                        printf("Here is your access token:\n\n%s\n\n", access_token);
                        free(access_token);
                    }               
                }
            }
        }
    }

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&core_configuration);

    if (MCL_OK != code)
    {
        return -1;
    }

    return 0;
}

mcl_size_t get_string_to_buffer(char *buffer, mcl_size_t buffer_size)
{
    mcl_size_t current_length = 0;
    char c = 0;

    c = (char) getchar();

    while (c != EOF && current_length + 1 < buffer_size && c != '\r' && c != '\n')
    {
        buffer[current_length++] = c;
        c = (char) getchar();
    }

    if (current_length != 0)
    {
        buffer[current_length] = '\0';
    }

    if (c == '\r')
    {
        // Assume there is '\n' after '\r'.
        c = (char) getchar();
    }

    return current_length;
}
