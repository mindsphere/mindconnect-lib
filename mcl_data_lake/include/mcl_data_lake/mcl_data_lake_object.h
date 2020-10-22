/*!
 * @file     mcl_data_lake_object.h
 * @brief    Data Lake object interface header file.
 *
 * This interface introduces data lake object structure.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DATA_LAKE_OBJECT_H_
#define MCL_DATA_LAKE_OBJECT_H_

#include "mcl_data_lake/mcl_data_lake_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Callback function prototype used by http client to copy object data to http request.
 * The function will be called repeatedly until the function returns zero.
 *
 * @param [in] buffer Destination address for object.
 * @param [in] size Size, in bytes, of each element to be read.
 * @param [in] count Number of elements, each one with a size of @p size bytes.
 * @param [in] user_context Source address for object.
 * @return The total number of bytes successfully written.
 */
typedef mcl_size_t (*mcl_data_lake_upload_callback)(char *buffer, mcl_size_t size, mcl_size_t count, void *user_context);

/**
 * Handle for data lake object.
 */
typedef struct mcl_data_lake_object_t mcl_data_lake_object_t;

/**
 * Parameters for data lake object.
 */
typedef enum E_MCL_DATA_LAKE_OBJECT_PARAMETER
{
    MCL_DATA_LAKE_OBJECT_PARAMETER_PATH = 0,            //!< Remote path for data lake object as char*.
    MCL_DATA_LAKE_OBJECT_PARAMETER_SIZE,                //!< Size of the data lake object in bytes as mcl_size_t*.
    MCL_DATA_LAKE_OBJECT_PARAMETER_UPLOAD_CALLBACK,     //!< Callback as #mcl_data_lake_upload_callback, which is used to upload data lake object.
    MCL_DATA_LAKE_OBJECT_PARAMETER_USER_CONTEXT,        //!< Context as void* which will be passed to the callback.
    MCL_DATA_LAKE_OBJECT_PARAMETER_END
} E_MCL_DATA_LAKE_OBJECT_PARAMETER;

/**
 * This function creates and initializes a data struct of #mcl_data_lake_object_t.
 *
 * @param [out] object Object to initialize.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY if memory allocation fails.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case input parameter is NULL.</li>
 * </ul>
 */
extern MCL_DATA_LAKE_EXPORT mcl_error_t mcl_data_lake_object_initialize(mcl_data_lake_object_t **object);

/**
 * This function is used to set a parameter of a data lake object.
 *
 * @param [in] object Data lake object to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_MCL_DATA_LAKE_OBJECT_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p object or @p value is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_DATA_LAKE_EXPORT mcl_error_t mcl_data_lake_object_set_parameter(mcl_data_lake_object_t *object, E_MCL_DATA_LAKE_OBJECT_PARAMETER parameter, const void *value);

/**
 * This function destroys data lake object structure.
 *
 * @param [in] object Object to be destroyed.
 */
extern MCL_DATA_LAKE_EXPORT void mcl_data_lake_object_destroy(mcl_data_lake_object_t **object);

#ifdef  __cplusplus
}
#endif

#endif //MCL_DATA_LAKE_OBJECT_H_
