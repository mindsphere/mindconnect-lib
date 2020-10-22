/*!
 * @file     string_util.c
 * @brief    String utility module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "string_util.h"
#include "mcl_core/mcl_string_util.h"
#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_memory.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define LOWERCASE(n) ((n >= 'A' && n <= 'Z') ? (n + ('a' - 'A')) : n)

// This function receives variable arguments as va_list type and print them in the desired format.
static mcl_error_t _print_formatted_string(char *string, mcl_size_t length, const char *format, va_list args);

mcl_size_t mcl_string_util_strlen(const char *buffer)
{
    mcl_size_t length;

    MCL_DEBUG_ENTRY("const char *buffer = <%p>", buffer);

    (MCL_NULL == buffer) ? (length = 0) : (length = string_util_strlen(buffer));

    MCL_DEBUG_LEAVE("retVal = <%lu>", length);
    return length;
}

mcl_size_t string_util_strlen(const char *buffer)
{
    mcl_size_t return_value;

    MCL_DEBUG_ENTRY("const char *buffer = <%p>", buffer);

    return_value = strlen(buffer);

    MCL_DEBUG_LEAVE("retVal = <%lu>", return_value);
    return return_value;
}

mcl_size_t mcl_string_util_strnlen(const char *buffer, mcl_size_t maximum_length)
{
    mcl_size_t length;

    MCL_DEBUG_ENTRY("const char *buffer = <%p>, mcl_size_t maximum_length = <%lu>", buffer, maximum_length);

    (MCL_NULL == buffer) ? (length = 0) : (length = string_util_strnlen(buffer, maximum_length));

    MCL_DEBUG_LEAVE("retVal = <%lu>", length);
    return length;
}

mcl_size_t string_util_strnlen(const char *buffer, mcl_size_t maximum_length)
{
    mcl_size_t count = 0;

    MCL_DEBUG_ENTRY("const char *buffer = <%p>, mcl_size_t maximum_length = <%lu>", buffer, maximum_length);

    while ((count < maximum_length) && (MCL_NULL_CHAR != buffer[count]))
    {
        ++count;
    }

    MCL_DEBUG_LEAVE("retVal = <%lu>", count);
    return count;
}

void mcl_string_util_strncpy(char *destination, const char *source, mcl_size_t count)
{
    MCL_DEBUG_ENTRY("char *destination = <%p>, const char *source = <%p>, mcl_size_t count = <%lu>", destination, source, count);

    string_util_strncpy(destination, source, count);

    MCL_DEBUG_LEAVE("retVal = void");
}

void string_util_strncpy(char *destination, const char *source, mcl_size_t count)
{
    MCL_DEBUG_ENTRY("char *destination = <%p>, const char *source = <%p>, mcl_size_t count = <%lu>", destination, source, count);

#if defined(WIN32) || defined(WIN64)
    strncpy_s(destination, count + 1, source, count);
#else
    strncpy(destination, source, count);
#endif

    MCL_DEBUG_LEAVE("retVal = void");
}

void mcl_string_util_strncat(char *destination, const char *source, mcl_size_t count)
{
    MCL_DEBUG_ENTRY("char *destination = <%p>, const char *source = <%p>, mcl_size_t count = <%lu>", destination, source, count);

    string_util_strncat(destination, source, count);

    MCL_DEBUG_LEAVE("retVal = void");
}

void string_util_strncat(char *destination, const char *source, mcl_size_t count)
{
    MCL_DEBUG_ENTRY("char *destination = <%p>, const char *source = <%p>, mcl_size_t count = <%lu>", destination, source, count);

#if defined(WIN32) || defined(WIN64)
    strncat_s(destination, strlen(destination) + count + 1, source, count);
#else
    strncat(destination, source, count);
#endif

    MCL_DEBUG_LEAVE("retVal = void");
}

mcl_error_t mcl_string_util_strncmp(const char *string_1, const char *string_2, mcl_size_t count)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("const char *string_1 = <%p>, const char *string_2 = <%p>, mcl_size_t count = <%lu>", string_1, string_2, count);

    // Null check.
    MCL_ASSERT_NOT_NULL(string_1, code);
    MCL_ASSERT_NOT_NULL(string_2, code);

    code = string_util_strncmp(string_1, string_2, count);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t string_util_strncmp(const char *string_1, const char *string_2, mcl_size_t count)
{
    mcl_error_t code;
    int result;

    MCL_DEBUG_ENTRY("const char *string_1 = <%p>, const char *string_2 = <%p>, mcl_size_t count = <%lu>", string_1, string_2, count);

    result = strncmp(string_1, string_2, count);

    // This check is necessary since result can be < 0.
    if (0 == result)
    {
        code = MCL_OK;
    }
    else
    {
        code = MCL_FAIL;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_string_util_snprintf(char *string, mcl_size_t length, const char *format, ...)
{
    mcl_error_t code;
    va_list args;

    MCL_DEBUG_ENTRY("char *string = <%p>, mcl_size_t length = <%lu>, const char *format = <%p>", string, length, format);

    // Initialize argument list.
    va_start(args, format);

    code = _print_formatted_string(string, length, format, args);

    // End using variable argument list.
    va_end(args);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t string_util_snprintf(char *string, mcl_size_t length, const char *format, ...)
{
    mcl_error_t code;
    va_list args;

    MCL_DEBUG_ENTRY("char *string = <%p>, mcl_size_t length = <%lu>, const char *format = <%p>", string, length, format);

    // Initialize argument list.
    va_start(args, format);

    code = _print_formatted_string(string, length, format, args);

    // End using variable argument list.
    va_end(args);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_bool_t mcl_string_util_memcmp(const void *block_1, const void *block_2, mcl_size_t count)
{
    mcl_bool_t result;

    MCL_DEBUG_ENTRY("const void *block_1 = <%p>, const void *block_2 = <%p>, mcl_size_t count = <%lu>", block_1, block_2, count);

    result = string_util_memcmp(block_1, block_2, count);

    MCL_DEBUG_LEAVE("retVal = <%s>", result ? "MCL_TRUE" : "MCL_FALSE");
    return result;
}

mcl_bool_t string_util_memcmp(const void *block_1, const void *block_2, mcl_size_t count)
{
    mcl_bool_t result;

    MCL_DEBUG_ENTRY("const void *block_1 = <%p>, const void *block_2 = <%p>, mcl_size_t count = <%lu>", block_1, block_2, count);

    result = (0 == memcmp(block_1, block_2, count)) ? MCL_TRUE : MCL_FALSE;

    MCL_DEBUG_LEAVE("retVal = <%s>", result ? "MCL_TRUE" : "MCL_FALSE");
    return result;
}

void mcl_string_util_memcpy(void *destination, const void *source, mcl_size_t count)
{
    MCL_DEBUG_ENTRY("void *destination = <%p>, const void *source = <%p>, mcl_size_t count = <%lu>", destination, source, count);

    string_util_memcpy(destination, source, count);

    MCL_DEBUG_LEAVE("retVal = void");
}

void string_util_memcpy(void *destination, const void *source, mcl_size_t count)
{
    MCL_DEBUG_ENTRY("void *destination = <%p>, const void *source = <%p>, mcl_size_t count = <%lu>", destination, source, count);

    memcpy(destination, source, count);

    MCL_DEBUG_LEAVE("retVal = void");
}

void mcl_string_util_memset(void *destination, mcl_uint8_t value, mcl_size_t count)
{
    MCL_DEBUG_ENTRY("void *destination = <%p>, mcl_uint8_t value = <%u>, mcl_size_t count = <%lu>", destination, value, count);

    if (MCL_NULL != destination)
    {
        string_util_memset(destination, value, count);
    }

    MCL_DEBUG_LEAVE("retVal = void");
}

void string_util_memset(void *destination, mcl_uint8_t value, mcl_size_t count)
{
    MCL_DEBUG_ENTRY("void *destination = <%p>, mcl_uint8_t value = <%u>, mcl_size_t count = <%lu>", destination, value, count);

    memset(destination, value, count);

    MCL_DEBUG_LEAVE("retVal = void");
}

char *mcl_string_util_strdup(const char *string)
{
    char *result = MCL_NULL;

    MCL_DEBUG_ENTRY("const char *string = <%p>", string);

    if (MCL_NULL != string)
    {
        result = string_util_strdup(string);
    }

    MCL_DEBUG_LEAVE("retVal = <%p>", result);
    return result;
}

char *string_util_strdup(const char *string)
{
    char *result;
    mcl_size_t string_length;

    MCL_DEBUG_ENTRY("const char *string = <%p>", string);

    string_length = string_util_strlen(string);
    result = MCL_MALLOC(string_length + MCL_NULL_CHAR_SIZE);

    if (MCL_NULL != result)
    {
        string_util_memcpy(result, string, string_length + MCL_NULL_CHAR_SIZE);
    }

    MCL_DEBUG_LEAVE("retVal = <%p>", result);
    return result;
}

mcl_bool_t mcl_string_util_find(const char *source, const char *target, mcl_size_t *start_index)
{
    mcl_bool_t is_found = MCL_FALSE;

    MCL_DEBUG_ENTRY("const char *source = <%p>, const char *target = <%p>, mcl_size_t *start_index = <%p>", source, target, start_index);

    if ((MCL_NULL != source) && (MCL_NULL != target))
    {
        is_found = string_util_find(source, target, start_index);
    }

    MCL_DEBUG_LEAVE("retVal = <%s>", is_found ? "MCL_TRUE" : "MCL_FALSE");
    return is_found;
}

mcl_bool_t string_util_find(const char *source, const char *target, mcl_size_t *start_index)
{
    mcl_size_t index;
    mcl_size_t source_length;
    mcl_size_t target_length;
    mcl_bool_t is_found = MCL_FALSE;

    MCL_DEBUG_ENTRY("const char *source = <%p>, const char *target = <%p>, mcl_size_t *start_index = <%p>", source, target, start_index);

    source_length = string_util_strlen(source);
    target_length = string_util_strlen(target);

    // Search target string in source string. If it is found, assign index of the first character of target to start index and return MCL_TRUE.
    if ((0 != target_length) && (target_length <= source_length))
    {
        mcl_size_t state = 0;

        for (index = 0; index < source_length; ++index)
        {
            state = (source[index] == target[state]) ? (state + 1) : 0;

            if (state == target_length)
            {
                *start_index = index - (target_length - 1);
                is_found = MCL_TRUE;
                break;
            }
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%s>", is_found ? "MCL_TRUE" : "MCL_FALSE");
    return is_found;
}

mcl_bool_t mcl_string_util_find_case_insensitive(const char *source, const char *target, mcl_size_t *start_index)
{
    mcl_bool_t is_found = MCL_FALSE;

    MCL_DEBUG_ENTRY("const char *source = <%p>, const char *target = <%p>, mcl_size_t *start_index = <%p>", source, target, start_index);

    if ((MCL_NULL != source) && (MCL_NULL != target))
    {
        is_found = string_util_find_case_insensitive(source, target, start_index);
    }

    MCL_DEBUG_LEAVE("retVal = <%s>", is_found ? "MCL_TRUE" : "MCL_FALSE");
    return is_found;
}

mcl_bool_t string_util_find_case_insensitive(const char *source, const char *target, mcl_size_t *start_index)
{
    mcl_size_t index;
    mcl_size_t source_length;
    mcl_size_t target_length;
    mcl_bool_t is_found = MCL_FALSE;

    MCL_DEBUG_ENTRY("char *source = <%p>, char *target = <%p>, mcl_size_t *start_index = <%p>", source, target, start_index);

    source_length = string_util_strlen(source);
    target_length = string_util_strlen(target);

    if ((0 != target_length) && (target_length <= source_length))
    {
        mcl_size_t state = 0;

        for (index = 0; index < source_length; ++index)
        {
            state = ((source[index] == target[state]) || (LOWERCASE(source[index]) == LOWERCASE(target[state]))) ? (state + 1) : 0;

            if (state == target_length)
            {
                *start_index = index - (target_length - 1);
                is_found = MCL_TRUE;
                break;
            }
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%s>", is_found ? "MCL_TRUE" : "MCL_FALSE");
    return is_found;
}

long mcl_string_util_strtol(const char *source, int base, char **end_pointer)
{
    long result;

    MCL_DEBUG_ENTRY("const char *source = <%p>, int base = <%d>, char **end_pointer = <%p>", source, base, end_pointer);

    result = string_util_strtol(source, base, end_pointer);

    MCL_DEBUG_LEAVE("retVal = <%u>", result);
    return result;
}

long string_util_strtol(const char *source, int base, char **end_pointer)
{
    long result;

    MCL_DEBUG_ENTRY("char* source = <%p>, int base = <%p>, char *end_pointer = <%p>", source, end_pointer, base);

    result = strtol(source, end_pointer, base);

    MCL_DEBUG_LEAVE("retVal = <%ld>", result);
    return result;
}

mcl_error_t mcl_string_util_reset(const void *value, char **target)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("const void *value = <%p>, char **target = <%p>", value, target);

    // Null check.
    MCL_ASSERT_NOT_NULL(value, code);
    MCL_ASSERT_NOT_NULL(target, code);

    code = string_util_reset(value, target);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t string_util_reset(const void *value, char **target)
{
    mcl_error_t code = MCL_OUT_OF_MEMORY;

    MCL_DEBUG_ENTRY("const void *value = <%p>, char **target = <%p>", value, target);

    // Clean previous target if exists.
    MCL_FREE(*target);

    // Duplicate value to target.
    *target = string_util_strdup(value);

    if (MCL_NULL != *target)
    {
        code = MCL_OK;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_string_util_concatenate(const char *string_1, const char *string_2, char **result)
{
    mcl_error_t code = MCL_TRIGGERED_WITH_NULL;

    MCL_DEBUG_ENTRY("const char *string_1 = <%p>, const char *string_2 = <%p>, char **result = <%p>", string_1, string_2, result);

    if ((MCL_NULL != string_1) && (MCL_NULL != string_2))
    {
        code = string_util_concatenate(string_1, string_2, result);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t string_util_concatenate(const char *string_1, const char *string_2, char **result)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t string_1_length;
    mcl_size_t string_2_length;

    MCL_DEBUG_ENTRY("const char *string_1 = <%p>, const char *string_2 = <%p>, char **result = <%p>", string_1, string_2, result);

    string_1_length = string_util_strlen(string_1);
    string_2_length = string_util_strlen(string_2);

    if (MCL_NULL != (*result = MCL_MALLOC(string_1_length + string_2_length + MCL_NULL_CHAR_SIZE)))
    {
        string_util_memcpy(*result, string_1, string_1_length);
        string_util_memcpy((*result) + string_1_length, string_2, string_2_length + MCL_NULL_CHAR_SIZE);
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _print_formatted_string(char *string, mcl_size_t length, const char *format, va_list args)
{
    mcl_error_t code;
    mcl_error_t count;

    MCL_DEBUG_ENTRY("char *string = <%p>, mcl_size_t length = <%lu>, const char *format = <%p>, va_list args = <%p>", string, length, format, args);

    count = vsnprintf(string, length, format, args);

    if ((count > 0) && (((mcl_size_t) count) <= length))
    {
        code = MCL_OK;
    }
    else
    {
        MCL_ERROR("Couldn't write the string!");
        code = MCL_FAIL;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
