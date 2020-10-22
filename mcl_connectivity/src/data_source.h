/*!
 * @file     data_source.h
 * @brief    Data source module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DATA_SOURCE_H_
#define DATA_SOURCE_H_

#include "data_point.h"
#include "mcl_core/mcl_list.h"

/**
 * Handle for data source.
 */
typedef struct mcl_data_source_t
{
    char *name;              //!< Name of the data source.
    char *description;       //!< Description of the data source.
    mcl_list_t *data_points; //!< List of data points in the data source.
    mcl_json_t *custom_data; //!< Custom parameters for data source.
} data_source_t;

/**
 * This function checks whether all mandatory parameters of a data source are set or not.
 *
 * @param [in] data_source Data source to validate.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case data source has one or more missing mandatory parameters.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t data_source_validate(data_source_t *data_source);

#endif //DATA_SOURCE_H_
