/*!
 * @file     deployment_workflow_model.h
 * @brief    Internal header for deployment workflow model.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DEPLOYMENT_WORKFLOW_MODEL_H_
#define DEPLOYMENT_WORKFLOW_MODEL_H_

#include "mcl_deployment/mcl_deployment_common.h"
#include "deployment_workflow_model_state.h"
#include "deployment_workflow_model_transition.h"
#include "deployment_workflow_model_state_group.h"
#include "mcl_core/mcl_list.h"

/**
 * Handle for deployment workflow model.
 */
typedef struct mcl_deployment_workflow_model_t
{
    char *key;
    mcl_list_t *states;
    mcl_list_t *transitions;
    mcl_list_t *state_groups;
} deployment_workflow_model_t;

/**
 * This function initializes a deployment workflow model.
 *
 * @param [out] model Deployment workflow model.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t deployment_workflow_model_initialize(deployment_workflow_model_t **model);

/**
 * This function destroys deployment workflow model.
 *
 * @param [in] model Deployment workflow model to destroy.
 */
MCL_LOCAL void deployment_workflow_model_destroy(deployment_workflow_model_t **model);

#endif //DEPLOYMENT_WORKFLOW_MODEL_H_
