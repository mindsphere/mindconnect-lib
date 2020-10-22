/*!
 * @file     mcl_core.h
 * @brief    Core module interface header file.
 *
 * MindConnect Library is organized as a core component and extensions
 * to the core component. This header is the top level interface for
 * the core component. Including this header in an application is
 * sufficient to access all core functionality of MCL. Core component
 * includes onboarding, key rotation, and access token retrieval functions
 * as well as HTTP, TLS and cryptography utilities.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_CORE_H_
#define MCL_CORE_H_

#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_core_configuration.h"
#include "mcl_core/mcl_http_client.h"
#include "mcl_core/mcl_json_util.h"
#include "mcl_core/mcl_time_util.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * This data structure is the main handle for Agent - MindSphere communication.
 */
typedef struct mcl_core_t mcl_core_t;

/**
 * This function creates and initializes an object of type #mcl_core_t
 * according to the configuration parameters passed in as an argument.
 * Memory is allocated for the @p core object and configuration parameters
 * are copied to it.
 *
 * @param [in] configuration Data structure holding the configuration parameters for the @p core object.
 * @param [out] core Core handle which is going to be initialized.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any mandatory input parameter is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_NO_ACCESS_TOKEN_PROVIDED if neither initial access token nor loading/saving credentials callback functions are provided.
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_core_initialize(mcl_core_configuration_t *configuration, mcl_core_t **core);

/**
 * This function registers the agent to MindSphere which is called Onboarding.
 *
 * @param [in] core Preinitialized #mcl_core_t object.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any input parameter is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_ALREADY_ONBOARDED in case the agent is already onboarded using @p core.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_PROXY if the proxy host name can not be resolved.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_HOST if the remote host name can not be resolved.</li>
 * <li>#MCL_SSL_HANDSHAKE_FAIL if a problem occurs during SSL handshake.</li>
 * <li>#MCL_COULD_NOT_CONNECT if there is a connection error with the server.</li>
 * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED if the server can not be verified.</li>
 * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
 * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
 * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
 * <li>#MCL_UNAUTHORIZED if response status code of server is related to authorization.</li>
 * <li>#MCL_SERVER_FAIL if the server returns 500 response status code.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_core_onboard(mcl_core_t *core);

/**
 * This function is used by the agent which is already onboarded to MindSphere to generate a new authentication key using the
 * authentication key acquired earlier. Key rotation can be applied only when there is an authentication key available. Onboarding key
 * is not necessary for key rotation.
 *
 * @param [in] core #mcl_core_t object which is initialized and onboarded to MindSphere.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p core is NULL.</li>
 * <li>#MCL_NOT_ONBOARDED if the agent is not onboarded using @p core.</li>
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
 * <li>#MCL_SERVER_FAIL if the server returns 500 response status code.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_core_rotate_key(mcl_core_t *core);

/**
 * This function is used by the agent which is already onboarded to update credentials
 * #mcl_core_t handle bears typically because the credentials are changed by another process
 * and the #mcl_core_t handle has to be synchronized.
 *
 * @param [in] core #mcl_core_t object which is initialized and onboarded to MindSphere.
 * @return
 * <ul>
 * <li>#MCL_OK in case of successful update.</li>
 * <li>#MCL_CREDENTIALS_UP_TO_DATE if credentials of the mcl_core is already up to date.
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_core_update_credentials(mcl_core_t *core);

/**
 * This function is used to get access token from MindSphere.
 * The access token contains the necessary authentication credentials for further exchange requests.
 * The agent can use this function to get a new access token when #MCL_UNAUTHORIZED is returned.
 * The agent can use #mcl_core_rotate_key function to renew its secret key if #mcl_core_get_access_token function returns #MCL_BAD_REQUEST.
 *
 * @param [in] core #mcl_core_t object which is initialized and onboarded to MindSphere.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p core is NULL.</li>
 * <li>#MCL_NOT_ONBOARDED if the agent is not onboarded using @p core.</li>
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
 * <li>#MCL_SERVER_FAIL if response status code of server is 500.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_core_get_access_token(mcl_core_t *core);

/**
 * This function is used to determine if @p core handle is onboarded.
 *
 * @param [in] core The core handle on which the onboarding check will be performed.
 * @return MCL_TRUE if @p core is onboarded. Otherwise MCL_FALSE.
 */
extern MCL_CORE_EXPORT mcl_bool_t mcl_core_is_onboarded(mcl_core_t *core);

/**
 * This function returns a copy of last received access token.
 *
 * @param [in] core Preinitialized #mcl_core_t object.
 * @param [out] token The lastly received access token.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p core or @p token is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
 * <li>#MCL_NOT_ONBOARDED if the agent is not onboarded using @p core.</li>
 * <li>#MCL_NO_ACCESS_TOKEN_EXISTS if @p core does not have an access token.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_core_get_last_access_token(mcl_core_t *core, char **token);

/**
 * This function returns the time when last received access token is generated, NULL if server time is not received.
 *
 * @param [in] core Preinitialized # mcl_core_t object.
 * @param [out] token_time The epoch time when last received access token is generated.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p core or @p token_time is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
 * <li>#MCL_NO_SERVER_TIME if server time is not received.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_core_get_last_token_time(mcl_core_t *core, char **token_time);

/**
 * This function returns the http client.
 *
 * @param [in] core Preinitialized #mcl_core_t object.
 * @return
 * <ul>
 * <li>Pointer to #mcl_http_client_t in case of success.</li>
 * <li>NULL if core is not initialized.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_http_client_t *mcl_core_get_http_client(mcl_core_t *core);

/**
 * This function returns the host name.
 *
 * @param [in] core Preinitialized #mcl_core_t object.
 * @return
 * <ul>
 * <li>Pointer to host name in case of success.</li>
 * <li>NULL if core is either NULL or is not initialized.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT const char *mcl_core_get_host_name(mcl_core_t *core);

/**
 * This function returns the client id.
 *
 * User should not free/manipulate the client id buffer.
 *
 * @param [in] core Preinitialized #mcl_core_t object.
 * @return
 * <ul>
 * <li>Pointer to client id in case of success.</li>
 * <li>NULL if core is either NULL or is not initialized.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT const char *mcl_core_get_client_id(mcl_core_t *core);

/**
 * This function destroys the #mcl_core_t object and frees any memory allocated.
 *
 * @param [in] core Preinitialized #mcl_core_t object to destroy.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case input parameter is NULL.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_core_destroy(mcl_core_t **core);

#ifdef  __cplusplus
}
#endif

#endif //MCL_CORE_H_
