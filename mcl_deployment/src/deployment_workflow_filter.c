/*!
 * @file     deployment_workflow_filter.c
 * @brief    Implementation of deployment workflow filter.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_deployment/mcl_deployment_workflow_filter.h"
#include "deployment_workflow_filter.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

mcl_error_t mcl_deployment_workflow_filter_initialize(mcl_deployment_workflow_filter_t **filter)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_deployment_workflow_filter_t **filter = <%p>", filter);

    MCL_ASSERT_NOT_NULL(filter, code);

    if (MCL_NULL != MCL_NEW(*filter))
    {
        (*filter)->model = MCL_FALSE;
        (*filter)->history = MCL_FALSE;
        (*filter)->current_state = MCL_NULL;
        (*filter)->group = MCL_NULL;
        (*filter)->device_id = MCL_NULL;
        (*filter)->model_key = MCL_NULL;
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_deployment_workflow_filter_set_parameter(mcl_deployment_workflow_filter_t *filter,
    E_MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER parameter,const void *value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_deployment_workflow_filter_t *filter = <%p>, E_MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER parameter = <%d>, const void *value = <%p>",
        filter, parameter, value);

    MCL_ASSERT_NOT_NULL(filter, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_MODEL:
            filter->model =  *((mcl_bool_t*) value);
            break;
        case MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_HISTORY:
            filter->history = *((mcl_bool_t*) value);
            break;
        case MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_CURRENT_STATE:
            code = mcl_string_util_reset(value, &filter->current_state);
            break;
        case MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_GROUP:
            code = mcl_string_util_reset(value, &filter->group);
            break;
        case MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_DEVICE_ID:
            code = mcl_string_util_reset(value, &filter->device_id);
            break;
        case MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_MODEL_KEY:
            code = mcl_string_util_reset(value, &filter->model_key);
            break;
        default:
            code = MCL_INVALID_PARAMETER;
            break;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_deployment_workflow_filter_destroy(mcl_deployment_workflow_filter_t **filter)
{
    MCL_DEBUG_ENTRY("mcl_deployment_workflow_filter_t **filter = <%p>", filter);

    if ((MCL_NULL != filter) && (MCL_NULL != *filter))
    {
        MCL_FREE((*filter)->current_state);
        MCL_FREE((*filter)->group);
        MCL_FREE((*filter)->device_id);
        MCL_FREE((*filter)->model_key);
        MCL_FREE(*filter);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
