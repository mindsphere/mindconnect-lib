/*!
 * @file     connectivity_configuration.h
 * @brief    Connectivity configuration module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef CONNECTIVITY_CONFIGURATION_H_
#define CONNECTIVITY_CONFIGURATION_H_

#include "mcl_core/mcl_core.h"

typedef struct mcl_connectivity_configuration_t
{
    mcl_core_t *core;
    char *exchange_url;
    char *mapping_url;
    mcl_size_t max_http_payload_size;
} connectivity_configuration_t;

/**
 * This function checks whether all mandatory parameters of a connectivity configuration are set or not.
 *
 * @param [in] configuration Connectivity configuration to validate.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case Connectivity configuration has one or more missing mandatory parameter.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t connectivity_configuration_validate(connectivity_configuration_t *configuration);

#endif //CONNECTIVITY_CONFIGURATION_H_
