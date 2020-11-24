/*!
 * @file     http_client_basic.c
 * @brief    Basic HTTP client implementation file.
 *
 * @copyright Copyright (C) 2020 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_list.h"
#include "mcl_core/mcl_string_util.h"
#include "http_client_basic.h"
#include <stdio.h>

static const char crlf[] = "\r\n";
static const char https_prefix[] = "https://";

static const char get_string[] = "GET ";
static const char put_string[] = "PUT ";
static const char post_string[] = "POST ";
static const char http_string[] = " HTTP/1.1\r\n";

static const char content_length_header[] = "Content-Length";
static const char transfer_encoding_header[] = "Transfer-Encoding: chunked";

#define TLS_DEFAULT_PORT 443
#define HTTP_CALLBACK_BUFFER_SIZE 1024
#define HTTP_PARSE_BUFFER_SIZE 2048
#define SIZE_OF_STRING(x) (sizeof(x) - MCL_NULL_CHAR_SIZE)

#define TO_STRING_HELPER(x) (#x)
#define TO_STRING(x) TO_STRING_HELPER(x)

#define PORT_STRING_BUFFER_SIZE 6
#define CRLF_SIZE (sizeof(crlf) - MCL_NULL_CHAR_SIZE)
#define HTTPS_PREFIX_SIZE (sizeof(https_prefix) - MCL_NULL_CHAR_SIZE)
#define GET_STRING_SIZE (sizeof(get_string) - MCL_NULL_CHAR_SIZE)
#define PUT_STRING_SIZE (sizeof(put_string) - MCL_NULL_CHAR_SIZE)
#define POST_STRING_SIZE (sizeof(post_string) - MCL_NULL_CHAR_SIZE)
#define HTTP_STRING_SIZE (sizeof(http_string) - MCL_NULL_CHAR_SIZE)
#define CONTENT_LENGTH_BUFFER_SIZE (sizeof(TO_STRING(SIZE_MAX)))
#define CONTENT_LENGTH_SIZE (sizeof(content_length_header) - MCL_NULL_CHAR_SIZE)
#define TRANSFER_ENCODING_SIZE (sizeof(transfer_encoding_header) - MCL_NULL_CHAR_SIZE)
#define CHUNK_LENGTH_IN_HEX_BUFFER_SIZE (sizeof(TO_STRING(HTTP_CALLBACK_BUFFER_SIZE)) + CRLF_SIZE)

static mcl_error_t start_http(mcl_tls_socket_handle tls_socket, mcl_http_client_t *http_client, mcl_http_request_t *request);
static mcl_error_t get_response(mcl_tls_socket_handle tls_socket, mcl_http_response_t **http_response);
static mcl_error_t get_chunked_response(mcl_tls_socket_handle tls_socket, char **response_body, mcl_size_t *body_size);
static mcl_error_t get_content_length_response(mcl_tls_socket_handle tls_socket, char **response_body, mcl_size_t body_size);
static mcl_error_t receive_to_buffer(mcl_tls_socket_handle tls_socket, void *buffer, mcl_size_t size);
static mcl_error_t receive_until_lf(mcl_tls_socket_handle tls_socket, void *buffer, mcl_size_t *size);
static mcl_error_t send_buffer(mcl_tls_socket_handle tls_socket, const void *buffer, mcl_size_t size);
static mcl_error_t send_header_list(mcl_tls_socket_handle tls_socket, mcl_list_t *header_list);
static mcl_error_t send_header(mcl_tls_socket_handle tls_socket, const char *header_name, const char *header_value);
static mcl_error_t send_with_callback(mcl_tls_socket_handle tls_socket, mcl_http_payload_callback callback, void *stream_data, mcl_size_t payload_size);
static void header_list_destroy_callback(void **item);

mcl_error_t mcl_http_client_initialize(mcl_http_client_configuration_t *configuration, mcl_http_client_t **http_client)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_http_client_configuration_t *configuration = <%p>, mcl_http_client_t **http_client = <%p>", configuration, http_client);

    MCL_ASSERT_NOT_NULL(http_client, code);

    *http_client = MCL_NULL;

    MCL_ASSERT_NOT_NULL(configuration, code);

    if (MCL_NULL != configuration->proxy_hostname)
    {
        MCL_ERROR_STRING("Proxy is not supported for this specific configuration.");
        code = MCL_INVALID_PARAMETER;
    }

    if (MCL_OK == code)
    {
        MCL_NEW(*http_client);

        if (MCL_NULL == *http_client)
        {
            code = MCL_OUT_OF_MEMORY;
        }
        else
        {
            (*http_client)->user_agent = configuration->user_agent;
            (*http_client)->timeout = configuration->http_request_timeout;
            (*http_client)->port = configuration->port;

            code = mcl_tls_ca_chain_init(&((*http_client)->certificate_chain));
        }
    }

    if ((MCL_OK == code) && (MCL_NULL != configuration->certificate))
    {
        code = mcl_tls_ca_chain_add_certificate((*http_client)->certificate_chain, configuration->certificate, configuration->certificate_is_file);
    }

MCL_FUNCTION_LEAVE_LABEL:
    if ((MCL_OK != code) && (MCL_TRIGGERED_WITH_NULL != code))
    {
        mcl_http_client_destroy(http_client);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_http_client_add_certificate(mcl_http_client_t *http_client, const char *certificate, mcl_bool_t is_file)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_http_client_t *http_client = <%p>, const char *certificate = <%p>, mcl_bool_t certificate_is_file = <%u>", http_client, certificate, is_file);

    MCL_ASSERT_NOT_NULL(http_client, code);
    MCL_ASSERT_NOT_NULL(certificate, code);

    code = mcl_tls_ca_chain_add_certificate(http_client->certificate_chain, certificate, is_file);
 
MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_http_client_send(mcl_http_client_t *http_client, mcl_http_request_t *http_request, mcl_http_response_t **http_response)
{
    mcl_error_t code;
    mcl_tls_socket_handle tls_socket;

    MCL_ASSERT_NOT_NULL(http_response, code);
    *http_response = MCL_NULL;

    MCL_ASSERT_NOT_NULL(http_client, code);
    MCL_ASSERT_NOT_NULL(http_request, code);

    if ((MCL_HTTP_GET != http_request->method) && (MCL_NULL == http_request->payload) && (MCL_NULL == http_request->stream_callback))
    {
        code = MCL_INVALID_PARAMETER;
    }
    else if ((MCL_NULL != http_request->payload) && (0 == http_request->payload_size))
    {
        MCL_ERROR_STRING("If payload buffer is given, payload size must be set.");
        code = MCL_INVALID_PARAMETER;
    }
    else if (MCL_OK != mcl_string_util_strncmp(http_request->uri, https_prefix, HTTPS_PREFIX_SIZE))
    {
        MCL_ERROR_STRING("Only HTTPS is supported.");
        code = MCL_OPERATION_NOT_SUPPORTED;
    }
    else
    {
        code = mcl_tls_socket_init(&tls_socket);
    }

    if (MCL_OK == code)
    {
        code = mcl_tls_socket_set_parameter(tls_socket, MCL_TLS_SOCKET_PARAMETER_TIMEOUT, &(http_client->timeout));
    }

    if (MCL_OK == code)
    {
        code = mcl_tls_socket_set_parameter(tls_socket, MCL_TLS_SOCKET_PARAMETER_CERTIFICATE_CHAIN, http_client->certificate_chain);
    }

    if (MCL_OK == code)
    {
        code = mcl_tls_socket_open(tls_socket);
    }

    if (MCL_OK == code)
    {
        code = start_http(tls_socket, http_client, http_request);
    }

    if (MCL_OK == code)
    {
        code = send_header_list(tls_socket, http_request->header);
    }

    if (MCL_OK == code)
    {
        code = send_header(tls_socket, "User-Agent", http_client->user_agent);
    }
    
    if ((MCL_OK == code) && (MCL_HTTP_GET != http_request->method))
    {
        if (0 != http_request->payload_size)
        {
            char content_length[CONTENT_LENGTH_BUFFER_SIZE];

            code = mcl_string_util_snprintf(content_length, CONTENT_LENGTH_BUFFER_SIZE, "%lu", (unsigned long) http_request->payload_size);

            if (MCL_OK == code)
            {
                code = send_header(tls_socket, content_length_header, content_length);
            }
        }
        else
        {
            code = send_buffer(tls_socket, (const mcl_uint8_t *) transfer_encoding_header, TRANSFER_ENCODING_SIZE);

            if (MCL_OK == code)
            {
                code = send_buffer(tls_socket, (const mcl_uint8_t *) crlf, CRLF_SIZE);
            }
        }
    }

    // One more CRLF to complete headers.
    if (MCL_OK == code)
    {
        code = send_buffer(tls_socket, (const mcl_uint8_t *) crlf, CRLF_SIZE);
    }

    if ((MCL_OK == code) && (MCL_HTTP_GET != http_request->method))
    {
        if (MCL_NULL == http_request->payload)
        {
            code = send_with_callback(tls_socket, http_request->stream_callback, http_request->stream_data, http_request->payload_size);
        }
        else
        {
            code = send_buffer(tls_socket, http_request->payload, http_request->payload_size);
        }
    }

    if (MCL_OK == code)
    {
        code = get_response(tls_socket, http_response);
    }

    mcl_tls_socket_destroy(&tls_socket);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_http_client_destroy(mcl_http_client_t **http_client)
{
    MCL_DEBUG_ENTRY("mcl_http_client_t **http_client = <%p>", http_client);

    if ((MCL_NULL != http_client) && (MCL_NULL != *http_client))
    {
        mcl_tls_ca_chain_destroy(&(*http_client)->certificate_chain);
        MCL_FREE(*http_client);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}

static mcl_error_t start_http(mcl_tls_socket_handle tls_socket, mcl_http_client_t *http_client, mcl_http_request_t *http_request)
{
    mcl_error_t code = MCL_OK;
    char *hostname = MCL_NULL;
    mcl_size_t hostname_length;
    mcl_size_t uri_length;

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle tls_socket = <%p>, mcl_http_client_t *http_client = <%p>, mcl_http_request_t *http_request = <%p>", tls_socket, http_client, http_request); 

    uri_length = mcl_string_util_strlen(http_request->uri) - HTTPS_PREFIX_SIZE;

    for (hostname_length = 0; hostname_length < uri_length; ++hostname_length)
    {
        if('/' == http_request->uri[hostname_length + HTTPS_PREFIX_SIZE])
        {
            break;
        }
    }

    hostname = MCL_MALLOC(hostname_length + MCL_NULL_CHAR_SIZE);

    if (MCL_NULL == hostname)
    {
        code = MCL_OUT_OF_MEMORY;
    }
    else
    {
        mcl_string_util_memcpy(hostname, http_request->uri + HTTPS_PREFIX_SIZE, hostname_length);
        hostname[hostname_length] = MCL_NULL_CHAR;
    }

    if (MCL_OK == code)
    {
        code = mcl_tls_socket_connect(tls_socket, hostname, http_client->port);
    }

    if (MCL_OK == code)
    {
        switch(http_request->method)
        {
            case MCL_HTTP_GET:
                code = send_buffer(tls_socket, get_string, GET_STRING_SIZE);
                break;

            case MCL_HTTP_PUT:
                code = send_buffer(tls_socket, put_string, PUT_STRING_SIZE);
                break;

            case MCL_HTTP_POST:
                code = send_buffer(tls_socket, post_string, POST_STRING_SIZE);
                break;

            default:
                code = MCL_FAIL;
                break;
        }
    }

    if (MCL_OK == code)
    {
        code = send_buffer(tls_socket, http_request->uri + HTTPS_PREFIX_SIZE + hostname_length, uri_length - hostname_length);
    }

    if (MCL_OK == code)
    {
        code = send_buffer(tls_socket, http_string, HTTP_STRING_SIZE);
    }

    if (MCL_OK == code)
    {
        code = send_buffer(tls_socket, "Host: ", SIZE_OF_STRING("Host: "));
    }

    if (MCL_OK == code)
    {
        code = send_buffer(tls_socket, hostname, hostname_length);
    }

    MCL_FREE(hostname);

    if ((MCL_OK == code) && (TLS_DEFAULT_PORT != http_client->port))
    {
        char port_string[PORT_STRING_BUFFER_SIZE];

        code = mcl_string_util_snprintf(port_string, PORT_STRING_BUFFER_SIZE, "%u", http_client->port);

        if (MCL_OK == code)
        {
             code = send_buffer(tls_socket, ":", SIZE_OF_STRING(":"));
        }

        if (MCL_OK == code)
        {
             code = send_buffer(tls_socket, port_string, mcl_string_util_strlen(port_string));
        }
    }

    if (MCL_OK == code)
    {
        code = send_buffer(tls_socket, crlf, CRLF_SIZE);
    }
    
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t send_header_list(mcl_tls_socket_handle tls_socket, mcl_list_t *header_list)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t index;

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle tls_socket = <%p>, mcl_list_t *header_list = <%p>", tls_socket, header_list);

    mcl_list_reset(header_list);

    for (index = 0; (MCL_OK == code) && (index < header_list->count); ++index)
    {
        mcl_list_node_t *header_node = MCL_NULL;

        code = mcl_list_next(header_list, &header_node);

        if (MCL_OK == code)
        {
            const char *header = (const char *) header_node->data;

            code = send_buffer(tls_socket, header, mcl_string_util_strlen(header));
        }

        if (MCL_OK == code)
        {
            code = send_buffer(tls_socket, crlf, CRLF_SIZE);
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t send_header(mcl_tls_socket_handle tls_socket, const char *header_name, const char *header_value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle tls_socket = <%p>, const char *header_name = <%p>, const char *header_value = <%p>", tls_socket, header_name, header_value);

    code = send_buffer(tls_socket, header_name, mcl_string_util_strlen(header_name));

    if (MCL_OK == code)
    {
        code = send_buffer(tls_socket, ": ", sizeof(": ") - MCL_NULL_CHAR_SIZE);
    }

    if (MCL_OK == code)
    {
        code = send_buffer(tls_socket, header_value, mcl_string_util_strlen(header_value));
    }

    if (MCL_OK == code)
    {
        code = send_buffer(tls_socket, crlf, CRLF_SIZE);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t send_buffer(mcl_tls_socket_handle tls_socket, const void *buffer, mcl_size_t size)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t sent_already = 0;

    MCL_VERBOSE_ENTRY("mcl_tls_socket_handle tls_socket = <%p>, const void *buffer = <%p>, mcl_size_t size = <%lu>", tls_socket, buffer, (long unsigned) size);

    while ((MCL_OK == code) && (size > sent_already))
    {
        mcl_size_t in_requested_out_sent = size - sent_already;

        code = mcl_tls_socket_send(tls_socket, ((const mcl_uint8_t *) buffer ) + sent_already, &in_requested_out_sent);

        sent_already += in_requested_out_sent;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t send_with_callback(mcl_tls_socket_handle tls_socket, mcl_http_payload_callback callback, void *stream_data, mcl_size_t payload_size)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t size;
    char callback_buffer[HTTP_CALLBACK_BUFFER_SIZE];

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle tls_socket = <%p>, mcl_http_payload_callback callback = <%p>, void *stream_data = <%p>, mcl_size_t payload_size = <%lu>", tls_socket, callback, stream_data, payload_size);

    // remaining_size will not be used if transfer encoding is used.
    mcl_size_t remaining_size = payload_size;
    mcl_bool_t using_transfer_encoding = (0 == payload_size) ? MCL_TRUE : MCL_FALSE;

    do
    {
        size = callback(callback_buffer, 1, HTTP_CALLBACK_BUFFER_SIZE, stream_data);

        if (HTTP_CALLBACK_BUFFER_SIZE < size)
        {
            MCL_ERROR_STRING("HTTP payload callback cannot return greater value than HTTP_CALLBACK_BUFFER_SIZE.");
            code = MCL_FAIL;
        }
        else if (MCL_TRUE == using_transfer_encoding)
        {
            char hex_string[CHUNK_LENGTH_IN_HEX_BUFFER_SIZE];

            code = mcl_string_util_snprintf(hex_string, CHUNK_LENGTH_IN_HEX_BUFFER_SIZE, "%X\r\n", size);

            if (MCL_OK == code)
            {
                code = send_buffer(tls_socket, hex_string, mcl_string_util_strlen(hex_string));
            }
        }
        // If content-length is given, track remaining_size.
        else if (size > remaining_size)
        {
            MCL_ERROR_STRING("User tries to send more data than Content-Length.");
            code = MCL_FAIL;
        }
        else
        {
            remaining_size -= size;
        }

        if ((MCL_OK == code) && (0 != size))
        {
            code = send_buffer(tls_socket, callback_buffer, size);
        }

        // While using "Transfer-Encoding: chunked", CR LF must be sent after chunk, even if chunk size is 0.
        if ((MCL_OK == code) && (MCL_TRUE == using_transfer_encoding))
        {
            code = send_buffer(tls_socket, crlf, CRLF_SIZE);
        }
    
    } while((MCL_OK == code) && (0 != size));
    
    if ((MCL_OK == code) && (0 != remaining_size))
    {
        MCL_ERROR_STRING("User sent less data than expected.");
        code = MCL_FAIL;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t get_response(mcl_tls_socket_handle tls_socket, mcl_http_response_t **http_response)
{
    mcl_error_t code;
    mcl_list_t *header_list = MCL_NULL;
    mcl_size_t in_requested_out_received;
    E_MCL_HTTP_STATUS_CODE status_code;
    char parse_buffer[HTTP_PARSE_BUFFER_SIZE];
    char *response_body = MCL_NULL;
    mcl_size_t body_size = 0;
    mcl_bool_t using_transfer_encoding = MCL_FALSE;
    mcl_bool_t found_content_length = MCL_FALSE;

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle tls_socket = <%p>, mcl_http_response_t **http_response = <%p>", tls_socket, http_response);

    in_requested_out_received = HTTP_PARSE_BUFFER_SIZE;

    code = receive_until_lf(tls_socket, parse_buffer, &in_requested_out_received);

    if (MCL_OK == code)
    {
        mcl_uint32_t status;

        if (1 == sscanf(parse_buffer, "HTTP/1.1 %u", &status))
        {
            status_code = (E_MCL_HTTP_STATUS_CODE) status;
        }
        else
        {
            code = MCL_FAIL;
        }
    }

    if (MCL_OK == code)
    {
        code = mcl_list_initialize(&header_list);
    }

    while (MCL_OK == code)
    {
        char *header = MCL_NULL;
        in_requested_out_received = HTTP_PARSE_BUFFER_SIZE;

        code = receive_until_lf(tls_socket, parse_buffer, &in_requested_out_received);

        if (CRLF_SIZE >= in_requested_out_received)
        {
            break;
        }

        header = MCL_MALLOC(in_requested_out_received - CRLF_SIZE + MCL_NULL_CHAR_SIZE);

        if (MCL_NULL == header)
        {
            code = MCL_OUT_OF_MEMORY;
        }
        else
        {
            mcl_string_util_memcpy(header, parse_buffer, in_requested_out_received - CRLF_SIZE);
            header[in_requested_out_received - CRLF_SIZE] = MCL_NULL_CHAR;
        }

        if (MCL_OK == code)
        {
            code = mcl_list_add(header_list, header);

            if (MCL_OK != code)
            {
                MCL_FREE(header);
            }
        }

        if ((MCL_OK == code) && (MCL_FALSE == found_content_length) && (MCL_FALSE == using_transfer_encoding))
        {
            if (1 == sscanf(parse_buffer, "Content-Length: %lu", (unsigned long *) &body_size))
            {
                found_content_length = MCL_TRUE;
                MCL_DEBUG("Received Content-Length header: %lu.", (unsigned long *) body_size);
            }
            else if (MCL_TRUE == mcl_string_util_find_case_insensitive(parse_buffer, transfer_encoding_header, &in_requested_out_received))
            {
                using_transfer_encoding = MCL_TRUE;
                MCL_DEBUG("Received %s header.", transfer_encoding_header);
            }
        }
    }

    if (MCL_OK == code)
    {
        if (MCL_TRUE == using_transfer_encoding)
        {
            code = get_chunked_response(tls_socket, &response_body, &body_size);
        }
        else if (MCL_TRUE == found_content_length)
        {
            if (0 != body_size)
            {
                code = get_content_length_response(tls_socket, &response_body, body_size);
            }
        }
        else
        {
            code = MCL_FAIL;
        }
    }

    if (MCL_OK == code)
    {
        code = mcl_http_response_initialize(header_list, (mcl_uint8_t *) response_body, body_size, status_code, http_response);
    }

    if (MCL_OK != code)
    {
        mcl_list_destroy_with_content(&header_list, header_list_destroy_callback);
        MCL_FREE(response_body);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t get_chunked_response(mcl_tls_socket_handle tls_socket, char **response_body, mcl_size_t *body_size)
{
    mcl_error_t code = MCL_OK;
    char chunk_string[CONTENT_LENGTH_BUFFER_SIZE + CRLF_SIZE];
    *response_body = MCL_NULL;
    *body_size = 0;

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle tls_socket = <%p>, char **response_body = <%p>, mcl_size_t *body_size = <%p>", tls_socket, response_body, body_size);

    while (MCL_OK == code)
    {
        mcl_size_t chunk_size;
        mcl_size_t in_requested_out_received;

        in_requested_out_received = CONTENT_LENGTH_BUFFER_SIZE + CRLF_SIZE;
        code = receive_until_lf(tls_socket, chunk_string, &in_requested_out_received);

        if (MCL_OK == code)
        {
            if (1 == sscanf(chunk_string, "%x\r\n", (unsigned int *) &chunk_size))
            {
                MCL_DEBUG("Chunk size: %lu.", (unsigned long) chunk_size);
            }
            else
            {
                code = MCL_FAIL;
                MCL_ERROR_STRING("Could not get chunk size.");
            } 
        }

        if ((MCL_OK == code) && (0 != chunk_size))
        {
            if (MCL_NULL == *response_body)
            {
                *response_body = MCL_MALLOC(chunk_size);
            }
            else
            {
                *response_body = MCL_RESIZE(*response_body, *body_size + chunk_size);
            }

            if (MCL_NULL == *response_body)
            {
                code = MCL_OUT_OF_MEMORY;
            }
            else
            {
                code = receive_to_buffer(tls_socket, *response_body + *body_size, chunk_size);
            }

            *body_size += chunk_size;
        }

        if (MCL_OK == code)
        {
            code = receive_to_buffer(tls_socket, chunk_string, CRLF_SIZE);
        }

        if (0 == chunk_size)
        {
            break;
        }
    }

    if (MCL_OK != code)
    {
        MCL_FREE(*response_body);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t get_content_length_response(mcl_tls_socket_handle tls_socket, char **response_body, mcl_size_t body_size)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle tls_socket = <%p>, char **response_body = <%p>, mcl_size_t body_size = <%lu>", tls_socket, response_body, (long unsigned) body_size);

    *response_body = MCL_MALLOC(body_size);

    if (MCL_NULL == *response_body)
    {
        code = MCL_OUT_OF_MEMORY;
    }
    else
    {
        code = receive_to_buffer(tls_socket, *response_body, body_size);

        if (MCL_OK != code)
        {
            MCL_FREE(*response_body);
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t receive_to_buffer(mcl_tls_socket_handle tls_socket, void *buffer, mcl_size_t size)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t received_already = 0;

    MCL_VERBOSE_ENTRY("mcl_tls_socket_handle tls_socket = <%p>, void *buffer = <%p>, mcl_size_t size = <%lu>", tls_socket, buffer, (long unsigned) size);

    while ((MCL_OK == code) && (size > received_already))
    {
        mcl_size_t in_requested_out_received = size - received_already;

        code = mcl_tls_socket_receive(tls_socket, ((mcl_uint8_t *) buffer ) + received_already, &in_requested_out_received);

        received_already += in_requested_out_received;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t receive_until_lf(mcl_tls_socket_handle tls_socket, void *buffer, mcl_size_t *size)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t requested_size = *size;
    *size = 0;

    MCL_DEBUG_ENTRY("mcl_tls_socket_handle tls_socket = <%p>, void *buffer = <%p>, mcl_size_t *size = <%p>", tls_socket, buffer, size);

    while (MCL_OK == code)
    {
        mcl_size_t single_byte_size = 1;
        char received_char;

        code = mcl_tls_socket_receive(tls_socket, &received_char, &single_byte_size);

        if (MCL_OK == code)
        {
            ((char *) buffer)[*size] = received_char;
            ++(*size);

            if ('\n' == received_char)
            {
                break;
            }

            if ((requested_size - MCL_NULL_CHAR_SIZE) == *size)
            {
                code = MCL_FAIL;
            }
        }    
    }

    if (MCL_OK == code)
    {
        ((char *) buffer)[*size] = MCL_NULL_CHAR;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static void header_list_destroy_callback(void **item)
{
    MCL_FREE(*item);
}
