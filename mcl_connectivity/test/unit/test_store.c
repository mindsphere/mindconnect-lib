/*!
 * @file     test_store.c
 * @brief    Test module for testing functions wich operates on a MCL store.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mock_mcl_event.h"
#include "mock_event.h"
#include "mock_mcl_timeseries.h"
#include "mock_timeseries.h"
#include "mock_mcl_file.h"
#include "mock_file.h"
#include "mock_mcl_data_source_configuration.h"
#include "mock_data_source_configuration.h"
#include "mock_mcl_custom_data.h"
#include "mock_custom_data.h"
#include "store.h"
#include "mcl_core/mcl_memory.h"
#include "memory.h"
#include "list.h"
#include "string_util.h"
#include "mcl_connectivity/mcl_store.h"
#include "time_util.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : No special condition.
 * WHEN  : mcl_store_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized store struct.
 */
void test_initialize_001()
{
    mcl_store_t *store = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_store_initialize(&store);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for store.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, store->last_item_id, "Last item id is not 0.");
    TEST_ASSERT_NOT_NULL_MESSAGE(store, "Store must point to allocated memory!");

    // Clean up.
    mcl_store_destroy(&store);
    TEST_ASSERT_NULL_MESSAGE(store, "Store is not null after it is destroyed.");
}

/**
 * GIVEN : Store parameter is null.
 * WHEN  : mcl_store_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002()
{
    // Test.
    mcl_error_t code = mcl_store_initialize(MCL_NULL);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_store_initialize() unexpected return error.");
}

/**
 * GIVEN : Store parameter is null.
 * WHEN  : mcl_store_add() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_store_add_001()
{
    // Initialize event.
    mcl_event_t *event = MCL_NULL;
    MCL_NEW(event);
    const uint32_t item_preamble = (('M' << 24) | ('C' << 16) | ('L' << 8));
    event->item_base.preamble = item_preamble;
    event->item_base.type = MCL_ITEM_TYPE_EVENT;

    // Test.
    mcl_error_t code = mcl_store_add(MCL_NULL, &event);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_store_add() unexpected return error.");

    // Clean up.
    MCL_FREE(event);
}

/**
 * GIVEN : Item parameter is null.
 * WHEN  : mcl_store_add() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_store_add_002()
{
    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(&store);

    // Test.
    mcl_error_t code = mcl_store_add(store, MCL_NULL);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_store_add() unexpected return error.");

    // Clean up.
    mcl_store_destroy(&store);
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : mcl_store_add() is called for an invalid item type.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_store_add_003()
{
    // Initialize dummy item.
    const uint32_t item_preamble = (('M' << 24) | ('C' << 16) | ('L' << 8));
    mcl_item_t item =
    {
        .preamble = item_preamble,
        .type = MCL_ITEM_TYPE_STORE
    };

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(&store);

    // Test.
    mcl_error_t code = mcl_store_add(store, &item);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "mcl_store_add() unexpected return error.");

    // Clean up.
    mcl_store_destroy(&store);
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : mcl_store_add() is called for an event item having an invalid preamble.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_store_add_004()
{
    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(&store);

    // Initialize event.
    mcl_event_t *event = MCL_NULL;
    MCL_NEW(event);
    event->item_base.preamble = 0xFF;
    event->item_base.type = MCL_ITEM_TYPE_EVENT;

    // Test.
    mcl_error_t code = mcl_store_add(store, event);

    // Check result.
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "mcl_store_add() unexpected return error.");

    // Clean up.
    mcl_store_destroy(&store);
    MCL_FREE(event);
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : mcl_store_add() is called for an event.
 * THEN  : MCL_OK is returned and item is added to store.
 */
void test_store_add_005()
{
    // Initialize event.
    mcl_event_t *event = MCL_NULL;
    MCL_NEW(event);
    MCL_NEW(event->payload);
    const uint32_t item_preamble = (('M' << 24) | ('C' << 16) | ('L' << 8));
    event->item_base.preamble = item_preamble;
    event->item_base.type = MCL_ITEM_TYPE_EVENT;

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(&store);

    // Set up mocks.
    event_validate_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_event_destroy_ExpectAnyArgs();

    // Test.
    mcl_error_t code = mcl_store_add(store, event);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_store_add() unexpected return error.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(event, (((store_item_t *)store->item_list->current->data)->item), "Event and added item to store have different addresses.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, store->last_item_id, "Last item id is not 1.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(STORE_ITEM_STATUS_READY, (((store_item_t *)store->item_list->current->data)->status), "Status of item is not ready.");

    // Clean up.
    mcl_store_destroy(&store);
    MCL_FREE(event->payload);
    MCL_FREE(event);
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : Two sequential mcl_store_add() functions are called for an event and a timeseries.
 * THEN  : MCL_OK is returned for both calls and items are added to store.
 */
void test_store_add_006()
{
    mcl_list_node_t *node = MCL_NULL;

    // Initialize event.
    mcl_event_t *event = MCL_NULL;
    MCL_NEW(event);
    MCL_NEW(event->payload);
    const uint32_t item_preamble = (('M' << 24) | ('C' << 16) | ('L' << 8));
    event->item_base.preamble = item_preamble;
    event->item_base.type = MCL_ITEM_TYPE_EVENT;

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(&store);

    // Set up mocks.
    event_validate_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_event_destroy_ExpectAnyArgs();

    // Test.
    mcl_error_t code = mcl_store_add(store, event);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_store_add() unexpected return error.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, store->last_item_id, "Last item id is not 1.");

    // Initialize timeseries.
    mcl_timeseries_t *timeseries = MCL_NULL;
    MCL_NEW(timeseries);
    MCL_NEW(timeseries->payload);
    timeseries->item_base.preamble = item_preamble;
    timeseries->item_base.type = MCL_ITEM_TYPE_TIMESERIES;

    // Set up mocks.
    timeseries_validate_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_timeseries_destroy_ExpectAnyArgs();

    // Test.
    code = mcl_store_add(store, timeseries);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_store_add() unexpected return error.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, store->last_item_id, "Last item id is not 2.");

    // Check event is added to store.
    code = mcl_list_next(store->item_list, &node);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_list_next() unexpected return error.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(event, (((store_item_t *)node->data)->item), "Event and added item to store have different addresses.");

    // Check timeseries is added to store.
    code = mcl_list_next(store->item_list, &node);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_list_next() unexpected return error.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(timeseries, (((store_item_t *)node->data)->item), "Timeseries and added item to store have different addresses.");

    // Clean up.
    mcl_store_destroy(&store);
    MCL_FREE(event->payload);
    MCL_FREE(event);
    MCL_FREE(timeseries->payload);
    MCL_FREE(timeseries);
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : mcl_store_add() is called for a file.
 * THEN  : MCL_OK is returned and item is added to store.
 */
void test_store_add_007()
{
    // Initialize file.
    mcl_file_t *file = MCL_NULL;
    MCL_NEW(file);
    MCL_NEW(file->payload);
    const uint32_t item_preamble = (('M' << 24) | ('C' << 16) | ('L' << 8));
    file->item_base.preamble = item_preamble;
    file->item_base.type = MCL_ITEM_TYPE_FILE;

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(&store);

    // Set up mocks.
    file_validate_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_file_destroy_ExpectAnyArgs();

    // Test.
    mcl_error_t code = mcl_store_add(store, file);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_store_add() unexpected return error.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(file, (((store_item_t *)store->item_list->current->data)->item), "File and added item to store have different addresses.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, store->last_item_id, "Last item id is not 1.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(STORE_ITEM_STATUS_READY, (((store_item_t *)store->item_list->current->data)->status), "Status of item is not ready.");

    // Clean up.
    mcl_store_destroy(&store);
    MCL_FREE(file->payload);
    MCL_FREE(file);
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : mcl_store_add() is called for a data source configuration.
 * THEN  : MCL_OK is returned and item is added to store.
 */
void test_store_add_008()
{
    // Initialize data source configuration.
    mcl_data_source_configuration_t *data_source_configuration = MCL_NULL;
    MCL_NEW(data_source_configuration);
    MCL_NEW(data_source_configuration->payload);
    const uint32_t item_preamble = (('M' << 24) | ('C' << 16) | ('L' << 8));
    data_source_configuration->item_base.preamble = item_preamble;
    data_source_configuration->item_base.type = MCL_ITEM_TYPE_DATA_SOURCE_CONFIGURATION;

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(&store);

    // Set up mocks.
    data_source_configuration_validate_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_data_source_configuration_destroy_ExpectAnyArgs();

    // Test.
    mcl_error_t code = mcl_store_add(store, data_source_configuration);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_store_add() unexpected return error.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(data_source_configuration, (((store_item_t *)store->item_list->current->data)->item), "Data source configuration and added item to store have different addresses.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, store->last_item_id, "Last item id is not 1.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(STORE_ITEM_STATUS_READY, (((store_item_t *)store->item_list->current->data)->status), "Status of item is not ready.");

    // Clean up.
    mcl_store_destroy(&store);
    MCL_FREE(data_source_configuration->payload);
    MCL_FREE(data_source_configuration);
}

/**
 * GIVEN : Store is initialized.
 * WHEN  : mcl_store_add() is called for a custom data.
 * THEN  : MCL_OK is returned and item is added to store.
 */
void test_store_add_009()
{
    // Initialize custom data.
    mcl_custom_data_t *custom_data = MCL_NULL;
    MCL_NEW(custom_data);
    MCL_NEW(custom_data->payload);
    const uint32_t item_preamble = (('M' << 24) | ('C' << 16) | ('L' << 8));
    custom_data->item_base.preamble = item_preamble;
    custom_data->item_base.type = MCL_ITEM_TYPE_CUSTOM_DATA;

    // Initialize store.
    mcl_store_t *store = MCL_NULL;
    mcl_store_initialize(&store);

    // Set up mocks.
    custom_data_validate_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_custom_data_destroy_ExpectAnyArgs();

    // Test.
    mcl_error_t code = mcl_store_add(store, custom_data);

    // Check results.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_store_add() unexpected return error.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(custom_data, (((store_item_t *)store->item_list->current->data)->item), "Custom data and added item to store have different addresses.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, store->last_item_id, "Last item id is not 1.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(STORE_ITEM_STATUS_READY, (((store_item_t *)store->item_list->current->data)->status), "Status of item is not ready.");

    // Clean up.
    mcl_store_destroy(&store);
    MCL_FREE(custom_data->payload);
    MCL_FREE(custom_data);
}
