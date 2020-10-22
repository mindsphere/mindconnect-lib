/*!
 * @file     mcl_custom_data.h
 * @brief    Custom data module interface header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_CUSTOM_DATA_H_
#define MCL_CUSTOM_DATA_H_

#include "mcl_connectivity/mcl_connectivity_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Parameters for custom data.
 */
typedef enum E_MCL_CUSTOM_DATA_PARAMETER
{
    MCL_CUSTOM_DATA_PARAMETER_CONTENT_TYPE = 0, //!< Custom data content type parameter as char*.
    MCL_CUSTOM_DATA_PARAMETER_TYPE,             //!< Custom data type parameter as char*.
    MCL_CUSTOM_DATA_PARAMETER_BUFFER,           //!< Custom data buffer parameter as char*.
    MCL_CUSTOM_DATA_PARAMETER_BUFFER_SIZE,      //!< Custom data buffer size parameter as mcl_size_t.
    MCL_CUSTOM_DATA_PARAMETER_DETAILS,          //!< Custom data details parameter as mcl_json_t* (optional).
    MCL_CUSTOM_DATA_PARAMETER_END
} E_MCL_CUSTOM_DATA_PARAMETER;

/**
 * Handle for custom data.
 */
typedef struct mcl_custom_data_t mcl_custom_data_t;

/**
 * This function creates and initializes a data struct of #mcl_custom_data_t.
 *
 * @param [in] version Version of the custom data.
 * @param [out] custom_data Custom data handle which is going to be initialized.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p custom_data or @p version is null.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_custom_data_initialize(const char *version, mcl_custom_data_t **custom_data);

/**
 * This function is used to set a parameter of a custom data.
 *
 * @param [in] custom_data Custom data to set its parameter.
 * @param [in] parameter One of the parameters listed in #E_MCL_CUSTOM_DATA_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p custom_data or @p value is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
extern MCL_CONNECTIVITY_EXPORT mcl_error_t mcl_custom_data_set_parameter(mcl_custom_data_t *custom_data, E_MCL_CUSTOM_DATA_PARAMETER parameter, const void *value);

/**
 * This function destroys custom data data structure.
 *
 * @param [in] custom_data Custom data handle which is going to be destroyed.
 */
extern MCL_CONNECTIVITY_EXPORT void mcl_custom_data_destroy(mcl_custom_data_t **custom_data);

#ifdef  __cplusplus
}
#endif

#endif //MCL_CUSTOM_DATA_H_
