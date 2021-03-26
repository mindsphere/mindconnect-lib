/*!
 * @file     deployment_workflow_model_state_group.c
 * @brief    Implementation of deployment workflow model state group.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_deployment/mcl_deployment_workflow_model_state_group.h"
#include "deployment_workflow_model_state_group.h"
#include "mcl_core/mcl_memory.h"

static void _destroy_string(char **str);

mcl_error_t deployment_workflow_model_state_group_initialize(deployment_workflow_model_state_group_t **state_group)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("deployment_workflow_model_state_group_t **state_group = <%p>", state_group);

    if (MCL_NULL != MCL_NEW(*state_group))
    {
        (*state_group)->name = MCL_NULL;
        (*state_group)->states = MCL_NULL;
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    // Error check.
    if (MCL_OK != code)
    {
        deployment_workflow_model_state_group_destroy(state_group);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_deployment_workflow_model_state_group_get_parameter(mcl_deployment_workflow_model_state_group_t *state_group,
    E_MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_PARAMETER parameter, void **value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_deployment_workflow_model_state_group_t *state_group = <%p>, "\
        "E_MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_PARAMETER parameter = <%d>, void **value = <%p>", state_group, parameter, value);

    MCL_ASSERT_NOT_NULL(state_group, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_PARAMETER_NAME:
            *value = state_group->name;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_PARAMETER_STATES:
            *value = state_group->states;
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

void deployment_workflow_model_state_group_destroy(deployment_workflow_model_state_group_t **state_group)
{
    MCL_DEBUG_ENTRY("deployment_workflow_model_state_group_t **state_group = <%p>", state_group);

    if ((MCL_NULL != state_group) && (MCL_NULL != *state_group))
    {
        MCL_FREE((*state_group)->name);
        mcl_list_destroy_with_content(&(*state_group)->states, (mcl_list_item_destroy_callback) _destroy_string);
        MCL_FREE(*state_group);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}

static void _destroy_string(char **str)
{
    MCL_FREE(*str);
}
