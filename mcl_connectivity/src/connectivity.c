/*!
 * @file     connectivity.c
 * @brief    MCL Connectivity module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "connectivity.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

mcl_error_t mcl_connectivity_initialize(mcl_connectivity_configuration_t *configuration, mcl_connectivity_t **connectivity)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_connectivity_configuration_t *configuration = <%p>, mcl_connectivity_t **connectivity = <%p>", configuration, connectivity);

    // Null check of configuration.
    MCL_ASSERT_NOT_NULL(configuration, code);

    // Null check of connectivity.
    MCL_ASSERT_NOT_NULL(connectivity, code);

    // Validate connectivity configuration.
    code = connectivity_configuration_validate(configuration);

    if (MCL_OK == code)
    {
        // Allocate memory for connectivity handle.
        MCL_NEW(*connectivity);

        if (MCL_NULL != *connectivity)
        {
            // Set configuration.
            (*connectivity)->configuration = configuration;

            // Initialize connectivity processor.
            (*connectivity)->processor.agent_id = mcl_core_get_client_id(configuration->core);
            (*connectivity)->processor.hostname = mcl_core_get_host_name(configuration->core);
            (*connectivity)->processor.access_token = MCL_NULL;
            (*connectivity)->processor.http_client = mcl_core_get_http_client(configuration->core);
            (*connectivity)->processor.max_http_payload_size = configuration->max_http_payload_size;
            (*connectivity)->processor.exchange_url = configuration->exchange_url;
            (*connectivity)->processor.mapping_url = configuration->mapping_url;
        }
        else
        {
            code = MCL_OUT_OF_MEMORY;
        }
    }

    if (MCL_OK != code)
    {
        mcl_connectivity_destroy(connectivity);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_connectivity_exchange(mcl_connectivity_t *connectivity, void *item)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_connectivity_t *connectivity = <%p>, void *item = <%p>", connectivity, item);

    // Null check.
    MCL_ASSERT_NOT_NULL(connectivity, code);
    MCL_ASSERT_NOT_NULL(item, code);

    // Free previous access token.
    MCL_FREE(connectivity->processor.access_token);

    // Get access token.
    code = mcl_core_get_last_access_token(connectivity->configuration->core, &connectivity->processor.access_token);

    if (MCL_OK == code)
    {
        // Exchange item.
        code = connectivity_processor_exchange(&(connectivity->processor), item);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_connectivity_create_mapping(mcl_connectivity_t *connectivity, mcl_mapping_t *mapping)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_connectivity_t *connectivity = <%p>, mcl_mapping_t *mapping = <%p>", connectivity, mapping);

    // Null check.
    MCL_ASSERT_NOT_NULL(connectivity, code);
    MCL_ASSERT_NOT_NULL(mapping, code);

    // Free previous access token.
    MCL_FREE(connectivity->processor.access_token);

    // Get access token.
    code = mcl_core_get_last_access_token(connectivity->configuration->core, &connectivity->processor.access_token);

    if (MCL_OK == code)
    {
        // Create data point mapping.
        code = connectivity_processor_create_mapping(&(connectivity->processor), mapping);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_connectivity_get_data_source_configuration(mcl_connectivity_t *connectivity, mcl_data_source_configuration_t **configuration)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_connectivity_t *connectivity = <%p>, mcl_data_source_configuration_t **configuration = <%p>", connectivity, configuration);

    // Null check.
    MCL_ASSERT_NOT_NULL(configuration, code);
    *configuration = MCL_NULL;
    MCL_ASSERT_NOT_NULL(connectivity, code);

    // Free previous access token.
    MCL_FREE(connectivity->processor.access_token);

    // Get access token.
    code = mcl_core_get_last_access_token(connectivity->configuration->core, &connectivity->processor.access_token);

    if (MCL_OK == code)
    {
        // Get data source configuration.
        code = connectivity_processor_get_data_source_configuration(&(connectivity->processor), configuration);
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_connectivity_destroy(mcl_connectivity_t **connectivity)
{
    MCL_DEBUG_ENTRY("mcl_connectivity_t **connectivity = <%p>", connectivity);

    // Destroys the connectivity data structure.
    if ((MCL_NULL != connectivity) && (MCL_NULL != *connectivity))
    {
        MCL_FREE((*connectivity)->processor.access_token);
        MCL_FREE(*connectivity);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
