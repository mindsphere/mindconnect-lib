/*!
 * @file     mcl_data_point.h
 * @brief    Data point module interface header file.
 *
 * Data points hold metadata about a specific metric that the agent generates or measures.
 *
 * This module enables the agent to initialize a data point structure using #mcl_data_point_initialize function.
 * Following initialization, the parameters of this data point can be set using #mcl_data_point_set_parameter function.
 * Then, data point(s) can be added to data source (#mcl_data_source_t) using #mcl_data_source_add_data_point function.
 *
 * If the data point is added to a data source, #mcl_data_source_t will handle the destroy operation of the data point
 * and the agent does not need to take any action. Otherwise, #mcl_data_point_destroy function is used to destroy data point.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DATA_POINT_H_
#define MCL_DATA_POINT_H_

#include "mcl_connectivity/mcl_connectivity_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for data point.
 */
typedef struct mcl_data_point_t mcl_data_point_t;

/**
 * Parameters for data point.
 */
typedef enum E_MCL_DATA_POINT_PARAMETER
{
    MCL_DATA_POINT_PARAMETER_ID,          //!< Parameter ID.
    MCL_DATA_POINT_PARAMETER_NAME,        //!< Parameter name.
    MCL_DATA_POINT_PARAMETER_DESCRIPTION, //!< Parameter description.
    MCL_DATA_POINT_PARAMETER_TYPE,        //!< Parameter type.
    MCL_DATA_POINT_PARAMETER_UNIT,        //!< Parameter unit.
    MCL_DATA_POINT_PARAMETER_CUSTOM_DATA, //!< Parameter custom data.
    MCL_DATA_POINT_PARAMETER_END
} E_MCL_DATA_POINT_PARAMETER;

/**
 * This function creates and initializes a data struct of #mcl_data_point_t.
 *
 * @param [out] data_point Data point handle which is going to be initialized.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p data_point is null.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_data_point_initialize(mcl_data_point_t **data_point);

/**
 * This function is used to set a parameter of a data point.
 *
 * @param [in] data_point Data point to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_MCL_DATA_POINT_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p data_point or @p value is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_data_point_set_parameter(mcl_data_point_t *data_point, E_MCL_DATA_POINT_PARAMETER parameter, const void *value);

/**
 * This function destroys data point data structure.
 *
 * @param [in] data_point Data point handle which is going to be destroyed.
 */
extern MCL_CONNECTIVITY_EXPORT void mcl_data_point_destroy(mcl_data_point_t **data_point);

#ifdef  __cplusplus
}
#endif

#endif //MCL_DATA_POINT_H_
