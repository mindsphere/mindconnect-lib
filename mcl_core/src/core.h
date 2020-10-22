/*!
 * @file     core.h
 * @brief    Core module header file.
 *
 * This module implements MCL core interface.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef CORE_H_
#define CORE_H_

#include "core_processor.h"

/**
 * This is the main structure to be used for communication with MindSphere.
 */
typedef struct mcl_core_t
{
    core_configuration_t *configuration;        //!< Core configuration handle.
    core_processor_t *core_processor;           //!< Core processor handle.
} core_t;

#endif //CORE_H_
