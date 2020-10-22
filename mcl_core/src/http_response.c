/*!
 * @file     http_response.c
 * @brief    HTTP response module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "http_response.h"
#include "string_util.h"
#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_memory.h"
#include <stdlib.h>

static void _free_string(char **header);

mcl_error_t mcl_http_response_initialize(mcl_list_t *header, mcl_uint8_t *payload, mcl_size_t payload_size, E_MCL_HTTP_STATUS_CODE status_code, mcl_http_response_t **http_response)
{
    mcl_error_t result = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_list_t *header = <%p>, mcl_uint8_t *payload = <%p>, mcl_size_t payload_size = <%u>, E_MCL_HTTP_STATUS_CODE status_code = <%d>, mcl_http_response_t **http_response = <%p>",
        header, payload, payload_size, status_code, http_response);

    // Null check.
    MCL_ASSERT_NOT_NULL(header, result);
    MCL_ASSERT_NOT_NULL(http_response, result);

    // Create a new http response object.
    MCL_NEW(*http_response);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != *http_response, MCL_OUT_OF_MEMORY, "Memory can not be allocated for http response object.");

    // Set members of the http_response object.
    (*http_response)->header = header;
    (*http_response)->payload = payload;
    (*http_response)->payload_size = payload_size;
    (*http_response)->status_code = status_code;

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

mcl_error_t mcl_http_response_get_header(mcl_http_response_t *http_response, const char *header_name, char **header_value)
{
    mcl_error_t result = MCL_FAIL;

    MCL_DEBUG_ENTRY("mcl_http_response_t *http_response = <%p>, char *header_name = <%s>, char **header_value = <%p>", http_response, header_name, header_value);

    // Null check.
    MCL_ASSERT_NOT_NULL(http_response, result);
    MCL_ASSERT_NOT_NULL(header_name, result);
    MCL_ASSERT_NOT_NULL(header_value, result);

    // Reset the list in case it has been iterated over before.
    mcl_list_reset(http_response->header);

    while (MCL_TRUE)
    {
        mcl_list_node_t *header_node = MCL_NULL;
        mcl_size_t index = 0;
        char *header;
        mcl_bool_t found;
        result = mcl_list_next(http_response->header, &header_node);
        MCL_ASSERT_CODE_MESSAGE(MCL_OK == result, MCL_FAIL, "The header was not found.");

        // Null terminated header.
        header = (char *) header_node->data;

        found = string_util_find_case_insensitive(header, header_name, &index);

        // If correct header is found then get the value.
        if (MCL_TRUE == found)
        {
            mcl_size_t value_length;
            mcl_size_t header_length = string_util_strlen(header);
            string_util_find(header, ":", &index);

            // If there is a space after ":", index must show the next address.
            if (' ' == header[index + 1])
            {
                ++index;
            }

            *header_value = MCL_CALLOC(header_length - index, 1);
            MCL_ASSERT_CODE_MESSAGE(MCL_NULL != *header_value, MCL_OUT_OF_MEMORY, "Could not allocate memory for *header_value.");

            value_length = header_length - index - 1;
            string_util_strncpy(*header_value, header + index + 1, value_length);

            // Make sure there is null termination at the end.
            (*header_value)[value_length] = '\0';
            result = MCL_OK;

            break;
        }
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

void mcl_http_response_destroy(mcl_http_response_t **http_response)
{
    MCL_DEBUG_ENTRY("mcl_http_response_t **http_response = <%p>", http_response);

    if (MCL_NULL != *http_response)
    {
        // Destroy http_response together with its members.
        mcl_list_destroy_with_content(&((*http_response)->header), (mcl_list_item_destroy_callback)_free_string);
        MCL_FREE((*http_response)->payload);
        MCL_FREE(*http_response);

        MCL_DEBUG("Http response is successfully destroyed.");
    }
    else
    {
        MCL_DEBUG("Http response is already NULL.");
    }

    MCL_DEBUG_LEAVE("retVal = void");
}

static void _free_string(char **header)
{
    MCL_FREE(*header);
}
