/*!
 * @file     mcl_deployment_workflow_model_state.h
 * @brief    Header for deployment workflow model state.
 *
 * This module defines a state in a workflow model.
 * 
 * Use #mcl_deployment_workflow_model_state_get_parameter function to get 
 * any parameter of the state in the workflow model.
 *
 * See #mcl_deployment_workflow_model module to get the state of the workflow model.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_H_
#define MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_H_

#include "mcl_deployment/mcl_deployment_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for deployment workflow model state.
 */
typedef struct mcl_deployment_workflow_model_state_t mcl_deployment_workflow_model_state_t;

/**
 * Parameters of deployment workflow model state.
 */
typedef enum E_MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER
{
    MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_NAME,        //!< Name of workflow model state (char*).
    MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_DESCRIPTION, //!< Description of workflow model state (char*).
    MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_INITIAL,     //!< Boolean flag to show if the state is the initial state (mcl_bool_t).
    MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_FINAL,       //!< Boolean flag to show if the state is the final state (mcl_bool_t).
    MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_CANCEL       //!< Boolean flag to show if the state can be cancelled (mcl_bool_t).
} E_MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER;

/**
 * This function is used to get the value of a parameter of the deployment workflow model state.
 * Reference to the parameter is returned and no new memory allocation is performed.
 * Hence, there is no need to free any memory for the returned values.
 *
 * @param [in] state Deployment workflow model state.
 * @param [in] parameter One of the parameters listed in #E_MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER.
 * @param [out] value Value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if the function is called with an unexpected NULL parameter.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_workflow_model_state_get_parameter(mcl_deployment_workflow_model_state_t *state, E_MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER parameter, void **value);

#ifdef  __cplusplus
}
#endif

#endif //MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_H_
