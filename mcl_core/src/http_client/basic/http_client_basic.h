/*!
 * @file     http_client_basic.h
 * @brief    Basic HTTP client module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef HTTP_CLIENT_BASIC_H_
#define HTTP_CLIENT_BASIC_H_

#include "mcl_core/mcl_http_client.h"
#include "mcl_tls_socket.h"

typedef struct mcl_http_client_t
{
    mcl_tls_ca_chain_handle certificate_chain;
    const char *user_agent;
    mcl_uint16_t port;
    mcl_size_t timeout;
} mcl_http_client_t;

#endif //HTTP_CLIENT_BASIC_H_
