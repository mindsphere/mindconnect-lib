/*!
 * @file     data_lake_processor.c
 * @brief    Data lake processor module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "data_lake_processor.h"
#include "data_lake_object.h"
#include "data_lake_json.h"
#include "mcl_data_lake/mcl_data_lake_common.h"
#include "mcl_data_lake/mcl_data_lake_configuration.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

#define SINGLE_OBJECT_SIZE 1

static const char bearer_format[] = "Bearer %s";
static const char string_identifier[] = "%s";
static const char application_json[] = "application/json";
static const char content_type[] = "Content-Type";
static const char azure_storage_url[] = "blob.core.windows.net";
static const char azure_header_blob_type[] = "x-ms-blob-type";
static const char azure_header_block_blob[] = "BlockBlob";

// Function used to add authorization header to the request.
static mcl_error_t _add_authorization_header(mcl_http_request_t *request, const char *access_token);

// Since signed URLs may not be in the same order with the request body, array may save time.
// It also helps to implement a function which can handle both single object and multiple objects.
static data_lake_object_t** _data_lake_object_list_to_array(mcl_list_t *object_list);

// Function to get signed urls from the server.
static mcl_error_t _generate_upload_urls(data_lake_processor_t *processor, data_lake_object_t **object_array, mcl_size_t array_size, const char *subtenant_id);

// Function to clear signed urls.
static void _clear_signed_urls(data_lake_object_t **object_array, mcl_size_t array_size);

// Check signed URL if data lake service uses Azure Storage.
static mcl_bool_t _check_url_if_it_is_for_azure_storage(data_lake_object_t *object);

mcl_error_t data_lake_processor_generate_upload_url(data_lake_processor_t *processor, data_lake_object_t *object, const char *subtenant_id)
{
    mcl_error_t code;
    data_lake_object_t *single_object = object;

    MCL_DEBUG_ENTRY("data_lake_processor_t *processor = <%p>, data_lake_object_t *object = <%p>, const char *subtenant_id = <%p>",
        processor, object, subtenant_id);

    code = _generate_upload_urls(processor, &single_object, SINGLE_OBJECT_SIZE, subtenant_id);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t data_lake_processor_generate_upload_urls(data_lake_processor_t *processor, mcl_list_t *object_list, const char *subtenant_id)
{
    mcl_error_t code;
    data_lake_object_t **object_array;

    MCL_DEBUG_ENTRY("data_lake_processor_t *processor = <%p>, mcl_list_t *object_list = <%p>, const char *subtenant_id = <%p>",
        processor, object_list, subtenant_id);

    object_array = _data_lake_object_list_to_array(object_list);

    if (MCL_NULL == object_array)
    {
        code = MCL_OUT_OF_MEMORY;
    }
    else
    {
        code = _generate_upload_urls(processor, object_array, object_list->count, subtenant_id);
    }

    MCL_FREE(object_array);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t data_lake_processor_upload(data_lake_processor_t *processor, data_lake_object_t *object)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_http_request_t *request = MCL_NULL;
    mcl_http_response_t *response = MCL_NULL;
    E_MCL_HTTP_METHOD method = MCL_HTTP_PUT;

    MCL_DEBUG_ENTRY("data_lake_processor_t *processor = <%p>, data_lake_object_t *object = <%p>", processor, object);

    // Validate object.
    code = data_lake_object_validate(object);

    // Initialize request.
    if (MCL_OK == code)
    {
        code = mcl_http_request_initialize(&request);
    }

    // Set method.
    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_METHOD, &method);
    }

    // Set URL.
    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_URL, object->signed_url);
    }

    // Set callback.
    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_STREAM_CALLBACK, object->upload_callback);
    }

    // Set user context.
    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_STREAM_DATA, object->user_context);
    }

    // Set payload size.
    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_BODY_SIZE, &(object->size));
    }

    if ((MCL_OK == code) && (MCL_TRUE == _check_url_if_it_is_for_azure_storage(object)))
    {
        code = mcl_http_request_add_header(request, azure_header_blob_type, azure_header_block_blob);
    }

    // Send request.
    if (MCL_OK == code)
    {
        code = mcl_http_client_send(processor->http_client, request, &response);
    }

    mcl_http_request_destroy(&request);

    if (MCL_OK == code)
    {
        if ((MCL_HTTP_STATUS_CODE_SUCCESS == response->status_code) || (MCL_HTTP_STATUS_CODE_CREATED == response->status_code))
        {
            MCL_INFO("Uploaded to data lake successfully.");
        }
        else
        {
            code = mcl_http_response_get_status(response);
            MCL_ERROR("HTTP %d received from server for the request.", response->status_code);

            if (MCL_NULL != response->payload)
            {
                MCL_ERROR("HTTP Response:\n%.*s", response->payload_size, response->payload);
            }
        }
    }

    mcl_http_response_destroy(&response);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _add_authorization_header(mcl_http_request_t *request, const char *access_token)
{
    // Variable declaration.
    mcl_error_t code = MCL_OUT_OF_MEMORY;
    mcl_size_t jwt_length;
    char *jwt;

    MCL_DEBUG_ENTRY("mcl_http_request_t *request = <%p>, const char *access_token = <%p>", request, access_token);

    // Size of jwt including null character.
    jwt_length = (sizeof(bearer_format) - sizeof(string_identifier)) + mcl_string_util_strlen(access_token) + MCL_NULL_CHAR_SIZE;

    // Allocate buffer for jwt.
    jwt = MCL_MALLOC(jwt_length);

    if (MCL_NULL != jwt)
    {
        code = mcl_string_util_snprintf(jwt, jwt_length, bearer_format, access_token);

        if (MCL_OK == code)
        {
            code = mcl_http_request_add_header(request, "Authorization", jwt);
        }

        MCL_FREE(jwt);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _generate_upload_urls(data_lake_processor_t *processor, data_lake_object_t **object_array, mcl_size_t array_size, const char *subtenant_id)
{
    mcl_error_t code = MCL_OK;
    mcl_http_request_t *request = MCL_NULL;
    mcl_size_t client_id_length;
    mcl_http_response_t *response = MCL_NULL;
    char *body = MCL_NULL;
    E_MCL_HTTP_METHOD method = MCL_HTTP_POST;

    MCL_DEBUG_ENTRY("data_lake_processor_t *processor = <%p>, data_lake_object_t **object_array = <%p>, mcl_size_t array_size = <%lu>, "\
        "const char *subtenant_id = <%p>", processor, object_array, array_size, subtenant_id);

    // First clear signed urls.
    _clear_signed_urls(object_array, array_size);

    // Calculate client id length.
    client_id_length = mcl_string_util_strlen(processor->client_id);

    // Create json body.
    code = data_lake_json_from_objects(object_array, array_size, processor->client_id, subtenant_id, &body);

    // Initialize request.
    if (MCL_OK == code)
    {
        code = mcl_http_request_initialize(&request);
    }

    // Set method.
    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_METHOD, &method);
    }

    // Set URL.
    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_URL, processor->upload_url_generation_url);
    }

    // Set body.
    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_BODY, body);
    }

    // Set body size.
    if (MCL_OK == code)
    {
        mcl_size_t body_size = mcl_string_util_strlen(body);

        code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_BODY_SIZE, &body_size);
    }

    // Add content type header.
    if (MCL_OK == code)
    {
        code = mcl_http_request_add_header(request, content_type, application_json);
    }

    // Add authorization header.
    if (MCL_OK == code)
    {
        code = _add_authorization_header(request, processor->access_token);
    }

    // Send request.
    if (MCL_OK == code)
    {
        code = mcl_http_client_send(processor->http_client, request, &response);
    }

    MCL_FREE(body);
    mcl_http_request_destroy(&request);

    if (MCL_OK == code)
    {
        if (MCL_HTTP_STATUS_CODE_CREATED == response->status_code)
        {
            code = data_lake_json_match_signed_urls_with_objects(object_array, array_size, (char *)response->payload, response->payload_size, client_id_length);
        }
        else
        {
            code = mcl_http_response_get_status(response);
            MCL_ERROR("HTTP %d received from server for the request.", response->status_code);

            if (MCL_NULL != response->payload)
            {
                MCL_ERROR("HTTP Response:\n%.*s", response->payload_size, response->payload);
            }
        }
    }

    mcl_http_response_destroy(&response);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static void _clear_signed_urls(data_lake_object_t **object_array, mcl_size_t array_size)
{
    mcl_size_t index;
    MCL_DEBUG_ENTRY("data_lake_object_t **object_array = <%p>, mcl_size_t array_size = <%lu>", object_array, array_size);

    for (index = 0; index < array_size; ++index)
    {
        MCL_FREE(object_array[index]->signed_url);
    }

    MCL_DEBUG_LEAVE("retVal = <void>");
}

static data_lake_object_t** _data_lake_object_list_to_array(mcl_list_t *object_list)
{
    data_lake_object_t **data_lake_object_array;
    mcl_size_t count = object_list->count;

    MCL_DEBUG_ENTRY("mcl_list_t *object_list = <%p>", object_list);

    // Size of pointer, not actual struct.
    data_lake_object_array = MCL_MALLOC(count * sizeof(data_lake_object_t *));

    if (MCL_NULL != data_lake_object_array)
    {
        mcl_size_t index;

        for (index = 0; index < count; ++index)
        {
            mcl_list_node_t *node;

            // Get next data lake object.
            (void) mcl_list_next(object_list, &node);

            data_lake_object_array[index] = (mcl_data_lake_object_t *) node->data;
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%p>", data_lake_object_array);
    return data_lake_object_array;
}

static mcl_bool_t _check_url_if_it_is_for_azure_storage(data_lake_object_t *object)
{
    mcl_bool_t is_for_azure = MCL_FALSE;
    mcl_int32_t index = 0;
    mcl_size_t url_length;

    MCL_DEBUG_ENTRY("data_lake_object_t *object = <%p>", object);

    url_length = mcl_string_util_strlen(object->signed_url);

    while (('.' != object->signed_url[index++]) && (index < url_length))
    {
        // Empty body.
    }

    if ((index < (url_length - sizeof(azure_storage_url))) &&
        (MCL_TRUE == mcl_string_util_memcmp(&(object->signed_url[index]), azure_storage_url, sizeof(azure_storage_url) - MCL_NULL_CHAR_SIZE)))
    {
        is_for_azure = MCL_TRUE;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", (int)is_for_azure);
    return is_for_azure;
}
