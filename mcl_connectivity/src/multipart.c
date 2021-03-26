/*!
 * @file     multipart.c
 * @brief    Manages the multipart message structure of HTTP request.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "multipart.h"
#include "mcl_connectivity/mcl_connectivity_common.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_random.h"
#include "mcl_core/mcl_string_util.h"
#include "json.h"

static const char boundary_sign[] = "--";
static const char content_type[] = "Content-Type: ";
static const char boundary_key[] = ";boundary=";
static const char endline[] = "\r\n\r\n";
static const char boundary_characters[] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char content_type_multipart_related[] = "multipart/related";
static const char content_type_meta_json[] = "application/vnd.siemens.mindsphere.meta+json";
static const char content_type_application_json[] = "application/json";

#define BOUNDARY_SIGN_LENGTH        (sizeof(boundary_sign) - 1)
#define CONTENT_TYPE_LENGTH         (sizeof(content_type) - 1)
#define BOUNDARY_KEY_LENGTH         (sizeof(boundary_key) - 1)
#define DOUBLE_ENDLINE_LENGTH       (sizeof(endline) - 1)
#define ENDLINE_LENGTH              (DOUBLE_ENDLINE_LENGTH / 2)
#define BOUNDARY_LENGTH 20
#define CONTENT_TYPE_MULTIPART_RELATED_LENGTH       (sizeof(content_type_multipart_related) - 1)
#define CONTENT_TYPE_META_JSON_LENGTH               (sizeof(content_type_meta_json) - 1)
#define CONTENT_TYPE_APPLICATION_JSON_LENGTH        (sizeof(content_type_application_json) - 1)

#define MULTIPART_BASE_SIZE (5 * BOUNDARY_SIGN_LENGTH + 5 * BOUNDARY_LENGTH + 3 * CONTENT_TYPE_LENGTH + CONTENT_TYPE_MULTIPART_RELATED_LENGTH \
                             + BOUNDARY_KEY_LENGTH + CONTENT_TYPE_META_JSON_LENGTH + 12 * ENDLINE_LENGTH)

#define MULTIPART_CLOSE_LENGTH (BOUNDARY_LENGTH + 2 * BOUNDARY_SIGN_LENGTH)

/**
 * Multipart states.
 */
typedef enum
{
    MULTIPART_STATE_OPEN_MAIN_BOUNDARY = 0,
    MULTIPART_STATE_CONTENT_TYPE_KEY_FOR_MULTIPART_RELATED,
    MULTIPART_STATE_CONTENT_TYPE_VALUE_MULTIPART_RELATED,
    MULTIPART_STATE_BOUNDARY_KEY,
    MULTIPART_STATE_BOUNDARY_VALUE_SUB_BOUNDARY,
    MULTIPART_STATE_DOUBLE_ENDLINE_FIRST,
    MULTIPART_STATE_OPEN_SUB_BOUNDARY_FIRST,
    MULTIPART_STATE_CONTENT_TYPE_KEY_FOR_META_JSON,
    MULTIPART_STATE_CONTENT_TYPE_VALUE_META_JSON,
    MULTIPART_STATE_DOUBLE_ENDLINE_SECOND,
    MULTIPART_STATE_ITEM_META_JSON,
    MULTIPART_STATE_ENDLINE_FIRST,
    MULTIPART_STATE_OPEN_SUB_BOUNDARY_SECOND,
    MULTIPART_STATE_CONTENT_TYPE_KEY_FOR_PAYLOAD,
    MULTIPART_STATE_CONTENT_TYPE_VALUE_PAYLOAD,
    MULTIPART_STATE_DOUBLE_ENDLINE_THIRD,
    MULTIPART_STATE_ITEM_PAYLOAD,
    MULTIPART_STATE_ENDLINE_SECOND,
    MULTIPART_STATE_CLOSE_SUB_BOUNDARY,
    MULTIPART_STATE_END
} E_MULTIPART_STATE;

static const mcl_size_t _space_needed_for_state[MULTIPART_STATE_END] =
{
    // MULTIPART_STATE_OPEN_MAIN_BOUNDARY
    BOUNDARY_SIGN_LENGTH + BOUNDARY_LENGTH + ENDLINE_LENGTH,
    // MULTIPART_STATE_CONTENT_TYPE_KEY_FOR_MULTIPART_RELATED
    CONTENT_TYPE_LENGTH,
    // MULTIPART_STATE_CONTENT_TYPE_VALUE_MULTIPART_RELATED
    CONTENT_TYPE_MULTIPART_RELATED_LENGTH,
    // MULTIPART_STATE_BOUNDARY_KEY
    BOUNDARY_KEY_LENGTH,
    // MULTIPART_STATE_BOUNDARY_VALUE_SUB_BOUNDARY
    BOUNDARY_LENGTH,
    // MULTIPART_STATE_DOUBLE_ENDLINE_FIRST
    DOUBLE_ENDLINE_LENGTH,
    // MULTIPART_STATE_OPEN_SUB_BOUNDARY_FIRST
    BOUNDARY_SIGN_LENGTH + BOUNDARY_LENGTH + ENDLINE_LENGTH,
    // MULTIPART_STATE_CONTENT_TYPE_KEY_FOR_META_JSON
    CONTENT_TYPE_LENGTH,
    // MULTIPART_STATE_CONTENT_TYPE_VALUE_META_JSON
    CONTENT_TYPE_META_JSON_LENGTH,
    // MULTIPART_STATE_DOUBLE_ENDLINE_SECOND
    DOUBLE_ENDLINE_LENGTH,
    // MULTIPART_STATE_ITEM_META_JSON (variable, give as 0)
    0,
    // MULTIPART_STATE_ENDLINE_FIRST
    ENDLINE_LENGTH,
    // MULTIPART_STATE_OPEN_SUB_BOUNDARY_SECOND
    BOUNDARY_SIGN_LENGTH + BOUNDARY_LENGTH + ENDLINE_LENGTH,
    // MULTIPART_STATE_CONTENT_TYPE_KEY_FOR_PAYLOAD
    CONTENT_TYPE_LENGTH,
    // MULTIPART_STATE_CONTENT_TYPE_VALUE_PAYLOAD (variable, give as 0)
    0,
    // MULTIPART_STATE_DOUBLE_ENDLINE_THIRD
    DOUBLE_ENDLINE_LENGTH,
    // MULTIPART_STATE_ITEM_PAYLOAD (variable, give as 0)
    0,
    // MULTIPART_STATE_ENDLINE_SECOND
    ENDLINE_LENGTH,
    // MULTIPART_STATE_CLOSE_SUB_BOUNDARY
    (2 * BOUNDARY_SIGN_LENGTH) + BOUNDARY_LENGTH + ENDLINE_LENGTH
};

static void _add_boundary(char *buffer, const char *boundary, mcl_bool_t add_and_close);

static mcl_error_t _add_item_meta_json(char **position, mcl_size_t *remaining_size, void *item);

static mcl_error_t _add_item_payload_content_type(char **position, mcl_size_t *remaining_size, const char *payload_content_type);

static mcl_error_t _add_item_payload(char **position, mcl_size_t *remaining_size, void *item, multipart_add_payload_callback callback, void *user_context);

mcl_error_t multipart_add_tuple(char *buffer, mcl_size_t *buffer_size, void *item, const char *boundary)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("char *buffer = <%p>, mcl_size_t *buffer_size, void *item = <%p>, const char *boundary = <%p>", buffer, buffer_size, item, boundary);

    // Add tuple.
    code = multipart_add_tuple_with_callback(buffer, buffer_size, item, boundary, MCL_NULL, MCL_NULL, MCL_NULL);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t multipart_add_tuple_with_callback(char *buffer, mcl_size_t *buffer_size, void *item, const char *boundary, const char *payload_content_type,
    multipart_add_payload_callback callback, void *user_context)
{
    mcl_error_t code;
    char *sub_boundary = MCL_NULL;
    char *position = buffer;
    mcl_size_t remaining_size = *buffer_size;
    E_MULTIPART_STATE multipart_state = MULTIPART_STATE_OPEN_MAIN_BOUNDARY;

    MCL_DEBUG_ENTRY("char *buffer = <%p>, void *item = <%p>, const char *boundary = <%p>, const char *payload_content_type = <%p>, "\
        "multipart_add_payload_callback callback = <%p>, void *user_context = <%p>", buffer, item, boundary, payload_content_type, callback, user_context);

    code = multipart_generate_boundary(&sub_boundary);

    while (MULTIPART_STATE_END != multipart_state)
    {
        if ((0 == remaining_size) || (_space_needed_for_state[multipart_state] > remaining_size))
        {
            code = MCL_FAIL;
            break;
        }

        switch (multipart_state)
        {
            case MULTIPART_STATE_OPEN_MAIN_BOUNDARY:
                _add_boundary(position, boundary, MCL_FALSE);
                break;

            case MULTIPART_STATE_CONTENT_TYPE_KEY_FOR_MULTIPART_RELATED:
            case MULTIPART_STATE_CONTENT_TYPE_KEY_FOR_META_JSON:
            case MULTIPART_STATE_CONTENT_TYPE_KEY_FOR_PAYLOAD:
                mcl_string_util_memcpy(position, content_type, CONTENT_TYPE_LENGTH);
                break;

            case MULTIPART_STATE_CONTENT_TYPE_VALUE_MULTIPART_RELATED:
                mcl_string_util_memcpy(position, content_type_multipart_related, CONTENT_TYPE_MULTIPART_RELATED_LENGTH);
                break;

            case MULTIPART_STATE_BOUNDARY_KEY:
                mcl_string_util_memcpy(position, boundary_key, BOUNDARY_KEY_LENGTH);
                break;

            case MULTIPART_STATE_BOUNDARY_VALUE_SUB_BOUNDARY:
                mcl_string_util_memcpy(position, sub_boundary, BOUNDARY_LENGTH);
                break;

            case MULTIPART_STATE_DOUBLE_ENDLINE_FIRST:
            case MULTIPART_STATE_DOUBLE_ENDLINE_SECOND:
            case MULTIPART_STATE_DOUBLE_ENDLINE_THIRD:
                mcl_string_util_memcpy(position, endline, DOUBLE_ENDLINE_LENGTH);
                break;

            case MULTIPART_STATE_OPEN_SUB_BOUNDARY_FIRST:
            case MULTIPART_STATE_OPEN_SUB_BOUNDARY_SECOND:
                _add_boundary(position, sub_boundary, MCL_FALSE);
                break;

            case MULTIPART_STATE_CLOSE_SUB_BOUNDARY:
                _add_boundary(position, sub_boundary, MCL_TRUE);
                break;

            case MULTIPART_STATE_CONTENT_TYPE_VALUE_META_JSON:
                mcl_string_util_memcpy(position, content_type_meta_json, CONTENT_TYPE_META_JSON_LENGTH);
                break;

            case MULTIPART_STATE_ITEM_META_JSON:
                code = _add_item_meta_json(&position, &remaining_size, item);
                break;

            case MULTIPART_STATE_ENDLINE_FIRST:
            case MULTIPART_STATE_ENDLINE_SECOND:
                mcl_string_util_memcpy(position, endline, ENDLINE_LENGTH);
                break;

            case MULTIPART_STATE_CONTENT_TYPE_VALUE_PAYLOAD:
                code = _add_item_payload_content_type(&position, &remaining_size, payload_content_type);
                break;

            case MULTIPART_STATE_ITEM_PAYLOAD:
                code = _add_item_payload(&position, &remaining_size, item, callback, user_context);
                break;

            default:
                break;
        }

        if (MCL_OK != code)
        {
            break;
        }

        remaining_size -= _space_needed_for_state[multipart_state];
        position += _space_needed_for_state[multipart_state];
        ++multipart_state;
    }

    MCL_FREE(sub_boundary);

    if (MCL_OK == code)
    {
        *buffer_size = remaining_size;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_size_t multipart_get_tuple_size(void *item, const char *payload_content_type)
{
    mcl_size_t size = MULTIPART_BASE_SIZE;

    MCL_DEBUG_ENTRY("void *item = <%p>, const char *payload_content_type = <%p>", item, payload_content_type);

    size += json_get_item_size(item);
    size += mcl_string_util_strlen(payload_content_type);

    MCL_DEBUG_LEAVE("retVal = <%lu>", size);
    return size;
}

mcl_error_t multipart_close(char *buffer, mcl_size_t *buffer_size, const char *boundary)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("char *buffer, mcl_size_t *buffer_size = <%p>, const char *boundary = <%s>", buffer, buffer_size, boundary);

    if (*buffer_size >= MULTIPART_CLOSE_LENGTH)
    {
        mcl_string_util_memcpy(buffer, boundary_sign, BOUNDARY_SIGN_LENGTH);
        mcl_string_util_memcpy(buffer + BOUNDARY_SIGN_LENGTH, boundary, BOUNDARY_LENGTH);
        mcl_string_util_memcpy(buffer + BOUNDARY_SIGN_LENGTH + BOUNDARY_LENGTH, boundary_sign, BOUNDARY_SIGN_LENGTH);

        (*buffer_size) -= MULTIPART_CLOSE_LENGTH;
    }
    else
    {
        code = MCL_FAIL;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_size_t multipart_get_overhead_size(void)
{
    MCL_DEBUG_ENTRY("<void>");
    MCL_DEBUG_LEAVE("retVal = <%lu>", (long unsigned) MULTIPART_CLOSE_LENGTH);
    return MULTIPART_CLOSE_LENGTH;
}

mcl_error_t multipart_generate_boundary(char **boundary)
{
    mcl_error_t code;
    mcl_size_t index;

    MCL_DEBUG_ENTRY("char **boundary = <%p>", boundary);

    // Allocate for boundary string.
    *boundary = MCL_MALLOC(BOUNDARY_LENGTH + 1);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != *boundary, MCL_OUT_OF_MEMORY, "Memory can not be allocated for boundary.");

    // Make sure boundary string is null terminated.
    (*boundary)[BOUNDARY_LENGTH] = MCL_NULL_CHAR;

    // Generate random numbers for boundary characters.
    code = mcl_random_generate_bytes((unsigned char *) *boundary, BOUNDARY_LENGTH);
    MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, MCL_FREE(*boundary), code, "Random number generation for boundary failed.");

    // Pick boundary characters randomly.
    for (index = 0; index < BOUNDARY_LENGTH; ++index)
    {
        (*boundary)[index] = boundary_characters[((mcl_uint8_t) (*boundary)[index]) % (sizeof(boundary_characters) - 1)];
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static void _add_boundary(char *buffer, const char *boundary, mcl_bool_t add_and_close)
{
    char *position = buffer;

    MCL_DEBUG_ENTRY("char *buffer = <%p>, const char *boundary = <%p>, mcl_bool_t add_and_close = <%d>", buffer, boundary, add_and_close);

    mcl_string_util_memcpy(position, boundary_sign, BOUNDARY_SIGN_LENGTH);
    position += BOUNDARY_SIGN_LENGTH;

    mcl_string_util_memcpy(position, boundary, BOUNDARY_LENGTH);
    position += BOUNDARY_LENGTH;

    if (MCL_TRUE == add_and_close)
    {
        mcl_string_util_memcpy(position, boundary_sign, BOUNDARY_SIGN_LENGTH);
        position += BOUNDARY_SIGN_LENGTH;
    }

    mcl_string_util_memcpy(position, endline, ENDLINE_LENGTH);

    MCL_DEBUG_LEAVE("retVal = <void>");
}

static mcl_error_t _add_item_meta_json(char **position, mcl_size_t *remaining_size, void *item)
{
    mcl_error_t code;
    char *item_meta_json = MCL_NULL;

    code = json_from_item_meta(item, &item_meta_json);

    if (MCL_OK == code)
    {
        mcl_size_t item_meta_json_length = mcl_string_util_strlen(item_meta_json);

        if (*remaining_size >= item_meta_json_length)
        {
            mcl_string_util_memcpy(*position, item_meta_json, item_meta_json_length);
            (*remaining_size) -= item_meta_json_length;
            (*position) += item_meta_json_length;
        }
        else
        {
            code = MCL_FAIL;
        }
    }

    MCL_FREE(item_meta_json);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _add_item_payload_content_type(char **position, mcl_size_t *remaining_size, const char *payload_content_type)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t length;
    const char *source;

    if (MCL_NULL == payload_content_type)
    {
        length = CONTENT_TYPE_APPLICATION_JSON_LENGTH;
        source = content_type_application_json;
    }
    else
    {
        length = mcl_string_util_strlen(payload_content_type);
        source = payload_content_type;
    }

    if (*remaining_size >= length)
    {
        mcl_string_util_memcpy(*position, source, length);
        (*position) += length;
        (*remaining_size) -= length;
    }
    else
    {
        code = MCL_FAIL;
    }

    return code;
}

static mcl_error_t _add_item_payload(char **position, mcl_size_t *remaining_size, void *item, multipart_add_payload_callback callback, void *user_context)
{
    mcl_error_t code = MCL_OK;

    if (MCL_NULL == callback)
    {
        char *payload = MCL_NULL;

        code = json_from_item_payload(item, &payload);

        if (MCL_OK == code)
        {
            mcl_size_t payload_length = mcl_string_util_strlen(payload);

            if (*remaining_size >= payload_length)
            {
                mcl_string_util_memcpy(*position, payload, payload_length);
                (*position) += payload_length;
                (*remaining_size) -= payload_length;
            }
            else
            {
                code = MCL_FAIL;
            }

            MCL_FREE(payload);
        }
    }
    else
    {
        mcl_size_t callback_value;

        do
        {
            callback_value = callback(*position, 1, *remaining_size, user_context);
            (*position) += callback_value;
            (*remaining_size) -= callback_value;
        } while ((0 != callback_value) && (0 != *remaining_size));

        if ((0 == *remaining_size) && (0 != callback_value))
        {
            code = MCL_FAIL;
        }
    }

    return code;
}
