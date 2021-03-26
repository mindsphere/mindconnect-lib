/*!
 * @file     deployment_workflow_state.h
 * @brief    Internal header for deployment workflow state.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DEPLOYMENT_WORKFLOW_STATE_H_
#define DEPLOYMENT_WORKFLOW_STATE_H_

#include "mcl_deployment/mcl_deployment_common.h"
#include "mcl_core/mcl_json_util.h"

/**
 * Handle for deployment workflow state.
 */
typedef struct mcl_deployment_workflow_state_t
{
    char *state;
    double progress;
    char *entered;
    char *updated;
    char *message;
    mcl_json_t *details;
} deployment_workflow_state_t;

/**
 * This function initializes a deployment workflow state instance.
 *
 * @param [out] workflow_state Deployment workflow state instance.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t deployment_workflow_state_initialize(deployment_workflow_state_t **workflow_state);

/**
 * This function validates a deployment workflow state.
 *
 * @param [in] workflow_state Deployment workflow state instance.
 * @return
 * <ul>
 * <li>#MCL_OK in case validity.</li>
 * <li>#MCL_INVALID_PARAMETER in case of an invalid or missing parameter.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t deployment_workflow_state_validate(deployment_workflow_state_t *workflow_state);

#endif //DEPLOYMENT_WORKFLOW_STATE_H_
