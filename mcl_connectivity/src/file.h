/*!
 * @file     file.h
 * @brief    File module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef FILE_H_
#define FILE_H_

#include "item.h"
#include "mcl_core/mcl_time_util.h"

/**
 * Handle for file payload.
 */
typedef struct file_payload_t
{
    char *creation_date;   //!< Creation date of file.
    char *local_path;      //!< Local path of file.
    char *remote_name;     //!< Remote name of file.
    char *type;            //!< Type of file.
    mcl_size_t size;       //!< Size of file.
    void *file_descriptor; //!< Descriptor of file.
} file_payload_t;

/**
 * Handle for file.
 */
typedef struct mcl_file_t
{
    mcl_item_t item_base;    //!< Item base of file.
    file_payload_t *payload; //!< Payload of file.
} file_t;

/**
 * This function checks whether all mandatory parameters of a file are set or not.
 *
 * @param [in] file File to validate.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case file has one or more missing mandatory parameters.</li>
 * <li>#MCL_NO_FILE_SUPPORT in case there is no file system.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t file_validate(file_t *file);

extern const char *mcl_file_versions[];

#endif //FILE_H_
