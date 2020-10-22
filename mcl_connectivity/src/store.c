/*!
 * @file     store.c
 * @brief    This file implements MCL Store Interface.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_connectivity/mcl_store.h"
#include "store.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_list.h"
#include "mcl_connectivity/mcl_event.h"
#include "mcl_connectivity/mcl_timeseries.h"
#include "mcl_connectivity/mcl_file.h"
#include "mcl_connectivity/mcl_custom_data.h"
#include "mcl_connectivity/mcl_data_source_configuration.h"
#include "event.h"
#include "timeseries.h"
#include "file.h"
#include "custom_data.h"
#include "data_source_configuration.h"

// This function adds item to store.
static mcl_error_t _store_add_data(mcl_store_t *store, void *item);

// This function destroys item.
static mcl_error_t _item_destroy(void **item);

mcl_error_t mcl_store_initialize(mcl_store_t **store)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_store_t **store = <%p>", store);

    // Null check.
    MCL_ASSERT_NOT_NULL(store, code);

    // Allocate store.
    MCL_NEW(*store);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != *store, MCL_OUT_OF_MEMORY, "Memory for store could not be allocated!");

    (*store)->item_base.preamble = MCL_ITEM_PREAMBLE;
    (*store)->item_base.type = MCL_ITEM_TYPE_STORE;
    (*store)->last_item_id = 0;

    // Initialize list containing mcl data types.
    code = mcl_list_initialize(&((*store)->item_list));
    MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, MCL_FREE(*store), code, "Initialization of store list failed!");

    MCL_DEBUG("Store list has been successfully initialized.");

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_store_add(mcl_store_t *store, void *item)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_store_t *store = <%p>, void *item = <%p>", store, item);

    // Null check.
    MCL_ASSERT_NOT_NULL(store, code);
    MCL_ASSERT_NOT_NULL(item, code);

    if (MCL_ITEM_PREAMBLE != ((mcl_item_t *)item)->preamble)
    {
        MCL_DEBUG_LEAVE("retVal = <%d>", MCL_INVALID_PARAMETER);
        return MCL_INVALID_PARAMETER;
    }

    code = _store_add_data(store, item);

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_store_destroy(mcl_store_t **store)
{
    MCL_DEBUG_ENTRY("mcl_store_t **store = <%p>", store);

    // Make sure input argument is not NULL.
    if ((MCL_NULL != store) && (MCL_NULL != *store))
    {
        mcl_list_destroy_with_content(&(*store)->item_list, (mcl_list_item_destroy_callback)store_item_destroy);
        MCL_FREE(*store);
    }
    else
    {
        MCL_DEBUG("Store is already NULL. Nothing will be destroyed.");
    }

    MCL_DEBUG_LEAVE("retVal = <void>");
}

static mcl_error_t _store_add_data(mcl_store_t *store, void *item)
{
    mcl_error_t code;
    store_item_t *store_item;

    MCL_DEBUG_ENTRY("mcl_store_t *store = <%p>, void *item = <%p>", store, item);

    // Check data fields if they are filled according to item type
    switch (((mcl_item_t *) item)->type)
    {
        case MCL_ITEM_TYPE_TIMESERIES:
            code = timeseries_validate((timeseries_t *) item);
            break;

        case MCL_ITEM_TYPE_EVENT:
            code = event_validate((event_t *) item);
            break;

        case MCL_ITEM_TYPE_FILE:
            code = file_validate((file_t *) item);
            break;

        case MCL_ITEM_TYPE_CUSTOM_DATA:
            code = custom_data_validate((custom_data_t *) item);
            break;

        case MCL_ITEM_TYPE_DATA_SOURCE_CONFIGURATION:
            code = data_source_configuration_validate((data_source_configuration_t *) item);
            break;

        default:
            return MCL_INVALID_PARAMETER;
    }

    MCL_ASSERT_CODE_MESSAGE(MCL_OK == code, MCL_INVALID_PARAMETER, "Item validation failed for the selected data type.");

    MCL_NEW(store_item);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != store_item, MCL_OUT_OF_MEMORY, "Not enough memory to create store_item!");

    store_item->item = item;
    store_item->id = (store)->last_item_id + 1;
    store_item->status = STORE_ITEM_STATUS_READY;

    code = mcl_list_add(store->item_list, store_item);
    MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, MCL_FREE(store_item), code, "Add to list failed!");

    // Increase the last_item_id if the item is added to store.
    (store)->last_item_id = store_item->id;

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static mcl_error_t _item_destroy(void **item)
{
    MCL_DEBUG_ENTRY("void **item = <%p>", item);

    switch (((mcl_item_t *) *item)->type)
    {
        case MCL_ITEM_TYPE_TIMESERIES:
            mcl_timeseries_destroy((timeseries_t **) item);
            break;

        case MCL_ITEM_TYPE_EVENT:
            mcl_event_destroy((event_t **) item);
            break;

        case MCL_ITEM_TYPE_FILE:
            mcl_file_destroy((file_t **) item);
            break;

        case MCL_ITEM_TYPE_CUSTOM_DATA:
            mcl_custom_data_destroy((custom_data_t **) item);
            break;

        case MCL_ITEM_TYPE_DATA_SOURCE_CONFIGURATION:
            mcl_data_source_configuration_destroy((data_source_configuration_t **) item);
            break;

        default:
            break;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void store_item_destroy(store_item_t **store_item)
{
    MCL_DEBUG_ENTRY("store_item_t **store_item = <%p>", store_item);

    // Destroy item.
    _item_destroy(&((*store_item)->item));
    MCL_FREE(*store_item);

    MCL_DEBUG_LEAVE("retVal = void");
}
