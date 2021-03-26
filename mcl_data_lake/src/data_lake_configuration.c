/*!
 * @file     data_lake_configuration.c
 * @brief    Data lake configuration module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_data_lake/mcl_data_lake_configuration.h"
#include "data_lake_configuration.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

// Endpoint for generating a signed url to upload data to.
static const char _upload_url_generation_endpoint[] = "/api/datalake/v3/generateUploadObjectUrls";

// This function sets core parameter of data lake configuration.
static mcl_error_t _set_data_lake_configuration_core_parameter(mcl_data_lake_configuration_t *configuration, mcl_core_t *core);

mcl_error_t mcl_data_lake_configuration_initialize(mcl_data_lake_configuration_t **configuration)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_data_lake_configuration_t **configuration = <%p>", configuration);

    // Null check.
    MCL_ASSERT_NOT_NULL(configuration, code);

    // Allocate memory for configuration data structure.
    MCL_NEW(*configuration);

    if (MCL_NULL == *configuration)
    {
        code = MCL_OUT_OF_MEMORY;
    }
    else
    {
        // Set default values of configuration parameters.
        (*configuration)->core = MCL_NULL;
        (*configuration)->upload_url_generation_url = MCL_NULL;
        (*configuration)->certificate = MCL_NULL;
        (*configuration)->certificate_is_file = MCL_FALSE;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_data_lake_configuration_set_parameter(mcl_data_lake_configuration_t *configuration,
    E_MCL_DATA_LAKE_CONFIGURATION_PARAMETER parameter, const void *value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_data_lake_configuration_t *configuration = <%p>, E_MCL_DATA_LAKE_CONFIGURATION_PARAMETER parameter = <%d>, const void *value = <%p>",
        configuration, parameter, value);

    MCL_ASSERT_NOT_NULL(configuration, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CORE:
            code = _set_data_lake_configuration_core_parameter(configuration, (mcl_core_t *) value);
            break;

        case MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CERTIFICATE:
            code = mcl_string_util_reset(value, &configuration->certificate);
            configuration->certificate_is_file = MCL_FALSE;
            break;

        case MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CERTIFICATE_FILE:
#if HAVE_FILE_SYSTEM_
            code = mcl_string_util_reset(value, &configuration->certificate);
            configuration->certificate_is_file = MCL_TRUE;
#else
            code = MCL_NO_FILE_SUPPORT;
#endif
            break;

        default:
            code = MCL_INVALID_PARAMETER;
            break;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t data_lake_configuration_validate(data_lake_configuration_t *configuration)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("data_lake_configuration_t *configuration = <%p>", configuration);

    // Checks whether core parameter of configuration is set or not.
    if (MCL_NULL == configuration->core)
    {
        code = MCL_INVALID_PARAMETER;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_data_lake_configuration_destroy(mcl_data_lake_configuration_t **configuration)
{
    MCL_DEBUG_ENTRY("mcl_data_lake_configuration_t **configuration = <%p>", configuration);

    if ((MCL_NULL != configuration) && (MCL_NULL != *configuration))
    {
        MCL_FREE((*configuration)->upload_url_generation_url);
        MCL_FREE((*configuration)->certificate);
        MCL_FREE(*configuration);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}

static mcl_error_t _set_data_lake_configuration_core_parameter(mcl_data_lake_configuration_t *configuration, mcl_core_t *core)
{
    mcl_error_t code = MCL_OK;
    const char *hostname;

    MCL_DEBUG_ENTRY("mcl_data_lake_configuration_t *configuration = <%p>, mcl_core_t *core = <%p>", configuration, core);

    // Set core.
    configuration->core = core;

    // Get hostname.
    hostname = mcl_core_get_host_name(configuration->core);

    if (MCL_NULL != hostname)
    {
        // Calculate hostname length.
        mcl_size_t hostname_length = mcl_string_util_strlen(hostname);

        // Allocate buffer for data lake upload url.
        configuration->upload_url_generation_url = MCL_MALLOC(hostname_length + sizeof(_upload_url_generation_endpoint));

        if (MCL_NULL == configuration->upload_url_generation_url)
        {
            code = MCL_OUT_OF_MEMORY;
        }
        else
        {
            // Concatenate host name with the constant endpoint for data lake upload url.
            mcl_string_util_memcpy(configuration->upload_url_generation_url, hostname, hostname_length);
            mcl_string_util_memcpy(configuration->upload_url_generation_url + hostname_length, _upload_url_generation_endpoint,
                sizeof(_upload_url_generation_endpoint));
        }
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
