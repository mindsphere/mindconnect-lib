/*!
 * @file     json.h
 * @brief    Json module header file.
 *
 * This module creates json strings for all data types.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef JSON_H_
#define JSON_H_

#include "mcl_connectivity/mcl_connectivity_common.h"
#include "mapping.h"

/**
 * This function creates json representation of an item meta.
 *
 * @param [in] item MCL item (Event, Timeseries, etc.).
 * @param [out] json_string Json string representation of item meta.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t json_from_item_meta(void *item, char **json_string);

/**
 * This function creates json representation of an item payload in a json array.
 *
 * @param [in] item Item which will be used to create json string.
 * @param [out] json_string Json string representation of item payload.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t json_from_item_payload(void *item, char **json_string);

/**
 * This function calculates the item size.
 *
 * @param [in] item Item to calculate its size.
 * @return Size of the item.
 */
MCL_LOCAL mcl_size_t json_get_item_size(void *item);

/**
 * This function parses json representation of an item to its struct (Currently, only for data source configuration).
 *
 * @param [in] json_string Json string.
 * @param [in] string_length Length of json string.
 * @param [out] item Item struct.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_JSON_NON_EXISTING_CHILD in case there is a missing mandatory field.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t json_parse_item(const char *json_string, mcl_size_t string_length, void **item);

#endif //JSON_H_
