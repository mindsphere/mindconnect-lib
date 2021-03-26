/*!
 * @file     jwt.c
 * @brief    JWT module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "jwt.h"
#include "time_util.h"
#include "json_util.h"
#include "definitions.h"
#include "string_util.h"
#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_memory.h"

// JWT specific constants.
#define JWT_HEADER_NAME_ALG "alg"
#define JWT_HEADER_NAME_TYPE "typ"
#define JWT_HEADER_NAME_KID "kid"

#define JWT_PAYLOAD_NAME_ISS "iss"
#define JWT_PAYLOAD_NAME_SUB "sub"
#define JWT_PAYLOAD_NAME_AUD "aud"
#define JWT_PAYLOAD_NAME_IAT "iat"
#define JWT_PAYLOAD_NAME_NBF "nbf"
#define JWT_PAYLOAD_NAME_EXP "exp"
#define JWT_PAYLOAD_NAME_SCHEMAS "schemas"
#define JWT_PAYLOAD_NAME_TEN "ten"
#define JWT_PAYLOAD_NAME_JTI "jti"
#define JWT_PAYLOAD_NAME_PUBLIC_KEY "public_key"
#define JWT_PAYLOAD_NAME_CONTENT_MD5 "content-md5"

#define JWT_HEADER_VALUE_ALG_SHARED_SECRET "HS256"
#define JWT_HEADER_VALUE_ALG_RSA_3072 "RS256"
#define JWT_HEADER_VALUE_TYP "JWT"

#define JWT_PAYLOAD_VALUE_SCHEMAS "urn:siemens:mindsphere:v1"
#define JWT_PAYLOAD_VALUE_AUD "southgate"

// Creates header JSON object of JWT.
static mcl_error_t _create_self_issued_jwt_header(E_MCL_SECURITY_PROFILE security_profile, mcl_json_t **header);

// Creates payload JSON object of JWT.
static mcl_error_t _create_self_issued_jwt_payload(security_handler_t *security_handler, char *tenant, jwt_t *jwt);

// Adds schema array to payload of JWT.
static mcl_error_t _add_schema_to_jwt(mcl_json_t *payload);

// Encodes header and payload with base64. Clears the input strings if fails.
static mcl_error_t _get_header_and_payload_encoded_base64_url(char *header, char *payload, char **header_encoded, char **payload_encoded);

// Joins two strings with dot (.) in between and generates a new string while destroying the two input strings.
static mcl_error_t _join_with_dot(char *string_a, char *string_b, char **joined);

// Calculates the signature from the header and payload strings.
static mcl_error_t _calculate_signature(jwt_t *jwt, char *header_and_payload, char **signature);

// Generates the JWT.
static mcl_error_t _generate_token(jwt_t *jwt, char *header, char *payload, char **token);

mcl_error_t jwt_initialize(security_handler_t *security_handler, E_MCL_SECURITY_PROFILE security_profile, char *tenant, jwt_t **jwt)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("security_handler_t *security_handler = <%p>, E_MCL_SECURITY_PROFILE security_profile = <%d>, char *tenant = <%s>, jwt_t **jwt = <%p>",
        security_handler, security_profile, tenant, jwt);

    // Allocate memory for JWT handle.
    MCL_NEW(*jwt);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != *jwt, MCL_OUT_OF_MEMORY, "Not enough memory to allocate jwt!");

    (*jwt)->header = MCL_NULL;
    (*jwt)->payload = MCL_NULL;
    (*jwt)->security_handler = security_handler;
    (*jwt)->security_profile = security_profile;

    // Create JWT header.
    code = _create_self_issued_jwt_header(security_profile, &((*jwt)->header));
    MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, jwt_destroy(jwt), code, "Header of JWT can not be created.");

    // Create JWT payload.
    code = _create_self_issued_jwt_payload(security_handler, tenant, *jwt);
    MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, jwt_destroy(jwt), code, "Payload of JWT can not be created.");

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

char *jwt_get_token(jwt_t *jwt)
{
    mcl_error_t code;
    char *token = MCL_NULL;
    char *header_json = MCL_NULL;
    char *payload_json = MCL_NULL;

    MCL_DEBUG_ENTRY("jwt_t *jwt = <%p>", jwt);

    code = mcl_json_util_to_string(jwt->header, &header_json);

    if (MCL_OK == code)
    {
        code = mcl_json_util_to_string(jwt->payload, &payload_json);
    }

    if (MCL_OK == code)
    {
        code = _generate_token(jwt, header_json, payload_json, &token);
    }

    if (MCL_OK != code)
    {
        MCL_ERROR("Token could not be generated <%s>", mcl_core_return_code_strings[code]);
    }

    // Clean up.
    MCL_FREE(header_json);
    MCL_FREE(payload_json);

    MCL_DEBUG_LEAVE("retVal = <%p>", token);
    return token;
}

void jwt_destroy(jwt_t **jwt)
{
    MCL_DEBUG_ENTRY("jwt_t **jwt = <%p>", jwt);

    if (MCL_NULL != *jwt)
    {
        if (MCL_NULL != (*jwt)->header)
        {
            json_util_destroy(&(*jwt)->header);
        }

        if (MCL_NULL != (*jwt)->payload)
        {
            json_util_destroy(&(*jwt)->payload);
        }

        MCL_FREE(*jwt);

        MCL_DEBUG("JWT handle is destroyed.");
    }
    else
    {
        MCL_DEBUG("JWT handle is already NULL.");
    }

    MCL_DEBUG_LEAVE("retVal = void");
}

static mcl_error_t _create_self_issued_jwt_header(E_MCL_SECURITY_PROFILE security_profile, mcl_json_t **header)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("E_MCL_SECURITY_PROFILE security_profile = <%d>, mcl_json_t **header = <%p>", security_profile, header);

    code = json_util_initialize(MCL_JSON_OBJECT, header);

    if (MCL_SECURITY_SHARED_SECRET == security_profile)
    {
        if (MCL_OK == code)
        {
            code = json_util_add_string(*header, JWT_HEADER_NAME_TYPE, JWT_HEADER_VALUE_TYP);
        }

        if (MCL_OK == code)
        {
            code = json_util_add_string(*header, JWT_HEADER_NAME_ALG, JWT_HEADER_VALUE_ALG_SHARED_SECRET);
        }
    }
    else
    {
        // It has to be MCL_SECURITY_RSA_3072.
        if (MCL_OK == code)
        {
            code = json_util_add_string(*header, JWT_HEADER_NAME_TYPE, JWT_HEADER_VALUE_TYP);
        }

        if (MCL_OK == code)
        {
            code = json_util_add_string(*header, JWT_HEADER_NAME_ALG, JWT_HEADER_VALUE_ALG_RSA_3072);
        }
    }

    if (MCL_OK != code)
    {
        json_util_destroy(header);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _create_self_issued_jwt_payload(security_handler_t *security_handler, char *tenant, jwt_t *jwt)
{
    mcl_error_t code;
    mcl_time_t current_time;
    char *jti;
    mcl_json_t *payload = MCL_NULL;

    MCL_DEBUG_ENTRY("security_handler_t *security_handler = <%p>, char *tenant = <%s>, jwt_t *jwt = <%p>", security_handler, tenant, jwt);

    // Create JTI.
    code = security_handler_generate_jti(&jti);
    MCL_ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Generate jti from security_handler failed!");

    // Initialize the payload json object.
    code = json_util_initialize(MCL_JSON_OBJECT, &payload);
    MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, MCL_FREE(jti), code, "JSON payload can not be initialized.");

    // Get current time in seconds.
    time_util_get_time(&current_time);

    // Fill payload json object.
    code = json_util_add_string(payload, JWT_PAYLOAD_NAME_ISS, security_handler->client_id);

    if (MCL_OK == code)
    {
        code = json_util_add_string(payload, JWT_PAYLOAD_NAME_SUB, security_handler->client_id);
    }

    if (MCL_OK == code)
    {
        code = json_util_add_string(payload, JWT_PAYLOAD_NAME_AUD, JWT_PAYLOAD_VALUE_AUD);
    }

    if (MCL_OK == code)
    {
        code = json_util_add_double(payload, JWT_PAYLOAD_NAME_IAT, (double) current_time);
    }

    if (MCL_OK == code)
    {
        code = json_util_add_double(payload, JWT_PAYLOAD_NAME_NBF, (double) current_time);
    }

    if (MCL_OK == code)
    {
        mcl_time_t expiration = current_time + JWT_EXPIRATION_TIME;
        code = json_util_add_double(payload, JWT_PAYLOAD_NAME_EXP, (double) expiration);
    }

    if (MCL_OK == code)
    {
        code = _add_schema_to_jwt(payload);
    }

    if (MCL_OK == code)
    {
        code = json_util_add_string(payload, JWT_PAYLOAD_NAME_TEN, tenant);
    }

    if (MCL_OK == code)
    {
        code = json_util_add_string(payload, JWT_PAYLOAD_NAME_JTI, jti);
    }

    if (MCL_OK == code)
    {
        jwt->payload = payload;
        jwt->issued_at = current_time;
    }
    else
    {
        json_util_destroy(&payload);
    }
    MCL_FREE(jti);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _generate_token(jwt_t *jwt, char *header, char *payload, char **token)
{
    char *header_encoded;
    char *payload_encoded;
    char *header_and_payload = MCL_NULL;
    char *signature = MCL_NULL;

    MCL_DEBUG_ENTRY("jwt_t *jwt = <%p>, char *header = <%s>, char *payload = <%s>, char **token = <%p>", jwt, header, payload, token);

    // Encode header and payload to base64 url.
    MCL_ASSERT_CODE_MESSAGE(MCL_OK == _get_header_and_payload_encoded_base64_url(header, payload, &header_encoded, &payload_encoded), MCL_FAIL,
        "Encode header - payload failed!");

    // Join header and payload. This joined header and payload must not be lost, it will be used again.
    MCL_ASSERT_CODE_MESSAGE(MCL_OK == _join_with_dot(header_encoded, payload_encoded, &header_and_payload), MCL_FAIL, "Join header and payload failed!");

    // Calculate the base64 url encoded signature from header and payload joined together.
    if (MCL_OK != _calculate_signature(jwt, header_and_payload, &signature))
    {
        MCL_FREE(header_and_payload);
        MCL_ERROR_RETURN(MCL_FAIL, "Generate signature failed!");
    }

    // Join signature with header and payload to get the final token.
    MCL_ASSERT_CODE_MESSAGE(MCL_OK == _join_with_dot(header_and_payload, signature, token), MCL_FAIL, "Joining header and payload with signature failed!");

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static mcl_error_t _get_header_and_payload_encoded_base64_url(char *header, char *payload, char **header_encoded, char **payload_encoded)
{
    MCL_DEBUG_ENTRY("char *header = <%s>, char *payload = <%s>, char **header_encoded = <%p>, char **payload_encoded = <%p>",
        header, payload, header_encoded, payload_encoded);

    // Encode header.
    MCL_ASSERT_CODE_MESSAGE(MCL_OK == security_handler_base64_url_encode((mcl_uint8_t *) header, string_util_strlen(header), header_encoded),
        MCL_FAIL, "Header encoding failed!");

    // Encode payload.
    MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == security_handler_base64_url_encode((mcl_uint8_t *) payload, string_util_strlen(payload), payload_encoded),
        MCL_FREE(*header_encoded), MCL_FAIL, "Payload encoding failed!");

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static mcl_error_t _join_with_dot(char *string_a, char *string_b, char **joined)
{
    mcl_error_t code = MCL_OK;
    char *dot = ".";
    mcl_size_t dot_length = 1;
    mcl_size_t joined_string_length;

    MCL_DEBUG_ENTRY("char *string_a = <%s>, char *string_b = <%s>, char **joined = <%p>", string_a, string_b, joined);

    // Allocate memory for joined string.
    joined_string_length = string_util_strlen(string_a) + dot_length + string_util_strlen(string_b);
    *joined = MCL_MALLOC(joined_string_length + MCL_NULL_CHAR_SIZE);

    if (MCL_NULL != *joined)
    {
        // Copy "string a" to target string.
        string_util_memcpy(*joined, string_a, string_util_strlen(string_a));

        // Copy "." to target string.
        string_util_memcpy(*joined + string_util_strlen(string_a), dot, dot_length);

        // Copy "string b" to target string with null termination.
        string_util_memcpy(*joined + string_util_strlen(string_a) + dot_length, string_b, string_util_strlen(string_b) + MCL_NULL_CHAR_SIZE);
    }
    else
    {
        code = MCL_OUT_OF_MEMORY;
    }

    // Destroy strings a and b.
    MCL_FREE(string_a);
    MCL_FREE(string_b);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _calculate_signature(jwt_t *jwt, char *header_and_payload, char **signature)
{
    mcl_error_t code;
    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size = 0;

    MCL_DEBUG_ENTRY("jwt_t *jwt = <%p>, char *header_and_payload = <%s>, char **signature = <%p>", jwt, header_and_payload, signature);

    // Calculate HMAC SHA256 of header and payload.
    if (MCL_SECURITY_SHARED_SECRET == jwt->security_profile)
    {
        code = security_handler_hmac_sha256(jwt->security_handler, (mcl_uint8_t *) header_and_payload, string_util_strlen(header_and_payload),
            &hash, &hash_size);
    }
    else
    {
        code = security_handler_rsa_sign(jwt->security_handler->rsa.private_key, header_and_payload, string_util_strlen(header_and_payload), &hash, &hash_size);
    }
    MCL_ASSERT_CODE_MESSAGE(MCL_OK == code, MCL_FAIL, "Can not sign JWT!");

    // Encode calculated signature.
    code = security_handler_base64_url_encode(hash, hash_size, signature);

    // Free hash, not needed anymore.
    MCL_FREE(hash);

    // Check encode operation.
    MCL_ASSERT_CODE_MESSAGE(MCL_OK == code, MCL_FAIL, "Encode calculated signature failed!");

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static mcl_error_t _add_schema_to_jwt(mcl_json_t *payload)
{
    mcl_json_t *schemas_array;
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_json_t *payload = <%p>", payload);

    code = json_util_start_array(payload, JWT_PAYLOAD_NAME_SCHEMAS, &schemas_array);

    if (MCL_OK == code)
    {
        code = json_util_add_string(schemas_array, MCL_NULL, JWT_PAYLOAD_VALUE_SCHEMAS);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
