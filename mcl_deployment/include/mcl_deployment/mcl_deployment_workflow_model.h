/*!
 * @file     mcl_deployment_workflow_model.h
 * @brief    Header for deployment workflow model.
 *
 * This module defines the workflow model.
 * 
 * Use #mcl_deployment_workflow_model_get_parameter function to get any parameter of the workflow model.
 *
 * See #mcl_deployment_workflow module to get the model of the workflow.
 * 
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DEPLOYMENT_WORKFLOW_MODEL_H_
#define MCL_DEPLOYMENT_WORKFLOW_MODEL_H_

#include "mcl_deployment/mcl_deployment_common.h"
#include "mcl_deployment/mcl_deployment_workflow_model_state.h"
#include "mcl_deployment/mcl_deployment_workflow_model_transition.h"
#include "mcl_deployment/mcl_deployment_workflow_model_state_group.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for deployment workflow model.
 */
typedef struct mcl_deployment_workflow_model_t mcl_deployment_workflow_model_t;

/**
 * Parameters of deployment workflow model.
 */
typedef enum E_MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER
{
    MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER_KEY,          //!< Key of the workflow model (char*).
    MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER_STATES,       //!< List of states in workflow model (mcl_list_t* of mcl_deployment_workflow_model_state*).
    MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER_TRANSITIONS,  //!< List of transitions in workflow model (mcl_list_t* of mcl_deployment_workflow_model_transition*).
    MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER_STATE_GROUPS  //!< List of state groups in workflow model (mcl_list_t* of mcl_deployment_workflow_model_state_group*).
} E_MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER;

/**
 * This function is used to get the value of a parameter of the deployment workflow model.
 * Reference to the parameter is returned and no new memory allocation is performed.
 * Hence, there is no need to free any memory for the returned values.
 * 
 * @param [in] model Deployment workflow model.
 * @param [in] parameter One of the parameters listed in #E_MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER.
 * @param [out] value Value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if the function is called with an unexpected NULL parameter.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_workflow_model_get_parameter(mcl_deployment_workflow_model_t *model, E_MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER parameter, void **value);

#ifdef  __cplusplus
}
#endif

#endif //MCL_DEPLOYMENT_WORKFLOW_MODEL_H_
