/*!
 * @file     core.c
 * @brief    Core module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_core/mcl_core.h"
#include "core.h"
#include "definitions.h"
#include "mcl_core/mcl_memory.h"
#include "string_util.h"

#define USER_AGENT_HEADER_FORMAT "MCL/" MCL_VERSION_STRING " (%s)"

mcl_error_t mcl_core_initialize(mcl_core_configuration_t *configuration, mcl_core_t **core)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_core_configuration_t *configuration = <%p>, mcl_core_t **core = <%p>", configuration, core);

    // Null check of configuration.
    MCL_ASSERT_NOT_NULL(configuration, code);

    // Null check for input argument **core.
    MCL_ASSERT_NOT_NULL(core, code);

    // Validate core configuration.
    code = core_configuration_validate(configuration);

    if (MCL_OK == code)
    {
        // Allocate memory for mcl handle.
        MCL_NEW(*core);
        MCL_ASSERT_CODE_MESSAGE(MCL_NULL != *core, MCL_OUT_OF_MEMORY, "Memory can not be allocated for core object.");

        // Set core configuration.
        (*core)->configuration = configuration;

        // Initialize core processor.
        code = core_processor_initialize((*core)->configuration, &((*core)->core_processor));
        MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, mcl_core_destroy(core), code, "Core processor initialization failed.");

        MCL_INFO("Core structure is initialized.");
        core_configuration_log(configuration);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return code;
}

mcl_error_t mcl_core_onboard(mcl_core_t *core)
{
    mcl_error_t result;
    mcl_bool_t critical_section_callbacks_are_used;

    MCL_DEBUG_ENTRY("mcl_core_t *core = <%p>", core);

    // Null check.
    MCL_ASSERT_NOT_NULL(core, result);

    critical_section_callbacks_are_used = (MCL_NULL != core->configuration->critical_section_enter_callback) &&
        (MCL_NULL != core->configuration->critical_section_leave_callback);
    if (MCL_FALSE != critical_section_callbacks_are_used)
    {
        result = core->configuration->critical_section_enter_callback();
        MCL_ASSERT_OK(result);
    }

    if (MCL_FALSE == mcl_core_is_onboarded(core))
    {
        // Start the onboarding procedure.
        result = core_processor_register(core->core_processor);
        if (MCL_OK == result)
        {
            MCL_INFO("Agent is successfully onboarded.");
        }
        else
        {
            MCL_ERROR("Onboarding of agent failed.");
        }
    }
    else
    {
        MCL_INFO("Agent is already onboarded.");
        result = MCL_ALREADY_ONBOARDED;
    }

    if (MCL_FALSE != critical_section_callbacks_are_used)
    {
        core->configuration->critical_section_leave_callback();
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

mcl_error_t mcl_core_rotate_key(mcl_core_t *core)
{
    mcl_error_t result;
    mcl_bool_t critical_section_callbacks_are_used;

    MCL_DEBUG_ENTRY("mcl_core_t *core = <%p>", core);

    // Null check.
    MCL_ASSERT_NOT_NULL(core, result);

    critical_section_callbacks_are_used = (MCL_NULL != core->configuration->critical_section_enter_callback) &&
        (MCL_NULL != core->configuration->critical_section_leave_callback);
    if (MCL_FALSE != critical_section_callbacks_are_used)
    {
        result = core->configuration->critical_section_enter_callback();
        MCL_ASSERT_OK(result);
    }

    // Check if the agent is onboarded or not.
    if (MCL_FALSE != mcl_core_is_onboarded(core))
    {
        // Perform key rotation.
        MCL_INFO("Key rotation started.");
        result = core_processor_register(core->core_processor);
        if (MCL_OK == result)
        {
            MCL_INFO("Key successfully rotated.");
        }
        else
        {
            MCL_ERROR("Key rotation failed.");
        }
    }
    else
    {
        result = MCL_NOT_ONBOARDED;
    }

    if (MCL_FALSE != critical_section_callbacks_are_used)
    {
        core->configuration->critical_section_leave_callback();
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

mcl_error_t mcl_core_update_credentials(mcl_core_t *core)
{
    mcl_error_t result;
    mcl_bool_t critical_section_callbacks_are_used;

    MCL_DEBUG_ENTRY("mcl_core_t *core = <%p>", core);

    // Null check.
    MCL_ASSERT_NOT_NULL(core, result);

    critical_section_callbacks_are_used = (MCL_NULL != core->configuration->critical_section_enter_callback) &&
        (MCL_NULL != core->configuration->critical_section_leave_callback);
    if (MCL_TRUE == critical_section_callbacks_are_used)
    {
        result = core->configuration->critical_section_enter_callback();
        MCL_ASSERT_OK(result);
    }

    // Check if the agent is onboarded.
    if (MCL_TRUE == mcl_core_is_onboarded(core))
    {
        // Update credentials.
        result = core_processor_update_credentials(core->core_processor);
    }
    else
    {
        result = MCL_NOT_ONBOARDED;
    }

    if (MCL_TRUE == critical_section_callbacks_are_used)
    {
        core->configuration->critical_section_leave_callback();
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

mcl_error_t mcl_core_get_access_token(mcl_core_t *core)
{
    mcl_error_t result;

    MCL_DEBUG_ENTRY("mcl_core_t *core = <%p>", core);

    // Null check.
    MCL_ASSERT_NOT_NULL(core, result);

    if (MCL_TRUE == mcl_core_is_onboarded(core))
    {
        result = core_processor_get_access_token(core->core_processor);

        if (MCL_OK == result)
        {
            MCL_INFO("Access token received.");
        }
        else
        {
            MCL_ERROR("Failed to get access token.");
        }
    }
    else
    {
        result = MCL_NOT_ONBOARDED;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

mcl_bool_t mcl_core_is_onboarded(mcl_core_t *core)
{
    mcl_bool_t is_onboarded = MCL_FALSE;

    MCL_DEBUG_ENTRY("mcl_core_t *core = <%p>", core);

    if (MCL_NULL != core)
    {
        is_onboarded = (MCL_NULL == core->core_processor->security_handler->registration_access_token) ? MCL_FALSE : MCL_TRUE;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", is_onboarded);
    return is_onboarded;
}

mcl_error_t mcl_core_get_last_access_token(mcl_core_t *core, char **token)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_core_t *core = <%p>, char **token = <%p>", core, token);

    // Null check.
    MCL_ASSERT_NOT_NULL(core, code);
    MCL_ASSERT_NOT_NULL(token, code);

    if (MCL_TRUE != mcl_core_is_onboarded(core))
    {
        code = MCL_NOT_ONBOARDED;
    }
    else if(MCL_NULL == core->core_processor->security_handler->access_token)
    {
        code = MCL_NO_ACCESS_TOKEN_EXISTS;
    }
    else
    {
        mcl_size_t token_length;
        token_length = string_util_strlen(core->core_processor->security_handler->access_token) + 1;
        *token = MCL_CALLOC(token_length, 1);

        if (MCL_NULL == *token)
        {
            code = MCL_OUT_OF_MEMORY;
        }
        else
        {
            string_util_memcpy(*token, core->core_processor->security_handler->access_token, token_length);
        }
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_core_get_last_token_time(mcl_core_t *core, char **token_time)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_core_t *core = <%p>, char **token_time = <%p>", core, token_time);

    // Null check.
    MCL_ASSERT_NOT_NULL(core, code);
    MCL_ASSERT_NOT_NULL(token_time, code);
    *token_time = MCL_NULL;

    if (MCL_NULL == core->core_processor->security_handler->last_token_time)
    {
        code = MCL_NO_SERVER_TIME;
    }
    else
    {
        mcl_size_t token_time_length;
        token_time_length = string_util_strlen(core->core_processor->security_handler->last_token_time) + 1;
        *token_time = MCL_CALLOC(token_time_length, 1);

        if (MCL_NULL == *token_time)
        {
            code = MCL_OUT_OF_MEMORY;
        }
        else
        {
            string_util_memcpy(*token_time, core->core_processor->security_handler->last_token_time, token_time_length);
        }
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_http_client_t *mcl_core_get_http_client(mcl_core_t *core)
{
    mcl_http_client_t *http_client = MCL_NULL;

    MCL_DEBUG_ENTRY("mcl_core_t *core = <%p>", core);

    // Validate core before getting http client.
    if (MCL_NULL != core)
    {
        http_client = core->core_processor->http_client;
    }

    MCL_DEBUG_LEAVE("retVal = <%p>", http_client);
    return http_client;
}

const char *mcl_core_get_host_name(mcl_core_t *core)
{
    char *host_name = MCL_NULL;

    MCL_DEBUG_ENTRY("mcl_core_t *core = <%p>", core);

    // Validate core before getting host name.
    if (MCL_NULL != core)
    {
        host_name = core->configuration->mindsphere_hostname;
    }

    MCL_DEBUG_LEAVE("retVal = <%p>", host_name);
    return host_name;
}

const char *mcl_core_get_client_id(mcl_core_t *core)
{
    const char *client_id = MCL_NULL;

    MCL_DEBUG_ENTRY("mcl_core_t *core = <%p>", core);

    // Validate core before getting client id.
    if ((MCL_NULL != core) && (MCL_TRUE == mcl_core_is_onboarded(core)))
    {
        client_id = (const char *)core->core_processor->security_handler->client_id;
    }

    MCL_DEBUG_LEAVE("retVal = <%p>", client_id);
    return client_id;
}

mcl_error_t mcl_core_destroy(mcl_core_t **core)
{
    MCL_DEBUG_ENTRY("mcl_core_t **core = <%p>", core);

    // Free data structure.
    if ((MCL_NULL != core) && (MCL_NULL != *core))
    {
        // Destroy core processor.
        core_processor_destroy(&((*core)->core_processor));

        // Free MCL handle.
        MCL_FREE(*core);

        MCL_DEBUG("Core handle is destroyed.");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}
