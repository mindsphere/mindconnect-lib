/*!
 * @file     data_lake_object.c
 * @brief    Data lake object module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "data_lake_object.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

mcl_error_t mcl_data_lake_object_initialize(mcl_data_lake_object_t **object)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_data_lake_object_t **object = <%p>", object);

    // Null check.
    MCL_ASSERT_NOT_NULL(object, code);

    // Allocate memory for object data structure.
    MCL_NEW(*object);

    if(MCL_NULL == *object)
    {
        code = MCL_OUT_OF_MEMORY;
    }
    else
    {
        // Set default values of object parameters.
        (*object)->path = MCL_NULL;
        (*object)->signed_url = MCL_NULL;
        (*object)->user_context = MCL_NULL;
        (*object)->upload_callback = MCL_NULL;
        (*object)->size = 0;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_data_lake_object_set_parameter(mcl_data_lake_object_t *object, E_MCL_DATA_LAKE_OBJECT_PARAMETER parameter, const void *value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_data_lake_object_t *object = <%p>, E_MCL_DATA_LAKE_OBJECT_PARAMETER parameter = <%d>, const void *value = <%p>", object, parameter, value);

    MCL_ASSERT_NOT_NULL(object, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_DATA_LAKE_OBJECT_PARAMETER_PATH:
            code = mcl_string_util_reset(value, &object->path);
            break;

        case MCL_DATA_LAKE_OBJECT_PARAMETER_UPLOAD_CALLBACK:
            object->upload_callback = (mcl_data_lake_upload_callback) value;
            break;

        case MCL_DATA_LAKE_OBJECT_PARAMETER_USER_CONTEXT:
            object->user_context = (void *) value;
            break;

        case MCL_DATA_LAKE_OBJECT_PARAMETER_SIZE:
            object->size = *((const mcl_size_t *) value);
            break;

        default:
            code = MCL_INVALID_PARAMETER;
            break;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t data_lake_object_set_internal_parameter(mcl_data_lake_object_t *object, E_DATA_LAKE_OBJECT_INTERNAL_PARAMETER parameter, const void *value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_data_lake_object_t *object = <%p>, E_DATA_LAKE_OBJECT_INTERNAL_PARAMETER parameter = <%p>, const void *value = <%p>", object, &parameter, value);
    
    if (DATA_LAKE_OBJECT_INTERNAL_PARAMETER_SIGNED_URL == parameter)
    {
        code = mcl_string_util_reset(value, &object->signed_url);
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_data_lake_object_destroy(mcl_data_lake_object_t **object)
{
    MCL_DEBUG_ENTRY("mcl_data_lake_object_t **object = <%p>", object);

    if ((MCL_NULL != object) && (MCL_NULL != *object))
    {
        MCL_FREE((*object)->path);
        MCL_FREE((*object)->signed_url);
        MCL_FREE(*object);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}

mcl_error_t data_lake_object_validate(data_lake_object_t *object)
{
    mcl_error_t code = MCL_INVALID_PARAMETER;

    MCL_DEBUG_ENTRY("data_lake_object_t *object = <%p>", object);

    // Check all mandatory parameters.
    if (MCL_NULL == object->path)
    {
        MCL_ERROR_STRING("Path parameter of object is not set.");
    }
    else if (MCL_NULL == object->signed_url)
    {
        MCL_ERROR_STRING("Signed url parameter of object is not set.");
    }
    else if (MCL_NULL == object->upload_callback)
    {
        MCL_ERROR_STRING("Upload callback parameter of object is not set.");
    }
    else if (0 == object->size)
    {
        MCL_ERROR_STRING("Payload size should be given.");
    }
    else
    {
        code = MCL_OK;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
