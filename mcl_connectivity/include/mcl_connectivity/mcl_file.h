/*!
 * @file     mcl_file.h
 * @brief    File module interface header file.
 *
 * Agents can upload files to MindSphere. This module enables the agent to initialize a file
 * data type using #mcl_file_initialize function. Following initialization, the parameters of this
 * file data type can be set using #mcl_file_set_parameter function. Once the file data type is
 * ready, it can be uploaded to MindSphere using #mcl_connectivity_exchange function. Optionally, the
 * file can be added to #mcl_store_t and the store containing the file and possible other data can be
 * uploaded to MindSphere. If the file data type is exchanged as a single item, the agent is expected
 * to destroy the file after the exchange operation using #mcl_file_destroy function. If the file is
 * exchanged in a store, store will handle the destroy operation of the file and the agent does not
 * need to take any action.
 *
 * For more information, please look at example file "mcl_connectivity/examples/file_upload.c".
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_FILE_H_
#define MCL_FILE_H_

#include "mcl_connectivity/mcl_connectivity_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for file.
 */
typedef struct mcl_file_t mcl_file_t;

/**
 * Version of file item payload.
 */
typedef enum E_MCL_FILE_VERSION
{
    MCL_FILE_VERSION_1_0 = 0, //!< File version 1.0.
    MCL_FILE_VERSION_END      //!< End of file version enumeration.
} E_MCL_FILE_VERSION;

/**
 * Parameters for files.
 */
typedef enum E_MCL_FILE_PARAMETER
{
    MCL_FILE_PARAMETER_LOCAL_PATH = 0, //!< File local path parameter as char*.
    MCL_FILE_PARAMETER_REMOTE_NAME,    //!< File remote name parameter as char*.
    MCL_FILE_PARAMETER_TYPE,           //!< File type parameter as char*.
    MCL_FILE_PARAMETER_END
} E_MCL_FILE_PARAMETER;

/**
 * This function creates and initializes a data struct of #mcl_file_t.
 *
 * @param [in] version One of the versions listed in #E_MCL_FILE_VERSION.
 * @param [out] file File handle which is going to be initialized.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER in case meta payload version parameter is invalid.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p file is null.</li>
 * <li>#MCL_NO_FILE_SUPPORT in case there is no file system.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_file_initialize(E_MCL_FILE_VERSION version, mcl_file_t **file);

/**
 * This function is used to set a parameter of a file.
 *
 * @param [in] file File to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_MCL_FILE_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p file or @p value is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * <li>#MCL_FILE_CANNOT_BE_OPENED if file can not be opened.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * <li>#MCL_NO_FILE_SUPPORT in case there is no file system.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_file_set_parameter(mcl_file_t *file, E_MCL_FILE_PARAMETER parameter, const void *value);

/**
 * This function destroys file data structure.
 *
 * @param [in] file File handle which is going to be destroyed.
 */
extern MCL_CONNECTIVITY_EXPORT void mcl_file_destroy(mcl_file_t **file);

#ifdef  __cplusplus
}
#endif

#endif //MCL_FILE_H_
