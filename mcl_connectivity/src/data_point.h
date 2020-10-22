/*!
 * @file     data_point.h
 * @brief    Data point module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DATA_POINT_H_
#define DATA_POINT_H_

#include "mcl_core/mcl_json_util.h"

/**
 * Handle for data point.
 */
typedef struct mcl_data_point_t
{
    char *id;                //!< Agent-unique identifier of the data point.
    char *name;              //!< Name of the data point.
    char *description;       //!< Description of the data point.
    char *type;              //!< Type of the data point.
    char *unit;              //!< Measurement unit of the data point.
    mcl_json_t *custom_data; //!< Custom parameters for the data point.
} data_point_t;

/**
 * This function checks whether all mandatory parameters of a data point are set or not.
 *
 * @param [in] data_point Data point to validate.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case data point has one or more missing mandatory parameters.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t data_point_validate(data_point_t *data_point);

#endif //DATA_POINT_H_
