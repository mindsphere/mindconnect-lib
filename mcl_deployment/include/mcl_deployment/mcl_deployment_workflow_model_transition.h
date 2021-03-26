/*!
 * @file     mcl_deployment_workflow_model_transition.h
 * @brief    Header for deployment workflow model transition.
 *
 * This module defines a transition in a workflow model.
 * 
 * Use #mcl_deployment_workflow_model_transition_get_parameter function to get 
 * any parameter of the transition in the workflow model.
 *
 * See #mcl_deployment_workflow_model module to get the transition of the workflow model.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_H_
#define MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_H_

#include "mcl_deployment/mcl_deployment_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for deployment workflow model transition.
 */
typedef struct mcl_deployment_workflow_model_transition_t mcl_deployment_workflow_model_transition_t;

/**
 * Parameters of deployment workflow model transition.
 */
typedef enum E_MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER
{
    MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER_FROM,   //!< State the transition is from (char*).
    MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER_TO,     //!< State the transition is to (char*).
    MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER_TYPE,   //!< Type of the transition (char*).
    MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER_DETAILS //!< Details about the transition (mcl_json_t).
} E_MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER;

/**
 * This function is used to get the value of a parameter of the deployment workflow model transition.
 * Reference to the parameter is returned and no new memory allocation is performed.
 * Hence, there is no need to free any memory for the returned values.
 *
 * @param [in] transition Deployment workflow model transition.
 * @param [in] parameter One of the parameters listed in #E_MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER.
 * @param [out] value Value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if the function is called with an unexpected NULL parameter.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_workflow_model_transition_get_parameter(mcl_deployment_workflow_model_transition_t *transition, E_MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER parameter, void **value);

#ifdef  __cplusplus
}
#endif

#endif //MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_H_
