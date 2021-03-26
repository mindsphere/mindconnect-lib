/*!
 * @file     deployment_processor.c
 * @brief    Implementation of deployment processor.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "deployment_processor.h"
#include "deployment_json.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

// Constants.
static const char application_json[] = "application/json";
static const char content_type[] = "Content-Type";

// Function used to add authorization header to the request.
static mcl_error_t _add_authorization_header(mcl_http_request_t *request, const char *access_token);

// Function used to get url to list workflows including query parameters.
static mcl_error_t _get_url_to_list_workflows(char *path, deployment_workflow_filter_t *filter, char **url);

// Function used to get url to get and patch a workflow including query parameters.
static mcl_error_t _get_url_to_get_patch_workflow(char *path, char *id, deployment_workflow_filter_t *filter, char **url);

// Function to get query string from filter parameters for a single workflos.
static mcl_error_t _get_query_string_for_single_workflow(deployment_workflow_filter_t *filter, char **query_string);

// Function to get query string from filter parameters for list of workflows.
static mcl_error_t _get_query_string_for_list_of_workflows(deployment_workflow_filter_t *filter, char **query_string);

// Function to add a query parameter to destination buffer.
static char *_add_query_parameter(char *to, char *key, char *value);

// Function to calculate length of the query string.
static mcl_size_t _calculate_query_string_length(const deployment_workflow_filter_t *filter);

mcl_error_t deployment_processor_list_workflows(deployment_processor_t *deployment_processor, deployment_workflow_filter_t *filter, mcl_list_t **workflows)
{
    mcl_error_t code;
    mcl_http_request_t *http_request = MCL_NULL;
    mcl_http_response_t *http_response = MCL_NULL;
    char *url = MCL_NULL;

    MCL_DEBUG_ENTRY("deployment_processor_t *deployment_processor = <%p>, deployment_workflow_filter_t *filter = <%p>, mcl_list_t **workflows = <%p>",
        deployment_processor, filter, workflows);

    // Initialize an http request.
    code = mcl_http_request_initialize(&http_request);

    // Set method of http request.
    if (MCL_OK == code)
    {
        E_MCL_HTTP_METHOD method = MCL_HTTP_GET;
        code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_METHOD, &method);
    }

    // Get url.
    if (MCL_OK == code)
    {
        code = _get_url_to_list_workflows(deployment_processor->workflow_instances_url, filter, &url);
    }

    // Set URL of http request.
    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_URL, url);
    }

    // Free temporarily allocated memory.
    MCL_FREE(url);

    // Add authorization header to http request.
    if (MCL_OK == code)
    {
        code = _add_authorization_header(http_request, deployment_processor->access_token);
    }

    // Add http header to accept application/json as the response content type.
    if (MCL_OK == code)
    {
        code = mcl_http_request_add_header(http_request, "Accept", application_json);
    }

    // Send the http request.
    if (MCL_OK == code)
    {
        MCL_INFO("Getting the list of deployment workflows.");
        code = mcl_http_client_send(deployment_processor->http_client, http_request, &http_response);
    }

    mcl_http_request_destroy(&http_request);

    if (MCL_OK == code)
    {
        if (MCL_HTTP_STATUS_CODE_SUCCESS == http_response->status_code)
        {
            MCL_INFO("Received the list of deployment workflows.");
            code = deployment_json_parse_workflows((char *) http_response->payload, http_response->payload_size, workflows);
        }
        else
        {
            code = mcl_http_response_get_status(http_response);
            MCL_INFO("Failed to get list of deployment workflows. MCL error code : %s.", MCL_DEPLOYMENT_CODE_TO_STRING(code));
        }
    }

    mcl_http_response_destroy(&http_response);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t deployment_processor_get_workflow(deployment_processor_t *deployment_processor, char *id, deployment_workflow_filter_t *filter,
    deployment_workflow_t **workflow)
{
    mcl_error_t code;
    mcl_http_request_t *http_request = MCL_NULL;
    mcl_http_response_t *http_response = MCL_NULL;
    char *url = MCL_NULL;

    MCL_DEBUG_ENTRY("deployment_processor_t *deployment_processor = <%p>, char *id = <%p>, deployment_workflow_filter_t *filter = <%p>, "\
        "deployment_workflow_t **workflow = <%p>", deployment_processor, id, filter, workflow);

    // Initialize an http request.
    code = mcl_http_request_initialize(&http_request);

    // Set method of http request.
    if (MCL_OK == code)
    {
        E_MCL_HTTP_METHOD method = MCL_HTTP_GET;
        code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_METHOD, &method);
    }

    // Concatenate workflow id to the workflow instances url.
    if (MCL_OK == code)
    {
        code = _get_url_to_get_patch_workflow(deployment_processor->workflow_instances_url, id, filter, &url);
    }

    // Set URL of http request.
    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_URL, url);
    }

    // Free temporarily allocated memory.
    MCL_FREE(url);

    // Add authorization header to http request.
    if (MCL_OK == code)
    {
        code = _add_authorization_header(http_request, deployment_processor->access_token);
    }

    // Add http header to accept application/json as the response content type.
    if (MCL_OK == code)
    {
        code = mcl_http_request_add_header(http_request, "Accept", application_json);
    }

    // Send the http request.
    if (MCL_OK == code)
    {
        MCL_INFO("Getting the description of deployment workflow with id %s.", id);
        code = mcl_http_client_send(deployment_processor->http_client, http_request, &http_response);
    }

    mcl_http_request_destroy(&http_request);

    if (MCL_OK == code)
    {
        if (MCL_HTTP_STATUS_CODE_SUCCESS == http_response->status_code)
        {
            MCL_INFO("Received the deployment workflow with id %s.", id);
            code = deployment_json_parse_workflow((char*)http_response->payload, http_response->payload_size, workflow);
        }
        else
        {
            code = mcl_http_response_get_status(http_response);
            MCL_INFO("Failed to get deployment workflow with id %s. MCL error code : %s.", id, MCL_DEPLOYMENT_CODE_TO_STRING(code));
        }
    }

    mcl_http_response_destroy(&http_response);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t deployment_processor_update_workflow(deployment_processor_t *deployment_processor, char *id, deployment_workflow_state_t *state,
    deployment_workflow_filter_t *filter, deployment_workflow_t **workflow)
{
    mcl_error_t code;
    mcl_http_request_t *http_request = MCL_NULL;
    mcl_http_response_t *http_response = MCL_NULL;
    char *url = MCL_NULL;
    char *body = MCL_NULL;

    MCL_DEBUG_ENTRY("deployment_processor_t *deployment_processor = <%p>, char *id = <%p>, deployment_workflow_state_t *state = <%p>, "\
        "deployment_workflow_filter_t *filter = <%p>, deployment_workflow_t **workflow = <%p>", deployment_processor, id, state, filter, workflow);

    // Initialize an http request.
    code = mcl_http_request_initialize(&http_request);

    // Set method of http request.
    if (MCL_OK == code)
    {
        E_MCL_HTTP_METHOD method = MCL_HTTP_PATCH;
        code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_METHOD, &method);
    }

    // Concatenate workflow id to the workflow instances url.
    if (MCL_OK == code)
    {
        code = _get_url_to_get_patch_workflow(deployment_processor->workflow_instances_url, id, filter, &url);
    }

    // Set URL of http request.
    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_URL, url);
    }

    // Free temporarily allocated memory.
    MCL_FREE(url);

    // Validate workflow state.
    if (MCL_OK == code)
    {
        code = deployment_workflow_state_validate(state);
    }

    // Create json body.
    if (MCL_OK == code)
    {
        code = deployment_json_from_workflow_state(state, &body);
    }

    // Set body.
    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_BODY, body);
    }

    // Set body size.
    if (MCL_OK == code)
    {
        mcl_size_t body_size = mcl_string_util_strlen(body);

        code = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_BODY_SIZE, &body_size);
    }

    // Add content type header.
    if (MCL_OK == code)
    {
        code = mcl_http_request_add_header(http_request, content_type, application_json);
    }

    // Add authorization header to http request.
    if (MCL_OK == code)
    {
        code = _add_authorization_header(http_request, deployment_processor->access_token);
    }

    // Add http header to accept application/json as the response content type.
    if (MCL_OK == code)
    {
        code = mcl_http_request_add_header(http_request, "Accept", application_json);
    }

    // Send the http request.
    if (MCL_OK == code)
    {
        MCL_INFO("Updating the state of deployment workflow with id %s.", id);
        code = mcl_http_client_send(deployment_processor->http_client, http_request, &http_response);
    }

    MCL_FREE(body);
    mcl_http_request_destroy(&http_request);

    if (MCL_OK == code)
    {
        if (MCL_HTTP_STATUS_CODE_SUCCESS == http_response->status_code)
        {
            MCL_INFO("Updated deployment workflow with id %s.", id);
            code = deployment_json_parse_workflow((char*)http_response->payload, http_response->payload_size, workflow);
        }
        else
        {
            code = mcl_http_response_get_status(http_response);
            MCL_INFO("Failed to update deployment workflow with id %s. MCL error code : %s.", id, MCL_DEPLOYMENT_CODE_TO_STRING(code));
        }
    }

    mcl_http_response_destroy(&http_response);

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

static mcl_error_t _get_url_to_get_patch_workflow(char *path, char *id, deployment_workflow_filter_t *filter, char **url)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t path_length;
    mcl_size_t id_length;
    mcl_size_t url_length;
    mcl_size_t query_length = 0;
    char *query_string = MCL_NULL;

    MCL_DEBUG_ENTRY("char *path = <%p>, char *id = <%p>, deployment_workflow_filter_t *filter = <%p>, char **url = <%p>", path, id, filter, url);

    // Calculate lengths.
    path_length = mcl_string_util_strlen(path);
    id_length = mcl_string_util_strlen(id);

    // Get query string if it exists.
    if (MCL_NULL != filter)
    {
        code = _get_query_string_for_single_workflow(filter, &query_string);

        // Get length of query string.
        if (MCL_OK == code)
        {
            query_length = mcl_string_util_strlen(query_string);
        }
    }

    // Allocate memory for url.
    url_length = path_length + mcl_string_util_strlen("/") + id_length + query_length;
    *url = MCL_MALLOC(url_length + MCL_NULL_CHAR_SIZE);

    if (MCL_NULL == *url)
    {
        code = MCL_OUT_OF_MEMORY;
    }
    else
    {
        char *position = *url;

        mcl_string_util_memcpy(position, path, path_length);
        position += path_length;

        mcl_string_util_memcpy(position, "/", 1);
        position += 1;

        mcl_string_util_memcpy(position, id, id_length + MCL_NULL_CHAR_SIZE);
        position += id_length;

        if (query_length != 0)
        {
            mcl_string_util_memcpy(position, query_string, query_length + MCL_NULL_CHAR_SIZE);
        }
    }

    MCL_FREE(query_string);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _get_url_to_list_workflows(char *path, deployment_workflow_filter_t *filter, char **url)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t path_length;
    mcl_size_t url_length;
    mcl_size_t query_length = 0;
    char *query_string = MCL_NULL;

    MCL_DEBUG_ENTRY("char *path = <%p>, deployment_workflow_filter_t *filter = <%p>, char **url = <%p>", path, filter, url);

    // Calculate path length.
    path_length = mcl_string_util_strlen(path);

    // Get query string.
    if (MCL_NULL != filter)
    {
        code = _get_query_string_for_list_of_workflows(filter, &query_string);

        // Get length of query string.
        if (MCL_OK == code)
        {
            query_length = mcl_string_util_strlen(query_string);
        }
    }

    // Allocate memory for url.
    url_length = path_length + query_length;
    *url = MCL_MALLOC(url_length + MCL_NULL_CHAR_SIZE);

    if (MCL_NULL == *url)
    {
        code = MCL_OUT_OF_MEMORY;
    }
    else
    {
        char *position = *url;

        mcl_string_util_memcpy(position, path, path_length + MCL_NULL_CHAR_SIZE);
        position += path_length;

        if (query_length != 0)
        {
            mcl_string_util_memcpy(position, query_string, query_length + MCL_NULL_CHAR_SIZE);
        }
    }

    MCL_FREE(query_string);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _get_query_string_for_single_workflow(deployment_workflow_filter_t *filter, char **query_string)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t query_string_length = 0;

    // model=true&
    const mcl_size_t model_query_length = 11;

    // history=true&
    const mcl_size_t history_query_length = 13;

    MCL_DEBUG_ENTRY("deployment_workflow_filter_t *filter = <%p>, char **query_string = <%p>", filter, query_string);

    // Make sure query string is null initially.
    *query_string = MCL_NULL;

    if (MCL_TRUE == filter->model)
    {
        query_string_length += model_query_length;
    }

    if (MCL_TRUE == filter->history)
    {
        query_string_length += history_query_length;
    }

    // If there are any query parameters.
    if (0 != query_string_length)
    {
        *query_string = MCL_MALLOC(query_string_length + MCL_NULL_CHAR_SIZE);

        if (MCL_NULL != *query_string)
        {
            char *position = *query_string;

            // Start query string.
            mcl_string_util_memcpy(position, "?", 1);
            position += 1;

            if (MCL_TRUE == filter->model)
            {
                position = _add_query_parameter(position, "model", "true");
            }

            if (MCL_TRUE == filter->history)
            {
                _add_query_parameter(position, "history", "true");
            }

            // Finalize the query string.
            (*query_string)[query_string_length] = MCL_NULL_CHAR;
        }
        else
        {
            code = MCL_OUT_OF_MEMORY;
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _get_query_string_for_list_of_workflows(deployment_workflow_filter_t *filter, char **query_string)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("deployment_workflow_filter_t *filter = <%p>, char **query_string = <%p>", filter, query_string);

    // Set query string to null initially.
    *query_string = MCL_NULL;

    // Calculate query string length.
    mcl_size_t query_string_length = _calculate_query_string_length(filter);

    // If there are any query parameters.
    if (0 != query_string_length)
    {
        // Allocate memory for the whole query string.
        *query_string = MCL_MALLOC(query_string_length + MCL_NULL_CHAR_SIZE);

        if (MCL_NULL != *query_string)
        {
            char *position = *query_string;

            // Start query string.
            mcl_string_util_memcpy(position, "?", 1);
            position += 1;

            if (MCL_TRUE == filter->model)
            {
                position = _add_query_parameter(position, "model", "true");
            }

            if (MCL_TRUE == filter->history)
            {
                position = _add_query_parameter(position, "history", "true");
            }

            position = _add_query_parameter(position, "currentState", filter->current_state);
            position = _add_query_parameter(position, "deviceId", filter->device_id);
            position = _add_query_parameter(position, "group", filter->group);
            _add_query_parameter(position, "modelKey", filter->model_key);

            // Finalize the query string.
            (*query_string)[query_string_length] = MCL_NULL_CHAR;
        }
        else
        {
            code = MCL_OUT_OF_MEMORY;
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static char *_add_query_parameter(char *to, char *key, char *value)
{
    char *position = to;
    mcl_size_t value_length = mcl_string_util_strlen(value);

    MCL_DEBUG_ENTRY("char *to = <%p>, char *key = <%p>, char *value = <%p>", to, key, value);

    if (0 != value_length)
    {
        mcl_size_t key_length = mcl_string_util_strlen(key);

        mcl_string_util_memcpy(position, key, key_length);
        position += key_length;

        mcl_string_util_memcpy(position, "=", 1);
        position += 1;

        mcl_string_util_memcpy(position, value, value_length);
        position += value_length;

        mcl_string_util_memcpy(position, "&", 1);
        position += 1;
    }

    MCL_DEBUG_LEAVE("retVal = <%p>", position);
    return position;
}

static mcl_size_t _calculate_query_string_length(const deployment_workflow_filter_t *filter)
{
    const mcl_size_t model_overhead = 11;
    const mcl_size_t history_overhead = 13;
    const mcl_size_t current_state_overhead = 14;
    const mcl_size_t device_id_overhead = 10;
    const mcl_size_t group_overhead = 7;
    const mcl_size_t model_key_overhead = 10;
    mcl_size_t query_string_length = 0;

    // Get lengths of each query parameter.
    mcl_size_t model_length = 0;
    mcl_size_t history_length = 0;
    mcl_size_t current_state_length = mcl_string_util_strlen(filter->current_state);
    mcl_size_t device_id_length = mcl_string_util_strlen(filter->device_id);
    mcl_size_t group_length = mcl_string_util_strlen(filter->group);
    mcl_size_t model_key_length = mcl_string_util_strlen(filter->model_key);

    // Update lengths with key names and &.
    if (MCL_TRUE == filter->model)
    {
        // model=true&
        model_length = model_overhead;
    }

    if (MCL_TRUE == filter->history)
    {
        // history=true&
        history_length = history_overhead;
    }

    if (0 != current_state_length)
    {
        // currentState={}&
        current_state_length += current_state_overhead;
    }

    if (0 != device_id_length)
    {
        // deviceId={}&
        device_id_length += device_id_overhead;
    }

    if (0 != group_length)
    {
        // group={}&
        group_length += group_overhead;
    }

    if (0 != model_key_length)
    {
        // modelKey={}&
        model_key_length += model_key_overhead;
    }

    // Calculate length of whole query string.
    query_string_length = model_length + history_length + current_state_length + device_id_length + group_length + model_key_length;

    return query_string_length;
}
