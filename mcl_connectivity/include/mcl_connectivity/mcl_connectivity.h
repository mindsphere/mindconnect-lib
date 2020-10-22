/*!
 * @file     mcl_connectivity.h
 * @brief    MCL Connectivity interface header file.
 *
 * This module presents MindConnect Library Connectivity capabilities.
 *
 * Make sure MCL_CONNECTIVITY build option is set to ON when building MCL.
 * 
 * Agents have to initialize an instance of MCL connectivity component using 
 * #mcl_connectivity_initialize function to exploit connectivity component features. 
 * 
 * Data can be uploaded to MindSphere using #mcl_connectivity_exchange function.
 * 
 * Agents can create mappings between their data points defined in their data source configuration 
 * and assets using #mcl_connectivity_create_mapping function.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_CONNECTIVITY_H_
#define MCL_CONNECTIVITY_H_

#include "mcl_core/mcl_core.h"
#include "mcl_connectivity/mcl_store.h"
#include "mcl_connectivity/mcl_event.h"
#include "mcl_connectivity/mcl_data_source_configuration.h"
#include "mcl_connectivity/mcl_timeseries.h"
#include "mcl_connectivity/mcl_file.h"
#include "mcl_connectivity/mcl_custom_data.h"
#include "mcl_connectivity/mcl_connectivity_configuration.h"
#include "mcl_connectivity/mcl_mapping.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * This is the main handle for connectivity component.
 */
typedef struct mcl_connectivity_t mcl_connectivity_t;

/**
 * This function creates and initializes an object of type #mcl_connectivity_t.
 * Memory is allocated for the @p connectivity object and @p configuration
 * is referenced by the @p connectivity object.
 *
 * @param [in] configuration Preinitialized #mcl_connectivity_configuration_t object.
 * @param [out] connectivity Connectivity handle which is going to be initialized.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p core or @p connectivity is null.</li>
 * <li>#MCL_INVALID_PARAMETER in case one of mandatory fields of @p configuration is not set.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_connectivity_initialize(mcl_connectivity_configuration_t *configuration, mcl_connectivity_t **connectivity);

/**
 * This function exchanges data in @p item to MindSphere.
 * Item can be either #mcl_store_t which contains multiple data types or single data type
 * such as #mcl_event_t, #mcl_data_source_configuration_t, #mcl_timeseries_t, #mcl_file_t and #mcl_custom_data_t.
 * @p connectivity has to be initialized before using this function.
 *
 * @param [in] connectivity Main handle for connectivity component.
 * @param [in] item The data to be uploaded to MindSphere.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any input parameter is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_NOT_ONBOARDED if the agent is not onboarded using core passed to @p connectivity.</li>
 * <li>#MCL_NO_ACCESS_TOKEN_EXISTS if core passed to @p connectivity does not have an access token.</li>
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
 * <li>#MCL_REQUEST_PAYLOAD_TOO_LARGE if the server returns HTTP 413 status code.</li>
 * <li>#MCL_TOO_MANY_REQUESTS if the server returns HTTP 429 status code.</li>
 * <li>#MCL_SERVER_FAIL if the server returns HTTP 500 status code.</li>
 * <li>#MCL_INVALID_PARAMETER in case item has one or more missing mandatory parameter.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_connectivity_exchange(mcl_connectivity_t *connectivity, void *item);

/**
 * This function creates a data point mapping.
 * @p connectivity has to be initialized before using this function.
 *
 * @param [in] connectivity Main handle for connectivity component.
 * @param [in] mapping Mapping to be created.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any input parameter is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_NOT_ONBOARDED if the agent is not onboarded using core passed to @p connectivity.</li>
 * <li>#MCL_NO_ACCESS_TOKEN_EXISTS if core passed to @p connectivity does not have an access token.</li>
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
 * <li>#MCL_INVALID_PARAMETER in case mapping has one or more missing mandatory parameter.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_connectivity_create_mapping(mcl_connectivity_t *connectivity, mcl_mapping_t *mapping);

/**
 * This function gets data source configuration from MindSphere.
 * @p connectivity has to be initialized before using this function.
 *
 * @param [in] connectivity Main handle for connectivity component.
 * @param [out] configuration Data source configuration.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any input parameter is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_NOT_ONBOARDED if the agent is not onboarded using core passed to @p connectivity.</li>
 * <li>#MCL_NO_ACCESS_TOKEN_EXISTS if core passed to @p connectivity does not have an access token.</li>
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
 * <li>#MCL_INVALID_PARAMETER in case mapping has one or more missing mandatory parameter.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_connectivity_get_data_source_configuration(mcl_connectivity_t *connectivity, mcl_data_source_configuration_t **configuration);

/**
 * This function destroys the #mcl_connectivity_t object and frees any memory allocated.
 *
 * @param [in] connectivity Preinitialized #mcl_connectivity_t object to destroy.
 */
extern MCL_CONNECTIVITY_EXPORT void mcl_connectivity_destroy(mcl_connectivity_t **connectivity);

#ifdef  __cplusplus
}
#endif

#endif //MCL_CONNECTIVITY_H_
