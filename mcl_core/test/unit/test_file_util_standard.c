/*!
 * @file     test_file_util_standard.c
 * @brief    This file contains test case functions to test file_util module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "file_util.h"
#include "unity.h"
#include "string_util.h"
#include "mcl_core/mcl_file_util.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_log_util.h"

char *file_name = "temp.txt";
char *data_written = "123456";
mcl_size_t data_size = 6;

void setUp(void)
{
}

void tearDown(void)
{
}

/**
* GIVEN : A file name.
* WHEN  : mcl_file_util_fopen and mcl_file_util_fclose are called.
* THEN  : MCL_OK is returned for both function calls. File descriptor is not null after mcl_file_util_fopen is called.
*/
void test_fopen_001(void)
{
    // Open file.
    void *file_descriptor = MCL_NULL;
    mcl_error_t return_code = mcl_file_util_fopen(file_name, "w", &file_descriptor);

    // Test.
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    // Close file.
    return_code = mcl_file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");
}

/**
* GIVEN : A file is opened in write mode.
* WHEN  : mcl_file_util_fwrite is called.
* THEN  : MCL_OK is returned.
*/
void test_fwrite_001(void)
{
    // Open file.
    void *file_descriptor = MCL_NULL;
    mcl_error_t return_code = mcl_file_util_fopen(file_name, "w", &file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    // Test.
    return_code = mcl_file_util_fwrite(data_written, sizeof(char), data_size, file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "Data can not be written to file.");

    // Close file.
    return_code = mcl_file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");
}

/**
* GIVEN : A file is opened in read mode and data known a priori is already written to the file.
* WHEN  : mcl_file_util_fread is called.
* THEN  : MCL_OK is returned. Data read equals to data written.
*/
void test_fread_001(void)
{
    // Open file.
    void *file_descriptor = MCL_NULL;
    mcl_error_t return_code = mcl_file_util_fopen(file_name, "r", &file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    // Test.
    char *data_read = MCL_MALLOC(data_size);
    mcl_size_t actual_data_size = 0;
    mcl_file_util_fread(data_read, sizeof(char), data_size, file_descriptor, &actual_data_size);
    TEST_ASSERT_MESSAGE(data_size == actual_data_size, "Data size read mismatch the data size written.");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(data_written, data_read, actual_data_size, "Data read is not equal to data written.");

    // Close file.
    return_code = mcl_file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");

    // Clean up.
    MCL_FREE(data_read);
}

/**
* GIVEN : A file is opened in write mode.
* WHEN  : mcl_file_util_fputs is called.
* THEN  : MCL_OK is returned.
*/
void test_fputs_001(void)
{
    // Open file.
    void *file_descriptor = MCL_NULL;
    mcl_error_t return_code = mcl_file_util_fopen(file_name, "w", &file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    // Test.
    return_code = mcl_file_util_fputs(data_written, file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "Data can not be written to file.");

    // Close file.
    return_code = mcl_file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");
}

/**
* GIVEN : A file is opened in read mode and data known a priori is already written to the file.
* WHEN  : mcl_file_util_fgets is called.
* THEN  : MCL_OK is returned. Data read equals to data written.
*/
void test_fgets_001(void)
{
    // Open file.
    void *file_descriptor = MCL_NULL;
    mcl_error_t return_code = mcl_file_util_fopen(file_name, "r", &file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    char *data_read = MCL_MALLOC(data_size + 1);
    return_code = mcl_file_util_fgets(data_read, data_size + 1, file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "Data can not be read from file.");

    // Test.
    mcl_size_t actual_data_size;
    actual_data_size = string_util_strlen(data_read);
    TEST_ASSERT_MESSAGE(data_size == actual_data_size, "Data size read mismatch the data size written.");

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(data_written, data_read, actual_data_size, "Data read is not equal to data written.");

    // Close file.
    return_code = mcl_file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");

    // Clean up.
    MCL_FREE(data_read);
}

/**
* GIVEN : A file is opened in read mode and data known a priori is already written to the file.
* WHEN  : mcl_file_util_get_file_size is called.
* THEN  : MCL_OK is returned. Data read equals to data written.
*/
void test_get_file_size_001(void)
{
    // Open file.
    void *file_descriptor = MCL_NULL;
    mcl_error_t return_code = mcl_file_util_fopen(file_name, "w", &file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    char *data = "0123456789";
    return_code = mcl_file_util_fputs(data, file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "Data can not be written to file.");

    return_code = mcl_file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");

    return_code = mcl_file_util_fopen(file_name, "r", &file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    // Test.
    mcl_int64_t file_size = mcl_file_util_get_file_size(file_descriptor);
    TEST_ASSERT_EQUAL_MESSAGE(10, file_size, "File size is not correct.");

    // Close file.
    return_code = mcl_file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");
}

/**
* GIVEN : A file is opened in read mode and data known a priori is already written to the file.
* WHEN  : mcl_file_util_rewind is called.
* THEN  : MCL_OK is returned. Position of stream is set to the beginning.
*/
void test_rewind_001(void)
{
    // Open file.
    void *file_descriptor = MCL_NULL;
    mcl_error_t return_code = mcl_file_util_fopen(file_name, "r", &file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    // Test.
    char *data_read = MCL_MALLOC(data_size);
    mcl_size_t actual_data_size_1 = 0;
    mcl_file_util_fread(data_read, sizeof(char), data_size, file_descriptor, &actual_data_size_1);
    mcl_file_util_rewind(file_descriptor);
    mcl_size_t actual_data_size_2 = 0;
    mcl_file_util_fread(data_read, sizeof(char), data_size, file_descriptor, &actual_data_size_2);
    TEST_ASSERT_MESSAGE(actual_data_size_1 == actual_data_size_2, "Data size read mismatch the data size written.");

    // Close file.
    return_code = mcl_file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");

    // Clean up.
    MCL_FREE(data_read);
}
