/*!
 * @file     security.h
 * @brief    Security interface header file.
 *
 * This module contains security related functions like hashing, public/private key generation.
 * Implementation file might change depending on the platform and will be in security_XXX.c format.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef SECURITY_H_
#define SECURITY_H_

#include "mcl_core/mcl_core_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * This function is used to initialize security implementation.
 *
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t security_initialize(void);

/**
 * This function is used to generate the sha256 hash of the given data. @see #security_handler_hash_sha256.
 */
MCL_LOCAL mcl_error_t security_hash_sha256(const mcl_uint8_t *data, mcl_size_t data_size, mcl_uint8_t **hash, mcl_size_t *hash_size);

/**
 * This function is used to sign data with RSA key.
 *
 * Received key will be used to sign.
 *
 * @param [in] rsa_key Private key to be used in signing.
 * @param [in] data The data to be signed.
 * @param [in] data_size Size of the data.
 * @param [out] signature Generated signature.
 * @param [out] signature_size Size of signature.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t security_rsa_sign(char *rsa_key, char *data, mcl_size_t data_size, mcl_uint8_t **signature, mcl_size_t *signature_size);

/**
 * This function is used to generate the RSA public/private keys.
 *
 * Generated keys will be returned as out parameters.
 *
 * @param [out] public_key Generated public key. New memory space will be allocated for this parameter. Ownership passed to caller. Caller must free the space.
 * @param [out] private_key Generated private key. New memory space will be allocated for this parameter. Ownership passed to caller. Caller must free the space.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t security_generate_rsa_key(char **public_key, char **private_key);

/**
 * This function is used to get the modulus (n) and public exponent (e) parameters of RSA key in base64 format.
 *
 * @param public_key Public key in PEM format.
 * @param modulus Base64 encoded modulus of RSA key.
 * @param exponent Base64 encoded public exponent of RSA key.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t security_rsa_get_modulus_and_exponent(char *public_key, char **modulus, char **exponent);

/**
 * This function is used to generate random bytes.
 *
 * @param [out] buffer Buffer which will be filled with random bytes.
 * @param [in] size Size of the buffer.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t security_generate_random_bytes(unsigned char *buffer, mcl_size_t size);

#ifdef  __cplusplus
}
#endif

#endif //SECURITY_H_
