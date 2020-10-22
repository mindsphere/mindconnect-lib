/*!
 * @file     mcl_mapping.h
 * @brief    Data point mapping module interface header file.
 *
 * Agents are expected to map data points (defined in data source configuration uploaded to MindSphere) to assets 
 * before they start exchanging their timeseries data.
 * 
 * This module enables the agent to initialize a mapping structure using #mcl_mapping_initialize function.
 * Following initialization, the parameters of this mapping can be set using #mcl_mapping_set_parameter function.
 * 
 * Once the mapping structure is finalized, mapping can be created on MindSphere using #mcl_connectivity_create_mapping function.
 * 
 * <a href="https://developer.mindsphere.io/apis/connectivity-mindconnect/api-mindconnect-overview.html#data-point-mapping" target="_blank">Click here for further information about data point mapping.</a>
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_MAPPING_H_
#define MCL_MAPPING_H_

#include "mcl_connectivity/mcl_connectivity_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Handle for mapping.
 */
typedef struct mcl_mapping_t mcl_mapping_t;

/**
 * Parameters for mapping.
 */
typedef enum E_MCL_MAPPING_PARAMETER
{
    MCL_MAPPING_PARAMETER_DATA_POINT_ID,        //!< Data point ID (as char *).
    MCL_MAPPING_PARAMETER_ENTITY_ID,            //!< Entity ID (as char *), do not set if it is the agent itself.
    MCL_MAPPING_PARAMETER_PROPERTY_SET_NAME,    //!< Property set name (as char *), e.g. ElectricalProperties.
    MCL_MAPPING_PARAMETER_PROPERTY_NAME,        //!< Property name (as char *), e.g. Voltage.
    MCL_MAPPING_PARAMETER_KEEP_MAPPING,         //!< Keep mapping (as mcl_bool_t *), true by default.
    MCL_MAPPING_PARAMETER_END
} E_MCL_MAPPING_PARAMETER;

/**
 * This function creates and initializes a data struct of #mcl_mapping_t.
 *
 * @param [out] mapping Mapping handle which is going to be initialized.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p mapping is null.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_mapping_initialize(mcl_mapping_t **mapping);

/**
 * This function is used to set a parameter of a mapping.
 *
 * @param [in] mapping Mapping to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_MCL_MAPPING_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p mapping or @p value is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_mapping_set_parameter(mcl_mapping_t *mapping, E_MCL_MAPPING_PARAMETER parameter, const void *value);

/**
 * This function destroys mapping data structure.
 *
 * @param [in] mapping Mapping handle which is going to be destroyed.
 */
extern MCL_CONNECTIVITY_EXPORT void mcl_mapping_destroy(mcl_mapping_t **mapping);

#ifdef  __cplusplus
}
#endif

#endif //MCL_MAPPING_H_
