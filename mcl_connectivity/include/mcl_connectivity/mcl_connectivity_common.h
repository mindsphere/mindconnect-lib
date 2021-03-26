/*!
 * @file     mcl_connectivity_common.h
 * @brief    Common module interface header file.
 *
 * This module contains common type definitions used in various MCL Connectivity modules.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_CONNECTIVITY_COMMON_H_
#define MCL_CONNECTIVITY_COMMON_H_

#include "mcl_core/mcl_assert.h"

#ifdef  __cplusplus
extern "C"
{
#endif

#ifndef MCL_CONNECTIVITY_EXPORT
#if MCL_STATICLIB
    #define MCL_CONNECTIVITY_EXPORT
#elif defined(WIN32) || defined(WIN64)
    #if MCL_CONNECTIVITY_BUILD
        #define MCL_CONNECTIVITY_EXPORT  __declspec(dllexport)
    #else
        #define MCL_CONNECTIVITY_EXPORT  __declspec(dllimport)
    #endif
#else
    #define MCL_CONNECTIVITY_EXPORT
#endif
#endif

/**
 * MCL connectivity return code definitions. Every function returning a status code uses this enum values.
 */
typedef enum E_MCL_CONNECTIVITY_RETURN_CODE
{
    MCL_FILE_CANNOT_BE_OPENED = MCL_CORE_RETURN_CODE_END,   //!< File can not be opened.
    MCL_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE,                 //!< Item exceeds max http payload request.
    MCL_STORE_IS_EMPTY,                                     //!< The store trying to be exchanged has no data inside.
    MCL_CONNECTIVITY_RETURN_CODE_END                        //!< End of return codes.
} E_MCL_CONNECTIVITY_RETURN_CODE;

extern MCL_CONNECTIVITY_EXPORT const char *mcl_connectivity_return_code_strings[MCL_CONNECTIVITY_RETURN_CODE_END - MCL_CORE_RETURN_CODE_END];

// This function converts the given return code to its string value for connectivity module.
#define MCL_CONNECTIVITY_CODE_TO_STRING(code) (code >= MCL_CORE_RETURN_CODE_END) ? mcl_connectivity_return_code_strings[code - MCL_CORE_RETURN_CODE_END] : MCL_CORE_CODE_TO_STRING(code)

#ifdef  __cplusplus
}
#endif

#endif //MCL_CONNECTIVITY_COMMON_H_
