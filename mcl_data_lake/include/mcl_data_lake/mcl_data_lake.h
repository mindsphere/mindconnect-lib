/*!
 * @file     mcl_data_lake.h
 * @brief    Data lake interface header file.
 *
 * Agents can store structured or unstructured data in MindSphere Data Lake using MCL's data lake component.
 *
 * This module enables the agent to initialize a data structure for MCL data lake component using
 * #mcl_data_lake_initialize function. Following initialization, a signed URL to upload data to can
 * be generated using #mcl_data_lake_get_upload_url, #mcl_data_lake_generate_upload_urls,
 * #mcl_data_lake_generate_upload_url_for_subtenant or #mcl_data_lake_generate_upload_urls_for_subtenant functions. 
 * Then, data lake object can be uploaded to the signed URL using #mcl_data_lake_upload function.
 *
 * Data structure for the data lake component is destroyed using #mcl_data_lake_destroy funtion.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_DATA_LAKE_H_
#define MCL_DATA_LAKE_H_

#include "mcl_data_lake/mcl_data_lake_object.h"
#include "mcl_data_lake/mcl_data_lake_configuration.h"
#include "mcl_core/mcl_list.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Handle for data lake.
 */
typedef struct mcl_data_lake_t mcl_data_lake_t;

/**
 * This function creates and initializes a data structure of type #mcl_data_lake_t.
 *
 * @param [in] configuration Data structure holding data lake configuration parameters.
 * @param [out] data_lake Data lake handle which is going to be initialized.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p configuration or @p data_lake is null.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p configuration is invalid.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_NOT_ONBOARDED if the agent is not onboarded using core.</li>
 * <li>#MCL_NO_ACCESS_TOKEN_EXISTS if core does not have an access token.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_DATA_LAKE_EXPORT mcl_error_t mcl_data_lake_initialize(mcl_data_lake_configuration_t *configuration, mcl_data_lake_t **data_lake);

/**
 * This function gets the signed URL to upload data to.
 *
 * @param [in] data_lake Data lake handle.
 * @param [in] object Data lake object.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_DATA_LAKE_SIGNED_URL_GENERATION_FAIL if signed URL could not be retrieved for the object.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p data_lake or @p object is null.</li>
 * <li>#MCL_INVALID_PARAMETER if path of @p object is not set.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_PROXY if the proxy host name can not be resolved.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_HOST if the remote host name can not be resolved.</li>
 * <li>#MCL_SSL_HANDSHAKE_FAIL if a problem occurs during SSL handshake.</li>
 * <li>#MCL_COULD_NOT_CONNECT if there is a connection error with the server.</li>
 * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED if the server can not be verified.</li>
 * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
 * <li>#MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL if the server certificate can not be authenticated by the root certificate.</li>
 * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
 * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
 * <li>#MCL_BAD_REQUEST if response status code of server is 400.</li>
 * <li>#MCL_UNAUTHORIZED if response status code of server is 401.</li>
 * <li>#MCL_SERVER_FAIL if response status code of server is 500.</li>
 * </ul>
 */
extern MCL_DATA_LAKE_EXPORT mcl_error_t mcl_data_lake_generate_upload_url(mcl_data_lake_t *data_lake, mcl_data_lake_object_t *object);

/**
 * This function gets the signed URLs for a list of objects to upload data to.
 *
 * @param [in] data_lake Data lake handle.
 * @param [in] object_list List of data lake objects.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success (all data lake objects with non-NULL paths have their signed urls).</li>
 * <li>#MCL_DATA_LAKE_SIGNED_URL_GENERATION_FAIL if signed URL could not be retrieved for all data lake objects with non-NULL paths.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p processor or @p object_list is null.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if all paths are NULL.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_PROXY if the proxy host name can not be resolved.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_HOST if the remote host name can not be resolved.</li>
 * <li>#MCL_SSL_HANDSHAKE_FAIL if a problem occurs during SSL handshake.</li>
 * <li>#MCL_COULD_NOT_CONNECT if there is a connection error with the server.</li>
 * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED if the server can not be verified.</li>
 * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
 * <li>#MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL if the server certificate can not be authenticated by the root certificate.</li>
 * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
 * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
 * <li>#MCL_BAD_REQUEST if response status code of server is 400.</li>
 * <li>#MCL_UNAUTHORIZED if response status code of server is 401.</li>
 * <li>#MCL_SERVER_FAIL if response status code of server is 500.</li>
 * </ul>
 */
extern MCL_DATA_LAKE_EXPORT mcl_error_t mcl_data_lake_generate_upload_urls(mcl_data_lake_t *data_lake, mcl_list_t *object_list);

/**
 * This function gets the signed URL for subtenant to upload data to.
 *
 * @param [in] data_lake Data lake handle.
 * @param [in] object Data lake object.
 * @param [in] subtenant_id Subtenant ID.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_DATA_LAKE_SIGNED_URL_GENERATION_FAIL if signed URL could not be retrieved for the object.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p data_lake or @p object is null.</li>
 * <li>#MCL_INVALID_PARAMETER if path of @p object is not set.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_PROXY if the proxy host name can not be resolved.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_HOST if the remote host name can not be resolved.</li>
 * <li>#MCL_SSL_HANDSHAKE_FAIL if a problem occurs during SSL handshake.</li>
 * <li>#MCL_COULD_NOT_CONNECT if there is a connection error with the server.</li>
 * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED if the server can not be verified.</li>
 * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
 * <li>#MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL if the server certificate can not be authenticated by the root certificate.</li>
 * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
 * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
 * <li>#MCL_BAD_REQUEST if response status code of server is 400.</li>
 * <li>#MCL_UNAUTHORIZED if response status code of server is 401.</li>
 * <li>#MCL_SERVER_FAIL if response status code of server is 500.</li>
 * </ul>
 */
extern MCL_DATA_LAKE_EXPORT mcl_error_t mcl_data_lake_generate_upload_url_for_subtenant(mcl_data_lake_t *data_lake, mcl_data_lake_object_t *object, const char *subtenant_id);

/**
 * This function gets the signed URLs for subtenant for a list of objects to upload data to.
 *
 * @param [in] data_lake Data lake handle.
 * @param [in] object_list List of data lake objects.
 * @param [in] subtenant_id Subtenant ID.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success (all data lake objects with non-NULL paths have their signed urls).</li>
 * <li>#MCL_DATA_LAKE_SIGNED_URL_GENERATION_FAIL if signed URL could not be retrieved for all data lake objects with non-NULL paths.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p processor or @p object_list is null.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if all paths are NULL.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_PROXY if the proxy host name can not be resolved.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_HOST if the remote host name can not be resolved.</li>
 * <li>#MCL_SSL_HANDSHAKE_FAIL if a problem occurs during SSL handshake.</li>
 * <li>#MCL_COULD_NOT_CONNECT if there is a connection error with the server.</li>
 * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED if the server can not be verified.</li>
 * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
 * <li>#MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL if the server certificate can not be authenticated by the root certificate.</li>
 * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
 * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
 * <li>#MCL_BAD_REQUEST if response status code of server is 400.</li>
 * <li>#MCL_UNAUTHORIZED if response status code of server is 401.</li>
 * <li>#MCL_SERVER_FAIL if response status code of server is 500.</li>
 * </ul>
 */
extern MCL_DATA_LAKE_EXPORT mcl_error_t mcl_data_lake_generate_upload_urls_for_subtenant(mcl_data_lake_t *data_lake, mcl_list_t *object_list, const char *subtenant_id);

/**
 * This function uploads data object to the signed URL.
 *
 * @param [in] data_lake Data lake handle.
 * @param [in] object Data lake object.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY if there is not enough memory in the system to proceed.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p data_lake or @p object is null.</li>
 * <li>#MCL_INVALID_PARAMETER in case item has one or more missing mandatory parameter.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_PROXY in case the proxy host name can not be resolved.</li>
 * <li>#MCL_COULD_NOT_RESOLVE_HOST in case the remote host name can not be resolved.</li>
 * <li>#MCL_SSL_HANDSHAKE_FAIL in case a problem occurs during SSL handshake.</li>
 * <li>#MCL_COULD_NOT_CONNECT in case there is a connection error.</li>
 * <li>#MCL_SERVER_CERTIFICATE_NOT_VERIFIED in case the server can not be verified.</li>
 * <li>#MCL_IMPROPER_CERTIFICATE in case the certificate provided is in improper format and it can not be parsed.</li>
 * <li>#MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL in case the server certificate can not be authenticated by the root certificate.</li>
 * <li>#MCL_NETWORK_SEND_FAIL in case of an error in sending data to network.</li>
 * <li>#MCL_NETWORK_RECEIVE_FAIL in case of an error in receiving data from the network.</li>
 * <li>#MCL_BAD_REQUEST if the server returns HTTP 400 status code.</li>
 * <li>#MCL_UNAUTHORIZED if the server returns HTTP 401 status code.</li>
 * <li>#MCL_SERVER_FAIL if the server returns HTTP 500 status code.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_DATA_LAKE_EXPORT mcl_error_t mcl_data_lake_upload(mcl_data_lake_t *data_lake, mcl_data_lake_object_t *object);

/**
 * This function destroys data lake structure.
 *
 * @param [in] data_lake Data lake handle which is going to be destroyed.
 */
extern MCL_DATA_LAKE_EXPORT void mcl_data_lake_destroy(mcl_data_lake_t **data_lake);

#ifdef  __cplusplus
}
#endif

#endif //MCL_DATA_LAKE_H_
