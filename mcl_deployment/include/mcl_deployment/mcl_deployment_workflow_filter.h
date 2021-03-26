/*!
 * @file     mcl_deployment_workflow_filter.h
 * @brief    Header for deployment workflow filter.
 *
 * This module defines the filter used to filter the response from MindSphere to 
 * #mcl_deployment_list_workflows, #mcl_deployment_get_workflow and #mcl_deployment_update_workflow functions.
 * See #E_MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER for possible filter parameters.
 * 
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DEPLOYMENT_WORKFLOW_FILTER_H_
#define MCL_DEPLOYMENT_WORKFLOW_FILTER_H_

#include "mcl_deployment/mcl_deployment_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for deployment workflow filter.
 */
typedef struct mcl_deployment_workflow_filter_t mcl_deployment_workflow_filter_t;

/**
 * Parameters of deployment workflow filter.
 * The response from MindSphere will be filtered based on these parameters.
 */
typedef enum E_MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER
{
    MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_MODEL,         //!< Boolean flag to include the model used for the workflow instance.
    MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_HISTORY,       //!< Boolean flag to include the transition history of the workflow instance.
    MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_CURRENT_STATE, //!< Include workflow instances based on the current state value (char*). Only valid for mcl_deployment_list_workflows function.
    MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_GROUP,         //!< Include workflow instances based on the state group (char*) they are in. Only valid for mcl_deployment_list_workflows function.
    MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_DEVICE_ID,     //!< Include workflow instances belonging to a specific device with device id (char*). Only valid for mcl_deployment_list_workflows function.
    MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_MODEL_KEY      //!< Include workflow instances that are created from a given model key (char*). Only valid for mcl_deployment_list_workflows function.
} E_MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER;

/**
 * This function initializes a deployment workflow filter instance.
 *
 * @param [out] filter Filter for the deployment workflow instance.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if the function is called with an unexpected NULL parameter.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_workflow_filter_initialize(mcl_deployment_workflow_filter_t **filter);

/**
 * This function is used to set the value of a parameter of the deployment workflow filter.
 *
 * @param [in] filter Deployment workflow filter instance.
 * @param [in] parameter One of the parameters listed in #E_MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER.
 * @param [in] value Value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if the function is called with an unexpected NULL parameter.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_workflow_filter_set_parameter(mcl_deployment_workflow_filter_t *filter, E_MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER parameter, const void *value);

/**
 * This function destroys deployment workflow filter instance.
 *
 * @param [in] filter Deployment workflow filter instance to destroy.
 */
extern MCL_DEPLOYMENT_EXPORT void mcl_deployment_workflow_filter_destroy(mcl_deployment_workflow_filter_t **filter);

#ifdef  __cplusplus
}
#endif

#endif //MCL_DEPLOYMENT__WORKFLOW_FILTER_H_
