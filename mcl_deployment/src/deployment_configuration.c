/*!
 * @file     deployment_configuration.c
 * @brief    Implementation of component configuration.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_deployment/mcl_deployment_configuration.h"
#include "deployment_configuration.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

// Endpoint to list deployment workflows.
static const char workflow_instances_endpoint[] = "/api/deploymentworkflow/v3/instances";

// This function sets core parameter of component configuration.
static mcl_error_t _set_deployment_configuration_core_parameter(mcl_deployment_configuration_t *configuration, mcl_core_t *core);

mcl_error_t mcl_deployment_configuration_initialize(mcl_deployment_configuration_t **configuration)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_deployment_configuration_t **configuration = <%p>", configuration);

    // Null check.
    MCL_ASSERT_NOT_NULL(configuration, code);

    // Allocate memory for configuration data structure.
    MCL_NEW(*configuration);

    if (MCL_NULL == *configuration)
    {
        code = MCL_OUT_OF_MEMORY;
    }
    else
    {
        // Set default values of configuration parameters.
        (*configuration)->core = MCL_NULL;
        (*configuration)->workflow_instances_url = MCL_NULL;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_deployment_configuration_set_parameter(mcl_deployment_configuration_t *configuration,
    E_MCL_DEPLOYMENT_CONFIGURATION_PARAMETER parameter, const void *value)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_deployment_configuration_t *configuration = <%p>, E_MCL_DEPLOYMENT_CONFIGURATION_PARAMETER parameter = <%d>, "\
        "const void *value = <%p>", configuration, parameter, value);

    MCL_ASSERT_NOT_NULL(configuration, code);
    MCL_ASSERT_NOT_NULL(value, code);

    if (parameter == MCL_DEPLOYMENT_CONFIGURATION_PARAMETER_CORE)
    {
        code = _set_deployment_configuration_core_parameter(configuration, (mcl_core_t *) value);
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t deployment_configuration_validate(deployment_configuration_t *configuration)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("deployment_configuration_t *configuration = <%p>", configuration);

    // Checks whether core parameter of configuration is set or not.
    if (MCL_NULL == configuration->core)
    {
        MCL_ERROR_STRING("Core parameter of deployment configuration is not set.");
        code = MCL_INVALID_PARAMETER;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_deployment_configuration_destroy(mcl_deployment_configuration_t **configuration)
{
    MCL_DEBUG_ENTRY("mcl_deployment_configuration_t **configuration = <%p>", configuration);

    if ((MCL_NULL != configuration) && (MCL_NULL != *configuration))
    {
        MCL_FREE((*configuration)->workflow_instances_url);
        MCL_FREE(*configuration);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}

static mcl_error_t _set_deployment_configuration_core_parameter(mcl_deployment_configuration_t *configuration, mcl_core_t *core)
{
    mcl_error_t code = MCL_OK;
    const char *hostname;

    MCL_DEBUG_ENTRY("mcl_deployment_configuration_t *configuration = <%p>, mcl_core_t *core = <%p>", configuration, core);

    // Set core.
    configuration->core = core;

    // Get hostname.
    hostname = mcl_core_get_host_name(configuration->core);

    if (MCL_NULL != hostname)
    {
        // Calculate hostname length.
        mcl_size_t hostname_length = mcl_string_util_strlen(hostname);

        // Allocate buffer for the url.
        configuration->workflow_instances_url = MCL_MALLOC(hostname_length + sizeof(workflow_instances_endpoint));

        if (MCL_NULL == configuration->workflow_instances_url)
        {
            code = MCL_OUT_OF_MEMORY;
        }
        else
        {
            // Concatenate host name with the constant endpoint to list deployment workflows.
            mcl_string_util_memcpy(configuration->workflow_instances_url, hostname, hostname_length);
            mcl_string_util_memcpy(configuration->workflow_instances_url + hostname_length, workflow_instances_endpoint, sizeof(workflow_instances_endpoint));
        }
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
