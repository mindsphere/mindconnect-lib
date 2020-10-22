/*!
 * @file     data_source_configuration.h
 * @brief    Data source configuration module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DATA_SOURCE_CONFIGURATION_H_
#define DATA_SOURCE_CONFIGURATION_H_

#include "data_source.h"
#include "item.h"

/**
 * Handle for data source configuration payload.
 */
typedef struct data_source_configuration_payload_t
{
    char *configuration_id;   //!< Unique identifier of the configuration.
    mcl_list_t *data_sources; //!< List of data sources definitions.
} data_source_configuration_payload_t;

/**
 * Handle for data source configuration.
 */
typedef struct mcl_data_source_configuration_t
{
    mcl_item_t item_base;
    data_source_configuration_payload_t *payload;
} data_source_configuration_t;

/**
 * This array is used to get the data source configuration version.
 */
extern const char *mcl_data_source_configuration_versions[];

/**
 * This function checks whether all mandatory parameters of a data source configuration are set or not.
 *
 * @param [in] data_source_configuration Data source configuration to validate.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case data source configuration has one or more missing mandatory parameters.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t data_source_configuration_validate(data_source_configuration_t *data_source_configuration);

#endif //DATA_SOURCE_CONFIGURATION_H_
