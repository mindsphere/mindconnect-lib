/*!
 * @file     mcl_deployment_common.h
 * @brief    Header for component commons.
 *
 * This module contains common type definitions used in various MCL Deployment component modules.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DEPLOYMENT_COMMON_H_
#define MCL_DEPLOYMENT_COMMON_H_

#include "mcl_core/mcl_assert.h"

#ifdef  __cplusplus
extern "C"
{
#endif

#ifndef MCL_DEPLOYMENT_EXPORT
#if MCL_STATICLIB
    #define MCL_DEPLOYMENT_EXPORT
#elif defined(WIN32) || defined(WIN64)
    #if MCL_DEPLOYMENT_BUILD
        #define MCL_DEPLOYMENT_EXPORT  __declspec(dllexport)
    #else
        #define MCL_DEPLOYMENT_EXPORT  __declspec(dllimport)
    #endif
#else
    #define MCL_DEPLOYMENT_EXPORT
#endif
#endif

 /**
  * MCL Deployment component functions' return code definitions.
  */
typedef enum E_MCL_DEPLOYMENT_RETURN_CODE
{
    MCL_DEPLOYMENT_FAIL = MCL_CORE_RETURN_CODE_END,    //!< General fail code for MCL Deployment component.
    MCL_DEPLOYMENT_RETURN_CODE_END                     //!< End of return codes.
} E_MCL_DEPLOYMENT_RETURN_CODE;

extern MCL_DEPLOYMENT_EXPORT const char *mcl_deployment_return_code_strings[MCL_DEPLOYMENT_RETURN_CODE_END - MCL_CORE_RETURN_CODE_END];

// This function converts the given return code to its string value for this component.
#define MCL_DEPLOYMENT_CODE_TO_STRING(code) (code >= MCL_CORE_RETURN_CODE_END) ? mcl_deployment_return_code_strings[code - MCL_CORE_RETURN_CODE_END] : MCL_CORE_CODE_TO_STRING(code)

#ifdef  __cplusplus
}
#endif

#endif //MCL_DEPLOYMENT_COMMON_H_
