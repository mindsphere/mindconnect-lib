/*!
 * @file     test_log_util.c
 * @brief    Unit test cases for mcl_log_util.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_core/mcl_log_util.h"
#include "log_util.h"
#include "mcl_core/mcl_file_util.h"
#include "file_util.h"

// _callback_check is used to check whether callback function is called.
static mcl_bool_t _callback_check;

// Callback function which only sets _callback_check.
static void _dummy_log_function(void *user_context, int log_level, const char *file, int line, const char *tag, const char *format, ...);

void setUp(void)
{

}

void tearDown(void)
{

}

/**
 * GIVEN : A valid callback function.
 * WHEN  : #mcl_log_util_set_callback() is called with the given callback function.
 * THEN  : Callback function is used for logging.
 */
void test_set_callback_001()
{
    mcl_error_t result;
    _callback_check = MCL_FALSE;

    // Test.
    result = mcl_log_util_set_callback(_dummy_log_function, MCL_NULL);
    MCL_FATAL_STRING("Test log");
    
    // Check result.
    TEST_ASSERT_MESSAGE(MCL_OK == result, "mcl_log_util_set_callback() does not return MCL_OK.");
    TEST_ASSERT_MESSAGE(MCL_TRUE == _callback_check, "Callback function has not been called.");
}

/**
 * GIVEN : MCL_LOG_LEVEL is not MCL_LOG_LEVEL_VERBOSE.
 * WHEN  : #mcl_log_util_set_output_level() is called with MCL_LOG_LEVEL_VERBOSE.
 * THEN  : MCL_INVALID_LOG_LEVEL is returned.
 */
void test_set_output_level_001()
{
#if !MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_VERBOSE)
    mcl_error_t result;

    // Test.
    result = mcl_log_util_set_output_level(MCL_LOG_LEVEL_VERBOSE);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_INVALID_LOG_LEVEL == result, "A level which is disabled at compile time, must be an invalid level to set.");

#else
    TEST_IGNORE();
#endif
}

/**
 * GIVEN : MCL_LOG_LEVEL_WARN is enabled at compile time.
 * WHEN  : #mcl_log_util_set_output_level() is called with MCL_LOG_LEVEL_FATAL.
 * THEN  : MCL_WARN does not log anything.
 */
void test_set_output_level_002()
{
#if MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_WARN)
    mcl_error_t result;
    (void) mcl_log_util_set_callback(_dummy_log_function, MCL_NULL);
    _callback_check = MCL_FALSE;

    // Test.
    result = mcl_log_util_set_output_level(MCL_LOG_LEVEL_FATAL);
    MCL_WARN("Test log");

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK should be returned.");
    TEST_ASSERT_MESSAGE(MCL_FALSE == _callback_check, "Callback function should not be called.");
#else
    TEST_IGNORE();
#endif
}

/**
 * GIVEN : No initial condition.
 * WHEN  : #mcl_log_util_set_output_level() is called with non-existent log level.
 * THEN  : MCL_INVALID_LOG_LEVEL must be returned.
 */
void test_set_output_level_003()
{
    mcl_error_t result;

    // Test.
    result = mcl_log_util_set_output_level(MCL_LOG_LEVEL_NONE - 1);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_INVALID_LOG_LEVEL == result, "mcl_log_util_set_output_level() does not return MCL_INVALID_LOG_LEVEL.");
}

static void _dummy_log_function(void *user_context, int log_level, const char *file, int line, const char *tag, const char *format, ...)
{
    _callback_check = MCL_TRUE;
}

