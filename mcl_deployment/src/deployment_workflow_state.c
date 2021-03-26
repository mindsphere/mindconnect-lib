/*!
 * @file     deployment_workflow_state.c
 * @brief    Implementation of deployment workflow state.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_deployment/mcl_deployment_workflow_state.h"
#include "deployment_workflow_state.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

mcl_error_t mcl_deployment_workflow_state_initialize(mcl_deployment_workflow_state_t **workflow_state)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_deployment_workflow_state_t **workflow_state = <%p>", workflow_state);

    MCL_ASSERT_NOT_NULL(workflow_state, code);

    code = deployment_workflow_state_initialize(workflow_state);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t deployment_workflow_state_initialize(deployment_workflow_state_t **workflow_state)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("deployment_workflow_state_t **workflow_state = <%p>", workflow_state);

    if (MCL_NULL != MCL_NEW(*workflow_state))
    {
        (*workflow_state)->state = MCL_NULL;
        (*workflow_state)->progress = 0.0;
        (*workflow_state)->entered = MCL_NULL;
        (*workflow_state)->updated = MCL_NULL;
        (*workflow_state)->message = MCL_NULL;
        (*workflow_state)->details = MCL_NULL;
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    // Error check.
    if (MCL_OK != code)
    {
        mcl_deployment_workflow_state_destroy(workflow_state);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_deployment_workflow_state_set_parameter(mcl_deployment_workflow_state_t *workflow_state,
    E_MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER parameter, const void *value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_deployment_workflow_state_t *workflow_state = <%p>, "\
        "E_MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER parameter = <%d>, const void *value = <%p>", workflow_state, parameter, value);

    MCL_ASSERT_NOT_NULL(workflow_state, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE:
            code = mcl_string_util_reset(value, &workflow_state->state);
            break;
        case MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_PROGRESS:
            workflow_state->progress = *((double *) value);
            break;
        case MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_MESSAGE:
            code = mcl_string_util_reset(value, &workflow_state->message);
            break;
        case MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_DETAILS:
            mcl_json_util_destroy(&workflow_state->details);
            code = mcl_json_util_duplicate(value, &workflow_state->details);
            break;
        default:
            code = MCL_INVALID_PARAMETER;
            break;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_deployment_workflow_state_get_parameter(mcl_deployment_workflow_state_t *workflow_state,
    E_MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER parameter, void **value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_deployment_workflow_state_t *workflow_state = <%p>, E_MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER parameter = <%d>, void **value = <%p>",
        workflow_state, parameter, value);

    MCL_ASSERT_NOT_NULL(workflow_state, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE:
            *value = workflow_state->state;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_PROGRESS:
            *value = &(workflow_state->progress);
            break;
        case MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_ENTERED:
            *value = workflow_state->entered;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_UPDATED:
            *value = workflow_state->updated;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_MESSAGE:
            *value = workflow_state->message;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_DETAILS:
            *value = workflow_state->details;
            break;
        default:
            *value = MCL_NULL;
            code = MCL_INVALID_PARAMETER;
            break;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t deployment_workflow_state_validate(deployment_workflow_state_t *workflow_state)
{
    mcl_error_t code = MCL_INVALID_PARAMETER;

    MCL_DEBUG_ENTRY("deployment_workflow_state_t *workflow_state = <%p>", workflow_state);

    // Check all mandatory parameters.
    if (MCL_NULL == workflow_state->state)
    {
        MCL_ERROR_STRING("State parameter of workflow state is not set.");
    }
    else if (MCL_NULL == workflow_state->message)
    {
        MCL_ERROR_STRING("Message parameter of workflow state is not set.");
    }
    else
    {
        code = MCL_OK;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_deployment_workflow_state_destroy(mcl_deployment_workflow_state_t **workflow_state)
{
    MCL_DEBUG_ENTRY("mcl_deployment_workflow_state_t **workflow_state = <%p>", workflow_state);

    if ((MCL_NULL != workflow_state) && (MCL_NULL != *workflow_state))
    {
        MCL_FREE((*workflow_state)->state);
        MCL_FREE((*workflow_state)->entered);
        MCL_FREE((*workflow_state)->updated);
        MCL_FREE((*workflow_state)->message);
        mcl_json_util_destroy(&(*workflow_state)->details);
        MCL_FREE(*workflow_state);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
