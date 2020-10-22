/*!
 * @file     test_string_util.c
 * @brief    This file contains test case functions to test string_util module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_string_util.h"
#include "string_util.h"
#include "mcl_core/mcl_memory.h"
#include "definitions.h"
#include "mcl_core/mcl_log_util.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Null terminated string.
 * WHEN  : mcl_string_util_strlen() is called.
 * THEN  : Length of the string is returned.
 */
void test_strlen_001(void)
{
    // Variable declaration.
    const char *string = "dummy";
    mcl_size_t length_of_string;

    // Test.
    length_of_string = mcl_string_util_strlen(string);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(5, length_of_string, "mcl_string_util_strlen() returned wrong value.");
}

/**
 * GIVEN : Null as string.
 * WHEN  : mcl_string_util_strlen() is called.
 * THEN  : Zero is returned as length.
 */
void test_strlen_002(void)
{
    // Variable declaration.
    mcl_size_t length_of_string;
    
    // Test.
    length_of_string = mcl_string_util_strlen(NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(0, length_of_string, "mcl_string_util_strlen() returned wrong value.");
}

/**
 * GIVEN : Null terminated string.
 * WHEN  : mcl_string_util_strnlen() is called.
 * THEN  : Length of the string is returned.
 */
void test_strnlen_001(void)
{
    // Variable declaration.
    const char *string = "dummy";
    mcl_size_t maximum_length = 10;
    mcl_size_t length_of_string;

    // Test.
    length_of_string = mcl_string_util_strnlen(string, maximum_length);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(5, length_of_string, "mcl_string_util_strnlen() returned wrong value.");
}

/**
 * GIVEN : Null as string.
 * WHEN  : mcl_string_util_strnlen() is called.
 * THEN  : Zero is returned as length.
 */
void test_strnlen_002(void)
{
    // Variable declaration.
    mcl_size_t maximum_length = 10;
    mcl_size_t length_of_string;

    // Test.
    length_of_string = mcl_string_util_strnlen(NULL, maximum_length);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(0, length_of_string, "mcl_string_util_strnlen() returned wrong value.");
}

/**
 * GIVEN : String without NULL termination.
 * WHEN  : mcl_string_util_strnlen() is called.
 * THEN  : maximum_length is returned as length.
 */
void test_strnlen_003(void)
{
    // Variable declaration.
    const char string[12] = "dummy string";
    mcl_size_t maximum_length = 10;
    mcl_size_t length_of_string;

    // Test.
    length_of_string = mcl_string_util_strnlen(string, maximum_length);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(maximum_length, length_of_string, "mcl_string_util_strnlen() returned wrong value.");
}

/**
 * GIVEN : String to be copied, destination to be copied to and count.
 * WHEN  : mcl_string_util_strncpy() is called.
 * THEN  : String is copied to destination as specified count.
 */
void test_strncpy_001(void)
{
    // Variable declaration.
    const char *source = "dummy";
    mcl_size_t count_to_copy = 5;
    char destination[6];
    destination[count_to_copy] = MCL_NULL_CHAR;

    // Test.
    mcl_string_util_strncpy(destination, source, count_to_copy);

    // Check result.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(source, destination, "mcl_string_util_strncpy() failed.");
}

/**
 * GIVEN : Source to be concatenated at the end of destination as specified count.
 * WHEN  : mcl_string_util_strncat() is called.
 * THEN  : Source is added at the end of destination.
 */
void test_strncat_001(void)
{
    // Store string in destination before concatenation. 20 = enough space. 6 = size of "dummy".
    char destination[20];
    mcl_string_util_strncpy(destination, "dummy", 6);
    const char *source = "string";

    // Test (7 = size of "string").
    mcl_string_util_strncat(destination, source, 7);

    // Cehck result.
    TEST_ASSERT_EQUAL_STRING_MESSAGE("dummystring", destination, "mcl_string_util_strncat() failed.");
}

/**
 * GIVEN : Two equal strings to compare and count.
 * WHEN  : mcl_string_util_strncmp() is called.
 * THEN  : MCL_OK is returned.
 */
void test_strncmp_001(void)
{
    // Variable declaration.
    char *string_1 = "dummy";
    const char *string_2 = "dummy";
    mcl_size_t count_to_compare = 6;

    // Test.
    mcl_error_t code = mcl_string_util_strncmp(string_1, string_2, count_to_compare);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_string_util_strncmp failed.");
}

/**
 * GIVEN : Two unequal strings to compare and count.
 * WHEN  : mcl_string_util_strncmp() is called.
 * THEN  : MCL_FAIL is returned.
 */
void test_strncmp_002(void)
{
    // Variable declaration.
    char *string_1 = "dummy";
    const char *string_2 = "dum";
    mcl_size_t count_to_compare = 6;

    // Test.
    mcl_error_t code = mcl_string_util_strncmp(string_1, string_2, count_to_compare);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_FAIL, code, "mcl_string_util_strncmp failed.");
}

/**
 * GIVEN : Enough space to store formatted string and two variable arguments.
 * WHEN  : mcl_string_util_snprintf() is called.
 * THEN  : MCL_OK is returned.
 */
void test_snprintf_001(void)
{
    // Variable declaration.
    mcl_size_t length = 50;
    char formatted_string[50];
    const char *format = "The half of %d is %d.";

    // Test.
    mcl_error_t code = mcl_string_util_snprintf(formatted_string, length, format, 60, 30);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_string_util_snprintf() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("The half of 60 is 30.", formatted_string, "mcl_string_util_snprintf() failed with formatted string.");
}

/**
 * GIVEN : Insufficient space to store formatted string and two variable arguments.
 * WHEN  : mcl_string_util_snprintf() is called.
 * THEN  : MCL_FAIL is returned.
 */
void test_snprintf_002(void)
{
    // Variable declaration.
    mcl_size_t length = 3;
    char formatted_string[3];
    const char *format = "The half of %d is %d.";

    // Test.
    mcl_error_t code = mcl_string_util_snprintf(formatted_string, length, format, 60, 30);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_FAIL, code, "mcl_string_util_snprintf() failed.");
}

/**
 * GIVEN : Enough space to store formatted string and two variable arguments.
 * WHEN  : string_util_snprintf() is called.
 * THEN  : MCL_OK is returned.
 */
void test_snprintf_003(void)
{
    // Variable declaration.
    mcl_size_t length = 50;
    char formatted_string[50];
    const char *format = "The half of %d is %d.";

    // Test.
    mcl_error_t code = string_util_snprintf(formatted_string, length, format, 60, 30);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "string_util_snprintf() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("The half of 60 is 30.", formatted_string, "string_util_snprintf() failed with formatted string.");
}

/**
 * GIVEN : Insufficient space to store formatted string and two variable arguments.
 * WHEN  : string_util_snprintf() is called.
 * THEN  : MCL_FAIL is returned.
 */
void test_snprintf_004(void)
{
    // Variable declaration.
    mcl_size_t length = 3;
    char formatted_string[3];
    const char *format = "The half of %d is %d.";

    // Test.
    mcl_error_t code = string_util_snprintf(formatted_string, length, format, 60, 30);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_FAIL, code, "string_util_snprintf() failed.");
}

/**
 * GIVEN : Two equal memory blocks to compare with specified count.
 * WHEN  : mcl_string_util_memcmp() is called.
 * THEN  : MCL_TRUE is returned.
 */
void test_memcmp_001(void)
{
    // Variable declaration.
    mcl_size_t count = 5;
    const char buffer1[] = "dummy";
    const char buffer2[] = "dummy";
    mcl_bool_t result;

    // Test.
    result = mcl_string_util_memcmp(buffer1, buffer2, count);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRUE, result, "mcl_string_util_memcmp() failed.");
}

/**
 * GIVEN : Two unequal memory blocks to compare with specified count.
 * WHEN  : mcl_string_util_memcmp() is called.
 * THEN  : MCL_FALSE is returned.
 */
void test_memcmp_002(void)
{
    // Variable declaration.
    mcl_size_t count = 6;
    const char buffer1[] = "dummy";
    const char buffer2[] = "dummy2";
    mcl_bool_t result;

    // Test.
    result = mcl_string_util_memcmp(buffer1, buffer2, count);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_FALSE, result, "mcl_string_util_memcmp() failed.");
}

/**
 * GIVEN : Source, destination and count for copy.
 * WHEN  : mcl_string_util_memcpy() is called.
 * THEN  : Source is copied to destination.
 */
void test_memcpy_001(void)
{
    // Variable declaration.
    char destination[10];
    const char *source = "dummy";

    // Test.
    mcl_string_util_memcpy(destination, source, 6);

    // Check result.
    TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE(source, destination, 5, "mcl_string_util_memcpy() failed.");
}

/**
 * GIVEN : String to duplicate.
 * WHEN  : mcl_string_util_strdup() is called.
 * THEN  : The string is duplicated and returned.
 */
void test_strdup_001(void)
{
    // Variable declaration.
    const char *string = "dummy";
    char *duplicated_string;

    // Test;
    duplicated_string = mcl_string_util_strdup(string);

    // Check result.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(string, duplicated_string, "mcl_string_util_strdup() failed.");

    // Clean up.
    MCL_FREE(duplicated_string);
}

/**
 * GIVEN : String which starts with number.
 * WHEN  : mcl_string_util_strtol() is called.
 * THEN  : The number at the beginning is returned.
 */
void test_strtol_001(void)
{
    // Variable declaration.
    char *string_part;
    const char *source = "2030300 This is test";
    long value;

    // Test.
    value = mcl_string_util_strtol(source, 10, &string_part);

    // Check results.
    TEST_ASSERT_EQUAL_INT32_MESSAGE(2030300, value, "mcl_string_util_strtol() failed.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(" This is test", string_part, "mcl_string_util_strtol() failed.");
}

/**
 * GIVEN : Buffer whose content is not initialised.
 * WHEN  : mcl_string_util_memset() is called.
 * THEN  : The buffer is filled with the specified value.
 */
void test_memset_001(void)
{
    // Variable declaration.
    mcl_uint8_t buffer[8];

    // Test.
    mcl_string_util_memset(buffer, 'M', 8);

    // Check result.
    for (mcl_size_t i = 0; i < 8; i++)
    {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE('M', buffer[i], "Buffer content is different than expected.");
    }
}

/**
 * GIVEN : String which contains a substring that is being looked for.
 * WHEN  : mcl_string_util_find() is called.
 * THEN  : MCL_TRUE is returned and start index is correct.
 */
void test_find_001(void)
{
    // Variable declaration.
    mcl_bool_t result;
    mcl_size_t start_index;

    // Initialize test strings.
    const char test_string[] = "This is a test string";
    const char test_substring[] = "test";

    // Test.
    result = mcl_string_util_find(test_string, test_substring, &start_index);

    // Check results.
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MCL_TRUE, result, "mcl_string_util_find() did not return MCL_TRUE.");
    TEST_ASSERT_EQUAL_MESSAGE(10, start_index, "Start index is wrong.");
}

/**
 * GIVEN : String which contains a substring (case insensitive) that is being looked for.
 * WHEN  : mcl_string_util_find_case_insensitive() is called.
 * THEN  : MCL_TRUE is returned and start index is correct.
 */
void test_find_case_insensitive_001(void)
{
    // Variable declaration.
    mcl_bool_t result;
    mcl_size_t start_index;

    // Initialize test strings.
    const char test_string[] = "This is a test string";
    const char test_substring[] = "TEST";

    // Test.
    result = mcl_string_util_find_case_insensitive(test_string, test_substring, &start_index);

    // Check results.
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MCL_TRUE, result, "mcl_string_util_find_case_insensitive() did not return MCL_TRUE.");
    TEST_ASSERT_EQUAL_MESSAGE(10, start_index, "Start index is wrong.");
}

/**
 * GIVEN : Value and target.
 * WHEN  : mcl_string_util_reset() is called two times.
 * THEN  : MCL_OK is returned and target is initialized with the given value each time.
 */
void test_reset_001(void)
{
    // Variable declaration.
    char *target = NULL;
    const void *value_1 = "value to initialize target.";
    const void *value_2 = "second value to initialize target.";

    // Test first call.
    mcl_error_t code = mcl_string_util_reset(value_1, &target);

    // Check first result.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(value_1, target, "mcl_string_util_reset() failed at first call.");

    // Test second call.
    code = mcl_string_util_reset(value_2, &target);

    // Check second result.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(value_2, target, "mcl_string_util_reset() failed at second call.");

    // Clean up.
    MCL_FREE(target);
}

/**
 * GIVEN : Value is null.
 * WHEN  : mcl_string_util_reset() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_reset_002(void)
{
    // Variable declaration.
    char *target = NULL;

    // Test.
    mcl_error_t code = mcl_string_util_reset(NULL, &target);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_string_util_reset() failed for null value parameter.");
}

/**
 * GIVEN : Target is null.
 * WHEN  : mcl_string_util_reset() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_reset_003(void)
{
    // Variable declaration.
    const void *value = "value to initialize target.";

    // Test.
    mcl_error_t code = mcl_string_util_reset(value, NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_string_util_reset() failed for null target parameter.");
}
