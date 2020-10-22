/*!
 * @file     test_time_util.c
 * @brief    This file contains test case functions to test time_util module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_core/mcl_time_util.h"
#include "time_util.h"
#include "string_util.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_log_util.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : A time value.
 * WHEN  : mcl_time_util_convert_to_iso_8601_format() is called.
 * THEN  : MCL_OK is returned and resulting ISO 8601 date and time corresponds to the given time value.
 */
void test_convert_to_iso_8601_format_001(void)
{
    // Variable declaration.
    time_t time_value = 1482402748;
    char iso8601_formatted_time[MCL_TIMESTAMP_LENGTH];
    char *expected_time = "2016-12-22T10:32:28.000Z";

    // Test.
    mcl_error_t return_code = mcl_time_util_convert_to_iso_8601_format(&time_value, iso8601_formatted_time);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "Time value can not be converted to ISO 8601 date and time format.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_time, iso8601_formatted_time, "ISO 8601 time mismatch.");
}

/**
 * GIVEN : A timestamp which has a wrong format.
 * WHEN  : mcl_time_util_validate_timestamp() is called.
 * THEN  : MCL_FALSE is returned.
 */
void test_validate_timestamp_001(void)
{
    // Variable declaration.
    char *timestamp = "2016-04-26X08:06:25.317Z";

    // Test.
    mcl_bool_t result = mcl_time_util_validate_timestamp(timestamp);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_FALSE == result, "MCL_FALSE didn't return for wrong format.");
}

/**
 * GIVEN : A timestamp which has a wrong length.
 * WHEN  : mcl_time_util_validate_timestamp() function is called.
 * THEN  : MCL_FALSE is returned.
 */
void test_validate_timestamp_002(void)
{
    // Variable declaration.
    char *timestamp = "2016-04-26T08:06:25.31";

    // Test.
    mcl_bool_t result = mcl_time_util_validate_timestamp(timestamp);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_FALSE == result, "MCL_FALSE didn't return for wrong length.");
}

/**
 * GIVEN : A timestamp which has an invalid hour value.
 * WHEN  : mcl_time_util_validate_timestamp() function is called.
 * THEN  : MCL_FALSE is returned.
 */
void test_validate_timestamp_003(void)
{
    // Variable declaration.
    char *timestamp = "2016-04-26T25:06:25.317Z";

    // Test.
    mcl_bool_t result = mcl_time_util_validate_timestamp(timestamp);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_FALSE == result, "MCL_FALSE didn't return for wrong time value.");
}

/**
* GIVEN : A timestamp which has a valid format.
* WHEN  : mcl_time_util_validate_timestamp() function is called.
* THEN  : MCL_TRUE is returned.
*/
void test_validate_timestamp_004(void)
{
    // Variable declaration.
    char *timestamp = "2018-02-19T20:06:25.317Z";

    // Test.
    mcl_bool_t result = mcl_time_util_validate_timestamp(timestamp);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_TRUE == result, "MCL_TRUE didn't return for correct time value.");
}
