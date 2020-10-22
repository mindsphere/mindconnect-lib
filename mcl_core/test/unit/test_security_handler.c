/*!
 * @file     test_security_handler.c
 * @brief    This file contains test case functions to test security_handler module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "security_handler.h"
#include "memory.h"
#include "string_util.h"
#include "mock_base64.h"
#include "mock_security.h"
#include "mock_hmac.h"
#include "mock_random.h"
#include "mcl_core/mcl_log_util.h"
#include <string.h>

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Input argument to security_handler_initialize() function is not null.
 * WHEN  : security_handler_initialize() is called.
 * THEN  : MCL_OK is returned, security_handler is not null but its pointer members are null.
 */
void test_initialize_001(void)
{
    security_handler_t *security_handler = MCL_NULL;
    mcl_error_t result = security_handler_initialize(&security_handler);

    TEST_ASSERT_MESSAGE(MCL_OK == result, "security_handler_initialize() does not return MCL_OK.");
    TEST_ASSERT_NOT_NULL_MESSAGE(security_handler, "Security handler is null after initialization.");
    TEST_ASSERT_NULL_MESSAGE(security_handler->rsa.private_key, "RSA private key is not null after initialization although it should be.");
    TEST_ASSERT_NULL_MESSAGE(security_handler->rsa.public_key, "RSA public key is not null after initialization although it should be.");
    TEST_ASSERT_NULL_MESSAGE(security_handler->access_token, "Access token is not null after initialization although it should be.");
    TEST_ASSERT_NULL_MESSAGE(security_handler->registration_access_token, "Registration access token is not null after initialization although it should be.");
    TEST_ASSERT_NULL_MESSAGE(security_handler->client_secret, "Client secret is not null after initialization although it should be.");
    TEST_ASSERT_NULL_MESSAGE(security_handler->registration_uri, "Registration URI is not null after initialization although it should be.");
    TEST_ASSERT_NULL_MESSAGE(security_handler->client_id, "Client id is not null after initialization although it should be.");
    TEST_ASSERT_NULL_MESSAGE(security_handler->last_token_time, "Last token time is not null after initialization although it should be.");

    security_handler_destroy(&security_handler);
}

/**
 * GIVEN : Security handler is initialized successfully.
 * WHEN  : security_handler_destroy() is called.
 * THEN  : MCL_OK is returned and security handler is null.
 */
void test_destroy_001(void)
{
    security_handler_t *security_handler = MCL_NULL;
    
    security_handler_initialize(&security_handler);

    security_handler_destroy(&security_handler);

    TEST_ASSERT_NULL_MESSAGE(security_handler, "Security handler is not null after it is destroyed.");
}

/**
 * GIVEN : Data to encode.
 * WHEN  : security_handler_base64_url_encode() is called.
 * THEN  : MCL_OK and encoded data is returned.
 */
void test_base64_url_encode_001()
{
    const mcl_uint8_t *data = "tmp_data";
    const mcl_size_t data_size = 0;
    char *encoded_data = MCL_NULL;
    char *expected_data = string_util_strdup("Test encoded data");

    base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    base64_url_encode_ReturnThruPtr_encoded_data(&expected_data);

    mcl_error_t code = security_handler_base64_url_encode(data, data_size, &encoded_data);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(expected_data, encoded_data, "Encoded data is not returned as expected!");

    MCL_FREE(expected_data);
}

/**
 * GIVEN : There is not enough memory to allocate.
 * WHEN  : security_handler_base64_url_encode() is called.
 * THEN  : MCL_OUT_OF_MEMORY is returned.
 */
void test_base64_url_encode_002()
{
    const mcl_uint8_t *data = "tmp_data";
    const mcl_size_t data_size = 0;
    char *encoded_data = MCL_NULL;

    base64_url_encode_ExpectAnyArgsAndReturn(MCL_OUT_OF_MEMORY);

    mcl_error_t code = security_handler_base64_url_encode(data, data_size, &encoded_data);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OUT_OF_MEMORY, code, "MCL_OUT_OF_MEMORY expected as return code!");
    TEST_ASSERT_NULL_MESSAGE(encoded_data, "Encoded data is not NULL!");
}

/**
* GIVEN : Data to hash.
* WHEN  : security_handler_hash_sha256() is called.
* THEN  : MCL_OK and hash data is returned.
*/
void test_hash_sha256_001()
{
    const mcl_uint8_t *data = "tmp";
    const mcl_size_t data_size = 0;
    mcl_uint8_t *expected_data = "Test hash";
    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size;

    security_hash_sha256_ExpectAnyArgsAndReturn(MCL_OK);
    security_hash_sha256_ReturnThruPtr_hash(&expected_data);

    mcl_error_t code = security_handler_hash_sha256(data, data_size, &hash, &hash_size);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(expected_data, hash, "Hash is not returned as expected!");
}

/**
* GIVEN : There is not enough memory to allocate.
* WHEN  : security_handler_hash_sha256() is called.
* THEN  : MCL_OUT_OF_MEMORY is returned.
*/
void test_hash_sha256_002()
{
    const mcl_uint8_t *data = "tmp";
    const mcl_size_t data_size = 0;
    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size;

    security_hash_sha256_ExpectAnyArgsAndReturn(MCL_OUT_OF_MEMORY);

    mcl_error_t code = security_handler_hash_sha256(data, data_size, &hash, &hash_size);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OUT_OF_MEMORY, code, "MCL_OUT_OF_MEMORY expected as return code!");
    TEST_ASSERT_NULL_MESSAGE(hash, "Hash is not NULL!");
}

/**
* GIVEN : Nothing.
* WHEN  : security_handler_generate_jti() is called.
* THEN  : MCL_OK and jti is returned.
*/
void test_generate_jti_001(void)
{
    char *jti = MCL_NULL;
    char *expected_data = string_util_strdup("2b1a7b1a56384e018fee50f02c38366a");
    
    
    random_generate_guid_ExpectAnyArgsAndReturn(MCL_OK);
    random_generate_guid_ReturnThruPtr_guid(&expected_data);

    mcl_error_t code = security_handler_generate_jti(&jti);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(expected_data, jti, "jti is not returned as expected!");

    MCL_FREE(expected_data);
}

/**
* GIVEN : Initialized security handler.
* WHEN  : security_handler_generate_rsa_key is() called.
* THEN  : MCL_OK, public and private keys are returned.
*/
void test_generate_rsa_key_001(void)
{
    security_handler_t *security_handler = MCL_NULL;
    
    mcl_error_t code = security_handler_initialize(&security_handler);
    
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(security_handler, "Security handler is NULL!");

    char *public_key_expected = MCL_MALLOC(sizeof("dummy_public_key"));
    memcpy(public_key_expected, "dummy_public_key", sizeof("dummy_public_key"));

    char *private_key_expected = MCL_MALLOC(sizeof("dummy_private_key"));
    memcpy(private_key_expected, "dummy_private_key", sizeof("dummy_private_key"));

    security_generate_rsa_key_ExpectAnyArgsAndReturn(MCL_OK);
    security_generate_rsa_key_ReturnThruPtr_public_key(&public_key_expected);
    security_generate_rsa_key_ReturnThruPtr_private_key(&private_key_expected);
   
    code = security_handler_generate_rsa_key(security_handler);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(public_key_expected, security_handler->rsa.public_key, "public_key is not returned as expected!");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(private_key_expected, security_handler->rsa.private_key, "private_key is not returned as expected!");

    security_handler_destroy(&security_handler);
}

/**
* GIVEN : Initialized security handler and data to hash.
* WHEN  : security_handler_hmac_sha256() is called.
* THEN  : MCL_OK and hash data are returned.
*/
void test_hmac_sha256_001(void)
{
    security_handler_t *security_handler = MCL_NULL;
    const mcl_uint8_t *data = "tmp";
    const mcl_size_t data_size = 0;
    mcl_uint8_t *expected_data = "Test hash";
    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size;
    const char *security_handler_client_secret = "test_client_secret";

    mcl_error_t code = security_handler_initialize(&security_handler);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(security_handler, "Security handler is NULL!");

    security_handler->client_secret = string_util_strdup(security_handler_client_secret);

    hmac_sha256_ExpectAnyArgsAndReturn(MCL_OK);
    hmac_sha256_ReturnThruPtr_hash(&expected_data);

    code = security_handler_hmac_sha256(security_handler, data, data_size, &hash, &hash_size);
   
    TEST_ASSERT_EQUAL_PTR_MESSAGE(expected_data, hash, "Hash is not returned as expected!");
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK expected as return code!");
    
    MCL_FREE(security_handler->client_secret);
    security_handler_destroy(&security_handler);
}

/**
* GIVEN : Initialized security handler and there is not enough memory to allocate hash.
* WHEN  : security_handler_hmac_sha256() is called.
* THEN  : MCL_OUT_OF_MEMORY is returned.
*/
void test_hmac_sha256_002(void)
{
    security_handler_t *security_handler = MCL_NULL;
    const mcl_uint8_t *data = "tmp";
    const mcl_size_t data_size = 0;
    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size = 0;
    const char *security_handler_client_secret = "test_client_secret";

    mcl_error_t code = security_handler_initialize(&security_handler);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(security_handler, "Security handler is NULL!");

    security_handler->client_secret = string_util_strdup(security_handler_client_secret);

    hmac_sha256_ExpectAnyArgsAndReturn(MCL_OUT_OF_MEMORY);

    code = security_handler_hmac_sha256(security_handler, data, data_size, &hash, &hash_size);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OUT_OF_MEMORY, code, "MCL_OUT_OF_MEMORY expected as return code!");
    TEST_ASSERT_NULL_MESSAGE(hash, "Hash is not NULL!");

    MCL_FREE(security_handler->client_secret);
    security_handler_destroy(&security_handler);
}

/**
* GIVEN : Data to encode.
* WHEN  : security_handler_base64_encode() is called.
* THEN  : MCL_OK and encoded data are returned.
*/
void test_base64_encode_001(void)
{
    const mcl_uint8_t *data = "tmp_data";
    const mcl_size_t data_size = 0;
    char *encoded_data = MCL_NULL;
    char *expected_data = string_util_strdup("Test encoded data");

    base64_encode_ExpectAnyArgsAndReturn(MCL_OK);
    base64_encode_ReturnThruPtr_encoded_data(&expected_data);

    mcl_error_t code = security_handler_base64_encode(data, data_size, &encoded_data);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(expected_data, encoded_data, "Encoded data is not returned as expected!");

    MCL_FREE(expected_data);
}

/**
* GIVEN : There is not enough memory to allocate.
* WHEN  : security_handler_base64_encode() is called.
* THEN  : MCL_OUT_OF_MEMORY is returned.
*/
void test_base64_encode_002(void)
{
    const mcl_uint8_t *data = "tmp_data";
    const mcl_size_t data_size = 0;
    char *encoded_data = MCL_NULL;

    base64_encode_ExpectAnyArgsAndReturn(MCL_OUT_OF_MEMORY);

    mcl_error_t code = security_handler_base64_encode(data, data_size, &encoded_data);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OUT_OF_MEMORY, code, "MCL_OUT_OF_MEMORY expected as return code!");
}

/**
* GIVEN : Data to decode.
* WHEN  : security_handler_base64_decode() is called.
* THEN  : MCL_OK and decoded data are returned.
*/
void test_base64_decode_001(void)
{  
    mcl_uint8_t *expected_data = "test decode data";
    char *encoded_data = string_util_strdup("tmp");
    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size = 0;

    base64_decode_ExpectAnyArgsAndReturn(MCL_OK);
    base64_decode_ReturnThruPtr_decoded_data(&expected_data);

    mcl_error_t code = security_handler_base64_decode(encoded_data, &decoded_data, &decoded_data_size);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(expected_data, decoded_data, "Decoded data is not returned as expected!");

    MCL_FREE(encoded_data);
}

/**
* GIVEN : There is not enough memory to allocate.
* WHEN  : security_handler_base64_decode() is called.
* THEN  : MCL_OUT_OF_MEMORY is returned.
*/
void test_base64_decode_002(void)
{
    char *encoded_data = string_util_strdup("tmp");
    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size = 0;

    base64_decode_ExpectAnyArgsAndReturn(MCL_OUT_OF_MEMORY);

    mcl_error_t code = security_handler_base64_decode(encoded_data, &decoded_data, &decoded_data_size);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OUT_OF_MEMORY, code, "MCL_OUT_OF_MEMORY expected as return code!");

    MCL_FREE(encoded_data);
}

/**
* GIVEN : Data to sign and RSA private key.
* WHEN  : security_handler_rsa_sign() is called.
* THEN  : MCL_OK and signature data are returned.
*/
void test_rsa_sign_001(void)
{
    mcl_uint8_t *expected_data = "test signature";
    char *data = "test_data";
    char *rsa_key = "test_rsa_key";
    mcl_uint8_t *signature = MCL_NULL;
    mcl_size_t signature_length = 0;
    mcl_size_t data_size = 0;

    security_rsa_sign_ExpectAnyArgsAndReturn(MCL_OK);
    security_rsa_sign_ReturnThruPtr_signature(&expected_data);

    mcl_error_t code = security_handler_rsa_sign(rsa_key, data, data_size, &signature, &signature_length);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(expected_data, signature, "Signature is not returned as expected!");
}


