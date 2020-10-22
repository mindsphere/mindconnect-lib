/*!
 * @file     mcl_tls_socket.h
 * @brief    TLS Socket header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_TLS_SOCKET_H_
#define MCL_TLS_SOCKET_H_

#include "mcl_core/mcl_assert.h"

#ifdef  __cplusplus
extern "C"
{
#endif

typedef struct mcl_tls_socket_t* mcl_tls_socket_handle;
typedef struct mcl_tls_ca_chain_t* mcl_tls_ca_chain_handle;

typedef enum
{
    MCL_TLS_SOCKET_PARAMETER_CERTIFICATE_CHAIN,
    MCL_TLS_SOCKET_PARAMETER_TIMEOUT,
    MCL_TLS_SOCKET_PARAMETER_END,
} E_MCL_TLS_SOCKET_PARAMETER;

/**
 * This function creates and initializes a #mcl_tls_ca_chain_handle.
 *
 * @param [out] tls_ca_chain_handle Certificate chain handle.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any mandatory input parameter is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
mcl_error_t mcl_tls_ca_chain_init(mcl_tls_ca_chain_handle *tls_ca_chain_handle);

/**
 * This function creates and initializes a #mcl_tls_ca_chain_handle.
 *
 * @param [in] tls_ca_chain_handle Certificate chain handle.
 * @param [in] certificate Certificate.
 * @param [in] is_file MCL_TRUE if certificate is given as file, MCL_FALSE otherwise.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any mandatory input parameter is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_IMPROPER_CERTIFICATE in case certificate is invalid.</li>
 * <li>#MCL_OPERATION_NOT_SUPPORTED in case certificate cannot be added to the chain.</li>
 * </ul>
 */
mcl_error_t mcl_tls_ca_chain_add_certificate(mcl_tls_ca_chain_handle tls_ca_chain_handle, const char *certificate, mcl_bool_t is_file);

/**
 * This function destroys the #mcl_tls_ca_chain_handle and frees any memory allocated.
 *
 * @param [in] tls_ca_chain_handle Preinitialized #mcl_tls_ca_chain_handle to destroy.
 */
void mcl_tls_ca_chain_destroy(mcl_tls_ca_chain_handle *tls_ca_chain_handle);

/**
 * This function creates and initializes a #mcl_tls_socket_handle.
 *
 * @param [out] tls_socket_handle TLS socket handle.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case any mandatory input parameter is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
mcl_error_t mcl_tls_socket_init(mcl_tls_socket_handle *tls_socket_handle);

/**
 * This function is used to set a parameter of a TLS socket handle.
 *
 * @param [in] tls_socket_handle TLS socket handle.
 * @param [in] parameter One of the parameters listed in #E_MCL_TLS_SOCKET_PARAMETER.
 * @param [in] value New value of the @p parameter.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p tls_socket_handle or @p value is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER if provided @p parameter is invalid.</li>
 * </ul>
 */
mcl_error_t mcl_tls_socket_set_parameter(mcl_tls_socket_handle tls_socket_handle, E_MCL_TLS_SOCKET_PARAMETER parameter, const void *value);

/**
 * This function is used to open a TLS socket.
 *
 * @param [in] tls_socket_handle TLS socket handle.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p tls_socket_handle or @p value is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
mcl_error_t mcl_tls_socket_open(mcl_tls_socket_handle tls_socket_handle);

/**
 * This function connects TLS socket to a remote host.
 *
 * @param [in] tls_socket_handle TLS socket handle.
 * @param [in] host Host to connect.
 * @param [in] port Port.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p tls_socket_handle or @p host is NULL.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_COULD_NOT_CONNECT in case of a connection error.</li>
 * <li>#MCL_SSL_HANDSHAKE_FAIL in case handshake with server fails.</li>
 * </ul>
 */
mcl_error_t mcl_tls_socket_connect(mcl_tls_socket_handle tls_socket_handle, const char *host, mcl_uint16_t port);

/**
 * This function is used to send data over TLS socket.
 *
 * @param [in] tls_socket_handle TLS socket handle.
 * @param [in] buffer Buffer to be sent.
 * @param [in,out] buffer_length Size of the buffer in bytes. It will be set to size of bytes successfully sent.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p tls_socket_handle or @p buffer_length is NULL.</li>
 * <li>#MCL_FAIL in case of an error.</li>
 * </ul>
 */
mcl_error_t mcl_tls_socket_send(mcl_tls_socket_handle tls_socket_handle, const mcl_uint8_t *buffer, mcl_size_t *buffer_length);

/**
 * This function is used to receive data over TLS socket.
 *
 * @param [in] tls_socket_handle TLS socket handle.
 * @param [out] buffer Buffer to write the received data.
 * @param [in,out] buffer_length Size of the buffer in bytes. It will be set to size of bytes successfully received.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p tls_socket_handle or @p buffer_length is NULL.</li>
 * <li>#MCL_FAIL in case of an error.</li>
 * </ul>
 */

mcl_error_t mcl_tls_socket_receive(mcl_tls_socket_handle tls_socket_handle, mcl_uint8_t *buffer, mcl_size_t *buffer_length);

/**
 * This function destroys the #mcl_tls_socket_handle and frees any memory allocated.
 *
 * @param [in] tls_ca_chain_handle Preinitialized #mcl_tls_socket_handle to destroy.
 */
void mcl_tls_socket_destroy(mcl_tls_socket_handle *tls_socket_handle);

#ifdef  __cplusplus
}
#endif

#endif //MCL_TLS_SOCKET_H_
