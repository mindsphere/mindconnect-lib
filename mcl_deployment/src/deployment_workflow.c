/*!
 * @file     deployment_workflow.c
 * @brief    Implementation of deployment workflow.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_deployment/mcl_deployment_workflow.h"
#include "deployment_workflow.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"
#include "mcl_core/mcl_list.h"

mcl_error_t deployment_workflow_initialize(deployment_workflow_t **workflow)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("deployment_workflow_t **workflow = <%p>", workflow);

    if (MCL_NULL != MCL_NEW(*workflow))
    {
        (*workflow)->id = MCL_NULL;
        (*workflow)->device_id = MCL_NULL;
        (*workflow)->created_at = MCL_NULL;
        (*workflow)->current_state = MCL_NULL;
        (*workflow)->history = MCL_NULL;
        (*workflow)->model = MCL_NULL;
        (*workflow)->data = MCL_NULL;
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    // Error check.
    if (MCL_OK != code)
    {
        mcl_deployment_workflow_destroy(workflow);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_deployment_workflow_get_parameter(mcl_deployment_workflow_t *workflow, E_MCL_DEPLOYMENT_WORKFLOW_PARAMETER parameter, void **value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_deployment_workflow_t *workflow = <%p>, E_MCL_DEPLOYMENT_WORKFLOW_PARAMETER parameter = <%d>, void **value = <%p>",
        workflow, parameter, value);

    MCL_ASSERT_NOT_NULL(workflow, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_DEPLOYMENT_WORKFLOW_PARAMETER_ID:
            *value = workflow->id;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_PARAMETER_DEVICE_ID:
            *value = workflow->device_id;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CREATED_AT:
            *value = workflow->created_at;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CURRENT_STATE:
            *value = workflow->current_state;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_PARAMETER_HISTORY:
            *value = workflow->history;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_PARAMETER_MODEL:
            *value = workflow->model;
            break;
        case MCL_DEPLOYMENT_WORKFLOW_PARAMETER_DATA:
            *value = workflow->data;
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

void mcl_deployment_workflow_destroy(mcl_deployment_workflow_t **workflow)
{
    MCL_DEBUG_ENTRY("mcl_deployment_workflow_t **workflow = <%p>", workflow);

    if ((MCL_NULL != workflow) && (MCL_NULL != *workflow))
    {
        MCL_FREE((*workflow)->id);
        MCL_FREE((*workflow)->device_id);
        MCL_FREE((*workflow)->created_at);
        mcl_deployment_workflow_state_destroy(&(*workflow)->current_state);
        mcl_list_destroy_with_content(&(*workflow)->history, (mcl_list_item_destroy_callback) mcl_deployment_workflow_state_destroy);
        deployment_workflow_model_destroy(&(*workflow)->model);
        mcl_json_util_destroy(&(*workflow)->data);
        MCL_FREE(*workflow);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
