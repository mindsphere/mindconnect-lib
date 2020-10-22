/*!
 * @file     core_configuration.h
 * @brief    Core configuration module header file.
 *
 * This module contains type definition of core configuration.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "mcl_core/mcl_core_common.h"

/**
 * This struct is used for building core configuration structure.
 */
typedef struct mcl_core_configuration_t
{
    char *mindsphere_hostname;                                              //!< Mindsphere hostname.
    mcl_uint16_t mindsphere_port;                                           //!< Mindsphere port number.
    char *mindsphere_certificate;                                           //!< Mindsphere certificate. Optional. If NULL, MCL will use default CA certificate store (if provided at build-time) for peer verification.
    mcl_bool_t certificate_is_file;                                         //!< Flag to check if certificate is given as file or string.
    char *proxy_hostname;                                                   //!< Proxy hostname. Optional.
    mcl_uint16_t proxy_port;                                                //!< Proxy port number. Mandatory if proxy host name is set, ineffective otherwise.
    E_MCL_PROXY proxy_type;                                                 //!< Proxy type #E_MCL_PROXY. Mandatory if proxy host name is set, ineffective otherwise.
    char *proxy_username;                                                   //!< Proxy username. Optional if proxy host name is set, ineffective otherwise.
    char *proxy_password;                                                   //!< Proxy password. Mandatory if proxy host name and proxy username are set, ineffective otherwise.
    char *proxy_domain;                                                     //!< Proxy domain. Optional if proxy host name and proxy username are set, ineffective otherwise.
    E_MCL_SECURITY_PROFILE security_profile;                                //!< Security profile #E_MCL_SECURITY_PROFILE.
    mcl_uint32_t http_request_timeout;                                      //!< Timeout value (in seconds) for HTTP requests. Default timeout is 300 seconds.
    char *user_agent;                                                       //!< User agent.
    char *initial_access_token;                                             //!< Initial access token.
    char *register_endpoint;                                                //!< Uri for register endpoint
    char *token_endpoint;                                                   //!< Uri for token endpoint.
    char *tenant;                                                           //!< Tenant name.
    mcl_credentials_load_callback_t credentials_load_callback;              //!< Custom function for loading credentials. If both credentials_load_callback and credentials_save_callback are non-null, custom functions will be used.
    mcl_credentials_save_callback_t credentials_save_callback;              //!< Custom function for saving credentials. If both credentials_load_callback and credentials_save_callback are non-null, custom functions will be used.
    mcl_critical_section_enter_callback_t critical_section_enter_callback;  //!< Custom function for entering critical section (optional, default is NULL).
    mcl_critical_section_leave_callback_t critical_section_leave_callback;  //!< Custom function for leaving critical section (optional, default is NULL).
} core_configuration_t;

/**
 * This function checks whether all mandatory parameters of a core configuration are set or not.
 *
 * @param [in] configuration Core configuration to validate.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case core configuration has one or more missing mandatory parameter.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t core_configuration_validate(core_configuration_t *configuration);

/**
 * This function is used to log core configuration which is used to initialize core.
 *
 * @param [in] configuration Core configuration to log.
 */
MCL_LOCAL void core_configuration_log(core_configuration_t *configuration);

#endif //CONFIGURATION_H_
