/*!
 * @file     time_util.c
 * @brief    Time utility module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_core/mcl_time_util.h"
#include "time_util.h"
#include "string_util.h"
#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_memory.h"

// Feature test for gmtime_r.
#define GMTIME_R_EXISTS ((defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE>= 1) || defined(_XOPEN_SOURCE) || defined(_BSD_SOURCE) || defined(_SVID_SOURCE) || defined(_POSIX_SOURCE))

#define TIMESTAMP_FIELD_COUNT 7
#define TIMESTAMP_DATE_INDEX 2

mcl_error_t mcl_time_util_convert_to_iso_8601_format(const time_t *time_value, char *iso8601_formatted_time)
{
    mcl_error_t return_code = MCL_OK;

#if defined(WIN32) || defined(WIN64) || GMTIME_R_EXISTS
    struct tm time_structure;
#endif

#if defined(WIN32) || defined(WIN64)
    errno_t result;
#else
    struct tm *temp;
#endif

    MCL_DEBUG_ENTRY("const time_t *time_value = <%p>, char **iso8601_formatted_time = <%p>", time_value, iso8601_formatted_time);

    // Populate time structure for the given time value.
#if defined(WIN32) || defined(WIN64)
    result = gmtime_s(&time_structure, time_value);
    MCL_ASSERT_CODE_MESSAGE(0 == result, MCL_FAIL, "gmtime_s() function returns error.");

#elif GMTIME_R_EXISTS
    temp = gmtime_r(time_value, &time_structure);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != temp, MCL_FAIL, "gmtime_r() function returns null.");
#else
    temp = gmtime(time_value);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != temp, MCL_FAIL, "gmtime() function returns null.");
#endif

    // Set empty right away.
    iso8601_formatted_time[0] = MCL_NULL_CHAR;

    // Finalize the buffer.
    iso8601_formatted_time[MCL_TIMESTAMP_LENGTH - MCL_NULL_CHAR_SIZE] = MCL_NULL_CHAR;

    // Compose the string in ISO 8601 date and time format.
#if defined(WIN32) || defined(WIN64) || GMTIME_R_EXISTS
    if (0 == strftime(iso8601_formatted_time, MCL_TIMESTAMP_LENGTH, "%Y-%m-%dT%H:%M:%S.000Z", &time_structure))
    {
        MCL_DEBUG("File creation time can not be converted to ISO-8601 date and time format.");
        return_code = MCL_FAIL;
    }
#else
    if (0 == strftime(iso8601_formatted_time, MCL_TIMESTAMP_LENGTH, "%Y-%m-%dT%H:%M:%S.000Z", temp))
    {
        MCL_DEBUG("File creation time can not be converted to ISO-8601 date and time format.");
        return_code = MCL_FAIL;
    }
#endif

    MCL_DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
}

mcl_bool_t mcl_time_util_validate_timestamp(const char *timestamp)
{
    const char characters_to_check[TIMESTAMP_FIELD_COUNT] = { '-', '-', 'T', ':', ':', '.', 'Z' };
    const uint16_t maximum_values[TIMESTAMP_FIELD_COUNT] = { 2999, 12, 31, 23, 59, 59, 999 };
    const uint8_t character_indexes_to_check[TIMESTAMP_FIELD_COUNT] = { 4, 7, 10, 13, 16, 19, 23 };
    const char *position = timestamp;
    char *end_pointer = MCL_NULL;
    mcl_bool_t ok;
    uint8_t index;
    const mcl_size_t expected_timestamp_length = MCL_TIMESTAMP_LENGTH - MCL_NULL_CHAR_SIZE;
    mcl_size_t actual_timestamp_length = 0;

    MCL_DEBUG_ENTRY("const char *timestamp = <%p>", timestamp);

    // Check timestamp length.
    actual_timestamp_length = string_util_strlen(timestamp);
    ok = expected_timestamp_length == actual_timestamp_length;

    // Check timestamp format.
    for (index = 0; (index < TIMESTAMP_FIELD_COUNT) && (MCL_TRUE == ok); ++index)
    {
        ok = (characters_to_check[index] == timestamp[character_indexes_to_check[index]]);
    }

    // Check date and time values.
    for (index = 0; (index < TIMESTAMP_FIELD_COUNT) && (MCL_TRUE == ok); ++index)
    {
        // 10 is base.
        mcl_int32_t int_value = (mcl_int32_t) string_util_strtol(position, 10, &end_pointer);

        ok = (maximum_values[index] >= int_value);

        if (MCL_TRUE == ok)
        {
            if (TIMESTAMP_DATE_INDEX >= index)
            {
                // Date values can not be zero.
                ok = (int_value > 0);
            }
            else
            {
                ok = (int_value >= 0);
            }
        }

        // To show the address after the special character.
        position = end_pointer + 1;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", ok);
    return ok;
}

void time_util_get_time(mcl_time_t *current_time)
{
    MCL_DEBUG_ENTRY("mcl_time_t *current_time = <%p>", current_time);

    time(current_time);

    MCL_DEBUG_LEAVE("retVal = void");
}
