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

const char mindsphere_certificate[] = "-----BEGIN CERTIFICATE-----\n"\
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
    mcl_uint16_t proxy_port = 3128;
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
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, "https://southgate.eu1.mindsphere.io");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mindsphere_certificate);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, "192.168.0.1");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &mcl_proxy);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, "my user agent");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, "mclibiot");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_LOAD_CALLBACK, custom_load_function);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_SAVE_CALLBACK, custom_save_function);

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
