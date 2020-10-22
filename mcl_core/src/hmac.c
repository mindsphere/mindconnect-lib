/*!
 * @file     hmac.c
 * @brief    HMAC module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "hmac.h"
#include "security.h"
#include "string_util.h"
#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_memory.h"

// Allowed maximum key size must be greater than SHA256 results size.
#define HMAC_MAXIMUM_KEY_SIZE 64
#define HMAC_SHA256_SIZE 32
static const mcl_uint8_t hmac_ipad = 0x36;
static const mcl_uint8_t hmac_opad = 0x5C;

mcl_error_t hmac_sha256(const mcl_uint8_t *data, mcl_size_t data_size, const mcl_uint8_t *key, mcl_size_t key_size, mcl_uint8_t **hash, mcl_size_t *hash_size)
{
    const mcl_uint8_t *key_local = key;
    mcl_error_t code;
    mcl_size_t key_local_size = key_size;
    mcl_size_t index = 0;
    mcl_size_t key_ipad_size;
    mcl_uint8_t *key_ipad;
    mcl_size_t key_opad_size;
    mcl_uint8_t *key_opad;
    mcl_uint8_t *sha_output = MCL_NULL;
    mcl_size_t sha_output_size;

    // If the key is too long, replace it by its hash sha256.
    mcl_uint8_t *key_resized = MCL_NULL;

    MCL_DEBUG_ENTRY("const mcl_uint8_t *data = <%p>, mcl_size_t data_size = <%u>, const mcl_uint8_t *key = <%p>, mcl_size_t key_size = <%u>, mcl_uint8_t **hash = <%p>, mcl_size_t *hash_size = <%p>",
                data, data_size, key, key_size, hash, hash_size);

    MCL_ASSERT_CODE_MESSAGE(key_size > 0, MCL_INVALID_PARAMETER, "Size of key for HMAC must be greater than 0!");

    if (key_size > HMAC_MAXIMUM_KEY_SIZE)
    {
        MCL_DEBUG("Resizing given key to its SHA256 value.");

        code = security_hash_sha256(key, key_size, &key_resized, &key_local_size);
        MCL_ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Given HMAC key size = <%u> was greater than allowed = <%u>, but new key couldn't be constructed from its SHA256 hash!",
                            key_size, HMAC_MAXIMUM_KEY_SIZE);
        MCL_ASSERT_STATEMENT_CODE_MESSAGE(HMAC_SHA256_SIZE == key_local_size, MCL_FREE(key_resized), MCL_SHA256_CALCULATION_FAIL,
                                      "SHA256 didn't returned expected length of data = <%u>!", HMAC_SHA256_SIZE);
        key_local = key_resized;
    }

    // Allocate buffers for key_ipad/key_opad.
    MCL_DEBUG("Preparing buffers key_ipad and key_opad.");

    key_ipad_size = HMAC_MAXIMUM_KEY_SIZE + data_size;
    key_ipad = MCL_MALLOC(key_ipad_size);

    if (MCL_NULL == key_ipad)
    {
        if (key_resized == key_local)
        {
            MCL_FREE(key_resized);
        }
        MCL_ERROR_RETURN(MCL_OUT_OF_MEMORY, "Memory to hold input data for XOR operation with ipad couldn't be allocated!");
    }

    key_opad_size = HMAC_MAXIMUM_KEY_SIZE + HMAC_SHA256_SIZE;
    key_opad = MCL_MALLOC(key_opad_size);

    if (MCL_NULL == key_opad)
    {
        MCL_FREE(key_ipad);
        if (key_resized == key_local)
        {
            MCL_FREE(key_resized);
        }
        MCL_ERROR_RETURN(MCL_OUT_OF_MEMORY, "Memory to hold input data for XOR operation with opad couldn't be allocated!");
    }

    // Initialize buffers as long as the key is sized.
    for (index = 0; index < key_local_size; ++index)
    {
        key_ipad[index] = key_local[index] ^ hmac_ipad;
        key_opad[index] = key_local[index] ^ hmac_opad;
    }

    // Release key_local if calculated new.
    if (key_resized == key_local)
    {
        MCL_FREE(key_resized);
    }

    // Initialize the rest with hmac_ipad/hamc_opad.
    for (index = key_local_size; index < HMAC_MAXIMUM_KEY_SIZE; ++index)
    {
        key_ipad[index] = hmac_ipad;
        key_opad[index] = hmac_opad;
    }

    // Copy given data.
    MCL_DEBUG("Copying data to buffer.");
    string_util_memcpy(&key_ipad[HMAC_MAXIMUM_KEY_SIZE], data, data_size);

    // Calculate SHA256 for key_ipad.
    MCL_DEBUG("Calculating first SHA256 from XORed key + data.");

    code = security_hash_sha256(key_ipad, key_ipad_size, &sha_output, &sha_output_size);

    if (MCL_OK != code)
    {
        MCL_FREE(key_ipad);
        MCL_FREE(key_opad);
        MCL_FREE(sha_output);

        MCL_ERROR_RETURN(code, "SHA256 calculation for key_ipad failed!");
    }

    if (HMAC_SHA256_SIZE != sha_output_size)
    {
        MCL_FREE(key_ipad);
        MCL_FREE(key_opad);
        MCL_FREE(sha_output);

        MCL_ERROR_RETURN(MCL_SHA256_CALCULATION_FAIL, "SHA256 calculation for key_ipad failed!");
    }

    // Set SHA256 output to key_opad.
    MCL_DEBUG("Copying first SHA256 result to buffer.");
    string_util_memcpy(&key_opad[HMAC_MAXIMUM_KEY_SIZE], sha_output, sha_output_size);

    // Calculate SHA256 for key_opad.
    MCL_DEBUG("Calculating second SHA256 from XORed key + <first SHA256>.");
    code = security_hash_sha256(key_opad, key_opad_size, hash, hash_size);

    MCL_FREE(key_ipad);
    MCL_FREE(key_opad);
    MCL_FREE(sha_output);

    if (MCL_OK != code)
    {
        MCL_FREE(*hash);
        *hash_size = 0;
        MCL_ERROR_RETURN(code, "SHA256 calculation for key_opad failed!");
    }

    if ((MCL_NULL != *hash) && (HMAC_SHA256_SIZE != *hash_size))
    {
        MCL_FREE(*hash);
        *hash_size = 0;
        MCL_ERROR_RETURN(MCL_SHA256_CALCULATION_FAIL, "SHA256 calculation for key_opad failed!");
    }
    else
    {
        MCL_DEBUG("HMAC SHA256 calculated hash = <%p>, hash_size = <%u>", *hash, *hash_size);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}
