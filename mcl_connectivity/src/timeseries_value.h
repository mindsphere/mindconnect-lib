/*!
 * @file     timeseries_value.h
 * @brief    Timeseries value module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef TIMESERIES_VALUE_H_
#define TIMESERIES_VALUE_H_

#include "mcl_connectivity/mcl_connectivity_common.h"

/**
 * Handle for timeseries value.
 */
typedef struct mcl_timeseries_value_t
{
    char *data_point_id; //!< Data point id of the timeseries value.
    char *quality_code;  //!< Quality code of the timeseries value.
    char *value;         //!< Value of the timeseries value.
} timeseries_value_t;

/**
 * This function checks whether all mandatory parameters of a timeseries value are set or not.
 *
 * @param [in] timeseries_value Timeseries value to validate.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case timeseries value has one or more missing mandatory parameters.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t timeseries_value_validate(timeseries_value_t *timeseries_value);

#endif //TIMESERIES_VALUE_H_
