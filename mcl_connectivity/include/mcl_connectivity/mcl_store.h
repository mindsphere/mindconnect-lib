/*!
 * @file     mcl_store.h
 * @brief    Store module interface header file.
 *
 * Store is the container for all types of data such as #mcl_custom_data_t, #mcl_event_t, #mcl_file_t,
 * #mcl_data_source_configuration_t and #mcl_timeseries_t to be exchanged using MCL.
 *
 * Agents can upload store to MindSphere. This module enables the agent to initialize a store
 * data type using #mcl_store_initialize function. Following initialization, all types of data can
 * be added to the store using #mcl_store_add function. Once the store data type is ready, it can be
 * uploaded to MindSphere using #mcl_connectivity_exchange function. After the exchange operation,
 * the agent is expected to destroy the store using #mcl_store_destroy function.
 *
 * For more information, please look at example file "mcl_connectivity/examples/store_upload.c".
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_STORE_H_
#define MCL_STORE_H_

#include "mcl_connectivity/mcl_connectivity_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for store.
 */
typedef struct mcl_store_t mcl_store_t;

/**
 * This function creates and initializes an object of type #mcl_store_t.
 *
 * Store is used to hold different types of data. These data will be processed later by connectivity module.
 * to perform exchange operation.
 *
 * @param [out] store The newly initialized store.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p store is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case initialization of store fails.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_store_initialize(mcl_store_t **store);

/**
 * This function adds a new item to store.
 *
 * @param [in] store MCL store which will contain any type of item created.
 * @param [in] item Item to be added to store.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any mandatory argument is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if data fields in item is not correct.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_store_add(mcl_store_t *store, void *item);

/**
 * This function destroys the #mcl_store_t object and frees any memory allocated.
 *
 * @param [in] store Preinitialized #mcl_store_t object to destroy.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p store is NULL.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT void mcl_store_destroy(mcl_store_t **store);

#ifdef  __cplusplus
}
#endif

#endif //MCL_STORE_H_
