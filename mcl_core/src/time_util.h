/*!
 * @file     time_util.h
 * @brief    Time utility module header file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef TIME_UTIL_H_
#define TIME_UTIL_H_

#include <time.h>
#include "mcl_core/mcl_core_common.h"

/**
 * This function returns the time elapsed in terms of seconds since Jan 1, 1970 UTC until now.
 *
 * @param [in] current_time Time value in seconds.
 */
MCL_LOCAL void time_util_get_time(mcl_time_t *current_time);

#endif //TIME_UTIL_H_
