/*!
 * @file     mcl_log_util.h
 * @brief    Log utility module interface header file.
 *
 * This utility module defines macros and functions for logging purposes.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_LOG_UTIL_H_
#define MCL_LOG_UTIL_H_

#include "mcl_core/mcl_core_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * Log utility callback function definition.
 *
 * @param [in] user_context User's own context.
 * @param [in] log_level Log level.
 * @param [in] file File where log function is called.
 * @param [in] line Line number.
 * @param [in] tag Message tag.
 * @param [in] format Message format.
 */
typedef void (*mcl_log_util_callback_t)(void *user_context, int log_level, const char *file, int line, const char *tag, const char * const format, ...);

/**
 * This function is used to set callback function for log utility.
 *
 * @param [in] callback Callback function.
 * @param [in] user_context User context argument.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_TRIGGERED_WITH_NULL in case @p callback is null.</li>
 * <li>#MCL_FAIL in case logging is disabled.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_log_util_set_callback(mcl_log_util_callback_t callback, void *user_context);

/**
 * This function sets the output level. The output level is used during runtime check.
 * No logs are written if given log level is below this output level.
 *
 * @param [in] log_level The output level to set with.
 * Following log levels can be set.
 * <ul>
 * <li>MCL_LOG_LEVEL_VERBOSE.</li>    //!< All other events.
 * <li>MCL_LOG_LEVEL_DEBUG.</li>      //!< Minimal set of events that could help to reconstruct the execution path.
 * <li>MCL_LOG_LEVEL_INFO.</li>       //!< Significant life cycle event or major state transition happened.
 * <li>MCL_LOG_LEVEL_WARN.</li>       //!< Something that *usually* should not occur happened and significantly changes application behavior for some period of time.
 * <li>MCL_LOG_LEVEL_ERROR.</li>      //!< Something possible, but highly unexpected happened. The process is able to recover and continue execution.
 * <li>MCL_LOG_LEVEL_FATAL.</li>      //!< Something impossible and absolutely unexpected happened. Process can't continue and must be terminated.
 * <li>MCL_LOG_LEVEL_NONE.</li>       //!< None.
 * </ul>
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_LOG_LEVEL if log level is invalid. Any level which is lower than #MCL_LOG_LEVEL is considered as invalid.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT mcl_error_t mcl_log_util_set_output_level(const int log_level);

/**
 * This function is used to get log output level.
 *
 * @return
 * <ul>
 * <li>Output level of log utility.</li>
 * </ul>
 */
extern MCL_CORE_EXPORT int mcl_log_util_get_output_level(void);

#define MCL_LOG_ENABLED_COMPILE_TIME(level)     ((level) >= MCL_LOG_LEVEL)
#define MCL_LOG_ENABLED_RUN_TIME(level)         ((level) >= mcl_log_util_get_output_level())
#define MCL_LOG_DISABLED                        (!MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_FATAL))

#define MCL_LOG_TAG_DEFAULT    "[..]"
#define MCL_LOG_TAG_ENTRY      "[->]"
#define MCL_LOG_TAG_LEAVE      "[<-]"

#if(MCL_LOG_DISABLED)
#define MCL_LOG_WRITE(...)
#else
extern MCL_CORE_EXPORT mcl_log_util_callback_t mcl_log_util_function;
extern MCL_CORE_EXPORT void *mcl_log_util_user_context;

#define MCL_LOG_WRITE(level, tag, ...) \
    do { \
        if (MCL_LOG_ENABLED_RUN_TIME(level) && NULL != mcl_log_util_function) \
            mcl_log_util_function(mcl_log_util_user_context, level, __FILE__, __LINE__, tag, __VA_ARGS__); \
    } while(0)
#endif

#if MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_VERBOSE)
#define MCL_VERBOSE(...)        MCL_LOG_WRITE(MCL_LOG_LEVEL_VERBOSE, MCL_LOG_TAG_DEFAULT, __VA_ARGS__)
#define MCL_VERBOSE_ENTRY(...)  MCL_LOG_WRITE(MCL_LOG_LEVEL_VERBOSE, MCL_LOG_TAG_ENTRY, __VA_ARGS__)
#define MCL_VERBOSE_LEAVE(...)  MCL_LOG_WRITE(MCL_LOG_LEVEL_VERBOSE, MCL_LOG_TAG_LEAVE, __VA_ARGS__)
#define MCL_VERBOSE_STRING(string)  MCL_VERBOSE("%s", string)
#else
#define MCL_VERBOSE(...)
#define MCL_VERBOSE_ENTRY(...)
#define MCL_VERBOSE_LEAVE(...)
#define MCL_VERBOSE_STRING(string)
#endif

#if MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_DEBUG)
#define MCL_DEBUG(...)              MCL_LOG_WRITE(MCL_LOG_LEVEL_DEBUG, MCL_LOG_TAG_DEFAULT, __VA_ARGS__)
#define MCL_DEBUG_ENTRY(...)        MCL_LOG_WRITE(MCL_LOG_LEVEL_DEBUG, MCL_LOG_TAG_ENTRY, __VA_ARGS__)
#define MCL_DEBUG_LEAVE(...)        MCL_LOG_WRITE(MCL_LOG_LEVEL_DEBUG, MCL_LOG_TAG_LEAVE, __VA_ARGS__)
#define MCL_DEBUG_STRING(string)    MCL_DEBUG("%s", string)
#else
#define MCL_DEBUG(...)
#define MCL_DEBUG_ENTRY(...)
#define MCL_DEBUG_LEAVE(...)
#define MCL_DEBUG_STRING(string)
#endif

#if MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_INFO)
#define MCL_INFO(...)               MCL_LOG_WRITE(MCL_LOG_LEVEL_INFO, MCL_LOG_TAG_DEFAULT, __VA_ARGS__)
#define MCL_INFO_STRING(string)     MCL_INFO("%s", string)
#else
#define MCL_INFO(...)
#define MCL_INFO_STRING(string)
#endif

#if MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_WARN)
#define MCL_WARN(...)               MCL_LOG_WRITE(MCL_LOG_LEVEL_WARN, MCL_LOG_TAG_DEFAULT, __VA_ARGS__)
#define MCL_WARN_STRING(string)     MCL_WARN("%s", string)
#else
#define MCL_WARN(...)
#define MCL_WARN_STRING(string)
#endif

#if MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_ERROR)
#define MCL_ERROR(...)              MCL_LOG_WRITE(MCL_LOG_LEVEL_ERROR, MCL_LOG_TAG_DEFAULT, __VA_ARGS__)
#define MCL_ERROR_STRING(string)    MCL_ERROR("%s", string)
#else
#define MCL_ERROR(...)
#define MCL_ERROR_STRING(string)
#endif

#if MCL_LOG_ENABLED_COMPILE_TIME(MCL_LOG_LEVEL_FATAL)
#define MCL_FATAL(...)              MCL_LOG_WRITE(MCL_LOG_LEVEL_FATAL, MCL_LOG_TAG_DEFAULT, __VA_ARGS__)
#define MCL_FATAL_STRING(string)    MCL_FATAL("%s", string)
#else
#define MCL_FATAL(...)
#define MCL_FATAL_STRING(string)
#endif

#ifdef  __cplusplus
}
#endif

#endif //MCL_LOG_UTIL_H_
