/*!
 * @file     item.h
 * @brief    Item module header file.
 *
 * This module contains item definition.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef ITEM_H_
#define ITEM_H_

#include "mcl_connectivity/mcl_connectivity_common.h"

// Preamble code is an integer data to verify items that will be stored.
#define MCL_ITEM_PREAMBLE (('M' << 24) | ('C' << 16) | ('L' << 8))

/**
 * Item type.
 */
typedef enum E_MCL_ITEM_TYPE
{
    MCL_ITEM_TYPE_TIMESERIES,                //!< Item type timeseries.
    MCL_ITEM_TYPE_EVENT,                     //!< Item type event.
    MCL_ITEM_TYPE_FILE,                      //!< Item type file.
    MCL_ITEM_TYPE_CUSTOM_DATA,               //!< Item type custom data.
    MCL_ITEM_TYPE_DATA_SOURCE_CONFIGURATION, //!< Item type data source configuration.
    MCL_ITEM_TYPE_STORE                      //!< Item type store.
} E_MCL_ITEM_TYPE;

/**
 * Handle for item.
 */
typedef struct mcl_item_t
{
    mcl_uint32_t preamble; //!< Item preamble.
    E_MCL_ITEM_TYPE type;  //!< Item type.
    mcl_uint32_t version;  //!< Item version.
} mcl_item_t;

#endif //ITEM_H_
