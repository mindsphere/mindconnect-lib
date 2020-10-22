/*!
 * @file     http_client_libcurl.c
 * @brief    HTTP client libcurl implementation file.
 *
 * HTTP client implementation file using LibCurl.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "http_client_libcurl.h"
#include "string_util.h"
#include "http_definitions.h"
#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_memory.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

#define CARRIAGE_RETURN '\r'
#define LINE_FEED '\n'
#define DOMAIN_SEPERATOR '\\'
#define CRLF_LENGTH 2

#define SUPPORTED_CIPHERS_LIST \
  "AES128-SHA256:AES256-SHA256:AES128-GCM-SHA256:AES256-GCM-SHA384:"\
  "ECDHE-RSA-AES128-SHA256:ECDHE-RSA-AES256-SHA384:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:"

// Curl CURLOPT_INFILESIZE accepts long argument and CURLOPT_INFILESIZE_LARGE accepts curl_off_t (long long) argument.
// 2GB is the limit for CURLOPT_INFILESIZE.
#define CURL_2GB_LIMIT 0x80000000UL

// Data structure to be passed as an argument to libcurl callback set with CURLOPT_WRITEFUNCTION option.
typedef struct libcurl_payload_t
{
    mcl_uint8_t *data;
    mcl_size_t size;
} libcurl_payload_t;

// Data structure for certificates.
typedef struct mcl_certificate_t
{
    char *content;
    mcl_bool_t is_file;
} mcl_certificate_t;

typedef struct default_callback_user_context_t
{
    mcl_http_request_t *http_request;
    mcl_size_t callback_offset;
} default_callback_user_context_t;

static mcl_bool_t curl_global_initialized = MCL_FALSE;

static CURLcode _ssl_context_callback(CURL *curl, void *ssl_context, void *certificates);
static mcl_size_t _response_payload_callback(void *received_data, mcl_size_t size, mcl_size_t count, void *response_payload);
static mcl_size_t _response_header_callback(void *received_data, mcl_size_t size, mcl_size_t count, void *response_header);
static mcl_size_t _request_payload_callback_for_put(char *buffer, mcl_size_t size, mcl_size_t count, void *user_context);
static mcl_bool_t _is_empty_line(char *line);
static struct curl_slist *_set_request_options(CURL *curl, mcl_http_request_t *http_request, default_callback_user_context_t *user_context);
static mcl_error_t _convert_to_mcl_return_code(CURLcode curl_code);
static void _header_list_destroy_callback(void **item);
static void _certificate_list_destroy_callback(mcl_certificate_t **certificate);

#if MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_DEBUG)
static int _curl_debug_callback(CURL *curl, curl_infotype info_type, char *data, mcl_size_t size, void *debug_data);
#endif

mcl_error_t mcl_http_client_initialize(mcl_http_client_configuration_t *configuration, mcl_http_client_t **http_client)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_http_client_configuration_t *configuration = <%p>, mcl_http_client_t *http_client = <%p>", configuration, http_client);

    MCL_ASSERT_NOT_NULL(http_client, code);

    *http_client = MCL_NULL;

    MCL_ASSERT_NOT_NULL(configuration, code);

    // Create http client.
    MCL_NEW(*http_client);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != *http_client, MCL_OUT_OF_MEMORY, "Memory can not be allocated for http client.");

    (*http_client)->certificates = MCL_NULL;
    (*http_client)->curl = MCL_NULL;

    code = mcl_list_initialize(&(*http_client)->certificates);

    if (MCL_OK == code)
    {
        // SSL_library_init always returns 1, safe to ignore.
        (void)SSL_library_init();

        // Initialize curl memory functions.
        if (MCL_FALSE == curl_global_initialized)
        {
            curl_global_initialized = MCL_TRUE;
            curl_global_init_mem(CURL_GLOBAL_DEFAULT, mcl_memory_malloc, mcl_memory_free, mcl_memory_realloc, string_util_strdup, mcl_memory_calloc);
        }

        // Initialize curl object.
        (*http_client)->curl = curl_easy_init();

        if (MCL_NULL == (*http_client)->curl)
        {
            MCL_ERROR_STRING("Libcurl easy interface can not be initialized.");
            code = MCL_FAIL;
        }
    }

    if (MCL_OK == code)
    {
        // Declare a local curl instance for code clarity.
        CURL *curl = (*http_client)->curl;

        // Set remote port number to work with.
        curl_easy_setopt(curl, CURLOPT_PORT, configuration->port);

        // Set timeout values.
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long) configuration->http_request_timeout);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, (long) configuration->http_request_timeout);

        // Set supported ciphers and tls version.
        curl_easy_setopt(curl, CURLOPT_SSL_CIPHER_LIST, SUPPORTED_CIPHERS_LIST);
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

        // Verify the server's SSL certificate.
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);

#if MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_DEBUG)
        // Set logging if enabled and level set at least DEBUG.
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, _curl_debug_callback);
#endif

        // Make sure header of the response is processed by a different callback from its payload.
        curl_easy_setopt(curl, CURLOPT_HEADER, 0);

        // Set callback for processing the received header.
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, _response_header_callback);

        // Set callback for processing the received payload.
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _response_payload_callback);

        // The headers specified in CURLOPT_HEADER will be used in requests to both servers and proxies.
        curl_easy_setopt(curl, CURLOPT_HEADEROPT, CURLHEADER_UNIFIED);

        // Close the connection when done with the transfer.
        curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);

        // Set user-agent.
        curl_easy_setopt(curl, CURLOPT_USERAGENT, configuration->user_agent);

        // Set certificate.
        if (MCL_NULL != configuration->certificate)
        {
            code = mcl_http_client_add_certificate(*http_client, configuration->certificate, configuration->certificate_is_file);
        }
    }

    // Set proxy options if proxy is used.
    if ((MCL_OK == code) && (MCL_NULL != configuration->proxy_hostname))
    {
        // Declare a local curl instance for code clarity.
        CURL *curl = (*http_client)->curl;

        curl_easy_setopt(curl, CURLOPT_PROXY, configuration->proxy_hostname);
        curl_easy_setopt(curl, CURLOPT_PROXYPORT, configuration->proxy_port);
        curl_easy_setopt(curl, CURLOPT_PROXYTYPE, configuration->proxy_type);
        curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_ANY);

        if (MCL_NULL != configuration->proxy_username)
        {
            if (MCL_NULL != configuration->proxy_domain)
            {
                char *proxy_username;

                mcl_size_t proxy_domain_length = string_util_strlen(configuration->proxy_domain);
                mcl_size_t proxy_username_length = string_util_strlen(configuration->proxy_username);

                // +1 for DOMAIN_SEPARATOR.
                mcl_size_t proxy_entire_username_length = proxy_domain_length + proxy_username_length + 1;

                proxy_username = MCL_MALLOC(proxy_entire_username_length + MCL_NULL_CHAR_SIZE);

                if (MCL_NULL == proxy_username)
                {
                    code = MCL_OUT_OF_MEMORY;
                }
                else
                {
                    // Concatenate domain and username.
                    string_util_memcpy(proxy_username, configuration->proxy_domain, proxy_domain_length);
                    proxy_username[proxy_domain_length] = DOMAIN_SEPERATOR;
                    string_util_memcpy(proxy_username + proxy_domain_length + 1, configuration->proxy_username, proxy_username_length + MCL_NULL_CHAR_SIZE);

                    curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, proxy_username);
                    free(proxy_username);
                }
            }
            else
            {
                curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, configuration->proxy_username);
            }

            curl_easy_setopt(curl, CURLOPT_PROXYPASSWORD, configuration->proxy_password);
        }
    }

MCL_FUNCTION_LEAVE_LABEL:
    if (MCL_OK != code)
    {
        mcl_http_client_destroy(http_client);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_http_client_add_certificate(mcl_http_client_t *http_client, const char *certificate, mcl_bool_t is_file)
{
    mcl_certificate_t *certificate_item = MCL_NULL;
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_http_client_t *http_client = <%p>, const char *certificate = <%p>, mcl_bool_t certificate_is_file = <%u>", http_client, certificate, is_file);

    MCL_ASSERT_NOT_NULL(http_client, code);
    MCL_ASSERT_NOT_NULL(certificate, code);

    MCL_NEW(certificate_item);

    if (MCL_NULL != certificate_item)
    {
        certificate_item->content = string_util_strdup(certificate);
        certificate_item->is_file = is_file;

        if (MCL_NULL != certificate_item->content)
        {
            code = mcl_list_add(http_client->certificates, certificate_item);
        }
        else
        {
            code = MCL_OUT_OF_MEMORY;
        }
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    if (MCL_OK == code)
    {
        if ((1 == http_client->certificates->count) && (MCL_TRUE == is_file))
        {
            curl_easy_setopt(http_client->curl, CURLOPT_CAINFO, certificate_item->content);
        }
        else
        {
            curl_easy_setopt(http_client->curl, CURLOPT_SSL_CTX_DATA, http_client->certificates);
            curl_easy_setopt(http_client->curl, CURLOPT_SSL_CTX_FUNCTION, _ssl_context_callback);
        }
    }

MCL_FUNCTION_LEAVE_LABEL:

    if (MCL_OK != code)
    {
        _certificate_list_destroy_callback(&certificate_item);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_http_client_send(mcl_http_client_t *http_client, mcl_http_request_t *http_request, mcl_http_response_t **http_response)
{
    mcl_error_t return_code;
    struct curl_slist *request_header_list;
    mcl_list_t *response_header = MCL_NULL;
    libcurl_payload_t *response_payload = MCL_NULL;
    CURLcode curl_code;
    mcl_int64_t response_code;

    // default_callback_user_context will be used only for PUT requests without user callback function.
    default_callback_user_context_t default_callback_user_context;

    MCL_DEBUG_ENTRY("mcl_http_client_t *http_client = <%p>, mcl_http_request_t *http_request = <%p>, mcl_http_response_t **http_response = <%p>", http_client, http_request, http_response);

    // Set request options. If there are no request headers, this function returns null but the other options for the request are set anyway.
    request_header_list = _set_request_options(http_client->curl, http_request, &default_callback_user_context);

    // Initialize a header list to store response header, clear the list of request headers if this initialization fails and return.
    return_code = mcl_list_initialize(&response_header);

    if (MCL_OK == return_code)
    {
        // Set pointer passed to the _response_header_callback function as fourth argument.
        curl_easy_setopt(http_client->curl, CURLOPT_HEADERDATA, response_header);

        // Set pointer passed to the _response_callback function as fourth argument.
        MCL_NEW(response_payload);
        if (MCL_NULL != response_payload)
        {
            response_payload->data = MCL_NULL;
            response_payload->size = 0;
            curl_easy_setopt(http_client->curl, CURLOPT_WRITEDATA, response_payload);
        }
        else
        {
            return_code = MCL_OUT_OF_MEMORY;
        }
    }

    if (MCL_OK == return_code)
    {
        // Perform the transfer.
        MCL_INFO("Sending HTTP request...");

        curl_code = curl_easy_perform(http_client->curl);
        return_code = _convert_to_mcl_return_code(curl_code);
        MCL_INFO("HTTP request sent. Result code = <%u>", return_code);
    }

    // Free the list of http request header.
    curl_slist_free_all(request_header_list);

    if (MCL_OK == return_code)
    {
        // Gather response into http_response object.
        curl_easy_getinfo(http_client->curl, CURLINFO_RESPONSE_CODE, &response_code);

        return_code = mcl_http_response_initialize(response_header, response_payload->data, response_payload->size, (E_MCL_HTTP_STATUS_CODE)response_code, http_response);
    }

    if (MCL_OK != return_code)
    {
        mcl_list_destroy_with_content(&response_header, (mcl_list_item_destroy_callback)_header_list_destroy_callback);

        if (MCL_NULL != response_payload)
        {
            MCL_FREE(response_payload->data);
        }
    }
    MCL_FREE(response_payload);

    MCL_DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
}

void mcl_http_client_destroy(mcl_http_client_t **http_client)
{
    MCL_DEBUG_ENTRY("mcl_http_client_t **http_client = <%p>", http_client);

    if ((MCL_NULL != http_client) && (MCL_NULL != *http_client))
    {
        if (MCL_NULL != (*http_client)->curl)
        {
            curl_easy_cleanup((*http_client)->curl);
        }

        mcl_list_destroy_with_content(&(*http_client)->certificates, (mcl_list_item_destroy_callback) &_certificate_list_destroy_callback);
        MCL_FREE(*http_client);

        MCL_DEBUG("Http client handle is destroyed.");
    }
    else
    {
        MCL_DEBUG("Http client is already null.");
    }

    MCL_DEBUG_LEAVE("retVal = void");
}

// This function is the callback which is called to set the SSL certificate given by the parameter "certificate".
static CURLcode _ssl_context_callback(CURL *curl, void *ssl_context, void *certificates)
{
    CURLcode curl_code = CURLE_OK;
    X509_STORE *store;
    mcl_list_t *certificate_list_local = (mcl_list_t *)certificates;
    mcl_list_node_t *certificate_node = MCL_NULL;

    MCL_DEBUG_ENTRY("CURL *curl = <%p>, void *ssl_context = <%p>, void *certificate_list = <%p>", curl, ssl_context, certificates);

    // Unused parameter.
    (void) curl;

    if (0 == certificate_list_local->count)
    {
        MCL_INFO("No certificate is provided by the user for peer verification. Continuing with the existing CA certificate store.");
        return CURLE_OK;
    }

    // Get a pointer to the X509 certificate store (which may be empty!).
    store = SSL_CTX_get_cert_store((SSL_CTX *)ssl_context);

    mcl_list_reset(certificate_list_local);

    while ((MCL_OK == mcl_list_next(certificate_list_local, &certificate_node)) && (CURLE_OK == curl_code))
    {
        BIO *bio;
        int index;
        int certificates_number = 0;
        struct stack_st_X509_INFO *certificate_info = MCL_NULL;
        mcl_certificate_t *certificate = (mcl_certificate_t *) certificate_node->data;

        if (MCL_TRUE == certificate->is_file)
        {
            bio = BIO_new_file(certificate->content, "r");

            if (MCL_NULL == bio)
            {
                MCL_ERROR("Certificate file could not be read: <%s>", certificate->content);
                curl_code = CURLE_READ_ERROR;
            }
        }
        else
        {
            bio = BIO_new_mem_buf(certificate->content, (int) string_util_strlen(certificate->content));

            if (MCL_NULL == bio)
            {
                curl_code = CURLE_OUT_OF_MEMORY;
            }
        }

        if (CURLE_OK == curl_code)
        {
            // Read certificate info.
            certificate_info = PEM_X509_INFO_read_bio(bio, MCL_NULL, MCL_NULL, MCL_NULL);
            BIO_free(bio);

            MCL_ASSERT_CODE_MESSAGE(MCL_NULL != certificate_info, CURLE_SSL_CERTPROBLEM, "Certificate info can not be read from memory.");

            // Read all PEM formatted certificates from memory into an X509 structure that SSL can use.
            certificates_number = sk_X509_INFO_num(certificate_info);
        }

        for (index = 0; (index < certificates_number) && (CURLE_OK == curl_code); ++index)
        {
            X509_INFO *temp_info = sk_X509_INFO_value(certificate_info, index);
            if (MCL_NULL != temp_info)
            {
                if (MCL_NULL != temp_info->x509)
                {
                    if (0 == X509_STORE_add_cert(store, temp_info->x509))
                    {
                        // Ignore error X509_R_CERT_ALREADY_IN_HASH_TABLE which means the certificate is already in the store.
                        // That could happen if libcurl already loaded the certificate from a ca cert bundle set at libcurl build-time or runtime.
                        unsigned long error = ERR_peek_last_error();

                        if ((ERR_GET_LIB(error) != ERR_LIB_X509) || (ERR_GET_REASON(error) != X509_R_CERT_ALREADY_IN_HASH_TABLE))
                        {
                            MCL_ERROR_STRING("Certificate can not be added to store.");
                            curl_code = CURLE_SSL_CERTPROBLEM;
                        }
                        else
                        {
                            MCL_INFO("Did not add the certificate to store since it is already in the store.");
                        }
                    }
                }
                else if ((MCL_NULL != temp_info->crl) && (0 == X509_STORE_add_crl(store, temp_info->crl)))
                {
                    MCL_ERROR_STRING("Certificate can not be added to store.");
                    curl_code = CURLE_SSL_CERTPROBLEM;
                }
            }
            else
            {
                MCL_ERROR_STRING("Certificate info value can not be read from memory.");
                curl_code = CURLE_SSL_CERTPROBLEM;
            }
        }

        // Clean up.
        sk_X509_INFO_pop_free(certificate_info, X509_INFO_free);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", curl_code);
    return curl_code;
}

// This function is the callback which is called once for the payload of the received response.
// The function initializes a string from the received data in the buffer "received_data"
// composed of "count" elements of each "size" bytes long and copies this string to the string array "response_payload".
static mcl_size_t _response_payload_callback(void *received_data, mcl_size_t size, mcl_size_t count, void *response_payload)
{
    mcl_size_t received_data_size = size * count;
    libcurl_payload_t *payload = (libcurl_payload_t *) response_payload;

    MCL_DEBUG_ENTRY("void *received_data = <%p>, mcl_size_t size = <%u>, mcl_size_t count = <%u>, void *response_payload = <%p>", received_data, size, count, response_payload);

    if (payload->data)
    {
        MCL_RESIZE(payload->data, payload->size + received_data_size);
        MCL_ASSERT_CODE_MESSAGE(MCL_NULL != payload->data, 0, "Memory re-allocation for payload data failed!");
    }
    else
    {
        payload->data = MCL_MALLOC(received_data_size);
        MCL_ASSERT_CODE_MESSAGE(MCL_NULL != payload->data, 0, "Memory allocation for payload data failed!");
    }

    string_util_memcpy(payload->data + payload->size, received_data, received_data_size);

    payload->size += received_data_size;

    MCL_DEBUG_LEAVE("retVal = <%d>", received_data_size);
    return received_data_size;
}

// This function is the callback which is called once for every header line of the received http response.
// The function initializes a string from the received data in the buffer "received_data"
// composed of "count" elements of each "size" bytes long and copies this string to the string array "response_header".
static mcl_size_t _response_header_callback(void *received_data, mcl_size_t size, mcl_size_t count, void *response_header)
{
    mcl_list_t *header_list = (mcl_list_t *) response_header;
    char *header_line = MCL_NULL;
    mcl_size_t received_data_size = size * count;

    MCL_DEBUG_ENTRY("void *received_data = <%p>, mcl_size_t size = <%u>, mcl_size_t count = <%u>, void *response_header = <%p>", received_data, size, count, response_header);

    // Eliminate empty line.
    if ((CRLF_LENGTH == received_data_size) && (MCL_TRUE == _is_empty_line((char *) received_data)))
    {
        MCL_DEBUG_LEAVE("retVal = <%d>", received_data_size);
        return received_data_size;
    }

    // Create an buffer for the received header line (CRLF excluded), -2 for CRLF, +1 for NULL.
    header_line = (char *) MCL_MALLOC(received_data_size - 1);

    if (MCL_NULL == header_line)
    {
        // Could not allocate.
        MCL_DEBUG_LEAVE("retVal = <0>");
        return 0;
    }

    // Received data length is received_data_size - CRLF_LENGTH (CRLF excluded).
    string_util_memcpy(header_line, received_data, received_data_size - CRLF_LENGTH);

    // Make sure the header line is null terminated.
    header_line[received_data_size - CRLF_LENGTH] = MCL_NULL_CHAR;

    // Add header line to header list, destroy header line if the add operation is unsuccessful.
    if (MCL_OK != mcl_list_add(header_list, header_line))
    {
        MCL_FREE(header_line);
        MCL_DEBUG_LEAVE("retVal = <0>");
        return 0;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", received_data_size);
    return received_data_size;
}

// This function is the callback which is called when HTTP PUT is requested.
// The function takes in parameter "http_request" of type http_request_t and copies its payload to "buffer".
static mcl_size_t _request_payload_callback_for_put(char *buffer, mcl_size_t size, mcl_size_t count, void *user_context)
{
    default_callback_user_context_t *default_callback_user_context = (default_callback_user_context_t *) user_context;
    mcl_size_t buffer_size = size * count;
    mcl_size_t remaining_size = default_callback_user_context->http_request->payload_size - default_callback_user_context->callback_offset;
    mcl_size_t payload_size = (buffer_size > remaining_size) ? remaining_size : buffer_size;

    MCL_DEBUG_ENTRY("char *buffer = <%s>, mcl_size_t size = <%u>, mcl_size_t count = <%u>, void *user_context = <%p>", buffer, size, count, user_context);

    if (0 == payload_size)
    {
        MCL_DEBUG("Remaining payload size is zero. Nothing will be copied.");
    }
    else
    {
        string_util_memcpy(buffer, default_callback_user_context->http_request->payload + default_callback_user_context->callback_offset, payload_size);
    }

    default_callback_user_context->callback_offset += payload_size;

    MCL_DEBUG_LEAVE("retVal = <%u>", payload_size);
    return payload_size;
}

// This function checks if the given line is an empty line or not.
static mcl_bool_t _is_empty_line(char *line)
{
    mcl_bool_t is_empty;

    MCL_DEBUG_ENTRY("char *line = <%s>", line);

    is_empty = ((CARRIAGE_RETURN == line[0]) && (LINE_FEED == line[1])) ? MCL_TRUE : MCL_FALSE;

    MCL_DEBUG_LEAVE("retVal = <%d>", is_empty);
    return is_empty;
}

// This function sets the options for the http request and returns the curl list of request headers.
static struct curl_slist *_set_request_options(CURL *curl, mcl_http_request_t *http_request, default_callback_user_context_t *user_context)
{
    struct curl_slist *request_header_list = MCL_NULL;
    char *request_header_line = MCL_NULL;
    mcl_size_t index;

    MCL_DEBUG_ENTRY("CURL *curl = <%p>, http_request_t *http_request = <%p>,  default_callback_user_context_t *user_context = <%p>", curl, http_request, user_context);

    // Set the URL to use in the request.
    // Reminder: Request URI is an absolute path including the host name.
    curl_easy_setopt(curl, CURLOPT_URL, http_request->uri);

    // Set http method and data for post and put methods.
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 0);
    curl_easy_setopt(curl, CURLOPT_POST, 0);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 0);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, -1);
    curl_easy_setopt(curl, CURLOPT_READDATA, MCL_NULL);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, MCL_NULL);

    switch (http_request->method)
    {
        case MCL_HTTP_GET :
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
            break;

        case MCL_HTTP_POST :
            curl_easy_setopt(curl, CURLOPT_POST, 1);

            // If a callback function is present, use Transfer-Encoding : chunked.
            if (MCL_NULL == http_request->stream_callback)
            {
                // Normal http transfer without chunked encoding.
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (void *)http_request->payload);

                // If post field size is not set, you have to use transfer-encoding:chunked otherwise no data will be sent.
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, http_request->payload_size);
            }
            else
            {
                // Transfer-Encoding: chunked.
                curl_easy_setopt(curl, CURLOPT_READFUNCTION, http_request->stream_callback);
                curl_easy_setopt(curl, CURLOPT_READDATA, http_request->stream_data);
                request_header_list = curl_slist_append(request_header_list, http_header_names[HTTP_HEADER_TRANSFER_ENCODING_CHUNKED]);
            }
            break;

        case MCL_HTTP_PUT :

            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);

            if (MCL_NULL == http_request->stream_callback)
            {
                // Set user context parameters.
                user_context->http_request = http_request;
                user_context->callback_offset = 0;

                // Set curl callback and user context.
                curl_easy_setopt(curl, CURLOPT_READFUNCTION, _request_payload_callback_for_put);
                curl_easy_setopt(curl, CURLOPT_READDATA, user_context);

                if (http_request->payload_size < CURL_2GB_LIMIT)
                {
                    curl_easy_setopt(curl, CURLOPT_INFILESIZE, (long) http_request->payload_size);
                }
                else
                {
                    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t) http_request->payload_size);
                }
            }
            else
            {
                curl_easy_setopt(curl, CURLOPT_READFUNCTION, http_request->stream_callback);
                curl_easy_setopt(curl, CURLOPT_READDATA, http_request->stream_data);

                if (0 == http_request->payload_size)
                {
                    // Transfer-Encoding: chunked.
                    request_header_list = curl_slist_append(request_header_list, http_header_names[HTTP_HEADER_TRANSFER_ENCODING_CHUNKED]);
                }
                else
                {
                    if (http_request->payload_size < CURL_2GB_LIMIT)
                    {
                        curl_easy_setopt(curl, CURLOPT_INFILESIZE, (long) http_request->payload_size);
                    }
                    else
                    {
                        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t) http_request->payload_size);
                    }
                }
            }

            break;

        default :
            MCL_ERROR_RETURN(MCL_NULL, "Unsupported HTTP method is requested.");
    }

    // Set request header.
    mcl_list_reset(http_request->header);
    for (index = 0; (index < http_request->header->count) && (MCL_NULL != http_request->header->current); ++index)
    {
        mcl_list_node_t *header_node = http_request->header->current;
        request_header_line = header_node->data;
        request_header_list = curl_slist_append(request_header_list, request_header_line);
        http_request->header->current = http_request->header->current->next;
    }

    // Add expect header to prevent that libcurl adds it automatically (HTTP continue bug).
    request_header_list = curl_slist_append(request_header_list, "Expect:");

    // Separate HTTP headers for MindSphere and Proxy.
    curl_easy_setopt(curl, CURLOPT_HEADEROPT, CURLHEADER_SEPARATE);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, request_header_list);
    curl_easy_setopt(curl, CURLOPT_PROXYHEADER, MCL_NULL);

    MCL_DEBUG_LEAVE("retVal = <%p>", request_header_list);
    return request_header_list;
}

#if MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_DEBUG)
static int _curl_debug_callback(CURL *curl, curl_infotype info_type, char *data, mcl_size_t size, void *debug_data)
{
    // Unused parameters.
    (void) curl;
    (void) debug_data;

    switch (info_type)
    {
        case CURLINFO_TEXT :
            MCL_DEBUG("Curl debug info: %.*s", (int) size, data);
            break;

        case CURLINFO_HEADER_OUT :
            MCL_DEBUG("Curl: sent header.");
            break;

        case CURLINFO_DATA_OUT :
            MCL_DEBUG("Curl: sent protocol data.");
            break;

        case CURLINFO_SSL_DATA_OUT :
            MCL_DEBUG("Curl: sent SSL/TLS (binary) data.");
            break;

        case CURLINFO_HEADER_IN :
            MCL_DEBUG("Curl: received header.");
            break;

        case CURLINFO_DATA_IN :
            MCL_DEBUG("Curl: received protocol data.");
            break;

        case CURLINFO_SSL_DATA_IN :
            MCL_DEBUG("Curl: received SSL/TLS (binary) data.");
            break;

        default :
            break;
    }

    return CURLE_OK;
}
#endif

static mcl_error_t _convert_to_mcl_return_code(CURLcode curl_code)
{
    mcl_error_t mcl_code;

    MCL_DEBUG_ENTRY("CURLcode curl_code = <%d>", curl_code);

    switch (curl_code)
    {
        case CURLE_OK :
            mcl_code = MCL_OK;
            break;

        case CURLE_COULDNT_RESOLVE_PROXY :
            mcl_code = MCL_COULD_NOT_RESOLVE_PROXY;
            break;

        case CURLE_COULDNT_RESOLVE_HOST :
            mcl_code = MCL_COULD_NOT_RESOLVE_HOST;
            break;

        case CURLE_COULDNT_CONNECT :
            mcl_code = MCL_COULD_NOT_CONNECT;
            break;

        case CURLE_OUT_OF_MEMORY :
            mcl_code = MCL_OUT_OF_MEMORY;
            break;

        case CURLE_SSL_CONNECT_ERROR :
            mcl_code = MCL_SSL_HANDSHAKE_FAIL;
            break;

        case CURLE_PEER_FAILED_VERIFICATION :
            mcl_code = MCL_SERVER_CERTIFICATE_NOT_VERIFIED;
            break;

        case CURLE_SEND_ERROR :
            mcl_code = MCL_NETWORK_SEND_FAIL;
            break;

        case CURLE_RECV_ERROR :
            mcl_code = MCL_NETWORK_RECEIVE_FAIL;
            break;

        case CURLE_SSL_CERTPROBLEM :
            mcl_code = MCL_IMPROPER_CERTIFICATE;
            break;

        case CURLE_OPERATION_TIMEDOUT  :
            mcl_code = MCL_REQUEST_TIMEOUT;
            break;

        default :
            mcl_code = MCL_FAIL;
            break;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", mcl_code);
    return mcl_code;
}

static void _header_list_destroy_callback(void **item)
{
    MCL_FREE(*item);
}

static void _certificate_list_destroy_callback(mcl_certificate_t **certificate)
{
    MCL_DEBUG_ENTRY("mcl_certificate_t **certificate = <%p>", certificate);

    if ((MCL_NULL != certificate) && (MCL_NULL != *certificate))
    {
        MCL_FREE((*certificate)->content);
        MCL_FREE(*certificate);
    }

    MCL_DEBUG_LEAVE("retVal = <void>");
}
