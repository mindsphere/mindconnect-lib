/*!
 * @file     tls_socket_mbedtls.c
 * @brief    TLS Socket implementation file using mbedTLS.
 *
 * @copyright Copyright (C) 2020 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "../mcl_tls_socket.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"
#include "mcl_core/mcl_log_util.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"

#define PORT_STRING_BUFFER_SIZE 6

static const unsigned char entropy_string[] = "MCL TLS socket wrapper using mbedTLS.";

typedef struct mcl_tls_socket_t
{
    mbedtls_net_context net_context;
    mbedtls_ssl_context ssl_context;
    mbedtls_ssl_config ssl_config;
    mbedtls_ctr_drbg_context drbg_context;
    mbedtls_entropy_context entropy_context;
    mcl_size_t timeout;
} mcl_tls_socket_t;

typedef struct mcl_tls_ca_chain_t
{
    mbedtls_x509_crt certificate_chain;
} mcl_tls_ca_chain_t;

#if defined MCL_MOCKSERVER_INTEGRATION
#if defined(WIN32) || defined(WIN64)
#pragma message("This callback function should not be enabled except for integration test with mock server!")
#else
#warning This callback function should not be enabled except for integration test with mock server!
#endif

static int verify_callback(void *context, mbedtls_x509_crt *certificate, int remaining, uint32_t *flags)
{
    const char issuer[] = "www.mockserver.com";
    if (((sizeof(issuer) - MCL_NULL_CHAR_SIZE) == certificate->issuer.val.len) &&
        (MCL_TRUE == mcl_string_util_memcmp(issuer, certificate->issuer.val.p, certificate->issuer.val.len)))
    {
        (*flags) &= (~(MBEDTLS_X509_BADCERT_BAD_KEY | MBEDTLS_X509_BADCERT_CN_MISMATCH));
    }
    
    return 0;
}
#endif

static void mbedtls_debug_function(void *ctx, int level, const char *file, int line, const char *str)
{
    // Unused parameter ctx.
    (void) ctx;

#if MCL_LOG_DISABLED
    (void) level;
    (void) file;
    (void) line;
    (void) str;
#else
    switch (level)
    {
#if MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_ERROR)
        case 1:
            if (MCL_LOG_ENABLED_RUN_TIME(MCL_LOG_LEVEL_ERROR))
            {
                mcl_log_util_function(mcl_log_util_user_context, MCL_LOG_LEVEL_ERROR, file, line, "mbedTLS", "%s", str); 
            }
            break;
#endif
#if MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_DEBUG)
        case 2:
        case 3:
            if (MCL_LOG_ENABLED_RUN_TIME(MCL_LOG_LEVEL_DEBUG))
            {
                mcl_log_util_function(mcl_log_util_user_context, MCL_LOG_LEVEL_DEBUG, file, line, "mbedTLS", "%s", str);
            }
            break;
#endif
#if MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_VERBOSE)
        case 4:
            if (MCL_LOG_ENABLED_RUN_TIME(MCL_LOG_LEVEL_VERBOSE))
            {
                mcl_log_util_function(mcl_log_util_user_context, MCL_LOG_LEVEL_VERBOSE, file, line, "mbedTLS", "%s", str); 
            }
            break;
#endif
        default:
            break;
    }
#endif
}

mcl_error_t mcl_tls_ca_chain_init(mcl_tls_ca_chain_handle *tls_ca_chain_handle)
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

mcl_error_t mcl_tls_ca_chain_add_certificate(mcl_tls_ca_chain_handle tls_ca_chain_handle, const char *certificate, mcl_bool_t is_file)
{
    mcl_error_t code;
    int mbedtls_code;

    MCL_DEBUG_ENTRY("mcl_tls_ca_chain_handle tls_ca_chain_handle = <%p>, const char *certificate = <%p>, mcl_bool_t is_file = <%u>", tls_ca_chain_handle, certificate, is_file);

    if (MCL_TRUE == is_file)
    {
#ifdef MBEDTLS_FS_IO
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

    switch (mbedtls_code)
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

void mcl_tls_ca_chain_destroy(mcl_tls_ca_chain_handle *tls_ca_chain_handle)
{
    MCL_DEBUG_ENTRY("mcl_tls_ca_chain_handle *tls_ca_chain_handle = <%p>", tls_ca_chain_handle);

    if ((NULL != tls_ca_chain_handle) && (NULL != *tls_ca_chain_handle))
    {
        mbedtls_x509_crt_free(&(*tls_ca_chain_handle)->certificate_chain);
        MCL_FREE(*tls_ca_chain_handle);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}

mcl_error_t mcl_tls_socket_init(mcl_tls_socket_handle *tls_socket_handle)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle *tls_socket_handle = <%p>", tls_socket_handle);

    MCL_ASSERT_NOT_NULL(tls_socket_handle, code);

    *tls_socket_handle = MCL_MALLOC(sizeof(mcl_tls_socket_t));

    if (MCL_NULL == *tls_socket_handle)
    {
        code = MCL_OUT_OF_MEMORY;
    }
    else
    {
        int mbedtls_debug;

        (*tls_socket_handle)->timeout = 0;
        mbedtls_net_init(&((*tls_socket_handle)->net_context));
        mbedtls_ssl_init(&((*tls_socket_handle)->ssl_context));
        mbedtls_ctr_drbg_init(&((*tls_socket_handle)->drbg_context));
        mbedtls_entropy_init(&((*tls_socket_handle)->entropy_context));
        mbedtls_ssl_config_init(&((*tls_socket_handle)->ssl_config));
        mbedtls_ssl_conf_rng(&((*tls_socket_handle)->ssl_config), mbedtls_ctr_drbg_random, &((*tls_socket_handle)->drbg_context));
        mbedtls_ssl_conf_dbg(&((*tls_socket_handle)->ssl_config), mbedtls_debug_function, MCL_NULL);
        mbedtls_ssl_conf_authmode(&((*tls_socket_handle)->ssl_config), MBEDTLS_SSL_VERIFY_REQUIRED);

        if (MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_VERBOSE))
        {
            mbedtls_debug = 4;
        }
        else if (MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_DEBUG))
        {
            mbedtls_debug = 3;
        }
        else if (MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_ERROR))
        {
            mbedtls_debug = 1;
        }
        else
        {
            mbedtls_debug = 0;
        }

        mbedtls_debug_set_threshold(mbedtls_debug);

#if defined MCL_MOCKSERVER_INTEGRATION
        mbedtls_ssl_conf_verify(&((*tls_socket_handle)->ssl_config), verify_callback, MCL_NULL);
#endif
    }

    if ((MCL_OK == code) && (0 != mbedtls_ssl_config_defaults(&((*tls_socket_handle)->ssl_config), MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)))
    {
        code = MCL_FAIL;
    }

    if ((MCL_OK == code) && (0 != mbedtls_ctr_drbg_seed(&((*tls_socket_handle)->drbg_context), mbedtls_entropy_func, &((*tls_socket_handle)->entropy_context), entropy_string, sizeof(entropy_string) - MCL_NULL_CHAR_SIZE)))
    {
        code = MCL_FAIL;
    }

    if (MCL_OK != code)
    {
        mcl_tls_socket_destroy(tls_socket_handle);
    }
    
MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_tls_socket_set_parameter(mcl_tls_socket_handle tls_socket_handle, E_MCL_TLS_SOCKET_PARAMETER parameter, const void *value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle tls_socket_handle = <%p>", tls_socket_handle);

    MCL_ASSERT_NOT_NULL(tls_socket_handle, code);

    switch(parameter)
    {
        case MCL_TLS_SOCKET_PARAMETER_CERTIFICATE_CHAIN:
            mbedtls_ssl_conf_ca_chain(&(tls_socket_handle->ssl_config), &((mcl_tls_ca_chain_handle) value)->certificate_chain, MCL_NULL);
            break;

        case MCL_TLS_SOCKET_PARAMETER_TIMEOUT:
            tls_socket_handle->timeout = *((const int *) value);
            break;

        default:
            code = MCL_INVALID_PARAMETER;
            break;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_tls_socket_open(mcl_tls_socket_handle tls_socket_handle)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle tls_socket_handle = <%p>", tls_socket_handle);

    MCL_ASSERT_NOT_NULL(tls_socket_handle, code);

    // mbedtls does not wrap socket function, mbedtls_net_connect does both socket and connect.

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;            
}

mcl_error_t mcl_tls_socket_connect(mcl_tls_socket_handle tls_socket_handle, const char *host, mcl_uint16_t port)
{
    mcl_error_t code;
    char port_string[PORT_STRING_BUFFER_SIZE];
    int mbedtls_code;

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle tls_socket_handle = <%p>, const char *host = <%s>, mcl_uint16_t port = <%u>", tls_socket_handle, host, (unsigned int) port);

    if (NULL == tls_socket_handle)
    {
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else
    {
        code = mcl_string_util_snprintf(port_string, PORT_STRING_BUFFER_SIZE, "%u", port);
    }

    if (MCL_OK == code)
    {
        mbedtls_code = mbedtls_net_connect(&tls_socket_handle->net_context, host, port_string, MBEDTLS_NET_PROTO_TCP);

        if (0 != mbedtls_code)
        {
            MCL_ERROR("mbedtls_net_connect returned: -0x%x", (0 - mbedtls_code));
            
            switch (mbedtls_code)
            {
                case MBEDTLS_ERR_NET_UNKNOWN_HOST:
                    code = MCL_COULD_NOT_RESOLVE_HOST;
                    break;

                case MBEDTLS_ERR_NET_CONNECT_FAILED:
                    code = MCL_COULD_NOT_CONNECT;
                    break;

                default:
                    code = MCL_FAIL;
                    break;
            }
        }
    }

    if (MCL_OK == code)
    {
        mbedtls_code = mbedtls_ssl_setup(&tls_socket_handle->ssl_context, &tls_socket_handle->ssl_config);

        if (0 != mbedtls_code)
        {
            MCL_ERROR("mbedtls_ssl_setup returned: -0x%x", (0 - mbedtls_code));
            code = MCL_OUT_OF_MEMORY;
        }
    }

    if (MCL_OK == code)
    {
        mbedtls_code = mbedtls_ssl_set_hostname(&tls_socket_handle->ssl_context, host);
        if (0 != mbedtls_code)
        {
            MCL_ERROR("mbedtls_ssl_set_hostname returned: -0x%x", (0 - mbedtls_code));
            
            switch (mbedtls_code)
            {
                case MBEDTLS_ERR_SSL_ALLOC_FAILED:
                    code = MCL_OUT_OF_MEMORY;
                    break;

                case MBEDTLS_ERR_SSL_BAD_INPUT_DATA:
                    code = MCL_INVALID_PARAMETER;
                    break;

                default:
                    code = MCL_FAIL;
                    break;
            }
        }
    }

    if (MCL_OK == code)
    {
        mbedtls_ssl_set_bio(&tls_socket_handle->ssl_context, &tls_socket_handle->net_context, mbedtls_net_send, mbedtls_net_recv, MCL_NULL);

        do
        {
            mbedtls_code = mbedtls_ssl_handshake(&tls_socket_handle->ssl_context);
        } while ((MBEDTLS_ERR_SSL_WANT_READ == mbedtls_code) || (MBEDTLS_ERR_SSL_WANT_WRITE == mbedtls_code));

        if (0 != mbedtls_code)
        {
            MCL_ERROR("mbedtls_ssl_handshake returned: -0x%x", (0 - mbedtls_code));

            switch (mbedtls_code)
            {
                case MBEDTLS_ERR_X509_CERT_VERIFY_FAILED:
                    code = MCL_SERVER_CERTIFICATE_NOT_VERIFIED;
                    break;

                case MBEDTLS_ERR_X509_ALLOC_FAILED:
                    code = MCL_OUT_OF_MEMORY;
                    break;

                case MBEDTLS_ERR_X509_FEATURE_UNAVAILABLE:
                case MBEDTLS_ERR_X509_UNKNOWN_OID:
                case MBEDTLS_ERR_X509_FATAL_ERROR:
                case MBEDTLS_ERR_X509_FILE_IO_ERROR:
                    code = MCL_SSL_HANDSHAKE_FAIL;
                    break;

                default:
                    code = MCL_SERVER_CERTIFICATE_NOT_VERIFIED;
                    break;
            }
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;   
}

mcl_error_t mcl_tls_socket_send(mcl_tls_socket_handle tls_socket_handle, const mcl_uint8_t *buffer, mcl_size_t *size)
{
    mcl_error_t code = MCL_OK;

    MCL_VERBOSE_ENTRY("mcl_tls_socket_handle tls_socket_handle = <%p>, const mcl_uint8_t *buffer = <%p>, mcl_size_t *size = <%p>", tls_socket_handle, buffer, size);

    if ((NULL == tls_socket_handle) || (NULL == size))
    {
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else if (0 == *size)
    {
        code = MCL_INVALID_PARAMETER;
    }
    else
    {
        int mbedtls_code;

        do
        {
            mbedtls_code = mbedtls_ssl_write(&tls_socket_handle->ssl_context, buffer, *size);
        } while ((MBEDTLS_ERR_SSL_WANT_READ == mbedtls_code) || (MBEDTLS_ERR_SSL_WANT_WRITE == mbedtls_code));
        
        if (0 > mbedtls_code)
        {
            MCL_ERROR("mbedtls_ssl_write returned: -0x%x", (0 - mbedtls_code));
            code = MCL_FAIL;
        }
        else
        {
            *size = mbedtls_code;
        }
        
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;  
}

mcl_error_t mcl_tls_socket_receive(mcl_tls_socket_handle tls_socket_handle, mcl_uint8_t *buffer, mcl_size_t *size)
{
    mcl_error_t code = MCL_OK;

    MCL_VERBOSE_ENTRY("mcl_tls_socket_handle tls_socket_handle = <%p>, const mcl_uint8_t *buffer = <%p>, mcl_size_t *size = <%p>", tls_socket_handle, buffer, size);

    if ((NULL == tls_socket_handle) || (NULL == size))
    {
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else if (0 == *size)
    {
        code = MCL_INVALID_PARAMETER;
    }
    else
    {
        int mbedtls_code;

        do
        {
            mbedtls_code = mbedtls_ssl_read(&tls_socket_handle->ssl_context, buffer, *size);
        } while ((MBEDTLS_ERR_SSL_WANT_READ == mbedtls_code) || (MBEDTLS_ERR_SSL_WANT_WRITE == mbedtls_code));
        
        if (0 >= mbedtls_code)
        {
            MCL_ERROR("mbedtls_ssl_read returned: -0x%x", (0 - mbedtls_code));
            code = MCL_FAIL;
        }
        else
        {
            *size = mbedtls_code;
        }
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;   
}

void mcl_tls_socket_destroy(mcl_tls_socket_handle *tls_socket_handle)
{
    MCL_DEBUG_ENTRY("mcl_tls_socket_handle *tls_socket_handle = <%p>", tls_socket_handle);

    if ((MCL_NULL != tls_socket_handle) && (MCL_NULL != *tls_socket_handle))
    {
        mbedtls_net_free(&(*tls_socket_handle)->net_context);
        mbedtls_ssl_free(&(*tls_socket_handle)->ssl_context);
        mbedtls_ssl_config_free(&(*tls_socket_handle)->ssl_config);
        mbedtls_ctr_drbg_free(&(*tls_socket_handle)->drbg_context);
        mbedtls_entropy_free(&(*tls_socket_handle)->entropy_context);
        MCL_FREE(*tls_socket_handle);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
