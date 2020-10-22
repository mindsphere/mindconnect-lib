/*!
 * @file     jwt.h
 * @brief    JWT module header file.
 *
 * This module is used to generate JWT.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef JWT_H_
#define JWT_H_

#include "security_handler.h"
#include "json_util.h"

/**
 * Handle for JWT.
 */
typedef struct jwt_t
{
    mcl_json_t *header;                      //!< Header of JWT.
    mcl_json_t *payload;                     //!< Payload of JWT.
    security_handler_t *security_handler;    //!< Security handler.
    E_MCL_SECURITY_PROFILE security_profile; //!< Security profile.
    mcl_time_t issued_at;                    //!< Time of issue.
} jwt_t;

/**
 * This function creates and initializes a data struct of #jwt_t.
 * @param [in] security_handler Already initialized security handler. All JWT operations will be done using this object.
 * @param [in] security_profile Onboarding security profile.
 * @param [in] tenant Tenant which will be set in JWT payload.
 * @param [out] jwt The newly initialized jwt handle.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case JSON object can not be created.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t jwt_initialize(security_handler_t *security_handler, E_MCL_SECURITY_PROFILE security_profile, char *tenant, jwt_t **jwt);

/**
 * This function is used to generate the JWT as json string.
 *
 * @param [in] jwt JWT handle.
 * @return Pointer to the JWT of type char or NULL in case of an error.
 */
MCL_LOCAL char *jwt_get_token(jwt_t *jwt);

/**
 * This function destroys #jwt_t data structure.
 *
 * @param [in] jwt JWT handle which is going to be destroyed.
 */
MCL_LOCAL void jwt_destroy(jwt_t **jwt);

#endif //JWT_H_
