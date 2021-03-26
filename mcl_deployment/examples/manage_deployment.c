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

const char* mindsphere_certificate = "-----BEGIN CERTIFICATE-----\n"\
"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\n"\
"-----END CERTIFICATE-----";

// Buffer size for initial access token.
#define IAT_BUFFER_SIZE 2048

#define MINDSPHERE_HOSTNAME     "https://southgate.eu1.mindsphere.io"
#define MINDSPHERE_TENANT       "mclibiot"
#define USE_HTTP_PROXY          1
#define USE_SHARED_SECRET       1

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
    char *user_agent = "Custom Agent v0.0.1";

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
