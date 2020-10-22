/*!
 * @file     test_crypto_openssl.c
 * @brief    This file contains test case functions to test security module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "security_libcrypto.h"
#include "security.h"
#include "memory.h"
#include "string_util.h"
#include "base64.h"
#include "string.h"
#include "mcl_core/mcl_log_util.h"

char *private_key_1 = "-----BEGIN RSA PRIVATE KEY-----\n" \
"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\n" \
"-----END RSA PRIVATE KEY-----";

char *public_key_1 = "-----BEGIN PUBLIC KEY-----\n" \
"MIIBojANBgkqhkiG9w0BAQEFAAOCAY8AMIIBigKCAYEAtKfmMiKnbsDbeXYDMb52\n" \
"JNv8bho+hKp0gA/rlnpErxj7skpBftKEaj05mNGfAoWBJvlA0lzf5LB6mBeziBxg\n" \
"NDINMxMZsV3EXwkAGQHxAviDQLEBDCYxFgmArimm1VzaQ5FRv4smIOpHhMMTJnJl\n" \
"3B0a/tp+dlNwKzUKoR9K8b7aH9YsoObApG8Bl6S5ZsI+dhR04Vm0LcMiYVs0+Xge\n" \
"4K4C/VAaQUFnDL3Tlgf2F/1nw9/SyPn+HROmXJbiM+Bsvql0umlx+dOJSFne7Uk5\n" \
"vq+eAjsFUJMVXXTS1+N63DNQFh16iZvur6dsC3T/xREfMtg8zd6t1BmOp2mNj7Ds\n" \
"7DUcyecuqhehQeKoAuSjH3DrPxXP+28ASKPBcRWpp+VHiantj/bxV+STGCYTdSZL\n" \
"TN/V5nE5tSkW9xQAXYIFBNYWy3tVvzbaWSKgr11FF/0mR2z1CX4pCteftbRktgU0\n" \
"fWwceranuWntea8mE2If0GyUY3Ultz5V1TZ/e0idekgbAgMBAAE=\n" \
"-----END PUBLIC KEY-----";

static mcl_error_t _string_convert_binary_to_hex(const mcl_uint8_t *buffer, mcl_size_t buffer_size, char **hex_data);

void setUp(void)
{
}

void tearDown(void)
{
}

/**
* GIVEN : Test data for calculating SHA256.
* WHEN  : security_hash_sha256() is called.
* THEN  : Expected SHA256 with correct size must be returned.
*/
void test_sha256_001()
{
	char *data = "The quick brown fox jumps over the lazy dog";
	mcl_uint8_t *hash = MCL_NULL;
	mcl_size_t hash_size = 0;

	mcl_error_t code = security_hash_sha256((mcl_uint8_t *)data, string_util_strlen(data), &hash, &hash_size);

	TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected return code not received.");
	TEST_ASSERT_NOT_NULL_MESSAGE(hash, "SHA256 calculation failed!");
	TEST_ASSERT_MESSAGE(32 == hash_size, "Hash size must be 32 bytes long!");

	// Convert for hex comparison.
	char *hex_hash;
	code = _string_convert_binary_to_hex(hash, hash_size, &hex_hash);

	TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected return code not received.");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("D7A8FBB307D7809469CA9ABCB0082E4F8D5651E46D3CDB762D02D0BF37C9E592", hex_hash, "Expected SHA256 not achieved!");

	MCL_FREE(hash);
	MCL_FREE(hex_hash);
}

/**
* GIVEN : Empty test data for calculating SHA256.
* WHEN  : security_hash_sha256() is called.
* THEN  : Expected SHA256 with correct size must be returned.
*/
void test_hash_sha256_002(void)
{
	char *data = "";
	mcl_uint8_t *hash = MCL_NULL;
	mcl_size_t hash_size = 0;

	mcl_error_t code = security_hash_sha256((mcl_uint8_t *)data, string_util_strlen(data), &hash, &hash_size);

	TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected return code not received");
	TEST_ASSERT_NOT_NULL_MESSAGE(hash, "SHA256 calculation failed!");
	TEST_ASSERT_MESSAGE(32 == hash_size, "Hash size must be 32 bytes long!");

	// Convert for hex comparison.
	char *hex_hash;
	code = _string_convert_binary_to_hex(hash, hash_size, &hex_hash);

	TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected return code not received.");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("E3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855", hex_hash, "Expected SHA256 not achieved!");

	MCL_FREE(hash);
	MCL_FREE(hex_hash);
}

/**
* GIVEN : Nothing.
* WHEN  : security_generate_rsa_key() is called.
* THEN  : MCL_OK is returned and the keys are generated.
*/
void test_generate_rsa_key_001(void)
{
	char *public_key = MCL_NULL;
	char *private_key = MCL_NULL;

	mcl_error_t code = security_generate_rsa_key(&public_key, &private_key);

	TEST_ASSERT_EQUAL(MCL_OK, code);
	TEST_ASSERT_NOT_NULL(public_key);
	TEST_ASSERT_NOT_NULL(private_key);

	MCL_FREE(public_key);
	MCL_FREE(private_key);
}

/**
* GIVEN : Data to be signed and RSA key to sign.
* WHEN  : security_rsa_sign() is called.
* THEN  : MCL_OK is returned and a signature is generated.
*/
void test_rsa_sign_001(void)
{
	char *data = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJkYmE4YmFiNy0yODZkLTRiODUtNTUyMS00OTliYmViMWE1OGQiLCJzdWIiOiJkYmE4YmFiNy0yODZkLTRiODUtNTUyMS00OTliYmViMWE1OGQiLCJhdWQiOiJNaW5kU3BoZXJlIEFTIiwiaWF0IjoxNTAxNjcyODQyLCJuYmYiOjE1MDE2NzI4NDIsImV4cCI6MTUwMTc1OTI0MiwianRpIjoiZnAxWHFvcCtjZGd3QUpxTkpxOHpad1x1MDAzZFx1MDAzZCIsInNjaGVtYXMiOlsidXJuOnNpZW1lbnM6bWluZHNwaGVyZTp2MSJdLCJ0ZW4iOiJici1zbWsxIn0";
	char *private_key = private_key_1;

	unsigned char *signature = MCL_NULL;
	mcl_size_t signature_length = 0;

	mcl_error_t code = security_rsa_sign(private_key, data, strlen(data), &signature, &signature_length);

	TEST_ASSERT_EQUAL(MCL_OK, code);
	TEST_ASSERT_NOT_NULL(signature);
	TEST_ASSERT_NOT_EQUAL(0, signature_length);

	MCL_FREE(signature);
}

/**
* GIVEN : RSA public key.
* WHEN  : security_rsa_get_modulus_and_exponent() is called.
* THEN  : MCL_OK is returned with expected modulus and exponent.
*/
void test_rsa_get_modulus_and_exponent_001(void)
{
	char *public_key = "-----BEGIN PUBLIC KEY-----\n"\
		"MIIBojANBgkqhkiG9w0BAQEFAAOCAY8AMIIBigKCAYEA2eJOyqOS6o/tz8i+wGHZ\n"\
		"k3IPcTwbm7neoKftjcFPzmNuTfVbTBU/IC+eRrn6dOAUXl926A36pa+I+brFqafi\n"\
		"qWklbmIk/C4+5CWfpGH8Fzw6PtnVcKTW7mJ8AfiKog0EHuy0Z9FhMhaa4ER48L2R\n"\
		"Fwelr+TKCFaDuNTCc60Lr74SeZFN2p7C0R0cIzVMWJYglEDTcKjiFkFycBd6Rc4Y\n"\
		"XPXZCm+Jyq5VSxu/ZDToTdB20jfIWPQNv/jyqrkwkSQ7Rl5Mgf4dFHNN81WB44Nf\n"\
		"0EkbA8o6TR3xZ3wxDlKuPmksDN4Xcdi8G6k6maQEl4fGvjcjcAdds6qN1Aq//xPJ\n"\
		"ZIr1/MiVVa3MXJLKBEgVznbPP4QwcamGjUK6S1YWo+5OtLdwaFMI3Sj2u7dy54lL\n"\
		"oWhib1QAF8RglYZIF4M8cyt2mxDZkPULE2b3esenQhY1nSHMeNXDLRWjeVze6AdA\n"\
		"jJWuutw7zjn6gBiZEmp+Q+nIlF75nvORuYm64eiQgYHxAgMBAAE=\n"\
		"-----END PUBLIC KEY-----";

	char *expected_modulus = "2eJOyqOS6o_tz8i-wGHZk3IPcTwbm7neoKftjcFPzmNuTfVbTBU_IC-eRrn6dOAUXl926A36pa-I-brFqafiqWklbmIk_C4-5CWfpGH8Fzw6PtnVcKTW7mJ8AfiKog0EHuy0Z9FhMhaa4ER48L2RFwelr-TKCFaDuNTCc60Lr74SeZFN2p7C0R0cIzVMWJYglEDTcKjiFkFycBd6Rc4YXPXZCm-Jyq5VSxu_ZDToTdB20jfIWPQNv_jyqrkwkSQ7Rl5Mgf4dFHNN81WB44Nf0EkbA8o6TR3xZ3wxDlKuPmksDN4Xcdi8G6k6maQEl4fGvjcjcAdds6qN1Aq__xPJZIr1_MiVVa3MXJLKBEgVznbPP4QwcamGjUK6S1YWo-5OtLdwaFMI3Sj2u7dy54lLoWhib1QAF8RglYZIF4M8cyt2mxDZkPULE2b3esenQhY1nSHMeNXDLRWjeVze6AdAjJWuutw7zjn6gBiZEmp-Q-nIlF75nvORuYm64eiQgYHx";    char *expected_exponent = "AQAB";
	char *modulus = MCL_NULL;
	char *exponent = MCL_NULL;

	mcl_error_t code = security_rsa_get_modulus_and_exponent(public_key, &modulus, &exponent);

	TEST_ASSERT_EQUAL(MCL_OK, code);
	TEST_ASSERT_EQUAL_STRING(expected_modulus, modulus);
	TEST_ASSERT_EQUAL_STRING(expected_exponent, exponent);

	MCL_FREE(modulus);
	MCL_FREE(exponent);
}

/**
* GIVEN : Buffer.
* WHEN  : security_generate_random_bytest() is called.
* THEN  : MCL_OK is returned.
*/
void test_security_generate_random_bytes_001(void)
{
    uint8_t random_bytes_buffer[16];
    
    mcl_error_t code = security_generate_random_bytes(random_bytes_buffer, sizeof(random_bytes_buffer));
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "security_generate_random_bytes() unexpected return error!");
}

static mcl_error_t _string_convert_binary_to_hex(const mcl_uint8_t *buffer, mcl_size_t buffer_size, char **hex_data)
{
    mcl_size_t index;
    const char *hex_table = "0123456789ABCDEF";

    // allocate space for hex result (zero terminated)
    *hex_data = MCL_CALLOC(1, 2 * buffer_size + 1);

    if (MCL_NULL == *hex_data)
    {
        return MCL_OUT_OF_MEMORY;
    }

    // convert byte to hex using table
    for (index = 0; index < buffer_size; index++)
    {
        (*hex_data)[index * 2 + 0] = hex_table[(buffer[index] >> 4) & 0x0F];
        (*hex_data)[index * 2 + 1] = hex_table[buffer[index] & 0x0F];
    }
    (*hex_data)[buffer_size * 2] = MCL_NULL_CHAR;

    return MCL_OK;
}
