/*!
 * @file     test_event.c
 * @brief    This file implements all test cases of mcl_event module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_connectivity/mcl_event.h"
#include "event.h"
#include "mock_mcl_random.h"
#include "mcl_core/mcl_json_util.h"
#include "mcl_core/mcl_memory.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Meta payload version is given as 1.0.
 * WHEN  : mcl_event_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized event data struct with meta payload version 1.0.
 */
void test_initialize_001(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_event_t *event = MCL_NULL;
    E_MCL_EVENT_VERSION version = MCL_EVENT_VERSION_1_0;

    // Initialize event.
    mcl_random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    code = mcl_event_initialize(version, &event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for event.");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((mcl_uint32_t)version, event->item_base.version, "Event meta payload version is wrong");
    TEST_ASSERT_NOT_NULL_MESSAGE(event, "Event is Null.");
    TEST_ASSERT_NOT_NULL_MESSAGE(event->payload, "Event payload is Null.");

    // Clean up.
    mcl_event_destroy(&event);
}

/**
 * GIVEN : mcl_event_t parameter is given as MCL_NULL.
 * WHEN  : mcl_event_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL must be returned.
 */
void test_initialize_002(void)
{
    // Variable declaration.
    mcl_error_t code;
    E_MCL_EVENT_VERSION version = MCL_EVENT_VERSION_1_0;

    // Try to initialize with MCL_NULL output parameter.
    code = mcl_event_initialize(version, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_event_initialize() unexpected return error!");
}

/**
 * GIVEN : Meta payload version is given as undefined E_MCL_EVENT_VERSION.
 * WHEN  : mcl_event_initialize() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_initialize_003(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_event_t *event = MCL_NULL;
    E_MCL_EVENT_VERSION version_invalid = MCL_EVENT_VERSION_END;

    // Try to initialize with invalid meta payload version parameter.
    code = mcl_event_initialize(version_invalid, &event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_event_initialize() unexpected return error!");
}

/**
 * GIVEN : mcl_event_t parameter is given as MCL_NULL.
 * WHEN  : mcl_event_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_001(void)
{
    // Variable declaration.
    mcl_error_t code;
    const char *event_payload_type = "thresholdViolation";

    // Call mcl_event_set_parameter with MCL_NULL event parameter.
    code = mcl_event_set_parameter(MCL_NULL, MCL_EVENT_PARAMETER_TYPE, event_payload_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_event_set_parameter() unexpected return error!");
}

/**
 * GIVEN : Value parameter is given as MCL_NULL.
 * WHEN  : mcl_event_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_set_parameter_002(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_event_t event;

    // Call mcl_event_set_parameter with MCL_NULL value parameter.
    code = mcl_event_set_parameter(&event, MCL_EVENT_PARAMETER_TYPE, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_event_set_parameter() unexpected return error!");
}

/**
 * GIVEN : Initialized event.
 * WHEN  : mcl_event_set_parameter() is called.
 * THEN  : MCL_OK is returned and all fields are set.
 */
void test_set_parameter_003(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_event_t *event = MCL_NULL;
    E_MCL_EVENT_VERSION version = MCL_EVENT_VERSION_1_0;
    const char *event_payload_type = "thresholdViolation";
    const char *event_payload_version = "1.0.1";
    E_MCL_EVENT_SEVERITY severity = MCL_EVENT_SEVERITY_ERROR;
    const char *event_payload_timestamp = "2018-04-26T08:06:25.317Z";
    const char *event_payload_description = "Best event ever";
    const char *event_payload_correlation_id = "6ba7b810-9dad-11d1-80b4-00c04fd430c8";
    mcl_json_t *event_payload_details = MCL_NULL;
    const char *event_payload_details_name = "message1";
    const char *event_payload_details_value = "Critical level";
    char *event_payload_actual_details = MCL_NULL;

    // Initialize event.
    mcl_random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    code = mcl_event_initialize(version, &event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for event.");

    // Check event type parameter.
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_TYPE, event_payload_type);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_event_set_parameter() unexpected return error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(event_payload_type, event->payload->type, "Payload type parameter could not be set.");

    // Check event version parameter.
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_VERSION, event_payload_version);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_event_set_parameter() unexpected return error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(event_payload_version, event->payload->version, "Payload version parameter could not be set.");

    // Check event severity parameter.
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_SEVERITY, &severity);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, event->payload->severity, "Payload severity parameter could not be set.");

    // Check event timestamp parameter.
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_TIMESTAMP_ISO8601, event_payload_timestamp);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_event_set_parameter() unexpected return error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(event_payload_timestamp, event->payload->timestamp, "Payload timestamp parameter could not be set.");

    // Check event description parameter.
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_DESCRIPTION, event_payload_description);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_event_set_parameter() unexpected return error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(event_payload_description, event->payload->description, "Payload description parameter could not be set.");

    // Check event correlation id parameter.
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_CORRELATION_ID, event_payload_correlation_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_event_set_parameter() unexpected return error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(event_payload_correlation_id, event->payload->correlation_id, "Payload correlation id parameter could not be set.");

    // Check event details parameter.
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, &event_payload_details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for json.");

    // Add event payload details.
    code = mcl_json_util_add_string(event_payload_details, event_payload_details_name, event_payload_details_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "String could not be added to the json object.");

    // Set event payload details.
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_DETAILS, event_payload_details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_event_set_parameter() unexpected return error!");

    // Convert event payload details to string.
    code = mcl_json_util_to_string(event->payload->details, &event_payload_actual_details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Json could not be converted to string.");

    // Check event payload details.
    TEST_ASSERT_EQUAL_STRING_MESSAGE("{\"message1\":\"Critical level\"}", event_payload_actual_details, "Payload details parameter could not be set.");

    // Clean up.
    mcl_event_destroy(&event);
    mcl_json_util_destroy(&event_payload_details);
    MCL_FREE(event_payload_actual_details);
}

/**
 * GIVEN : Initialized event and undefined E_MCL_EVENT_PARAMETER parameter are given.
 * WHEN  : mcl_event_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_004(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_event_t *event = MCL_NULL;
    E_MCL_EVENT_VERSION version = MCL_EVENT_VERSION_1_0;
    E_MCL_EVENT_PARAMETER parameter = MCL_EVENT_PARAMETER_END;
    const char *dummy_string = "dummy";

    // Initialize event.
    mcl_random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    code = mcl_event_initialize(version, &event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for event.");

    // Try to set parameter with invalid event parameter.
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_END, dummy_string);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_event_set_option() unexpected return error!");

    // Clean up.
    mcl_event_destroy(&event);
}

/**
 * GIVEN : Initialized event and invalid timestamp value are given.
 * WHEN  : mcl_event_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_005(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_event_t *event = MCL_NULL;
    E_MCL_EVENT_VERSION version = MCL_EVENT_VERSION_1_0;
    const char *event_payload_invalid_timestamp = "123456789";

    // Initialize event.
    mcl_random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    code = mcl_event_initialize(version, &event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for event.");

    // Try to set parameter with invalid timestamp value.
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_TIMESTAMP_ISO8601, event_payload_invalid_timestamp);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_event_set_option() unexpected return error!");

    // Clean up.
    mcl_event_destroy(&event);
}

/**
 * GIVEN : Initialized event and invalid severity value are given.
 * WHEN  : mcl_event_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_006(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_event_t *event = MCL_NULL;
    E_MCL_EVENT_VERSION version = MCL_EVENT_VERSION_1_0;
    E_MCL_EVENT_SEVERITY severity = MCL_EVENT_SEVERITY_END;

    // Initialize event.
    mcl_random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    code = mcl_event_initialize(version, &event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for event.");

    // Try to set parameter with invalid severity value.
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_SEVERITY, &severity);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_event_set_option() unexpected return error!");

    // Clean up.
    mcl_event_destroy(&event);
}

/**
 * GIVEN : Initialized event and description value that longer than 255 characters are given.
 * WHEN  : mcl_event_set_parameter() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_007(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_event_t *event = MCL_NULL;
    E_MCL_EVENT_VERSION version = MCL_EVENT_VERSION_1_0;
    const char *long_event_payload_description = "very_very_long_event_payload_description_even_longer_than_two_hundreds_and_fifty_five_characters_count_if_you_do_not_believe_in_me_you_can_count_ok_than_lets_start_1_2_3_4_5_6_7_8_9_10_11_12_13_14_15_16_17_18_19_20_21_22_23_24_25_26_27_28_29_30_31_32_33_34";

    // Initialize event.
    mcl_random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    code = mcl_event_initialize(version, &event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for event.");

    // Try to set parameter with long description value (256 characters).
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_DESCRIPTION, long_event_payload_description);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_event_set_option() unexpected return error!");

    // Clean up.
    mcl_event_destroy(&event);
}

/**
 * GIVEN : Initialized event and timestamp parameter as time_t.
 * WHEN  : mcl_event_set_parameter() is called.
 * THEN  : MCL_OK is returned.
 */
void test_set_parameter_008(void)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_event_t *event = MCL_NULL;
    E_MCL_EVENT_VERSION version = MCL_EVENT_VERSION_1_0;
  
    // Initialize event.
    mcl_random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    code = mcl_event_initialize(version, &event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for event.");

    // Epoch for 2019-03-06T08:00:00.000Z.
    time_t test_time = 1551859200;

    // Try to set timestamp parameter .
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_TIMESTAMP_EPOCH, &test_time);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_event_set_parameter() should have returned MCL_OK.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2019-03-06T08:00:00.000Z", event->payload->timestamp, "Timestamp is wrong!");

    // Clean up.
    mcl_event_destroy(&event);
}
