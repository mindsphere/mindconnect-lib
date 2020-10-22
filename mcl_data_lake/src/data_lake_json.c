/*!
 * @file     data_lake_json.c
 * @brief    Data lake json module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "data_lake_json.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"
#include "mcl_core/mcl_json_util.h"

static const char generate_url_body_pre_path_list[] = "{\"paths\":[";
static const char generate_url_body_pre_path_object[] = "{\"path\":\"";
static const char generate_url_body_post_path_object[] = "\"}";
static const char object_urls_key[] = "objectUrls";
static const char signed_url_key[] = "signedUrl";
static const char object_path_key[] = "path";
static const char subtenant_id_key[] = ",\"subtenantId\":\"";

#define GENERATE_URL_BODY_PRE_PATH_LIST_LENGTH   (sizeof(generate_url_body_pre_path_list) - MCL_NULL_CHAR_SIZE)
#define GENERATE_URL_BODY_POST_PATH_LIST_LENGTH  (sizeof("]}") - MCL_NULL_CHAR_SIZE)
#define GENERATE_URL_BODY_PRE_PATH_OBJECT_LENGTH   (sizeof(generate_url_body_pre_path_object) - MCL_NULL_CHAR_SIZE)
#define GENERATE_URL_BODY_POST_PATH_OBJECT_LENGTH  (sizeof(generate_url_body_post_path_object) - MCL_NULL_CHAR_SIZE)
#define SUBTENANT_KEY_LENGTH  (sizeof(subtenant_id_key) - MCL_NULL_CHAR_SIZE)
#define SUBTENANT_OVERHEAD (SUBTENANT_KEY_LENGTH + sizeof("\"") - MCL_NULL_CHAR_SIZE)
#define DELIMITER_LENGTH 1
#define SLASH_LENGTH 1
#define SINGLE_CHARACTER_SIZE 1

// Calculate total size for object paths.
static mcl_size_t _calculate_total_path_size(data_lake_object_t **object_array, mcl_size_t array_size, mcl_size_t *valid_path_count);

// Find matching object and set signed url.
static mcl_error_t _find_matching_object(mcl_json_t *object_url_item, data_lake_object_t **object_array, mcl_size_t array_size, char *object_path, mcl_size_t compare_size);

// Function to check if there is any object with path but without signed url.
static mcl_error_t _check_signed_urls(data_lake_object_t **object_array, mcl_size_t array_size);

// Function to calculate body size for request to generate upload urls.
static mcl_size_t _data_lake_json_get_body_size_generate_upload_urls(data_lake_object_t **object_array, mcl_size_t array_size, mcl_size_t client_id_length, mcl_size_t subtenant_id_length);

mcl_error_t data_lake_json_from_objects(data_lake_object_t **object_array, mcl_size_t array_size, const char *client_id, const char *subtenant_id, char **json)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t index;
    mcl_bool_t is_first_object = MCL_TRUE;
    mcl_size_t client_id_length;
    mcl_size_t subtenant_id_length = 0;
    mcl_size_t body_size;
    char *position = MCL_NULL;

    MCL_DEBUG_ENTRY("data_lake_object_t *object_array = <%p>, mcl_size_t array_size = <%lu>, const char *client_id = <%p>, const char *subtenant_id = <%p>, char **json = <%p>", object_array, array_size, client_id, subtenant_id, json);

    client_id_length = mcl_string_util_strlen(client_id);

    if (MCL_NULL != subtenant_id)
    {
        subtenant_id_length = mcl_string_util_strlen(subtenant_id);
    }

    body_size = _data_lake_json_get_body_size_generate_upload_urls(object_array, array_size, client_id_length, subtenant_id_length);

    if (0 == body_size)
    {
        code = MCL_INVALID_PARAMETER;
    }
    else
    {
        *json = MCL_MALLOC(body_size + MCL_NULL_CHAR_SIZE);

        if (MCL_NULL == *json)
        {
            code = MCL_OUT_OF_MEMORY;
        }
        else
        {
            position = *json;
        }
    }

    if (MCL_OK == code)
    {
        // Prepare payload.
        mcl_string_util_memcpy(position, generate_url_body_pre_path_list, GENERATE_URL_BODY_PRE_PATH_LIST_LENGTH);
        position += GENERATE_URL_BODY_PRE_PATH_LIST_LENGTH;

        // Array of paths.
        for (index = 0; (index < array_size) && (MCL_OK == code); ++index)
        {
            mcl_size_t path_length;

            if (MCL_NULL == object_array[index]->path)
            {
                continue;
            }

            if (MCL_FALSE == is_first_object)
            {
                mcl_string_util_memcpy(position, ",", DELIMITER_LENGTH);
                position += DELIMITER_LENGTH;
            }

            mcl_string_util_memcpy(position, generate_url_body_pre_path_object, GENERATE_URL_BODY_PRE_PATH_OBJECT_LENGTH);
            position += GENERATE_URL_BODY_PRE_PATH_OBJECT_LENGTH;

            mcl_string_util_memcpy(position, client_id, client_id_length);
            position += client_id_length;

            mcl_string_util_memcpy(position, "/", SLASH_LENGTH);
            position += SLASH_LENGTH;

            path_length = mcl_string_util_strlen(object_array[index]->path);
            mcl_string_util_memcpy(position, object_array[index]->path, path_length);
            position += path_length;

            mcl_string_util_memcpy(position, generate_url_body_post_path_object, GENERATE_URL_BODY_POST_PATH_OBJECT_LENGTH);
            position += GENERATE_URL_BODY_POST_PATH_OBJECT_LENGTH;

            if (MCL_TRUE == is_first_object)
            {
                is_first_object = MCL_FALSE;
            }
        }

        // Finish array.
        mcl_string_util_memcpy(position, "]", SINGLE_CHARACTER_SIZE);
        ++position;

        // Add subtenant id.
        if (0 != subtenant_id_length)
        {
            mcl_string_util_memcpy(position, subtenant_id_key, SUBTENANT_KEY_LENGTH);
            position += SUBTENANT_KEY_LENGTH;

            mcl_string_util_memcpy(position, subtenant_id, subtenant_id_length);
            position += subtenant_id_length;

            mcl_string_util_memcpy(position, "\"", SINGLE_CHARACTER_SIZE);
            ++position;
        }

        // Finish json.
        mcl_string_util_memcpy(position, "}", SINGLE_CHARACTER_SIZE);
        (*json)[body_size] = MCL_NULL_CHAR;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t data_lake_json_match_signed_urls_with_objects(data_lake_object_t **object_array, mcl_size_t array_size, char *json, mcl_size_t json_size, mcl_size_t client_id_length)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t index;
    mcl_size_t object_urls_array_size = 0;
    mcl_json_t *response_json = MCL_NULL;
    mcl_json_t *object_urls_array = MCL_NULL;
    mcl_size_t path_overhead_length = client_id_length + SLASH_LENGTH;

    MCL_DEBUG_ENTRY("data_lake_object_t **object_array = <%p>, mcl_size_t array_size = <%lu>, char *json = <%p>, mcl_size_t json_size = <%lu>, mcl_size_t client_id_length = <%lu>", object_array, array_size, json, json_size, client_id_length);

    if (0 == json_size)
    {
        code = MCL_INVALID_PARAMETER;
    }

    if (MCL_OK == code)
    {
        // Parse json.
        code = mcl_json_util_parse(json, json_size, &response_json);
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_get_object_item(response_json, object_urls_key, &object_urls_array);
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_get_array_size(object_urls_array, &object_urls_array_size);
    }

    for (index = 0; (index < object_urls_array_size) && (MCL_OK == code); ++index)
    {
        mcl_size_t object_path_length;
        mcl_json_t *object_url_item = MCL_NULL;
        mcl_json_t *object_path_json = MCL_NULL;
        char *object_path = MCL_NULL;

        code = mcl_json_util_get_array_item(object_urls_array, (int) index, &object_url_item);

        if (MCL_OK == code)
        {
            code = mcl_json_util_get_object_item(object_url_item, object_path_key, &object_path_json);
        }

        if (MCL_OK == code)
        {
            code = mcl_json_util_get_string(object_path_json, &object_path);
        }

        if (MCL_OK == code)
        {
            object_path_length = mcl_string_util_strlen(object_path);
        }

        if (MCL_OK == code)
        {
            if (object_path_length > path_overhead_length)
            {
                // Adding MCL_NULL_CHAR_SIZE to compare with null character.
                _find_matching_object(object_url_item, object_array, array_size, object_path + path_overhead_length, (object_path_length - path_overhead_length) + MCL_NULL_CHAR_SIZE);
            }
            else
            {
                MCL_WARN("Received invalid path %s", object_path);
            }
        }

        MCL_FREE(object_path);
    }

    // Clean up.
    mcl_json_util_destroy(&response_json);

    // Check signed urls.
    if (MCL_OK == code)
    {
        code = _check_signed_urls(object_array, array_size);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_size_t _calculate_total_path_size(data_lake_object_t **object_array, mcl_size_t array_size, mcl_size_t *valid_path_count)
{
    mcl_size_t total_path_size = 0;
    mcl_size_t index;
    *valid_path_count = 0;

    MCL_DEBUG_ENTRY("data_lake_object_array **object_array = <%p>, mcl_size_t array_size = <%lu>, mcl_size_t *valid_path_count = <%p>", object_array, array_size, valid_path_count);

    for (index = 0; index < array_size; ++index)
    {
        if (MCL_NULL == object_array[index]->path)
        {
            MCL_WARN("Data lake object <%p> has no path to request signed URL.", object_array[index]);
        }
        else
        {
            ++(*valid_path_count);
            total_path_size += mcl_string_util_strlen(object_array[index]->path);
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%lu>", total_path_size);
    return total_path_size;
}

static mcl_error_t _find_matching_object(mcl_json_t *object_url_item, data_lake_object_t **object_array, mcl_size_t array_size, char *object_path, mcl_size_t compare_size)
{
    mcl_size_t object_array_index;
    mcl_error_t code = MCL_OK;

    for (object_array_index = 0; (object_array_index < array_size) && (MCL_OK == code); ++object_array_index)
    {
        char *signed_url = MCL_NULL;
        mcl_json_t *signed_url_json = MCL_NULL;

        // If signed url is not NULL, it is already set.
        if (MCL_NULL != object_array[object_array_index]->signed_url)
        {
            continue;
        }

        // Compare with + MCL_NULL_CHAR_SIZE to include NULL character.
        if (MCL_OK == mcl_string_util_strncmp(object_path, object_array[object_array_index]->path, compare_size))
        {
            code = mcl_json_util_get_object_item(object_url_item, signed_url_key, &signed_url_json);

            if (MCL_OK == code)
            {
                code = mcl_json_util_get_string(signed_url_json, &signed_url);
            }

            if (MCL_OK == code)
            {
                object_array[object_array_index]->signed_url = signed_url;
            }

            break;
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _check_signed_urls(data_lake_object_t **object_array, mcl_size_t array_size)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t index;
    MCL_DEBUG_ENTRY("data_lake_object_t **object_array = <%p>, mcl_size_t array_size = <%lu>", object_array, array_size);

    for (index = 0; index < array_size; ++index)
    {
        if ((MCL_NULL != object_array[index]->path) && (MCL_NULL == object_array[index]->signed_url))
        {
            code = MCL_DATA_LAKE_SIGNED_URL_GENERATION_FAIL;
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_size_t _data_lake_json_get_body_size_generate_upload_urls(data_lake_object_t **object_array, mcl_size_t array_size, mcl_size_t client_id_length, mcl_size_t subtenant_id_length)
{
    mcl_size_t total_path_length;
    mcl_size_t valid_path_count;
    mcl_size_t path_overhead_length;
    mcl_size_t body_size;

    MCL_DEBUG_ENTRY("data_lake_object_t **object_array = <%p>, mcl_size_t array_size = <%lu>, mcl_size_t client_id_length = <%lu>, mcl_size_t subtenant_id_length = <%lu>", object_array, array_size, client_id_length, subtenant_id_length);

    total_path_length = _calculate_total_path_size(object_array, array_size, &valid_path_count);

    if (0 == valid_path_count)
    {
        body_size = 0;
    }
    else
    {
        path_overhead_length = client_id_length + SLASH_LENGTH;
        body_size = GENERATE_URL_BODY_PRE_PATH_LIST_LENGTH + GENERATE_URL_BODY_POST_PATH_LIST_LENGTH;
        body_size += (GENERATE_URL_BODY_PRE_PATH_OBJECT_LENGTH + GENERATE_URL_BODY_POST_PATH_OBJECT_LENGTH + path_overhead_length) * valid_path_count;
        body_size += total_path_length + ((valid_path_count - 1) * DELIMITER_LENGTH);

        if (0 != subtenant_id_length)
        {
            body_size += SUBTENANT_OVERHEAD;
            body_size += subtenant_id_length;
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%lu>", body_size);
    return body_size;
}
