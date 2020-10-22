/*!
 * @file     mcl_core_configuration.h
 * @brief    Core configuration module interface header file.
 *
 * This module is used for configuring core component.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_CORE_CONFIGURATION_H_
#define MCL_CORE_CONFIGURATION_H_

#include "mcl_core/mcl_core_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * This is the handle for core configuration instance.
 */
typedef struct mcl_core_configuration_t mcl_core_configuration_t;

/**
 * Parameters for core configuration.
 */
typedef enum E_MCL_CORE_CONFIGURATION_PARAMETER
{
    MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST,                         //!< Mindsphere hostname parameter as char*.
    MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT,                         //!< Mindsphere port parameter as mcl_uint16_t.
    MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE,                       //!< Mindsphere certificate parameter as char*. If certificate is not set (as itself or as file), default CA certificate store will be used (if available).
    MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE_FILE,                  //!< Mindsphere certificate file parameter as char*. If certificate is not set (as itself or as file), default CA certificate store will be used (if available).
    MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE,                        //!< Proxy type parameter as #E_MCL_PROXY. Mandatory if proxy host name is set, ineffective otherwise.
    MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST,                        //!< Proxy host name parameter as char* (optional).
    MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT,                        //!< Proxy port number parameter as mcl_uint16_t. Mandatory if proxy host name is set, ineffective otherwise.
    MCL_CORE_CONFIGURATION_PARAMETER_PROXY_USER,                        //!< Proxy username parameter as char*. Optional if proxy host name is set, ineffective otherwise.
    MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PASS,                        //!< Proxy password parameter as char*. Mandatory if proxy host name and proxy username are set, ineffective otherwise.
    MCL_CORE_CONFIGURATION_PARAMETER_PROXY_DOMAIN,                      //!< Proxy domain parameter as char*. Optional if proxy host name and proxy username are set, ineffective otherwise.
    MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE,                  //!< Security profile parameter as #E_MCL_SECURITY_PROFILE.
	MCL_CORE_CONFIGURATION_PARAMETER_MAX_HTTP_PAYLOAD_SIZE,             //!< This is an obsolete parameter. Setting it will have no effect.
	MCL_CORE_CONFIGURATION_PARAMETER_HTTP_REQUEST_TIMEOUT,              //!< HTTP request timeout (in seconds) parameter as mcl_uint32_t. Default timeout is 300 seconds.
    MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT,                        //!< User agent parameter as char*.
    MCL_CORE_CONFIGURATION_PARAMETER_TENANT,                            //!< Tenant parameter as char*.
    MCL_CORE_CONFIGURATION_PARAMETER_IAT,                               //!< Initial access token parameter as char*.
    MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_LOAD_CALLBACK,         //!< Custom function for loading credentials parameter as #mcl_credentials_load_callback_t.
    MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_SAVE_CALLBACK,         //!< Custom function for saving credentials parameter as #mcl_credentials_save_callback_t.
    MCL_CORE_CONFIGURATION_PARAMETER_CRITICAL_SECTION_ENTER_CALLBACK,   //!< Custom function for entering critical section parameter as #mcl_critical_section_enter_callback_t (optional, default is NULL).
    MCL_CORE_CONFIGURATION_PARAMETER_CRITICAL_SECTION_LEAVE_CALLBACK,   //!< Custom function for leaving critical section parameter as #mcl_critical_section_leave_callback_t (optional, default is NULL).
    MCL_CORE_CONFIGURATION_PARAMETER_END
} E_MCL_CORE_CONFIGURATION_PARAMETER;

/**
 * This function initializes a new #mcl_core_configuration_t data structure with default values.
 *
 * @param [out] configuration Data structure holding the configuration parameters.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p configuration is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_core_configuration_initialize(mcl_core_configuration_t **configuration);

/**
 * This function is used to set a parameter of a core configuration.
 *
 * @param [in] configuration Core configuration to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_MCL_CORE_CONFIGURATION_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p configuration or @p value is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_core_configuration_set_parameter(mcl_core_configuration_t *configuration,
    E_MCL_CORE_CONFIGURATION_PARAMETER parameter, const void *value);

/**
 * This function destroys the #mcl_core_configuration_t data structure.
 *
 * @param [in] configuration Core configuration handle which is going to be destroyed.
 */
extern MCL_CORE_EXPORT void mcl_core_configuration_destroy(mcl_core_configuration_t **configuration);

#ifdef  __cplusplus
}
#endif

#endif //MCL_CORE_CONFIGURATION_H_
