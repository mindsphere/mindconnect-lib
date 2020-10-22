/*!
 * @file     store.h
 * @brief    Store module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef STORE_H_
#define STORE_H_

#include "mcl_core/mcl_list.h"
#include "item.h"

/**
 * Store item status.
 */
typedef enum E_MCL_STORE_ITEM_STATUS
{
    STORE_ITEM_STATUS_READY,        //!< This item is not processed before.
    STORE_ITEM_STATUS_SELECTED,     //!< This item is not processed before, selected for current request.
    STORE_ITEM_STATUS_PROCESSED,    //!< Current item has been added to the current http request.
    STORE_ITEM_STATUS_IGNORED,      //!< There is not enough space in the http request for the current item.
} E_MCL_STORE_ITEM_STATUS;

/**
 * General data type for item in #store_item_t.
 * Data can be one of the following (distinguish by type):
 * <ul>
 * <li>time_series_t *</li>
 * <li>event_t *</li>
 * <li>file_t *</li>
 * <li>custom_data_t *</li>
 * <li>data_source_configuration_t *</li>
 * </ul>
 */
typedef struct mcl_store_item_t
{
    void *item;                     //!< Item to be added to the store.
    E_MCL_STORE_ITEM_STATUS status; //!< Status of item in the store.
    mcl_size_t id;                  //!< Id of the item in the store.
    mcl_size_t item_size;           //!< Size of the item in the store.
} store_item_t;

/**
 * Handle for store.
 */
typedef struct mcl_store_t
{
    mcl_item_t item_base;    //!< Item base of an item.
    mcl_list_t *item_list;   //!< Contains #store_item_t items.
    mcl_size_t last_item_id; //!< Id of the last item in the store.
} store_t;

/**
 * This function is called to destroy store item.
 *
 * @param [in] store_item Store item to be destroyed.
 */
MCL_LOCAL void store_item_destroy(store_item_t **store_item);

#endif //STORE_H_
