/*!
 * @file     mcl_connectivity_configuration.h
 * @brief    Connectivity configuration module header file.
 *
 * This interface introduces configuration data structure for mcl_connectivity module
 * and functions to initialize and set parameters of this configuration.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_CONNECTIVITY_CONFIGURATION_H_
#define MCL_CONNECTIVITY_CONFIGURATION_H_

#include "mcl_connectivity/mcl_connectivity_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Http request payload size limit for MindSphere Connectivity.
 */
#define MCL_CONNECTIVITY_MAX_HTTP_PAYLOAD_SIZE (10485760)

/**
 * 16K is default value for maximum HTTP payload size for connectivity module,
 * although server can handle up to MCL_CONNECTIVITY_MAX_HTTP_PAYLOAD_SIZE.
 */
#define MCL_CONNECTIVITY_DEFAULT_MAX_HTTP_PAYLOAD_SIZE (16384)

/**
 * This is the minimum value which you can set MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_MAX_HTTP_PAYLOAD_SIZE to.
 */
#define MCL_CONNECTIVITY_MIN_HTTP_PAYLOAD_SIZE (400)

typedef struct mcl_connectivity_configuration_t mcl_connectivity_configuration_t;

/**
 * Parameters for connectivity configuration.
 */
typedef enum E_MCL_CONNECTIVITY_CONFIGURATION_PARAMETER
{   
    MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE = 0,              //!< Core parameter as mcl_core_t *.
    MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_MAX_HTTP_PAYLOAD_SIZE, //!< Max HTTP payload size parameter as mcl_size_t *.
    MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_END
} E_MCL_CONNECTIVITY_CONFIGURATION_PARAMETER;

/*
 * This function creates and initializes a data struct of #mcl_connectivity_configuration_t.
 *
 * @param [out] configuration Data structure holding the connectivity configuration parameters.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p configuration is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_connectivity_configuration_initialize(mcl_connectivity_configuration_t **configuration);

/**
 * This function is used to set a parameter of a connectivity configuration.
 *
 * @param [in] configuration Connectivity configuration to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_MCL_CONNECTIVITY_CONFIGURATION_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p configuration or @p value is NULL.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_connectivity_configuration_set_parameter(mcl_connectivity_configuration_t *configuration, E_MCL_CONNECTIVITY_CONFIGURATION_PARAMETER parameter, const void *value);

/**
 * This function destroys #mcl_connectivity_configuration_t data structure.
 *
 * @param [in] configuration Connectivity configuration handle which is going to be destroyed.
 */
extern MCL_CONNECTIVITY_EXPORT void mcl_connectivity_configuration_destroy(mcl_connectivity_configuration_t **configuration);

#ifdef  __cplusplus
}
#endif

#endif //MCL_CONNECTIVITY_CONFIGURATION_H_
