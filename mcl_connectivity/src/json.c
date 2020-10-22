/*!
 * @file     json.c
 * @brief    Json module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "json.h"
#include "mcl_core/mcl_string_util.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_connectivity/mcl_event.h"
#include "event.h"
#include "data_source_configuration.h"
#include "timeseries.h"
#include "file.h"
#include "custom_data.h"
#include "mcl_connectivity/mcl_data_source_configuration.h"

#define ITEM_TYPE "item"
#define ITEM_VERSION "1.0"

#define DOUBLE_BRACE_SIZE   2
#define DELIMITER_SIZE      1

// Definitions to calculate item size. It is better to make them all compile-time constants.

// {"type":"ITEM_TYPE","version":"ITEM_VERSION","payload":<meta_payload_json>}
#define ITEM_META_BASE_SIZE (sizeof("{\"type\":\""ITEM_TYPE"\",\"version\":\""ITEM_VERSION"\",\"payload\":}") - MCL_NULL_CHAR_SIZE)

// {"type":"standardTimeSeries","version":"1.0","details":{"configurationId":"<configuration_id>"}}
#define ITEM_TIMESERIES_META_PAYLOAD_BASE_SIZE (sizeof("{\"type\":\"standardTimeSeries\",\"version\":\"\",\"details\":{\"configurationId\":\"\"}}") - MCL_NULL_CHAR_SIZE)

// {"dataPointId":"<data_point_id>","value":"<value>","qualityCode":"<quality_code>"}
#define ITEM_TIMESERIES_VALUE_BASE_SIZE (sizeof("{\"dataPointId\":\"\",\"value\":\"\",\"qualityCode\":\"\"}") - MCL_NULL_CHAR_SIZE)

// {"timestamp":"<timestamp>","values":[<values>]}
#define ITEM_TIMESERIES_VALUE_LIST_BASE_SIZE (sizeof("{\"timestamp\":\"\",\"values\":[]}") + MCL_TIMESTAMP_LENGTH - 2 * MCL_NULL_CHAR_SIZE)

// {"type":"businessEvent","version":"<version>"}
#define ITEM_EVENT_META_PAYLOAD_BASE_SIZE (sizeof("{\"type\":\"businessEvent\",\"version\":\"\"}") - MCL_NULL_CHAR_SIZE)

// {"id":"<id>","timestamp":"<timestamp>","severity":<severity>,"type":"<type>","version":"<version>","details":<details_json>}
#define ITEM_EVENT_PAYLOAD_BASE_SIZE (sizeof("{\"id\":\"\",\"timestamp\":\"\",\"severity\":,\"type\":\"\",\"version\":\"\",\"details\":}") + MCL_TIMESTAMP_LENGTH - 2 * MCL_NULL_CHAR_SIZE)

// Correlation ID is optional for event.
#define ITEM_EVENT_PAYLOAD_CORRELATION_ID_SIZE (sizeof(",\"correlationId\":\"\"") - MCL_NULL_CHAR_SIZE)

// Description is optional for event.
#define ITEM_EVENT_PAYLOAD_DESCRIPTION_SIZE (sizeof(",\"description\":\"\"") - MCL_NULL_CHAR_SIZE)

// {"type":"file","version":"1.0","details":{"fileName":"<fileName>","creationDate":"<creationDate>"}}
#define ITEM_FILE_META_PAYLOAD_BASE_SIZE (sizeof("{\"type\":\"file\",\"version\":\"\",\"details\":{\"fileName\":\"\",\"creationDate\":\"\"}}") + MCL_TIMESTAMP_LENGTH - 2 * MCL_NULL_CHAR_SIZE)

// File type is optional for file meta payload.
#define ITEM_FILE_META_PAYLOAD_FILE_TYPE_SIZE (sizeof(",\"fileType\":\"\"") - MCL_NULL_CHAR_SIZE)

// {"type":"<type>","version":"<version>"}
#define ITEM_CUSTOM_DATA_META_PAYLOAD_BASE_SIZE (sizeof("{\"type\":\"\",\"version\":\"\"}") - MCL_NULL_CHAR_SIZE)

// Json object can be added for custom data as details.
#define ITEM_CUSTOM_DATA_META_PAYLOAD_DETAILS_SIZE (sizeof(",\"details\":") - MCL_NULL_CHAR_SIZE)

// {\"type\":\"dataSourceConfiguration\",\"version\":\"<version>\"}
#define ITEM_DATA_SOURCE_CONFIGURATION_META_PAYLOAD_BASE_SIZE (sizeof("{\"type\":\"dataSourceConfiguration\",\"version\":\"\"}") - MCL_NULL_CHAR_SIZE)

// {"configurationId":"<configurationId>","dataSources":[<dataSources>]}
#define ITEM_DATA_SOURCE_CONFIGURATION_PAYLOAD_BASE_SIZE (sizeof("{\"configurationId\":\"\",\"dataSources\":[]}") - MCL_NULL_CHAR_SIZE)

// {"name":"<name>","description":"<description>","dataPoints":[<dataPoints>]}
#define ITEM_DATA_SOURCE_BASE_SIZE (sizeof("{\"name\":\"\",\"description\":\"\",\"dataPoints\":[]}") - MCL_NULL_CHAR_SIZE)

// {"id":"<id>","name":"<name>","description":"<description>","type":"<type>","unit":"<unit>"}
#define ITEM_DATA_POINT_BASE_SIZE (sizeof("{\"id\":\"\",\"name\":\"\",\"description\":\"\",\"type\":\"\",\"unit\":\"\"}") - MCL_NULL_CHAR_SIZE)

// Json object can be added to data source and data point as custom data.
#define ITEM_DATA_SOURCE_CONFIGURATION_PAYLOAD_CUSTOM_DATA_SIZE (sizeof(",\"customData\":") - MCL_NULL_CHAR_SIZE)

/**
 * This is used with #meta_field_names array to get the related string.
 * ex: meta_field_names[META_FIELD_TYPE] returns the string of "type".
 */
typedef enum E_META_FIELD_NAMES
{
    META_FIELD_TYPE,                             //!< Type of meta field.
    META_FIELD_VERSION,                          //!< Version of meta field.
    META_FIELD_PAYLOAD_META,                     //!< Meta of meta field payload.
    META_FIELD_PAYLOAD,                          //!< Meta field payload.
    META_FIELD_PAYLOAD_TYPE,                     //!< Type of meta field payload.
    META_FIELD_PAYLOAD_VERSION,                  //!< Version of meta field payload.
    META_FIELD_PAYLOAD_DETAILS,                  //!< Meta field payload details.
    META_FIELD_PAYLOAD_DETAILS_CONFIGURATION_ID, //!< Configuration id of meta field payload details.
    META_FIELD_DETAILS,                          //!< Meta field details.
    META_FIELD_PAYLOAD_DETAILS_FILE_NAME,        //!< File name of meta field details.
    META_FIELD_PAYLOAD_DETAILS_FILE_TYPE,        //!< File type of meta field details.
    META_FIELD_PAYLOAD_DETAILS_CREATION_DATE,    //!< Creation date of meta field details.
    META_FIELD_DETAILS_TOTAL_ITEMS,              //!< Total items of meta field details.
    META_FIELD_PAYLOAD_DETAILS_TIMESTAMP,        //!< Timestamp of meta field payload details.
    META_FIELD_PAYLOAD_DETAILS_DURATION,         //!< Duration of meta field payload details.
    META_FIELD_NAMES_END                         //!< End of meta field names enumeration.
} E_META_FIELD_NAMES;

/**
 * This is used with #meta_field_values array to get the related string.
 * ex: meta_field_values[META_FIELD_TYPE_ITEM] returns the string of "item".
 * Please refer to the MindConnectAPI Specifications for detailed info.
 */
typedef enum E_META_FIELD_VALUES
{
    META_FIELD_PAYLOAD_TYPE_TIME_SERIES,               //!< Time series type of meta field payload.
    META_FIELD_PAYLOAD_TYPE_BUSINESS_EVENT,            //!< Business event type of meta field payload.
    META_FIELD_PAYLOAD_TYPE_FILE,                      //!< File type of meta field payload.
    META_FIELD_PAYLOAD_TYPE_DATA_SOURCE_CONFIGURATION, //!< Data source configuration type of meta field payload.
    META_FIELD_VALUES_END                              //!< End of meta field values enumeration.
} E_META_FIELD_VALUES;

/**
 * This is used with #payload_field_names to get the related string.
 * ex: payload_field_names[PAYLOAD_FIELD_TIMESTAMP] returns the string of "timeStamp".
 */
typedef enum E_PAYLOAD_FIELD_NAMES
{
    PAYLOAD_FIELD_TIMESTAMP,                     //!< Timestamp of payload field.
    PAYLOAD_FIELD_VALUES,                        //!< Payload field values.
    PAYLOAD_FIELD_VALUES_DATA_POINT_ID,          //!< Data point id of payload field values.
    PAYLOAD_FIELD_VALUES_VALUE,                  //!< Value of payload field values.
    PAYLOAD_FIELD_VALUES_QUALITY_CODE,           //!< Quality code of payload field values.
    PAYLOAD_FIELD_ID,                            //!< Payload field id.
    PAYLOAD_FIELD_CUSTOM_EVENT_ID,               //!< Custom event id of payload field.
    PAYLOAD_FIELD_CORRELATION_ID,                //!< Correlation id of payload field.
    PAYLOAD_FIELD_SOURCE_TYPE,                   //!< Source type of payload field.
    PAYLOAD_FIELD_SOURCE_ID,                     //!< Source id of payload field.
    PAYLOAD_FIELD_SEVERITY,                      //!< Severity of payload field.
    PAYLOAD_FIELD_DESCRIPTION,                   //!< Description of payload field.
    PAYLOAD_FIELD_STATUS,                        //!< Status of payload field.
    PAYLOAD_FIELD_TYPE,                          //!< Type of payload field.
    PAYLOAD_FIELD_VERSION,                       //!< Version of payload field.
    PAYLOAD_FIELD_CUSTOM_DATA,                   //!< Custom data of payload field.
    PAYLOAD_FIELD_DETAILS,                       //!< Details of payload field.
    PAYLOAD_FIELD_DETAILS_MESSAGE,               //!< Message of payload field details.
    PAYLOAD_FIELD_DETAILS_ERROR_CODE,            //!< Error code of payload field details.
    PAYLOAD_FIELD_DETAILS_DETAILS,               //!< Details of payload field details.
    PAYLOAD_FIELD_DETAILS_VALUE,                 //!< Value of payload field details.
    PAYLOAD_FIELD_DETAILS_DOWNLOAD_LINK,         //!< Download link of payload field details.
    PAYLOAD_FIELD_DETAILS_NAME,                  //!< Name of payload field details.
    PAYLOAD_FIELD_DETAILS_VERSION,               //!< Version of payload field details.
    PAYLOAD_FIELD_DETAILS_DESCRIPTION,           //!< Description of payload field details.
    PAYLOAD_FIELD_CONFIGURATION_ID,              //!< Configuration id of payload field.
    PAYLOAD_FIELD_DATA_SOURCES,                  //!< Payload field data sources.
    PAYLOAD_FIELD_DATA_SOURCES_NAME,             //!< Name of payload field data sources.
    PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS,      //!< Payload field data sources data points.
    PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_ID,   //!< Data point id of payload field assets data sources data points.
    PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_NAME, //!< Name of payload field data sources data points.
    PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_TYPE, //!< Type of payload field data sources data points.
    PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_UNIT, //!< Unit of payload field data sources data points.
    PAYLOAD_FIELD_NAMES_END                      //!< End of payload field names enumeration.
} E_PAYLOAD_FIELD_NAMES;

/**
 * Meta field names.
 */
static const char *meta_field_names[META_FIELD_NAMES_END] =
{
    "type",
    "version",
    "meta",
    "payload",
    "type",
    "version",
    "details",
    "configurationId",
    "details",
    "fileName",
    "fileType",
    "creationDate",
    "totalItems",
    "timestamp",
    "duration"
};

/**
 * Meta field values.
 */
static const char *meta_field_values[META_FIELD_VALUES_END] =
{
    "standardTimeSeries",
    "businessEvent",
    "file",
    "dataSourceConfiguration"
};

/**
 * Payload field names.
 */
static const char *payload_field_names[PAYLOAD_FIELD_NAMES_END] =
{
     "timestamp",
     "values",
     "dataPointId",
     "value",
     "qualityCode",
     "id",
     "customEventId",
     "correlationId",
     "sourceType",
     "sourceId",
     "severity",
     "description",
     "status",
     "type",
     "version",
     "customData",
     "details",
     "message",
     "errorCode",
     "details",
     "value",
     "download_link",
     "name",
     "version",
     "description",
     "configurationId",
     "dataSources",
     "name",
     "dataPoints",
     "id",
     "name",
     "type",
     "unit"
};

// Function pointer definition for _fill_array_using_list.
typedef mcl_error_t (*json_converter_callback)(void *any_struct, mcl_json_t **json);

// Function pointer definition for _fill_list_using_json_array.
typedef mcl_error_t (*json_parser_callback)(mcl_json_t *json, void **any_struct);

// Convert nodes in the list to json object via callback function and add them to the json array.
static mcl_error_t _fill_array_using_list(mcl_json_t *json_array, mcl_list_t *list, json_converter_callback callback);

// Fill list of structures using json array.
static mcl_error_t _fill_list_using_json_array(mcl_json_t *json_array, json_parser_callback parser_callback, mcl_list_item_destroy_callback destroy_callback, mcl_list_t *list);

// Add item meta payload to root json.
static mcl_error_t _add_item_meta_payload(mcl_item_t *item, mcl_json_t *root);

// Add item meta payload details to the payload json.
static mcl_error_t _add_item_meta_payload_details(mcl_item_t *item, mcl_json_t *payload);

// Add item meta details to root json.
static mcl_error_t _add_item_meta_details(mcl_item_t *item, mcl_json_t *root);

// Convert event to json and add to event payload array.
static mcl_error_t _add_event_to_payload_array(event_t *event, mcl_json_t *event_payload_array);

// Get json string (which is a json array containing one item) from event payload.
static mcl_error_t _json_from_event_payload(event_t *event, char **json_string);

// Get json string from data source configuration payload.
static mcl_error_t _json_from_data_source_configuration_payload(data_source_configuration_t *data_source_configuration, char **json_string);

// Get json string from timeseries payload.
static mcl_error_t _json_from_timeseries_payload(timeseries_t *timeseries, char **json_string);

// Convert data point to json object.
static mcl_error_t _json_from_data_point(data_point_t *data_point, mcl_json_t **json);

// Convert data source to json object.
static mcl_error_t _json_from_data_source(data_source_t *data_source, mcl_json_t **json);

// Convert timeseries value to json object.
static mcl_error_t _json_from_timeseries_value(timeseries_value_t *timeseries_value, mcl_json_t **json);

// Convert timeseries value list to json object.
static mcl_error_t _json_from_timeseries_value_list(timeseries_value_list_t *timeseries_value_list, mcl_json_t **json);

// Returns item meta size of an item.
static mcl_size_t _item_meta_size(mcl_item_t *item);

// Returns item meta size of a custom data.
static mcl_size_t _custom_data_meta_size(custom_data_t *custom_data);

// Returns item payload size of a timeseries.
static mcl_size_t _item_payload_size_for_timeseries(timeseries_t *timeseries);

// Returns item payload size of a timeseries value list.
static mcl_size_t _item_payload_size_for_timeseries_value_list(timeseries_value_list_t *timeseries_value_list);

// Returns item payload size of a timeseries value.
static mcl_size_t _item_payload_size_for_timeseries_value(timeseries_value_t *timeseries_value);

// Returns item payload size of an event.
static mcl_size_t _item_payload_size_for_event(event_t *event);

// Returns item payload size of a data source configuration.
static mcl_size_t _item_payload_size_for_data_source_configuration(data_source_configuration_t *data_source_configuration);

// Returns item payload size of a data source.
static mcl_size_t _item_payload_size_for_data_source(data_source_t *data_source);

// Returns item payload size of a data point.
static mcl_size_t _item_payload_size_for_data_point(data_point_t *data_point);

// Add item meta payload timeseries details.
static mcl_error_t _add_item_meta_payload_timeseries_details(timeseries_t *timeseries, mcl_json_t *payload);

// Add item meta payload file details.
static mcl_error_t _add_item_meta_payload_file_details(file_t *file, mcl_json_t *payload);

// Parse and get string value.
static mcl_error_t _parse_and_get_string_value(mcl_json_t *json, const char *field_name, char **string);

// Parse data point.
static mcl_error_t _parse_data_point(mcl_json_t *json, data_point_t **data_point);

// Parse data source.
static mcl_error_t _parse_data_source(mcl_json_t *json, data_source_t **data_source);

// Parse data source configuration.
static mcl_error_t _parse_data_source_configuration(mcl_json_t *json, data_source_configuration_t **data_source_configuration);

mcl_error_t json_from_item_meta(void *item, char **json_string)
{
    mcl_error_t code;
    mcl_json_t *root = MCL_NULL;
    *json_string = MCL_NULL;

    MCL_DEBUG_ENTRY("void *item = <%p>, char **json_string = <%p>", item, json_string);

    code = mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Add item type.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(root, meta_field_names[META_FIELD_TYPE], ITEM_TYPE);
    }

    // Add item version.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(root, meta_field_names[META_FIELD_VERSION], ITEM_VERSION);
    }

    // Add details.
    if (MCL_OK == code)
    {
        code = _add_item_meta_details((mcl_item_t *) item, root);
    }

    if (MCL_OK == code)
    {
        code = _add_item_meta_payload((mcl_item_t *) item, root);
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_to_string(root, json_string);
    }

    mcl_json_util_destroy(&root);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t json_from_item_payload(void *item, char **json_string)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("void *item = <%p>, char **json_string = <%p>", item, json_string);

    switch (((mcl_item_t *) item)->type)
    {
        case MCL_ITEM_TYPE_EVENT:
            code = _json_from_event_payload((event_t *) item, json_string);
            break;

        case MCL_ITEM_TYPE_DATA_SOURCE_CONFIGURATION:
            code = _json_from_data_source_configuration_payload((data_source_configuration_t *) item, json_string);
            break;

        case MCL_ITEM_TYPE_TIMESERIES:
            code = _json_from_timeseries_payload((timeseries_t *) item, json_string);
            break;

        default:
            code = MCL_FAIL;
            break;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _json_from_event_payload(event_t *event, char **json_string)
{
    mcl_error_t code;
    mcl_json_t *json_array = MCL_NULL;
    *json_string = MCL_NULL;

    MCL_DEBUG_ENTRY("event_t *event = <%p>, char **json_string = <%p>", event, json_string);

    // Initialize a json array.
    code = mcl_json_util_initialize(MCL_JSON_ARRAY, &json_array);

    // Add event payload to json array.
    if (MCL_OK == code)
    {
        code = _add_event_to_payload_array(event, json_array);
    }

    // Convert json array to string.
    if (MCL_OK == code)
    {
        code = mcl_json_util_to_string(json_array, json_string);
    }

    // Destroy json array.
    mcl_json_util_destroy(&json_array);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_size_t json_get_item_size(void *item)
{
    mcl_size_t size;

    MCL_DEBUG_ENTRY("void *item = <%p>", item);

    size = _item_meta_size((mcl_item_t *) item);

    switch (((mcl_item_t *) item)->type)
    {
        case MCL_ITEM_TYPE_TIMESERIES:
            size += _item_payload_size_for_timeseries((timeseries_t *) item);
            break;

        case MCL_ITEM_TYPE_EVENT:
            size += _item_payload_size_for_event((event_t *) item);
            break;

        case MCL_ITEM_TYPE_FILE:
            size += ((file_t *) item)->payload->size;
            break;

        case MCL_ITEM_TYPE_CUSTOM_DATA:
            size += ((custom_data_t *) item)->payload->size;
            break;

        case MCL_ITEM_TYPE_DATA_SOURCE_CONFIGURATION:
            size += _item_payload_size_for_data_source_configuration((data_source_configuration_t *) item);
            break;

        default:
            break;
    }

    MCL_DEBUG_LEAVE("retVal = <%lu>", size);
    return size;
}

mcl_error_t json_parse_item(const char *json_string, mcl_size_t string_length, void **item)
{
    mcl_error_t code;
    mcl_json_t *json = MCL_NULL;

    MCL_DEBUG_ENTRY("const char *json_string = <%p>, mcl_size_t string_length = <%lu>, void **item = <%p>", json_string, (long unsigned) string_length, item);

    code = mcl_json_util_parse(json_string, string_length, &json);

    if (MCL_OK == code)
    {
        // Currently, only data source configuration is parsed.
        code = _parse_data_source_configuration(json, (data_source_configuration_t **) item);
    }

    mcl_json_util_destroy(&json);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _add_item_meta_payload(mcl_item_t *item, mcl_json_t *root)
{
    mcl_error_t code = MCL_OK;
    mcl_json_t *payload = MCL_NULL;
    const char *payload_type = MCL_NULL;
    const char *payload_version = MCL_NULL;

    MCL_DEBUG_ENTRY("mcl_item_t *item = <%p>, mcl_json_t *root = <%p>", item, root);

    switch (item->type)
    {
        case MCL_ITEM_TYPE_EVENT:
            payload_type = meta_field_values[META_FIELD_PAYLOAD_TYPE_BUSINESS_EVENT];
            payload_version = mcl_event_versions[item->version];
            break;

        case MCL_ITEM_TYPE_DATA_SOURCE_CONFIGURATION:
            payload_type = meta_field_values[META_FIELD_PAYLOAD_TYPE_DATA_SOURCE_CONFIGURATION];
            payload_version = mcl_data_source_configuration_versions[item->version];
            break;

        case MCL_ITEM_TYPE_TIMESERIES:
            payload_type = meta_field_values[META_FIELD_PAYLOAD_TYPE_TIME_SERIES];
            payload_version = mcl_timeseries_versions[item->version];
            break;

        case MCL_ITEM_TYPE_FILE:
            payload_type = meta_field_values[META_FIELD_PAYLOAD_TYPE_FILE];
            payload_version = mcl_file_versions[item->version];
            break;

        case MCL_ITEM_TYPE_CUSTOM_DATA:
            payload_type = ((custom_data_t *) item)->payload->type;
            payload_version = ((custom_data_t *) item)->payload->version;
            break;

        default:
            code = MCL_FAIL;
            break;
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_start_object(root, meta_field_names[META_FIELD_PAYLOAD], &payload);
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(payload, meta_field_names[META_FIELD_PAYLOAD_TYPE], payload_type);
    }
    else
    {
        // payload is not added to root parent. So destroy payload struct with root_handle in it.
        mcl_json_util_destroy(&payload);
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(payload, meta_field_names[META_FIELD_PAYLOAD_VERSION], payload_version);
    }

    if (MCL_OK == code)
    {
        code = _add_item_meta_payload_details(item, payload);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _add_item_meta_payload_details(mcl_item_t *item, mcl_json_t *payload)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_item_t *item = <%p>, mcl_json_t *payload = <%p>", item, payload);

    switch (item->type)
    {
        // There is no meta payload details for event, data source configuration.
        case MCL_ITEM_TYPE_EVENT:
        case MCL_ITEM_TYPE_DATA_SOURCE_CONFIGURATION:
            break;

        case MCL_ITEM_TYPE_TIMESERIES:
            code = _add_item_meta_payload_timeseries_details((timeseries_t *) item, payload);
            break;

        case MCL_ITEM_TYPE_FILE:
            code = _add_item_meta_payload_file_details((file_t *) item, payload);
            break;

        case MCL_ITEM_TYPE_CUSTOM_DATA:
            if (MCL_NULL != ((custom_data_t *) item)->payload->details)
            {
                mcl_json_t *details_json = MCL_NULL;
                code = mcl_json_util_duplicate(((custom_data_t *) item)->payload->details, &details_json);

                if (MCL_OK == code)
                {
                    code = mcl_json_util_add_object(payload, meta_field_names[META_FIELD_PAYLOAD_DETAILS], details_json);
                }

                if (MCL_OK != code)
                {
                    mcl_json_util_destroy(&details_json);
                }
            }
            break;

        default:
            code = MCL_FAIL;
            break;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _add_event_to_payload_array(event_t *event, mcl_json_t *event_payload_array)
{
    mcl_error_t code;
    mcl_json_t *details_json = MCL_NULL;
    mcl_json_t *event_payload_json = MCL_NULL;

    MCL_DEBUG_ENTRY("event_t *event = <%p>, mcl_json_t *event_payload_array = <%p>", event, event_payload_array);

    // Initialize new json object to add to json array.
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, &event_payload_json);

    // Add id.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(event_payload_json, payload_field_names[PAYLOAD_FIELD_ID], event->payload->id);
    }

    // Add correlationId.
    if ((MCL_OK == code) && (MCL_NULL != event->payload->correlation_id) && (MCL_NULL_CHAR != event->payload->correlation_id[0]))
    {
        code = mcl_json_util_add_string(event_payload_json, payload_field_names[PAYLOAD_FIELD_CORRELATION_ID], event->payload->correlation_id);
    }

    // Add timestamp.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(event_payload_json, payload_field_names[PAYLOAD_FIELD_TIMESTAMP], event->payload->timestamp);
    }

    // Add severity.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_uint(event_payload_json, payload_field_names[PAYLOAD_FIELD_SEVERITY], event->payload->severity);
    }

    // Add description.
    if ((MCL_OK == code) && (MCL_NULL != event->payload->description) && (MCL_NULL_CHAR != event->payload->description[0]))
    {
        code = mcl_json_util_add_string(event_payload_json, payload_field_names[PAYLOAD_FIELD_DESCRIPTION], event->payload->description);
    }

    // Add type.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(event_payload_json, payload_field_names[PAYLOAD_FIELD_TYPE], event->payload->type);
    }

    // Add version.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(event_payload_json, payload_field_names[PAYLOAD_FIELD_VERSION], event->payload->version);
    }

    // Create details.
    if ((MCL_OK == code) && (MCL_NULL != event->payload->details))
    {
        code = mcl_json_util_duplicate(event->payload->details, &details_json);
    }
    else
    {
        code = mcl_json_util_initialize(MCL_JSON_OBJECT, &details_json);
    }

    // Add details even if it is empty.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_object(event_payload_json, payload_field_names[PAYLOAD_FIELD_DETAILS], details_json);
    }

    // Add json object to json array.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_item_to_array(event_payload_array, event_payload_json);

        if (MCL_OK != code)
        {
            mcl_json_util_destroy(&event_payload_json);
        }
    }
    else
    {
        mcl_json_util_destroy(&details_json);
        mcl_json_util_destroy(&event_payload_json);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _add_item_meta_details(mcl_item_t *item, mcl_json_t *root)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_item_t *item = <%p>, mcl_json_t *root = <%p>", item, root);

    // Unused parameter.
    (void) root;

    switch (item->type)
    {
        // There is no meta details for event, data source configuration, timeseries, file.
        case MCL_ITEM_TYPE_EVENT:
        case MCL_ITEM_TYPE_DATA_SOURCE_CONFIGURATION:
        case MCL_ITEM_TYPE_TIMESERIES:
        case MCL_ITEM_TYPE_FILE:
        case MCL_ITEM_TYPE_CUSTOM_DATA:
            break;

        default:
            code = MCL_FAIL;
            break;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _json_from_data_source_configuration_payload(data_source_configuration_t *data_source_configuration, char **json_string)
{
    mcl_error_t code;
    mcl_json_t *json_object = MCL_NULL;
    *json_string = MCL_NULL;

    MCL_DEBUG_ENTRY("data_source_configuration_t *data_source_configuration = <%p>, char **json_string = <%p>", data_source_configuration, json_string);

    // Initialize a json object.
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, &json_object);

    // Add configuration id.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(json_object, payload_field_names[PAYLOAD_FIELD_CONFIGURATION_ID], data_source_configuration->payload->configuration_id);
    }

    // Add data sources
    if (MCL_OK == code)
    {
        mcl_json_t *data_sources_array = MCL_NULL;

        // Initialize a json array.
        code = mcl_json_util_start_array(json_object, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES], &data_sources_array);

        // Fill data sources array.
        if (MCL_OK == code)
        {
            code = _fill_array_using_list(data_sources_array, data_source_configuration->payload->data_sources, (json_converter_callback)_json_from_data_source);
        }
    }

    // Get json string from json object.
    if (MCL_OK == code)
    {
        code = mcl_json_util_to_string(json_object, json_string);
    }

    mcl_json_util_destroy(&json_object);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _json_from_timeseries_payload(timeseries_t *timeseries, char **json_string)
{
    mcl_error_t code;
    mcl_json_t *value_list_array = MCL_NULL;
    *json_string = MCL_NULL;

    MCL_DEBUG_ENTRY("timeseries_t *timeseries = <%p>, char **json_string = <%p>", timeseries, json_string);

    // Initialize a json array.
    code = mcl_json_util_initialize(MCL_JSON_ARRAY, &value_list_array);

    // Fill value list array.
    if (MCL_OK == code)
    {
        code = _fill_array_using_list(value_list_array,timeseries->payload->value_lists, (json_converter_callback)_json_from_timeseries_value_list);
    }

    // Get json string from json object.
    if (MCL_OK == code)
    {
        code = mcl_json_util_to_string(value_list_array, json_string);
    }

    mcl_json_util_destroy(&value_list_array);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _fill_array_using_list(mcl_json_t *json_array, mcl_list_t *list, json_converter_callback callback)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t index;

    MCL_DEBUG_ENTRY("mcl_json_t *json_array = <%p>, mcl_list_t *list = <%p>, json_converter_callback callback = <%p>", json_array, list, callback);

    // Reset list to iterate from the first node.
    mcl_list_reset(list);

    for (index = 0; index < list->count; ++index)
    {
        mcl_list_node_t *node = MCL_NULL;
        mcl_json_t *json_object = MCL_NULL;
        code = mcl_list_next(list, &node);

        // Convert struct to json object via callback function.
        if (MCL_OK == code)
        {
            code = callback(node->data, &json_object);
        }

        // Add json item to json array.
        if (MCL_OK == code)
        {
            code = mcl_json_util_add_item_to_array(json_array, json_object);
        }

        if (MCL_OK != code)
        {
            // Destroy all content.
            mcl_json_util_destroy(&json_object);
            break;
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _json_from_data_point(data_point_t *data_point, mcl_json_t **json)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("data_point_t *data_point = <%p>, mcl_json_t **json = <%p>", data_point, json);

    // Initialize a json object.
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, json);

    // Add ID.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(*json, payload_field_names[PAYLOAD_FIELD_ID], data_point->id);
    }

    // Add name.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(*json, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_NAME], data_point->name);
    }

    // Add description.
    if ((MCL_OK == code) && (MCL_NULL != data_point->description))
    {
        code = mcl_json_util_add_string(*json, payload_field_names[PAYLOAD_FIELD_DESCRIPTION], data_point->description);
    }

    // Add type.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(*json, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_TYPE], data_point->type);
    }

    // Add unit.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(*json, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_UNIT], data_point->unit);
    }

    // Add custom data.
    if ((MCL_OK == code) && (MCL_NULL != data_point->custom_data))
    {
        mcl_json_t *duplicate_custom_data = MCL_NULL;

        code = mcl_json_util_duplicate(data_point->custom_data, &duplicate_custom_data);

        if (MCL_OK == code)
        {
            code = mcl_json_util_add_object(*json, payload_field_names[PAYLOAD_FIELD_CUSTOM_DATA], duplicate_custom_data);
        }

        if (MCL_OK != code)
        {
            mcl_json_util_destroy(&duplicate_custom_data);
        }
    }

    if (MCL_OK != code)
    {
        mcl_json_util_destroy(json);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _json_from_data_source(data_source_t *data_source, mcl_json_t **json)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("data_source *data_source = <%p>, mcl_json_t **json = <%p>", data_source, json);

    // Initialize a json object.
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, json);

    // Add name.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(*json, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_NAME], data_source->name);
    }

    // Add description.
    if ((MCL_OK == code) && (MCL_NULL != data_source->description))
    {
        code = mcl_json_util_add_string(*json, payload_field_names[PAYLOAD_FIELD_DESCRIPTION], data_source->description);
    }

    // Add data points.
    if (MCL_OK == code)
    {
        mcl_json_t *data_points_array = MCL_NULL;

        // Initialize a json array.
        code = mcl_json_util_start_array(*json, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS], &data_points_array);

        // Fill data points array.
        if (MCL_OK == code)
        {
            code = _fill_array_using_list(data_points_array, data_source->data_points, (json_converter_callback)_json_from_data_point);
        }
    }

    // Add custom data.
    if ((MCL_OK == code) && (MCL_NULL != data_source->custom_data))
    {
        mcl_json_t *duplicate_custom_data = MCL_NULL;

        code = mcl_json_util_duplicate(data_source->custom_data, &duplicate_custom_data);

        if (MCL_OK == code)
        {
            code = mcl_json_util_add_object(*json, payload_field_names[PAYLOAD_FIELD_CUSTOM_DATA], duplicate_custom_data);
        }

        if (MCL_OK != code)
        {
            mcl_json_util_destroy(&duplicate_custom_data);
        }
    }

    if (MCL_OK != code)
    {
        mcl_json_util_destroy(json);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _json_from_timeseries_value(timeseries_value_t *timeseries_value, mcl_json_t **json)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("timeseries_value_t *timeseries_value = <%p>, mcl_json_t **json = <%p>", timeseries_value, json);

    // Initialize a json object.
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, json);

    // Add dataPointId.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(*json, payload_field_names[PAYLOAD_FIELD_VALUES_DATA_POINT_ID], timeseries_value->data_point_id);
    }

    // Add value.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(*json, payload_field_names[PAYLOAD_FIELD_VALUES_VALUE], timeseries_value->value);
    }

    // Add qualityCode.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(*json, payload_field_names[PAYLOAD_FIELD_VALUES_QUALITY_CODE], timeseries_value->quality_code);
    }

    if (MCL_OK != code)
    {
        mcl_json_util_destroy(json);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _json_from_timeseries_value_list(timeseries_value_list_t *value_list, mcl_json_t **json)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("timeseries_value_list_t *value_list = <%p>, mcl_json_t **json = <%p>", value_list, json);

    // Initialize a json object.
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, json);

    // Add timestamp.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(*json, payload_field_names[PAYLOAD_FIELD_TIMESTAMP], value_list->timestamp);
    }

    // Add values.
    if (MCL_OK == code)
    {
        mcl_json_t *value_array = MCL_NULL;

        // Initialize a json array.
        code = mcl_json_util_start_array(*json, payload_field_names[PAYLOAD_FIELD_VALUES], &value_array);

        // Fill value array.
        if (MCL_OK == code)
        {
            code = _fill_array_using_list(value_array, value_list->values, (json_converter_callback)_json_from_timeseries_value);
        }
    }

    if (MCL_OK != code)
    {
        mcl_json_util_destroy(json);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_size_t _item_payload_size_for_timeseries(timeseries_t *timeseries)
{
    // Timeseries payload is array of timeseries lists.
    mcl_size_t size = DOUBLE_BRACE_SIZE;
    mcl_size_t list_count = timeseries->payload->value_lists->count;
    mcl_size_t index;

    MCL_VERBOSE_ENTRY("timeseries_t *timeseries = <%p>", timeseries);

    // Add size of timeseries value list(s).
    mcl_list_reset(timeseries->payload->value_lists);

    for (index = 0; index < list_count; ++index)
    {
        mcl_list_node_t *node;
        mcl_list_next(timeseries->payload->value_lists, &node);

        size += _item_payload_size_for_timeseries_value_list((timeseries_value_list_t *) node->data);
    }

    // Add delimiter (',') size.
    size += (list_count - 1) * DELIMITER_SIZE;

    MCL_VERBOSE_LEAVE("retVal = <%lu>", size);
    return size;
}

static mcl_size_t _item_payload_size_for_timeseries_value_list(timeseries_value_list_t *timeseries_value_list)
{
    mcl_size_t size = ITEM_TIMESERIES_VALUE_LIST_BASE_SIZE;
    mcl_size_t list_count = timeseries_value_list->values->count;
    mcl_size_t index;

    MCL_VERBOSE_ENTRY("timeseries_value_list_t *timeseries_value_list = <%p>", timeseries_value_list);

    mcl_list_reset(timeseries_value_list->values);

    // Add size of timeseries value(s).
    for (index = 0; index < list_count; ++index)
    {
        mcl_list_node_t *node;
        mcl_list_next(timeseries_value_list->values, &node);

        size += _item_payload_size_for_timeseries_value((timeseries_value_t *) node->data);
    }

    // Add delimiter (',') size.
    size += (list_count - 1) * DELIMITER_SIZE;

    MCL_VERBOSE_LEAVE("retVal = <%lu>", size);
    return size;
}

static mcl_size_t _item_payload_size_for_timeseries_value(timeseries_value_t *timeseries_value)
{
    mcl_size_t size = ITEM_TIMESERIES_VALUE_BASE_SIZE;

    MCL_VERBOSE_ENTRY("timeseries_value_t *timeseries_value = <%p>", timeseries_value);

    // Add size of timeseries value fields.
    size += mcl_string_util_strlen(timeseries_value->data_point_id);
    size += mcl_string_util_strlen(timeseries_value->quality_code);
    size += mcl_string_util_strlen(timeseries_value->value);

    MCL_VERBOSE_LEAVE("retVal = <%lu>", size);
    return size;
}

static mcl_size_t _item_payload_size_for_event(event_t *event)
{
    // Event will be sent in a json array.
    mcl_size_t size = ITEM_EVENT_PAYLOAD_BASE_SIZE + DOUBLE_BRACE_SIZE;
    char *temp_json = MCL_NULL;

    MCL_VERBOSE_ENTRY("event_t *event = <%p>", event);

    // Add size of event fields.
    size += mcl_string_util_strlen(event->payload->id);
    size += mcl_string_util_strlen(event->payload->type);
    size += mcl_string_util_strlen(event->payload->version);

    // For event version 1.0, severity is a single digit number.
    // For event version 2.0, severity is a double digit number.
    if (MCL_EVENT_VERSION_1_0 == event->item_base.version)
    {
        ++size;
    }
    else if (MCL_EVENT_VERSION_2_0 == event->item_base.version)
    {
        size += 2;
    }

    // Add correlation id size.
    if (MCL_NULL != event->payload->correlation_id)
    {
        size += ITEM_EVENT_PAYLOAD_CORRELATION_ID_SIZE;
        size += mcl_string_util_strlen(event->payload->correlation_id);
    }

    // Add description size.
    if (MCL_NULL != event->payload->description)
    {
        size += ITEM_EVENT_PAYLOAD_DESCRIPTION_SIZE;
        size += mcl_string_util_strlen(event->payload->description);
    }

    // Details json for event can be null, but it will be added as empty json anyway.
    if (MCL_NULL != event->payload->details)
    {
        (void) mcl_json_util_to_string(event->payload->details, &temp_json);
    }

    if (MCL_NULL != temp_json)
    {
        size += mcl_string_util_strlen(temp_json);
        MCL_FREE(temp_json);
    }
    else
    {
        size += DOUBLE_BRACE_SIZE;
    }

    MCL_VERBOSE_LEAVE("retVal = <%lu>", size);
    return size;
}

static mcl_size_t _item_payload_size_for_data_source_configuration(data_source_configuration_t *data_source_configuration)
{
    mcl_size_t size = ITEM_DATA_SOURCE_CONFIGURATION_PAYLOAD_BASE_SIZE;
    mcl_size_t list_count = data_source_configuration->payload->data_sources->count;
    mcl_size_t index;

    MCL_VERBOSE_ENTRY("data_source_configuration_payload_t *data_source_configuration = <%p>", data_source_configuration);

    size += mcl_string_util_strlen(data_source_configuration->payload->configuration_id);

    mcl_list_reset(data_source_configuration->payload->data_sources);

    // Add size of data source(s).
    for (index = 0; index < list_count; ++index)
    {
        mcl_list_node_t *node;
        mcl_list_next(data_source_configuration->payload->data_sources, &node);

        size += _item_payload_size_for_data_source((data_source_t *) node->data);
    }

    // Add delimiter (',') size.
    size += (list_count - 1) * DELIMITER_SIZE;

    MCL_VERBOSE_LEAVE("retVal = <%lu>", size);
    return size;
}

static mcl_size_t _item_payload_size_for_data_source(data_source_t *data_source)
{
    mcl_size_t size = ITEM_DATA_SOURCE_BASE_SIZE;
    mcl_size_t list_count = data_source->data_points->count;
    mcl_size_t index;
    char *temp_json = MCL_NULL;

    MCL_VERBOSE_ENTRY("data_source_t *data_source = <%p>", data_source);

    // Add size of data source fields.
    size += mcl_string_util_strlen(data_source->name);
    size += mcl_string_util_strlen(data_source->description);

    mcl_list_reset(data_source->data_points);

    // Add size of data point(s).
    for (index = 0; index < list_count; ++index)
    {
        mcl_list_node_t *node;
        mcl_list_next(data_source->data_points, &node);

        size += _item_payload_size_for_data_point((data_point_t *) node->data);
    }

    // Add delimiter (',') size.
    size += (list_count - 1) * DELIMITER_SIZE;

    // Data source may have a custom data json.
    if (MCL_NULL != data_source->custom_data)
    {
        size += ITEM_DATA_SOURCE_CONFIGURATION_PAYLOAD_CUSTOM_DATA_SIZE;
        (void) mcl_json_util_to_string(data_source->custom_data, &temp_json);

        // If temp_json is NULL when data_source->custom_data is not NULL, calculated size will be less than actual size.
        if (MCL_NULL != temp_json)
        {
            size += mcl_string_util_strlen(temp_json);
            MCL_FREE(temp_json);
        }
        else
        {
            size += DOUBLE_BRACE_SIZE;
        }
    }

    MCL_VERBOSE_LEAVE("retVal = <%lu>", size);
    return size;
}

static mcl_size_t _item_payload_size_for_data_point(data_point_t *data_point)
{
    mcl_size_t size = ITEM_DATA_POINT_BASE_SIZE;
    char *temp_json = MCL_NULL;

    MCL_VERBOSE_ENTRY("data_point_t *data_point = <%p>", data_point);

    // Add size of data point fields.
    size += mcl_string_util_strlen(data_point->id);
    size += mcl_string_util_strlen(data_point->name);
    size += mcl_string_util_strlen(data_point->description);
    size += mcl_string_util_strlen(data_point->type);
    size += mcl_string_util_strlen(data_point->unit);

    // Data point may have a custom data json.
    if (MCL_NULL != data_point->custom_data)
    {
        size += ITEM_DATA_SOURCE_CONFIGURATION_PAYLOAD_CUSTOM_DATA_SIZE;
        (void) mcl_json_util_to_string(data_point->custom_data, &temp_json);

        // If temp_json is NULL when data_point->custom_data is not NULL, calculated size will be less than actual size.
        if (MCL_NULL != temp_json)
        {
            size += mcl_string_util_strlen(temp_json);
            MCL_FREE(temp_json);
        }
        else
        {
            size += DOUBLE_BRACE_SIZE;
        }
    }

    MCL_VERBOSE_LEAVE("retVal = <%lu>", size);
    return size;
}

static mcl_size_t _item_meta_size(mcl_item_t *item)
{
    mcl_size_t size = ITEM_META_BASE_SIZE;

    MCL_VERBOSE_ENTRY("mcl_item_t *item = <%p>", item);

    switch (item->type)
    {
        case MCL_ITEM_TYPE_TIMESERIES:
            size += ITEM_TIMESERIES_META_PAYLOAD_BASE_SIZE;
            size += mcl_string_util_strlen(((timeseries_t *) item)->payload->configuration_id);
            size += mcl_string_util_strlen(mcl_timeseries_versions[item->version]);
            break;

        case MCL_ITEM_TYPE_EVENT:
            size += ITEM_EVENT_META_PAYLOAD_BASE_SIZE;
            size += mcl_string_util_strlen(mcl_event_versions[item->version]);
            break;

        case MCL_ITEM_TYPE_FILE:
            size += ITEM_FILE_META_PAYLOAD_BASE_SIZE;
            size += mcl_string_util_strlen(((file_t *) item)->payload->remote_name);

            if (MCL_NULL != ((file_t *) item)->payload->type)
            {
                size += ITEM_FILE_META_PAYLOAD_FILE_TYPE_SIZE;
                size += mcl_string_util_strlen(((file_t *)item)->payload->type);
            }

            size += mcl_string_util_strlen(mcl_file_versions[item->version]);
            break;

        case MCL_ITEM_TYPE_CUSTOM_DATA:
            size += _custom_data_meta_size((custom_data_t *) item);
            break;

        case MCL_ITEM_TYPE_DATA_SOURCE_CONFIGURATION:
            size += ITEM_DATA_SOURCE_CONFIGURATION_META_PAYLOAD_BASE_SIZE;
            size += mcl_string_util_strlen(mcl_data_source_configuration_versions[item->version]);
            break;

        default:
            break;
    }

    MCL_VERBOSE_LEAVE("retVal = <%lu>", size);
    return size;
}

static mcl_size_t _custom_data_meta_size(custom_data_t *custom_data)
{
    mcl_size_t size = ITEM_CUSTOM_DATA_META_PAYLOAD_BASE_SIZE;

    MCL_VERBOSE_ENTRY("custom_data_t *custom_data = <%p>", custom_data);

    size += mcl_string_util_strlen(custom_data->payload->type);
    size += mcl_string_util_strlen(custom_data->payload->version);

    if (MCL_NULL != custom_data->payload->details)
    {
        char *temp_json = MCL_NULL;
        size += ITEM_CUSTOM_DATA_META_PAYLOAD_DETAILS_SIZE;

        (void) mcl_json_util_to_string(custom_data->payload->details, &temp_json);

        // If temp_json is NULL, calculated size will be less than actual size.
        if (MCL_NULL != temp_json)
        {
            size += mcl_string_util_strlen(temp_json);
            MCL_FREE(temp_json);
        }
        else
        {
            size += DOUBLE_BRACE_SIZE;
        }
    }

    MCL_VERBOSE_LEAVE("retVal = <%lu>", size);
    return size;
}

static mcl_error_t _add_item_meta_payload_timeseries_details(timeseries_t *timeseries, mcl_json_t *payload)
{
    mcl_error_t code;
    mcl_json_t *details_json = MCL_NULL;

    MCL_DEBUG_ENTRY("timeseries_t *timeseries = <%p>, mcl_json_t *payload = <%p>", timeseries, payload);

    code = mcl_json_util_start_object(payload, meta_field_names[META_FIELD_PAYLOAD_DETAILS], &details_json);

    // Add configuration id.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(details_json, meta_field_names[META_FIELD_PAYLOAD_DETAILS_CONFIGURATION_ID], timeseries->payload->configuration_id);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _add_item_meta_payload_file_details(file_t *file, mcl_json_t *payload)
{
    mcl_error_t code;
    mcl_json_t *details_json = MCL_NULL;

    MCL_DEBUG_ENTRY("file_t *file = <%p>, mcl_json_t *payload = <%p>", file, payload);

    code = mcl_json_util_start_object(payload, meta_field_names[META_FIELD_PAYLOAD_DETAILS], &details_json);

    // Add file name.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(details_json, meta_field_names[META_FIELD_PAYLOAD_DETAILS_FILE_NAME], file->payload->remote_name);
    }

    // Add creation date.
    if (MCL_OK == code)
    {
        code = mcl_json_util_add_string(details_json, meta_field_names[META_FIELD_PAYLOAD_DETAILS_CREATION_DATE], file->payload->creation_date);
    }

    // Add file type.
    if ((MCL_OK == code) && (MCL_NULL != file->payload->type))
    {
        code = mcl_json_util_add_string(details_json, meta_field_names[META_FIELD_PAYLOAD_DETAILS_FILE_TYPE], file->payload->type);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _fill_list_using_json_array(mcl_json_t *json_array, json_parser_callback parser_callback, mcl_list_item_destroy_callback destroy_callback, mcl_list_t *list)
{
    mcl_error_t code;
    mcl_size_t index;
    mcl_size_t array_size;

    MCL_DEBUG_ENTRY("mcl_json_t *json_array = <%p>, json_parser_callback callback = <%p>, mcl_list_item_destroy_callback destroy_callback = <%p>, mcl_list_t *list = <%p>", json_array, parser_callback, destroy_callback, list);

    // Get array size.
    code = mcl_json_util_get_array_size(json_array, &array_size);

    for (index = 0; (MCL_OK == code) && (index < array_size); ++index)
    {
        mcl_json_t *item;
        void *a_struct = MCL_NULL;

        // Get item from array.
        code = mcl_json_util_get_array_item(json_array, (int) index, &item);

        // Parse json into a struct via parser_callback.
        if (MCL_OK == code)
        {
            code = parser_callback(item, &a_struct);
        }

        // Add parsed struct to the list.
        if (MCL_OK == code)
        {
            code = mcl_list_add(list, a_struct);
        }

        if (MCL_OK != code)
        {
            destroy_callback(a_struct);
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_data_point(mcl_json_t *json, data_point_t **data_point)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, data_point_t **data_point = <%p>", json, data_point);

    code = mcl_data_point_initialize(data_point);

    // Parse id.
    if (MCL_OK == code)
    {
        code = _parse_and_get_string_value(json, payload_field_names[PAYLOAD_FIELD_ID], &((*data_point)->id));
    }

    // Parse name.
    if (MCL_OK == code)
    {
        code = _parse_and_get_string_value(json, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_NAME], &((*data_point)->name));
    }

    // Parse type.
    if (MCL_OK == code)
    {
        code = _parse_and_get_string_value(json, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_TYPE], &((*data_point)->type));
    }

    // Parse unit.
    if (MCL_OK == code)
    {
        code = _parse_and_get_string_value(json, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_UNIT], &((*data_point)->unit));
    }

    // Parse description if exists.
    if (MCL_OK == code)
    {
        code = _parse_and_get_string_value(json, payload_field_names[PAYLOAD_FIELD_DESCRIPTION], &((*data_point)->description));

        if (MCL_JSON_NON_EXISTING_CHILD == code)
        {
            code = MCL_OK;
        }
    }

    // Duplicate custom data json if exists.
    if (MCL_OK == code)
    {
        mcl_json_t *custom_data_json = MCL_NULL;

        code = mcl_json_util_get_object_item(json, payload_field_names[PAYLOAD_FIELD_CUSTOM_DATA], &custom_data_json);

        if (MCL_JSON_NON_EXISTING_CHILD == code)
        {
            code = MCL_OK;
        }
        else
        {
            code = mcl_json_util_duplicate(custom_data_json, &((*data_point)->custom_data));
        }
    }

    if (MCL_OK != code)
    {
        mcl_data_point_destroy(data_point);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_data_source(mcl_json_t *json, data_source_t **data_source)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, data_source_t **data_source = <%p>", json, data_source);

    code = mcl_data_source_initialize(data_source);

    // Parse name.
    if (MCL_OK == code)
    {
        code = _parse_and_get_string_value(json, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_NAME], &((*data_source)->name));
    }

    // Parse description if exists.
    if (MCL_OK == code)
    {
        code = _parse_and_get_string_value(json, payload_field_names[PAYLOAD_FIELD_DESCRIPTION], &((*data_source)->description));

        if (MCL_JSON_NON_EXISTING_CHILD == code)
        {
            code = MCL_OK;
        }
    }

    // Duplicate custom data json if exists.
    if (MCL_OK == code)
    {
        mcl_json_t *custom_data_json = MCL_NULL;

        code = mcl_json_util_get_object_item(json, payload_field_names[PAYLOAD_FIELD_CUSTOM_DATA], &custom_data_json);

        if (MCL_JSON_NON_EXISTING_CHILD == code)
        {
            code = MCL_OK;
        }
        else
        {
            code = mcl_json_util_duplicate(custom_data_json, &((*data_source)->custom_data));
        }
    }

    // Parse data points.
    if (MCL_OK == code)
    {
        mcl_json_t *data_point_array = MCL_NULL;

        code = mcl_json_util_get_object_item(json, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS], &data_point_array);

        if (MCL_OK == code)
        {
            code = _fill_list_using_json_array(data_point_array, (json_parser_callback) _parse_data_point, (mcl_list_item_destroy_callback) mcl_data_point_destroy, (*data_source)->data_points);
        }
    }

    if (MCL_OK != code)
    {
        mcl_data_source_destroy(data_source);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_data_source_configuration(mcl_json_t *json, data_source_configuration_t **data_source_configuration)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, data_source_configuration_t **data_source_configuration = <%p>", json, data_source_configuration);

    code = mcl_data_source_configuration_initialize(MCL_DATA_SOURCE_CONFIGURATION_1_0, data_source_configuration);

    // Parse configuration id.
    if (MCL_OK == code)
    {
        code = _parse_and_get_string_value(json, payload_field_names[PAYLOAD_FIELD_CONFIGURATION_ID], &((*data_source_configuration)->payload->configuration_id));
    }

    // Parse data sources.
    if (MCL_OK == code)
    {
        mcl_json_t *data_point_array = MCL_NULL;

        code = mcl_json_util_get_object_item(json, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES], &data_point_array);

        if (MCL_OK == code)
        {
            code = _fill_list_using_json_array(data_point_array, (json_parser_callback) _parse_data_source, (mcl_list_item_destroy_callback) mcl_data_source_destroy, (*data_source_configuration)->payload->data_sources);
        }
    }

    if (MCL_OK != code)
    {
        mcl_data_source_configuration_destroy(data_source_configuration);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _parse_and_get_string_value(mcl_json_t *json, const char *field_name, char **string)
{
    mcl_error_t code;
    mcl_json_t *object;

    MCL_DEBUG_ENTRY("mcl_json_t *json = <%p>, const char *field_name = <%p>, char **string = <%p>", json, field_name, string);

    code = mcl_json_util_get_object_item(json, field_name, &object);

    if (MCL_OK == code)
    {
        code = mcl_json_util_get_string(object, string);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
