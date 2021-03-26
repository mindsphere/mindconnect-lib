/*!
 * @file     connectivity_processor.h
 * @brief    Connectivity processor module header file for connectivity module.
 *
 * This module contains the functions for composing the http requests to
 * send to MindSphere endpoints and parse the received http responses.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef CONNECTIVITY_PROCESSOR_H_
#define CONNECTIVITY_PROCESSOR_H_

#include "mcl_core/mcl_http_client.h"
#include "connectivity_configuration.h"
#include "store.h"
#include "mapping.h"
#include "data_source_configuration.h"

/**
 * Handle for connectivity processor.
 */
typedef struct connectivity_processor_t
{
    const char *agent_id;
    const char *hostname;
    char *access_token;
    char *exchange_url;
    char *mapping_url;
    mcl_http_client_t *http_client;
    mcl_size_t max_http_payload_size;
} connectivity_processor_t;

/**
 * This function creates and initializes a data struct of #connectivity_processor_t.
 *
 * @param [in] configuration The configuration parameters.
 * @param [out] processor The newly initialized connectivity processor.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t connectivity_processor_initialize(connectivity_configuration_t *configuration, connectivity_processor_t **processor);

/**
 * This function exchanges data with MindSphere.
 *
 * @param [in] connectivity_processor Connectivity Processor handle to be used.
 * @param [in] item Data to be exchanged.
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
 * <li>#MCL_REQUEST_PAYLOAD_TOO_LARGE if the server returns HTTP 413 status code.</li>
 * <li>#MCL_TOO_MANY_REQUESTS if the server returns HTTP 429 status code.</li>
 * <li>#MCL_SERVER_FAIL if the server returns HTTP 500 status code.</li>
 * <li>#MCL_INVALID_PARAMETER in case item has one or more missing mandatory parameter.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t connectivity_processor_exchange(connectivity_processor_t *connectivity_processor, mcl_item_t *item);

/**
 * This function creates a data point mapping.
 *
 * @param [in] connectivity_processor Connectivity Processor handle to be used.
 * @param [in] mapping Mapping handle.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
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
MCL_LOCAL mcl_error_t connectivity_processor_create_mapping(connectivity_processor_t *connectivity_processor, mapping_t *mapping);

/**
 * This function gets data source configuration.
 *
 * @param [in] connectivity_processor Connectivity Processor handle to be used.
 * @param [out] configuration Data source configuration.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
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
MCL_LOCAL mcl_error_t connectivity_processor_get_data_source_configuration(connectivity_processor_t *connectivity_processor,
    data_source_configuration_t **configuration);

/**
 * This function destroys connectivity processor data structure.
 *
 * @param [in] processor Connectivity processor handle to be destroyed.
 */
MCL_LOCAL void connectivity_processor_destroy(connectivity_processor_t **processor);

#endif //CONNECTIVITY_PROCESSOR_H_
