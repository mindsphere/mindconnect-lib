/*!
 * @file     timeseries.c
 * @brief    Timeseries module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_connectivity/mcl_timeseries.h"
#include "timeseries.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

const char *mcl_timeseries_versions[MCL_TIMESERIES_VERSION_END] = { "1.0" };

mcl_error_t mcl_timeseries_initialize(E_MCL_TIMESERIES_VERSION version, mcl_timeseries_t **timeseries)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("E_MCL_TIMESERIES_VERSION version = <%d>, mcl_time_series_t **timeseries = <%p>", version, timeseries);

    // Null check.
    MCL_ASSERT_NOT_NULL(timeseries, code);

    // Check meta version parameter.
    MCL_ASSERT_CODE_MESSAGE(MCL_TIMESERIES_VERSION_1_0 <= version && MCL_TIMESERIES_VERSION_END > version, MCL_INVALID_PARAMETER,
        "Invalid meta payload version parameter.");

    // Allocate memory for timeseries.
    if (MCL_NULL != MCL_NEW(*timeseries))
    {
        // Set base parameters for timeseries.
        (*timeseries)->item_base.preamble = MCL_ITEM_PREAMBLE;
        (*timeseries)->item_base.type = MCL_ITEM_TYPE_TIMESERIES;
        (*timeseries)->item_base.version = (mcl_uint32_t)version;

        // Allocate memory for timeseries payload.
        MCL_NEW((*timeseries)->payload);
    }

    if ((MCL_NULL != (*timeseries)) && (MCL_NULL != (*timeseries)->payload))
    {
        (*timeseries)->payload->configuration_id = MCL_NULL;
        (*timeseries)->payload->value_lists = MCL_NULL;

        code = mcl_list_initialize(&((*timeseries)->payload->value_lists));
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    // Error check.
    if (MCL_OK != code)
    {
        mcl_timeseries_destroy(timeseries);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_timeseries_set_parameter(mcl_timeseries_t *timeseries, E_MCL_TIMESERIES_PARAMETER parameter, const void *value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_timeseries_t *timeseries = <%p>, E_MCL_TIMESERIES_PARAMETER parameter = <%d>, const void *value = <%p>", timeseries, parameter, value);

    // Null checks.
    MCL_ASSERT_NOT_NULL(timeseries, code);
    MCL_ASSERT_NOT_NULL(value, code);

    if (MCL_TIMESERIES_PARAMETER_CONFIGURATION_ID == parameter)
    {
        code = mcl_string_util_reset(value, &timeseries->payload->configuration_id);
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_timeseries_add_value_list(mcl_timeseries_t *timeseries, mcl_timeseries_value_list_t *timeseries_value_list)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_timeseries_t *timeseries = <%p>, mcl_timeseries_value_list_t *timeseries_value_list = <%p>", timeseries, timeseries_value_list);

    // Null check.
    if ((MCL_NULL == timeseries) || (MCL_NULL == timeseries_value_list))
    {
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else
    {
        // Add timeseries value list to timeseries.
        code = mcl_list_add(timeseries->payload->value_lists, timeseries_value_list);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t timeseries_validate(timeseries_t *timeseries)
{
    mcl_list_t *timeseries_value_list = timeseries->payload->value_lists;
    mcl_size_t index;
    mcl_list_node_t *node = MCL_NULL;

    MCL_DEBUG_ENTRY("timeseries_t *timeseries = <%p>", timeseries);

    // Check configuration id.
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != timeseries->payload->configuration_id, MCL_INVALID_PARAMETER, "Configuration id is not set.");

    // Check timeseries value list existence.
    mcl_list_reset(timeseries_value_list);
    MCL_ASSERT_CODE_MESSAGE(0 < timeseries_value_list->count, MCL_INVALID_PARAMETER, "Timeseries has no timeseries value list.");

    // Validate each timeseries value list.
    for (index = 0; index < timeseries_value_list->count; ++index)
    {
        mcl_error_t code = mcl_list_next(timeseries_value_list, &node);
        MCL_ASSERT_CODE_MESSAGE(MCL_OK == code, MCL_INVALID_PARAMETER, "mcl_list_next() failed");

        code = timeseries_value_list_validate((timeseries_value_list_t *) (node->data));
        MCL_ASSERT_CODE_MESSAGE(MCL_OK == code, MCL_INVALID_PARAMETER, "timeseries_value_list_validate() failed.");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void mcl_timeseries_destroy(mcl_timeseries_t **timeseries)
{
    MCL_DEBUG_ENTRY("mcl_timeseries_t **timeseries = <%p>", timeseries);

    // Destroys the timeseries.
    if ((MCL_NULL != timeseries) && (MCL_NULL != *timeseries))
    {
        if (MCL_NULL != (*timeseries)->payload)
        {
            MCL_FREE((*timeseries)->payload->configuration_id);
            mcl_list_destroy_with_content(&((*timeseries)->payload->value_lists), (mcl_list_item_destroy_callback) mcl_timeseries_value_list_destroy);
            MCL_FREE((*timeseries)->payload);
        }

        MCL_FREE(*timeseries);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
