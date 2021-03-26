/*!
 * @file     deployment_workflow_model_state.c
 * @brief    Implementation of deployment workflow model state.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_deployment/mcl_deployment_workflow_model_state.h"
#include "deployment_workflow_model_state.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

mcl_error_t deployment_workflow_model_state_initialize(deployment_workflow_model_state_t **state)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("deployment_workflow_model_state_t **state = <%p>", state);

    if (MCL_NULL != MCL_NEW(*state))
    {
        (*state)->name = MCL_NULL;
        (*state)->description = MCL_NULL;
        (*state)->initial = MCL_FALSE;
        (*state)->final = MCL_FALSE;
        (*state)->cancel = MCL_FALSE;
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    // Error check.
    if (MCL_OK != code)
    {
        deployment_workflow_model_state_destroy(state);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_deployment_workflow_model_state_get_parameter(mcl_deployment_workflow_model_state_t *state,
    E_MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER parameter, void **value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_deployment_workflow_model_state_t *state = <%p>, E_MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER parameter = <%d>, "\
        "void **value = <%p>", state, parameter, value);

    MCL_ASSERT_NOT_NULL(state, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_NAME:
            *value = state->name;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_DESCRIPTION:
            *value = state->description;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_INITIAL:
            *value = &state->initial;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_FINAL:
            *value = &state->final;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_CANCEL:
            *value = &state->cancel;
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

void deployment_workflow_model_state_destroy(deployment_workflow_model_state_t **state)
{
    MCL_DEBUG_ENTRY("deployment_workflow_model_state_t **state = <%p>", state);

    if ((MCL_NULL != state) && (MCL_NULL != *state))
    {
        MCL_FREE((*state)->name);
        MCL_FREE((*state)->description);
        MCL_FREE(*state);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
