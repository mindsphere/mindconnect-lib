/*!
 * @file     http_request.c
 * @brief    HTTP request module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "http_request.h"
#include "definitions.h"
#include "string_util.h"
#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_memory.h"

// This function is used as callback to list_destroy_with_content() function.
static void _http_request_header_destroy(char **header);

mcl_error_t mcl_http_request_initialize(mcl_http_request_t **http_request)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_http_request_t **http_request = <%p>", http_request);

    // Null check.
    MCL_ASSERT_NOT_NULL(http_request, code);

    MCL_NEW(*http_request);

    if (MCL_NULL == *http_request)
    {
        code = MCL_OUT_OF_MEMORY;
    }
    else
    {
        // Set initial values.
        (*http_request)->header = MCL_NULL;
        (*http_request)->payload = MCL_NULL;
        (*http_request)->uri = MCL_NULL;
        (*http_request)->method = MCL_HTTP_GET;
        (*http_request)->payload_size = 0;
        (*http_request)->stream_callback = MCL_NULL;
        (*http_request)->stream_data = MCL_NULL;

        code = mcl_list_initialize(&(*http_request)->header);
    }

    if (MCL_OK != code)
    {
        mcl_http_request_destroy(http_request);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_http_request_add_header(mcl_http_request_t *http_request, const char *header_name, const char *header_value)
{
    mcl_error_t code;
    char *header_format = "%s: %s";
    mcl_size_t header_line_length;
    char *header_line;

    MCL_DEBUG_ENTRY("mcl_http_request_t *http_request = <%p>, const char *header_name = <%p>, const char *header_value = <%p>", http_request, header_name, header_value);

    // Null check.
    MCL_ASSERT_NOT_NULL(http_request, code);
    MCL_ASSERT_NOT_NULL(header_name, code);
    MCL_ASSERT_NOT_NULL(header_value, code);

    // Length of the header line, +2 is for the string ": ".
    header_line_length = string_util_strlen(header_name) + string_util_strlen(header_value) + 2;

    header_line = MCL_MALLOC(header_line_length + 1);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != header_line, MCL_OUT_OF_MEMORY, "Memory can not be allocated for header line.");

    // Compose the header line.
    code = string_util_snprintf(header_line, header_line_length + 1, header_format, header_name, header_value);

    // Add header line to the list of header lines.
    if (MCL_OK == code)
    {
        code = mcl_list_add(http_request->header, header_line);
    }

    if (MCL_OK != code)
    {
        MCL_FREE(header_line);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_http_request_set_parameter(mcl_http_request_t *http_request, E_MCL_HTTP_REQUEST_PARAMETER parameter, const void *value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_http_request_t *http_request = <%p>, E_MCL_HTTP_REQUEST_PARAMETER parameter = <%d>, const void *value = <%p>", http_request, parameter, value);

    // Null check.
    MCL_ASSERT_NOT_NULL(http_request, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_HTTP_REQUEST_PARAMETER_METHOD:

            if (*((E_MCL_HTTP_METHOD *) value) < MCL_HTTP_END)
            {
                http_request->method = *((E_MCL_HTTP_METHOD *) value);
            }
            else
            {
                code = MCL_INVALID_PARAMETER;
            }

            break;

        case MCL_HTTP_REQUEST_PARAMETER_URL:
            code = string_util_reset(value, &(http_request->uri));
            break;

        case MCL_HTTP_REQUEST_PARAMETER_BODY:
            http_request->payload = (mcl_uint8_t *) value;
            http_request->stream_callback = MCL_NULL;
            break;

        case MCL_HTTP_REQUEST_PARAMETER_BODY_SIZE:
            http_request->payload_size = *((mcl_size_t *) value);
            break;

        case MCL_HTTP_REQUEST_PARAMETER_STREAM_CALLBACK:
            http_request->stream_callback = (mcl_http_payload_callback) value;
            http_request->payload = MCL_NULL;
            break;

        case MCL_HTTP_REQUEST_PARAMETER_STREAM_DATA:
            http_request->stream_data = (void *) value;
            break;

        default:
            code = MCL_INVALID_PARAMETER;
            break;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_http_request_destroy(mcl_http_request_t **http_request)
{
    MCL_DEBUG_ENTRY("mcl_http_request_t **http_request = <%p>", http_request);

    if (MCL_NULL != *http_request)
    {
        // Free http request with its members.
        mcl_list_destroy_with_content(&((*http_request)->header), (mcl_list_item_destroy_callback)_http_request_header_destroy);
        MCL_FREE((*http_request)->uri);
        MCL_FREE(*http_request);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}

static void _http_request_header_destroy(char **header)
{
    MCL_FREE(*header);
}
