/*!
 * @file     hmac.h
 * @brief    HMAC module header file.
 *
 * This module defines several hashing functions required by other modules.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef HMAC_H_
#define HMAC_H_

#include "mcl_core/mcl_core_common.h"

/**
 * This function is used for calculating HMAC SHA256 for given data with provided secret @p key.
 * If secret @p key exceeds maximum allowed size of 64 bytes it will be reduced to 32 bytes.
 * The reduction is done by calculating SHA256 hash of the key, which returns 32 bytes.
 *
 * @param [in] data Data to calculate HMAC SHA256 for.
 * @param [in] data_size Size of @p data.
 * @param [in] key Secret key to be used during calculation.
 * @param [in] key_size Size of @p key.
 * @param [out] hash A newly allocated memory which contains the result of HMAC SHA256.
 * @param [out] hash_size Size of @p hash, which should be 32 bytes after SHA256 calculation.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER if @p key_size is zero.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_SHA256_CALCULATION_FAIL if SHA256 calculation fails.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t hmac_sha256(const mcl_uint8_t *data, mcl_size_t data_size, const mcl_uint8_t *key, mcl_size_t key_size,
    mcl_uint8_t **hash, mcl_size_t *hash_size);

#endif //HMAC_H_
