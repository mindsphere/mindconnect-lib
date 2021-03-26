/*!
 * @file     deployment.h
 * @brief    Internal header for the component functionality.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DEPLOYMENT_H_
#define DEPLOYMENT_H_

#include "deployment_configuration.h"
#include "deployment_processor.h"

/**
 * This is the main handle for deployment component.
 */
typedef struct mcl_deployment_t
{
    deployment_configuration_t *configuration;
    deployment_processor_t processor;
} deployment_t;

#endif //DEPLOYMENT_H_
