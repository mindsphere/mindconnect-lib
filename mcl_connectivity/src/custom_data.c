/*!
 * @file     custom_data.c
 * @brief    Custom data module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_connectivity/mcl_custom_data.h"
#include "custom_data.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

mcl_error_t mcl_custom_data_initialize(const char *version, mcl_custom_data_t **custom_data)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("const char version = <%p>, mcl_custom_data_t  **custom_data = <%p>", version, custom_data);

    // Null check.
    MCL_ASSERT_NOT_NULL(custom_data, code);
    MCL_ASSERT_NOT_NULL(version, code);

    // Allocate memory for custom data.
    if (MCL_NULL != MCL_NEW(*custom_data))
    {
        // Set base parameters for custom data.
        (*custom_data)->item_base.preamble = MCL_ITEM_PREAMBLE;
        (*custom_data)->item_base.type = MCL_ITEM_TYPE_CUSTOM_DATA;
        (*custom_data)->item_base.version = 0;

        // Allocate memory for custom_data payload.
        MCL_NEW((*custom_data)->payload);
    }

    if ((MCL_NULL != (*custom_data)) && (MCL_NULL != (*custom_data)->payload))
    {
        (*custom_data)->payload->version = MCL_NULL;
        code = mcl_string_util_reset(version, &((*custom_data)->payload->version));
        (*custom_data)->payload->buffer = MCL_NULL;
        (*custom_data)->payload->content_type = MCL_NULL;
        (*custom_data)->payload->type = MCL_NULL;
        (*custom_data)->payload->details = MCL_NULL;
        (*custom_data)->payload->size = 0;
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    if (MCL_OK != code)
    {
        mcl_custom_data_destroy(custom_data);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_custom_data_set_parameter(mcl_custom_data_t *custom_data, E_MCL_CUSTOM_DATA_PARAMETER parameter, const void *value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_custom_data_t *custom_data = <%p>, E_MCL_CUSTOM_DATA_PARAMETER parameter = <%d>, const void *value = <%p>", custom_data, parameter, value);

    // Null check.
    MCL_ASSERT_NOT_NULL(custom_data, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_CUSTOM_DATA_PARAMETER_CONTENT_TYPE:
            code = mcl_string_util_reset(value, &custom_data->payload->content_type);
            break;

        case MCL_CUSTOM_DATA_PARAMETER_TYPE:
            code = mcl_string_util_reset(value, &custom_data->payload->type);
            break;

        case MCL_CUSTOM_DATA_PARAMETER_BUFFER:
            custom_data->payload->buffer = (const mcl_uint8_t *) value;
            break;

        case MCL_CUSTOM_DATA_PARAMETER_BUFFER_SIZE:
            custom_data->payload->size = *(const mcl_size_t *) value;
            break;

        case MCL_CUSTOM_DATA_PARAMETER_DETAILS:
            mcl_json_util_destroy(&custom_data->payload->details);
            code = mcl_json_util_duplicate((const mcl_json_t *) value, &custom_data->payload->details);
            break;

        default:
            code = MCL_INVALID_PARAMETER;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t custom_data_validate(custom_data_t *custom_data)
{
    mcl_error_t code = MCL_OK;
    custom_data_payload_t *payload = custom_data->payload;

    MCL_DEBUG_ENTRY("custom_data_t *custom_data = <%p>", custom_data);

    // Checks whether all mandatory parameters of a custom data are set or not.
    if ((MCL_NULL == payload->buffer) || (MCL_NULL == payload->content_type) || (MCL_NULL == payload->type) || (MCL_NULL == payload->version) || (0 == payload->size))
    {
        code = MCL_INVALID_PARAMETER;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_custom_data_destroy(mcl_custom_data_t **custom_data)
{
    MCL_DEBUG_ENTRY("mcl_custom_data_t **custom_data = <%p>", custom_data);

    // Destroys the custom data data structure.
    if ((MCL_NULL != custom_data) && (MCL_NULL != *custom_data))
    {
        if (MCL_NULL != (*custom_data)->payload)
        {
            // Destroy payload.
            MCL_FREE((*custom_data)->payload->version);
            MCL_FREE((*custom_data)->payload->content_type);
            MCL_FREE((*custom_data)->payload->type);
            mcl_json_util_destroy(&((*custom_data)->payload->details));
            MCL_FREE((*custom_data)->payload);
        }

        MCL_FREE(*custom_data);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
