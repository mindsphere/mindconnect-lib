/*!
 * @file     timeseries_value_list.h
 * @brief    Timeseries value list header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef TIMESERIES_VALUE_LIST_H_
#define TIMESERIES_VALUE_LIST_H_

#include "timeseries_value.h"
#include "mcl_core/mcl_list.h"
#include "mcl_core/mcl_time_util.h"

/**
 * Handle for timeseries value list.
 */
typedef struct mcl_timeseries_value_list_t
{
    mcl_list_t *values;                     //!< List of timeseries values.
    char timestamp[MCL_TIMESTAMP_LENGTH];   //!< Time of values in yyyy-MM-ddTHH:mm:ss.SSSZ format.
} timeseries_value_list_t;

/**
 * This function checks whether all mandatory parameters of a timeseries value list are set or not.
 *
 * @param [in] timeseries_value_list Timeseries value list to validate.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case timeseries value list has one or more missing mandatory parameters.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t timeseries_value_list_validate(timeseries_value_list_t *timeseries_value_list);

#endif //TIMESERIES_VALUE_LIST_H_
