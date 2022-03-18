/*!
 * @file     json_util.c
 * @brief    Json utility module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "cJSON/cJSON.h"
#include "mcl_core/mcl_json_util.h"
#include "json_util.h"
#include "string_util.h"
#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_memory.h"

static cJSON_Hooks cjson_hooks;

// Integers and doubles are both treated as numbers in cJSON.
static mcl_error_t _add_number(mcl_json_t *root, const char *object_name, const double number);

void mcl_json_util_library_initialize(void)
{
    MCL_DEBUG_ENTRY("void");

    json_util_initialize_json_library();

    MCL_DEBUG_LEAVE("retVal = void");
}

void json_util_initialize_json_library(void)
{
    MCL_DEBUG_ENTRY("void");

    cjson_hooks.malloc_fn = mcl_memory_malloc;
    cjson_hooks.free_fn = mcl_memory_free;
    cJSON_InitHooks(&cjson_hooks);

    MCL_DEBUG_LEAVE("retVal = void");
}

mcl_error_t mcl_json_util_initialize(E_MCL_JSON_TYPE mcl_json_type, mcl_json_t **root)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("E_MCL_JSON_TYPE mcl_json_type = <%d>, mcl_json_t **root = <%p>", mcl_json_type, root);

    // Null check.
    MCL_ASSERT_NOT_NULL(root, code);

    code = json_util_initialize(mcl_json_type, root);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_initialize(E_MCL_JSON_TYPE mcl_json_type, mcl_json_t **root)
{
    mcl_error_t code = MCL_OK;

    MCL_VERBOSE_ENTRY("E_MCL_JSON_TYPE mcl_json_type = <%p>, mcl_json_t **root = <%p>", mcl_json_type, root);

    (*root) = MCL_NULL;

    switch (mcl_json_type)
    {
        case MCL_JSON_ARRAY:
            *root = cJSON_CreateArray();
            break;

        case MCL_JSON_OBJECT :
            *root = cJSON_CreateObject();
            break;

        default :
            MCL_VERBOSE("mcl_json_type = <%d> is out of valid types.", mcl_json_type);
            break;
    }

    if (MCL_NULL == *root)
    {
        code = MCL_OUT_OF_MEMORY;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_start_array(mcl_json_t *root, const char *array_name, mcl_json_t **json_array)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *array_name = <%p>, mcl_json_t **json_array = <%p>", root, array_name, json_array);

    // Null check.
    MCL_ASSERT_NOT_NULL(root, code);
    MCL_ASSERT_NOT_NULL(array_name, code);
    MCL_ASSERT_NOT_NULL(json_array, code);

    code = json_util_start_array(root, array_name, json_array);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_start_array(mcl_json_t *root, const char *array_name, mcl_json_t **json_array)
{
    mcl_json_t *json_child = MCL_NULL;
    mcl_error_t code;

    MCL_VERBOSE_ENTRY("mcl_json_t *root = <%p>, const char *array_name = <%p>, mcl_json_t **json_array = <%p>", root, array_name, json_array);

    // Check whether object already exists or not.
    code = json_util_get_object_item(root, array_name, &json_child);

    if (MCL_JSON_NON_EXISTING_CHILD == code)
    {
        // Initialize array.
        code = json_util_initialize(MCL_JSON_ARRAY, json_array);

        if (MCL_OK == code)
        {
            cJSON_AddItemToObject((cJSON *) root, array_name, *json_array);
        }
    }
    else
    {
        code = MCL_JSON_NAME_DUPLICATION;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_get_array_item(mcl_json_t *array, int index, mcl_json_t **item)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *array = <%p>, int index = <%d>, mcl_json_t **item = <%p>", array, index, item);

    // Null check.
    MCL_ASSERT_NOT_NULL(array, code);
    MCL_ASSERT_NOT_NULL(item, code);

    code = json_util_get_array_item(array, index, item);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_get_array_item(mcl_json_t *array, int index, mcl_json_t **item)
{
    mcl_error_t code = MCL_JSON_TYPE_MISMATCH;

    MCL_VERBOSE_ENTRY("mcl_json_t *array = <%p>, int index = <%d>, mcl_json_t **item = <%p>", array, index, item);

    if (cJSON_Array == ((cJSON *) array)->type)
    {
        (*item) = cJSON_GetArrayItem((cJSON *) array, index);
        if (MCL_NULL != *item)
        {
            code = MCL_OK;
        }
        else
        {
            code = MCL_INVALID_PARAMETER;
        }
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_get_array_size(mcl_json_t *array, mcl_size_t *size)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_json_t *array = <%p>, mcl_size_t *size = <%p>", array, size);

    MCL_ASSERT_NOT_NULL(array, code);
    MCL_ASSERT_NOT_NULL(size, code);

    code = json_util_get_array_size(array, size);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_get_array_size(mcl_json_t *array, mcl_size_t *size)
{
    mcl_error_t code = MCL_JSON_TYPE_MISMATCH;

    MCL_VERBOSE_ENTRY("mcl_json_t *array = <%p>, mcl_size_t *size = <%p>", array, size);

    if (cJSON_Array == ((cJSON *) array)->type)
    {
        *size = (mcl_size_t) cJSON_GetArraySize((cJSON *) array);
        code = MCL_OK;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_start_object(mcl_json_t *root, const char *object_name, mcl_json_t **json_object)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, mcl_json_t **json_object = <%p>", root, object_name, json_object);

    // Null check.
    MCL_ASSERT_NOT_NULL(root, code);
    MCL_ASSERT_NOT_NULL(object_name, code);
    MCL_ASSERT_NOT_NULL(json_object, code);

    code = json_util_start_object(root, object_name, json_object);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_start_object(mcl_json_t *root, const char *object_name, mcl_json_t **json_object)
{
    // Check whether object already exists or not.
    mcl_json_t *json_child = MCL_NULL;
    mcl_error_t code;

    MCL_VERBOSE_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, mcl_json_t **json_object = <%p>", root, object_name, json_object);

    code = json_util_get_object_item(root, object_name, &json_child);

    if (MCL_JSON_NON_EXISTING_CHILD != code)
    {
        code = MCL_JSON_NAME_DUPLICATION;
    }
    else
    {
        // Initialize object.
        code = json_util_initialize(MCL_JSON_OBJECT, json_object);

        if (MCL_OK == code)
        {
            // Add object.
            cJSON_AddItemToObject((cJSON *) root, object_name, *json_object);
        }
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_add_string(mcl_json_t *root, const char *object_name, const char *object_value)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, const char *object_value = <%p>", root, object_name, object_value);

    // Null check.
    MCL_ASSERT_NOT_NULL(root, code);
    MCL_ASSERT_NOT_NULL(object_value, code);

    code = json_util_add_string(root, object_name, object_value);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_add_string(mcl_json_t *root, const char *object_name, const char *object_value)
{
    mcl_error_t code = MCL_OK;

    MCL_VERBOSE_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, const char *object_value = <%p>", root, object_name, object_value);

    // Check type of root.
    if ((cJSON_Array == ((cJSON *) root)->type) && (MCL_NULL == object_name))
    {
        // Add string to array.
        cJSON *string = cJSON_CreateString(object_value);

        if (MCL_NULL == string)
        {
            code = MCL_OUT_OF_MEMORY;
        }
        else
        {
            cJSON_AddItemToArray((cJSON *) root, string);
        }
    }
    else if ((cJSON_Object == ((cJSON *) root)->type) && (MCL_NULL != object_name))
    {
        // Get object to add string.
        mcl_json_t *json_child = MCL_NULL;
        code = json_util_get_object_item(root, object_name, &json_child);

        if (MCL_JSON_NON_EXISTING_CHILD == code)
        {
            // Add string to object.
            cJSON_AddStringToObject((cJSON *) root, object_name, object_value);
            code = MCL_OK;
        }
        else
        {
            code = MCL_JSON_NAME_DUPLICATION;
        }
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_add_uint(mcl_json_t *root, const char *object_name, const mcl_size_t number)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, const mcl_size_t number = <%u>", root, object_name, &number);

    // Null check.
    MCL_ASSERT_NOT_NULL(root, code);

    code = json_util_add_uint(root, object_name, number);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_add_uint(mcl_json_t *root, const char *object_name, const mcl_size_t number)
{
    mcl_error_t code;

    MCL_VERBOSE_ENTRY("mcl_json_t *root = <%p>, char *object_name = <%p>, mcl_size_t number = <%u>", root, object_name, number);

    code = _add_number(root, object_name, (double) number);

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_add_double(mcl_json_t *root, const char *object_name, const double number)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, const double number = <%f>", root, object_name, &number);

    // Null check.
    MCL_ASSERT_NOT_NULL(root, code);

    code = json_util_add_double(root, object_name, number);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_add_double(mcl_json_t *root, const char *object_name, const double number)
{
    mcl_error_t code;

    MCL_VERBOSE_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, const double number = <%f>", root, object_name, number);

    code = _add_number(root, object_name, number);

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_add_bool(mcl_json_t *root, const char *object_name, const mcl_bool_t bool_value)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, const mcl_bool_t bool_value = <%d>", root, object_name, &bool_value);

    // Null check.
    MCL_ASSERT_NOT_NULL(root, code);

    code = json_util_add_bool(root, object_name, bool_value);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_add_bool(mcl_json_t *root, const char *object_name, const mcl_bool_t bool_value)
{
    mcl_error_t code = MCL_OK;

    MCL_VERBOSE_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, const mcl_bool_t bool_value = <%d>", root, object_name, bool_value);

    // Check type of root.
    if ((cJSON_Array == ((cJSON *) root)->type) && (MCL_NULL == object_name))
    {
        // Add bool to array.
        cJSON *json_bool = cJSON_CreateBool(bool_value);

        if (MCL_NULL == json_bool)
        {
            code = MCL_OUT_OF_MEMORY;
        }
        else
        {
            cJSON_AddItemToArray((cJSON *) root, json_bool);
        }
    }
    else if ((cJSON_Object == ((cJSON *) root)->type) && (MCL_NULL != object_name))
    {
        // Get object to add bool.
        mcl_json_t *json_child = MCL_NULL;
        code = json_util_get_object_item(root, object_name, &json_child);

        if (MCL_JSON_NON_EXISTING_CHILD == code)
        {
            // Add uint to object. This function expects a double and type cast to integer inside the function.
            cJSON_AddBoolToObject((cJSON *) root, object_name, bool_value);
            code = MCL_OK;
        }
        else
        {
            code = MCL_JSON_NAME_DUPLICATION;
        }
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_add_null(mcl_json_t *root, const char *object_name)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>", root, object_name);

    // Null check.
    MCL_ASSERT_NOT_NULL(root, code);

    code = json_util_add_null(root, object_name);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_add_null(mcl_json_t *root, const char *object_name)
{
    mcl_error_t code = MCL_OK;

    MCL_VERBOSE_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>", root, object_name);

    // Check type of root.
    if ((cJSON_Array == ((cJSON *) root)->type) && (MCL_NULL == object_name))
    {
        // Add null to array.
        cJSON *json_null = cJSON_CreateNull();
        cJSON_AddItemToArray(root, json_null);
    }
    else if ((cJSON_Object == ((cJSON *) root)->type) && (MCL_NULL != object_name))
    {
        // Get object to add null.
        mcl_json_t *json_child = MCL_NULL;
        code = json_util_get_object_item(root, object_name, &json_child);

        if (MCL_JSON_NON_EXISTING_CHILD == code)
        {
            // Add null to object.
            cJSON_AddNullToObject((cJSON *) root, object_name);
            code = MCL_OK;
        }
        else
        {
            code = MCL_JSON_NAME_DUPLICATION;
        }
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_add_object(mcl_json_t *root, const char *object_name, mcl_json_t *object)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, mcl_json_t *object = <%p>", root, object_name, object);

    // Null check.
    MCL_ASSERT_NOT_NULL(root, code);
    MCL_ASSERT_NOT_NULL(object_name, code);
    MCL_ASSERT_NOT_NULL(object, code);

    code = json_util_add_object(root, object_name, object);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_add_object(mcl_json_t *root, const char *object_name, mcl_json_t *object)
{
    mcl_json_t *json_child = MCL_NULL;
    mcl_error_t code;

    MCL_VERBOSE_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, mcl_json_t *object = <%p>", root, object_name, object);

    // Check whether object already exists or not.
    code = json_util_get_object_item(root, object_name, &json_child);

    if (MCL_JSON_NON_EXISTING_CHILD == code)
    {
        // Add object.
        cJSON_AddItemToObject((cJSON *) root, object_name, object);
        code = MCL_OK;
    }
    else
    {
        code = MCL_JSON_NAME_DUPLICATION;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_add_item_to_array(mcl_json_t *root, mcl_json_t *object)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_json_t *root = <%p>, mcl_json_t *object = <%p>", root, object);

    // Null check.
    MCL_ASSERT_NOT_NULL(root, code);
    MCL_ASSERT_NOT_NULL(object, code);

    json_util_add_item_to_array(root, object);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void json_util_add_item_to_array(mcl_json_t *root, mcl_json_t *object)
{
    MCL_VERBOSE_ENTRY("mcl_json_t *root = <%p>, mcl_json_t *object = <%p>", root, object);

    cJSON_AddItemToArray((cJSON *) root, object);

    MCL_VERBOSE_LEAVE("retVal = void");
}

mcl_error_t mcl_json_util_get_object_item(mcl_json_t *json_parent, const char *child_name, mcl_json_t **json_child)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *json_parent = <%p>, const char *child_name = <%p>, mcl_json_t **json_child = <%p>", json_parent, child_name, json_child);

    // Null check.
    MCL_ASSERT_NOT_NULL(json_parent, code);
    MCL_ASSERT_NOT_NULL(child_name, code);
    MCL_ASSERT_NOT_NULL(json_child, code);

    code = json_util_get_object_item(json_parent, child_name, json_child);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_get_object_item(mcl_json_t *json_parent, const char *child_name, mcl_json_t **json_child)
{
    mcl_error_t return_code = MCL_OK;

    MCL_VERBOSE_ENTRY("mcl_json_t *json_parent = <%p>, const char *child_name = <%p>, mcl_json_t **json_child = <%p>", json_parent, child_name, json_child);

    // Returns the address of object item to root handle w/o allocation.
    (*json_child) = cJSON_GetObjectItem((cJSON *) json_parent, child_name);

    // Check if child object doesn't exist.
    if (MCL_NULL == (*json_child))
    {
        return_code = MCL_JSON_NON_EXISTING_CHILD;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", return_code);
    return return_code;
}

mcl_error_t mcl_json_util_has_child(mcl_json_t *root, mcl_bool_t *result)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_json_t *root = <%p>, mcl_bool_t *result = <%p>", root, result);

    // Null check.
    MCL_ASSERT_NOT_NULL(root, code);
    MCL_ASSERT_NOT_NULL(result, code);

    *result = json_util_has_child((cJSON *) root);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_bool_t json_util_has_child(mcl_json_t *root)
{
    mcl_bool_t return_value = MCL_FALSE;

    MCL_VERBOSE_ENTRY("mcl_json_t *root = <%p>", root);

    // If root has child make result true.
    if (MCL_NULL != ((cJSON *) root)->child)
    {
        return_value = MCL_TRUE;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", return_value);
    return return_value;
}

mcl_error_t mcl_json_util_get_number_value(mcl_json_t *json, mcl_int32_t *number_value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, mcl_int32_t *number_value = <%p>", json, number_value);

    // Null check.
    MCL_ASSERT_NOT_NULL(json, code);
    MCL_ASSERT_NOT_NULL(number_value, code);

    code = json_util_get_number_value(json, number_value);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_get_number_value(mcl_json_t *json, mcl_int32_t *number_value)
{
    mcl_error_t code = MCL_JSON_TYPE_MISMATCH;

    MCL_VERBOSE_ENTRY("mcl_json_t *json = <%p>, mcl_int32_t *number_value = <%p>", json, number_value);

    if (cJSON_Number == ((cJSON *) json)->type)
    {
        *number_value = ((cJSON *) json)->valueint;
        code = MCL_OK;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_get_double_value(mcl_json_t *json, double *double_value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, double *double_value = <%p>", json, double_value);

    // Null check.
    MCL_ASSERT_NOT_NULL(json, code);
    MCL_ASSERT_NOT_NULL(double_value, code);

    code = json_util_get_double_value(json, double_value);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_get_double_value(mcl_json_t *json, double *double_value)
{
    mcl_error_t code = MCL_JSON_TYPE_MISMATCH;

    MCL_VERBOSE_ENTRY("mcl_json_t *json = <%p>, double *double_value = <%p>", json, double_value);

    if (cJSON_Number == ((cJSON *) json)->type)
    {
        *double_value = ((cJSON *) json)->valuedouble;
        code = MCL_OK;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_get_bool_value(mcl_json_t *json, mcl_bool_t *bool_value)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, mcl_bool_t *bool_value = <%p>", json, bool_value);

    // Null check.
    MCL_ASSERT_NOT_NULL(json, code);
    MCL_ASSERT_NOT_NULL(bool_value, code);

    code = json_util_get_bool_value(json, bool_value);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_get_bool_value(mcl_json_t *json, mcl_bool_t *bool_value)
{
    mcl_error_t code = MCL_JSON_TYPE_MISMATCH;

    MCL_VERBOSE_ENTRY("mcl_json_t *json = <%p>, mcl_bool_t *bool_value = <%p>", json, bool_value);

    if (cJSON_True == ((cJSON *) json)->type)
    {
        *bool_value = MCL_TRUE;
        code = MCL_OK;
    }
    else if (cJSON_False == ((cJSON *) json)->type)
    {
        *bool_value = MCL_FALSE;
        code = MCL_OK;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_get_string(mcl_json_t *json, char **string_value)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, char **string_value = <%p>", json, string_value);

    // Null check.
    MCL_ASSERT_NOT_NULL(json, code);
    MCL_ASSERT_NOT_NULL(string_value, code);

    code = json_util_get_string(json, string_value);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_get_string(mcl_json_t *json, char **string_value)
{
    mcl_size_t string_length;
    mcl_error_t code;

    MCL_VERBOSE_ENTRY("mcl_json_t *json = <%p>, char **string_value = <%p>", json, string_value);

    if (cJSON_String == ((cJSON *) json)->type)
    {
        // Calculate string length.
        string_length = string_util_strlen(((cJSON *) json)->valuestring);

        *string_value = MCL_MALLOC(string_length + MCL_NULL_CHAR_SIZE);

        if(MCL_NULL == *string_value)
        {
            code = MCL_OUT_OF_MEMORY;
        }
        else
        {
            string_util_memcpy(*string_value, ((cJSON *) json)->valuestring, string_length + MCL_NULL_CHAR_SIZE);
            code = MCL_OK;
        }
    }
    else if (cJSON_NULL == ((cJSON *) json)->type)
    {
        *string_value = MCL_NULL;
        code = MCL_OK;
    }
    else
    {
        code = MCL_JSON_TYPE_MISMATCH;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_to_string(mcl_json_t *root, char **json_string)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *root = <%p>, char **json_string = <%p>", root, json_string);

    // Null check.
    MCL_ASSERT_NOT_NULL(root, code);
    MCL_ASSERT_NOT_NULL(json_string, code);

    code = json_util_to_string(root, json_string);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_to_string(mcl_json_t *root, char **json_string)
{
    mcl_error_t code = MCL_OK;

    MCL_VERBOSE_ENTRY("mcl_json_t *root = <%p>, char **json_string = <%p>", root, json_string);

    *json_string = cJSON_PrintUnformatted((cJSON *) root);

    if (MCL_NULL == *json_string)
    {
        code = MCL_FAIL;
        MCL_ERROR("Either the given json object is invalid or memory can not be allocated for root.");
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_parse(const char *json_string, mcl_size_t buffer_size, mcl_json_t **root)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("const char *json_string = <%p>, mcl_size_t buffer_size = <%lu>, mcl_json_t **root = <%p>", json_string, buffer_size, root);

    // Null check.
    MCL_ASSERT_NOT_NULL(json_string, code);
    MCL_ASSERT_NOT_NULL(root, code);

    code = json_util_parse(json_string, buffer_size, root);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_parse(const char *json_string, mcl_size_t size, mcl_json_t **root)
{
    mcl_error_t code = MCL_OK;
    *root = MCL_NULL;

    MCL_VERBOSE_ENTRY("const char *json_string = <%p>,  mcl_size_t size = <%lu>, mcl_json_t **root = <%p>", json_string, size, root);

    if (0 == size)
    {
        (*root) = cJSON_Parse(json_string);
    }
    else
    {
        (*root) = cJSON_ParseWithLength(json_string, size);
    }

    if (MCL_NULL == (*root))
    {
        code = MCL_FAIL;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_json_util_duplicate(const mcl_json_t *source_json, mcl_json_t **duplicated_json)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("const mcl_json_t *source_json = <%p>, mcl_json_t **duplicated_json = <%p>", source_json, duplicated_json);

    MCL_ASSERT_NOT_NULL(duplicated_json, code);
    *duplicated_json = MCL_NULL;

    MCL_ASSERT_NOT_NULL(source_json, code);

    code = json_util_duplicate(source_json, duplicated_json);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_util_duplicate(const mcl_json_t *source_json, mcl_json_t **duplicated_json)
{
    mcl_error_t code = MCL_OK;

    MCL_VERBOSE_ENTRY("const mcl_json_t *source_json = <%p>, mcl_json_t **duplicated_json = <%p>", source_json, duplicated_json);

    (*duplicated_json) = cJSON_Duplicate((const cJSON *) source_json, MCL_TRUE);

    if (MCL_NULL == *duplicated_json)
    {
        code = MCL_OUT_OF_MEMORY;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_json_util_destroy(mcl_json_t **root)
{
    MCL_DEBUG_ENTRY("mcl_json_t **root = <%p>", root);

    json_util_destroy(root);

    MCL_DEBUG_LEAVE("retVal = void");
}

void json_util_destroy(mcl_json_t **root)
{
    MCL_VERBOSE_ENTRY("mcl_json_t **root = <%p>", root);

    if (MCL_NULL != *root)
    {
        cJSON_Delete((cJSON *) *root);
        *root = MCL_NULL;
    }

    MCL_VERBOSE_LEAVE("retVal = void");
}

static mcl_error_t _add_number(mcl_json_t *root, const char *object_name, const double number)
{
    mcl_error_t code = MCL_OK;

    MCL_VERBOSE_ENTRY("mcl_json_t *root = <%p>, char *object_name = <%p>, mcl_size_t number = <%u>", root, object_name, number);

    // Check type of root.
    if ((cJSON_Array == ((cJSON *) root)->type) && (MCL_NULL == object_name))
    {
        cJSON *json_number = cJSON_CreateNumber(number);

        if (MCL_NULL == json_number)
        {
            code = MCL_OUT_OF_MEMORY;
        }
        else
        {
            cJSON_AddItemToArray((cJSON *) root, json_number);
        }
    }
    else if ((cJSON_Object == ((cJSON *) root)->type) && (MCL_NULL != object_name))
    {
        mcl_json_t *json_child = MCL_NULL;
        code = json_util_get_object_item(root, object_name, &json_child);

        if (MCL_JSON_NON_EXISTING_CHILD == code)
        {
            cJSON_AddNumberToObject((cJSON *) root, object_name, number);
            code = MCL_OK;
        }
        else
        {
            code = MCL_JSON_NAME_DUPLICATION;
        }
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

    MCL_VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}
