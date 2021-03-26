/*!
 * @file     data_lake_configuration.h
 * @brief    Data lake configuration module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DATA_LAKE_CONFIGURATION_H_
#define DATA_LAKE_CONFIGURATION_H_

#include "mcl_core/mcl_core.h"

/**
 * Data lake configuration struct.
 */
typedef struct mcl_data_lake_configuration_t
{
    mcl_core_t *core;
    char *upload_url_generation_url;
    char *certificate;
    mcl_bool_t certificate_is_file;
} data_lake_configuration_t;

/**
 * This function checks whether all mandatory parameters of a data lake configuration are set or not.
 *
 * @param [in] configuration Data lake configuration to validate.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case data lake configuration has one or more missing mandatory parameters.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t data_lake_configuration_validate(data_lake_configuration_t *configuration);

#endif //DATA_LAKE_CONFIGURATION_H_
