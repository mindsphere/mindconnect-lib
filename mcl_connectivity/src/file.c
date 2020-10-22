/*!
 * @file     file.c
 * @brief    File module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_connectivity/mcl_file.h"
#include "mcl_core/mcl_file_util.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"
#include "file.h"

const char *mcl_file_versions[MCL_FILE_VERSION_END] = { "1.0" };

// Function to set file local path.
static mcl_error_t _set_file_local_path(mcl_file_t *file, const char *path);

mcl_error_t mcl_file_initialize(E_MCL_FILE_VERSION version, mcl_file_t **file)
{
#if 1 == HAVE_FILE_SYSTEM_
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("E_MCL_FILE_VERSION version = <%d>, mcl_file_t **file = <%p>", version, file);

    // Null check.
    MCL_ASSERT_NOT_NULL(file, code);

    // Check meta version parameter.
    MCL_ASSERT_CODE_MESSAGE(MCL_FILE_VERSION_1_0 <= version && MCL_FILE_VERSION_END > version, MCL_INVALID_PARAMETER, "Invalid meta payload version parameter.");

    // Allocate memory for file.
    if (MCL_NULL != MCL_NEW(*file))
    {
        // Set base parameters for file.
        (*file)->item_base.preamble = MCL_ITEM_PREAMBLE;
        (*file)->item_base.type = MCL_ITEM_TYPE_FILE;
        (*file)->item_base.version = (mcl_uint32_t) version;

        // Allocate memory for file payload.
        MCL_NEW((*file)->payload);
    }

    // Allocate memory for file payload.
    if ((MCL_NULL != (*file)) && (MCL_NULL != (*file)->payload))
    {
        (*file)->payload->creation_date = MCL_NULL;
        (*file)->payload->local_path = MCL_NULL;
        (*file)->payload->remote_name = MCL_NULL;
        (*file)->payload->size = 0;
        (*file)->payload->type = MCL_NULL;
        (*file)->payload->file_descriptor = MCL_NULL;
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    if (MCL_OK != code)
    {
        mcl_file_destroy(file);
    }
#else
    mcl_error_t code = MCL_NO_FILE_SUPPORT;

    MCL_DEBUG_ENTRY("E_MCL_FILE_VERSION version = <%d>, mcl_file_t **file = <%p>", version, file);
#endif

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_file_set_parameter(mcl_file_t *file, E_MCL_FILE_PARAMETER parameter, const void *value)
{
#if 1 == HAVE_FILE_SYSTEM_
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_file_t *file = <%p>, E_MCL_FILE_PARAMETER parameter = <%d>, const void *value = <%p>", file, parameter, value);

    // Null check.
    MCL_ASSERT_NOT_NULL(file, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_FILE_PARAMETER_LOCAL_PATH:

            // Set file local path.
            code = _set_file_local_path(file, (const char *) value);
            break;

        case MCL_FILE_PARAMETER_REMOTE_NAME:

            // Set file remote name.
            code = mcl_string_util_reset(value, &file->payload->remote_name);
            break;

        case MCL_FILE_PARAMETER_TYPE:

            // Set file type.
            code = mcl_string_util_reset(value, &file->payload->type);
            break;

        default:
            code = MCL_INVALID_PARAMETER;
    }
#else
    mcl_error_t code = MCL_NO_FILE_SUPPORT;

    MCL_DEBUG_ENTRY("mcl_file_t *file = <%p>, E_MCL_FILE_PARAMETER parameter = <%d>, const void *value = <%p>", file, parameter, value);
#endif

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t file_validate(file_t *file)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("file_t *file = <%p>", file);

#if 1 == HAVE_FILE_SYSTEM_
    if (MCL_NULL == file->payload->local_path)
    {
        code = MCL_INVALID_PARAMETER;
    }
    else if (MCL_NULL == file->payload->remote_name)
    {
        code = MCL_INVALID_PARAMETER;
    }
    else
    {
        code = MCL_OK;
    }
#else
    code = MCL_NO_FILE_SUPPORT;
#endif

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_file_destroy(mcl_file_t **file)
{
    MCL_DEBUG_ENTRY("mcl_file_t **file = <%p>", file);

#if 1 == HAVE_FILE_SYSTEM_
    if ((MCL_NULL != file) && (MCL_NULL != *file))
    {
        if (MCL_NULL != (*file)->payload)
        {
            MCL_FREE((*file)->payload->creation_date);
            MCL_FREE((*file)->payload->local_path);
            MCL_FREE((*file)->payload->remote_name);
            MCL_FREE((*file)->payload->type);
            (void)mcl_file_util_fclose((*file)->payload->file_descriptor);
            MCL_FREE((*file)->payload);
        }

        MCL_FREE(*file);
    }
#endif

    MCL_DEBUG_LEAVE("retVal = void");
}

static mcl_error_t _set_file_local_path(mcl_file_t *file, const char *path)
{
    mcl_error_t code;
    time_t creation_date;

    MCL_DEBUG_ENTRY("mcl_file_t *file = <%p>, const void *path = <%p>", file, path);

    // Close previous file if exists.
    code = mcl_file_util_fclose(file->payload->file_descriptor);
    MCL_ASSERT_OK(code);

    // Set file descriptor of previous file to null.
    file->payload->file_descriptor = MCL_NULL;

    // Set file local path.
    code = mcl_string_util_reset(path, &file->payload->local_path);

    // Open file.
    if ((MCL_OK == code) && (MCL_OK != mcl_file_util_fopen((const char *) path, "rb", &file->payload->file_descriptor)))
    {
        code = MCL_FILE_CANNOT_BE_OPENED;
    }

    // Check if the file is regular.
    if ((MCL_OK == code) && (MCL_TRUE != mcl_file_util_check_if_regular_file(file->payload->file_descriptor)))
    {
        code = MCL_FILE_CANNOT_BE_OPENED;
    }

    if (MCL_OK == code)
    {
        // Set file size.
        file->payload->size = mcl_file_util_get_file_size(file->payload->file_descriptor);

        // Get file creation date.
        creation_date = mcl_file_util_get_time_of_last_status_change(file->payload->file_descriptor);

        // If creation_date buffer is NULL, then allocate it. If it is not, continue to use it.
        if (MCL_NULL == file->payload->creation_date)
        {
            file->payload->creation_date = MCL_MALLOC(MCL_TIMESTAMP_LENGTH);

            if (MCL_NULL == file->payload->creation_date)
            {
                code = MCL_OUT_OF_MEMORY;
            }
        }

        // Convert file creation date to ISO 8601 format.
        if (MCL_OK == code)
        {
            code = mcl_time_util_convert_to_iso_8601_format((const time_t *)&creation_date, file->payload->creation_date);
        }
    }

    if (MCL_OK != code)
    {
        // Clean up.
        MCL_FREE(file->payload->local_path);
        MCL_FREE(file->payload->creation_date);
        (void)mcl_file_util_fclose(file->payload->file_descriptor);
        file->payload->file_descriptor = MCL_NULL;
        file->payload->size = 0;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
