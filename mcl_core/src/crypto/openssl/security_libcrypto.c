/*!
 * @file     security_libcrypto.c
 * @brief    Security libcrypto module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "security_libcrypto.h"
#include "base64.h"
#include "string_util.h"
#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_memory.h"
#include <openssl/bn.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include <openssl/rand.h>

#define KEY_LENGTH_BITS 3072

// This function is used for base64 encoding a big number such as RSA modulus, public exponent and private exponent.
static mcl_error_t _base64_encode_big_number(const BIGNUM *big_number, char **encoded);

// This function is used to get the public key in PEM format from OpenSSL RSA structure.
static mcl_error_t _get_rsa_public_key(RSA *rsa, char **public_key);

// This function is used to get the private key in PEM format from OpenSSL RSA structure.
static mcl_error_t _get_rsa_private_key(RSA *rsa, char **private_key);

// Memory function wrappers for OpenSSL.
static void *_libcrypto_malloc(size_t size, const char *file, int line);
static void *_libcrypto_realloc(void *p, size_t size, const char *file, int line);
static void _libcrypto_free(void *p, const char *file, int line);

mcl_error_t security_initialize(void)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("void");

    // Set memory functions.
    if (1 == CRYPTO_set_mem_functions(_libcrypto_malloc, _libcrypto_realloc, _libcrypto_free))
    {
        code = MCL_OK;
    }
    else
    {
        code = MCL_FAIL;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t security_hash_sha256(const mcl_uint8_t *data, mcl_size_t data_size, mcl_uint8_t **hash, mcl_size_t *hash_size)
{
    MCL_DEBUG_ENTRY("const mcl_uint8_t *data = <%p>, mcl_size_t data_size = <%u>, mcl_uint8_t **hash = <%p>, mcl_size_t *hash_size = <%p>", data, data_size, hash, hash_size);

    // Set hash size to zero.
    *hash_size = 0;

    // Allocate memory to store SHA256 result.
    *hash = MCL_CALLOC(1, SHA256_DIGEST_LENGTH);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != *hash, MCL_OUT_OF_MEMORY, "Memory allocation for SHA256 failed!");

    *hash_size = SHA256_DIGEST_LENGTH;

    // Perform SHA256 calculation.
    MCL_DEBUG("Calculating SHA256 with OpenSSL.");
    SHA256(data, data_size, *hash);

    MCL_DEBUG("Calculated SHA256 *hash = <%p>, hash_size = <%u>", *hash, hash_size);
    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

mcl_error_t security_rsa_sign(char *rsa_key, char *data, mcl_size_t data_size, mcl_uint8_t **signature, mcl_size_t *signature_size)
{
    mcl_error_t code = MCL_OK;
    RSA *rsa = MCL_NULL;
    unsigned int sign_length = 0;
    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size = 0;
    mcl_bool_t ok;
    mcl_size_t length;
    BIO *bio;

    MCL_DEBUG_ENTRY("char *rsa_key = <%p>, char *data = <%p>, mcl_size_t data_size = <%u>, mcl_uint8_t **signature = <%p>, mcl_size_t *signature_size = <%p>", rsa_key, data,
                    data_size, signature, signature_size);

    length = string_util_strlen(rsa_key);
    bio = BIO_new_mem_buf(rsa_key, (int) length);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != bio, MCL_OUT_OF_MEMORY, "Memory can not be allocated for BIO.");

    rsa = PEM_read_bio_RSAPrivateKey(bio, &rsa, 0, MCL_NULL);
    MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != rsa, BIO_free(bio), MCL_FAIL, "RSA structure can not be generated.");

    ok = (MCL_NULL != (*signature = MCL_MALLOC(RSA_size(rsa))));
    ok = (MCL_TRUE == ok) && (MCL_OK == security_hash_sha256((mcl_uint8_t *)data, data_size, &hash, &hash_size));
    ok = (MCL_TRUE == ok) && (1 == RSA_sign(NID_sha256, hash, (unsigned int) hash_size, *signature, &sign_length, rsa));
    *signature_size = sign_length;

    if (MCL_FALSE == ok)
    {
        MCL_FREE(*signature);
        code = MCL_FAIL;
    }

    MCL_FREE(hash);
    RSA_free(rsa);
    BIO_free(bio);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t security_generate_rsa_key(char **public_key, char **private_key)
{
    RSA * rsa;
    BIGNUM *exponent = MCL_NULL;
    int result = 0;
    mcl_error_t code = MCL_FAIL;

    MCL_DEBUG_ENTRY("char **public_key = <%p>, char **private_key = <%p>", public_key, private_key);

    // Allocate for RSA structure.
    rsa = RSA_new();
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != rsa, MCL_OUT_OF_MEMORY, "OpenSSL RSA structure can not be allocated.");

    // Allocate for the public exponent (e).
    exponent = BN_new();
    MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != exponent, RSA_free(rsa), MCL_OUT_OF_MEMORY, "OpenSSL big number for exponent can not be allocated.");

    // Set the value of public exponent.
    BN_set_word(exponent, RSA_F4);

    // Generate RSA structure containing public/private key pair.
    result = RSA_generate_key_ex(rsa, KEY_LENGTH_BITS, exponent, MCL_NULL);

    // Free public exponent.
    BN_free(exponent);

    // Get private key.
    if (0 != result)
    {
        code = _get_rsa_private_key(rsa, private_key);
    }

    // Get public key.
    if (MCL_OK == code)
    {
        code = _get_rsa_public_key(rsa, public_key);
    }

    if (MCL_OK != code)
    {
        MCL_FREE(*private_key);
        MCL_FREE(*public_key);
    }

    RSA_free(rsa);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t security_rsa_get_modulus_and_exponent(char *public_key, char **modulus, char **exponent)
{
    mcl_error_t code;
    RSA *rsa = MCL_NULL;
    mcl_size_t length;
    BIO *bio;
    const BIGNUM *n;
    const BIGNUM *e;

    MCL_DEBUG_ENTRY("char *public_key = <%s>, char **modulus = <%p>, char **exponent = <%p>", public_key, modulus, exponent);

    length = string_util_strlen(public_key);
    bio = BIO_new_mem_buf(public_key, (int) length);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != bio, MCL_OUT_OF_MEMORY, "Memory can not be allocated for BIO.");

    rsa = PEM_read_bio_RSA_PUBKEY(bio, &rsa, 0, MCL_NULL);
    MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != rsa, BIO_free(bio), MCL_FAIL, "RSA structure can not be generated.");

    // Get modulus and exponent.
    RSA_get0_key(rsa, &n, &e, MCL_NULL);

    code = _base64_encode_big_number(n, modulus);

    if (MCL_OK == code)
    {
        code = _base64_encode_big_number(e, exponent);
    }

    if (MCL_OK != code)
    {
        MCL_FREE(*modulus);
        MCL_FREE(*exponent);
    }

    RSA_free(rsa);
    BIO_free(bio);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t security_generate_random_bytes(unsigned char *buffer, mcl_size_t size)
{
    mcl_error_t code = MCL_FAIL;

    MCL_DEBUG_ENTRY("unsigned char *buffer = <%p>, mcl_size_t size = <%u>", buffer, size);

    if (1 == RAND_bytes(buffer, (int) size))
    {
        code = MCL_OK;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _base64_encode_big_number(const BIGNUM *big_number, char **encoded)
{
    mcl_error_t code;
    int binary_length;
    char *binary_encoded = MCL_NULL;
    mcl_uint8_t *binary;

    MCL_DEBUG_ENTRY("const BIGNUM *big_number = <%p>, char **encoded = <%p>", big_number, encoded);

    binary = MCL_MALLOC(BN_num_bytes(big_number));
    MCL_ASSERT_CODE(MCL_NULL != binary, MCL_OUT_OF_MEMORY);

    binary_length = BN_bn2bin(big_number, binary);
    code = base64_url_encode(binary, binary_length, &binary_encoded);

    if (MCL_OK == code)
    {
        *encoded = binary_encoded;
    }

    MCL_FREE(binary);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _get_rsa_private_key(RSA *rsa, char **private_key)
{
    mcl_error_t code = MCL_OK;
    BIO *bio;
    int key_length = 0;

    MCL_DEBUG_ENTRY("RSA *rsa = <%p>, char **private_key = <%p>", rsa, private_key);

    bio = BIO_new(BIO_s_mem());
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != bio, MCL_OUT_OF_MEMORY, "Memory can not allocated for BIO.");

    if (0 == PEM_write_bio_RSAPrivateKey(bio, rsa, NULL, NULL, 0, NULL, NULL))
    {
        BIO_free_all(bio);
        MCL_ERROR_RETURN(MCL_FAIL, "Private key can not be written from RSA struct to BIO.");
    }

    key_length = BIO_pending(bio);

    *private_key = MCL_MALLOC(key_length + 1);
    MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != *private_key, BIO_free_all(bio), MCL_OUT_OF_MEMORY, "Memory can not be allocated for private key.");

    if (key_length != BIO_read(bio, *private_key, key_length))
    {
        MCL_DEBUG("Private key can not be read from BIO.");
        code = MCL_FAIL;
        MCL_FREE(*private_key);
    }
    else
    {
        (*private_key)[key_length] = MCL_NULL_CHAR;
    }

    BIO_free_all(bio);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _get_rsa_public_key(RSA *rsa, char **public_key)
{
    mcl_error_t code = MCL_OK;
    BIO *bio;
    int key_length = 0;

    MCL_DEBUG_ENTRY("RSA *rsa = <%p>, char **public_key = <%p>", rsa, public_key);

    bio = BIO_new(BIO_s_mem());
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != bio, MCL_OUT_OF_MEMORY, "Memory can not allocated for BIO.");

    if (0 == PEM_write_bio_RSA_PUBKEY(bio, rsa))
    {
        BIO_free_all(bio);
        MCL_ERROR_RETURN(MCL_FAIL, "Public key can not be written from RSA struct to BIO.");
    }

    key_length = BIO_pending(bio);

    *public_key = MCL_MALLOC(key_length + 1);
    MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != *public_key, BIO_free_all(bio), MCL_OUT_OF_MEMORY, "Memory can not be allocated for public key.");

    if (key_length != BIO_read(bio, *public_key, key_length))
    {
        MCL_DEBUG("Public key can not be read from BIO.");
        code = MCL_FAIL;
        MCL_FREE(*public_key);
    }
    else
    {
        (*public_key)[key_length] = MCL_NULL_CHAR;
    }

    BIO_free_all(bio);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static void *_libcrypto_malloc(size_t size, const char *file, int line)
{
    void *p = MCL_NULL;

    MCL_VERBOSE_ENTRY("size_t size = <%lu>, const char *file = <%s>, int line = <%d>", size, file, line);

#if !MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_VERBOSE)
    // Unused parameters.
    (void) file;
    (void) line;
#endif

    p = mcl_memory_malloc(size);

    MCL_VERBOSE_LEAVE("retVal = <%p>", p);
    return p;
}

static void *_libcrypto_realloc(void *p, size_t size, const char *file, int line)
{
    void *memory_reallocated;

    MCL_VERBOSE_ENTRY("void *p = <%p>, size_t size = <%lu>, const char *file = <%s>, int line = <%d>", p, size, file, line);

#if !MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_VERBOSE)
    // Unused parameters.
    (void) file;
    (void) line;
#endif

    memory_reallocated = mcl_memory_realloc(p, size);

    MCL_VERBOSE_LEAVE("retVal = <%p>", memory_reallocated);
    return memory_reallocated;
}

static void _libcrypto_free(void *p, const char *file, int line)
{
    MCL_VERBOSE_ENTRY("void *p = <%p>, const char *file = <%s>, int line = <%d>", p, file, line);

#if !MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_VERBOSE)
    // Unused parameters.
    (void) file;
    (void) line;
#endif

    mcl_memory_free(p);

    MCL_VERBOSE_LEAVE("retVal = <void>");
}
