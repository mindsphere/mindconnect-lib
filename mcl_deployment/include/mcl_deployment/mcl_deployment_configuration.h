/*!
 * @file     mcl_deployment_configuration.h
 * @brief    Header for component configuration.
 *
 * This interface introduces configuration data structure for #mcl_deployment module
 * and functions to initialize and set parameters of this configuration.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DEPLOYMENT_CONFIGURATION_H_
#define MCL_DEPLOYMENT_CONFIGURATION_H_

#include "mcl_deployment/mcl_deployment_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for deployment configuration.
 */
typedef struct mcl_deployment_configuration_t mcl_deployment_configuration_t;

/**
 * Parameters for deployment configuration.
 */
typedef enum E_MCL_DEPLOYMENT_CONFIGURATION_PARAMETER
{   
    MCL_DEPLOYMENT_CONFIGURATION_PARAMETER_CORE = 0, //!< Handle of the core component (mcl_core_t *).
    MCL_DEPLOYMENT_CONFIGURATION_PARAMETER_END
} E_MCL_DEPLOYMENT_CONFIGURATION_PARAMETER;

/**
 * This function creates and initializes a data struct of #mcl_deployment_configuration_t.
 *
 * @param [out] configuration Data structure holding the configuration parameters.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p configuration is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_configuration_initialize(mcl_deployment_configuration_t **configuration);

/**
 * This function is used to set a parameter of the configuration.
 *
 * @param [in] configuration Configuration to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_MCL_DEPLOYMENT_CONFIGURATION_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p configuration or @p value is NULL.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
extern MCL_DEPLOYMENT_EXPORT mcl_error_t mcl_deployment_configuration_set_parameter(mcl_deployment_configuration_t *configuration, E_MCL_DEPLOYMENT_CONFIGURATION_PARAMETER parameter, const void *value);

/**
 * This function destroys #mcl_deployment_configuration_t data structure.
 *
 * @param [in] configuration Configuration handle which is going to be destroyed.
 */
extern MCL_DEPLOYMENT_EXPORT void mcl_deployment_configuration_destroy(mcl_deployment_configuration_t **configuration);

#ifdef  __cplusplus
}
#endif

#endif //MCL_DEPLOYMENT_CONFIGURATION_H_
