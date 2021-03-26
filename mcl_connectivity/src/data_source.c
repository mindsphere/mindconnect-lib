/*!
 * @file     data_source.c
 * @brief    Data source module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_connectivity/mcl_data_source.h"
#include "data_source.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

mcl_error_t mcl_data_source_initialize(mcl_data_source_t **data_source)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_data_source_t **data_source = <%p>", data_source);

    // Null check.
    MCL_ASSERT_NOT_NULL(data_source, code);

    // Allocate memory for data source.
    if (MCL_NULL != MCL_NEW(*data_source))
    {
        // Set initial values.
        (*data_source)->custom_data = MCL_NULL;
        (*data_source)->data_points = MCL_NULL;
        (*data_source)->description = MCL_NULL;
        (*data_source)->name = MCL_NULL;

        // Initialize list for data points.
        code = mcl_list_initialize(&(*data_source)->data_points);
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    // Error check.
    if (MCL_OK != code)
    {
        mcl_data_source_destroy(data_source);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_data_source_set_parameter(mcl_data_source_t *data_source, E_MCL_DATA_SOURCE_PARAMETER parameter, const void *value)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_data_source_t *data_source = <%p>, E_MCL_DATA_SOURCE_PARAMETER parameter = <%d>, const void *value = <%p>",
        data_source, parameter, value);

    // Null check.
    MCL_ASSERT_NOT_NULL(data_source, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_DATA_SOURCE_PARAMETER_NAME:
            code = mcl_string_util_reset(value, &data_source->name);
            break;

        case MCL_DATA_SOURCE_PARAMETER_DESCRIPTION:
            code = mcl_string_util_reset(value, &data_source->description);
            break;

        case MCL_DATA_SOURCE_PARAMETER_CUSTOM_DATA:
            mcl_json_util_destroy(&data_source->custom_data);
            code = mcl_json_util_duplicate((const mcl_json_t *) value, &data_source->custom_data);
            break;

        default:
            code = MCL_INVALID_PARAMETER;
            break;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_data_source_add_data_point(mcl_data_source_t *data_source, mcl_data_point_t *data_point)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_data_source_t *data_source = <%p>, mcl_data_point_t *data_point = <%p>", data_source, data_point);

    // Null check.
    MCL_ASSERT_NOT_NULL(data_source, code);
    MCL_ASSERT_NOT_NULL(data_point, code);

    // Add data point to data source.
    code = mcl_list_add(data_source->data_points, data_point);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t data_source_validate(data_source_t *data_source)
{
    mcl_error_t code = MCL_OK;
    mcl_list_t *data_points = data_source->data_points;
    mcl_size_t index;

    MCL_DEBUG_ENTRY("data_source_t *data_source = <%p>", data_source);

    // Check data source name.
    if (MCL_NULL == data_source->name)
    {
        code = MCL_INVALID_PARAMETER;
        MCL_ERROR_STRING("Data source name is not set.");
    }
    else if (0 == data_points->count)
    {
        code = MCL_INVALID_PARAMETER;
        MCL_ERROR_STRING("Data source has no data point.");
    }
    else
    {
        mcl_list_reset(data_points);
    }

    // Validate each data point.
    for (index = 0; (index < data_points->count) && (MCL_OK == code); ++index)
    {
        mcl_list_node_t *node = MCL_NULL;

        (void) mcl_list_next(data_points, &node);

        if ((MCL_NULL == node) || (MCL_OK != data_point_validate((data_point_t *) (node->data))))
        {
            code = MCL_INVALID_PARAMETER;
            MCL_ERROR_STRING("Data point is not valid.");
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_data_source_destroy(mcl_data_source_t **data_source)
{
    MCL_DEBUG_ENTRY("mcl_data_source_t **data_source = <%p>", data_source);

    // Destroys the data source data structure.
    if ((MCL_NULL != data_source) && (MCL_NULL != *data_source))
    {
        MCL_FREE((*data_source)->name);
        MCL_FREE((*data_source)->description);
        mcl_list_destroy_with_content(&(*data_source)->data_points, (mcl_list_item_destroy_callback)mcl_data_point_destroy);
        mcl_json_util_destroy(&(*data_source)->custom_data);
        MCL_FREE(*data_source);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
