/*!
 * @file     data_lake_processor.h
 * @brief    Data lake processor module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DATA_LAKE_PROCESSOR_H_
#define DATA_LAKE_PROCESSOR_H_

#include "mcl_core/mcl_http_client.h"
#include "data_lake_object.h"

/**
 * Handle for data lake processor.
 */
typedef struct data_lake_processor_t
{
    char *access_token;                //!< Access token.
    const char *client_id;             //!< Client id.
    mcl_http_client_t *http_client;    //!< Http client handle.
    char *upload_url_generation_url;   //!< Endpoint to generate upload URL.
} data_lake_processor_t;

/**
 * This function gets the signed URL to upload data to.
 *
 * @param [in] processor Data lake processor.
 * @param [in] object Data lake object.
 * @param [in] subtenant_id Subtenant ID.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_DATA_LAKE_SIGNED_URL_GENERATION_FAIL if signed URL could not be retrieved for the object.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p processor or @p object is null.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if path of @p object is not set.</li>
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
mcl_error_t data_lake_processor_generate_upload_url(data_lake_processor_t *processor, data_lake_object_t* object, const char *subtenant_id);

/**
 * This function gets the signed URLs for a list of objects to upload data to.
 *
 * @param [in] processor Data lake processor.
 * @param [in] object_list List of data lake objects.
 * @param [in] subtenant_id Subtenant ID.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
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
mcl_error_t data_lake_processor_generate_upload_urls(data_lake_processor_t *processor, mcl_list_t *object_list, const char *subtenant_id);

/**
 * This function is used to upload objects to data lake.
 *
 * @param [in] processor Data lake processor handle to be used.
 * @param [in] object Data lake object to be uploaded.
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
mcl_error_t data_lake_processor_upload(data_lake_processor_t *processor, mcl_data_lake_object_t *object);

#endif //DATA_LAKE_PROCESSOR_H_
