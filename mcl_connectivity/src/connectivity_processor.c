/*!
 * @file     connectivity_processor.c
 * @brief    Connectivity processor module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "connectivity_processor.h"
#include "multipart.h"
#include "store.h"
#include "event.h"
#include "data_source_configuration.h"
#include "timeseries.h"
#include "file.h"
#include "custom_data.h"
#include "json.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"
#include "mcl_core/mcl_file_util.h"
#include "mcl_core/mcl_json_util.h"
#include "mcl_core/mcl_random.h"

/**
 * Content types.
 *
 * This is used with #content_type_values array to get the related string.
 * ex: content_type_values[CONTENT_TYPE_APPLICATION_OCTET_STREAM] returns the string of "application/octet-stream".
 */
typedef enum E_CONTENT_TYPE_VALUES
{
    CONTENT_TYPE_APPLICATION_OCTET_STREAM, //!< Content type is application octet stream.
    CONTENT_TYPE_TEXT_PLAIN,               //!< Content type is text plain.
    CONTENT_TYPE_APPLICATION_JSON,         //!< Content type is application json.
    CONTENT_TYPE_MULTIPART_MIXED,          //!< Content type is multipart mixed.
    CONTENT_TYPE_URL_ENCODED,              //!< Content type is application/x-www-form-urlencoded.
    TRANSFER_ENCODING_CHUNKED,             //!< Transfer encoding chunked.
    CONTENT_TYPE_VALUES_END                //!< End of content type enumeration.
} E_CONTENT_TYPE_VALUES;

/**
 * Content type values.
 */
static const char *content_type_values[CONTENT_TYPE_VALUES_END] =
{
     "application/octet-stream",
     "text/plain",
     "application/json",
     "multipart/mixed",
     "application/x-www-form-urlencoded",
     "chunked"
};

// Function to scan store items according to exchange result.
static mcl_error_t _scan_store_after_exchange(store_t *store, mcl_bool_t *store_fully_processed, mcl_error_t exchange_response);

// Function to add all http headers for the exchange request.
static mcl_error_t _add_http_headers_for_exchange(connectivity_processor_t *processor, mcl_http_request_t *request, const char *boundary);

// Function used to add Content-Type header to the request including the main boundary for the multipart message and charset.
static mcl_error_t _add_multipart_mixed_content_type_header(mcl_http_request_t *request, const char *boundary);

// Function used to add authorization header to the request.
static mcl_error_t _add_authorization_header(mcl_http_request_t *request, const char *access_token);

// Function used to generate correlation id for the request.
static mcl_error_t _generate_correlation_id_string(char *correlation_id);

// Function used to evaluate the response to the exchange request.
static mcl_error_t _evaluate_response_for_exchange(mcl_http_response_t *response, const char *correlation_id);

// Function used to add file tuple with callback.
static mcl_size_t _file_payload_callback(char *buffer, mcl_size_t size, mcl_size_t count, void *user_context);

// Function used to add custom data tuple with callback.
static mcl_size_t _custom_data_payload_callback(char *buffer, mcl_size_t size, mcl_size_t count, void *user_context);

// Function to prepare body for store.
static mcl_error_t _prepare_body_for_store(store_t *store, mcl_size_t max_http_payload_size, char *boundary, mcl_uint8_t **body, mcl_size_t *body_size);

// Function to prepare body for item (including store).
static mcl_error_t _prepare_body(mcl_item_t *item, mcl_size_t max_http_payload_size, char *boundary, mcl_uint8_t **body, mcl_size_t *body_size);

// Function to add custom data to buffer.
static mcl_error_t _add_custom_data_to_buffer(char *buffer, mcl_size_t *buffer_size, custom_data_t *custom_data, const char *boundary);

// Function to calculate size of each item in store.
static mcl_error_t _check_store_size(store_t *store, mcl_size_t max_http_payload_size);

// Revert status of items due to exchange failure.
static void _revert_store_item_status(store_t *store);

// Remove items which are successfully exchanged from store.
static mcl_bool_t _remove_uploaded_store_items(store_t *store);

// Prepare data point mapping json.
static mcl_error_t _prepare_mapping_json(mapping_t *mapping, const char *agent_id, char **json);

#define CORRELATION_ID_BYTE_LENGTH 16
#define CORRELATION_ID_BUFFER_LENGTH ((CORRELATION_ID_BYTE_LENGTH * 2) + MCL_NULL_CHAR_SIZE)

/**
 * Handle for user context of custom data callback.
 */
typedef struct custom_data_callback_user_context
{
    custom_data_t *custom_data;
    mcl_size_t offset;
} custom_data_callback_user_context;

mcl_error_t connectivity_processor_exchange(connectivity_processor_t *connectivity_processor, mcl_item_t *item)
{
    mcl_error_t code;
    mcl_bool_t is_store = MCL_FALSE;
    mcl_bool_t store_fully_processed = MCL_FALSE;

    MCL_DEBUG_ENTRY("connectivity_processor_t *connectivity_processor = <%p>, mcl_item_t *item = <%p>", connectivity_processor, item);

    // Validate item structure.
    if (MCL_ITEM_PREAMBLE != (item)->preamble)
    {
        MCL_DEBUG_LEAVE("retVal = <%d>", MCL_INVALID_PARAMETER);
        return MCL_INVALID_PARAMETER;
    }

    // Validate item.
    switch ((item)->type)
    {
        case MCL_ITEM_TYPE_EVENT:
            code = event_validate((event_t *) item);
            break;

        case MCL_ITEM_TYPE_DATA_SOURCE_CONFIGURATION:
            code = data_source_configuration_validate((data_source_configuration_t *) item);
            break;

        case MCL_ITEM_TYPE_TIMESERIES:
            code = timeseries_validate((timeseries_t *) item);
            break;

        case MCL_ITEM_TYPE_FILE:
            code = file_validate((file_t *) item);
            break;

        case MCL_ITEM_TYPE_CUSTOM_DATA:
            code = custom_data_validate((custom_data_t *) item);
            break;

        case MCL_ITEM_TYPE_STORE:
            is_store = MCL_TRUE;
            code = _check_store_size((store_t *) item, connectivity_processor->max_http_payload_size);
            break;

        default:
            code = MCL_FAIL;
            break;
    }

    do
    {
        mcl_uint8_t *body = MCL_NULL;
        mcl_size_t body_size;
        mcl_http_request_t *request = MCL_NULL;
        mcl_http_response_t *response = MCL_NULL;
        char *boundary = MCL_NULL;
        char correlation_id[CORRELATION_ID_BUFFER_LENGTH];
        E_MCL_HTTP_METHOD method = MCL_HTTP_POST;

        // Generate boundary.
        if (MCL_OK == code)
        {
            code = multipart_generate_boundary(&boundary);
        }

        // Prepare HTTP request body.
        if (MCL_OK == code)
        {
            code = _prepare_body(item, connectivity_processor->max_http_payload_size, boundary, &body, &body_size);
        }

        if (MCL_OK == code)
        {
            code = mcl_http_request_initialize(&request);
        }

        if (MCL_OK == code)
        {
            code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_BODY, body);
        }

        if (MCL_OK == code)
        {
            code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_METHOD, &method);
        }

        if (MCL_OK == code)
        {
            code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_BODY_SIZE, &body_size);
        }

        if (MCL_OK == code)
        {
            code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_URL, connectivity_processor->exchange_url);
        }

        // Add http headers to the request.
        if (MCL_OK == code)
        {
            code = _add_http_headers_for_exchange(connectivity_processor, request, boundary);
        }

        // Free boundary since it is not necessary anymore.
        MCL_FREE(boundary);

        // Generate correlation id.
        if (MCL_OK == code)
        {
            code = _generate_correlation_id_string(correlation_id);
        }

        // Add correlation id to request for debugging purposes.
        if (MCL_OK == code)
        {
            code = mcl_http_request_add_header(request, "Correlation-ID", correlation_id);
        }

        // Send the request and get the response.
        if (MCL_OK == code)
        {
            code = mcl_http_client_send(connectivity_processor->http_client, request, &response);
        }

        // Destroy request since it is not necessary anymore.
        mcl_http_request_destroy(&request);
        MCL_FREE(body);

        // Evaluate the response.
        if (MCL_OK == code)
        {
            code = _evaluate_response_for_exchange(response, correlation_id);
        }

        // Destroy http response.
        mcl_http_response_destroy(&response);

        if (MCL_TRUE == is_store)
        {
            code = _scan_store_after_exchange((store_t *) item, &store_fully_processed, code);
        }
    } while ((MCL_TRUE == is_store) && (MCL_FALSE == store_fully_processed) && (MCL_OK == code));

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t connectivity_processor_create_mapping(connectivity_processor_t *connectivity_processor, mapping_t *mapping)
{
    mcl_error_t code;
    mcl_http_request_t *http_request = MCL_NULL;
    mcl_http_response_t *http_response = MCL_NULL;
    char *json_string = MCL_NULL;
    char correlation_id[CORRELATION_ID_BUFFER_LENGTH];

    MCL_DEBUG_ENTRY("connectivity_processor_t *connectivity_processor = <%p>, mapping_t *mapping = <%p>", connectivity_processor, mapping);

    // Validate mapping.
    code = mapping_validate(mapping);

    // Prepare json for http request.
    if (MCL_OK == code)
    {
        code = _prepare_mapping_json(mapping, connectivity_processor->agent_id, &json_string);
    }

    if (MCL_OK == code)
    {
        code = mcl_http_request_initialize(&http_request);
    }

    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_BODY, json_string);
    }

    if (MCL_OK == code)
    {
        mcl_size_t body_size = mcl_string_util_strlen(json_string);

        code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_BODY_SIZE, &body_size);
    }

    if (MCL_OK == code)
    {
        E_MCL_HTTP_METHOD method = MCL_HTTP_POST;

        code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_METHOD, &method);
    }

    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_URL, connectivity_processor->mapping_url);
    }

    // Generate correlation id.
    if (MCL_OK == code)
    {
        code = _generate_correlation_id_string(correlation_id);
    }

    // Add correlation id to request for debugging purposes.
    if (MCL_OK == code)
    {
        code = mcl_http_request_add_header(http_request, "Correlation-ID", correlation_id);
    }

    if (MCL_OK == code)
    {
        code = mcl_http_request_add_header(http_request, "Content-Type", content_type_values[CONTENT_TYPE_APPLICATION_JSON]);
    }

    if (MCL_OK == code)
    {
        code = mcl_http_request_add_header(http_request, "Accept", content_type_values[CONTENT_TYPE_APPLICATION_JSON]);
    }

    if (MCL_OK == code)
    {
        code = _add_authorization_header(http_request, connectivity_processor->access_token);
    }

    if (MCL_OK == code)
    {
        code = mcl_http_client_send(connectivity_processor->http_client, http_request, &http_response);
    }

    mcl_http_request_destroy(&http_request);
    MCL_FREE(json_string);

    if (MCL_OK == code)
    {
        if (MCL_HTTP_STATUS_CODE_CREATED == http_response->status_code)
        {
            MCL_INFO("HTTP 201 OK received from server for the request with correlation-id = \"%s\".", correlation_id);
            code = MCL_OK;
        }
        else
        {
            MCL_ERROR("HTTP <%d> received from server for the request with correlation-id = \"%s\".", http_response->status_code, correlation_id);

            if (MCL_NULL != http_response->payload)
            {
                MCL_ERROR("HTTP Response:\n%.*s", http_response->payload_size, http_response->payload);
            }

            code = mcl_http_response_get_status(http_response);

            if (MCL_OK == code)
            {
                // This means that HTTP 200 is received instead of 201.
                code = MCL_UNEXPECTED_RESULT_CODE;
            }
        }
    }

    mcl_http_response_destroy(&http_response);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t connectivity_processor_get_data_source_configuration(connectivity_processor_t *connectivity_processor, data_source_configuration_t **configuration)
{
    mcl_error_t code = MCL_OK;
    mcl_http_request_t *http_request = MCL_NULL;
    mcl_http_response_t *http_response = MCL_NULL;
    mcl_size_t agent_id_length;
    mcl_size_t hostname_length;
    mcl_size_t url_length;
    char *url;
    *configuration = MCL_NULL;

    MCL_DEBUG_ENTRY("connectivity_processor_t *connectivity_processor = <%p>, data_source_configuration_t **configuration = <%p>",
        connectivity_processor, configuration);

    // Calculate url length.
    agent_id_length = mcl_string_util_strlen(connectivity_processor->agent_id);
    hostname_length = mcl_string_util_strlen(connectivity_processor->hostname);
    url_length = hostname_length + agent_id_length + sizeof("/api/agentmanagement/v3/agents//dataSourceConfiguration");

    url = MCL_MALLOC(url_length);

    if (MCL_NULL == url)
    {
        code = MCL_OUT_OF_MEMORY;
    }
    else
    {
        char *position = url;

        mcl_string_util_memcpy(position, connectivity_processor->hostname, hostname_length);
        position += hostname_length;

        mcl_string_util_memcpy(position, "/api/agentmanagement/v3/agents/", sizeof("/api/agentmanagement/v3/agents/") - MCL_NULL_CHAR_SIZE);
        position += sizeof("/api/agentmanagement/v3/agents/") - MCL_NULL_CHAR_SIZE;

        mcl_string_util_memcpy(position, connectivity_processor->agent_id, agent_id_length);
        position += agent_id_length;

        mcl_string_util_memcpy(position, "/dataSourceConfiguration", sizeof("/dataSourceConfiguration"));
    }

    // Initialize http request.
    if (MCL_OK == code)
    {
        code = mcl_http_request_initialize(&http_request);
    }

    // Set url.
    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_URL, url);
    }

    MCL_FREE(url);

    // Set method.
    if (MCL_OK == code)
    {
        E_MCL_HTTP_METHOD method = MCL_HTTP_GET;

        code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_METHOD, &method);
    }

    // Add authorization header.
    if (MCL_OK == code)
    {
        code = _add_authorization_header(http_request, connectivity_processor->access_token);
    }

    // Response content type application/json.
    if (MCL_OK == code)
    {
        code = mcl_http_request_add_header(http_request, "Accept", content_type_values[CONTENT_TYPE_APPLICATION_JSON]);
    }

    // Send the request.
    if (MCL_OK == code)
    {
        code = mcl_http_client_send(connectivity_processor->http_client, http_request, &http_response);
    }

    mcl_http_request_destroy(&http_request);

    if (MCL_OK == code)
    {
        if (MCL_HTTP_STATUS_CODE_SUCCESS == http_response->status_code)
        {
            code = json_parse_item((char *) http_response->payload, http_response->payload_size, (void**)configuration);
        }
        else
        {
            code = mcl_http_response_get_status(http_response);
        }
    }

    mcl_http_response_destroy(&http_response);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _scan_store_after_exchange(store_t *store, mcl_bool_t *store_fully_processed, mcl_error_t exchange_response)
{
    mcl_error_t code = exchange_response;

    MCL_DEBUG_ENTRY("store_t *store = <%p>, mcl_bool_t *store_fully_processed = <%p>, mcl_error_t exchange_response = <%d>",
        store, store_fully_processed, exchange_response);

    if (MCL_OK == code)
    {
        *store_fully_processed = _remove_uploaded_store_items(store);
    }

    if (((MCL_TRUE == *store_fully_processed) && (0 != store->item_list->count)) || (MCL_OK != code))
    {
        _revert_store_item_status(store);

        // If code is MCL_OK, indicate that the store has at least one item.
        if (MCL_OK == code)
        {
            code = MCL_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE;
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _add_http_headers_for_exchange(connectivity_processor_t *processor, mcl_http_request_t *request, const char *boundary)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("connectivity_processor_t *processor = <%p>, mcl_http_request_t *request = <%p>, const char *boundary = <%p>",
        processor, request, boundary);

    // Add boundary.
    code = _add_multipart_mixed_content_type_header(request, boundary);

    if (MCL_OK == code)
    {
        code = mcl_http_request_add_header(request, "Accept", content_type_values[CONTENT_TYPE_APPLICATION_JSON]);
    }

    if (MCL_OK == code)
    {
        code = _add_authorization_header(request, processor->access_token);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _add_multipart_mixed_content_type_header(mcl_http_request_t *request, const char *boundary)
{
    mcl_error_t code;
    char *header_value;
    mcl_size_t header_value_length;
    mcl_size_t constant_part_length = 24;

    MCL_DEBUG_ENTRY("mcl_http_request_t *request = <%p>, const char *boundary = <%p>", request, boundary);

    header_value_length = mcl_string_util_strlen(content_type_values[CONTENT_TYPE_MULTIPART_MIXED]) + mcl_string_util_strlen(boundary) + constant_part_length;

    header_value = MCL_MALLOC(header_value_length + 1);

    if (MCL_NULL == header_value)
    {
        code = MCL_OUT_OF_MEMORY;
        MCL_ERROR_STRING("Memory can not be allocated for content-type header.");
    }
    else
    {
        // Compose header value.
        code = mcl_string_util_snprintf(header_value, header_value_length + 1, "%s;boundary=%s;charset=utf-8",
            content_type_values[CONTENT_TYPE_MULTIPART_MIXED], boundary);

        // Add header to http request.
        if (MCL_OK == code)
        {
            code = mcl_http_request_add_header(request, "Content-Type", header_value);
        }

        // Free header value.
        MCL_FREE(header_value);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _add_authorization_header(mcl_http_request_t *request, const char *access_token)
{
    mcl_error_t code;
    char *jwt = MCL_NULL;

    MCL_DEBUG_ENTRY("mcl_http_request_t *request = <%p>, const char *access_token = <%s>", request, access_token);

    // Add "Bearer" before access token.
    code = mcl_string_util_concatenate("Bearer ", access_token, &jwt);

    if (MCL_OK == code)
    {
        code = mcl_http_request_add_header(request, "Authorization", jwt);
    }

    MCL_FREE(jwt);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _evaluate_response_for_exchange(mcl_http_response_t *response, const char *correlation_id)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_http_response_t *response = <%p>, const char *correlation_id = <%p>", response, correlation_id);

    if (MCL_HTTP_STATUS_CODE_SUCCESS == response->status_code)
    {
        MCL_INFO("HTTP 200 OK received from server for the request with correlation-id = \"%s\".", correlation_id);
        code = MCL_OK;
    }
    else
    {
        MCL_ERROR("HTTP <%d> received from server for the request with correlation-id = \"%s\".", response->status_code, correlation_id);

        if (MCL_NULL != response->payload)
        {
            MCL_ERROR("HTTP Response:\n%.*s", response->payload_size, response->payload);
        }

        code = mcl_http_response_get_status(response);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _generate_correlation_id_string(char *correlation_id)
{
    mcl_error_t code;
    mcl_int16_t index;
    unsigned char *id = (unsigned char*) correlation_id;

    MCL_DEBUG_ENTRY("char *correlation_id = <%p>", correlation_id);

    code = mcl_random_generate_bytes(id + CORRELATION_ID_BYTE_LENGTH, CORRELATION_ID_BYTE_LENGTH);

    for (index = 0; (index < CORRELATION_ID_BYTE_LENGTH) && (MCL_OK == code); ++index)
    {
        code = mcl_string_util_snprintf(&(id[index * 2]), (mcl_size_t) 3, "%02x", (unsigned int)id[index + CORRELATION_ID_BYTE_LENGTH]);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_size_t _file_payload_callback(char *buffer, mcl_size_t size, mcl_size_t count, void *user_context)
{
    mcl_size_t written_size = 0;

    MCL_DEBUG_ENTRY("char *buffer = <%p>, mcl_size_t size = <%u>, mcl_size_t count = <%u>, void *user_context = <%p>", buffer, size, count, user_context);

    mcl_file_util_fread(buffer, size, count, ((file_t *) user_context)->payload->file_descriptor, &written_size);

    MCL_DEBUG_LEAVE("retVal = <%u>", written_size);
    return written_size;
}

static mcl_size_t _custom_data_payload_callback(char *buffer, mcl_size_t size, mcl_size_t count, void *user_context)
{
    custom_data_t *custom_data;
    mcl_size_t remaining_size;
    mcl_size_t write_size;

    MCL_DEBUG_ENTRY("char *buffer = <%p>, mcl_size_t size = <%u>, mcl_size_t count = <%u>, void *user_context = <%p>", buffer, size, count, user_context);

    // Calculate write size.
    custom_data = ((custom_data_callback_user_context *) user_context)->custom_data;
    remaining_size = custom_data->payload->size - ((custom_data_callback_user_context *) user_context)->offset;
    write_size = (remaining_size < (size * count)) ?  remaining_size : (size * count);

    if (0 != write_size)
    {
        mcl_string_util_memcpy(buffer, custom_data->payload->buffer + ((custom_data_callback_user_context *) user_context)->offset, write_size);
        ((custom_data_callback_user_context *) user_context)->offset += write_size;
    }

    MCL_DEBUG_LEAVE("retVal = <%u>", write_size);
    return write_size;
}

static mcl_error_t _add_item_to_buffer(char *buffer, mcl_size_t *buffer_size, mcl_item_t *item, const char *boundary)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("char *buffer = <%p>, mcl_size_t *buffer_size = <%p>, mcl_item_t *item = <%p>, const char *boundary = <%p>",
        buffer, buffer_size, item, boundary);

    switch (item->type)
    {
        case MCL_ITEM_TYPE_EVENT:
        case MCL_ITEM_TYPE_DATA_SOURCE_CONFIGURATION:
        case MCL_ITEM_TYPE_TIMESERIES:
            code = multipart_add_tuple(buffer, buffer_size, item, boundary);
            break;

        case MCL_ITEM_TYPE_FILE:
            mcl_file_util_rewind(((file_t *) item)->payload->file_descriptor);
            code = multipart_add_tuple_with_callback(buffer, buffer_size, item, boundary,
                content_type_values[CONTENT_TYPE_APPLICATION_OCTET_STREAM], _file_payload_callback, item);
            break;

        case MCL_ITEM_TYPE_CUSTOM_DATA:
            code = _add_custom_data_to_buffer(buffer, buffer_size, (custom_data_t *) item, boundary);
            break;

        default:
            code = MCL_FAIL;
            break;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_size_t _get_item_size(mcl_item_t *item)
{
    mcl_size_t item_size = 0;

    MCL_DEBUG_ENTRY("mcl_item_t *item = <%p>", item);

    switch (item->type)
    {
        case MCL_ITEM_TYPE_EVENT:
        case MCL_ITEM_TYPE_DATA_SOURCE_CONFIGURATION:
        case MCL_ITEM_TYPE_TIMESERIES:
            item_size = multipart_get_tuple_size(item, content_type_values[CONTENT_TYPE_APPLICATION_JSON]);
            break;

        case MCL_ITEM_TYPE_FILE:
            item_size = multipart_get_tuple_size(item, content_type_values[CONTENT_TYPE_APPLICATION_OCTET_STREAM]);
            break;

        case MCL_ITEM_TYPE_CUSTOM_DATA:
            item_size = multipart_get_tuple_size(item, ((custom_data_t *) item)->payload->content_type);
            break;

        default:
            break;
    }

    MCL_DEBUG_LEAVE("retVal = <%lu>", item_size);
    return item_size;
}

static mcl_error_t _check_store_size(store_t *store, mcl_size_t max_http_payload_size)
{
    mcl_error_t code = MCL_OK;
    mcl_list_node_t *item_node = MCL_NULL;
    mcl_size_t overhead_size;
    mcl_bool_t all_ignored = MCL_TRUE;

    MCL_DEBUG_ENTRY("store_t *store = <%p>, mcl_size_t max_http_payload_size = <%lu>", store, max_http_payload_size);

    if (0 == store->item_list->count)
    {
        code = MCL_STORE_IS_EMPTY;
    }
    else
    {
        overhead_size = multipart_get_overhead_size();
        mcl_list_reset(store->item_list);

        while (MCL_OK == mcl_list_next(store->item_list, &item_node))
        {
            store_item_t *store_item = (store_item_t *)item_node->data;
            store_item->item_size = _get_item_size(store_item->item);

            if (max_http_payload_size < (overhead_size + store_item->item_size))
            {
                store_item->status = STORE_ITEM_STATUS_IGNORED;
            }
            else
            {
                all_ignored = MCL_FALSE;
            }
        }

        if (MCL_TRUE == all_ignored)
        {
            code = MCL_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE;
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_size_t _select_store_items_to_exchange(store_t *store, mcl_size_t max_http_payload_size)
{
    mcl_list_node_t *item_node = MCL_NULL;
    mcl_size_t remaining_size = max_http_payload_size;
    mcl_size_t needed_buffer_size;

    MCL_DEBUG_ENTRY("store_t *store = <%p>, mcl_size_t max_http_payload_size = <%lu>", store, max_http_payload_size);

    remaining_size -= multipart_get_overhead_size();
    mcl_list_reset(store->item_list);

    while (MCL_OK == mcl_list_next(store->item_list, &item_node))
    {
        store_item_t *store_item = (store_item_t *)item_node->data;

        if (remaining_size >= store_item->item_size)
        {
            store_item->status = STORE_ITEM_STATUS_SELECTED;
            remaining_size -= store_item->item_size;
        }
    }

    needed_buffer_size = max_http_payload_size - remaining_size;
    mcl_list_reset(store->item_list);

    MCL_DEBUG_LEAVE("retVal = <%lu>", needed_buffer_size);
    return needed_buffer_size;
}

static store_item_t * _get_next_selected_store_item(store_t *store)
{
    mcl_list_node_t *item_node = MCL_NULL;
    store_item_t *store_item = MCL_NULL;

    MCL_DEBUG_ENTRY("store_t *store = <%p>", store);

    while (MCL_OK == mcl_list_next(store->item_list, &item_node))
    {
        store_item_t *current_store_item = (store_item_t *)item_node->data;

        if (STORE_ITEM_STATUS_SELECTED == current_store_item->status)
        {
            store_item = current_store_item;
            break;
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%p>", store_item);
    return store_item;
}

static mcl_error_t _prepare_body_for_store(store_t *store, mcl_size_t max_http_payload_size, char *boundary, mcl_uint8_t **body, mcl_size_t *body_size)
{
    mcl_size_t buffer_size;
    mcl_size_t remaining_size;
    mcl_error_t code;

    MCL_DEBUG_ENTRY("store_t *store = <%p>, mcl_size_t max_http_payload_size = <%lu>, char *boundary = <%p>, mcl_uint8_t **body = <%p>, "\
        "mcl_size_t *body_size = <%p>", store, max_http_payload_size, boundary, body, body_size);

    *body_size = 0;
    remaining_size = _select_store_items_to_exchange(store, max_http_payload_size);
    buffer_size = remaining_size;

    *body = MCL_MALLOC(buffer_size);

    if (MCL_NULL == *body)
    {
        code = MCL_OUT_OF_MEMORY;
    }
    else
    {
        for (;;)
        {
            store_item_t *store_item = _get_next_selected_store_item(store);

            if (MCL_NULL == store_item)
            {
                break;
            }

            code = _add_item_to_buffer((char *)(*body + (buffer_size - remaining_size)), &remaining_size, store_item->item, boundary);

            if (MCL_OK == code)
            {
                store_item->status = STORE_ITEM_STATUS_PROCESSED;
            }
            else
            {
                MCL_ERROR("Item <%p> could not be added to body. Error: %s", store_item->item, MCL_CONNECTIVITY_CODE_TO_STRING(code));
            }
        }

        if ((buffer_size != remaining_size) && (MCL_OK == multipart_close((char *)(*body + (buffer_size - remaining_size)), &remaining_size, boundary)))
        {
            *body_size = buffer_size - remaining_size;
            code = MCL_OK;
        }
        else
        {
            code = MCL_FAIL;
            MCL_FREE(*body);
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _prepare_body(mcl_item_t *item, mcl_size_t max_http_payload_size, char *boundary, mcl_uint8_t **body, mcl_size_t *body_size)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_item_t *item = <%p>, mcl_size_t max_http_payload_size = <%lu>, char *boundary = <%p>, mcl_uint8_t **body = <%p>, "\
        "mcl_size_t *body_size = <%lu>", item, max_http_payload_size, boundary, body, body_size);

    if (MCL_ITEM_TYPE_STORE == item->type)
    {
        code = _prepare_body_for_store((store_t *) item, max_http_payload_size, boundary, body, body_size);
    }
    else
    {
        mcl_size_t remaining_size;
        *body_size = _get_item_size(item) + multipart_get_overhead_size();
        remaining_size = *body_size;

        if (*body_size > max_http_payload_size)
        {
            code = MCL_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE;
        }

        if (MCL_OK == code)
        {
            *body = MCL_MALLOC(*body_size);

            if (MCL_NULL == *body)
            {
                code = MCL_OUT_OF_MEMORY;
                *body_size = 0;
            }
        }

        if (MCL_OK == code)
        {
            code = _add_item_to_buffer((char *) *body, &remaining_size, item, boundary);

            if (MCL_OK == code)
            {
                code = multipart_close((char *) (*body + (*body_size - remaining_size)), &remaining_size, boundary);
            }

            if (MCL_OK != code)
            {
                MCL_FREE(*body);
                *body_size = 0;
            }
            else if (0 != remaining_size)
            {
                *body_size -= remaining_size;
            }
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static void _revert_store_item_status(store_t *store)
{
    mcl_size_t index;
    mcl_size_t count = store->item_list->count;

    MCL_DEBUG_ENTRY("store_t *store = <%p>", store);

    mcl_list_reset(store->item_list);

    for (index = 0; index < count; ++index)
    {
        mcl_list_node_t *node = MCL_NULL;

        (void) mcl_list_next(store->item_list, &node);
        ((store_item_t *) node->data)->status = STORE_ITEM_STATUS_READY;
    }

    MCL_DEBUG_LEAVE("retVal = <void>");
}

static mcl_bool_t _remove_uploaded_store_items(store_t *store)
{
    mcl_bool_t fully_processed = MCL_TRUE;
    mcl_size_t index;
    mcl_size_t count = store->item_list->count;

    MCL_DEBUG_ENTRY("store_t *store = <%p>", store);

    mcl_list_reset(store->item_list);

    for (index = 0; index < count; ++index)
    {
        mcl_list_node_t *node = MCL_NULL;

        (void) mcl_list_next(store->item_list, &node);

        switch (((store_item_t *) node->data)->status)
        {
            case STORE_ITEM_STATUS_PROCESSED:
                (void) mcl_list_remove_with_content(store->item_list, node, (mcl_list_item_destroy_callback) store_item_destroy);
                break;

            case STORE_ITEM_STATUS_SELECTED:
                ((store_item_t *) node->data)->status = STORE_ITEM_STATUS_READY;
                fully_processed = MCL_FALSE;
                break;

            case STORE_ITEM_STATUS_READY:
                fully_processed = MCL_FALSE;
                break;

            default:
                break;
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%u>", fully_processed);
    return fully_processed;
}

static mcl_error_t _add_custom_data_to_buffer(char *buffer, mcl_size_t *buffer_size, custom_data_t *custom_data, const char *boundary)
{
    mcl_error_t code;
    custom_data_callback_user_context user_context;

    MCL_DEBUG_ENTRY("char *buffer = <%p>, mcl_size_t *buffer_size = <%p>, custom_data_t *custom_data = <%p>, const char *boundary = <%p>",
        buffer, buffer_size, custom_data, boundary);

    // Set user context.
    user_context.custom_data = custom_data;
    user_context.offset = 0;

    // Add custom data to http_request.
    code = multipart_add_tuple_with_callback(buffer, buffer_size, custom_data, boundary,
        custom_data->payload->content_type, _custom_data_payload_callback, &user_context);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _prepare_mapping_json(mapping_t *mapping, const char *agent_id, char **json)
{
    mcl_error_t code;
    mcl_json_t *json_object = MCL_NULL;
    *json = MCL_NULL;

    MCL_DEBUG_ENTRY("mapping_t *mapping = <%p>, const char *agent_id = <%p>, char **json = <%p>", mapping, agent_id, json);

    code = mcl_json_util_initialize(MCL_JSON_OBJECT, &json_object);

    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(json_object, "agentId", agent_id);
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(json_object, "dataPointId", mapping->data_point_id);
    }

    if (MCL_OK == code)
    {
        const char *entity_id;

        if (MCL_NULL == mapping->entity_id)
        {
            entity_id = agent_id;
        }
        else
        {
            entity_id = mapping->entity_id;
        }

        code = mcl_json_util_add_string(json_object, "entityId", entity_id);
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(json_object, "propertySetName", mapping->property_set_name);
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(json_object, "propertyName", mapping->property_name);
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_add_bool(json_object, "keepMapping", mapping->keep_mapping);
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_to_string(json_object, json);
    }

    mcl_json_util_destroy(&json_object);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
