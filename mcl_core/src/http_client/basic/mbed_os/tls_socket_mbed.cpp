/*!
 * @file     tls_socket_mbed.cpp
 * @brief    TLS Socket implementation file using mbed OS TLSSocket.
 *
 * @copyright Copyright (C) 2020 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "../mcl_tls_socket.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include <TLSSocket.h>
#include <SocketAddress.h>

typedef struct mcl_tls_socket_t
{
    TLSSocket socket;
} mcl_tls_socket_t;

typedef struct mcl_tls_ca_chain_t
{
    mbedtls_x509_crt certificate_chain;
} mcl_tls_ca_chain_t;

extern NetworkInterface *mcl_network_interface;

extern "C" mcl_error_t mcl_tls_ca_chain_init(mcl_tls_ca_chain_handle *tls_ca_chain_handle)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_tls_ca_chain_handle *tls_ca_chain_handle = <%p>", tls_ca_chain_handle);

    MCL_ASSERT_NOT_NULL(tls_ca_chain_handle, code);

    *tls_ca_chain_handle = (mcl_tls_ca_chain_handle) MCL_MALLOC(sizeof(mcl_tls_ca_chain_t));

    if (NULL == *tls_ca_chain_handle)
    {
        code = MCL_OUT_OF_MEMORY;
    }
    else
    {
        mbedtls_x509_crt_init(&(*tls_ca_chain_handle)->certificate_chain);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

extern "C" mcl_error_t mcl_tls_ca_chain_add_certificate(mcl_tls_ca_chain_handle tls_ca_chain_handle, const char *certificate, mcl_bool_t is_file)
{
    mcl_error_t code;
    int mbedtls_code;

    MCL_DEBUG_ENTRY("mcl_tls_ca_chain_handle tls_ca_chain_handle = <%p>, const char *certificate = <%p>, mcl_bool_t is_file = <%u>", tls_ca_chain_handle, certificate, is_file);

    if (MCL_TRUE == is_file)
    {
#if MBEDTLS_FS_IO
        mbedtls_code = mbedtls_x509_crt_parse_file(&tls_ca_chain_handle->certificate_chain, certificate);
#else
        MCL_ERROR_RETURN(MCL_OPERATION_NOT_SUPPORTED, "Certificate cannot be given as file for this specific configuration.")
#endif   
    }
    else
    {
        size_t buffer_length = mcl_string_util_strlen(certificate) + MCL_NULL_CHAR_SIZE;
        mbedtls_code =  mbedtls_x509_crt_parse(&tls_ca_chain_handle->certificate_chain, (const unsigned char *)certificate, buffer_length);
    }

    switch(mbedtls_code)
    {
        case 0:
            code = MCL_OK;
            break;

        case MBEDTLS_ERR_X509_ALLOC_FAILED:
            code = MCL_OUT_OF_MEMORY;
            break;

        default:
            code = MCL_IMPROPER_CERTIFICATE;
            break;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

extern "C" void mcl_tls_ca_chain_destroy(mcl_tls_ca_chain_handle *tls_ca_chain_handle)
{
    MCL_DEBUG_ENTRY("mcl_tls_ca_chain_handle *tls_ca_chain_handle = <%p>", tls_ca_chain_handle);

    if ((NULL != tls_ca_chain_handle) && (NULL != *tls_ca_chain_handle))
    {
        mbedtls_x509_crt_free(&(*tls_ca_chain_handle)->certificate_chain);
        MCL_FREE(*tls_ca_chain_handle);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}

extern "C" mcl_error_t mcl_tls_socket_init(mcl_tls_socket_handle *tls_socket_handle)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle *tls_socket_handle = <%p>", tls_socket_handle);

    MCL_ASSERT_NOT_NULL(tls_socket_handle, code);

    *tls_socket_handle = new mcl_tls_socket_t;

    /*
     * --no_exceptions for mbed os.
     * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0491c/CIHJDJEI.html
     */

    if (nullptr == *tls_socket_handle)
    {
        code = MCL_OUT_OF_MEMORY;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

extern "C" mcl_error_t mcl_tls_socket_set_parameter(mcl_tls_socket_handle tls_socket_handle, E_MCL_TLS_SOCKET_PARAMETER parameter, const void *value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle tls_socket_handle = <%p>", tls_socket_handle);

    MCL_ASSERT_NOT_NULL(tls_socket_handle, code);

    switch(parameter)
    {
        case MCL_TLS_SOCKET_PARAMETER_CERTIFICATE_CHAIN:
            tls_socket_handle->socket.set_ca_chain(&((mcl_tls_ca_chain_handle) value)->certificate_chain);
            break;

        case MCL_TLS_SOCKET_PARAMETER_TIMEOUT:
            tls_socket_handle->socket.set_timeout(*((const int *) value));
            break;

        default:
            code = MCL_INVALID_PARAMETER;
            break;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

extern "C" mcl_error_t mcl_tls_socket_open(mcl_tls_socket_handle tls_socket_handle)
{
    mcl_error_t code = MCL_FAIL;

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle tls_socket_handle = <%p>", tls_socket_handle);

    MCL_ASSERT_NOT_NULL(tls_socket_handle, code);

    if (nullptr == mcl_network_interface)
    {
        MCL_FATAL_STRING("Network interface is NULL.");
    }
    else if (NSAPI_ERROR_OK == tls_socket_handle->socket.open(mcl_network_interface))
    {
        code = MCL_OK;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;            
}

extern "C" mcl_error_t mcl_tls_socket_connect(mcl_tls_socket_handle tls_socket_handle, const char *host, mcl_uint16_t port)
{
    mcl_error_t code = MCL_COULD_NOT_CONNECT;
    nsapi_error_t nsapi_code;

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle tls_socket_handle = <%p>, const char *host = <%s>, mcl_uint16_t port = <%u>", tls_socket_handle, host, (unsigned int) port);

    if (NULL == tls_socket_handle)
    {
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else
    {
        SocketAddress address;
        
        nsapi_code = mcl_network_interface->gethostbyname(host, &address);
    
        if (nsapi_code != NSAPI_ERROR_OK)
        {
            MCL_ERROR("Cannot resolve DNS, gethostbyname returned: %d", nsapi_code);
            code = MCL_COULD_NOT_RESOLVE_HOST;
        }
        else
        {
            address.set_port(port);
            tls_socket_handle->socket.set_hostname(host);
            
            nsapi_code = tls_socket_handle->socket.connect(address);
        
            if (NSAPI_ERROR_OK == nsapi_code)
            {
                code = MCL_OK;
            }
            else
            {
                 MCL_ERROR("Mbed TLS Socket returned %d.", nsapi_code);
            }
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;   
}

extern "C" mcl_error_t mcl_tls_socket_send(mcl_tls_socket_handle tls_socket_handle, const mcl_uint8_t *buffer, mcl_size_t *size)
{
    mcl_error_t code = MCL_OK;

    MCL_VERBOSE_ENTRY("mcl_tls_socket_handle tls_socket_handle = <%p>, const mcl_uint8_t *buffer = <%p>, mcl_size_t *size = <%p>", tls_socket_handle, buffer, size);

    if (NULL == tls_socket_handle)
    {
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else if (0 == size)
    {
        code = MCL_INVALID_PARAMETER;
    }
    else
    {
        nsapi_size_or_error_t nsapi_code;

        nsapi_code = tls_socket_handle->socket.send(buffer, (nsapi_size_t) *size);

        if (0 >= nsapi_code)
        {
            code = MCL_FAIL;
            *size = 0;
        }
        else
        {
            *size = nsapi_code;
        }
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;  
}

extern "C" mcl_error_t mcl_tls_socket_receive(mcl_tls_socket_handle tls_socket_handle, mcl_uint8_t *buffer, mcl_size_t *size)
{
    mcl_error_t code = MCL_OK;

    MCL_VERBOSE_ENTRY("mcl_tls_socket_handle tls_socket_handle = <%p>, const mcl_uint8_t *buffer = <%p>, mcl_size_t *size = <%p>", tls_socket_handle, buffer, size);

    if (NULL == tls_socket_handle)
    {
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else if (0 == size)
    {
        code = MCL_INVALID_PARAMETER;
    }
    else
    {
        nsapi_size_or_error_t nsapi_code;

        nsapi_code = tls_socket_handle->socket.recv(buffer, (nsapi_size_t) *size);

        if (0 >= nsapi_code)
        {
            code = MCL_FAIL;
            *size = 0;
        }
        else
        {
            *size = nsapi_code;
        }
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;   
}

extern "C" void mcl_tls_socket_destroy(mcl_tls_socket_handle *tls_socket_handle)
{
    MCL_DEBUG_ENTRY("mcl_tls_socket_handle *tls_socket_handle = <%p>", tls_socket_handle);

    if ((nullptr != tls_socket_handle) && (nullptr != *tls_socket_handle))
    {
        delete *tls_socket_handle;
        *tls_socket_handle = nullptr;
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
