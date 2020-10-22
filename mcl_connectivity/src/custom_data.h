/*!
 * @file     custom_data.h
 * @brief    Custom data module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef CUSTOM_DATA_H_
#define CUSTOM_DATA_H_

#include "item.h"
#include "mcl_core/mcl_json_util.h"

/**
 * Handle for custom data payload.
 */
typedef struct custom_data_payload_t
{
    char *version;             //!< Version of custom data.
    const mcl_uint8_t *buffer; //!< Buffer of custom data.
    char *content_type;        //!< Content type of custom data.
    char *type;                //!< Type of custom data.
    mcl_size_t size;           //!< Size of custom data.
    mcl_json_t *details;       //!< Details of custom data.
} custom_data_payload_t;

/**
 * Handle for custom data.
 */
typedef struct mcl_custom_data_t
{
    mcl_item_t item_base;           //!< Item base of custom data.
    custom_data_payload_t *payload; //!< Payload of custom data.
} custom_data_t;

/**
 * This function checks whether all mandatory parameters of a custom data are set or not.
 *
 * @param [in] custom_data Custom data to validate.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case custom data has one or more missing mandatory parameter.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t custom_data_validate(custom_data_t *custom_data);

#endif //CUSTOM_DATA_H_
