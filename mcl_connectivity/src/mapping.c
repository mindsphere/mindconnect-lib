/*!
 * @file     mapping.c
 * @brief    Mapping module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_connectivity/mcl_mapping.h"
#include "mapping.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"

mcl_error_t mcl_mapping_initialize(mcl_mapping_t **mapping)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_mapping_t **mapping = <%p>", mapping);

    // Null check.
    MCL_ASSERT_NOT_NULL(mapping, code);

    // Allocate memory for mapping.
    if (MCL_NULL != MCL_NEW(*mapping))
    {
        // Set initial values.
        (*mapping)->data_point_id = MCL_NULL;
        (*mapping)->entity_id = MCL_NULL;
        (*mapping)->property_set_name = MCL_NULL;
        (*mapping)->property_name = MCL_NULL;
        (*mapping)->keep_mapping = MCL_TRUE;
    }
    else
    {
        MCL_ERROR_STRING("Not enough memory to allocate new mapping.");
        code = MCL_OUT_OF_MEMORY;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_mapping_set_parameter(mcl_mapping_t *mapping, E_MCL_MAPPING_PARAMETER parameter, const void *value)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_mapping_t *mapping = <%p>, E_MCL_MAPPING_PARAMETER parameter = <%d>, const void *value = <%p>", mapping, parameter, value);

    // Null check.
    MCL_ASSERT_NOT_NULL(mapping, code);
    MCL_ASSERT_NOT_NULL(value, code);

    switch (parameter)
    {
        case MCL_MAPPING_PARAMETER_DATA_POINT_ID:
            code = mcl_string_util_reset(value, &(mapping->data_point_id));
            break;

        case MCL_MAPPING_PARAMETER_ENTITY_ID:
            code = mcl_string_util_reset(value, &(mapping->entity_id));
            break;

        case MCL_MAPPING_PARAMETER_PROPERTY_SET_NAME:
            code = mcl_string_util_reset(value, &(mapping->property_set_name));
            break;

        case MCL_MAPPING_PARAMETER_PROPERTY_NAME:
            code = mcl_string_util_reset(value, &(mapping->property_name));
            break;

        case MCL_MAPPING_PARAMETER_KEEP_MAPPING:
            mapping->keep_mapping = *((const mcl_bool_t *) value);
            code = MCL_OK;
            break;

        default:
            code = MCL_INVALID_PARAMETER;
            break;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mapping_validate(mapping_t *mapping)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mapping_t *mapping = <%p>", mapping);

    // Checks whether all mandatory parameters of a mapping are set or not.
    if ((MCL_NULL == mapping->data_point_id) || (MCL_NULL == mapping->property_name) || (MCL_NULL == mapping->property_set_name))
    {
        code = MCL_INVALID_PARAMETER;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_mapping_destroy(mcl_mapping_t **mapping)
{
    MCL_DEBUG_ENTRY("mcl_mapping_t **mapping = <%p>", mapping);

    // Destroys the mapping data structure.
    if ((MCL_NULL != mapping) && (MCL_NULL != *mapping))
    {
        MCL_FREE((*mapping)->data_point_id);
        MCL_FREE((*mapping)->entity_id);
        MCL_FREE((*mapping)->property_set_name);
        MCL_FREE((*mapping)->property_name);
        MCL_FREE(*mapping);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
