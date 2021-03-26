/*!
 * @file     deployment_workflow_model_state_group.h
 * @brief    Internal header for deployment workflow model.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_H_
#define DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_H_

#include "mcl_deployment/mcl_deployment_common.h"
#include "mcl_core/mcl_list.h"

/**
 * Handle for deployment workflow model state group.
 */
typedef struct mcl_deployment_workflow_model_state_group_t
{
    char *name;
    mcl_list_t *states;
} deployment_workflow_model_state_group_t;

/**
 * This function initializes a deployment workflow model state group.
 *
 * @param [out] state_group Deployment workflow model state group.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t deployment_workflow_model_state_group_initialize(deployment_workflow_model_state_group_t **state_group);

/**
 * This function destroys deployment workflow model state group.
 *
 * @param [in] state_group Deployment workflow model state group to destroy.
 */
MCL_LOCAL void deployment_workflow_model_state_group_destroy(deployment_workflow_model_state_group_t **state_group);

#endif //DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_H_
