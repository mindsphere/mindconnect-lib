/*!
 * @file     deployment_configuration.h
 * @brief    Internal header for component configuration.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DEPLOYMENT_CONFIGURATION_H_
#define DEPLOYMENT_CONFIGURATION_H_

#include "mcl_core/mcl_core.h"

/**
 * Handle for the component configuration.
 */
typedef struct mcl_deployment_configuration_t
{
    mcl_core_t *core;
    char *workflow_instances_url;
} deployment_configuration_t;

/**
 * This function checks whether all mandatory parameters of a component configuration are set or not.
 *
 * @param [in] configuration Component configuration to validate.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case component configuration has one or more missing mandatory parameters.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t deployment_configuration_validate(deployment_configuration_t *configuration);

#endif //DEPLOYMENT_CONFIGURATION_H_
