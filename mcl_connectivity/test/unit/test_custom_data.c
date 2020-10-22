/*!
 * @file     test_custom_data.c
 * @brief    This file implements all test cases of mcl_custom_data module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_connectivity/mcl_custom_data.h"
#include "custom_data.h"
#include "mcl_core/mcl_json_util.h"
#include "mcl_core/mcl_memory.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Meta payload version is given as "version 2.0".
 * WHEN  : mcl_custom_data_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized custom_data data struct with meta payload version "version 2.0".
 */
void test_initialize_001(void)
{
    // Initialize variables.
    mcl_error_t code;
    mcl_custom_data_t *custom_data = MCL_NULL;
    char *version = "version 2.0";

    // Test.
    code = mcl_custom_data_initialize(version, &custom_data);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for custom data.");
    TEST_ASSERT_NOT_NULL_MESSAGE(custom_data, "Custom data is NULL.");
    TEST_ASSERT_NOT_NULL_MESSAGE(custom_data->payload, "Custom data payload is NULL.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(version, custom_data->payload->version, "Custom data meta payload version is wrong.");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_ITEM_TYPE_CUSTOM_DATA, custom_data->item_base.type, "Item type is not custom data.");
    TEST_ASSERT_NULL_MESSAGE(custom_data->payload->buffer, "Custom data buffer is not NULL.");
    TEST_ASSERT_NULL_MESSAGE(custom_data->payload->content_type, "Custom data content type is not NULL.");
    TEST_ASSERT_NULL_MESSAGE(custom_data->payload->type, "Custom data type is not NULL.");
    TEST_ASSERT_NULL_MESSAGE(custom_data->payload->details, "Custom data details is not NULL.");
    TEST_ASSERT_EQUAL_MESSAGE(0, custom_data->payload->size, "Custom data size is not 0.");

    // Clean up.
    mcl_custom_data_destroy(&custom_data);
}

/**
 * GIVEN : mcl_custom_data_t parameter is given as MCL_NULL.
 * WHEN  : mcl_custom_data_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002(void)
{
    // Initialize variables.
    mcl_error_t code;
    char *version = "1.0";

    // Test.
    code = mcl_custom_data_initialize(version, MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_custom_data_initialize() unexpected return error!");
}

/**
 * GIVEN : version parameter is given as MCL_NULL.
 * WHEN  : mcl_custom_data_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_003(void)
{
    // Initialize variables.
    mcl_error_t code;
    mcl_custom_data_t *custom_data = MCL_NULL;

    // Test.
    code = mcl_custom_data_initialize(MCL_NULL, &custom_data);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_custom_data_initialize() unexpected return error!");
}

/**
 * GIVEN : mcl_custom_datat_t parameter is given as MCL_NULL.
 * WHEN  : mcl_custom_data_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_001(void)
{
    mcl_error_t code;
    const char *custom_data_content_type = "custom_payload_content_type";

    // Call mcl_custom_data_set_parameter with MCL_NULL custom_data parameter.
    code = mcl_custom_data_set_parameter(MCL_NULL, MCL_CUSTOM_DATA_PARAMETER_CONTENT_TYPE, custom_data_content_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_custom_data_set_parameter() unexpected return error!");
}

/**
 * GIVEN : Value parameter is given as MCL_NULL.
 * WHEN  : mcl_custom_data_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_002(void)
{
    mcl_error_t code;
    mcl_custom_data_t custom_data;

    // Call mcl_custom_data_set_parameter with MCL_NULL value parameter.
    code = mcl_custom_data_set_parameter(&custom_data, MCL_CUSTOM_DATA_PARAMETER_CONTENT_TYPE, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_custom_data_set_parameter() unexpected return error!");
}

/**
 * GIVEN : Initialized custom_data.
 * WHEN  : mcl_custom_data_set_parameter() is called.
 * THEN  : MCL_OK is returned and all fields are set.
 */
void test_set_parameter_003(void)
{
    mcl_error_t code;
    mcl_custom_data_t *custom_data = MCL_NULL;
    const char *version = "version 3.0.1";

    const char *custom_data_payload_type = "anyTypeSpecifiedByAgent";
    const char *custom_data_payload_content_type = "custom_payload_content_type1";
    const char *custom_data_payload_buffer = "buffer-1";
    mcl_size_t buffer_size = 8;
    mcl_json_t *custom_data_payload_details = MCL_NULL;
    const char *custom_data_payload_details_name = "Motor-1";
    const char *custom_data_payload_details_value = "Out of service";
    char *custom_data_payload_actual_details = MCL_NULL;

    // Initialize custom_data.
    code = mcl_custom_data_initialize(version, &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for custom_data.");

    // Check custom_data type parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_TYPE, custom_data_payload_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_custom_data_set_parameter() unexpected return error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(custom_data_payload_type, custom_data->payload->type, "Payload type parameter could not be set.");

    // Check custom_data content type parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_CONTENT_TYPE, custom_data_payload_content_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_custom_data_set_parameter() unexpected return error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(custom_data_payload_content_type, custom_data->payload->content_type, "Payload content type parameter could not be set.");

    // Check custom_data buffer parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_BUFFER, custom_data_payload_buffer);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_custom_data_set_parameter() unexpected return error!");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(custom_data_payload_buffer, custom_data->payload->buffer, "Payload buffer parameter could not be set.");

    // Check custom_data size parameter.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_BUFFER_SIZE, &buffer_size);
    TEST_ASSERT_EQUAL_INT_MESSAGE(buffer_size, custom_data->payload->size, "Payload size parameter could not be set.");

    // Check custom_data details parameter.
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, &custom_data_payload_details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for json.");

    code = mcl_json_util_add_string(custom_data_payload_details, custom_data_payload_details_name, custom_data_payload_details_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "String could not be added to the json object.");

    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_DETAILS, custom_data_payload_details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_custom_data_set_parameter() unexpected return error!");

    code = mcl_json_util_to_string(custom_data->payload->details, &custom_data_payload_actual_details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Json could not be converted to string.");

    TEST_ASSERT_EQUAL_STRING_MESSAGE("{\"Motor-1\":\"Out of service\"}", custom_data_payload_actual_details, "Payload details parameter could not be set.");

    mcl_custom_data_destroy(&custom_data);
    mcl_json_util_destroy(&custom_data_payload_details);
    MCL_FREE(custom_data_payload_actual_details);
}

/**
 * GIVEN : size parameter is not set.
 * WHEN  : custom_data_validate() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_validate_001(void)
{
    // Initialize variables.
    mcl_error_t code;
    mcl_custom_data_t *custom_data = MCL_NULL;
    mcl_uint8_t custom_data_buffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    // Initialize custom data.
    code = mcl_custom_data_initialize("1.0", &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for custom data.");

    // Set parameters.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_BUFFER, custom_data_buffer);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Buffer could not be set.");

    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_CONTENT_TYPE, "myContentType");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Content type could not be set.");

    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_TYPE, "myCustomDataType");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Type could not be set.");

    // Test.
    code = custom_data_validate(custom_data);
    
    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "This is not a valid custom data, MCL_FAIL should be returned.");

    // Clean up.
    mcl_custom_data_destroy(&custom_data);
}

/**
 * GIVEN : All mandatory fields are set.
 * WHEN  : custom_data_validate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_validate_002(void)
{
    // Initialize variables.
    mcl_error_t code;
    mcl_custom_data_t *custom_data = MCL_NULL;
    mcl_uint8_t custom_data_buffer[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mcl_size_t buffer_size = 8;

    // Initialize custom data.
    code = mcl_custom_data_initialize("1.0", &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for custom data.");

    // Set parameters.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_BUFFER, custom_data_buffer);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Buffer could not be set.");

    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_CONTENT_TYPE, "myContentType");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Content type could not be set.");

    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_TYPE, "myCustomDataType");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Type could not be set.");

    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_BUFFER_SIZE, &buffer_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Buffer size could not be set.");

    // Test.
    code = custom_data_validate(custom_data);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should be returned.");

    // Clean up.
    mcl_custom_data_destroy(&custom_data);
}
