/*!
 * @file     data_lake.c
 * @brief    Data lake module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_data_lake/mcl_data_lake.h"
#include "data_lake.h"
#include "data_lake_configuration.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_core.h"

mcl_error_t mcl_data_lake_initialize(mcl_data_lake_configuration_t *configuration, mcl_data_lake_t **data_lake)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_data_lake_configuration_t *configuration = <%p>, mcl_data_lake_t **data_lake = <%p>", configuration, data_lake);

    // Null check.
    MCL_ASSERT_NOT_NULL(data_lake, code);

    // Data lake must be NULL if anything goes wrong.
    *data_lake = MCL_NULL;

    // Null check.
    MCL_ASSERT_NOT_NULL(configuration, code);

    // Validate data lake configuration.
    code = data_lake_configuration_validate(configuration);

    if (MCL_OK == code)
    {
        // Allocate memory for data_lake handle.
        MCL_NEW(*data_lake);

        if (MCL_NULL == *data_lake)
        {
            code = MCL_OUT_OF_MEMORY;
        }
        else
        {
            // Set data lake configuration.
            (*data_lake)->configuration = configuration;

            // Initialize data lake processor.
            (*data_lake)->processor.access_token = MCL_NULL;
            (*data_lake)->processor.client_id = mcl_core_get_client_id(configuration->core);
            (*data_lake)->processor.http_client = mcl_core_get_http_client(configuration->core);
            (*data_lake)->processor.upload_url_generation_url = configuration->upload_url_generation_url;

            if (MCL_NULL != configuration->certificate)
            {
                code = mcl_http_client_add_certificate((*data_lake)->processor.http_client, configuration->certificate, configuration->certificate_is_file);
            } 
        }
    }

MCL_FUNCTION_LEAVE_LABEL:

    if (MCL_OK != code)
    {
        mcl_data_lake_destroy(data_lake);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_data_lake_generate_upload_url(mcl_data_lake_t *data_lake, mcl_data_lake_object_t *object)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_data_lake_t *data_lake = <%p>, mcl_data_lake_object_t *object = <%p>", data_lake, object);

    if (MCL_NULL == data_lake)
    {
        MCL_ERROR("data_lake parameter is NULL.");
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else if (MCL_NULL == object)
    {
        MCL_ERROR("object parameter is NULL.");
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else
    {
        // Free previous access token.
        MCL_FREE(data_lake->processor.access_token);

        // Get access token.
        code = mcl_core_get_last_access_token(data_lake->configuration->core, &data_lake->processor.access_token);

        if (MCL_OK == code)
        {
            // Get URL.
            code = data_lake_processor_generate_upload_url(&data_lake->processor, object, MCL_NULL);
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_data_lake_generate_upload_urls(mcl_data_lake_t *data_lake, mcl_list_t *object_list)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_data_lake_t *data_lake = <%p>, mcl_list_t *object_list = <%p>", data_lake, object_list);

    if (MCL_NULL == data_lake)
    {
        MCL_ERROR("data_lake parameter is NULL.");
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else if (MCL_NULL == object_list)
    {
        MCL_ERROR("object_list parameter is NULL.");
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else
    {
        // Free previous access token.
        MCL_FREE(data_lake->processor.access_token);

        // Get access token.
        code = mcl_core_get_last_access_token(data_lake->configuration->core, &data_lake->processor.access_token);

        if (MCL_OK == code)
        {
            // Get URLs.
            code = data_lake_processor_generate_upload_urls(&data_lake->processor, object_list, MCL_NULL);
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_data_lake_generate_upload_url_for_subtenant(mcl_data_lake_t *data_lake, mcl_data_lake_object_t *object, const char *subtenant_id)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_data_lake_t *data_lake = <%p>, mcl_data_lake_object_t *object = <%p>", data_lake, object);

    if (MCL_NULL == data_lake)
    {
        MCL_ERROR("data_lake parameter is NULL.");
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else if (MCL_NULL == object)
    {
        MCL_ERROR("object parameter is NULL.");
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else
    {
        // Free previous access token.
        MCL_FREE(data_lake->processor.access_token);

        // Get access token.
        code = mcl_core_get_last_access_token(data_lake->configuration->core, &data_lake->processor.access_token);

        if (MCL_OK == code)
        {
            // Get URL.
            code = data_lake_processor_generate_upload_url(&data_lake->processor, object, subtenant_id);
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_data_lake_generate_upload_urls_for_subtenant(mcl_data_lake_t *data_lake, mcl_list_t *object_list, const char *subtenant_id)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_data_lake_t *data_lake = <%p>, mcl_list_t *object_list = <%p>", data_lake, object_list);

    if (MCL_NULL == data_lake)
    {
        MCL_ERROR("data_lake parameter is NULL.");
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else if (MCL_NULL == object_list)
    {
        MCL_ERROR("object_list parameter is NULL.");
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else
    {
        // Free previous access token.
        MCL_FREE(data_lake->processor.access_token);

        // Get access token.
        code = mcl_core_get_last_access_token(data_lake->configuration->core, &data_lake->processor.access_token);

        if (MCL_OK == code)
        {
            // Get URLs.
            code = data_lake_processor_generate_upload_urls(&data_lake->processor, object_list, subtenant_id);
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_data_lake_upload(mcl_data_lake_t *data_lake, mcl_data_lake_object_t *object)
{
    // Variable declaration.
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_data_lake_t *data_lake = <%p>, mcl_data_lake_object_t *object = <%p>", data_lake, object);

    // Null check.
    if (MCL_NULL == data_lake)
    {
        MCL_ERROR("data_lake parameter is NULL.");
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else if (MCL_NULL == object)
    {
        MCL_ERROR("object parameter is NULL.");
        code = MCL_TRIGGERED_WITH_NULL;
    }
    else
    {
        // Upload object.
        code = data_lake_processor_upload(&(data_lake->processor), object);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_data_lake_destroy(mcl_data_lake_t **data_lake)
{
    MCL_DEBUG_ENTRY("mcl_data_lake_t **data_lake = <%p>", data_lake);

    if ((MCL_NULL != data_lake) && (MCL_NULL != *data_lake))
    {
        MCL_FREE((*data_lake)->processor.access_token);
        MCL_FREE(*data_lake);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
