/*!
 * @file     deployment_workflow_model_state.h
 * @brief    Internal header for deployment workflow model state.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DEPLOYMENT_WORKFLOW_MODEL_STATE_H_
#define DEPLOYMENT_WORKFLOW_MODEL_STATE_H_

#include "mcl_deployment/mcl_deployment_common.h"

/**
 * Handle for deployment workflow model state.
 */
typedef struct mcl_deployment_workflow_model_state_t
{
    char *name;
    char *description;
    mcl_bool_t initial;
    mcl_bool_t final;
    mcl_bool_t cancel;
} deployment_workflow_model_state_t;

/**
 * This function initializes a deployment workflow model state.
 *
 * @param [out] state Deployment workflow model state.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t deployment_workflow_model_state_initialize(deployment_workflow_model_state_t **state);

/**
 * This function destroys deployment workflow model state.
 *
 * @param [in] state Deployment workflow model state to destroy.
 */
MCL_LOCAL void deployment_workflow_model_state_destroy(deployment_workflow_model_state_t **state);

#endif //DEPLOYMENT_WORKFLOW_MODEL_STATE_H_
