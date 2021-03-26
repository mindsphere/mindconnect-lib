/*!
 * @file     deployment_workflow.h
 * @brief    Internal header for deployment workflow.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DEPLOYMENT_WORKFLOW_H_
#define DEPLOYMENT_WORKFLOW_H_

#include "mcl_deployment/mcl_deployment_common.h"
#include "deployment_workflow_state.h"
#include "deployment_workflow_model.h"
#include "mcl_core/mcl_list.h"

/**
 * Handle for deployment workflow.
 */
typedef struct mcl_deployment_workflow_t
{
    char *id;
    char *device_id;
    char *created_at;
    deployment_workflow_state_t *current_state;
    mcl_list_t *history;
    deployment_workflow_model_t *model;
    mcl_json_t *data;
} deployment_workflow_t;

/**
 * This function initializes a deployment workflow instance.
 *
 * @param [out] workflow Deployment workflow instance.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p workflow is null.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t deployment_workflow_initialize(deployment_workflow_t **workflow);

#endif //DEPLOYMENT_WORKFLOW_H_
