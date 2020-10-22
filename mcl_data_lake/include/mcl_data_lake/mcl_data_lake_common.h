/*!
 * @file     mcl_data_lake_common.h
 * @brief    Data lake common module interface header file.
 *
 * This module contains common type definitions used in various MCL Data Lake modules.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DATA_LAKE_COMMON_H_
#define MCL_DATA_LAKE_COMMON_H_

#include "mcl_core/mcl_assert.h"

#ifdef  __cplusplus
extern "C"
{
#endif

#ifndef MCL_DATA_LAKE_EXPORT
#if MCL_STATICLIB
    #define MCL_DATA_LAKE_EXPORT
#elif defined(WIN32) || defined(WIN64)
    #if MCL_DATA_LAKE_BUILD
        #define MCL_DATA_LAKE_EXPORT  __declspec(dllexport)
    #else
        #define MCL_DATA_LAKE_EXPORT  __declspec(dllimport)
    #endif
#else
    #define MCL_DATA_LAKE_EXPORT
#endif
#endif

 /**
  * MCL Data Lake return code definitions.
  */
typedef enum E_MCL_DATA_LAKE_RETURN_CODE
{
    MCL_DATA_LAKE_SIGNED_URL_GENERATION_FAIL = MCL_CORE_RETURN_CODE_END,    //!< Could not get signed url for all object paths.
    MCL_DATA_LAKE_RETURN_CODE_END                                           //!< End of return codes.
} E_MCL_DATA_LAKE_RETURN_CODE;

extern MCL_DATA_LAKE_EXPORT const char *mcl_data_lake_return_code_strings[MCL_DATA_LAKE_RETURN_CODE_END - MCL_CORE_RETURN_CODE_END];

// This function converts the given return code to its string value for data lake module.
#define MCL_DATA_LAKE_CODE_TO_STRING(code) (code >= MCL_CORE_RETURN_CODE_END) ? mcl_data_lake_return_code_strings[code - MCL_CORE_RETURN_CODE_END] : MCL_CORE_CODE_TO_STRING(code)

#ifdef  __cplusplus
}
#endif

#endif //MCL_DATA_LAKE_COMMON_H_
