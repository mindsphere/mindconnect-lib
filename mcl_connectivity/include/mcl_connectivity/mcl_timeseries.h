/*!
 * @file     mcl_timeseries.h
 * @brief    Timeseries module interface header file.
 *
 * Agents can upload timeseries data to MindSphere. Before exchanging any timeseries, data source
 * configuration must be uploaded to MindSphere which describes timeseries. Timeseries data is
 * coupled with the data source configuration by the configuration id of the data source configuration
 * which is assigned when uploading data source configuration.
 *
 * This module enables the agent to initialize a timeseries using #mcl_timeseries_initialize
 * function. Following initialization, the parameters of this timeseries can be set using
 * #mcl_timeseries_set_parameter function. Then timeseries value list(s) can be added to the timeseries
 * using #mcl_timeseries_add_value_list function. See mcl_connectivity/mcl_timeseries_value_list.h for information
 * about timeseries value lists. Once the timeseries is ready, it can be uploaded to MindSphere
 * using #mcl_connectivity_exchange function. Optionally, the timeseries can be added to #mcl_store_t
 * and the store containing the timeseries and possible other data can be uploaded to MindSphere. If the
 * timeseries is exchanged as a single item, the agent is expected to destroy the timeseries
 * after the exchange operation using #mcl_timeseries_destroy function. If the timeseries is exchanged
 * in a store, store will handle the destroy operation of the timeseries and the agent does not need to
 * take any action.
 *
 * For more information, please look at example file "mcl_connectivity/examples/timeseries_upload.c".
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_TIMESERIES_H_
#define MCL_TIMESERIES_H_

#include "mcl_connectivity/mcl_timeseries_value_list.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for timeseries.
 */
typedef struct mcl_timeseries_t mcl_timeseries_t;

/**
 * Version of timeseries item payload.
 */
typedef enum E_MCL_TIMESERIES_VERSION
{
    MCL_TIMESERIES_VERSION_1_0 = 0, //!< Timeseries version 1.0.
    MCL_TIMESERIES_VERSION_END      //!< End of timeseries version enumeration.
} E_MCL_TIMESERIES_VERSION;

/**
 * Parameters for timeseries.
 */
typedef enum E_MCL_TIMESERIES_PARAMETER
{
    MCL_TIMESERIES_PARAMETER_CONFIGURATION_ID = 0, //!< Timeseries configuration ID parameter as char*.
    MCL_TIMESERIES_PARAMETER_END
} E_MCL_TIMESERIES_PARAMETER;

/**
 * This function creates and initializes a data struct of #mcl_timeseries_t.
 *
 * @param [in] version One of the versions listed in #E_MCL_TIMESERIES_VERSION.
 * @param [out] timeseries Timeseries handle which is going to be initialized.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER in case meta payload version parameter is invalid.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p timeseries is null.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_timeseries_initialize(E_MCL_TIMESERIES_VERSION version, mcl_timeseries_t **timeseries);

/**
 * This function is used to set a parameter of a timeseries.
 *
 * @param [in] timeseries Timeseries to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_MCL_TIMESERIES_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p timeseries or @p value is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_timeseries_set_parameter(mcl_timeseries_t *timeseries, E_MCL_TIMESERIES_PARAMETER parameter, const void *value);

/**
 * This function adds @p timeseries_value_list to @p timeseries.
 *
 * @param [in] timeseries #mcl_timeseries_t object handle which @p timeseries_value_list is added to.
 * @param [in] timeseries_value_list #mcl_timeseries_value_list_t object handle which contains timeseries value list information.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p timeseries or @p timeseries_value_list is null.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_LIMIT_EXCEEDED in case @p timeseries has no space for a new time series value list.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_timeseries_add_value_list(mcl_timeseries_t *timeseries, mcl_timeseries_value_list_t *timeseries_value_list);

/**
 * This function destroys timeseries data structure.
 *
 * @param [in] timeseries Timeseries handle which is going to be destroyed.
 */
extern MCL_CONNECTIVITY_EXPORT void mcl_timeseries_destroy(mcl_timeseries_t **timeseries);

#ifdef  __cplusplus
}
#endif

#endif //MCL_TIMESERIES_H_
