/*!
 * @file     data_lake_json.h
 * @brief    Data lake json module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DATA_LAKE_JSON_H_
#define DATA_LAKE_JSON_H_

#include "data_lake_object.h"

/**
 * This function generates a json string from an array of datalake objects.
 *
 * @param [in] object_array Array of data lake objects.
 * @param [in] array_size Size of array.
 * @param [in] client_id Client ID.
 * @param [in] subtenant_id Subtenant ID.
 * @param [out] json Json body for the request which is needed to generate upload urls.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER in case there is no path to generate upload url.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
mcl_error_t data_lake_json_from_objects(data_lake_object_t **object_array, mcl_size_t array_size, const char *client_id, const char *subtenant_id, char **json);

/**
 * This function is used to match signed urls with objects.
 *
 * @param [in] object_array Array of data lake objects.
 * @param [in] array_size Size of array.
 * @param [in] json Json string to parse, does not need to be NULL terminated.
 * @param [in] json_size Size of json string.
 * @param [in] client_id_length Length of Client ID.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_DATA_LAKE_SIGNED_URL_GENERATION_FAIL if signed URL could not be retrieved for all data lake objects with non-NULL paths.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
mcl_error_t data_lake_json_match_signed_urls_with_objects(data_lake_object_t **object_array, mcl_size_t array_size, char *json,
    mcl_size_t json_size, mcl_size_t client_id_length);

#endif //DATA_LAKE_JSON_H_
