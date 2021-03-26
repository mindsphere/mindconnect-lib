/*!
 * @file     deployment_workflow_model.c
 * @brief    Implementation of deployment workflow model.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_deployment/mcl_deployment_workflow_model.h"
#include "deployment_workflow_model.h"
#include "mcl_core/mcl_memory.h"

mcl_error_t deployment_workflow_model_initialize(deployment_workflow_model_t **model)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("deployment_workflow_model_t **model = <%p>", model);

    if (MCL_NULL != MCL_NEW(*model))
    {
        (*model)->key = MCL_NULL;
        (*model)->states = MCL_NULL;
        (*model)->transitions = MCL_NULL;
        (*model)->state_groups = MCL_NULL;
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    // Error check.
    if (MCL_OK != code)
    {
        deployment_workflow_model_destroy(model);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_deployment_workflow_model_get_parameter(mcl_deployment_workflow_model_t *model,
    E_MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER parameter, void **value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_deployment_workflow_model_t *model = <%p>, E_MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER parameter = <%d>, void **value = <%p>",
        model, parameter, value);

    MCL_ASSERT_NOT_NULL(model, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER_KEY:
            *value = model->key;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER_STATES:
            *value = model->states;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER_TRANSITIONS:
            *value = model->transitions;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER_STATE_GROUPS:
            *value = model->state_groups;
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

void deployment_workflow_model_destroy(deployment_workflow_model_t **model)
{
    MCL_DEBUG_ENTRY("deployment_workflow_model_t **model = <%p>", model);

    if ((MCL_NULL != model) && (MCL_NULL != *model))
    {
        MCL_FREE((*model)->key);
        mcl_list_destroy_with_content(&(*model)->states, (mcl_list_item_destroy_callback) deployment_workflow_model_state_destroy);
        mcl_list_destroy_with_content(&(*model)->transitions, (mcl_list_item_destroy_callback) deployment_workflow_model_transition_destroy);
        mcl_list_destroy_with_content(&(*model)->state_groups, (mcl_list_item_destroy_callback) deployment_workflow_model_state_group_destroy);
        MCL_FREE(*model);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
