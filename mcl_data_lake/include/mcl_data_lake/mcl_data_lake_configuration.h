/*!
 * @file     mcl_data_lake_configuration.h
 * @brief    Data lake configuration module header file.
 *
 * This interface introduces configuration data structure for #mcl_data_lake module
 * and functions to initialize and set parameters of this configuration.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DATA_LAKE_CONFIGURATION_H_
#define MCL_DATA_LAKE_CONFIGURATION_H_

#include "mcl_data_lake/mcl_data_lake_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for data lake configuration.
 */
typedef struct mcl_data_lake_configuration_t mcl_data_lake_configuration_t;

/**
 * Parameters for data lake configuration.
 */
typedef enum E_MCL_DATA_LAKE_CONFIGURATION_PARAMETER
{   
    MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CORE = 0,             //!< Core parameter as mcl_core_t *.
    MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CERTIFICATE,          //!< Certificate parameter as char* for the server of the pre-signed url (e.g. AWS certificate if the pre-signed url belongs to AWS.). If certificate is not set (as itself or as file), default CA certificate store will be used (if available).
    MCL_DATA_LAKE_CONFIGURATION_PARAMETER_CERTIFICATE_FILE,     //!< Certificate file parameter as char* for the server of the pre-signed url (e.g. AWS certificate if the pre-signed url belongs to AWS.). If certificate is not set (as itself or as file), default CA certificate store will be used (if available).
    MCL_DATA_LAKE_CONFIGURATION_PARAMETER_END
} E_MCL_DATA_LAKE_CONFIGURATION_PARAMETER;

/**
 * This function creates and initializes a data struct of #mcl_data_lake_configuration_t.
 *
 * @param [out] configuration Data structure holding the data lake configuration parameters.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p configuration is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
extern MCL_DATA_LAKE_EXPORT mcl_error_t mcl_data_lake_configuration_initialize(mcl_data_lake_configuration_t **configuration);

/**
 * This function is used to set a parameter of a data lake configuration.
 *
 * @param [in] configuration Data lake configuration to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_MCL_DATA_LAKE_CONFIGURATION_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p configuration or @p value is NULL.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
extern MCL_DATA_LAKE_EXPORT mcl_error_t mcl_data_lake_configuration_set_parameter(mcl_data_lake_configuration_t *configuration, E_MCL_DATA_LAKE_CONFIGURATION_PARAMETER parameter, const void *value);

/**
 * This function destroys #mcl_data_lake_configuration_t data structure.
 *
 * @param [in] configuration Data lake configuration handle which is going to be destroyed.
 */
extern MCL_DATA_LAKE_EXPORT void mcl_data_lake_configuration_destroy(mcl_data_lake_configuration_t **configuration);

#ifdef  __cplusplus
}
#endif

#endif //MCL_DATA_LAKE_CONFIGURATION_H_
