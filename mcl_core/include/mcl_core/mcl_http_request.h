/*!
 * @file     mcl_http_request.h
 * @brief    HTTP request module interface header file.
 *
 * This module is used internally by other MCL modules to prepare an HTTP request before
 * #mcl_http_client_send is called. HTTP request (#mcl_http_request_t) can be initialized with
 * #mcl_http_request_initialize function. Following initialization, header fields can be set
 * with #mcl_http_request_add_header function. HTTP request options listed in
 * #E_MCL_HTTP_REQUEST_PARAMETER can be set using #mcl_http_request_set_parameter function.
 * After HTTP request is sent, it has to be destroyed using #mcl_http_request_destroy function.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_HTTP_REQUEST_H_
#define MCL_HTTP_REQUEST_H_

#include "mcl_core/mcl_list.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * HTTP method types.
 */
typedef enum E_MCL_HTTP_METHOD
{
    MCL_HTTP_GET = 0,   //!< Http get method.
    MCL_HTTP_HEAD,      //!< Http head method.
    MCL_HTTP_POST,      //!< Http post method.
    MCL_HTTP_PUT,       //!< Http put method.
    MCL_HTTP_DELETE,    //!< Http delete method.
    MCL_HTTP_PATCH,     //!< Http patch method.
    MCL_HTTP_CONNECT,   //!< Http connect method.
    MCL_HTTP_OPTIONS,   //!< Http options method.
    MCL_HTTP_TRACE,     //!< Http trace method.
    MCL_HTTP_END
} E_MCL_HTTP_METHOD;

/**
 * HTTP request options.
 */
typedef enum E_MCL_HTTP_REQUEST_PARAMETER
{
    MCL_HTTP_REQUEST_PARAMETER_METHOD = 0,      //!< Method of the http request as E_MCL_HTTP_METHOD.
    MCL_HTTP_REQUEST_PARAMETER_URL,             //!< Url of the http request as char*.
    MCL_HTTP_REQUEST_PARAMETER_BODY,            //!< Body of the http request as char*. HTTP Request neither copies the buffer, nor takes ownership.
    MCL_HTTP_REQUEST_PARAMETER_BODY_SIZE,       //!< Size of the body of the http request in bytes as mcl_size_t.
    MCL_HTTP_REQUEST_PARAMETER_STREAM_CALLBACK, //!< Stream callback.
    MCL_HTTP_REQUEST_PARAMETER_STREAM_DATA,     //!< Stream data.
    MCL_HTTP_REQUEST_PARAMETER_END
} E_MCL_HTTP_REQUEST_PARAMETER;

/**
 * Callback function prototype for adding payload to HTTP request.
 *
 * @param [in] buffer Destination address for payload.
 * @param [in] size Size, in bytes, of each element to be read.
 * @param [in] count Number of elements, each one with a size of @p size bytes.
 * @param [in] user_context Source address for payload.
 * @return The total number of bytes successfully written is returned.
 */
typedef mcl_size_t (*mcl_http_payload_callback)(char *buffer, mcl_size_t size, mcl_size_t count, void *user_context);

/**
 * HTTP request handle.
 */
typedef struct mcl_http_request_t
{
    mcl_list_t *header;                          //!< Header of http request.
    mcl_uint8_t *payload;                        //!< Payload of http request.
    mcl_size_t payload_size;                     //!< Payload size of http request.
    E_MCL_HTTP_METHOD method;                    //!< Http method of http request.
    char *uri;                                   //!< Uri of http request.
    mcl_http_payload_callback stream_callback;   //!< Callback to be used with chunked Transfer-Encoding. If not used, it must be NULL.
    void *stream_data;                           //!< Stream data.
} mcl_http_request_t;

/**
 * This function creates and initializes an #mcl_http_request_t.
 *
 * @param [out] http_request The newly initialized HTTP request.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p uri or @p http_request is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_http_request_initialize(mcl_http_request_t **http_request);

/**
 * This function is used to add an HTTP header to the request with its value.
 *
 * @param [in] http_request HTTP request handle to be used.
 * @param [in] header_name Header name.
 * @param [in] header_value Header value.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p http_request, @p header_name or @p header_value is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_http_request_add_header(mcl_http_request_t *http_request, const char *header_name, const char *header_value);

/**
 * This function is used to set a parameter of HTTP request.
 *
 * @param [in] http_request HTTP request handle to be used.
 * @param [in] parameter One of the parameters listed in #E_MCL_HTTP_REQUEST_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p http_request or @p value is NULL.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_http_request_set_parameter(mcl_http_request_t *http_request, E_MCL_HTTP_REQUEST_PARAMETER parameter, const void *value);

/**
 * This function destroys HTTP request data structure.
 *
 * @param [in] http_request HTTP request handle to be freed.
 */
extern MCL_CORE_EXPORT void mcl_http_request_destroy(mcl_http_request_t **http_request);

#ifdef  __cplusplus
}
#endif

#endif //MCL_HTTP_REQUEST_H_
