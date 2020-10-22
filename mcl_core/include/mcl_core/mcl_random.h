/*!
 * @file     mcl_random.h
 * @brief    Random module interface header file.
 *
 * This module includes functions to generate arbitrary size random numbers and GUID.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_RANDOM_H_
#define MCL_RANDOM_H_

#include "mcl_core/mcl_core_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * This function generates globally unique identifier.
 *
 * @param [out] guid Random guid.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in random number generation.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_random_generate_guid(char **guid);

/**
 * This function generates random bytes.
 *
 * @param [out] buffer Array for random bytes.
 * @param [in]  size Size of array in bytes.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p buffer is null.</li>
 * <li>#MCL_FAIL in case of an internal error in random number generation.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_random_generate_bytes(unsigned char *buffer, mcl_size_t size);

#ifdef  __cplusplus
}
#endif

#endif //MCL_RANDOM_H_
