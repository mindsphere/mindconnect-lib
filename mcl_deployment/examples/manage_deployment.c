/*!
 * @file     manage_deployment.c
 * @brief    Example custom agent that gets and updates a workflow assigned to it.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mcl_core/mcl_core.h"
#include "mcl_deployment/mcl_deployment.h"
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


// Buffer size for initial access token.
#define IAT_BUFFER_SIZE 2048

#define MINDSPHERE_HOSTNAME     "https://api.eu1.siemens.app"
#define MINDSPHERE_TENANT       "mclibiot"
#define USE_HTTP_PROXY          1
#define USE_SHARED_SECRET       0

#if USE_HTTP_PROXY
#define HTTP_PROXY_HOSTNAME     "127.0.0.1"
#define HTTP_PROXY_PORT         9400
#endif

void print_workflows(mcl_list_t *workflows);
void print_workflow(mcl_deployment_workflow_t *workflow);
mcl_error_t create_new_state(char *name, char *message, double progress, mcl_deployment_workflow_state_t** state);
mcl_size_t get_string_to_buffer(char *buffer, mcl_size_t buffer_size);

int main(void)
{
    printf("***** CUSTOM AGENT using MCL *****\n");

    mcl_error_t code;
    mcl_core_configuration_t *core_configuration = NULL;
    mcl_deployment_configuration_t *configuration = MCL_NULL;
    mcl_core_t *core = NULL;
    mcl_deployment_t *deployment = MCL_NULL;

    char *initial_access_token = NULL;
    mcl_uint16_t mindsphere_port = 443;
    char *user_agent = "custom agent v1.0";

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
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
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
                printf("Enter Initial Access Token:\n");
                string_length = get_string_to_buffer(initial_access_token, IAT_BUFFER_SIZE);
            }

            mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, initial_access_token);
            code = mcl_core_initialize(core_configuration, &core);
            printf("Initializing core with initial access token: %s.\n", MCL_CORE_CODE_TO_STRING(code));
            free(initial_access_token);
        }
    }

    // Onboard if not already onboarded.
    if (MCL_OK == code)
    {
        code = mcl_core_onboard(core);
        printf("Onboarding: %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    // Get access token.
    if ((MCL_OK == code) || (MCL_ALREADY_ONBOARDED == code))
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
    }

    // Initialize configuration for deployment component.
    if (MCL_OK == code)
    {
        code = mcl_deployment_configuration_initialize(&configuration);
    }

    // Set core parameter of deployment configuration.
    if (MCL_OK == code)
    {
        code = mcl_deployment_configuration_set_parameter(configuration, MCL_DEPLOYMENT_CONFIGURATION_PARAMETER_CORE, core);
    }

    // Initialize deployment component.
    if (MCL_OK == code)
    {
        code = mcl_deployment_initialize(configuration, &deployment);
    }

    if (MCL_OK == code)
    {
        char selection = '\0';

        while ('e' != selection)
        {
            if ('\r' != selection && '\n' != selection)
            {
                printf("\n");
                printf("Press 'l' to list deployment workflow instances for your agent.\n");
                printf("Press 'g' to get the workflow instance with given id.\n");
                printf("Press 'u' to update the workflow instance with given id.\n");
                printf("Press 'e' to exit.\n");           
            }

            // Get user selection.
            selection = (char) getchar();

            if ('l' == selection)
            {
                mcl_list_t *workflows = MCL_NULL;
                
                // Get the list of workflows assigned to the agent.
                code = mcl_deployment_list_workflows(deployment, MCL_NULL, &workflows);

                // Display the result from MindSphere.
                if (MCL_OK == code)
                {
                    print_workflows(workflows);
                }

                // Free allocated memory.
                mcl_list_destroy_with_content(&workflows, (mcl_list_item_destroy_callback) mcl_deployment_workflow_destroy);
            }
            else if ('g' == selection)
            {
                mcl_size_t length = 0;
                mcl_size_t id_buffer_size = 64;
                mcl_deployment_workflow_t *workflow = MCL_NULL;

                char *id = malloc(id_buffer_size);

                // Ask for workflow id from user.
                printf("Enter the ID of the workflow to get:\n");
                while (0 == length)
                {
                    length = get_string_to_buffer(id, id_buffer_size);
                }

                // Get the workflow from MindSphere.
                code = mcl_deployment_get_workflow(deployment, id, MCL_NULL, &workflow);
                
                // Display the result from MindSphere.
                if (MCL_OK == code)
                {
                    printf("\n");
                    printf("Workflow Description:\n");
                    printf("---------------------\n");
                    print_workflow(workflow);
                }
     
                // Free allocated memory.
                free(id);
                mcl_deployment_workflow_destroy(&workflow);
            }
            else if ('u' == selection)
            {
                mcl_size_t length = 0;
                mcl_size_t id_buffer_size = 64;
                mcl_deployment_workflow_t *workflow = MCL_NULL;
                mcl_deployment_workflow_state_t *state = MCL_NULL;

                // Ask for workflow id from user.
                char *id = malloc(id_buffer_size);
                printf("Enter the ID of the workflow to update:\n");
                while (0 == length)
                {
                    length = get_string_to_buffer(id, id_buffer_size);
                }

                // Ask for new state from user.
                char *state_name = malloc(id_buffer_size);
                printf("Enter the name of the new state:\n");
                length = 0;
                while (0 == length)
                {
                    length = get_string_to_buffer(state_name, id_buffer_size);
                }

                // Ask for the progress of the new state from user.
                printf("Enter the progress of the new state:\n");
                double progress;
                scanf("%lf", &progress);

                // Create new state for the workflow.
                code = create_new_state(state_name, "Some Message", progress, &state);

                // Update the workflow on MindSphere with new state.
                if (MCL_OK == code)
                {
                    code = mcl_deployment_update_workflow(deployment, id, state, MCL_NULL, &workflow);
                }

                // Display the result from MindSphere.
                if (MCL_OK == code)
                {
                    printf("\n");
                    printf("Workflow Description:\n");
                    printf("---------------------\n");
                    print_workflow(workflow);
                }

                // Free allocated memory.
                free(id);
                free(state_name);
                mcl_deployment_workflow_destroy(&workflow);
                mcl_deployment_workflow_state_destroy(&state);
            }
        }
    }

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&core_configuration);
    mcl_deployment_destroy(&deployment);
    mcl_deployment_configuration_destroy(&configuration);

    if (MCL_OK != code)
    {
        return -1;
    }

    return 0;
}

mcl_size_t get_string_to_buffer(char* buffer, mcl_size_t buffer_size)
{
    mcl_size_t current_length = 0;
    char c = 0;

    c = (char) getchar();

    while (c != EOF && current_length + 1 < buffer_size && c != '\r' && c != '\n')
    {
        buffer[current_length++] = c;
        c = (char)getchar();
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

void print_workflows(mcl_list_t *workflows)
{
    int index;
    mcl_list_node_t *node = MCL_NULL;
    mcl_deployment_workflow_t *workflow = MCL_NULL;

    int count = (int) workflows->count;
    
    printf("\nListing %d workflow(s) ... \n\n", count);

    for (index = 0; index < count; ++index)
    {
        mcl_list_next(workflows, &node);
        workflow = (mcl_deployment_workflow_t*) node->data;

        printf("\nWorkflow-%d :\n", index + 1);
        printf("-------------\n");
        
        print_workflow(workflow);
    }
}

void print_workflow(mcl_deployment_workflow_t *workflow)
{
    char *id;
    char *device_id;
    char *created_at;
    mcl_deployment_workflow_state_t *workflow_state;
    char *state;
    double *progress;
    char *entered;
    char *updated;
    char *message;

    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_ID, &id);
    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_DEVICE_ID, &device_id);
    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CREATED_AT, &created_at);
    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CURRENT_STATE, &workflow_state);

    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, &state);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_PROGRESS, &progress);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_ENTERED, &entered);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_UPDATED, &updated);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_MESSAGE, &message);

    printf("Id         : %s\n", id);
    printf("Device Id  : %s\n", device_id);
    printf("Created At : %s\n", created_at);
    printf("State      : %s\n", state);
    printf("Progress   : %f\n", *progress);
    printf("Entered    : %s\n", entered);
    printf("Updated    : %s\n", updated);
    printf("Message    : %s\n", message);
}

mcl_error_t create_new_state(char *name, char *message, double progress, mcl_deployment_workflow_state_t **state)
{
    mcl_error_t code;
    mcl_json_t *details = MCL_NULL;

    // Initialize the workflow state.
    code = mcl_deployment_workflow_state_initialize(state);

    // Set the parameters of new workflow state.
    if (MCL_OK == code)
    {
        code = mcl_json_util_initialize(MCL_JSON_OBJECT, &details);
    }
    
    if (MCL_OK == code)
    {
        code = mcl_deployment_workflow_state_set_parameter(*state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, name);
    } 

    if (MCL_OK == code)
    {
        code = mcl_deployment_workflow_state_set_parameter(*state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_PROGRESS, &progress);
    }

    if (MCL_OK == code)
    {
        code = mcl_deployment_workflow_state_set_parameter(*state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_MESSAGE, message);
    }

    if (MCL_OK == code)
    {
        code = mcl_deployment_workflow_state_set_parameter(*state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_DETAILS, details);
    }

    return code;
}
