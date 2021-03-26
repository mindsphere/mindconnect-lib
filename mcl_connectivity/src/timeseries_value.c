/*!
 * @file     timeseries_value.c
 * @brief    Timeseries value module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_connectivity/mcl_timeseries_value.h"
#include "timeseries_value.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

mcl_error_t mcl_timeseries_value_initialize(mcl_timeseries_value_t **timeseries_value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_timeseries_value_t **timeseries_value = <%p>", timeseries_value);

    // Null check.
    MCL_ASSERT_NOT_NULL(timeseries_value, code);

    // Allocate memory for timeseries value.
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != MCL_NEW(*timeseries_value), MCL_OUT_OF_MEMORY, "Not enough memory to allocate new timeseries value.");

    // Set initial values.
    (*timeseries_value)->data_point_id = MCL_NULL;
    (*timeseries_value)->quality_code = MCL_NULL;
    (*timeseries_value)->value = MCL_NULL;

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_timeseries_value_set_parameter(mcl_timeseries_value_t *timeseries_value, E_MCL_TIMESERIES_VALUE_PARAMETER parameter, const void *value)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_timeseries_value_t *timeseries_value = <%p>, E_MCL_TIMESERIES_VALUE_PARAMETER parameter = <%d>, const void *value = <%p>",
        timeseries_value, parameter, value);

    // Null check.
    MCL_ASSERT_NOT_NULL(timeseries_value, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_TIMESERIES_VALUE_PARAMETER_DATA_POINT_ID:
            code = mcl_string_util_reset(value, &timeseries_value->data_point_id);
            break;

        case MCL_TIMESERIES_VALUE_PARAMETER_VALUE:
            code = mcl_string_util_reset(value, &timeseries_value->value);
            break;

        case MCL_TIMESERIES_VALUE_PARAMETER_QUALITY_CODE:
            code = mcl_string_util_reset(value, &timeseries_value->quality_code);
            break;

        default:
            code = MCL_INVALID_PARAMETER;
            break;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t timeseries_value_validate(timeseries_value_t *timeseries_value)
{
    MCL_DEBUG_ENTRY("timeseries_value_t *timeseries_value = <%p>", timeseries_value);

    // Timeseries value validation checks.
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != timeseries_value->data_point_id, MCL_INVALID_PARAMETER, "Timeseries value data point id is not set.");
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != timeseries_value->quality_code, MCL_INVALID_PARAMETER, "Timeseries value quality code is not set.");
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != timeseries_value->value, MCL_INVALID_PARAMETER, "Timeseries value value is not set.");

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void mcl_timeseries_value_destroy(mcl_timeseries_value_t **timeseries_value)
{
    MCL_DEBUG_ENTRY("mcl_timeseries_value_t **timeseries_value = <%p>", timeseries_value);

    // Destroys the timeseries value data structure.
    if ((MCL_NULL != timeseries_value) && (MCL_NULL != *timeseries_value))
    {
        MCL_FREE((*timeseries_value)->data_point_id);
        MCL_FREE((*timeseries_value)->quality_code);
        MCL_FREE((*timeseries_value)->value);
        MCL_FREE(*timeseries_value);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
