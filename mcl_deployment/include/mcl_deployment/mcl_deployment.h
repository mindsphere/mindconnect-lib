/*!
 * @file     mcl_deployment.h
 * @brief    MCL deployment interface header file.
 *
 * This is the main header for MCL Deployment component.
 * 
 * MCL Deployment component presents functions for agents to access Deployment Workflow API of MindSphere.
 *
 * Make sure MCL_DEPLOYMENT build option is set to ON when building MCL.
 * 
 * Agents have to initialize an instance of MCL deployment component using 
 * #mcl_deployment_initialize function to exploit deployment component features. 
 * 
 * Deployment workflows defined for the agent can be listed using 
 * #mcl_deployment_list_workflows function.
 * 
 * Agents can get the description of a deployment workflow using  
 * #mcl_deployment_get_workflow function and update the state of the workflow using 
 * #mcl_deployment_update_workflow function.
 * 
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DEPLOYMENT_H_
#define MCL_DEPLOYMENT_H_

#include "mcl_core/mcl_core.h"
#include "mcl_core/mcl_list.h"
#include "mcl_deployment/mcl_deployment_configuration.h"
#include "mcl_deployment/mcl_deployment_workflow_filter.h"
#include "mcl_deployment/mcl_deployment_workflow.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * This is the main handle for deployment component.
 */
typedef struct mcl_deployment_t mcl_deployment_t;

/**
 * This function creates and initializes an object of type #mcl_deployment_t.
 * Memory is allocated for the @p deployment object and @p configuration
 * is referenced by the @p deployment object.
 *
 * @param [in] configuration Preinitialized #mcl_deployment_configuration_t object.
 * @param [out] deployment Deployment handle which is going to be initialized.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p configuration or @p deployment is null.</li>
 * <li>#MCL_INVALID_PARAMETER in case one of mandatory fields of @p configuration is not set.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_initialize(mcl_deployment_configuration_t *configuration, mcl_deployment_t **deployment);

/**
 * This function lists the deployment workflows defined for the agent.
 * @p deployment has to be initialized before using this function.
 *
 * @param [in] deployment Main handle for deployment component.
 * @param [in] filter Filter parameters to filter the list of workflow instances returned from server. Pass MCL_NULL in case no filtering is required.
 * @param [out] workflows List of deployment workflows.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any input parameter is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_NOT_ONBOARDED if the agent is not onboarded.</li>
 * <li>#MCL_NO_ACCESS_TOKEN_EXISTS if no access token exists.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_PROXY in case the proxy host name can not be resolved.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_HOST in case the remote host name can not be resolved.</li>
 * <li>#MCL_SSL_HANDSHAKE_FAIL in case a problem occurs during SSL handshake.</li>
 * <li>#MCL_COULD_NOT_CONNECT in case there is a connection error.</li>
 * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED in case the server can not be verified.</li>
 * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
 * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
 * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
 * <li>#MCL_BAD_REQUEST if the server returns HTTP 400 status code.</li>
 * <li>#MCL_UNAUTHORIZED if the server returns HTTP 401 status code.</li>
 * <li>#MCL_FORBIDDEN if the server returns HTTP 403 status code.</li>
 * <li>#MCL_TOO_MANY_REQUESTS if the server returns HTTP 429 status code.</li>
 * <li>#MCL_SERVER_FAIL if the server returns HTTP 500 status code.</li>
 * <li>#MCL_INVALID_PARAMETER in case item has one or more missing mandatory parameter.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_list_workflows(mcl_deployment_t *deployment, mcl_deployment_workflow_filter_t *filter, mcl_list_t **workflows);

/**
 * This function is used to get a deployment workflow's description given the id of the workflow.
 * @p deployment has to be initialized before using this function.
 *
 * @param [in] deployment Main handle for deployment component.
 * @param [in] id ID of the deployment workflow instance.
 * @param [in] filter Filter parameters to filter what is to be included in the workflow description returned from server. Pass MCL_NULL in case no filtering is required.
 * @param [out] workflow Description of deployment workflow.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any input parameter is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_NOT_ONBOARDED if the agent is not onboarded.</li>
 * <li>#MCL_NO_ACCESS_TOKEN_EXISTS if no access token exists.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_PROXY in case the proxy host name can not be resolved.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_HOST in case the remote host name can not be resolved.</li>
 * <li>#MCL_SSL_HANDSHAKE_FAIL in case a problem occurs during SSL handshake.</li>
 * <li>#MCL_COULD_NOT_CONNECT in case there is a connection error.</li>
 * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED in case the server can not be verified.</li>
 * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
 * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
 * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
 * <li>#MCL_BAD_REQUEST if the server returns HTTP 400 status code.</li>
 * <li>#MCL_UNAUTHORIZED if the server returns HTTP 401 status code.</li>
 * <li>#MCL_FORBIDDEN if the server returns HTTP 403 status code.</li>
 * <li>#MCL_TOO_MANY_REQUESTS if the server returns HTTP 429 status code.</li>
 * <li>#MCL_SERVER_FAIL if the server returns HTTP 500 status code.</li>
 * <li>#MCL_INVALID_PARAMETER in case item has one or more missing mandatory parameter.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_get_workflow(mcl_deployment_t *deployment, char *id, mcl_deployment_workflow_filter_t *filter, mcl_deployment_workflow_t **workflow);

/**
 * This function is used to update a deployment workflow's state given the id of the workflow.
 * @p deployment has to be initialized before using this function.
 *
 * @param [in] deployment Main handle for deployment component.
 * @param [in] id ID of the deployment workflow instance.
 * @param [in] state New state of the deployment workflow.
 * @param [in] filter Filter parameters to filter what is to be included in the workflow description returned from server. Pass MCL_NULL in case no filtering is required.
 * @param [out] workflow Updated description of deployment workflow.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any input parameter is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_NOT_ONBOARDED if the agent is not onboarded.</li>
 * <li>#MCL_NO_ACCESS_TOKEN_EXISTS if no access token exists.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_PROXY in case the proxy host name can not be resolved.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_HOST in case the remote host name can not be resolved.</li>
 * <li>#MCL_SSL_HANDSHAKE_FAIL in case a problem occurs during SSL handshake.</li>
 * <li>#MCL_COULD_NOT_CONNECT in case there is a connection error.</li>
 * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED in case the server can not be verified.</li>
 * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
 * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
 * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
 * <li>#MCL_BAD_REQUEST if the server returns HTTP 400 status code.</li>
 * <li>#MCL_UNAUTHORIZED if the server returns HTTP 401 status code.</li>
 * <li>#MCL_FORBIDDEN if the server returns HTTP 403 status code.</li>
 * <li>#MCL_TOO_MANY_REQUESTS if the server returns HTTP 429 status code.</li>
 * <li>#MCL_SERVER_FAIL if the server returns HTTP 500 status code.</li>
 * <li>#MCL_INVALID_PARAMETER in case item has one or more missing mandatory parameter.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_update_workflow(mcl_deployment_t *deployment, char *id, mcl_deployment_workflow_state_t *state, mcl_deployment_workflow_filter_t *filter, mcl_deployment_workflow_t **workflow);

/**
 * This function destroys the #mcl_deployment_t object and frees any memory allocated.
 *
 * @param [in] deployment #mcl_deployment_t object to destroy.
 */
extern MCL_DEPLOYMENT_EXPORT void mcl_deployment_destroy(mcl_deployment_t **deployment);

#ifdef  __cplusplus
}
#endif

#endif //MCL_DEPLOYMENT_H_
