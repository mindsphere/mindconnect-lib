/*!
 * @file     base64.h
 * @brief    Base64 module header file.
 *
 * This module defines methods for base64 and base64url encoding/decoding.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef BASE64_H_
#define BASE64_H_

#include "mcl_core/mcl_core_common.h"

/**
 * This function is used for decoding a base64 encoded, zero-terminated string in @p encoded_data
 * and return decoded data in @p decoded_data with size @p decoded_data_size.
 *
 * When decoded data length is 0, returns #MCL_NULL in @p decoded_data.
 *
 * @param encoded_data [in] Zero-terminated string which is base64 encoded and has to be decoded.
 * @param decoded_data [out] Newly allocated memory holding decoded data.
 * @param decoded_data_size [out] Size of decoded data.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_BAD_CONTENT_ENCODING if provided @p encoded_data has invalid length (0 or not multiples of 4) or is invalidly encoded.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t base64_decode(const char *encoded_data, mcl_uint8_t **decoded_data, mcl_size_t *decoded_data_size);

/**
 * This function is used for decoding a base64 URL encoded, zero-terminated string in @p encoded_data
 * and return decoded data in @p decoded_data with size @p decoded_data_size.
 *
 * When decoded data length is 0, returns #MCL_NULL in @p decoded_data.
 *
 * @param encoded_data [in] Zero-terminated string which is base64 URL encoded and has to be decoded.
 * @param decoded_data [out] Newly allocated memory holding decoded data.
 * @param decoded_data_size [out] Size of @p decoded_data.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_BAD_CONTENT_ENCODING if provided @p encoded_data has invalid length (0 or not multiples of 4) or is invalidly encoded.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t base64_url_decode(const char *encoded_data, mcl_uint8_t **decoded_data, mcl_size_t *decoded_data_size);

/**
 * This function is used for base64 encoding of data in @p data and return
 * encoded data in @p encoded_data.
 *
 * When encoded data length is 0, returns #MCL_NULL in @p encoded_data.
 *
 * @param data [in] Input data that has to be base64 encoded.
 * @param data_size [in] Size of given input @p data.
 * @param encoded_data [out] Newly allocated string holding encoded data.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t base64_encode(const mcl_uint8_t *data, mcl_size_t data_size, char **encoded_data);

/**
 * This function is used for base64 URL encoding of data in @p data and return
 * encoded data in @p encoded_data.
 *
 * When encoded data length is 0, returns #MCL_NULL in @p encoded_data.
 *
 * @param data [in] Input data that has to be base64 URL encoded.
 * @param data_size [in] Size of given input @p data.
 * @param encoded_data [out] Newly allocated string holding encoded data.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t base64_url_encode(const mcl_uint8_t *data, mcl_size_t data_size, char **encoded_data);

#endif //BASE64_H_
