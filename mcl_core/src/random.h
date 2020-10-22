/*!
 * @file     random.h
 * @brief    Random module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef RANDOM_H_
#define RANDOM_H_

#include "mcl_core/mcl_core_common.h"

/**
 * This function generates random integer number.
 *
 * @param [out] random_number Pointer to the random number generated.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of an internal error in random number generation.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t random_generate_number(mcl_uint32_t *random_number);

/**
 * This function generates random guid.
 *
 * @param [out] guid Random guid.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in random number generation.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t random_generate_guid(char **guid);

#endif //RANDOM_H_
