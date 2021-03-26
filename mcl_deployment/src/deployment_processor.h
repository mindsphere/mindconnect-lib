/*!
 * @file     deployment_processor.h
 * @brief    Header for deployment component processor.
 *
 * This module contains the functions for composing the http requests to
 * send to MindSphere endpoints and parse the received http responses.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DEPLOYMENT_PROCESSOR_H_
#define DEPLOYMENT_PROCESSOR_H_

#include "mcl_core/mcl_http_client.h"
#include "deployment_configuration.h"
#include "deployment_workflow_filter.h"
#include "deployment_workflow.h"

/**
 * Handle for deployment processor.
 */
typedef struct deployment_processor_t
{
    char *access_token;
    char *workflow_instances_url;
    mcl_http_client_t *http_client;
} deployment_processor_t;

/**
 * This function lists the deployment workflows defined for the agent.
 *
 * @param [in] deployment_processor Deployment Processor handle to be used.
 * @param [in] filter Filter parameters to filter the list of workflow instances returned from server. Pass MCL_NULL in case no filtering is required.
 * @param [out] workflows List of deployment workflows.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
 * <li>#MCL_STORE_IS_EMPTY if @p store has no data.
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
MCL_LOCAL mcl_error_t deployment_processor_list_workflows(deployment_processor_t *deployment_processor, deployment_workflow_filter_t *filter,
    mcl_list_t **workflows);

/**
 * This function is used to get a deployment workflow's description.
 *
 * @param [in] deployment_processor Deployment Processor handle to be used.
 * @param [in] id ID of the deployment workflow instance.
 * @param [in] filter Filter parameters to filter the content returned from server. Pass MCL_NULL in case no filtering is required.
 * @param [out] workflow Description of deployment workflow.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
 * <li>#MCL_STORE_IS_EMPTY if @p store has no data.
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
MCL_LOCAL mcl_error_t deployment_processor_get_workflow(deployment_processor_t *deployment_processor, char *id, deployment_workflow_filter_t *filter,
    deployment_workflow_t **workflow);

/**
 * This function is used to update the deployment workflow's state.
 *
 * @param [in] deployment_processor Deployment Processor handle to be used.
 * @param [in] id ID of the deployment workflow instance.
 * @param [in] state New state of the deployment workflow.
 * @param [in] filter Filter parameters to filter the content returned from server. Pass MCL_NULL in case no filtering is required.
 * @param [out] workflow Updated description of deployment workflow.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
 * <li>#MCL_STORE_IS_EMPTY if @p store has no data.
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
MCL_LOCAL mcl_error_t deployment_processor_update_workflow(deployment_processor_t *deployment_processor, char *id, deployment_workflow_state_t *state,
    deployment_workflow_filter_t *filter, deployment_workflow_t **workflow);

#endif //DEPLOYMENT_PROCESSOR_H_
