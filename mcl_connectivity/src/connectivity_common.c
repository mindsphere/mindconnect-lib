/*!
 * @file     connectivity_common.c
 * @brief    Strings for connectivity error codes.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "connectivity_common.h"

const char *mcl_connectivity_return_code_strings[MCL_CONNECTIVITY_RETURN_CODE_END - MCL_CORE_RETURN_CODE_END] =
{
    "MCL_FILE_CANNOT_BE_OPENED",
    "MCL_TOO_MANY_REQUESTS",
    "MCL_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE",
    "MCL_STORE_IS_EMPTY"
};
