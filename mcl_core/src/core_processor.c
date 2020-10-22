/*!
 * @file     core_processor.c
 * @brief    Core processor module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "core_processor.h"
#include "http_definitions.h"
#include "json_util.h"
#include "list.h"
#include "definitions.h"
#include "jwt.h"
#include "security.h"
#include "random.h"
#include "string_util.h"
#include "time_util.h"
#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_core_common.h"
#include "mcl_core/mcl_file_util.h"
#include "mcl_core/mcl_memory.h"

#define JSON_NAME_CLIENT_ID "client_id"
#define JSON_NAME_TOKEN_ENDPOINT_AUTH_METHOD "token_endpoint_auth_method"
#define JSON_NAME_GRANT_TYPES "grant_types"
#define JSON_NAME_CLIENT_SECRET "client_secret"
#define JSON_NAME_CLIENT_SECRET_EXPIRES_AT "client_secret_expires_at"
#define JSON_NAME_REGISTRATION_ACCESS_TOKEN "registration_access_token"
#define JSON_NAME_REGISTRATION_CLIENT_URI "registration_client_uri"
#define JSON_NAME_JWKS "jwks"
#define JSON_NAME_KEYS "keys"
#define JSON_NAME_E "e"
#define JSON_NAME_N "n"
#define JSON_NAME_KTY "kty"
#define JSON_NAME_KID "kid"
#define JSON_NAME_ACCESS_TOKEN "access_token"

#define REGISTER_URI_PATH "/register"
#define ACCESS_TOKEN_URI_PATH "/token"

#define CORRELATION_ID_BYTE_LENGTH 16

static const char _bearer_format[] = "Bearer %s";
static const char _client_id_format[] = "{\"client_id\":\"%s\"}";
static const char _string_identifier[] = "%s";

/**
 * Type of URIs for different endpoints.
 */
typedef enum E_ENDPOINT_URI
{
    ENDPOINT_URI_ACCESS_TOKEN,  //!< Uri for access token endpoint.
    ENDPOINT_URI_REGISTER,      //!< Uri for registration endpoint.
    ENDPOINT_URI_END            //!< End of uri endpoint.
} E_ENDPOINT_URI;

/**
 * Content types.
 *
 * This is used with http_header_values[E_HTTP_HEADER_VALUES] to get the related string.
 * ex: http_header_values[HTTP_HEADER_CONTENT_TYPE_APPLICATION_OCTET_STREAM] returns the string of "application/octet-stream".
 */
typedef enum E_CONTENT_TYPE_VALUES
{
    CONTENT_TYPE_APPLICATION_JSON,         //!< Content type is application json.
    CONTENT_TYPE_URL_ENCODED,              //!< Content type is application/x-www-form-urlencoded.
    CONTENT_TYPE_VALUES_END                //!< End of content type values.
} E_CONTENT_TYPE_VALUES;

// This function processes the HTTP response to an onboarding or key rotation request for shared secret security profile.
static mcl_error_t _process_registration_response_shared_secret(core_processor_t *core_processor, mcl_http_response_t *http_response);

// This function processes the HTTP response to an onboarding or key rotation request for shared RSA 3072 security profile.
static mcl_error_t _process_registration_response_rsa_3072(core_processor_t *core_processor, mcl_http_response_t *http_response);

// This is the array for endpoints.
static const char *endpoint_uri[ENDPOINT_URI_END] =
{
    "/api/agentmanagement/v3/oauth/token",
    "/api/agentmanagement/v3/register"
};

// Content type values.
static const char *_content_type_values[CONTENT_TYPE_VALUES_END] =
{
    "application/json",
    "application/x-www-form-urlencoded"
};

// Composes JSON string for onboarding with RSA security profile.
static mcl_error_t _compose_rsa_onboarding_json(security_handler_t *security_handler, char **payload);

// Composes JSON string for key rotation with RSA security profile.
static mcl_error_t _compose_rsa_key_rotation_json(security_handler_t *security_handler, char **payload);

// Adds key json object to keys json array.
static mcl_error_t _add_key_to_keys_array(mcl_json_t *root, mcl_json_t **json_object);

// Use custom function for loading register info.
static mcl_error_t _custom_load_register_info(core_processor_t *core_processor);
static mcl_error_t _evaluate_response_codes(mcl_http_response_t *response);
static mcl_error_t _generate_correlation_id_string(char **correlation_id);

// Saves credentials.
static mcl_error_t _save_credentials(core_processor_t *core_processor);

// Compose the payload for the request to be sent to /token endpoint.
static mcl_error_t _compose_access_token_request_payload(core_processor_t *core_processor, char **request_payload);

// Function to load initial credentials.
static mcl_error_t _load_initial_credentials(core_processor_t *core_processor);

// Function to process registration response.
static mcl_error_t _process_registration_response(core_processor_t *core_processor, mcl_http_response_t *http_response, char *correlation_id);

// Function to check if client secret is already up to date or not.
static mcl_error_t _check_client_secret(core_processor_t *core_processor, char *registration_access_token, char *client_secret);

// Function to check if RSA private key is already up to date or not.
static mcl_error_t _check_rsa_private_key(core_processor_t *core_processor, char *private_key);

mcl_error_t core_processor_initialize(core_configuration_t *configuration, core_processor_t **core_processor)
{
    // Return code for the functions to be called.
    mcl_error_t return_code;

    // We need http_client_configuration temporarily, no need for dynamic allocation.
    mcl_http_client_configuration_t http_client_configuration;

    MCL_DEBUG_ENTRY("core_configuration_t *configuration = <%p>, core_processor_t **core_processor = <%p>", configuration, core_processor);

    // Initialize json module.
    json_util_initialize_json_library();

    // Initialize security module.
    return_code = security_initialize();
    MCL_ASSERT_CODE_MESSAGE(MCL_OK == return_code, return_code, "Security initialization failed.");

    // Create core processor handle.
    MCL_NEW(*core_processor);
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != *core_processor, MCL_OUT_OF_MEMORY, "Memory can not be allocated for core processor.");

    // Make sure pointer members of core_processor which will be created inside this function are null initially.
    // This is necessary for an unexpected call to core_processor_destroy() function.
    (*core_processor)->http_client = MCL_NULL;
    (*core_processor)->security_handler = MCL_NULL;

    // Set pointer to configuration parameters.
    (*core_processor)->configuration = configuration;

    // Initialize http client configuration.
    http_client_configuration.certificate = configuration->mindsphere_certificate;
    http_client_configuration.proxy_hostname = configuration->proxy_hostname;
    http_client_configuration.proxy_username = configuration->proxy_username;
    http_client_configuration.proxy_domain = configuration->proxy_domain;
    http_client_configuration.user_agent = configuration->user_agent;
    http_client_configuration.proxy_password = configuration->proxy_password;
    http_client_configuration.http_request_timeout = configuration->http_request_timeout;
    http_client_configuration.port = configuration->mindsphere_port;
    http_client_configuration.proxy_port = configuration->proxy_port;
    http_client_configuration.proxy_type = configuration->proxy_type;
    http_client_configuration.certificate_is_file = configuration->certificate_is_file;

    // Initialize http client.
    return_code = mcl_http_client_initialize(&http_client_configuration, &((*core_processor)->http_client));
    MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, core_processor_destroy(core_processor), return_code, "Http client initialization failed.");
    MCL_DEBUG("Http client is successfully initialized.");

    // Initialize security handler.
    return_code = security_handler_initialize(&((*core_processor)->security_handler));
    MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == return_code, core_processor_destroy(core_processor), return_code, "Security handler initialization failed.");
    MCL_DEBUG("Security handler is successfully initialized.");

    // Load credentials via custom functions if both function pointers are not null.
    if ((MCL_NULL != (*core_processor)->configuration->credentials_load_callback.rsa) && (MCL_NULL != (*core_processor)->configuration->credentials_save_callback.rsa))
    {
        return_code = _custom_load_register_info(*core_processor);

        if (MCL_OK == return_code)
        {
            MCL_INFO("MCL is initialized with the credentials provided by the callback function.");
        }
        else if (MCL_CREDENTIALS_NOT_LOADED == return_code)
        {
            return_code = _load_initial_credentials(*core_processor);
        }
        else
        {
            MCL_INFO("Credentials could not be loaded.");
            core_processor_destroy(core_processor);
        }
    }
    else
    {
        return_code = _load_initial_credentials(*core_processor);
    }

    // Populate endpoints in configuration.
    if (MCL_OK == return_code)
    {
        return_code = string_util_concatenate((*core_processor)->configuration->mindsphere_hostname, endpoint_uri[ENDPOINT_URI_REGISTER], &(*core_processor)->configuration->register_endpoint);
    }

    if (MCL_OK == return_code)
    {
        return_code = string_util_concatenate((*core_processor)->configuration->mindsphere_hostname, endpoint_uri[ENDPOINT_URI_ACCESS_TOKEN], &(*core_processor)->configuration->token_endpoint);
    }

    if (MCL_OK != return_code)
    {
        core_processor_destroy(core_processor);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", return_code);
    return return_code;
}

static mcl_error_t _load_initial_credentials(core_processor_t *core_processor)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("core_processor_t *core_processor = <%p>", core_processor);

    if (MCL_NULL == core_processor->configuration->initial_access_token)
    {
        MCL_ERROR("No credential is provided.");
        code = MCL_NO_ACCESS_TOKEN_PROVIDED;
    }
    else
    {
        // Generate RSA keys if RSA security profile is selected.
        if (MCL_SECURITY_RSA_3072 == core_processor->configuration->security_profile)
        {
            code = security_handler_generate_rsa_key(core_processor->security_handler);
            MCL_ASSERT_CODE_MESSAGE(MCL_OK == code, code, "RSA keys can not be generated.");
            MCL_DEBUG("RSA keys are generated.");
        }
        MCL_INFO("MCL is initialized to onboard with initial access token.");
        code = MCL_OK;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t core_processor_register(core_processor_t *core_processor)
{
    mcl_error_t result = MCL_FAIL;
    mcl_uint32_t auth_header_value_length;
    mcl_http_request_t *http_request = MCL_NULL;
    mcl_http_response_t *http_response = MCL_NULL;
    char *auth_header_value = MCL_NULL;
    char *registration_uri = MCL_NULL;
    char *payload = MCL_NULL;
    char *correlation_id = MCL_NULL;
    char *token_for_auth = MCL_NULL;
    mcl_size_t payload_length;
    E_MCL_HTTP_METHOD http_method;

    MCL_DEBUG_ENTRY("core_processor_t *core_processor = <%p>", core_processor);

    // Determine if onboarding or key rotation is requested.
    if (MCL_NULL == core_processor->security_handler->registration_access_token)
    {
        // Compose payload of the request.
        if (MCL_SECURITY_SHARED_SECRET == core_processor->configuration->security_profile)
        {
            payload = MCL_MALLOC(2 + MCL_NULL_CHAR_SIZE);

            if (MCL_NULL != payload)
            {
                result = string_util_snprintf(payload, 2 + MCL_NULL_CHAR_SIZE, "{}");
            }
            else
            {
                result = MCL_OUT_OF_MEMORY;
            }
        }
        else if (MCL_SECURITY_RSA_3072 == core_processor->configuration->security_profile)
        {
            result = _compose_rsa_onboarding_json(core_processor->security_handler, &payload);
        }

        token_for_auth = core_processor->configuration->initial_access_token;
        http_method = MCL_HTTP_POST;
        registration_uri = core_processor->configuration->register_endpoint;
    }
    else
    {
        // Compose payload of the request.
        if (MCL_SECURITY_SHARED_SECRET == core_processor->configuration->security_profile)
        {
            // Payload length including null character size.
            payload_length = (mcl_uint32_t) ((sizeof(_client_id_format) - sizeof(_string_identifier)) + string_util_strlen(core_processor->security_handler->client_id) + MCL_NULL_CHAR_SIZE);
            payload = MCL_MALLOC(payload_length);
            if (MCL_NULL != payload)
            {
                result = string_util_snprintf(payload, payload_length, _client_id_format, core_processor->security_handler->client_id);
            }
            else
            {
                result = MCL_OUT_OF_MEMORY;
            }
        }
        else if (MCL_SECURITY_RSA_3072 == core_processor->configuration->security_profile)
        {
            result = _compose_rsa_key_rotation_json(core_processor->security_handler, &payload);
        }

        token_for_auth = core_processor->security_handler->registration_access_token;
        http_method = MCL_HTTP_PUT;
        registration_uri = core_processor->security_handler->registration_uri;
    }

    MCL_ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == result, MCL_FREE(payload), result, "Payload for register endpoint can not be constructed.");

    // Auth header value length including null character size.
    auth_header_value_length = (mcl_uint32_t) ((sizeof(_bearer_format) - sizeof(_string_identifier)) + string_util_strlen(token_for_auth) + MCL_NULL_CHAR_SIZE);
    auth_header_value = MCL_MALLOC(auth_header_value_length);

    if (MCL_NULL != auth_header_value)
    {
        result = string_util_snprintf(auth_header_value, auth_header_value_length, _bearer_format, token_for_auth);
    }
    else
    {
        MCL_FREE(payload);
        result = MCL_OUT_OF_MEMORY;
    }

    if (MCL_OK == result)
    {
        result = mcl_http_request_initialize(&http_request);
    }

    if (MCL_OK == result)
    {
        result = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_METHOD, &http_method);
    }

    if (MCL_OK == result)
    {
        result = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_URL, registration_uri);
    }

    if (MCL_OK == result)
    {
        result = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_BODY, payload);
    }

    if (MCL_OK == result)
    {
        payload_length = string_util_strlen(payload);
        result = mcl_http_request_set_parameter(http_request, MCL_HTTP_REQUEST_PARAMETER_BODY_SIZE, &payload_length);
    }

    // Compose headers of HTTP request.
    if (MCL_OK == result)
    {
        result = mcl_http_request_add_header(http_request, http_header_names[HTTP_HEADER_CONTENT_TYPE], _content_type_values[CONTENT_TYPE_APPLICATION_JSON]);
    }

    if (MCL_OK == result)
    {
        result = mcl_http_request_add_header(http_request, http_header_names[HTTP_HEADER_ACCEPT], _content_type_values[CONTENT_TYPE_APPLICATION_JSON]);
    }

    if (MCL_OK == result)
    {
        result = mcl_http_request_add_header(http_request, http_header_names[HTTP_HEADER_AUTHORIZATION], auth_header_value);
    }

    if (MCL_OK == result)
    {
        result = _generate_correlation_id_string(&correlation_id);
    }

    if (MCL_OK == result)
    {
        result = mcl_http_request_add_header(http_request, http_header_names[HTTP_HEADER_CORRELATION_ID], correlation_id);
    }

    // Send the request and get the response.
    if (MCL_OK == result)
    {
        // Send the onboarding request and retrieve the response.
        if (MCL_OK != (result = mcl_http_client_send(core_processor->http_client, http_request, &http_response)))
        {
            MCL_ERROR("HTTP request can not be sent to the server.");
        }
        else
        {
            result = _process_registration_response(core_processor, http_response, correlation_id);
        }
    }

    // Clean up.
    MCL_FREE(correlation_id);
    MCL_FREE(payload);
    MCL_FREE(auth_header_value);
    mcl_http_request_destroy(&http_request);
    mcl_http_response_destroy(&http_response);

    MCL_DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

static mcl_error_t _process_registration_response(core_processor_t *core_processor, mcl_http_response_t *http_response, char *correlation_id)
{
    mcl_error_t code;
    mcl_error_t server_response;
    mcl_bool_t is_onboard_request;

    MCL_DEBUG_ENTRY("core_processor_t *core_processor = <%p>, mcl_http_response_t *http_response = <%p>, char *correlation_id = <%p> ", core_processor, http_response, correlation_id);

    server_response = _evaluate_response_codes(http_response);
    is_onboard_request = (MCL_NULL == core_processor->security_handler->registration_access_token) ? MCL_TRUE : MCL_FALSE;

    if (MCL_TRUE == is_onboard_request)
    {
        if (MCL_OK == server_response)
        {
            // HTTP 201 should have been received instead of 200.
            code = MCL_UNEXPECTED_RESULT_CODE;
        }
        else if (MCL_CREATED == server_response)
        {
            code = MCL_OK;
        }
        else
        {
            code = server_response;
        }
    }
    else
    {
        code = server_response;
    }

    if (MCL_OK == code)
    {
        MCL_INFO("Correlation-ID = \"%s\"", correlation_id);

        if (MCL_SECURITY_SHARED_SECRET == core_processor->configuration->security_profile)
        {
            code = _process_registration_response_shared_secret(core_processor, http_response);
        }
        else
        {
            code = _process_registration_response_rsa_3072(core_processor, http_response);
        }

        if (MCL_OK == code)
        {
            code = _save_credentials(core_processor);
        }
    }
    else
    {
        if (MCL_TRUE == is_onboard_request)
        {
            MCL_ERROR("Onboarding failed.");
        }
        else
        {
            MCL_ERROR("Key rotation failed.");
        }

        MCL_ERROR("HTTP <%d> received from server for the request with correlation-id = \"%s\".", http_response->status_code, correlation_id);

        if (MCL_NULL != http_response->payload)
        {
            MCL_ERROR("HTTP Response:\n%.*s", http_response->payload_size, http_response->payload);
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t core_processor_update_credentials(core_processor_t *core_processor)
{
    mcl_error_t code;
    mcl_size_t length;
    mcl_bool_t callbacks_are_used;

    // These will be used in any security profile.
    char *client_id = MCL_NULL;
    char *registration_access_token = MCL_NULL;
    char *registration_uri = MCL_NULL;

    // For shared secret.
    char *client_secret = MCL_NULL;

    // For RSA 3072.
    char *public_key = MCL_NULL;
    char *private_key = MCL_NULL;

    MCL_DEBUG_ENTRY("core_processor_t *core_processor = <%p>", core_processor);

    callbacks_are_used = (MCL_NULL != core_processor->configuration->credentials_load_callback.shared_secret) && (MCL_NULL != core_processor->configuration->credentials_save_callback.shared_secret);
    MCL_ASSERT_CODE_MESSAGE(MCL_TRUE == callbacks_are_used, MCL_FAIL, "There is no way to update credentials.");

    if (MCL_SECURITY_SHARED_SECRET == core_processor->configuration->security_profile)
    {
        code = core_processor->configuration->credentials_load_callback.shared_secret(&client_id, &client_secret, &registration_access_token, &registration_uri);
    }
    else
    {
        code = core_processor->configuration->credentials_load_callback.rsa(&client_id, &public_key, &private_key, &registration_access_token, &registration_uri);
    }

    // If loading credentials is failed, user must have set all parameters to NULL. No need to clean up.
    MCL_ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Security information cannot be loaded.");

    // Registration URI will not be used.
    MCL_FREE(registration_uri);

    // Check whether client id is correct.
    length = string_util_strlen(core_processor->security_handler->client_id);
    code = MCL_FAIL;

    if (length == string_util_strlen(client_id))
    {
        code = string_util_strncmp(client_id, core_processor->security_handler->client_id, length);
    }

    MCL_FREE(client_id);

    if (MCL_OK == code)
    {
        if (MCL_SECURITY_SHARED_SECRET == core_processor->configuration->security_profile)
        {
            code = _check_client_secret(core_processor, registration_access_token, client_secret);
        }
        else
        {
            code = _check_rsa_private_key(core_processor, private_key);
        }

        // If code == MCL_OK, client secret is not different from what we have.
        code = (MCL_OK == code) ? MCL_CREDENTIALS_UP_TO_DATE : MCL_OK;
    }

    if (MCL_OK == code)
    {
        // Everything is fine, now we can replace old information.
        // There is no need to check security profile, unrelated parameters will be just reassigned to NULL.
        MCL_FREE((core_processor->security_handler->registration_access_token));
        MCL_FREE((core_processor->security_handler->client_secret));
        MCL_FREE(core_processor->security_handler->rsa.public_key);
        MCL_FREE(core_processor->security_handler->rsa.private_key);

        core_processor->security_handler->registration_access_token = registration_access_token;
        core_processor->security_handler->client_secret = client_secret;
        core_processor->security_handler->rsa.public_key = public_key;
        core_processor->security_handler->rsa.private_key = private_key;
    }
    else
    {
        MCL_FREE(registration_access_token);
        MCL_FREE(client_secret);
        MCL_FREE(public_key);
        MCL_FREE(private_key);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _check_client_secret(core_processor_t *core_processor, char *registration_access_token, char *client_secret)
{
    mcl_size_t length;
    mcl_error_t code = MCL_FAIL;

    MCL_DEBUG_ENTRY("core_processor_t *core_processor = <%p>, char *registration_access_token = <%p>, char *client_secret = <%p>", core_processor, registration_access_token, client_secret);

    length = string_util_strlen(core_processor->security_handler->client_secret);

    if (string_util_strlen(client_secret) == length)
    {
        code = string_util_strncmp(client_secret, core_processor->security_handler->client_secret, length);
    }

    if (MCL_OK == code)
    {

        length = string_util_strlen(core_processor->security_handler->registration_access_token);
        code = MCL_FAIL;

        if (string_util_strlen(registration_access_token) == length)
        {
            code = string_util_strncmp(registration_access_token, core_processor->security_handler->registration_access_token, length);
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _check_rsa_private_key(core_processor_t *core_processor, char *private_key)
{
    mcl_size_t length;
    mcl_error_t code = MCL_FAIL;

    MCL_DEBUG_ENTRY("core_processor_t *core_processor = <%p>, char *private_key = <%p>", core_processor, private_key);

    length = string_util_strlen(core_processor->security_handler->rsa.private_key);

    if (string_util_strlen(private_key) == length)
    {
        code = string_util_strncmp(private_key, core_processor->security_handler->rsa.private_key, length);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t core_processor_get_access_token(core_processor_t *core_processor)
{
    mcl_error_t code;
    char *request_payload = MCL_NULL;
    char *correlation_id = MCL_NULL;
    mcl_http_request_t *request = MCL_NULL;
    mcl_http_response_t *response = MCL_NULL;
    char *server_time_header = MCL_NULL;
    mcl_json_t *response_payload_json = MCL_NULL;
    mcl_json_t *access_token = MCL_NULL;
    E_MCL_HTTP_METHOD http_method = MCL_HTTP_POST;

    MCL_DEBUG_ENTRY("core_processor_t *core_processor = <%p>", core_processor);

    // Create access token request payload.
    code = _compose_access_token_request_payload(core_processor, &request_payload);

    // Initialize HTTP request.
    if (MCL_OK == code)
    {
        code = mcl_http_request_initialize(&request);
    }

    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_METHOD, &http_method);
    }

    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_URL, core_processor->configuration->token_endpoint);
    }

    // Add headers to the request.
    if (MCL_OK == code)
    {
        code = mcl_http_request_add_header(request, http_header_names[HTTP_HEADER_CONTENT_TYPE], _content_type_values[CONTENT_TYPE_URL_ENCODED]);
    }

    if (MCL_OK == code)
    {
        code = _generate_correlation_id_string(&correlation_id);
    }

    if (MCL_OK == code)
    {
        code = mcl_http_request_add_header(request, http_header_names[HTTP_HEADER_CORRELATION_ID], correlation_id);
    }

    // Add payload to the request.
    if (MCL_OK == code)
    {
        code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_BODY, request_payload);
    }

    if (MCL_OK == code)
    {
        mcl_size_t payload_length = string_util_strlen(request_payload);
        code = mcl_http_request_set_parameter(request, MCL_HTTP_REQUEST_PARAMETER_BODY_SIZE, &payload_length);
    }

    // Send the request.
    if (MCL_OK == code)
    {
        code = mcl_http_client_send(core_processor->http_client, request, &response);
    }

    mcl_http_request_destroy(&request);
    MCL_FREE(request_payload);

    // Get server time which is an optional header in the response.
    if (MCL_OK == code)
    {
        mcl_error_t optional_field_code = mcl_http_response_get_header(response, http_header_names[HTTP_HEADER_SERVER_TIME], &server_time_header);

        if (MCL_OK == optional_field_code)
        {
            MCL_FREE((core_processor->security_handler->last_token_time));
            core_processor->security_handler->last_token_time = server_time_header;
        }

        code = _evaluate_response_codes(response);

        if (MCL_OK == code)
        {
            MCL_INFO("Correlation-ID = \"%s\"", correlation_id);
        }
        else
        {
            MCL_ERROR("Getting access token failed.");
            MCL_ERROR("HTTP <%d> received from server for the request with correlation-id = \"%s\".", response->status_code, correlation_id);

            if (MCL_NULL != response->payload)
            {
                MCL_ERROR("HTTP Response:\n%.*s", response->payload_size, response->payload);
            }
        }
    }

    MCL_FREE(correlation_id);

    // Parse the response to get access token.
    if (MCL_OK == code)
    {
        code = json_util_parse((char *)response->payload, response->payload_size, &response_payload_json);
    }

    mcl_http_response_destroy(&response);

    if (MCL_OK == code)
    {
        code = json_util_get_object_item(response_payload_json, JSON_NAME_ACCESS_TOKEN, &access_token);
    }

    MCL_FREE(core_processor->security_handler->access_token);

    if (MCL_OK == code)
    {
        code = json_util_get_string(access_token, &core_processor->security_handler->access_token);
    }

    json_util_destroy(&response_payload_json);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void core_processor_destroy(core_processor_t **core_processor)
{
    MCL_DEBUG_ENTRY("core_processor_t **core_processor = <%p>", core_processor);

    if (MCL_NULL != *core_processor)
    {
        // Destroy http client handler.
        mcl_http_client_destroy(&((*core_processor)->http_client));

        // Destroy security handler.
        security_handler_destroy(&((*core_processor)->security_handler));

        MCL_FREE((*core_processor)->configuration->token_endpoint);
        MCL_FREE((*core_processor)->configuration->register_endpoint);

        // Free core_processor handle.
        MCL_FREE(*core_processor);

        MCL_DEBUG("Core processor handle is destroyed.");
    }
    else
    {
        MCL_DEBUG("Core processor handle is already NULL.");
    }

    MCL_DEBUG_LEAVE("retVal = void");
}

mcl_error_t _process_registration_response_shared_secret(core_processor_t *core_processor, mcl_http_response_t *http_response)
{
    mcl_error_t code;
    mcl_json_t *json_root = MCL_NULL;
    mcl_json_t *client_id = MCL_NULL;
    mcl_json_t *client_secret = MCL_NULL;
    mcl_json_t *registration_access_token = MCL_NULL;
    mcl_json_t *registration_client_uri = MCL_NULL;

    MCL_DEBUG_ENTRY("core_processor_t *core_processor = <%p>, mcl_http_response_t *http_response = <%p>", core_processor, http_response);

    code = json_util_parse((char *)http_response->payload, http_response->payload_size, &json_root);

    if (MCL_OK == code)
    {
        code = json_util_get_object_item(json_root, JSON_NAME_CLIENT_ID, &client_id);
    }

    if (MCL_OK == code)
    {
        code = json_util_get_object_item(json_root, JSON_NAME_CLIENT_SECRET, &client_secret);
    }

    if (MCL_OK == code)
    {
        code = json_util_get_object_item(json_root, JSON_NAME_REGISTRATION_ACCESS_TOKEN, &registration_access_token);
    }

    if (MCL_OK == code)
    {
        code = json_util_get_object_item(json_root, JSON_NAME_REGISTRATION_CLIENT_URI, &registration_client_uri);
    }

    if (MCL_OK == code)
    {
        char *temp_client_id = MCL_NULL;
        char *temp_client_secret = MCL_NULL;
        char *temp_registration_access_token = MCL_NULL;
        char *temp_registration_uri = MCL_NULL;

        code = json_util_get_string(client_id, &temp_client_id);

        if (MCL_OK == code)
        {
            code = json_util_get_string(client_secret, &temp_client_secret);
        }

        if (MCL_OK == code)
        {
            code = json_util_get_string(registration_access_token, &temp_registration_access_token);
        }

        if (MCL_OK == code)
        {
            code = json_util_get_string(registration_client_uri, &temp_registration_uri);
        }

        // If everything is fine, we can replace credentials.
        if (MCL_OK == code)
        {
            MCL_FREE(core_processor->security_handler->client_id);
            MCL_FREE(core_processor->security_handler->client_secret);
            MCL_FREE(core_processor->security_handler->registration_access_token);
            MCL_FREE(core_processor->security_handler->registration_uri);

            core_processor->security_handler->client_id = temp_client_id;
            core_processor->security_handler->client_secret = temp_client_secret;
            core_processor->security_handler->registration_access_token = temp_registration_access_token;
            core_processor->security_handler->registration_uri = temp_registration_uri;
        }
        else
        {
            MCL_FREE(temp_client_id);
            MCL_FREE(temp_client_secret);
            MCL_FREE(temp_registration_access_token);
            MCL_FREE(temp_registration_uri);
        }
    }

    json_util_destroy(&json_root);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t _process_registration_response_rsa_3072(core_processor_t *core_processor, mcl_http_response_t *http_response)
{
    mcl_error_t code;
    mcl_json_t *json_root = MCL_NULL;
    mcl_json_t *client_id = MCL_NULL;
    mcl_json_t *registration_access_token = MCL_NULL;
    mcl_json_t *registration_client_uri = MCL_NULL;

    MCL_DEBUG_ENTRY("core_processor_t *core_processor = <%p>, mcl_http_response_t *http_response = <%p>", core_processor, http_response);

    code = json_util_parse((char *)http_response->payload, http_response->payload_size, &json_root);

    if (MCL_OK == code)
    {
        code = json_util_get_object_item(json_root, JSON_NAME_CLIENT_ID, &client_id);
    }

    if (MCL_OK == code)
    {
        code = json_util_get_object_item(json_root, JSON_NAME_REGISTRATION_ACCESS_TOKEN, &registration_access_token);
    }

    if (MCL_OK == code)
    {
        code = json_util_get_object_item(json_root, JSON_NAME_REGISTRATION_CLIENT_URI, &registration_client_uri);
    }

    if (MCL_OK == code)
    {
        char *temp_client_id = MCL_NULL;
        char *temp_registration_access_token = MCL_NULL;
        char *temp_registration_uri = MCL_NULL;

        code = json_util_get_string(client_id, &temp_client_id);

        if (MCL_OK == code)
        {
            code = json_util_get_string(registration_access_token, &temp_registration_access_token);
        }

        if (MCL_OK == code)
        {
            code = json_util_get_string(registration_client_uri, &temp_registration_uri);
        }

        // If everything is fine, we can replace credentials.
        if (MCL_OK == code)
        {
            MCL_FREE(core_processor->security_handler->client_id);
            MCL_FREE(core_processor->security_handler->registration_access_token);
            MCL_FREE(core_processor->security_handler->registration_uri);

            core_processor->security_handler->client_id = temp_client_id;
            core_processor->security_handler->registration_access_token = temp_registration_access_token;
            core_processor->security_handler->registration_uri = temp_registration_uri;
        }
        else
        {
            MCL_FREE(temp_client_id);
            MCL_FREE(temp_registration_access_token);
            MCL_FREE(temp_registration_uri);
        }
    }

    // Child json objects are reference to their parts in root json. So we only free the struct.
    json_util_destroy(&json_root);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _compose_rsa_onboarding_json(security_handler_t * security_handler, char **payload)
{
    mcl_error_t code;
    mcl_json_t *root = MCL_NULL;
    mcl_json_t *jwks = MCL_NULL;
    mcl_json_t *keys = MCL_NULL;
    mcl_json_t *key = MCL_NULL;
    char *kid = MCL_NULL;
    char *modulus = MCL_NULL;
    char *public_exponent = MCL_NULL;

    MCL_DEBUG_ENTRY("security_handler_t *security_handler = <%p>, char **payload = <%p>", security_handler, payload);

    code = security_rsa_get_modulus_and_exponent(security_handler->rsa.public_key, &modulus, &public_exponent);

    if (MCL_OK == code)
    {
        code = json_util_initialize(MCL_JSON_OBJECT, &root);
    }

    if (MCL_OK == code)
    {
        code = json_util_start_object(root, JSON_NAME_JWKS, &jwks);
    }

    if (MCL_OK == code)
    {
        code = json_util_start_array(jwks, JSON_NAME_KEYS, &keys);
    }

    if (MCL_OK == code)
    {
        code = _add_key_to_keys_array(keys, &key);
    }

    if (MCL_OK == code)
    {
        code = json_util_add_string(key, JSON_NAME_E, public_exponent);
    }

    if (MCL_OK == code)
    {
        code = json_util_add_string(key, JSON_NAME_N, modulus);
    }

    if (MCL_OK == code)
    {
        code = json_util_add_string(key, JSON_NAME_KTY, "RSA");
    }

    if (MCL_OK == code)
    {
        code = random_generate_guid(&kid);
    }

    if (MCL_OK == code)
    {
        code = json_util_add_string(key, JSON_NAME_KID, kid);
    }

    if (MCL_OK == code)
    {
        code = json_util_to_string(root, payload);
    }

    if (MCL_OK != code)
    {
        MCL_FREE(*payload);
    }

    MCL_FREE(modulus);
    MCL_FREE(public_exponent);
    MCL_FREE(kid);
    json_util_destroy(&root);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _compose_rsa_key_rotation_json(security_handler_t * security_handler, char **payload)
{
    mcl_error_t code;
    mcl_json_t *root = MCL_NULL;
    mcl_json_t *jwks = MCL_NULL;
    mcl_json_t *keys = MCL_NULL;
    mcl_json_t *key = MCL_NULL;
    char *modulus = MCL_NULL;
    char *public_exponent = MCL_NULL;
    char *kid = MCL_NULL;

    MCL_DEBUG_ENTRY("security_handler_t *security_handler = <%p>, char **payload = <%p>", security_handler, payload);

    MCL_FREE(security_handler->rsa.public_key);
    MCL_FREE(security_handler->rsa.private_key);
    code = security_handler_generate_rsa_key(security_handler);

    if (MCL_OK == code)
    {
        code = security_rsa_get_modulus_and_exponent(security_handler->rsa.public_key, &modulus, &public_exponent);
    }

    if (MCL_OK == code)
    {
        code = json_util_initialize(MCL_JSON_OBJECT, &root);
    }

    if (MCL_OK == code)
    {
        code = json_util_add_string(root, JSON_NAME_CLIENT_ID, security_handler->client_id);
    }

    if (MCL_OK == code)
    {
        code = json_util_start_object(root, JSON_NAME_JWKS, &jwks);
    }

    if (MCL_OK == code)
    {
        code = json_util_start_array(jwks, JSON_NAME_KEYS, &keys);
    }

    if (MCL_OK == code)
    {
        code = _add_key_to_keys_array(keys, &key);
    }

    if (MCL_OK == code)
    {
        code = json_util_add_string(key, JSON_NAME_E, public_exponent);
    }

    if (MCL_OK == code)
    {
        code = json_util_add_string(key, JSON_NAME_N, modulus);
    }

    if (MCL_OK == code)
    {
        code = json_util_add_string(key, JSON_NAME_KTY, "RSA");
    }

    if (MCL_OK == code)
    {
        code = random_generate_guid(&kid);
    }

    if (MCL_OK == code)
    {
        code = json_util_add_string(key, JSON_NAME_KID, kid);
    }

    if (MCL_OK == code)
    {
        code = json_util_to_string(root, payload);
    }

    if (MCL_OK != code)
    {
        MCL_FREE(*payload);
    }

    MCL_FREE(modulus);
    MCL_FREE(public_exponent);
    MCL_FREE(kid);
    json_util_destroy(&root);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _add_key_to_keys_array(mcl_json_t *root, mcl_json_t **json_object)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("json_t *root = <%p>, mcl_json_t **json_object = <%p>", root, json_object);

    code = json_util_initialize(MCL_JSON_OBJECT, json_object);

    if (MCL_OK == code)
    {
        json_util_add_item_to_array(root, *json_object);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _custom_load_register_info(core_processor_t *core_processor)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("core_processor_t *core_processor = <%p>", core_processor);

    if (MCL_SECURITY_SHARED_SECRET == core_processor->configuration->security_profile)
    {
        code = core_processor->configuration->credentials_load_callback.shared_secret(&(core_processor->security_handler->client_id), &(core_processor->security_handler->client_secret), &(core_processor->security_handler->registration_access_token), &(core_processor->security_handler->registration_uri));
    }
    else
    {
        code = core_processor->configuration->credentials_load_callback.rsa(&(core_processor->security_handler->client_id), &(core_processor->security_handler->rsa.public_key), &(core_processor->security_handler->rsa.private_key), &(core_processor->security_handler->registration_access_token), &(core_processor->security_handler->registration_uri));
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _evaluate_response_codes(mcl_http_response_t *response)
{
    mcl_error_t code;

    MCL_DEBUG_ENTRY("mcl_http_response_t *response = <%p>", response);

    switch (response->status_code)
    {
        case MCL_HTTP_STATUS_CODE_SUCCESS:
            code = MCL_OK;
            break;

        case MCL_HTTP_STATUS_CODE_CREATED:
            code = MCL_CREATED;
            break;

        case MCL_HTTP_STATUS_CODE_PARTIAL_CONTENT:
            code = MCL_PARTIAL_CONTENT;
            break;

        case MCL_HTTP_STATUS_CODE_BAD_REQUEST:
            code = MCL_BAD_REQUEST;
            break;

        case MCL_HTTP_STATUS_CODE_UNAUTHORIZED:
            code = MCL_UNAUTHORIZED;
            break;

        case MCL_HTTP_STATUS_CODE_FORBIDDEN:
            code = MCL_FORBIDDEN;
            break;

        case MCL_HTTP_STATUS_CODE_NOT_FOUND:
            code = MCL_NOT_FOUND;
            break;

        case MCL_HTTP_STATUS_CODE_INTERNAL_SERVER_ERR:
            code = MCL_SERVER_FAIL;
            break;

        case MCL_HTTP_STATUS_CODE_CONFLICT:
            code = MCL_CONFLICT;
            break;

        case MCL_HTTP_STATUS_CODE_PRECONDITION_FAILED:
            code = MCL_PRECONDITION_FAIL;
            break;

        case MCL_HTTP_STATUS_CODE_PAYLOAD_TOO_LARGE:
            code = MCL_REQUEST_PAYLOAD_TOO_LARGE;
            break;

        default:
            code = MCL_UNEXPECTED_RESULT_CODE;
            MCL_INFO("Server responded with unexpected HTTP status code %d.", response->status_code);
            break;
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _generate_correlation_id_string(char **correlation_id)
{
    mcl_error_t code;
    mcl_uint16_t index;
    uint8_t random_bytes[CORRELATION_ID_BYTE_LENGTH] = { 0 };
    *correlation_id = MCL_NULL;

    MCL_DEBUG_ENTRY("char **correlation_id = <%p>", correlation_id);

    code = security_generate_random_bytes(random_bytes, CORRELATION_ID_BYTE_LENGTH);

    if (MCL_OK == code)
    {
        *correlation_id = MCL_MALLOC(CORRELATION_ID_BYTE_LENGTH * 2 + MCL_NULL_CHAR_SIZE);

        if (MCL_NULL == *correlation_id)
        {
            code = MCL_OUT_OF_MEMORY;
        }
    }

    for (index = 0; (index < CORRELATION_ID_BYTE_LENGTH) && (MCL_OK == code); ++index)
    {
        code = string_util_snprintf(&((*correlation_id)[index * 2]), 3, "%02x", random_bytes[index]);
    }

    if (MCL_OK != code)
    {
        MCL_FREE(*correlation_id);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static mcl_error_t _save_credentials(core_processor_t *core_processor)
{
    // Set default return code to MCL_OK assuming if no storage option is provided agent does not want to save data.
    mcl_error_t result = MCL_OK;

    MCL_DEBUG_ENTRY("core_processor_t *core_processor = <%p>", core_processor);

    // Store credentials via custom functions if both function pointers are not null.
    if ((MCL_NULL != core_processor->configuration->credentials_load_callback.rsa) && (MCL_NULL != core_processor->configuration->credentials_save_callback.rsa))
    {
        if (MCL_SECURITY_SHARED_SECRET == core_processor->configuration->security_profile)
        {
            result = core_processor->configuration->credentials_save_callback.shared_secret(core_processor->security_handler->client_id, core_processor->security_handler->client_secret, core_processor->security_handler->registration_access_token, core_processor->security_handler->registration_uri);
        }
        else
        {
            result = core_processor->configuration->credentials_save_callback.rsa(core_processor->security_handler->client_id, (char *)core_processor->security_handler->rsa.public_key, (char *)core_processor->security_handler->rsa.private_key, core_processor->security_handler->registration_access_token, core_processor->security_handler->registration_uri);
        }
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

static mcl_error_t _compose_access_token_request_payload(core_processor_t *core_processor, char **request_payload)
{
    mcl_error_t code;
    jwt_t *jwt = MCL_NULL;
    char *jwt_string = MCL_NULL;
    const char *head = "grant_type=client_credentials&client_assertion_type=urn:ietf:params:oauth:client-assertion-type:jwt-bearer&client_assertion=";

    MCL_DEBUG_ENTRY("core_processor_t *core_processor = <%p>", core_processor);

    // Create a new self issued jwt.
    code = jwt_initialize(core_processor->security_handler, core_processor->configuration->security_profile, core_processor->configuration->tenant, &jwt);

    // Compose jwt string.
    if (MCL_OK == code)
    {
        jwt_string = jwt_get_token(jwt);
        jwt_destroy(&jwt);
        if (MCL_NULL == jwt_string)
        {
            code = MCL_FAIL;
        }
    }

    // Form the final request payload.
    if (MCL_OK == code)
    {
        code = string_util_concatenate(head, jwt_string, request_payload);
    }

    MCL_FREE(jwt_string);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
