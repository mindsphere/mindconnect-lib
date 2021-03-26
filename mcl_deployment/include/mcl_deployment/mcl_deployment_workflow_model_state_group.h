/*!
 * @file     mcl_deployment_workflow_model_state_group.h
 * @brief    Header for deployment workflow model state group.
 *
 * This module defines a state group in a workflow model.
 * 
 * Use #mcl_deployment_workflow_model_state_group_get_parameter function to get 
 * any parameter of the state group in the workflow model.
 *
 * See #mcl_deployment_workflow_model module to get the state groups of the workflow model.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_H_
#define MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_H_

#include "mcl_deployment/mcl_deployment_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for deployment workflow model state group.
 */
typedef struct mcl_deployment_workflow_model_state_group_t mcl_deployment_workflow_model_state_group_t;

/**
 * Parameters of deployment workflow model state group.
 */
typedef enum E_MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_PARAMETER
{
    MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_PARAMETER_NAME,  //!< Name of workflow model state group (char*).
    MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_PARAMETER_STATES //!< List of state names in the state group (mcl_list_t of char*).
} E_MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_PARAMETER;

/**
 * This function is used to get the value of a parameter of the deployment workflow model state group.
 * Reference to the parameter is returned and no new memory allocation is performed.
 * Hence, there is no need to free any memory for the returned values.
 *
 * @param [in] state_group Deployment workflow model state group.
 * @param [in] parameter One of the parameters listed in #E_MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_PARAMETER.
 * @param [out] value Value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if the function is called with an unexpected NULL parameter.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_workflow_model_state_group_get_parameter(mcl_deployment_workflow_model_state_group_t *state_group, E_MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_PARAMETER parameter, void **value);

#ifdef  __cplusplus
}
#endif

#endif //MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_H_
