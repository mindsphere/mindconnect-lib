/*!
 * @file     mcl_http_response.h
 * @brief    HTTP response module header file.
 *
 * This module is used internally by other MCL modules to analyze and parse received
 * HTTP response messages. HTTP response (#mcl_http_response_t) can be initialized and
 * prepared with #mcl_http_response_initialize function. Following initialization,
 * value of HTTP header can be reached with #mcl_http_response_get_header.
 * HTTP response has to be destroyed when it is no longer needed
 * using #mcl_http_response_destroy function.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_HTTP_RESPONSE_H_
#define MCL_HTTP_RESPONSE_H_

#include "mcl_core/mcl_core_common.h"
#include "mcl_core/mcl_list.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * HTTP Status Codes
 */
typedef enum E_MCL_HTTP_STATUS_CODE
{
    MCL_HTTP_STATUS_CODE_UNKNOWN = 0,                       //!< Unknown status code
    MCL_HTTP_STATUS_CODE_CONTINUE = 100,                    //!< Actual code : 100
    MCL_HTTP_STATUS_CODE_SUCCESS = 200,                     //!< Actual code : 200
    MCL_HTTP_STATUS_CODE_CREATED = 201,                     //!< Actual code : 201
    MCL_HTTP_STATUS_CODE_PARTIAL_CONTENT = 206,             //!< Actual code : 206
    MCL_HTTP_STATUS_CODE_MOVED_PERMANENTLY = 301,           //!< Actual code : 301
    MCL_HTTP_STATUS_CODE_USE_PROXY = 305,                   //!< Actual code : 305
    MCL_HTTP_STATUS_CODE_BAD_REQUEST = 400,                 //!< Actual code : 400
    MCL_HTTP_STATUS_CODE_UNAUTHORIZED = 401,                //!< Actual code : 401
    MCL_HTTP_STATUS_CODE_FORBIDDEN = 403,                   //!< Actual code : 403
    MCL_HTTP_STATUS_CODE_NOT_FOUND = 404,                   //!< Actual code : 404
    MCL_HTTP_STATUS_CODE_METHOD_NOT_ALLOWED = 405,          //!< Actual code : 405
    MCL_HTTP_STATUS_CODE_PROXY_AUTH_REQUIRED = 407,         //!< Actual code : 407
    MCL_HTTP_STATUS_CODE_REQUEST_TIMEOUT = 408,             //!< Actual code : 408
    MCL_HTTP_STATUS_CODE_CONFLICT = 409,                    //!< Actual code : 409
    MCL_HTTP_STATUS_CODE_PRECONDITION_FAILED = 412,         //!< Actual code : 412
    MCL_HTTP_STATUS_CODE_PAYLOAD_TOO_LARGE = 413,           //!< Actual code : 413
    MCL_HTTP_RESULT_CODE_TOO_MANY_REQUESTS = 429,           //!< Actual code : 429
    MCL_HTTP_STATUS_CODE_INTERNAL_SERVER_ERR = 500,         //!< Actual code : 500
    MCL_HTTP_STATUS_CODE_NOT_IMPLEMENTED = 501,             //!< Actual code : 501
    MCL_HTTP_STATUS_CODE_BAD_GATEWAY = 502,                 //!< Actual code : 502
    MCL_HTTP_STATUS_CODE_SERVICE_UNAVAILABLE = 503,         //!< Actual code : 503
    MCL_HTTP_STATUS_CODE_GATEWAY_TIMEOUT = 504,             //!< Actual code : 504
    MCL_HTTP_STATUS_CODE_HTTP_VERSION_NOT_SUPPORTED = 505   //!< Actual code : 505
} E_MCL_HTTP_STATUS_CODE;

/**
 * HTTP response handle
 *
 * Contains HTTP status code, headers and payload.
 */
typedef struct mcl_http_response_t
{
    mcl_list_t *header;                     //!< Header of http response.
    mcl_uint8_t *payload;                   //!< Payload of http response.
    mcl_size_t payload_size;                //!< Payload size of http response.
    E_MCL_HTTP_STATUS_CODE status_code;     //!< Status code of http response.
} mcl_http_response_t;

/**
 * This function initializes an HTTP response message.
 *
 * @param [in] header List of received HTTP headers.
 * @param [in] payload The received HTTP payload.
 * @param [in] payload_size Size of @p payload.
 * @param [in] status_code The received status code.
 * @param [out] http_response Handle of the initialized #mcl_http_response_t object.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any mandatory input parameter is NULL.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_http_response_initialize(mcl_list_t *header, mcl_uint8_t *payload, mcl_size_t payload_size, E_MCL_HTTP_STATUS_CODE status_code, mcl_http_response_t **http_response);

/**
 * This function gets the value of a specified HTTP header.
 *
 * @param [in]  http_response HTTP response handle to be used.
 * @param [in]  header_name Name of the header whose value is requested.
 * @param [out] header_value Value of the header will be stored in @p header_value.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL If header cannot be found.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any mandatory input parameter is NULL.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_http_response_get_header(mcl_http_response_t *http_response, const char *header_name, char **header_value);

/**
 * This function destroys the HTTP response handle.
 *
 * @param [in] http_response HTTP response handle.
 */
extern MCL_CORE_EXPORT void mcl_http_response_destroy(mcl_http_response_t **http_response);

#ifdef  __cplusplus
}
#endif

#endif
