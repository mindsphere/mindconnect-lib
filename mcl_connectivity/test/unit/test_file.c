/*!
 * @file     test_file.c
 * @brief    This file implements all test cases of mcl_file module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_connectivity/mcl_file.h"
#include "file.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_file_util.h"
#include "mcl_core/mcl_string_util.h"

// Creates file.
static mcl_error_t _create_file(const char *file_content, const char *file_name);

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Meta payload version is given as 1.0.
 * WHEN  : mcl_file_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized file data struct with meta payload version 1.0.
 */
void test_initialize_001(void)
{
    // Initialize variables.
    mcl_error_t code;
    mcl_file_t *file = MCL_NULL;
    E_MCL_FILE_VERSION version = MCL_FILE_VERSION_1_0;

    // Test.
    code = mcl_file_initialize(version, &file);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for file.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file, "File is NULL.");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_ITEM_TYPE_FILE, file->item_base.type, "Item type is not file.");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((mcl_uint32_t)version, file->item_base.version, "File meta payload version is wrong.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file->payload, "File payload is NULL.");
    TEST_ASSERT_NULL_MESSAGE(file->payload->file_descriptor, "File descriptor is not NULL.");
    TEST_ASSERT_NULL_MESSAGE(file->payload->remote_name, "File remote name is not NULL.");
    TEST_ASSERT_NULL_MESSAGE(file->payload->local_path, "File local path is not NULL.");
    TEST_ASSERT_NULL_MESSAGE(file->payload->type, "File type is not NULL.");
    TEST_ASSERT_NULL_MESSAGE(file->payload->creation_date, "File creation date is not NULL.");
    TEST_ASSERT_EQUAL_MESSAGE(0, file->payload->size, "File size is not 0.");

    // Clean up.
    mcl_file_destroy(&file);
}

/**
 * GIVEN : mcl_file_t parameter is given as MCL_NULL.
 * WHEN  : mcl_file_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002(void)
{
    // Initialize variables.
    mcl_error_t code;
    E_MCL_FILE_VERSION version = MCL_FILE_VERSION_1_0;

    // Test.
    code = mcl_file_initialize(version, MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_file_initialize() unexpected return error!");
}

/**
 * GIVEN : Meta payload version is given as undefined E_MCL_FILE_VERSION.
 * WHEN  : mcl_file_initialize() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_initialize_003(void)
{
    // Initialize variables.
    mcl_error_t code;
    mcl_file_t *file = MCL_NULL;
    E_MCL_FILE_VERSION version_invalid = MCL_FILE_VERSION_END;

    // Test.
    code = mcl_file_initialize(version_invalid, &file);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_file_initialize() unexpected return error!");
}

/**
 * GIVEN : File parameter is MCL_NULL.
 * WHEN  : mcl_file_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_001(void)
{
    const char *file_type = "log";
    mcl_error_t code = mcl_file_set_parameter(MCL_NULL, MCL_FILE_PARAMETER_TYPE, file_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_file_set_parameter() failed when file is NULL.");
}

/**
 * GIVEN : Value parameter is MCL_NULL.
 * WHEN  : mcl_file_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_002(void)
{
    mcl_file_t file;
    mcl_error_t code = mcl_file_set_parameter(&file, MCL_FILE_PARAMETER_TYPE, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_file_set_parameter() failed when value is NULL.");
}

/**
 * GIVEN : Undefined E_MCL_FILE_PARAMETER parameter.
 * WHEN  : mcl_file_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_003(void)
{
    mcl_file_t file;
    const char *dummy_value = "log";
    mcl_error_t code = mcl_file_set_parameter(&file, MCL_FILE_PARAMETER_END, dummy_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_file_set_parameter() failed when E_MCL_FILE_PARAMETER is undefined.");
}

/**
 * GIVEN : Initialized file.
 * WHEN  : mcl_file_set_parameter() is called for each parameter.
 * THEN  : MCL_OK is returned and all fields are set.
 */
void test_set_parameter_004(void)
{
    // Initialize variables.
    mcl_file_t *file = MCL_NULL;
    const char *file_type = "text";
    const char *remote_file_name = "data_file.txt";
    const char *file_local_path = "data_file.txt";
    const char *file_data = "0123456789";
    void *file_descriptor = MCL_NULL;

    // Create a file containing data in it, to be used by mcl_file_set_parameter().
    mcl_error_t code = _create_file(file_data, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "File could not be created.");

    // Initialize file.
    code = mcl_file_initialize(MCL_FILE_VERSION_1_0, &file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for file.");

    // Set file type.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_TYPE, file_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file type.");

    // Set file remote name.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_REMOTE_NAME, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file name.");

    // Set file local path.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_LOCAL_PATH, file_local_path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file local path.");

    // Check file type.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(file_type, file->payload->type, "File type is wrong.");

    // Check file remote name.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(remote_file_name, file->payload->remote_name, "Remote file name is wrong.");

    // Check file local path.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(file_local_path, file->payload->local_path, "File local path is wrong.");

    // Check file size.
    TEST_ASSERT_EQUAL_MESSAGE(mcl_string_util_strlen(file_data), file->payload->size, "File size is wrong.");

    // Clean up.
    mcl_file_destroy(&file);
    remove(remote_file_name);
}

/**
 * GIVEN : Initialized file.
 * WHEN  : mcl_file_set_parameter() is called two times for each parameter.
 * THEN  : MCL_OK is returned and the parameter is reset.
 */
void test_set_parameter_005(void)
{
    // Initialize variables.
    mcl_file_t *file = MCL_NULL;
    const char *file_type = "text";
    const char *file_type_2 = "log";
    const char *remote_file_name = "data_file.txt";
    const char *remote_file_name_2 = "log_file.txt";
    const char *file_local_path = "data_file.txt";
    const char *file_local_path_2 = "log_file.txt";
    const char *file_data = "0123456789";
    void *file_descriptor = MCL_NULL;

    // Create a file containing data in it, to be used by mcl_file_set_parameter().
    mcl_error_t code = _create_file(file_data, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "File could not be created.");

    // Create second file containing data in it, to be used by mcl_file_set_parameter().
    code = _create_file(file_data, remote_file_name_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Second file could not be created.");

    // Initialize file.
    code = mcl_file_initialize(MCL_FILE_VERSION_1_0, &file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for file.");

    // Set file type.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_TYPE, file_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file type.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(file_type, file->payload->type, "File type is wrong.");

    // Set file type second time.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_TYPE, file_type_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file type second time.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(file_type_2, file->payload->type, "File type is wrong for second call.");

    // Set file remote name.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_REMOTE_NAME, remote_file_name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file name.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(remote_file_name, file->payload->remote_name, "Remote file name is wrong for second call.");

    // Set file remote name second time.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_REMOTE_NAME, remote_file_name_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file name second time.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(remote_file_name_2, file->payload->remote_name, "Remote file name is wrong for second call.");

    // Set file local path.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_LOCAL_PATH, file_local_path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file local path.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(file_local_path, file->payload->local_path, "File local path is wrong.");

    // Set file local path second time.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_LOCAL_PATH, file_local_path_2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_file_set_parameter() failed while setting file local path second time.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(file_local_path_2, file->payload->local_path, "File local path is wrong for second call.");

    // Check file size.
    TEST_ASSERT_EQUAL_MESSAGE(mcl_string_util_strlen(file_data), file->payload->size, "File size is wrong.");

    // Clean up.
    mcl_file_destroy(&file);
    remove(remote_file_name);
    remove(remote_file_name_2);
}

/**
 * GIVEN : No file.
 * WHEN  : mcl_file_set_parameter() is called.
 * THEN  : MCL_FILE_CANNOT_BE_OPENED is returned.
 */
void test_set_parameter_006(void)
{
    // Initialize variables.
    mcl_file_t *file = MCL_NULL;
    const char *file_local_path = "data_file.txt";

    // Initialize file.
    mcl_error_t code = mcl_file_initialize(MCL_FILE_VERSION_1_0, &file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for file.");

    // Set payload size to a value to see it is set to zero after mcl_file_set_parameter() fails.
    file->payload->size = 10;

    // Set file local path.
    code = mcl_file_set_parameter(file, MCL_FILE_PARAMETER_LOCAL_PATH, file_local_path);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_FILE_CANNOT_BE_OPENED, code, "mcl_file_set_parameter() failed while setting file local path.");
    TEST_ASSERT_NULL_MESSAGE(file->payload->local_path, "File local path is not null.");
    TEST_ASSERT_NULL_MESSAGE(file->payload->file_descriptor, "File descriptor is not null.");
    TEST_ASSERT_EQUAL_MESSAGE(file->payload->size, 0, "mcl_file_set_parameter() failed while setting file local path.");

    // Clean up.
    mcl_file_destroy(&file);
}

/**
 * GIVEN : Mandatory file parameters.
 * WHEN  : file_validate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_file_validate_001(void)
{
    mcl_error_t code;

    // Set file parameters.
    file_payload_t file_payload =
    {
       .creation_date = "2018-10-02T08:00:00.000Z",
       .local_path = "local_path",
       .remote_name = "remote_name",
       .type = "file_type",
       .size = 0,
       .file_descriptor = MCL_NULL
    };

    file_t file =
    {
       .item_base =
       {
           .preamble = MCL_ITEM_PREAMBLE,
           .type = MCL_ITEM_TYPE_FILE,
           .version = MCL_FILE_VERSION_1_0
       },
       .payload = &file_payload
    };

    // Test.
    code = file_validate(&file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "file_validate() failed.");
}

/**
 * GIVEN : Mandatory file parameters are not set.
 * WHEN  : file_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_file_validate_002(void)
{
    // Initialize file.
    mcl_file_t *file = MCL_NULL;
    E_MCL_FILE_VERSION version = MCL_FILE_VERSION_1_0;
    mcl_error_t code = mcl_file_initialize(version, &file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for file.");

    // Test.
    code = file_validate(file);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "file_validate() failed.");

    // Clean up.
    mcl_file_destroy(&file);
}

static mcl_error_t _create_file(const char *file_content, const char *file_name)
{
    MCL_DEBUG_ENTRY("const char *file_content = <%p>, const char *file_name = <%p>", file_content, file_name);

    void *file_descriptor = MCL_NULL;
    mcl_error_t code = mcl_file_util_fopen(file_name, "w", &file_descriptor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "File couldn't be opened.");

    // Write data to the file.
    code = mcl_file_util_fputs(file_content, file_descriptor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Data couldn't be written to file.");

    // Close the file.
    mcl_file_util_fclose(file_descriptor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "File can not be closed.");

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}
