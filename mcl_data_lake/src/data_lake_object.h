/*!
 * @file     data_lake_object.h
 * @brief    Data lake object module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DATA_LAKE_OBJECT_H_
#define DATA_LAKE_OBJECT_H_

#include "mcl_data_lake/mcl_data_lake_object.h"

/**
 * Handle for data lake object.
 */
typedef struct mcl_data_lake_object_t
{
    mcl_data_lake_upload_callback upload_callback;      //!< Callback used by http client to copy object data to http request.
    void *user_context;                                 //!< Context which will be passed to the callback.
    char *path;                                         //!< Object path in Mindsphere.
    char *signed_url;                                   //!< Signed URL for the object.
    mcl_size_t size;                                    //!< Size of the object.
} data_lake_object_t;

/**
 * Internal parameters for data lake object.
 */
typedef enum E_DATA_LAKE_OBJECT_INTERNAL_PARAMETER
{
    DATA_LAKE_OBJECT_INTERNAL_PARAMETER_SIGNED_URL = MCL_DATA_LAKE_OBJECT_PARAMETER_END,            //!< Signed url parameter as char *.
    DATA_LAKE_OBJECT_INTERNAL_PARAMETER_END
} E_DATA_LAKE_OBJECT_INTERNAL_PARAMETER;

/**
 * This function is used to set an internal parameter of a data lake object.
 *
 * @param [in] object Data lake object to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_DATA_LAKE_OBJECT_INTERNAL_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t data_lake_object_set_internal_parameter(mcl_data_lake_object_t *object, E_DATA_LAKE_OBJECT_INTERNAL_PARAMETER parameter, const void *value);

/**
 * This function checks whether all mandatory parameters of a data lake object are set or not.
 *
 * @param [in] object Data lake object to validate.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case data lake object has one or more missing mandatory parameters.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t data_lake_object_validate(data_lake_object_t *object);

#endif //DATA_LAKE_OBJECT_H_
