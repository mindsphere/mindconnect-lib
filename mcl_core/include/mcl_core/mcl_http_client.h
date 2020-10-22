/*!
 * @file     mcl_http_client.h
 * @brief    HTTP client interface header file.
 *
 * MCL presents an HTTP client interface for which different implementations can be provided.
 * This module is used internally by other MCL modules though it can also be used by the agent
 * as a generic HTTP client. Agent can initialize an HTTP client with #mcl_http_client_initialize
 * function based on the configuration in #mcl_http_client_configuration_t data structure.
 * Following initialization, agent can send HTTP requests (#mcl_http_request_t) and receive
 * HTTP responses (#mcl_http_response_t) using #mcl_http_client_send function. Agent is expected
 * to destroy the HTTP client when it is no longer needed using #mcl_http_client_destroy function.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_HTTP_CLIENT_H_
#define MCL_HTTP_CLIENT_H_

#include "mcl_core/mcl_core_common.h"
#include "mcl_core/mcl_http_response.h"
#include "mcl_core/mcl_http_request.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * HTTP client handle.
 */
typedef struct mcl_http_client_t mcl_http_client_t;

/**
 * This data structure holds the configuration parameters which are used to initialize #mcl_http_client_t data structure.
 */
typedef struct mcl_http_client_configuration_t
{
    const char *certificate;                    //!< Certificate. If it is NULL, default CA certificate store will be used (if available).
    mcl_bool_t certificate_is_file;             //!< Flag to check if certificate is given as file or string.
    const char *proxy_hostname;                 //!< Proxy hostname. Optional.
    const char *proxy_username;                 //!< Proxy username. Optional if proxy host name is set, ineffective otherwise.
    const char *proxy_password;                 //!< Proxy password. Mandatory if proxy host name and proxy username are set, ineffective otherwise.
    const char *proxy_domain;                   //!< Proxy domain. Optional if proxy host name and proxy username are set, ineffective otherwise.
    const char *user_agent;                     //!< User agent.
    mcl_uint32_t http_request_timeout;          //!< Timeout value (in seconds) for HTTP requests. Default timeout is 300 seconds.
    mcl_uint16_t port;                          //!< Port number.
    mcl_uint16_t proxy_port;                    //!< Proxy port number. Mandatory if proxy host name is set, ineffective otherwise.
    E_MCL_PROXY proxy_type;                     //!< Proxy type #E_MCL_PROXY. Mandatory if proxy host name is set, ineffective otherwise.
} mcl_http_client_configuration_t;

/**
 * This function initializes the underlying implementation (like libcurl) with the given configuration data.
 *
 * @param [in] configuration Pointer to preinitialized #mcl_http_client_configuration_t struct.
 * @param [out] http_client Handle for the http client initialized.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case input parameter is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_http_client_initialize(mcl_http_client_configuration_t *configuration, mcl_http_client_t **http_client);

/**
 * This function adds server certificate to certificate list of http client. The certificate list is used by the http client to identify server(s).
 *
 * @param [in] http_client HTTP client handle.
 * @param [in] certificate Certificate.
 * @param [in] is_file MCL_TRUE if certificate is given as file.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_http_client_add_certificate(mcl_http_client_t *http_client, const char *certificate, mcl_bool_t is_file);

/**
 * This function sends the given request to the pre-configured destination and returns the response using underlying implementation.
 * It is blocking until response is received or timeout occurred.
 *
 * @param [in] http_client HTTP client handle.
 * @param [in] http_request HTTP request object.
 * @param [out] http_response HTTP response object.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_PROXY in case the proxy host name can not be resolved.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_HOST in case the remote host name can not be resolved.</li>
 * <li>#MCL_SSL_HANDSHAKE_FAIL in case a problem occurs during SSL handshake.</li>
 * <li>#MCL_COULD_NOT_CONNECT in case there is a connection error.</li>
 * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED in case the server can not be verified.</li>
 * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
 * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
 * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_http_client_send(mcl_http_client_t *http_client, mcl_http_request_t *http_request, mcl_http_response_t **http_response);

/**
 * This function will release the resources of HTTP client handle.
 *
 * @param [in] http_client HTTP client handle.
 */
extern MCL_CORE_EXPORT void mcl_http_client_destroy(mcl_http_client_t **http_client);

#ifdef  __cplusplus
}
#endif

#endif //MCL_HTTP_CLIENT_H_
