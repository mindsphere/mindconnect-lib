/*!
 * @file     multipart.h
 * @brief    Multipart module header file.
 *
 * This module manages the multipart message structure of HTTP request.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MULTIPART_H_
#define MULTIPART_H_

#include "mcl_connectivity/mcl_connectivity_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Callback function prototype for adding payload to multipart.
 *
 * @param [in] buffer Destination address for payload.
 * @param [in] size Size, in bytes, of each element to be read.
 * @param [in] count Number of elements, each one with a size of @p size bytes.
 * @param [in] user_context Source address for payload.
 * @return The total number of bytes successfully written is returned.
 */
typedef mcl_size_t (*multipart_add_payload_callback)(char *buffer, mcl_size_t size, mcl_size_t count, void *user_context);

/**
 * This function is used to add a connectivity item to the HTTP request as a multipart content (meta + payload).
 *
 * @param [in] buffer Buffer to be used.
 * @param [in,out] buffer_size Size of the buffer. Remaining size will be returned via this pointer.
 * @param [in] item Item to be added.
 * @param [in] boundary Multipart boundary.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t multipart_add_tuple(char *buffer, mcl_size_t *buffer_size, void *item, const char *boundary);

/**
 * This function is used to add a connectivity item to the HTTP request using callback as a multipart content (meta + payload).
 *
 * @param [in] buffer Buffer to be used.
 * @param [in,out] buffer_size Size of the buffer. Remaining size will be returned via this pointer.
 * @param [in] item Item to be added.
 * @param [in] boundary Multipart boundary.
 * @param [in] payload_content_type Content type of payload.
 * @param [in] callback Callback function to be used to copy the payload to @p http_request.
 * @param [in] user_context User context pointer to pass to the callback function.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t multipart_add_tuple_with_callback(char *buffer, mcl_size_t *buffer_size, void *item, const char *boundary, const char *payload_content_type,
    multipart_add_payload_callback callback, void *user_context);

/**
 * This function calculates the tuple size of an item.
 *
 * @param [in] item Item to calculate its tuple size.
 * @param [in] payload_content_type Content type of payload.
 * @return Tuple size of the item.
 */
MCL_LOCAL mcl_size_t multipart_get_tuple_size(void *item, const char *payload_content_type);

/**
 * This function is used to close the multipart section of HTTP request with closing boundary.
 *
 * @param [in] buffer Buffer to be used.
 * @param [in,out] buffer_size Size of the buffer. Remaining size will be returned via this pointer.
 * @param [in] boundary Closing boundary.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t multipart_close(char *buffer, mcl_size_t *buffer_size, const char *boundary);

/**
 * This function is used to get multipart overhead size.
 *
 * @return The size of multipart overhead.
 */
MCL_LOCAL mcl_size_t multipart_get_overhead_size(void);

/**
 * This function is used to generate random boundary to be used in multipart messages.
 *
 * @param [out] boundary Generated boundary.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p boundary is null.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t multipart_generate_boundary(char **boundary);

#ifdef  __cplusplus
}
#endif

#endif //MULTIPART_H_
