/*!
 * @file     security_handler.c
 * @brief    Security handler module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "security_handler.h"
#include "security.h"
#include "base64.h"
#include "hmac.h"
#include "random.h"
#include "string_util.h"
#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_memory.h"

mcl_error_t security_handler_initialize(security_handler_t **security_handler)
{
    MCL_DEBUG_ENTRY("security_handler_t **security_handler = <%p>", security_handler);

    // Create security handler.
    MCL_NEW(*security_handler);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != *security_handler, MCL_OUT_OF_MEMORY, "Memory can not be allocated for security handler.");

    // Memory for members of security_handler_t will be allocated by the key generator function.
    (*security_handler)->rsa.private_key = MCL_NULL;
    (*security_handler)->rsa.public_key = MCL_NULL;
    (*security_handler)->last_token_time = MCL_NULL;
    (*security_handler)->access_token = MCL_NULL;
    (*security_handler)->registration_access_token = MCL_NULL;
    (*security_handler)->client_secret = MCL_NULL;
    (*security_handler)->registration_uri = MCL_NULL;
    (*security_handler)->client_id = MCL_NULL;

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

mcl_error_t security_handler_generate_jti(char **jti)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("char **jti = <%p>", jti);

    code = random_generate_guid(jti);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t security_handler_hash_sha256(const mcl_uint8_t *data, mcl_size_t data_size, mcl_uint8_t **hash, mcl_size_t *hash_size)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("const mcl_uint8_t *data = <%p>, mcl_size_t data_size = <%u>, mcl_uint8_t **hash = <%p>, mcl_size_t *hash_size = <%p>",
        data, data_size, hash, hash_size);

    code = security_hash_sha256(data, data_size, hash, hash_size);
    if (MCL_OK == code)
    {
        MCL_DEBUG("SHA256 calculation succeeded: hash = <%p>, hash_size = <%u>", *hash, *hash_size);
    }
    else
    {
        MCL_ERROR("SHA256 calculation failed!");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t security_handler_hmac_sha256(security_handler_t *security_handler, const mcl_uint8_t *data, mcl_size_t data_size,
    mcl_uint8_t **hash, mcl_size_t *hash_size)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("security_handler_t *security_handler = <%p>, const mcl_uint8_t *data = <%p>, mcl_size_t data_size = <%u>, "\
        "mcl_uint8_t **hash = <%p>, mcl_size_t *hash_size = <%p>", security_handler, data, data_size, hash, hash_size);

    code = hmac_sha256((const mcl_uint8_t *) data, data_size, (const mcl_uint8_t *) security_handler->client_secret,
        string_util_strlen(security_handler->client_secret), hash, hash_size);

    if (MCL_OK == code)
    {
        MCL_DEBUG("HMAC SHA256 calculation succeeded: hash = <%p>, hash_size = <%u>", *hash, *hash_size);
    }
    else
    {
        MCL_ERROR("HMAC SHA256 calculation failed!");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t security_handler_base64_encode(const mcl_uint8_t *data, mcl_size_t data_size, char **encoded_data)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("const mcl_uint8_t *data = <%p>, mcl_size_t data_size = <%u>, char **encoded_data = <%p>", data, data_size, encoded_data);

    code = base64_encode(data, data_size, encoded_data);

    if (MCL_OK == code)
    {
        MCL_DEBUG("Base64 encoding succeeded: encoded_data = <%s>, length encoded_data = <%u>", (*encoded_data), string_util_strlen(*encoded_data));
    }
    else
    {
        MCL_ERROR("Base64 encoding failed!");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t security_handler_base64_decode(const char *encoded_data, mcl_uint8_t **decoded_data, mcl_size_t *decoded_data_size)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("const char *encoded_data = <%p>, mcl_uint8_t **decoded_data = <%p>, mcl_size_t *decoded_data_size = <%p>",
        encoded_data, decoded_data, decoded_data_size);

    code = base64_decode(encoded_data, decoded_data, decoded_data_size);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t security_handler_base64_url_encode(const mcl_uint8_t *data, mcl_size_t data_size, char **encoded_data)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("const mcl_uint8_t *data = <%p>, mcl_size_t data_size = <%u>, char **encoded_data = <%p>", data, data_size, encoded_data);

    code = base64_url_encode(data, data_size, encoded_data);

    if (MCL_OK == code)
    {
        MCL_DEBUG("Base64 URL encoding succeeded: encoded_data = <%s>, length encoded_data = <%u>", (*encoded_data), string_util_strlen(*encoded_data));
    }
    else
    {
        MCL_ERROR("Base64 encoding failed!");
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t security_handler_generate_rsa_key(security_handler_t *security_handler)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("security_handler_t *security_handler = <%p>", security_handler);

    code = security_generate_rsa_key(&security_handler->rsa.public_key, &security_handler->rsa.private_key);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t security_handler_rsa_sign(char *rsa_key, char *data, mcl_size_t data_size, mcl_uint8_t **signature, mcl_size_t *signature_size)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("char *rsa_key = <%p>, char *data = <%p>, mcl_size_t data_size = <%u>, mcl_uint8_t **signature = <%p>, mcl_size_t *signature_size = <%p>",
        rsa_key, data, data_size, signature, signature_size);

    code = security_rsa_sign(rsa_key, data, data_size, signature, signature_size);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void security_handler_destroy(security_handler_t **security_handler)
{
    MCL_DEBUG_ENTRY("security_handler_t **security_handler = <%p>", security_handler);

    if (MCL_NULL != *security_handler)
    {
        // Free security handler with its members.
        MCL_FREE((*security_handler)->access_token);
        MCL_FREE((*security_handler)->registration_access_token);
        MCL_FREE((*security_handler)->registration_uri);
        MCL_FREE((*security_handler)->client_id);
        MCL_FREE((*security_handler)->last_token_time);
        MCL_FREE((*security_handler)->client_secret);
        MCL_FREE((*security_handler)->rsa.private_key);
        MCL_FREE((*security_handler)->rsa.public_key);
        MCL_FREE(*security_handler);

        MCL_DEBUG("Security handler is destroyed.");
    }
    else
    {
        MCL_DEBUG("Security handler is already NULL.");
    }

    MCL_DEBUG_LEAVE("retVal = void");
}
