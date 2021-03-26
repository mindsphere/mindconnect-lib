/*!
 * @file     deployment_workflow_model_transition.c
 * @brief    Implementation of deployment workflow model transition.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_deployment/mcl_deployment_workflow_model_transition.h"
#include "deployment_workflow_model_transition.h"
#include "mcl_core/mcl_memory.h"

mcl_error_t deployment_workflow_model_transition_initialize(deployment_workflow_model_transition_t **transition)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("deployment_workflow_model_transition_t **transition = <%p>", transition);

    if (MCL_NULL != MCL_NEW(*transition))
    {
        (*transition)->from = MCL_NULL;
        (*transition)->to = MCL_NULL;
        (*transition)->type = MCL_NULL;
        (*transition)->details = MCL_NULL;
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    // Error check.
    if (MCL_OK != code)
    {
        deployment_workflow_model_transition_destroy(transition);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_deployment_workflow_model_transition_get_parameter(mcl_deployment_workflow_model_transition_t *transition,
    E_MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER parameter, void **value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_deployment_workflow_model_transition_t *transition = <%p>, E_MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER parameter = <%d>, "\
        "void **value = <%p>", transition, parameter, value);

    MCL_ASSERT_NOT_NULL(transition, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER_FROM:
            *value = transition->from;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER_TO:
            *value = transition->to;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER_TYPE:
            *value = transition->type;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER_DETAILS:
            *value = transition->details;
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

void deployment_workflow_model_transition_destroy(deployment_workflow_model_transition_t **transition)
{
    MCL_DEBUG_ENTRY("deployment_workflow_model_transition_t **transition = <%p>", transition);

    if ((MCL_NULL != transition) && (MCL_NULL != *transition))
    {
        MCL_FREE((*transition)->from);
        MCL_FREE((*transition)->to);
        MCL_FREE((*transition)->type);
        mcl_json_util_destroy(&((*transition)->details));
        MCL_FREE(*transition);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
