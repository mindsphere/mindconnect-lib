/*!
 * @file     connectivity.h
 * @brief    Connectivity module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef CONNECTIVITY_H_
#define CONNECTIVITY_H_

#include "mcl_connectivity/mcl_connectivity.h"
#include "connectivity_processor.h"

/**
 * This is the main handle for connectivity component.
 */
typedef struct mcl_connectivity_t
{
    mcl_connectivity_configuration_t *configuration;
    connectivity_processor_t processor;
} connectivity_t;

#endif //CONNECTIVITY_H_
