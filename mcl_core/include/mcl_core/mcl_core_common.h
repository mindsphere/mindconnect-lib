/*!
 * @file     mcl_core_common.h
 * @brief    Common module interface header file.
 *
 * This module contains common type definitions used in various MCL modules.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_CORE_COMMON_H_
#define MCL_CORE_COMMON_H_

#include "mcl_core/mcl_config_setup.h"

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#ifdef  __cplusplus
extern "C"
{
#endif

typedef size_t mcl_size_t;
typedef int8_t mcl_int8_t;
typedef int16_t mcl_int16_t;
typedef int32_t mcl_int32_t;
typedef int64_t mcl_int64_t;
typedef uint8_t mcl_uint8_t;
typedef uint16_t mcl_uint16_t;
typedef uint32_t mcl_uint32_t;
typedef uint64_t mcl_uint64_t;
typedef mcl_uint8_t mcl_bool_t;
typedef time_t mcl_time_t;
typedef mcl_int32_t mcl_error_t;

/**
 * MCL function leave label.
 */
#define MCL_FUNCTION_LEAVE_LABEL mcl_function_leave_label

/**
 * MCL bool definition for boolean value of false.
 */
#define MCL_FALSE ((mcl_bool_t) 0)

/**
 * MCL bool definition for boolean value of true.
 */
#define MCL_TRUE ((mcl_bool_t) 1)

// Maximum value mcl_size_t can have.
// The expression below is portable and safe.
#define MCL_SIZE_MAX ((mcl_size_t) - 1)

// MCL_NULL Definition. Refer to C-FAQ Q5.2 - Q5.17 (http://c-faq.com/null/safermacs.html).
#define MCL_NULL ((void *) 0)

#define MCL_NULL_CHAR '\0'
#define MCL_NULL_CHAR_SIZE 1

/**
 * Http request payload size limit for MindSphere.
 * */
#define MCL_MAXIMUM_HTTP_PAYLOAD_SIZE (10485760)

/**
 * Proxy type definitions.
 */
typedef enum E_MCL_PROXY
{
    MCL_PROXY_UNKNOWN = -1,        //!< Unknown proxy.
    MCL_PROXY_HTTP = 0,            //!< Http proxy.
    MCL_PROXY_HTTP_1_0 = 1,        //!< Http 1.0 proxy.
    MCL_PROXY_SOCKS4 = 4,          //!< SOCKS4 proxy.
    MCL_PROXY_SOCKS5 = 5,          //!< SOCKS5 proxy.
    MCL_PROXY_SOCKS4A = 6,         //!< SOCKS4a proxy.
    MCL_PROXY_SOCKS5_HOSTNAME = 7, //!< SOCKS5 hostname proxy.
    MCL_PROXY_END                  //!< End of proxy codes.
} E_MCL_PROXY;

/**
 * Definitions of different security profiles for onboarding.
 */
typedef enum E_MCL_SECURITY_PROFILE
{
    MCL_SECURITY_SHARED_SECRET,  //!< Shared secret.
    MCL_SECURITY_RSA_3072,       //!< RSA 3072.
    MCL_SECURITY_PROFILE_END     //!< End of security profile codes.
} E_MCL_SECURITY_PROFILE;

/**
 * MCL core return code definitions. Every function in core component returning a status code uses this enum values.
 */
typedef enum E_MCL_CORE_RETURN_CODE
{
    // General return codes.
    MCL_OK = 0,                             //!< Success.
    MCL_FAIL,                               //!< Internal failure in MCL.
    MCL_TRIGGERED_WITH_NULL,                //!< Received parameter is null.
    MCL_OUT_OF_MEMORY,                      //!< Memory allocation fail.
    MCL_INVALID_PARAMETER,                  //!< General invalid parameter fail.
    MCL_INVALID_LOG_LEVEL,                  //!< Given log level is invalid.
    MCL_NO_SERVER_TIME,                     //!< Server time is not received from the server.
    MCL_NO_ACCESS_TOKEN_EXISTS,             //!< No access token exists in #mcl_core_t handle.
    MCL_NO_ACCESS_TOKEN_PROVIDED,           //!< Neither initial access token nor loading/saving credentials callback functions are provided.
    MCL_NO_FILE_SUPPORT,                    //!< The system does not support file handling.
    MCL_OPERATION_NOT_SUPPORTED,            //!< Requested operation is not supported.

    // HTTPS return codes.
    MCL_COULD_NOT_RESOLVE_PROXY,            //!< Proxy host name given as a configuration parameter could not be resolved.
    MCL_COULD_NOT_RESOLVE_HOST,             //!< Host name given as a configuration parameter could not be resolved.
    MCL_COULD_NOT_CONNECT,                  //!< MCL failed to connect to the host or proxy.
    MCL_SSL_HANDSHAKE_FAIL,                 //!< A problem occured during SSL/TLS handshake.
    MCL_NETWORK_SEND_FAIL,                  //!< A problem occured when sending data to the network.
    MCL_NETWORK_RECEIVE_FAIL,               //!< A problem occured when receiving data from the network.
    MCL_SERVER_CERTIFICATE_NOT_VERIFIED,    //!< Mindsphere certificate was not verified.
    MCL_IMPROPER_CERTIFICATE,               //!< The server certificate provided is in improper format and it can not be parsed.
    MCL_REQUEST_TIMEOUT,                    //!< The server did not respond within a timeout period.
    MCL_SERVER_FAIL,                        //!< Internal server error.
    MCL_CREATED,                            //!< If the response of server is HTTP 201.
    MCL_PARTIAL_CONTENT,                    //!< If the response of server is HTTP 206.
    MCL_BAD_REQUEST,                        //!< If the response of server is HTTP 400.
    MCL_UNAUTHORIZED,                       //!< If the response of server is HTTP 401.
    MCL_FORBIDDEN,                          //!< If the response of server is HTTP 403.
    MCL_NOT_FOUND,                          //!< If the response of server is HTTP 404.
    MCL_CONFLICT,                           //!< If the response of server is HTTP 409.
    MCL_PRECONDITION_FAIL,                  //!< If the response of server is HTTP 412.
    MCL_REQUEST_PAYLOAD_TOO_LARGE,          //!> If the response of server is HTTP 413.
    MCL_TOO_MANY_REQUESTS,                  //!> If the response of server is HTTP 429.
    MCL_UNEXPECTED_RESULT_CODE,             //!< If the response of server is unexpected.

    // Status return codes.
    MCL_NOT_ONBOARDED,                      //!< Agent is not onboarded to the server yet and does not possess an authentication key.
    MCL_ALREADY_ONBOARDED,                  //!< Agent is already onboarded to the server, hence the library did not try to onboard again.
    MCL_EXCHANGE_STREAMING_IS_ACTIVE,       //!< The streaming is active and shouldn't be interrupted.
    MCL_CREDENTIALS_UP_TO_DATE,             //!< Credentials of the mcl_core are already up to date.
    MCL_CANNOT_ENTER_CRITICAL_SECTION,      //!< If agent cannot enter critical section.

    // Operational return codes.
    MCL_BAD_CONTENT_ENCODING,               //!< If given content for Base64 encoding is bad.
    MCL_JSON_NON_EXISTING_CHILD,            //!< Json child which we try to get doesn't exist in the parent Json object.
    MCL_JSON_NAME_DUPLICATION,              //!< The same name can not be added in the same level of json object.
    MCL_JSON_TYPE_MISMATCH,                 //!< Type of the value of the json object does not match the type requested.
    MCL_SHA256_CALCULATION_FAIL,            //!< If SHA256 calculation fails.
    MCL_CREDENTIALS_NOT_SAVED,              //!< Credentials are not saved.
    MCL_CREDENTIALS_NOT_LOADED,             //!< Credentials are not loaded.

    // Internal return codes.
    MCL_LIST_IS_EMPTY,                      //!< There is no element in the list.
    MCL_LIMIT_EXCEEDED,                     //!< No more space is left to add an additional object.
    MCL_CORE_RETURN_CODE_END                //!< End of return codes.
} E_MCL_CORE_RETURN_CODE;

// Deprecated error code. Kept for backward compatibility.
#define MCL_NON_EXISTING_JSON_CHILD MCL_JSON_NON_EXISTING_CHILD

extern MCL_CORE_EXPORT const char *mcl_core_return_code_strings[MCL_CORE_RETURN_CODE_END];

// This function converts the given return code to its string value for core module.
#define MCL_CORE_CODE_TO_STRING(code) (code < MCL_CORE_RETURN_CODE_END ? mcl_core_return_code_strings[code] : NULL)

/**
 *
 * This is a callback function prototype to load credentials for Shared Secret security profile.
 *
 * @param [out] client_id Client ID.
 * @param [out] client_secret Client secret.
 * @param [out] registration_access_token Registration access token.
 * @param [out] registration_uri Registration URI.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success, all parameters must be dynamically allocated.</li>
 * <li>#MCL_CREDENTIALS_NOT_LOADED if credentials can not be loaded, all parameters must be set to NULL.</li>
 * </ul>
 */
typedef mcl_error_t (*mcl_credentials_load_shared_secret_callback_t)(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri);

/**
 *
 * This is a callback function prototype to save credentials for Shared Secret security profile.
 *
 * @param [in] client_id Client ID.
 * @param [in] client_secret Client secret.
 * @param [in] registration_access_token Registration access token.
 * @param [in] registration_uri Registration URI.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_CREDENTIALS_NOT_SAVED if credentials can not be saved.</li>
 * </ul>
 */
typedef mcl_error_t (*mcl_credentials_save_shared_secret_callback_t)(const char *client_id, const char *client_secret, const char *registration_access_token,
    const char *registration_uri);

/**
 * This is a callback function prototype to load credentials for RSA security profile.
 *
 * @param [out] client_id Client ID.
 * @param [out] public_key Public key.
 * @param [out] private_key Private key.
 * @param [out] registration_access_token Registration access token.
 * @param [out] registration_uri Registration URI.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success, all parameters must be dynamically allocated.</li>
 * <li>#MCL_CREDENTIALS_NOT_LOADED if credentials can not be loaded, all parameters must be set to NULL.</li>
 * </ul>
 */
typedef mcl_error_t (*mcl_credentials_load_rsa_callback_t)(char **client_id, char **public_key, char **private_key, char **registration_access_token, char **registration_uri);

/**
 * This is a callback function prototype to save credentials for RSA security profile.
 *
 * @param [in] client_id Client ID.
 * @param [in] public_key Public key.
 * @param [in] private_key Private key.
 * @param [in] registration_access_token Registration access token.
 * @param [in] registration_uri Registration URI.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_CREDENTIALS_NOT_SAVED if credentials can not be saved.</li>
 * </ul>
 */
typedef mcl_error_t (*mcl_credentials_save_rsa_callback_t)(const char *client_id, const char *public_key, const char *private_key, const char *registration_access_token,
    const char *registration_uri);

/**
 * This union is used to assign a callback function for loading credentials (whether RSA or shared secret).
 */
typedef union mcl_credentials_load_callback_t
{
    mcl_credentials_load_shared_secret_callback_t shared_secret; //!< Callback type to load shared secret.
    mcl_credentials_load_rsa_callback_t rsa;                     //!< Callback type to load RSA key.
} mcl_credentials_load_callback_t;

/**
 * This union is used to assign a callback function for saving credentials (whether RSA or shared secret).
 */
typedef union mcl_credentials_save_callback_t
{
    mcl_credentials_save_shared_secret_callback_t shared_secret; //!< Callback type to save shared secret.
    mcl_credentials_save_rsa_callback_t rsa;                     //!< Callback type to save RSA key.
} mcl_credentials_save_callback_t;

/**
 *
 * This is a callback function prototype to enter critical section (onboarding, key rotation, updating security information).
 *
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_CANNOT_ENTER_CRITICAL_SECTION if cannot enter critical section.</li>
 * </ul>
 */
typedef mcl_error_t (*mcl_critical_section_enter_callback_t)(void);

/**
 *
 * This is a callback function prototype to leave critical section (onboarding, key rotation, updating security information).
 *
 */
typedef void (*mcl_critical_section_leave_callback_t)(void);

#ifdef  __cplusplus
}
#endif

#endif //MCL_CORE_COMMON_H_
