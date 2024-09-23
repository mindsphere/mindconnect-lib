/*!
 * @file     data_lake_upload.c
 * @brief    Example custom agent code which onboards and uploads a file to data lake.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mcl_core/mcl_core.h"
#include "mcl_data_lake/mcl_data_lake.h"
#include "mcl_core/mcl_file_util.h"
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
"-----END CERTIFICATE-----\n"
"-----BEGIN CERTIFICATE-----\n"\
"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\n"\
"-----END CERTIFICATE-----";


const char *data_lake_certificate = "-----BEGIN CERTIFICATE-----\n" \
"MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n" \
"RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n" \
"VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n" \
"DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n" \
"ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n" \
"VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n" \
"mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n" \
"IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n" \
"mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n" \
"XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n" \
"dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n" \
"jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n" \
"BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n" \
"DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n" \
"9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n" \
"jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n" \
"Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n" \
"ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n" \
"R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n" \
"-----END CERTIFICATE-----";

// Buffer size for initial access token.
#define USER_INPUT_BUFFER_SIZE 1024

#define MINDSPHERE_HOSTNAME     "https://southgate.eu1-int.mindsphere.io"
#define MINDSPHERE_TENANT       "mclibiot"
#define USE_HTTP_PROXY          1
#define USE_SHARED_SECRET       0
#define SUBTENANT_ID            "a59e4760fe0b3731db4d36aea59b9742"

#if USE_HTTP_PROXY
#define HTTP_PROXY_HOSTNAME     "127.0.0.1"
#define HTTP_PROXY_PORT         9400
#endif

// Get string from terminal to buffer.
mcl_size_t get_string_to_buffer(char *buffer, mcl_size_t buffer_size);

// Callback to upload file to data lake.
mcl_size_t data_lake_file_callback(char *buffer, mcl_size_t size, mcl_size_t count, void *file_descriptor);

mcl_error_t get_access_token(mcl_core_t *core);

int main(void)
{
    printf("***** CUSTOM AGENT using MCL *****\n");

    mcl_error_t code;
    mcl_core_configuration_t *core_configuration = NULL;
    mcl_data_lake_configuration_t *data_lake_configuration = NULL;
    mcl_core_t *core = NULL;
    mcl_data_lake_t *data_lake = NULL;
    mcl_uint16_t mindsphere_port = 443;
    char user_input_buffer[USER_INPUT_BUFFER_SIZE];

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
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_LOAD_CALLBACK, &custom_load_function_shared_secret);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_SAVE_CALLBACK, &custom_save_function_shared_secret);
#else
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_LOAD_CALLBACK, &custom_load_function_rsa);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_SAVE_CALLBACK, &custom_save_function_rsa);
#endif

        // Initialize mcl core with the core configuration.
        code = mcl_core_initialize(core_configuration, &core);
        printf("Initializing mcl core: %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    // If no credentials are provided with load function, then request initial access token from user.
    if (MCL_NO_ACCESS_TOKEN_PROVIDED == code)
    {
        mcl_size_t string_length = 0;

        while (0 == string_length)
        {
            printf("Give me an initial access token.\n");
            string_length = get_string_to_buffer(user_input_buffer, USER_INPUT_BUFFER_SIZE);
        }

        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, user_input_buffer);
        code = mcl_core_initialize(core_configuration, &core);
        printf("Initialize core with initial access token: %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    // If mcl core is successfully initialized, try to onboard.
    if (MCL_OK == code)
    {
        code = mcl_core_onboard(core);
        printf("Onboarding: %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    // Get access token.
    if ((MCL_OK == code) || (MCL_ALREADY_ONBOARDED == code))
    {
        code = get_access_token(core);
    }

    // Initialize data lake configuration.
    if (MCL_OK == code)
    {
        code = mcl_data_lake_configuration_initialize(&data_lake_configuration);
        printf("Initializing data lake configuration: %s.\n", MCL_DATA_LAKE_CODE_TO_STRING(code));
    }

    // Initialize data lake.
    if (MCL_OK == code)
    {
        mcl_data_lake_configuration_set_parameter(data_lake_configuration, MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CORE, core);
        mcl_data_lake_configuration_set_parameter(data_lake_configuration, MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CERTIFICATE, data_lake_certificate);

        // Initialize mcl data lake.
        code = mcl_data_lake_initialize(data_lake_configuration, &data_lake);
        printf("Initializing mcl data lake: %s.\n", MCL_DATA_LAKE_CODE_TO_STRING(code));
    }

    if (MCL_OK == code)
    {
        char selection = '\0';

        while ('e' != selection)
        {
            mcl_data_lake_object_t *data_lake_object = NULL;
            void *file_descriptor = NULL;

            printf("\nPress 'u' to try to upload a file\n");
            printf("Press 's' to try to upload a file for subtenant\n");
            printf("Press 'e' to exit\n");

            selection = (char) getchar();

            // Remove CR LF from input buffer.
            while ('\n' != getchar());

            if ('u' != selection && 's' != selection)
            {
                continue;
            }

            // Initialize data lake object.
            code = mcl_data_lake_object_initialize(&data_lake_object);
            printf("Initializing mcl data lake object: %s.\n", MCL_DATA_LAKE_CODE_TO_STRING(code));

            if (MCL_OK == code)
            {
                mcl_size_t string_length = 0;

                while (0 == string_length)
                {
                    printf("Enter a path to generate upload url.\n");
                    string_length = get_string_to_buffer(user_input_buffer, USER_INPUT_BUFFER_SIZE);
                }

                // Set path to generate upload url.
                code = mcl_data_lake_object_set_parameter(data_lake_object, MCL_DATA_LAKE_OBJECT_PARAMETER_PATH, user_input_buffer);
                printf("Setting path of mcl data lake object: %s.\n", MCL_DATA_LAKE_CODE_TO_STRING(code));
            }

            // Generate upload url.
            if (MCL_OK == code)
            {
                if ('u' == selection)
                {
                    code = mcl_data_lake_generate_upload_url(data_lake, data_lake_object);
                }
                else
                {
                    code = mcl_data_lake_generate_upload_url_for_subtenant(data_lake, data_lake_object, SUBTENANT_ID);
                }
                
                printf("Generating upload url: %s.\n", MCL_DATA_LAKE_CODE_TO_STRING(code));
            }

            if (MCL_OK == code)
            {
                mcl_size_t string_length = 0;

                while (0 == string_length)
                {
                    printf("Enter the path of the file which will be uploaded.\n");
                    string_length = get_string_to_buffer(user_input_buffer, USER_INPUT_BUFFER_SIZE);
                }

                code = mcl_file_util_fopen(user_input_buffer, "rb", &file_descriptor);
                printf("Opening the file which will be uploaded: %s.\n", MCL_CORE_CODE_TO_STRING(code));
            }

            // Set parameters of data lake object.
            if (MCL_OK == code)
            {
                mcl_size_t file_size = mcl_file_util_get_file_size(file_descriptor);
                mcl_data_lake_object_set_parameter(data_lake_object, MCL_DATA_LAKE_OBJECT_PARAMETER_SIZE, &file_size);
                mcl_data_lake_object_set_parameter(data_lake_object, MCL_DATA_LAKE_OBJECT_PARAMETER_USER_CONTEXT, file_descriptor);
                mcl_data_lake_object_set_parameter(data_lake_object, MCL_DATA_LAKE_OBJECT_PARAMETER_UPLOAD_CALLBACK, &data_lake_file_callback);

                code = mcl_data_lake_upload(data_lake, data_lake_object);
                printf("Uploading file: %s.\n", MCL_DATA_LAKE_CODE_TO_STRING(code));
            }
            
            if((MCL_FORBIDDEN == code) || (MCL_UNAUTHORIZED == code))
            {
                get_access_token(core);
            }

            mcl_file_util_fclose(file_descriptor);
            mcl_data_lake_object_destroy(&data_lake_object);
        }
    }

    // Clean up.
    mcl_data_lake_destroy(&data_lake);
    mcl_data_lake_configuration_destroy(&data_lake_configuration);
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

    while ((EOF != c) && (current_length + 1 < buffer_size) && ('\r' != c) && ('\n' != c))
    {
        buffer[current_length++] = c;
        c = (char) getchar();
    }

    if (0 != current_length)
    {
        buffer[current_length] = '\0';
    }

    if ('\r' == c)
    {
        // Assume there is '\n' after '\r'.
        (void) getchar();
    }

    return current_length;
}

mcl_size_t data_lake_file_callback(char *buffer, mcl_size_t size, mcl_size_t count, void *file_descriptor)
{
    mcl_size_t read_size = 0;

    mcl_file_util_fread(buffer, size, count, file_descriptor, &read_size);

    return read_size;
}

mcl_error_t get_access_token(mcl_core_t *core)
{
    mcl_error_t code;

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

    return code;
}
