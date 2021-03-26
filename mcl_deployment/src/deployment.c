/*!
 * @file     deployment.c
 * @brief    Implementation of MCL Deployment component functionality.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_deployment/mcl_deployment.h"
#include "deployment.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

mcl_error_t mcl_deployment_initialize(mcl_deployment_configuration_t *configuration, mcl_deployment_t **deployment)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_deployment_configuration_t *configuration = <%p>, mcl_deployment_t **deployment = <%p>", configuration, deployment);

    // Null check of configuration.
    MCL_ASSERT_NOT_NULL(configuration, code);

    // Null check of deployment.
    MCL_ASSERT_NOT_NULL(deployment, code);

    // Validate deployment configuration.
    code = deployment_configuration_validate(configuration);

    if (MCL_OK == code)
    {
        // Allocate memory for deployment handle.
        MCL_NEW(*deployment);

        if (MCL_NULL != *deployment)
        {
            // Set configuration.
            (*deployment)->configuration = configuration;

            // Initialize deployment processor.
            (*deployment)->processor.access_token = MCL_NULL;
            (*deployment)->processor.http_client = mcl_core_get_http_client(configuration->core);
            (*deployment)->processor.workflow_instances_url = configuration->workflow_instances_url;
        }
        else
        {
            code = MCL_OUT_OF_MEMORY;
        }
    }

    if (MCL_OK != code)
    {
        mcl_deployment_destroy(deployment);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_deployment_list_workflows(mcl_deployment_t *deployment, mcl_deployment_workflow_filter_t *filter, mcl_list_t **workflows)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_deployment_t *deployment = <%p>, mcl_deployment_workflow_filter_t *filter = <%p>, mcl_list_t **workflows = <%p>",
        deployment, filter, workflows);

    MCL_ASSERT_NOT_NULL(deployment, code);
    MCL_ASSERT_NOT_NULL(workflows, code);

    // Free previous access token.
    MCL_FREE(deployment->processor.access_token);

    // Get access token from core.
    code = mcl_core_get_last_access_token(deployment->configuration->core, &deployment->processor.access_token);

    // Make the request and parse the response.
    if (MCL_OK == code)
    {
        code = deployment_processor_list_workflows(&(deployment->processor), filter, workflows);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_deployment_get_workflow(mcl_deployment_t *deployment, char *id, mcl_deployment_workflow_filter_t *filter, mcl_deployment_workflow_t **workflow)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_deployment_t *deployment = <%p>, char *id = <%p>, mcl_deployment_workflow_filter_t *filter = <%p>, "\
        "mcl_deployment_workflow_t **workflow = <%p>", deployment, id, filter, workflow);

    MCL_ASSERT_NOT_NULL(deployment, code);
    MCL_ASSERT_NOT_NULL(id, code);
    MCL_ASSERT_NOT_NULL(workflow, code);

    // Free previous access token.
    MCL_FREE(deployment->processor.access_token);

    // Get access token from core.
    code = mcl_core_get_last_access_token(deployment->configuration->core, &deployment->processor.access_token);

    // Make the request and parse the response.
    if (MCL_OK == code)
    {
        code = deployment_processor_get_workflow(&(deployment->processor), id, filter, workflow);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_deployment_update_workflow(mcl_deployment_t *deployment, char *id, mcl_deployment_workflow_state_t *state,
    mcl_deployment_workflow_filter_t *filter, mcl_deployment_workflow_t **workflow)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_deployment_t *deployment = <%p>, char *id = <%p>, mcl_deployment_workflow_state_t *state = <%p>, "\
        "mcl_deployment_workflow_filter_t *filter = <%p>, mcl_deployment_workflow_t **workflow = <%p>", deployment, id, state, filter, workflow);

    MCL_ASSERT_NOT_NULL(deployment, code);
    MCL_ASSERT_NOT_NULL(id, code);
    MCL_ASSERT_NOT_NULL(state, code);
    MCL_ASSERT_NOT_NULL(workflow, code);

    // Free previous access token.
    MCL_FREE(deployment->processor.access_token);

    // Get access token from core.
    code = mcl_core_get_last_access_token(deployment->configuration->core, &deployment->processor.access_token);

    // Make the request and parse the response.
    if (MCL_OK == code)
    {
        code = deployment_processor_update_workflow(&(deployment->processor), id, state, filter, workflow);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_deployment_destroy(mcl_deployment_t **deployment)
{
    MCL_DEBUG_ENTRY("mcl_deployment_t **deployment = <%p>", deployment);

    if ((MCL_NULL != deployment) && (MCL_NULL != *deployment))
    {
        MCL_FREE((*deployment)->processor.access_token);
        MCL_FREE(*deployment);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
