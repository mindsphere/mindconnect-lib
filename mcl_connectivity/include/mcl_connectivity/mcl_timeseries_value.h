/*!
 * @file     mcl_timeseries_value.h
 * @brief    Timeseries value module interface header file.
 *
 * Timeseries value is a single measurement of a data point, #mcl_data_point_t.
 *
 * This module enables the agent to initialize a timeseries value using
 * #mcl_timeseries_value_initialize function. Following initialization, the parameters
 * of this timeseries value can be set using #mcl_timeseries_value_set_parameter
 * function. Then timeseries value(s) can be added to the timeseries value list using
 * #mcl_timeseries_value_list_add_value function. See mcl_connectivity/mcl_timeseries_value_list.h for
 * information about timeseries value list. If the timeseries value is added to a timeseries
 * value list, #mcl_timeseries_value_list_t will handle the destroy operation of the timeseries
 * value and the agent does not need to take any action. Otherwise, #mcl_timeseries_value_destroy
 * function is used to destroy timeseries value.
 *
 * For more information, please look at example file "mcl_connectivity/examples/timeseries_upload.c".
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_TIMESERIES_VALUE_H_
#define MCL_TIMESERIES_VALUE_H_

#include "mcl_connectivity/mcl_connectivity_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for timeseries value.
 */
typedef struct mcl_timeseries_value_t mcl_timeseries_value_t;

/**
 * Parameters for timeseries value.
 */
typedef enum E_MCL_TIMESERIES_VALUE_PARAMETER
{
    MCL_TIMESERIES_VALUE_PARAMETER_DATA_POINT_ID, //!< Timeseries value data point id parameter as char*.
    MCL_TIMESERIES_VALUE_PARAMETER_VALUE,         //!< Timeseries value value parameter as char*.
    MCL_TIMESERIES_VALUE_PARAMETER_QUALITY_CODE,  //!< Timeseries value quality code parameter as char*.
    MCL_TIMESERIES_VALUE_PARAMETER_END
} E_MCL_TIMESERIES_VALUE_PARAMETER;

/**
 * This function creates and initializes a data struct of #mcl_timeseries_value_t.
 *
 * @param [out] timeseries_value Initialized #mcl_timeseries_value_t object handle.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p timeseries_value is null.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_timeseries_value_initialize(mcl_timeseries_value_t **timeseries_value);

/**
 * This function is used to set a parameter of a timeseries value.
 *
 * @param [in] timeseries_value Timeseries value to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_MCL_TIMESERIES_VALUE_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p timeseries_value or @p value is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_timeseries_value_set_parameter(mcl_timeseries_value_t *timeseries_value, E_MCL_TIMESERIES_VALUE_PARAMETER parameter, const void *value);

/**
 * This function destroys @p timeseries_value.
 *
 * @param [in] timeseries_value #mcl_timeseries_value_t object handle that will be destroyed.
 */
extern MCL_CONNECTIVITY_EXPORT void mcl_timeseries_value_destroy(mcl_timeseries_value_t **timeseries_value);

#ifdef  __cplusplus
}
#endif

#endif //MCL_TIMESERIES_VALUE_H_
