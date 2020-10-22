/*!
 * @file     timeseries.h
 * @brief    Timeseries module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef TIMESERIES_H_
#define TIMESERIES_H_

#include "timeseries_value_list.h"
#include "item.h"

/**
 * Handle for timeseries paylaod.
 */
typedef struct timeseries_payload_t
{
    char *configuration_id;  //!< Configuration ID of timeseries.
    mcl_list_t *value_lists; //!< List of timeseries value lists.
} timeseries_payload_t;

/**
 * Handle for timeseries.
 */
typedef struct mcl_timeseries_t
{
    mcl_item_t item_base;          //!< Item base of timeseries.
    timeseries_payload_t *payload; //!< Payload of timeseries.
} timeseries_t;

extern const char *mcl_timeseries_versions[];

/**
 * This function checks whether all mandatory parameters of a timeseries are set or not.
 *
 * @param [in] timeseries Timeseries to validate.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case timeseries has one or more missing mandatory parameters.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t timeseries_validate(timeseries_t *timeseries);

#endif //TIMESERIES_H_
