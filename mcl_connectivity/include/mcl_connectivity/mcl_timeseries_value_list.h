/*!
 * @file     mcl_timeseries_value_list.h
 * @brief    Timeseries value list module interface header file.
 *
 * Timeseries value list is a list of timeseries values at a specified timestamp.\n
 * See mcl_connectivity/mcl_timeseries_value.h for information about timeseries values.
 *
 * This module enables the agent to initialize a timeseries value list using #mcl_timeseries_value_list_initialize
 * function. Following initialization, the parameters of this timeseries value list can be set using
 * #mcl_timeseries_value_list_set_parameter function. Then timeseries value(s) can be added to the timeseries
 * value list using #mcl_timeseries_value_list_add_value function. If the timeseries value list is added to a
 * timeseries, #mcl_timeseries_t will handle the destroy operation of the timeseries value list and the agent
 * does not need to take any action. Otherwise, #mcl_timeseries_value_list_destroy function is used to destroy
 * timeseries value list.
 *
 * For more information, please look at example file "mcl_connectivity/examples/timeseries_upload.c".
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_TIMESERIES_VALUE_LIST_H_
#define MCL_TIMESERIES_VALUE_LIST_H_

#include "mcl_connectivity/mcl_timeseries_value.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for timeseries value list.
 */
typedef struct mcl_timeseries_value_list_t mcl_timeseries_value_list_t;

/**
 * Parameters for timeseries value list.
 */
typedef enum E_MCL_TIMESERIES_VALUE_LIST_PARAMETER
{
    MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601,  //!< Timeseries value list timestamp parameter as char* (which is in yyyy-MM-ddTHH:mm:ss.SSSZ format).
    MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_EPOCH,    //!< Timeseries value list timestamp parameter as time_t*.
    MCL_TIMESERIES_VALUE_LIST_PARAMETER_END
} E_MCL_TIMESERIES_VALUE_LIST_PARAMETER;

/**
 * This function creates and initializes a data struct of #mcl_timeseries_value_list_t.
 *
 * @param [out] timeseries_value_list Initialized #mcl_timeseries_value_list_t object handle.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p timeseries_value_list is null.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_timeseries_value_list_initialize(mcl_timeseries_value_list_t **timeseries_value_list);

/**
 * This function is used to set a parameter of a timeseries value list.
 *
 * @param [in] timeseries_value_list Timeseries value list to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_MCL_TIMESERIES_VALUE_LIST_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p timeseries_value_list or @p value is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_timeseries_value_list_set_parameter(mcl_timeseries_value_list_t *timeseries_value_list, E_MCL_TIMESERIES_VALUE_LIST_PARAMETER parameter, const void *value);

/**
 * This function adds @p timeseries_value to @p timeseries_value_list.
 *
 * @param [in] timeseries_value_list #mcl_timeseries_value_list_t object handle which @p timeseries_value is added to.
 * @param [in] timeseries_value #mcl_timeseries_value_t object handle which contains timeseries value information.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p timeseries_value_list or @p timeseries_value is null.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_LIMIT_EXCEEDED in case @p timeseries_value_list has no space for a new timeseries value.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_timeseries_value_list_add_value(mcl_timeseries_value_list_t *timeseries_value_list, mcl_timeseries_value_t *timeseries_value);

/**
 * This function destroys @p timeseries_value_list.
 *
 * @param [in] timeseries_value_list Timeseries value list handle which is going to be destroyed.
 */
extern MCL_CONNECTIVITY_EXPORT void mcl_timeseries_value_list_destroy(mcl_timeseries_value_list_t **timeseries_value_list);

#ifdef  __cplusplus
}
#endif

#endif //MCL_TIMESERIES_VALUE_LIST_H_
