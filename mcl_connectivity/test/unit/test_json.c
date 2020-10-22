/*!
 * @file     test_json.c
 * @brief    This file implements test cases of json module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_connectivity/mcl_event.h"
#include "event.h"
#include "mcl_connectivity/mcl_data_source_configuration.h"
#include "mcl_connectivity/mcl_data_source.h"
#include "mcl_connectivity/mcl_data_point.h"
#include "data_source_configuration.h"
#include "data_source.h"
#include "data_point.h"
#include "timeseries.h"
#include "timeseries_value.h"
#include "timeseries_value_list.h"
#include "mcl_connectivity/mcl_timeseries.h"
#include "mcl_connectivity/mcl_timeseries_value.h"
#include "mcl_connectivity/mcl_timeseries_value_list.h"
#include "mcl_connectivity/mcl_file.h"
#include "mcl_connectivity/mcl_custom_data.h"
#include "file.h"
#include "custom_data.h"
#include "json.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"
#include "mock_mcl_random.h"
#include <string.h>

void setUp(void)
{

}

void tearDown(void)
{

}

/**
 * GIVEN : Initialized event.
 * WHEN  : json_from_item_meta is called with the event.
 * THEN  : It returns the json string for the meta part of event.
 */
void test_json_from_item_meta_001(void)
{
    event_t *event;
    char *json_string;
    mcl_error_t code;
    char *guid = MCL_MALLOC(37);
    mcl_random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_random_generate_guid_ReturnThruPtr_guid(&guid);

    code = mcl_event_initialize(MCL_EVENT_VERSION_1_0, &event);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = json_from_item_meta(event, &json_string);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    char *expected_json_string = "{\"type\":\"item\",\"version\":\"1.0\",\"payload\":{\"type\":\"businessEvent\",\"version\":\"1.0\"}}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string, "json string fail");

    MCL_FREE(json_string);
    mcl_event_destroy(&event);
}

/**
* GIVEN : Initialized data source configuration.
* WHEN  : json_from_item_meta is called with the data source configuration.
* THEN  : It returns the json string for the meta part of data source configuration.
*/
void test_json_from_item_meta_002(void)
{
    data_source_configuration_t *data_source_configuration;
    char *json_string;
    mcl_error_t code;

    // Initialize data source configuration.
    code = mcl_data_source_configuration_initialize(MCL_DATA_SOURCE_CONFIGURATION_1_0, &data_source_configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Data source configuration cannot be initialized.");

    // Test.
    code = json_from_item_meta(data_source_configuration, &json_string);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "json_from_item_meta did not return MCL_OK.");

    // Check result.
    char *expected_json_string = "{\"type\":\"item\",\"version\":\"1.0\",\"payload\":{\"type\":\"dataSourceConfiguration\",\"version\":\"1.0\"}}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string, "json string fail");

    // Clean up.
    MCL_FREE(json_string);
    mcl_data_source_configuration_destroy(&data_source_configuration);
}

/**
 * GIVEN : Initialized timeseries.
 * WHEN  : json_from_item_meta is called with the timeseries.
 * THEN  : It returns the json string for the meta part of the timeseries.
 */
void test_json_from_item_meta_003(void)
{
    timeseries_t *timeseries;
    char *json_string;
    mcl_error_t code;

    // Initialize timeseries.
    code = mcl_timeseries_initialize(MCL_TIMESERIES_VERSION_1_0, &timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Timeseries cannot be initialized.");

    // Set configuration id.
    code = mcl_timeseries_set_parameter(timeseries, MCL_TIMESERIES_PARAMETER_CONFIGURATION_ID, "Configuration ID");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Configuration ID cannot be set.");

    // Test.
    code = json_from_item_meta(timeseries, &json_string);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "json_from_item_meta did not return MCL_OK.");

    // Check result.
    char *expected_json_string = "{\"type\":\"item\",\"version\":\"1.0\",\"payload\":{\"type\":\"standardTimeSeries\",\"version\":\"1.0\",\"details\":{\"configurationId\":\"Configuration ID\"}}}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string, "json string fail");

    // Clean up.
    MCL_FREE(json_string);
    mcl_timeseries_destroy(&timeseries);
}

/**
 * GIVEN : Initialized file.
 * WHEN  : json_from_item_meta is called with the file.
 * THEN  : It returns the json string for the meta part of the file.
 */
void test_json_from_item_meta_004(void)
{
    char *json_string;
    mcl_error_t code;

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
    code = json_from_item_meta(&file, &json_string);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "json_from_item_meta did not return MCL_OK.");

    // Check result.
    char *expected_json_string = "{\"type\":\"item\",\"version\":\"1.0\",\"payload\":{\"type\":\"file\",\"version\":\"1.0\",\"details\":{\"fileName\":\"remote_name\",\"creationDate\":\"2018-10-02T08:00:00.000Z\",\"fileType\":\"file_type\"}}}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string, "json string fail");

    // Clean up.
    MCL_FREE(json_string);
}

/**
 * GIVEN : Initialized custom data.
 * WHEN  : json_from_item_meta is called with the custom data.
 * THEN  : It returns the json string for the meta part of the custom data.
 */
void test_json_from_item_meta_005(void)
{
    char *json_string;
    mcl_error_t code;
    custom_data_t *custom_data;
    mcl_uint8_t custom_data_buffer[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mcl_size_t buffer_size = 8;

    mcl_json_t *custom_data_payload_details = MCL_NULL;
    const char *custom_data_payload_details_name = "color";
    const char *custom_data_payload_details_value = "white";

    // Check custom_data details parameter.
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, &custom_data_payload_details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for json.");

    code = mcl_json_util_add_string(custom_data_payload_details, custom_data_payload_details_name, custom_data_payload_details_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "String could not be added to the json object.");
	
    // Set version.
    code = mcl_custom_data_initialize("3.0.1", &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for custom data.");

    // Set type.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_TYPE, "anyTypeSpecifiedByAgent");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Type cannot be set.");

    // Set buffer.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_BUFFER, custom_data_buffer);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Buffer cannot be set.");

    // Set buffer size.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_BUFFER_SIZE, &buffer_size);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Buffer size cannot be set.");

    // Set content type.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_CONTENT_TYPE, "content_type_1");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Content type cannot be set.");

    // Set details.
    code = mcl_custom_data_set_parameter(custom_data, MCL_CUSTOM_DATA_PARAMETER_DETAILS, custom_data_payload_details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Details cannot be set.");

    // Test.
    code = json_from_item_meta(custom_data, &json_string);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "json_from_item_meta did not return MCL_OK.");

    // Check result.
    char *expected_json_string = "{\"type\":\"item\",\"version\":\"1.0\",\"payload\":{\"type\":\"anyTypeSpecifiedByAgent\",\"version\":\"3.0.1\",\"details\":{\"color\":\"white\"}}}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string, "json string fail");

    // Clean up.
    MCL_FREE(json_string);
    mcl_json_util_destroy(&custom_data_payload_details);
    mcl_custom_data_destroy(&custom_data);
}

/**
 * GIVEN : Initialized event.
 * WHEN  : json_from_item_payload is called with the event whose mandatory fields are set.
 * THEN  : It returns the json string for the payload part of event.
 */
void test_json_from_item_payload_001(void)
{
    event_t *event;
    char *json_string;
    mcl_error_t code;
    char *guid = MCL_MALLOC(37);
    memset(guid, '0', 36);
    guid[36] = '\0';

    mcl_random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_random_generate_guid_ReturnThruPtr_guid(&guid);

    code = mcl_event_initialize(MCL_EVENT_VERSION_1_0, &event);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_TIMESTAMP_ISO8601, "2018-07-04T12:34:56.000Z");
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_TYPE, "myEventType");
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_VERSION, "1.0");
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = json_from_item_payload(event, &json_string);

    char *expected_json_string = "[{\"id\":\"000000000000000000000000000000000000\",\"timestamp\":\"2018-07-04T12:34:56.000Z\",\"severity\":3,\"type\":\"myEventType\",\"version\":\"1.0\",\"details\":{}}]";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string, "json string fail");

    MCL_FREE(json_string);
    mcl_event_destroy(&event);
}

/**
 * GIVEN : Initialized data source configuration.
 * WHEN  : json_from_item_payload is called with the data source configuration whose all fields are set.
 * THEN  : It returns the json string for the payload part of data source configuration.
 */
void test_json_from_item_payload_002(void)
{
#define DATA_POINT_COUNT 3

    mcl_error_t code;
    mcl_size_t index;
    char *json_string;

    data_source_configuration_t *dsc;
    data_source_t *data_source;
    data_point_t *data_points_array[DATA_POINT_COUNT];
    char dummy_string[] = "AA";

    // Initialize data source configuration, and set parameter.
    mcl_data_source_configuration_initialize(MCL_DATA_SOURCE_CONFIGURATION_1_0, &dsc);
    mcl_data_source_configuration_set_parameter(dsc, MCL_DATA_SOURCE_CONFIGURATION_PARAMETER_ID, "dsc id");

    // Initialize data source, and set parameters.
    mcl_data_source_initialize(&data_source);
    mcl_data_source_set_parameter(data_source, MCL_DATA_SOURCE_PARAMETER_NAME, "source name");
    mcl_data_source_set_parameter(data_source, MCL_DATA_SOURCE_PARAMETER_DESCRIPTION, "source description");

    for (index = 0; index < DATA_POINT_COUNT; index++)
    {
        // Initialize custom data json for data point.
        mcl_json_t *custom_data = MCL_NULL;
        mcl_json_util_initialize(MCL_JSON_OBJECT, &custom_data);

        // Initialize data point.
        dummy_string[1] = 'A';
        mcl_data_point_initialize(&data_points_array[index]);

        // Set ID.
        mcl_data_point_set_parameter(data_points_array[index], MCL_DATA_POINT_PARAMETER_ID, dummy_string);

        // Change dummy_string eg. AA to AB, and set name.
        dummy_string[1]++;
        mcl_data_point_set_parameter(data_points_array[index], MCL_DATA_POINT_PARAMETER_NAME, dummy_string);

        // Set description.
        dummy_string[1]++;
        mcl_data_point_set_parameter(data_points_array[index], MCL_DATA_POINT_PARAMETER_DESCRIPTION, dummy_string);

        // Set type.
        dummy_string[1]++;
        mcl_data_point_set_parameter(data_points_array[index], MCL_DATA_POINT_PARAMETER_TYPE, dummy_string);

        // Set unit.
        dummy_string[1]++;
        mcl_data_point_set_parameter(data_points_array[index], MCL_DATA_POINT_PARAMETER_UNIT, dummy_string);

        // Set custom data.
        dummy_string[1]++;
        mcl_json_util_add_string(custom_data, "customField", dummy_string);
        mcl_data_point_set_parameter(data_points_array[index], MCL_DATA_POINT_PARAMETER_CUSTOM_DATA, custom_data);

        // Add data point to data source.
        mcl_data_source_add_data_point(data_source, data_points_array[index]);

        dummy_string[0]++;    
        mcl_json_util_destroy(&custom_data);
    }

    // Add data source to data source configuration.
    mcl_data_source_configuration_add_data_source(dsc, data_source);

    // Test.
    code = json_from_item_payload(dsc, &json_string);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "json_from_item_payload did not return MCL_OK.");

    char *expected_json_string = "{" \
        "\"configurationId\":\"dsc id\"," \
        "\"dataSources\":" \
        "[" \
        "{" \
        "\"name\":\"source name\"," \
        "\"description\":\"source description\"," \
        "\"dataPoints\":[{" \
        "\"id\":\"AA\"," \
        "\"name\":\"AB\"," \
        "\"description\":\"AC\"," \
        "\"type\":\"AD\"," \
        "\"unit\":\"AE\"," \
        "\"customData\":{\"customField\":\"AF\"}" \
        "}," \
        "{" \
        "\"id\":\"BA\"," \
        "\"name\":\"BB\"," \
        "\"description\":\"BC\"," \
        "\"type\":\"BD\"," \
        "\"unit\":\"BE\"," \
        "\"customData\":{\"customField\":\"BF\"}" \
        "}," \
        "{" \
        "\"id\":\"CA\"," \
        "\"name\":\"CB\"," \
        "\"description\":\"CC\"," \
        "\"type\":\"CD\"," \
        "\"unit\":\"CE\"," \
        "\"customData\":{\"customField\":\"CF\"}" \
        "}" \
        "]" \
        "}" \
        "]" \
        "}";

    // Check result.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string, "json string fail");

    // Clean up.
    MCL_FREE(json_string);
    mcl_data_source_configuration_destroy(&dsc);

#undef DATA_POINT_COUNT
}

/**
 * GIVEN : Initialized timeseries.
 * WHEN  : json_from_item_payload is called with the timeseries which has 3 value lists.
 * THEN  : It returns the json string for the payload part of the timeseries.
 */
void test_json_from_item_payload_003(void)
{
    mcl_error_t code;
    char *json_string;
    timeseries_t *timeseries;

    // Initialize a dummy string to set fields.
    char dummy_string[] = "AA";

    // Initialize a timestamp.
    char timestamp[] = "2018-09-17T11:11:11.000Z";

    // Initialize a timeseries.
    mcl_timeseries_initialize(MCL_TIMESERIES_VERSION_1_0, &timeseries);

    // Add 3 value lists in a loop.
    for (mcl_size_t i = 0; i < 3; i++)
    {
        // Initialize timeseries value list.
        mcl_timeseries_value_list_t *value_list;
        mcl_timeseries_value_list_initialize(&value_list);
        dummy_string[1] = 'A';

        // Set timestamp of timeseries value list.   
        mcl_timeseries_value_list_set_parameter(value_list, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, timestamp);

        // Increase timestamp 100 milliseconds.
        timestamp[20]++;

        // Add 2 values to each value list.
        for (mcl_size_t j = 0; j < 2; j++)
        {
            // Initialize timeseries value.
            mcl_timeseries_value_t *value;
            mcl_timeseries_value_initialize(&value);
        
            // Set data point id of timeseries value.
            mcl_timeseries_value_set_parameter(value, MCL_TIMESERIES_VALUE_PARAMETER_DATA_POINT_ID, dummy_string);
            dummy_string[1]++;
            
            // Set value of timeseries value.
            mcl_timeseries_value_set_parameter(value, MCL_TIMESERIES_VALUE_PARAMETER_VALUE, dummy_string);
            dummy_string[1]++;

            // Set quality code of timeseries value.
            mcl_timeseries_value_set_parameter(value, MCL_TIMESERIES_VALUE_PARAMETER_QUALITY_CODE, dummy_string);
            dummy_string[1]++;

            // Add timeseries value to timeseries value list.
            mcl_timeseries_value_list_add_value(value_list, value);
        }

        // Add timeseries value list to timeseries.
        mcl_timeseries_add_value_list(timeseries, value_list);
        dummy_string[0]++;
    }

    // Test.
    code = json_from_item_payload(timeseries, &json_string);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "json_from_item_payload did not return MCL_OK.");

    char *expected_json_string = "[" \
        "{\"timestamp\":\"2018-09-17T11:11:11.000Z\",\"values\":" \
        "[" \
        "{\"dataPointId\":\"AA\",\"value\":\"AB\",\"qualityCode\":\"AC\"}," \
        "{\"dataPointId\":\"AD\",\"value\":\"AE\",\"qualityCode\":\"AF\"}" \
        "]" \
        "}," \
        "{\"timestamp\":\"2018-09-17T11:11:11.100Z\",\"values\":" \
        "[" \
        "{\"dataPointId\":\"BA\",\"value\":\"BB\",\"qualityCode\":\"BC\"}," \
        "{\"dataPointId\":\"BD\",\"value\":\"BE\",\"qualityCode\":\"BF\"}" \
        "]" \
        "}," \
        "{\"timestamp\":\"2018-09-17T11:11:11.200Z\",\"values\":" \
        "[" \
        "{\"dataPointId\":\"CA\",\"value\":\"CB\",\"qualityCode\":\"CC\"}," \
        "{\"dataPointId\":\"CD\",\"value\":\"CE\",\"qualityCode\":\"CF\"}" \
        "]" \
        "}" \
        "]";

    // Check result.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_string, json_string, "json string fail");

    // Clean up.
    MCL_FREE(json_string);
    mcl_timeseries_destroy(&timeseries);
}

/**
 * GIVEN : Initialized event version 1.0.
 * WHEN  : json_get_item_size is called with the event.
 * THEN  : Item size of the event is returned.
 */
void test_json_get_item_size_001(void)
{
    event_t *event;
    char *item_meta;
    char *item_payload;
    mcl_error_t code;
    mcl_size_t calculated_item_size;
    mcl_size_t item_meta_size;
    mcl_size_t item_payload_size;

    char *guid = MCL_MALLOC(37);
    memset(guid, '0', 36);
    guid[36] = '\0';

    mcl_random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_random_generate_guid_ReturnThruPtr_guid(&guid);

    code = mcl_event_initialize(MCL_EVENT_VERSION_1_0, &event);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_TIMESTAMP_ISO8601, "2018-07-04T12:34:56.000Z");
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_TYPE, "myEventType");
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_VERSION, "1.0");
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = json_from_item_meta(event, &item_meta);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = json_from_item_payload(event, &item_payload);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Get size of item meta, item payload.
    item_meta_size = mcl_string_util_strlen(item_meta);
    item_payload_size = mcl_string_util_strlen(item_payload);

    // Free item meta and item payload.
    MCL_FREE(item_meta);
    MCL_FREE(item_payload);

    // Get size of item and check if it is correct.
    calculated_item_size = json_get_item_size(event);
    TEST_ASSERT_EQUAL_MESSAGE(item_meta_size + item_payload_size, calculated_item_size, "Item size for event is not calculated correctly!");

    // Set correlation id.
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_CORRELATION_ID, "0123456789");
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Get payload.
    code = json_from_item_payload(event, &item_payload);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Get item payload size.
    item_payload_size = mcl_string_util_strlen(item_payload);

    // Free item payload.
    MCL_FREE(item_payload);

    // Get size of item and check if it is correct.
    calculated_item_size = json_get_item_size(event);
    TEST_ASSERT_EQUAL_MESSAGE(item_meta_size + item_payload_size, calculated_item_size, "Item size for event is not calculated correctly.");

    // Set correlation id.
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_DESCRIPTION, "myDescription");
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Get payload.
    code = json_from_item_payload(event, &item_payload);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Get item payload size.
    item_payload_size = mcl_string_util_strlen(item_payload);

    // Free item payload.
    MCL_FREE(item_payload);

    // Get size of item and check if it is correct.
    calculated_item_size = json_get_item_size(event);
    TEST_ASSERT_EQUAL_MESSAGE(item_meta_size + item_payload_size, calculated_item_size, "Item size for event is not calculated correctly!");

    // Destroy event.
    mcl_event_destroy(&event);
}

/**
 * GIVEN : Initialized event version 2.0.
 * WHEN  : json_get_item_size is called with the event.
 * THEN  : Item size of the event is returned.
 */
void test_json_get_item_size_002(void)
{
    event_t *event;
    char *item_meta;
    char *item_payload;
    mcl_error_t code;
    mcl_size_t calculated_item_size;
    mcl_size_t item_meta_size;
    mcl_size_t item_payload_size;

    char *guid = MCL_MALLOC(37);
    memset(guid, '0', 36);
    guid[36] = '\0';

    mcl_random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_random_generate_guid_ReturnThruPtr_guid(&guid);

    code = mcl_event_initialize(MCL_EVENT_VERSION_2_0, &event);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_TIMESTAMP_ISO8601, "2018-07-04T12:34:56.000Z");
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_TYPE, "myEventType");
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_VERSION, "2.0");
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = json_from_item_meta(event, &item_meta);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = json_from_item_payload(event, &item_payload);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Get size of item meta, item payload.
    item_meta_size = mcl_string_util_strlen(item_meta);
    item_payload_size = mcl_string_util_strlen(item_payload);

    // Free item meta and item payload.
    MCL_FREE(item_meta);
    MCL_FREE(item_payload);

    // Get size of item and check if it is correct.
    calculated_item_size = json_get_item_size(event);
    TEST_ASSERT_EQUAL_MESSAGE(item_meta_size + item_payload_size, calculated_item_size, "Item size for event is not calculated correctly!");

    // Set correlation id.
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_CORRELATION_ID, "0123456789");
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Get payload.
    code = json_from_item_payload(event, &item_payload);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Get item payload size.
    item_payload_size = mcl_string_util_strlen(item_payload);

    // Free item payload.
    MCL_FREE(item_payload);

    // Get size of item and check if it is correct.
    calculated_item_size = json_get_item_size(event);
    TEST_ASSERT_EQUAL_MESSAGE(item_meta_size + item_payload_size, calculated_item_size, "Item size for event is not calculated correctly.");

    // Set correlation id.
    code = mcl_event_set_parameter(event, MCL_EVENT_PARAMETER_DESCRIPTION, "myDescription");
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Get payload.
    code = json_from_item_payload(event, &item_payload);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Get item payload size.
    item_payload_size = mcl_string_util_strlen(item_payload);

    // Free item payload.
    MCL_FREE(item_payload);

    // Get size of item and check if it is correct.
    calculated_item_size = json_get_item_size(event);
    TEST_ASSERT_EQUAL_MESSAGE(item_meta_size + item_payload_size, calculated_item_size, "Item size for event is not calculated correctly!");

    // Destroy event.
    mcl_event_destroy(&event);
}

/**
 * GIVEN : Initialized data source configuration.
 * WHEN  : json_get_item_size is called with the data source configuration.
 * THEN  : Item size of the data source configuration is returned.
 */
void test_json_get_item_size_003(void)
{
    mcl_error_t code;
    mcl_size_t index;
    char *item_meta;
    char *item_payload;
    mcl_size_t item_meta_size;
    mcl_size_t item_payload_size;
    mcl_size_t calculated_size;

    data_source_configuration_t *dsc;
    data_source_t *data_source;
    char dummy_string[] = "AA";

    // Initialize data source configuration, and set parameter.
    mcl_data_source_configuration_initialize(MCL_DATA_SOURCE_CONFIGURATION_1_0, &dsc);
    mcl_data_source_configuration_set_parameter(dsc, MCL_DATA_SOURCE_CONFIGURATION_PARAMETER_ID, "dsc id");

    // Initialize data source, and set parameters.
    mcl_data_source_initialize(&data_source);
    mcl_data_source_set_parameter(data_source, MCL_DATA_SOURCE_PARAMETER_NAME, "source name");
    mcl_data_source_set_parameter(data_source, MCL_DATA_SOURCE_PARAMETER_DESCRIPTION, "source description");

    for (index = 0; index < 3; index++)
    {
        // Initialize custom data json for data point.
        mcl_data_point_t *data_point = MCL_NULL;
        mcl_json_t *custom_data = MCL_NULL;
        mcl_json_util_initialize(MCL_JSON_OBJECT, &custom_data);

        // Initialize data point.
        dummy_string[1] = 'A';
        mcl_data_point_initialize(&data_point);

        // Set ID.
        mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_ID, dummy_string);

        // Change dummy_string eg. AA to AB, and set name.
        dummy_string[1]++;
        mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_NAME, dummy_string);

        // Set description.
        dummy_string[1]++;
        mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_DESCRIPTION, dummy_string);

        // Set type.
        dummy_string[1]++;
        mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_TYPE, dummy_string);

        // Set unit.
        dummy_string[1]++;
        mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_UNIT, dummy_string);

        // Set custom data.
        dummy_string[1]++;
        mcl_json_util_add_string(custom_data, "customField", dummy_string);
        mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_CUSTOM_DATA, custom_data);

        // Add data point to data source.
        mcl_data_source_add_data_point(data_source, data_point);

        dummy_string[0]++;
        mcl_json_util_destroy(&custom_data);
    }

    // Add data source to data source configuration.
    mcl_data_source_configuration_add_data_source(dsc, data_source);

    // Get item meta.
    code = json_from_item_meta(dsc, &item_meta);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Item meta cannot be created.");

    // Get item meta size.
    item_meta_size = mcl_string_util_strlen(item_meta);

    // Get item payload.
    code = json_from_item_payload(dsc, &item_payload);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Item payload cannot be created.");

    // Get item payload size.
    item_payload_size = mcl_string_util_strlen(item_payload);

    // Get calculated item size and check.
    calculated_size = json_get_item_size(dsc);
    TEST_ASSERT_EQUAL_MESSAGE(item_meta_size + item_payload_size, calculated_size, "Item size is not correct.");

    // Clean up.
    MCL_FREE(item_meta);
    MCL_FREE(item_payload);
    mcl_data_source_configuration_destroy(&dsc);
}

/**
 * GIVEN : Initialized timeseries.
 * WHEN  : json_get_item_size is called with the timeseries.
 * THEN  : Item size of the timeseries is returned.
 */
void test_json_get_item_size_004(void)
{
    mcl_error_t code;
    char *item_meta;
    char *item_payload;
    mcl_size_t item_meta_size;
    mcl_size_t item_payload_size;
    mcl_size_t calculated_size;
    timeseries_t *timeseries;

    // Initialize a dummy string to set fields.
    char dummy_string[] = "AA";

    // Initialize a timestamp.
    char timestamp[] = "2018-09-17T11:11:11.000Z";

    // Initialize a timeseries.
    mcl_timeseries_initialize(MCL_TIMESERIES_VERSION_1_0, &timeseries);

    // Set configuration id.
    code = mcl_timeseries_set_parameter(timeseries, MCL_TIMESERIES_PARAMETER_CONFIGURATION_ID, "Configuration ID");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Configuration ID cannot be set.");

    // Add 3 value lists in a loop.
    for (mcl_size_t i = 0; i < 3; i++)
    {
        // Initialize timeseries value list.
        mcl_timeseries_value_list_t *value_list;
        mcl_timeseries_value_list_initialize(&value_list);
        dummy_string[1] = 'A';

        // Set timestamp of timeseries value list.   
        mcl_timeseries_value_list_set_parameter(value_list, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, timestamp);

        // Increase timestamp 100 milliseconds.
        timestamp[20]++;

        // Add 2 values to each value list.
        for (mcl_size_t j = 0; j < 2; j++)
        {
            // Initialize timeseries value.
            mcl_timeseries_value_t *value;
            mcl_timeseries_value_initialize(&value);

            // Set data point id of timeseries value.
            mcl_timeseries_value_set_parameter(value, MCL_TIMESERIES_VALUE_PARAMETER_DATA_POINT_ID, dummy_string);
            dummy_string[1]++;

            // Set value of timeseries value.
            mcl_timeseries_value_set_parameter(value, MCL_TIMESERIES_VALUE_PARAMETER_VALUE, dummy_string);
            dummy_string[1]++;

            // Set quality code of timeseries value.
            mcl_timeseries_value_set_parameter(value, MCL_TIMESERIES_VALUE_PARAMETER_QUALITY_CODE, dummy_string);
            dummy_string[1]++;

            // Add timeseries value to timeseries value list.
            mcl_timeseries_value_list_add_value(value_list, value);
        }

        // Add timeseries value list to timeseries.
        mcl_timeseries_add_value_list(timeseries, value_list);
        dummy_string[0]++;
    }

    // Get item meta.
    code = json_from_item_meta(timeseries, &item_meta);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Item meta cannot be created.");

    // Get item meta size.
    item_meta_size = mcl_string_util_strlen(item_meta);

    // Get item payload.
    code = json_from_item_payload(timeseries, &item_payload);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Item payload cannot be created.");

    // Get item payload size.
    item_payload_size = mcl_string_util_strlen(item_payload);

    // Get calculated item size and check.
    calculated_size = json_get_item_size(timeseries);
    TEST_ASSERT_EQUAL_MESSAGE(item_meta_size + item_payload_size, calculated_size, "Item size is not correct.");

    // Clean up.
    MCL_FREE(item_meta);
    MCL_FREE(item_payload);
    mcl_timeseries_destroy(&timeseries);
}

/**
 * GIVEN : Initialized file.
 * WHEN  : json_get_item_size is called with the file.
 * THEN  : Item size of the file is returned.
 */
void test_json_get_item_size_005(void)
{
    mcl_error_t code;
    char *item_meta;
    mcl_size_t item_meta_size;
    mcl_size_t calculated_size;

    // Assign a dummy file size.
    mcl_size_t item_payload_size = 123;
    
    file_payload_t file_payload =
    {
        .creation_date = "2018-10-02T08:00:00.000Z",
        .local_path = "local_path",
        .remote_name = "remote_name",
        .type = "file_type",
        .size = item_payload_size,
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

    // Get item meta.
    code = json_from_item_meta(&file, &item_meta);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Item meta cannot be created.");

    // Get item meta size.
    item_meta_size = mcl_string_util_strlen(item_meta);

    // Get calculated item size and check.
    calculated_size = json_get_item_size(&file);
    TEST_ASSERT_EQUAL_MESSAGE(item_meta_size + item_payload_size, calculated_size, "Item size is not correct.");

    // Clean up.
    MCL_FREE(item_meta);
}

/**
 * GIVEN : Initialized custom data.
 * WHEN  : json_get_item_size is called with the custom data.
 * THEN  : Item size of the custom data is returned.
 */
void test_json_get_item_size_006(void)
{
    mcl_error_t code;
    custom_data_t custom_data;
    custom_data_payload_t custom_data_payload;
    char *item_meta = MCL_NULL;
    mcl_size_t item_meta_size;
    mcl_size_t calculated_size;

    // Assign a dummy custom data size.
    mcl_size_t item_payload_size = 234;

    // Set custom data payload parameters.
    custom_data_payload.content_type = "customContentType";
    custom_data_payload.type = "customType";
    custom_data_payload.version = "3.4";
    custom_data_payload.details = MCL_NULL;
    custom_data_payload.size = item_payload_size;

    // Set custom data parameters.
    custom_data.payload = &custom_data_payload;
    custom_data.item_base.preamble = MCL_ITEM_PREAMBLE;
    custom_data.item_base.type = MCL_ITEM_TYPE_CUSTOM_DATA;

    // Get item meta.
    code = json_from_item_meta(&custom_data, &item_meta);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Item meta cannot be created.");

    // Get item meta size.
    item_meta_size = mcl_string_util_strlen(item_meta);

    // Get calculated item size and check.
    calculated_size = json_get_item_size(&custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(item_meta_size + item_payload_size, calculated_size, "Item size is not correct.");

    // Clean up.
    MCL_FREE(item_meta);
}

/**
 * GIVEN : Data source configuration string.
 * WHEN  : json_parse_item is called with the string.
 * THEN  : Data source configuration is parsed and returned.
 */
void test_json_parse_item_001(void)
{
    mcl_error_t code;
    data_source_configuration_t *dsc;
    data_point_t *data_point;

    const char dsc_string[] = "{"
        "  \"id\": \"c3b7d31d-e966-46e6-9db1-d4b3e8c90d7b\","
        "  \"eTag\": \"2\","
        "  \"configurationId\": \"Configuration01\","
        "  \"dataSources\": ["
        "    {"
        "      \"name\": \"OPC-UA Server\","
        "      \"description\": \"OPC-UA Server installed on site.\","
        "      \"dataPoints\": ["
        "        {"
        "          \"id\": \"DP001\","
        "          \"name\": \"Voltage\","
        "          \"description\": \"Voltage value read.\","
        "          \"type\": \"DOUBLE\","
        "          \"unit\": \"V\","
        "          \"customData\": {"
        "            \"Nominal\": \"~220 Volts\""
        "          }"
        "        }"
        "      ],"
        "      \"customData\": {"
        "        \"Host\": \"192.168.0.111\","
        "        \"Port\": \"8765\""
        "      }"
        "    }"
        "  ]"
        "}";

    // Test.
    code = json_parse_item(dsc_string, sizeof(dsc_string) - MCL_NULL_CHAR_SIZE, (void **) &dsc);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK should have been returned.");

    // Check data source configuration.
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Configuration01", dsc->payload->configuration_id, "Configuration id is not correct.");

    // Check data source.
    TEST_ASSERT_EQUAL_STRING_MESSAGE("OPC-UA Server", ((data_source_t *) dsc->payload->data_sources->head->data)->name, "Data source name is not correct.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("OPC-UA Server installed on site.", ((data_source_t *) dsc->payload->data_sources->head->data)->description, "Data source description is not correct.");

    // Check data point.
    data_point = (data_point_t *) ((data_source_t *) dsc->payload->data_sources->head->data)->data_points->head->data;
    TEST_ASSERT_EQUAL_STRING_MESSAGE("DP001", data_point->id, "Data point id is not correct.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Voltage", data_point->name, "Data point name is not correct.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Voltage value read.", data_point->description, "Data point description is not correct.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("DOUBLE", data_point->type, "Data point type is not correct.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("V", data_point->unit, "Data point unit is not correct.");

    // Clean up.
    mcl_data_source_configuration_destroy(&dsc);
}
