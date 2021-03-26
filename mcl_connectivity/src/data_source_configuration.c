/*!
 * @file     data_source_configuration.c
 * @brief    Data source configuration module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_connectivity/mcl_data_source_configuration.h"
#include "data_source_configuration.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

// This array is used to get the data source configuration version.
const char *mcl_data_source_configuration_versions[MCL_DATA_SOURCE_CONFIGURATION_END] = { "1.0" };

mcl_error_t mcl_data_source_configuration_initialize(E_MCL_DATA_SOURCE_CONFIGURATION_VERSION version,
    mcl_data_source_configuration_t **data_source_configuration)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("E_MCL_DATA_SOURCE_CONFIGURATION_VERSION version = <%d>, mcl_data_source_configuration_t **data_source_configuration = <%p>",
        version, data_source_configuration);

    // Null check.
    MCL_ASSERT_NOT_NULL(data_source_configuration, code);

    // Check data source configuration version parameter.
    MCL_ASSERT_CODE_MESSAGE(MCL_DATA_SOURCE_CONFIGURATION_1_0 <= version && MCL_DATA_SOURCE_CONFIGURATION_END > version, MCL_INVALID_PARAMETER,
        "Invalid data source configuration version parameter.");

    // Allocate memory for data source configuration.
    if (MCL_NULL != MCL_NEW(*data_source_configuration))
    {
        // Set initial values.
        (*data_source_configuration)->item_base.preamble = MCL_ITEM_PREAMBLE;
        (*data_source_configuration)->item_base.version = (mcl_uint32_t)version;
        (*data_source_configuration)->item_base.type = MCL_ITEM_TYPE_DATA_SOURCE_CONFIGURATION;

        // Allocate memory for payload of data source configuration.
        MCL_NEW((*data_source_configuration)->payload);
    }

    if ((MCL_NULL != (*data_source_configuration)) && (MCL_NULL != (*data_source_configuration)->payload))
    {
        // Set initial values.
        (*data_source_configuration)->payload->configuration_id = MCL_NULL;

        // Initialize list for data sources.
        code = mcl_list_initialize(&(*data_source_configuration)->payload->data_sources);
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    // Error check.
    if (MCL_OK != code)
    {
        mcl_data_source_configuration_destroy(data_source_configuration);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_data_source_configuration_set_parameter(mcl_data_source_configuration_t *data_source_configuration,
    E_MCL_DATA_SOURCE_CONFIGURATION_PARAMETER parameter, const void *value)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_data_source_configuration_t *data_source_configuration = <%p>, "\
        "E_MCL_DATA_SOURCE_CONFIGURATION_PARAMETER parameter = <%d>, const void *value = <%p>", data_source_configuration, parameter, value);

    // Null check.
    MCL_ASSERT_NOT_NULL(data_source_configuration, code);
    MCL_ASSERT_NOT_NULL(value, code);

    if (MCL_DATA_SOURCE_CONFIGURATION_PARAMETER_ID == parameter)
    {
        code = mcl_string_util_reset(value, &data_source_configuration->payload->configuration_id);
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_data_source_configuration_add_data_source(mcl_data_source_configuration_t *data_source_configuration, mcl_data_source_t *data_source)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_data_source_configuration_t *data_source_configuration = <%p>, mcl_data_source_t *data_source = <%p>",
        data_source_configuration, data_source);

    // Null check.
    MCL_ASSERT_NOT_NULL(data_source_configuration, code);
    MCL_ASSERT_NOT_NULL(data_source, code);

    // Add data source to data source configuration.
    code = mcl_list_add(data_source_configuration->payload->data_sources, data_source);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t data_source_configuration_validate(data_source_configuration_t *data_source_configuration)
{
    mcl_error_t code = MCL_OK;
    mcl_list_t *data_sources = data_source_configuration->payload->data_sources;
    mcl_size_t index;

    MCL_DEBUG_ENTRY("data_source_configuration_t *data_source_configuration = <%p>", data_source_configuration);

    // Check configuration id.
    if (MCL_NULL == data_source_configuration->payload->configuration_id)
    {
        code = MCL_INVALID_PARAMETER;
        MCL_ERROR_STRING("Configuration id is not set.");
    }
    else if (0 == data_sources->count)
    {
        code = MCL_INVALID_PARAMETER;
        MCL_ERROR_STRING("Data source configuration has no data source.");
    }
    else
    {
        mcl_list_reset(data_sources);
    }

    // Validate each data source.
    for (index = 0; (index < data_sources->count) && (MCL_OK == code); ++index)
    {
        mcl_list_node_t *node = MCL_NULL;

        (void) mcl_list_next(data_sources, &node);

        if ((MCL_NULL == node) || (MCL_OK != data_source_validate((data_source_t *) (node->data))))
        {
            code = MCL_INVALID_PARAMETER;
            MCL_ERROR_STRING("Data source is not valid.");
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_data_source_configuration_destroy(mcl_data_source_configuration_t **data_source_configuration)
{
    MCL_DEBUG_ENTRY("mcl_data_source_configuration_t **data_source_configuration = <%p>", data_source_configuration);

    // Destroys the data source configuration data structure.
    if ((MCL_NULL != data_source_configuration) && (MCL_NULL != *data_source_configuration))
    {
        if (MCL_NULL != (*data_source_configuration)->payload)
        {
            // Destroy payload.
            MCL_FREE((*data_source_configuration)->payload->configuration_id);
            mcl_list_destroy_with_content(&(*data_source_configuration)->payload->data_sources, (mcl_list_item_destroy_callback)mcl_data_source_destroy);
            MCL_FREE((*data_source_configuration)->payload);
        }

        MCL_FREE(*data_source_configuration);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
