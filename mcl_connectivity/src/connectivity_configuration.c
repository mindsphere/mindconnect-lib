/*!
 * @file     connectivity_configuration.c
 * @brief    Connectivity configuration module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_connectivity/mcl_connectivity_configuration.h"
#include "connectivity_configuration.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

// Connectivity exchange api.
static const char _connectivity_exchange_uri[] = "/api/mindconnect/v3/exchange";
static const char _connectivity_mapping_url[] = "/api/mindconnect/v3/dataPointMappings";

// This function sets core parameter of connectivity configuration.
static mcl_error_t _set_connectivity_configuration_core_parameter(mcl_connectivity_configuration_t *configuration, mcl_core_t *core);

mcl_error_t mcl_connectivity_configuration_initialize(mcl_connectivity_configuration_t **configuration)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("connectivity_configuration_t *configuration = <%p>", configuration);

    // Null check.
    MCL_ASSERT_NOT_NULL(configuration, code);

    MCL_NEW(*configuration);

    if (MCL_NULL == *configuration)
    {
        code = MCL_OUT_OF_MEMORY;
    }
    else
    {
        // Initialize connectivity configuration.
        (*configuration)->core = MCL_NULL;
        (*configuration)->exchange_url = MCL_NULL;
        (*configuration)->mapping_url = MCL_NULL;
        (*configuration)->max_http_payload_size = MCL_CONNECTIVITY_DEFAULT_MAX_HTTP_PAYLOAD_SIZE;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_connectivity_configuration_set_parameter(mcl_connectivity_configuration_t *configuration, E_MCL_CONNECTIVITY_CONFIGURATION_PARAMETER parameter, const void *value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_connectivity_configuration_t *configuration = <%p>, E_MCL_CONNECTIVITY_CONFIGURATION_PARAMETER parameter = <%d>, const void *value = <%p>", configuration, parameter, value);

    MCL_ASSERT_NOT_NULL(configuration, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE:
            code = _set_connectivity_configuration_core_parameter(configuration, (mcl_core_t *) value);
            break;

        case MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_MAX_HTTP_PAYLOAD_SIZE:

            if ((MCL_CONNECTIVITY_MIN_HTTP_PAYLOAD_SIZE <= *((const mcl_size_t *) value)) && (MCL_CONNECTIVITY_MAX_HTTP_PAYLOAD_SIZE >= *((const mcl_size_t *) value)))
            {
                configuration->max_http_payload_size = *((const mcl_size_t *) value);
            }
            else
            {
                code = MCL_INVALID_PARAMETER;
            }
            break;

        default:
            code = MCL_INVALID_PARAMETER;
            break;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_connectivity_configuration_destroy(mcl_connectivity_configuration_t **configuration)
{
    MCL_DEBUG_ENTRY("mcl_connectivity_configuration_t **configuration = <%p>", configuration);

    if ((MCL_NULL != configuration) && (MCL_NULL != *configuration))
    {
        MCL_FREE((*configuration)->exchange_url);
        MCL_FREE((*configuration)->mapping_url);
        MCL_FREE(*configuration);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}

mcl_error_t connectivity_configuration_validate(connectivity_configuration_t *configuration)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("connectivity_configuration_t *configuration = <%p>", configuration);

    if (MCL_NULL == configuration->core)
    {
        code = MCL_INVALID_PARAMETER;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _set_connectivity_configuration_core_parameter(mcl_connectivity_configuration_t *configuration, mcl_core_t *core)
{
    mcl_error_t code;
    const char *hostname;

    MCL_DEBUG_ENTRY("mcl_connectivity_configuration_t *configuration = <%p>, mcl_core_t *core = <%p>", configuration, core);

    MCL_FREE(configuration->exchange_url);
    MCL_FREE(configuration->mapping_url);

    // Get hostname.
    hostname = mcl_core_get_host_name(core);

    if (MCL_NULL != hostname)
    {
        code = mcl_string_util_concatenate(hostname, _connectivity_exchange_uri, &(configuration->exchange_url));

        if (MCL_OK == code)
        {
            code = mcl_string_util_concatenate(hostname, _connectivity_mapping_url, &(configuration->mapping_url));

            if (MCL_OK != code)
            {
                MCL_FREE(configuration->exchange_url);
            }
            else
            {
                configuration->core = core;
            }
        }
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
