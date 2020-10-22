/*!
 * @file     data_point.c
 * @brief    Data point module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_connectivity/mcl_data_point.h"
#include "data_point.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

mcl_error_t mcl_data_point_initialize(mcl_data_point_t **data_point)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_data_point_t **data_point = <%p>", data_point);

    // Null check.
    MCL_ASSERT_NOT_NULL(data_point, code);

    // Allocate memory for data point.
    if (MCL_NULL != MCL_NEW(*data_point))
    {
        // Set initial values.
        (*data_point)->custom_data = MCL_NULL;
        (*data_point)->description = MCL_NULL;
        (*data_point)->id = MCL_NULL;
        (*data_point)->name = MCL_NULL;
        (*data_point)->type = MCL_NULL;
        (*data_point)->unit = MCL_NULL;
    }
    else
    {
        MCL_ERROR_STRING("Not enough memory to allocate new data point.");
        code = MCL_OUT_OF_MEMORY;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_data_point_set_parameter(mcl_data_point_t *data_point, E_MCL_DATA_POINT_PARAMETER parameter, const void *value)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_data_point_t *data_point = <%p>, E_MCL_DATA_POINT_PARAMETER parameter = <%d>, const void *value = <%p>", data_point, parameter, value);

    // Null check.
    MCL_ASSERT_NOT_NULL(data_point, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_DATA_POINT_PARAMETER_ID:
            code = mcl_string_util_reset(value, &data_point->id);
            break;

        case MCL_DATA_POINT_PARAMETER_NAME:
            code = mcl_string_util_reset(value, &data_point->name);
            break;

        case MCL_DATA_POINT_PARAMETER_DESCRIPTION:
            code = mcl_string_util_reset(value, &data_point->description);
            break;

        case MCL_DATA_POINT_PARAMETER_TYPE:
            code = mcl_string_util_reset(value, &data_point->type);
            break;

        case MCL_DATA_POINT_PARAMETER_UNIT:
            code = mcl_string_util_reset(value, &data_point->unit);
            break;

        case MCL_DATA_POINT_PARAMETER_CUSTOM_DATA:
            mcl_json_util_destroy(&data_point->custom_data);
            code = mcl_json_util_duplicate((const mcl_json_t *) value, &data_point->custom_data);
            break;

        default:
            code = MCL_INVALID_PARAMETER;
            break;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t data_point_validate(data_point_t *data_point)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("data_point_t *data_point = <%p>", data_point);

    // Checks whether all mandatory parameters of a data point are set or not.
    if ((MCL_NULL == data_point->id) || (MCL_NULL == data_point->name) || (MCL_NULL == data_point->type) || (MCL_NULL == data_point->unit))
    {
        code = MCL_INVALID_PARAMETER;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_data_point_destroy(mcl_data_point_t **data_point)
{
    MCL_DEBUG_ENTRY("mcl_data_point_t **data_point = <%p>", data_point);

    // Destroys the data point data structure.
    if ((MCL_NULL != data_point) && (MCL_NULL != *data_point))
    {
        MCL_FREE((*data_point)->id);
        MCL_FREE((*data_point)->name);
        MCL_FREE((*data_point)->description);
        MCL_FREE((*data_point)->type);
        MCL_FREE((*data_point)->unit);
        mcl_json_util_destroy(&(*data_point)->custom_data);
        MCL_FREE(*data_point);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
