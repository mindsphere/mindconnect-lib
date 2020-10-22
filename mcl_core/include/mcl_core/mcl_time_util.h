/*!
 * @file     mcl_time_util.h
 * @brief    Time utility module interface header file.
 *
 * This module provides time utility functions for converting time values
 * to ISO 8601 format and validating ISO 8601 formatted timestamps.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_TIME_UTIL_H_
#define MCL_TIME_UTIL_H_

#include "mcl_core/mcl_core_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

// yyyy-MM-ddTHH:mm:ss.SSSZ length including null character.
#define MCL_TIMESTAMP_LENGTH 25

/**
 * This function converts time value to ISO 8601 date and time format.
 *
 * @param [in] time_value Number of seconds passed since reference time.
 * @param [in,out] iso8601_formatted_time Time representation in ISO 8601 format, buffer size must be at least MCL_TIMESTAMP_LENGTH.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_time_util_convert_to_iso_8601_format(const time_t *time_value, char *iso8601_formatted_time);

/**
 * This function validates timestamp in terms of format, length and time value.
 *
 * @param [in] timestamp Timestamp value to be checked against yyyy-MM-ddTHH:mm:ss.SSSZ format. Ex:2016-04-26T08:06:25.317Z
 * @return
 * <ul>
 * <li>#MCL_TRUE in case timestamp is valid.</li>
 * <li>#MCL_FALSE in case timestamp is not valid in terms of format, length or time value.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_bool_t mcl_time_util_validate_timestamp(const char *timestamp);

#ifdef  __cplusplus
}
#endif

#endif //MCL_TIME_UTIL_H_
