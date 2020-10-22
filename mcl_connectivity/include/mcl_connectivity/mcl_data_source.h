/*!
 * @file     mcl_data_source.h
 * @brief    Data source module interface header file.
 *
 * Data source is a logical group that holds so called data points. See mcl_connectivity/mcl_data_point.h for data point description.
 *
 * This module enables the agent to initialize a data source structure using #mcl_data_source_initialize function.
 * Following initialization, the parameters of this data source can be set using #mcl_data_source_set_parameter function.
 * Then, data point(s) can be added to the data source using #mcl_data_source_add_data_point function.
 * Finally, the data source can be added to data source configuration using #mcl_data_source_configuration_add_data_source function.
 *
 * If the data source is added to a data source configuration, #mcl_data_source_configuration_t will handle the destroy operation
 * of the data source and the agent does not need to take any action. Otherwise, #mcl_data_source_destroy function is used to destroy data source.
 *
 * Check the example file "mcl_connectivity/examples/dsc_upload.c".
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DATA_SOURCE_H_
#define MCL_DATA_SOURCE_H_

#include "mcl_connectivity/mcl_data_point.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for data source.
 */
typedef struct mcl_data_source_t mcl_data_source_t;

/**
 * Parameters for data source.
 */
typedef enum E_MCL_DATA_SOURCE_PARAMETER
{
    MCL_DATA_SOURCE_PARAMETER_NAME,        //!< Data source name parameter as char*.
    MCL_DATA_SOURCE_PARAMETER_DESCRIPTION, //!< Data source description parameter as char*.
    MCL_DATA_SOURCE_PARAMETER_CUSTOM_DATA  //!< Data source custom data parameter as mcl_json_t*.
} E_MCL_DATA_SOURCE_PARAMETER;

/**
 * This function creates and initializes a data struct of #mcl_data_source_t.
 *
 * @param [out] data_source Data source handle which is going to be initialized.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p data_source is null.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_data_source_initialize(mcl_data_source_t **data_source);

/**
 * This function is used to set a parameter of a data source.
 *
 * @param [in] data_source Data source to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_MCL_DATA_SOURCE_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p data_source or @p value is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_data_source_set_parameter(mcl_data_source_t *data_source, E_MCL_DATA_SOURCE_PARAMETER parameter, const void *value);

/**
 * This function adds @p data_point to @p data_source.
 *
 * @param [in] data_source #mcl_data_source_t object handle which @p data_point is added to.
 * @param [in] data_point #mcl_data_point_t object handle which contains data point information.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p data_source or @p data_point is null.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_LIMIT_EXCEEDED in case @p data_source has no space for a new data point.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_data_source_add_data_point(mcl_data_source_t *data_source, mcl_data_point_t *data_point);

/**
 * This function destroys @p data_source.
 *
 * @param [in] data_source Data source handle which is going to be destroyed.
 */
extern MCL_CONNECTIVITY_EXPORT void mcl_data_source_destroy(mcl_data_source_t **data_source);

#ifdef  __cplusplus
}
#endif

#endif //MCL_DATA_SOURCE_H_
