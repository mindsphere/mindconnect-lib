/*!
 * @file     http_definitions.h
 * @brief    HTTP definitions module header file.
 *
 * This module contains general definitions used for HTTP.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef HTTP_DEFINITIONS_H_
#define HTTP_DEFINITIONS_H_

/**
 * @brief Used with http_header_names[E_HTTP_HEADER_NAMES] to get the related string.
 * ex: http_header_names[HTTP_HEADER_AUTHORIZATION] returns the string of "Authorization".
 */
typedef enum E_HTTP_HEADER_NAMES
{
    HTTP_HEADER_AUTHORIZATION,             //!< Http authorization header.
    HTTP_HEADER_CONTENT_TYPE,              //!< Http content type header.
    HTTP_HEADER_CONTENT_ID,                //!< Http content id header.
    HTTP_HEADER_CONTENT_LENGTH,            //!< Length of http content header.
    HTTP_HEADER_TRANSFER_ENCODING,         //!< Http transfer encoding header.
    HTTP_HEADER_TRANSFER_ENCODING_CHUNKED, //!< Http transfer encoding chunked header.
    HTTP_HEADER_USER_AGENT,                //!< Http user agent header.
    HTTP_HEADER_ACCEPT,                    //!< Http accept header.
    HTTP_HEADER_HOST,                      //!< Http host header.
    HTTP_HEADER_RANGE,                     //!< Http range header.
    HTTP_HEADER_CONTENT_RANGE,             //!< Http content range header.
    HTTP_HEADER_CONTENT_DISPOSITION,       //!< Http content disposition header.
    HTTP_HEADER_SERVER_TIME,               //!< Server-Time header name.
    HTTP_HEADER_IF_MATCH,                  //!< Http If-Match header name.
    HTTP_HEADER_ETAG,                      //!< Http Etag header name.
    HTTP_HEADER_CORRELATION_ID,            //!< Http Correlation-ID header name.
    HTTP_HEADER_NAMES_END                  //!< End of http header names.
} E_HTTP_HEADER_NAMES;

extern const char *http_header_names[HTTP_HEADER_NAMES_END];

#endif //HTTP_DEFINITIONS_H_
