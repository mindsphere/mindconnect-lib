/*!
 * @file     mcl_data_source_configuration.h
 * @brief    Data source configuration module interface header file.
 *
 * If an agent needs to upload timeseries data to MindSphere, MindSphere needs additional configuration
 * to know how to interpret the agent's data stream. This is called data source configuration.
 * Before uploading any timeseries, data source configuration must be uploaded to MindSphere.
 *
 * This module enables the agent to initialize a data source configuration using #mcl_data_source_configuration_initialize
 * function. Following initialization, a unique configuration id is assigned (which can be generated using
 * #mcl_random_generate_guid function) to the data source configuration using #mcl_data_source_configuration_set_parameter function.
 * This configuration id must be kept for the related timeseries exchange. Finally, the data source(s) can be added
 * to data source configuration using #mcl_data_source_configuration_add_data_source function.
 *
 * Data source configuration is destroyed using #mcl_data_source_configuration_destroy funtion.
 *
 * Check the example file "mcl_connectivity/examples/dsc_upload.c".
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DATA_SOURCE_CONFIGURATION_H_
#define MCL_DATA_SOURCE_CONFIGURATION_H_

#include "mcl_connectivity/mcl_data_source.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for data source configuration.
 */
typedef struct mcl_data_source_configuration_t mcl_data_source_configuration_t;

/**
 * Parameters for data source configuration.
 */
typedef enum E_MCL_DATA_SOURCE_CONFIGURATION_PARAMETER
{
    MCL_DATA_SOURCE_CONFIGURATION_PARAMETER_ID //!< Data source configuration ID parameter as char*.
} E_MCL_DATA_SOURCE_CONFIGURATION_PARAMETER;

/**
 * Version enumeration for data source configuration.
 */
typedef enum E_MCL_DATA_SOURCE_CONFIGURATION_VERSION
{
    MCL_DATA_SOURCE_CONFIGURATION_1_0 = 0, //!< Data source configuration version 1.0.
    MCL_DATA_SOURCE_CONFIGURATION_END      //!< End of data source configuration version.
} E_MCL_DATA_SOURCE_CONFIGURATION_VERSION;

/**
 * This function initializes @p data_source_configuration.
 *
 * @param [in] version Version of the data source configuration.
 * @param [out] data_source_configuration Initialized #mcl_data_source_configuration_t object handle.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p data_source_configuration is null.</li>
 * <li>#MCL_INVALID_PARAMETER in case @p version is invalid.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_data_source_configuration_initialize(E_MCL_DATA_SOURCE_CONFIGURATION_VERSION version, mcl_data_source_configuration_t **data_source_configuration);

/**
 * This function is used to set a parameter of a data source configuration.
 *
 * @param [in] data_source_configuration Data source configuration to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_MCL_DATA_SOURCE_CONFIGURATION_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p data_source_configuration or @p value is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_data_source_configuration_set_parameter(mcl_data_source_configuration_t *data_source_configuration, E_MCL_DATA_SOURCE_CONFIGURATION_PARAMETER parameter, const void *value);

/**
 * This function adds @p data_source to @p data_source_configuration.
 *
 * @param [in] data_source_configuration #mcl_data_source_configuration_t object handle which @p data_source is added to.
 * @param [in] data_source #mcl_data_source_t object handle which contains data source information.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p data_source_configuration or @p data_source is null.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_LIMIT_EXCEEDED in case @p data_source_configuration has no space for a new data source.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_data_source_configuration_add_data_source(mcl_data_source_configuration_t *data_source_configuration, mcl_data_source_t *data_source);

/**
 * This function destroys @p data_source_configuration.
 *
 * @param [in] data_source_configuration #mcl_data_source_configuration_t object handle that will be destroyed.
 */
extern MCL_CONNECTIVITY_EXPORT void mcl_data_source_configuration_destroy(mcl_data_source_configuration_t **data_source_configuration);

#ifdef  __cplusplus
}
#endif

#endif //MCL_DATA_SOURCE_CONFIGURATION_H_
