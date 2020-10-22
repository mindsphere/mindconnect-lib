/*!
 * @file     http_client_libcurl.h
 * @brief    HTTP client libcurl module header file.
 *
 * Implements the functions defined in http_client.h file. This implementation actively uses "libcurl" for HTTP Send/Receive operations.
 * For targets in which libcurl cannot be used, another implementation file should be created to implement the http_client.h functions.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef HTTP_CLIENT_LIBCURL_H_
#define HTTP_CLIENT_LIBCURL_H_

#include "mcl_core/mcl_http_client.h"

// If you want to use libcurl as static library, define CURL_STATICLIB before including curl.h otherwise linker won't be able to find __impl_* functions

#include "curl/curl.h"

#define SSL_CERTIFICATE_TYPE_PEM "PEM"

struct mcl_http_client_t
{
    CURL *curl;                 //!< Curl handle.
    mcl_list_t *certificates;   //!< List of server certificates.
};

#endif //HTTP_CLIENT_LIBCURL_H_
