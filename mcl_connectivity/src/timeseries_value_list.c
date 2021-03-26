/*!
 * @file     timeseries_value_list.c
 * @brief    Timeseries value list module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_connectivity/mcl_timeseries_value_list.h"
#include "timeseries_value_list.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

mcl_error_t mcl_timeseries_value_list_initialize(mcl_timeseries_value_list_t **timeseries_value_list)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_timeseries_value_list_t **timeseries_value_list = <%p>", timeseries_value_list);

    // Null check.
    MCL_ASSERT_NOT_NULL(timeseries_value_list, code);

    // Allocate memory for timeseries value list.
    if (MCL_NULL != MCL_NEW(*timeseries_value_list))
    {
        // Set initial values.
        (*timeseries_value_list)->values = MCL_NULL;
        (*timeseries_value_list)->timestamp[0] = MCL_NULL_CHAR;

        // Initialize list for values.
        code = mcl_list_initialize(&(*timeseries_value_list)->values);
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    // Error check.
    if (MCL_OK != code)
    {
        mcl_timeseries_value_list_destroy(timeseries_value_list);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_timeseries_value_list_set_parameter(mcl_timeseries_value_list_t *timeseries_value_list,
    E_MCL_TIMESERIES_VALUE_LIST_PARAMETER parameter, const void *value)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_timeseries_value_list_t *timeseries_value_list = <%p>, E_MCL_TIMESERIES_VALUE_LIST_PARAMETER parameter = <%d>, "\
        "const void *value = <%p>", timeseries_value_list, parameter, value);

    // Null check.
    MCL_ASSERT_NOT_NULL(timeseries_value_list, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601:
            if (MCL_TRUE == mcl_time_util_validate_timestamp((const char *) value))
            {
                mcl_string_util_memcpy(timeseries_value_list->timestamp, value, MCL_TIMESTAMP_LENGTH);
                code = MCL_OK;
            }
            else
            {
                code = MCL_INVALID_PARAMETER;
            }

            break;

        case MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_EPOCH:
            code = mcl_time_util_convert_to_iso_8601_format((const time_t *) value, timeseries_value_list->timestamp);
            break;

        default:
            code = MCL_INVALID_PARAMETER;
            break;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t timeseries_value_list_validate(timeseries_value_list_t *timeseries_value_list)
{
    mcl_error_t code = MCL_OK;
    mcl_list_t *values = timeseries_value_list->values;
    mcl_size_t index;

    MCL_DEBUG_ENTRY("timeseries_value_list_t *timeseries_value_list = <%p>", timeseries_value_list);

    // Check timestamp.
    if (MCL_TRUE != mcl_time_util_validate_timestamp(timeseries_value_list->timestamp))
    {
        code = MCL_INVALID_PARAMETER;
        MCL_ERROR_STRING("Timeseries value list timestamp is not set.");
    }
    else if (0 == values->count)
    {
        code = MCL_INVALID_PARAMETER;
        MCL_ERROR_STRING("Timeseries value list has no value.");
    }
    else
    {
        mcl_list_reset(values);
    }

    // Validate each value.
    for (index = 0; (index < values->count) && (MCL_OK == code); ++index)
    {
        mcl_list_node_t *node = MCL_NULL;

        (void) mcl_list_next(values, &node);

        if ((MCL_NULL == node) || (MCL_OK != timeseries_value_validate((timeseries_value_t *) (node->data))))
        {
            code = MCL_INVALID_PARAMETER;
            MCL_ERROR_STRING("Timeseries value is not valid.");
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_timeseries_value_list_add_value(mcl_timeseries_value_list_t *timeseries_value_list, mcl_timeseries_value_t *timeseries_value)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_timeseries_value_list_t *timeseries_value_list = <%p>, mcl_timeseries_value_t *timeseries_value = <%p>",
        timeseries_value_list, timeseries_value);

    // Null check.
    MCL_ASSERT_NOT_NULL(timeseries_value_list, code);
    MCL_ASSERT_NOT_NULL(timeseries_value, code);

    // Add timeseries value to timeseries value list.
    code = mcl_list_add(timeseries_value_list->values, timeseries_value);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_timeseries_value_list_destroy(mcl_timeseries_value_list_t **timeseries_value_list)
{
    MCL_DEBUG_ENTRY("mcl_timeseries_value_list_t **timeseries_value_list = <%p>", timeseries_value_list);

    // Destroys the timeseries value list data structure.
    if ((MCL_NULL != timeseries_value_list) && (MCL_NULL != *timeseries_value_list))
    {
        mcl_list_destroy_with_content(&(*timeseries_value_list)->values, (mcl_list_item_destroy_callback) mcl_timeseries_value_destroy);
        MCL_FREE(*timeseries_value_list);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
