/*!
 * @file     mapping.h
 * @brief    Mapping module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MAPPING_H_
#define MAPPING_H_

#include "mcl_core/mcl_core_common.h"

/**
 * Handle for mapping.
 */
typedef struct mcl_mapping_t
{
    char *data_point_id;        //!< Mapping ID.
    char *entity_id;            //!< Entity ID.
    char *property_set_name;    //!< Property set name.
    char *property_name;        //!< Property name.
    mcl_bool_t keep_mapping;    //!< Custom parameters for the mapping.
} mapping_t;

/**
 * This function checks whether all mandatory parameters of a mapping are set or not.
 *
 * @param [in] mapping Mapping to validate.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case mapping has one or more missing mandatory parameters.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t mapping_validate(mapping_t *mapping);

#endif //MAPPING_H_
