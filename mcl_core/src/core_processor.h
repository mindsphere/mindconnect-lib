/*!
 * @file     core_processor.h
 * @brief    Core processor module header file.
 *
 * This module contains the functions for composing the http requests to
 * send to MindSphere endpoints and parse the received http responses.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef CORE_PROCESSOR_H_
#define CORE_PROCESSOR_H_

#include "mcl_core/mcl_http_client.h"
#include "security_handler.h"
#include "core_configuration.h"

/**
 *  Handle for core processor.
 */
typedef struct core_processor_t
{
    core_configuration_t *configuration;       //!< Configuration parameters.
    security_handler_t *security_handler;      //!< Security handler.
    mcl_http_client_t *http_client;            //!< Http client handle.
} core_processor_t;

/**
 * This function initializes core processor.
 *
 * @param [in] configuration The configuration parameters to be used for initialization.
 * @param [out] core_processor The newly initialized core processor.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
 * <li>#MCL_NO_ACCESS_TOKEN_PROVIDED if neither initial access token nor loading/saving credentials callback functions are provided.
 * </ul>
 */
MCL_LOCAL mcl_error_t core_processor_initialize(core_configuration_t *configuration, core_processor_t **core_processor);

/**
 * This function is called by #mcl_core_onboard and #mcl_core_rotate_key. @see mcl_core_onboard and @see mcl_core_rotate_key for details.
 *
 * @param [in] core_processor Core processor handle to be used.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_PROXY if the proxy host name can not be resolved.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_HOST if the remote host name can not be resolved.</li>
 * <li>#MCL_SSL_HANDSHAKE_FAIL if a problem occurs during SSL handshake.</li>
 * <li>#MCL_COULD_NOT_CONNECT if there is a connection error with the server.</li>
 * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED if the server can not be verified.</li>
 * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
 * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
 * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
 * <li>#MCL_UNAUTHORIZED if response status code of server is related to authorization.</li>
 * <li>#MCL_SERVER_FAIL if the the server returns 500 response status code.</li>
 * <li>#MCL_CREDENTIALS_NOT_SAVED if credentials can not be saved.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t core_processor_register(core_processor_t *core_processor);

/**
 * This function is called by #mcl_core_update_credentials. @see mcl_core_update_credentials for details.
 *
 * @param [in] core_processor Core processor handle to be used.
 * @return
 * <ul>
 * <li>#MCL_OK in case of successful update.</li>
 * <li>#MCL_CREDENTIALS_UP_TO_DATE if credentials of the mcl_core is already up to date.
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t core_processor_update_credentials(core_processor_t *core_processor);

/**
 * This function is called by #mcl_core_get_access_token. @see mcl_core_get_access_token for details.
 *
 * @param [in] core_processor Core processor handle to be used.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_PROXY if the proxy host name can not be resolved.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_HOST if the remote host name can not be resolved.</li>
 * <li>#MCL_SSL_HANDSHAKE_FAIL if a problem occurs during SSL handshake.</li>
 * <li>#MCL_COULD_NOT_CONNECT if there is a connection error with the server.</li>
 * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED if the server can not be verified.</li>
 * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
 * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
 * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
 * <li>#MCL_BAD_REQUEST if response status code of server is 400.</li>
 * <li>#MCL_UNAUTHORIZED if response status code of server is 401.</li>
 * <li>#MCL_SERVER_FAIL if the response status code of server is 500.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t core_processor_get_access_token(core_processor_t *core_processor);

/**
 * This function destroys the core processor handle.
 *
 * @param [in] core_processor Core processor handle to be destroyed.
 */
MCL_LOCAL void core_processor_destroy(core_processor_t **core_processor);

#endif //CORE_PROCESSOR_H_
