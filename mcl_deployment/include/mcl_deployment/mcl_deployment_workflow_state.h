/*!
 * @file     mcl_deployment_workflow_state.h
 * @brief    Header for deployment workflow state.
 *
 * This module defines the state of the workflow.
 *
 * Use #mcl_deployment_workflow_state_get_parameter function to get any parameter of the current state
 * of workflow returned in the response from MindSphere.
 *
 * See #mcl_deployment_workflow module to get the current state of the workflow.
 *
 * To update the current state of a workflow, initialize a new state with
 * #mcl_deployment_workflow_state_initialize function and set the parameters of the current state
 * with #mcl_deployment_workflow_state_set_parameter function.
 *
 * See #mcl_deployment module to update the state of the workflow.
 * 
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DEPLOYMENT_WORKFLOW_STATE_H_
#define MCL_DEPLOYMENT_WORKFLOW_STATE_H_

#include "mcl_deployment/mcl_deployment_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for deployment workflow state.
 */
typedef struct mcl_deployment_workflow_state_t mcl_deployment_workflow_state_t;

/**
 * Parameters of deployment workflow state.
 */
typedef enum E_MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER
{
    MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE,    //!< State of the workflow (char*).
    MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_PROGRESS, //!< Progress of the state (double*).
    MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_ENTERED,  //!< Date and time when the state was first entered (char*). Read only parameter.
    MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_UPDATED,  //!< Date and time when the state was last updated (char*). Read only parameter.
    MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_MESSAGE,  //!< Free text message from the device about the state (char*).
    MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_DETAILS   //!< Arbitrary block of json data for additional information (mcl_json_t*). Optional to set.
} E_MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER;

/**
 * This function initializes a deployment workflow state instance.
 *
 * @param [out] workflow_state Deployment workflow state instance.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if the function is called with an unexpected NULL parameter.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_workflow_state_initialize(mcl_deployment_workflow_state_t **workflow_state);

/**
 * This function is used to set the value of a parameter of the deployment workflow state.
 *
 * @param [in] workflow_state Deployment workflow state instance.
 * @param [in] parameter One of the parameters listed in #E_MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER.
 * @param [in] value Value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if the function is called with an unexpected NULL parameter.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_workflow_state_set_parameter(mcl_deployment_workflow_state_t *workflow_state, E_MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER parameter, const void *value);

/**
 * This function is used to get the value of a parameter of the deployment workflow state.
 * Reference to the parameter is returned and no new memory allocation is performed.
 * Hence, there is no need to free any memory for the returned values.
 *
 * @param [in] workflow_state Deployment workflow state instance.
 * @param [in] parameter One of the parameters listed in #E_MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER.
 * @param [out] value Value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if the function is called with an unexpected NULL parameter.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_workflow_state_get_parameter(mcl_deployment_workflow_state_t *workflow_state, E_MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER parameter, void **value);

/**
 * This function destroys deployment workflow state instance.
 *
 * @param [in] workflow Deployment workflow state instance to destroy.
 */
extern MCL_DEPLOYMENT_EXPORT void mcl_deployment_workflow_state_destroy(mcl_deployment_workflow_state_t **workflow_state);

#ifdef  __cplusplus
}
#endif

#endif //MCL_DEPLOYMENT_WORKFLOW_STATE_H_
