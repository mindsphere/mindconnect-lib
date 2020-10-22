/*!
 * @file     security_mbedtls.c
 * @brief    Security module implementation file.
 *
 * @copyright Copyright (C) 2020 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_memory.h"
#include "mbedtls/sha256.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

#define SHA256_DIGEST_LENGTH    32

#define KEY_LENGTH_BITS 3072

static mbedtls_entropy_context entropy;
static mbedtls_ctr_drbg_context ctr_drbg;
static mcl_bool_t is_initialized = MCL_FALSE;

#ifdef MBEDTLS_ENTROPY_HARDWARE_ALT
extern int mbedtls_hardware_poll( void *data, unsigned char *output, size_t len, size_t *olen );
#endif

mcl_error_t security_initialize(void)
{
    MCL_DEBUG_ENTRY("void");

	if(!is_initialized)
	{
		mbedtls_entropy_init(&entropy);
#ifdef MBEDTLS_ENTROPY_HARDWARE_ALT
        (void) mbedtls_entropy_add_source(&entropy, mbedtls_hardware_poll, (void*)NULL, 1, MBEDTLS_ENTROPY_SOURCE_STRONG);    
#endif
        (void) mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, MCL_NULL, 0);
        is_initialized = MCL_TRUE;
	}

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

mcl_error_t security_hash_sha256(const mcl_uint8_t *data, mcl_size_t data_size, mcl_uint8_t **hash, mcl_size_t *hash_size)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("const mcl_uint8_t *data = <%p>, mcl_size_t data_size = <%u>, mcl_uint8_t **hash = <%p>, mcl_size_t *hash_size = <%p>", data, data_size, hash, hash_size);

    *hash_size = 0;

    // Allocate memory to store SHA256 result.
    *hash = MCL_CALLOC(1, SHA256_DIGEST_LENGTH);
    
    if (MCL_NULL == *hash)
    {
        code = MCL_OUT_OF_MEMORY;
    }
    else
    {
        *hash_size = SHA256_DIGEST_LENGTH;

        // Perform SHA256 calculation.
        MCL_DEBUG("Calculating SHA256 with mbedTLS.");
        mbedtls_sha256(data, data_size, *hash, 0);
    }
    
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t security_rsa_sign(char *rsa_key, char *data, mcl_size_t data_size, mcl_uint8_t **signature, mcl_size_t *signature_size)
{
    return MCL_FAIL;
}

mcl_error_t security_generate_rsa_key(char **public_key, char **private_key)
{
    return MCL_FAIL;
}

mcl_error_t security_rsa_get_modulus_and_exponent(char *public_key, char **modulus, char **exponent)
{
    return MCL_FAIL;
}

mcl_error_t security_generate_random_bytes(unsigned char *buffer, mcl_size_t size)
{
    mcl_error_t code = MCL_FAIL;
    
    if(is_initialized)
    {
        code = (0 == mbedtls_ctr_drbg_random(&ctr_drbg, buffer, size)) ? MCL_OK : MCL_FAIL;
    }
    
    return code;
}
