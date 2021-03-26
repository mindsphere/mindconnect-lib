/*!
 * @file     deployment_workflow_model_transition.h
 * @brief    Internal header for deployment workflow model transition.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_H_
#define DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_H_

#include "mcl_deployment/mcl_deployment_common.h"
#include "mcl_core/mcl_json_util.h"

/**
 * Handle for deployment workflow model transition.
 */
typedef struct mcl_deployment_workflow_model_transition_t
{
    char *from;
    char *to;
    char *type;
    mcl_json_t *details;
} deployment_workflow_model_transition_t;

/**
 * This function initializes a deployment workflow model transition.
 *
 * @param [out] transition Deployment workflow model transition.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t deployment_workflow_model_transition_initialize(deployment_workflow_model_transition_t **transition);

/**
 * This function destroys deployment workflow model transition.
 *
 * @param [in] transition Deployment workflow model transition to destroy.
 */
MCL_LOCAL void deployment_workflow_model_transition_destroy(deployment_workflow_model_transition_t **transition);

#endif //DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_H_
