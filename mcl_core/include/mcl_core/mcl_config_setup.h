/*!
 * @file     mcl_config_setup.h
 * @brief    Configuration setup module interface header file.
 *
 * Configuration setup header for MCL.\n
 * This configuration setup should be used by every module which requires external libraries.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_CONFIG_SETUP_H_
#define MCL_CONFIG_SETUP_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#if (defined(_WIN32) || defined(__WIN32__) || defined(_WIN32_WINNT)) && !defined(WIN32)
#define WIN32
#endif

#if (defined(_WIN64) || defined(__WIN64__) || defined(_WIN64_WINNT)) && !defined(WIN64)
#define WIN64
#endif

#ifndef MCL_CORE_EXPORT
#if MCL_STATICLIB
    #define MCL_CORE_EXPORT
#elif defined(WIN32) || defined(WIN64)
    #if MCL_CORE_BUILD
        #define MCL_CORE_EXPORT  __declspec(dllexport)
    #else
        #define MCL_CORE_EXPORT  __declspec(dllimport)
    #endif
#else
    #define MCL_CORE_EXPORT
#endif
#endif

#ifndef MCL_LOCAL
#if (MCL_STATICLIB || defined(WIN32) || defined(WIN64))
    #define MCL_LOCAL
#else
    #define MCL_LOCAL __attribute__ ((visibility ("hidden")))
#endif
#endif

#include "mcl_core/mcl_config.h"
#include "mcl_core/mcl_version.h"

#ifdef  __cplusplus
}
#endif

#endif //MCL_CONFIG_SETUP_H_
