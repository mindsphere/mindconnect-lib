/*!
 * @file     mcl_deployment_workflow.h
 * @brief    Header for deployment workflow.
 *
 * This module defines the deployment workflow structure.
 * 
 * Use #mcl_deployment_workflow_get_parameter function to get any paramater of the workflow returned 
 * in the response from MindSphere.
 * 
 * See #mcl_deployment module to get the deployment workflow from MindSphere.
 * 
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DEPLOYMENT_WORKFLOW_H_
#define MCL_DEPLOYMENT_WORKFLOW_H_

#include "mcl_deployment/mcl_deployment_common.h"
#include "mcl_deployment/mcl_deployment_workflow_state.h"
#include "mcl_deployment/mcl_deployment_workflow_model.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for deployment workflow.
 */
typedef struct mcl_deployment_workflow_t mcl_deployment_workflow_t;

/**
 * Parameters of deployment workflow.
 */
typedef enum E_MCL_DEPLOYMENT_WORKFLOW_PARAMETER
{
    MCL_DEPLOYMENT_WORKFLOW_PARAMETER_ID,            //!< Workflow id as char*.
    MCL_DEPLOYMENT_WORKFLOW_PARAMETER_DEVICE_ID,     //!< Device id as char*.
    MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CREATED_AT,    //!< Time when the workflow is created as char*.
    MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CURRENT_STATE, //!< Current state of the workflow as mcl_deployment_workflow_state_t*.
    MCL_DEPLOYMENT_WORKFLOW_PARAMETER_HISTORY,       //!< History of workflow states as mcl_list_t* of mcl_deployment_workflow_state_t*.
    MCL_DEPLOYMENT_WORKFLOW_PARAMETER_MODEL,         //!< Model of the workflow as mcl_deployment_workflow_model_t*.
    MCL_DEPLOYMENT_WORKFLOW_PARAMETER_DATA           //!< User defined data of the workflow as mcl_json_t*.
} E_MCL_DEPLOYMENT_WORKFLOW_PARAMETER;

/**
 * This function is used to get the value of a parameter of the deployment workflow.
 * Reference to the parameter is returned and no new memory allocation is performed.
 * Hence, there is no need to free any memory for the returned values.
 * 
 * @param [in] workflow Deployment workflow instance.
 * @param [in] parameter One of the parameters listed in #E_MCL_DEPLOYMENT_WORKFLOW_PARAMETER.
 * @param [out] value Value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p workflow is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter or @ value is invalid.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_workflow_get_parameter(mcl_deployment_workflow_t *workflow, E_MCL_DEPLOYMENT_WORKFLOW_PARAMETER parameter, void **value);

/**
 * This function destroys deployment workflow instance.
 *
 * @param [in] workflow Deployment workflow instance to destroy.
 */
extern MCL_DEPLOYMENT_EXPORT void mcl_deployment_workflow_destroy(mcl_deployment_workflow_t **workflow);

#ifdef  __cplusplus
}
#endif

#endif //MCL_DEPLOYMENT_WORKFLOW_H_
