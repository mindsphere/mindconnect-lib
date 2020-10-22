/*!
 * @file     data_lake.h
 * @brief    Data lake module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DATA_LAKE_H_
#define DATA_LAKE_H_

#include "mcl_data_lake/mcl_data_lake_configuration.h"
#include "data_lake_processor.h"

/**
 * Handle for data lake.
 */
typedef struct mcl_data_lake_t
{
    mcl_data_lake_configuration_t *configuration;   //!< Data structure holding the data lake configuration.
    data_lake_processor_t processor;                //!< Data lake processor handle.
} data_lake_t;

#endif //DATA_LAKE_H_
