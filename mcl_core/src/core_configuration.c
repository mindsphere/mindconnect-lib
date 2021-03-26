/*!
 * @file     core_configuration.c
 * @brief    Core configuration module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "mcl_core/mcl_core_configuration.h"
#include "core_configuration.h"
#include "definitions.h"
#include "mcl_core/mcl_string_util.h"
#include "mcl_core/mcl_assert.h"
#include "mcl_core/mcl_memory.h"
#include "string_util.h"

#define USER_AGENT_HEADER_FORMAT "MCL/" MCL_VERSION_STRING " (%s)"

mcl_error_t mcl_core_configuration_initialize(mcl_core_configuration_t **configuration)
{
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_core_configuration_t **configuration = <%p>", configuration);

    // Null check.
    MCL_ASSERT_NOT_NULL(configuration, code);

    // Allocate memory for configuration handle.
    MCL_ASSERT_CODE_MESSAGE(MCL_NULL != MCL_NEW(*configuration), MCL_OUT_OF_MEMORY, "Not enough memory to allocate new configuration.");

    (*configuration)->mindsphere_hostname = MCL_NULL;
    (*configuration)->mindsphere_port = 0;
    (*configuration)->mindsphere_certificate = MCL_NULL;
    (*configuration)->proxy_hostname = MCL_NULL;
    (*configuration)->proxy_port = 0;
    (*configuration)->proxy_type = MCL_PROXY_UNKNOWN;
    (*configuration)->proxy_username = MCL_NULL;
    (*configuration)->proxy_password = MCL_NULL;
    (*configuration)->proxy_domain = MCL_NULL;
    (*configuration)->security_profile = MCL_SECURITY_SHARED_SECRET;
    (*configuration)->http_request_timeout = DEFAULT_HTTP_REQUEST_TIMEOUT;
    (*configuration)->user_agent = MCL_NULL;
    (*configuration)->initial_access_token = MCL_NULL;
    (*configuration)->tenant = MCL_NULL;
    (*configuration)->credentials_load_callback.rsa = MCL_NULL;
    (*configuration)->credentials_save_callback.rsa = MCL_NULL;
    (*configuration)->critical_section_enter_callback = MCL_NULL;
    (*configuration)->critical_section_leave_callback = MCL_NULL;
    (*configuration)->register_endpoint = MCL_NULL;
    (*configuration)->token_endpoint = MCL_NULL;

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t mcl_core_configuration_set_parameter(mcl_core_configuration_t *configuration, E_MCL_CORE_CONFIGURATION_PARAMETER parameter, const void *value)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t user_agent_length = 0;

    MCL_DEBUG_ENTRY("mcl_core_configuration_t *configuration = <%p>, E_MCL_CORE_CONFIGURATION_PARAMETER parameter = <%d>, const void *value = <%p>",
        configuration, parameter, value);

    // Null check.
    MCL_ASSERT_NOT_NULL(configuration, code);
    MCL_ASSERT_NOT_NULL(value, code);

    // Set related parameter.
    switch (parameter)
    {
        case MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST:
            code = mcl_string_util_reset(value, &configuration->mindsphere_hostname);
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT:
            configuration->mindsphere_port = *((mcl_uint16_t *) value);
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE:
            code = mcl_string_util_reset(value, &configuration->mindsphere_certificate);
            configuration->certificate_is_file = MCL_FALSE;
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE_FILE:
#if HAVE_FILE_SYSTEM_
            code = mcl_string_util_reset(value, &configuration->mindsphere_certificate);
            configuration->certificate_is_file = MCL_TRUE;
#else
            code = MCL_NO_FILE_SUPPORT;
#endif
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE:

            // Validate proxy type.
            MCL_ASSERT_CODE_MESSAGE(MCL_PROXY_HTTP <= *(E_MCL_PROXY *) value && MCL_PROXY_END > *(E_MCL_PROXY *) value, MCL_INVALID_PARAMETER,
                "Invalid proxy type value.");
            configuration->proxy_type = *(E_MCL_PROXY *) value;
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST:
            code = mcl_string_util_reset(value, &configuration->proxy_hostname);
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT:
            configuration->proxy_port = *((mcl_uint16_t *) value);
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_PROXY_USER:
            code = mcl_string_util_reset(value, &configuration->proxy_username);
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PASS:
            code = mcl_string_util_reset(value, &configuration->proxy_password);
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_PROXY_DOMAIN:
            code = mcl_string_util_reset(value, &configuration->proxy_domain);
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE:

            // Validate security profile.
            MCL_ASSERT_CODE_MESSAGE(MCL_SECURITY_SHARED_SECRET <= *(E_MCL_SECURITY_PROFILE *) value &&
                MCL_SECURITY_PROFILE_END > *(E_MCL_SECURITY_PROFILE *) value, MCL_INVALID_PARAMETER, "Invalid security profile value.");
            configuration->security_profile = *(E_MCL_SECURITY_PROFILE *) value;
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_MAX_HTTP_PAYLOAD_SIZE:

            // This is an obsolete parameter. Kept for backward compatibility.
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_HTTP_REQUEST_TIMEOUT:
            configuration->http_request_timeout = *((mcl_uint32_t *) value);
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT:

            // Create User-Agent http header.
            // Example MCL/4.0.0 (Custom Agent v1.0)
            MCL_FREE(configuration->user_agent);
            user_agent_length += 4 + (sizeof(MCL_VERSION_STRING) - 1) + 3 + mcl_string_util_strlen((const char *) value);
            configuration->user_agent = MCL_MALLOC(user_agent_length + 1);

            if (MCL_NULL != configuration->user_agent)
            {
                code = mcl_string_util_snprintf(configuration->user_agent, user_agent_length + MCL_NULL_CHAR_SIZE, USER_AGENT_HEADER_FORMAT, value);

                if (MCL_OK != code)
                {
                    MCL_FREE(configuration->user_agent);
                }
            }
            else
            {
                code = MCL_OUT_OF_MEMORY;
            }
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_TENANT:
            code = mcl_string_util_reset(value, &configuration->tenant);
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_IAT:
            code = mcl_string_util_reset(value, &configuration->initial_access_token);
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_LOAD_CALLBACK:
            configuration->credentials_load_callback.rsa = (mcl_credentials_load_rsa_callback_t) value;
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_SAVE_CALLBACK:
            configuration->credentials_save_callback.rsa = (mcl_credentials_save_rsa_callback_t) value;
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_CRITICAL_SECTION_ENTER_CALLBACK:
            configuration->critical_section_enter_callback = (mcl_critical_section_enter_callback_t) value;
            break;

        case MCL_CORE_CONFIGURATION_PARAMETER_CRITICAL_SECTION_LEAVE_CALLBACK:
            configuration->critical_section_leave_callback = (mcl_critical_section_leave_callback_t) value;
            break;

        default:
            code = MCL_INVALID_PARAMETER;
            break;
    }

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

mcl_error_t core_configuration_validate(core_configuration_t *configuration)
{
    mcl_error_t code = MCL_OK;
    mcl_size_t host_name_length = 0;
    mcl_size_t proxy_host_length = 0;
    mcl_size_t proxy_user_name_length = 0;
    mcl_size_t proxy_password_length = 0;
    mcl_size_t proxy_domain_length = 0;
    mcl_size_t user_agent_length = 0;

    MCL_DEBUG_ENTRY("core_configuration_t *configuration = <%p>", configuration);

    // Validate host name.
    MCL_ASSERT_CODE(MCL_NULL != configuration->mindsphere_hostname, MCL_INVALID_PARAMETER);
    host_name_length = string_util_strnlen(configuration->mindsphere_hostname, MAXIMUM_HOST_NAME_LENGTH + 1);
    MCL_ASSERT_CODE((0 < host_name_length) && (MAXIMUM_HOST_NAME_LENGTH >= host_name_length), MCL_INVALID_PARAMETER);

    // Check if proxy is used but do not return error if not used.
    if (MCL_NULL != configuration->proxy_hostname)
    {
        // Validate proxy host name.
        proxy_host_length = string_util_strnlen(configuration->proxy_hostname, MAXIMUM_HOST_NAME_LENGTH + 1);
        MCL_ASSERT_CODE((0 < proxy_host_length) && (MAXIMUM_HOST_NAME_LENGTH >= proxy_host_length), MCL_INVALID_PARAMETER);

        // Check if user name is provided but do not return error if not.
        if (MCL_NULL != configuration->proxy_username)
        {
            // Validate proxy user name.
            proxy_user_name_length = string_util_strnlen(configuration->proxy_username, MAXIMUM_PROXY_USER_NAME_LENGTH + 1);
            MCL_ASSERT_CODE((0 < proxy_user_name_length) && (MAXIMUM_PROXY_USER_NAME_LENGTH >= proxy_user_name_length), MCL_INVALID_PARAMETER);

            // Validate proxy password.
            MCL_ASSERT_CODE(MCL_NULL != configuration->proxy_password, MCL_INVALID_PARAMETER);
            proxy_password_length = string_util_strnlen(configuration->proxy_password, MAXIMUM_PROXY_PASSWORD_LENGTH + 1);
            MCL_ASSERT_CODE((0 < proxy_password_length) && (MAXIMUM_PROXY_PASSWORD_LENGTH >= proxy_password_length), MCL_INVALID_PARAMETER);

            // Validate proxy domain.
            if (MCL_NULL != configuration->proxy_domain)
            {
                proxy_domain_length = string_util_strnlen(configuration->proxy_domain, MAXIMUM_PROXY_DOMAIN_LENGTH + 1);
                MCL_ASSERT_CODE((0 < proxy_domain_length) && (MAXIMUM_PROXY_DOMAIN_LENGTH >= proxy_domain_length), MCL_INVALID_PARAMETER);
            }
        }
    }

    // Validate User-Agent.
    MCL_ASSERT_CODE(MCL_NULL != configuration->user_agent, MCL_INVALID_PARAMETER);
    user_agent_length = string_util_strnlen(configuration->user_agent, MAXIMUM_USER_AGENT_LENGTH + 1);
    MCL_ASSERT_CODE((0 < user_agent_length) && (MAXIMUM_USER_AGENT_LENGTH >= user_agent_length), MCL_INVALID_PARAMETER);

    // Validate Tenant.
    MCL_ASSERT_CODE(MCL_NULL != configuration->tenant, MCL_INVALID_PARAMETER);

    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void mcl_core_configuration_destroy(mcl_core_configuration_t **configuration)
{
    MCL_DEBUG_ENTRY("mcl_core_configuration_t **configuration = <%p>", configuration);

    // Free configuration structure.
    if ((MCL_NULL != configuration) && (MCL_NULL != *configuration))
    {
        MCL_FREE((*configuration)->mindsphere_hostname);
        MCL_FREE((*configuration)->mindsphere_certificate);
        MCL_FREE((*configuration)->proxy_hostname);
        MCL_FREE((*configuration)->proxy_username);
        MCL_FREE((*configuration)->proxy_password);
        MCL_FREE((*configuration)->proxy_domain);
        MCL_FREE((*configuration)->user_agent);
        MCL_FREE((*configuration)->initial_access_token);
        MCL_FREE((*configuration)->tenant);
        MCL_FREE((*configuration)->register_endpoint);
        MCL_FREE((*configuration)->token_endpoint);
        MCL_FREE(*configuration);
    }

    MCL_DEBUG_LEAVE("retVal = <%d>", MCL_OK);
}

void core_configuration_log(core_configuration_t *configuration)
{
    MCL_DEBUG_ENTRY("core_configuration_t *configuration = <%p>", configuration);

    // This function will be called in success case, no need to check mandatory fields.
    MCL_INFO("Mindsphere Hostname: %s.", configuration->mindsphere_hostname);
    MCL_INFO("Mindsphere Port: %u.", configuration->mindsphere_port);

    if (MCL_NULL != configuration->mindsphere_certificate)
    {
        MCL_INFO("Mindsphere Certificate: Provided.");
    }
    else
    {
        MCL_INFO("Mindsphere Certificate: Not provided.");
    }

    if (MCL_NULL != configuration->proxy_hostname)
    {
        MCL_INFO("Proxy Hostname: %s.", configuration->proxy_hostname);
        MCL_INFO("Proxy Port: %u.", configuration->proxy_port);

        switch (configuration->proxy_type)
        {
            case MCL_PROXY_HTTP:
                MCL_INFO("Proxy Type: MCL_PROXY_HTTP.");
                break;

            case MCL_PROXY_HTTP_1_0:
                MCL_INFO("Proxy Type: MCL_PROXY_HTTP_1_0.");
                break;

            case MCL_PROXY_SOCKS4:
                MCL_INFO("Proxy Type: MCL_PROXY_SOCKS4.");
                break;

            case MCL_PROXY_SOCKS5:
                MCL_INFO("Proxy Type: MCL_PROXY_SOCKS5.");
                break;

            case MCL_PROXY_SOCKS4A:
                MCL_INFO("Proxy Type: MCL_PROXY_SOCKS4A.");
                break;

            case MCL_PROXY_SOCKS5_HOSTNAME:
                MCL_INFO("Proxy Type: MCL_PROXY_SOCKS5_HOSTNAME.");
                break;

            default:
                MCL_INFO("Proxy Type: MCL_PROXY_UNKNOWN.");
                break;
        }

        if (MCL_NULL != configuration->proxy_username)
        {
            MCL_INFO("Proxy Username: %s.", configuration->proxy_username);
            MCL_INFO("Proxy Password: %s.", configuration->proxy_password);

            if (MCL_NULL != configuration->proxy_domain)
            {
                MCL_INFO("Proxy Domain: %s.", configuration->proxy_domain);
            }
        }
    }
    else
    {
        MCL_INFO("Proxy: Not used.");
    }

    if (MCL_SECURITY_SHARED_SECRET == configuration->security_profile)
    {
        MCL_INFO("Security Profile: MCL_SECURITY_SHARED_SECRET.");
    }
    // Invalid parameter check is already done.
    else
    {
        MCL_INFO("Security Profile: MCL_SECURITY_RSA_3072.");
    }

    MCL_INFO("HTTP Request Timeout: %u seconds.", configuration->http_request_timeout);
    MCL_INFO("User Agent: %s.", configuration->user_agent);

    if (MCL_NULL != configuration->initial_access_token)
    {
        MCL_INFO("Initial Access Token: Provided.");
    }
    else
    {
        MCL_INFO("Initial Access Token: Not provided.");
    }

    MCL_INFO("Tenant: %s.", configuration->tenant);

    if ((MCL_NULL != configuration->credentials_load_callback.rsa) && (MCL_NULL != configuration->credentials_save_callback.rsa))
    {
        MCL_INFO("Security Information: Callback functions will be used.");
    }
    else
    {
        MCL_INFO("Security Information: Will not be saved.");
    }

    if ((MCL_NULL != configuration->critical_section_enter_callback) && (MCL_NULL != configuration->critical_section_leave_callback))
    {
        MCL_INFO("Critical Section: Callback functions are provided.");
    }
    else
    {
        MCL_INFO("Critical Section: Callback functions are not provided.");
    }

    MCL_DEBUG_LEAVE("retVal = void.");
}
