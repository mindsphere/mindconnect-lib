/*!
 * @file     log_util.c
 * @brief    Log utility module implementation file.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "log_util.h"
#include "mcl_core/mcl_assert.h"
#include <stdarg.h>
#include <stdio.h>

#if !MCL_LOG_DISABLED

static void _mcl_log_util_default_callback(void *user_context, int log_level, const char *file, int line, const char *tag, const char * const format, ...);
static int _mcl_log_level = MCL_LOG_LEVEL;

mcl_log_util_callback_t mcl_log_util_function = _mcl_log_util_default_callback;
void *mcl_log_util_user_context = NULL;

static const char *_mcl_level_strings[] =
{
    "VERBOSE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};
#endif

mcl_error_t mcl_log_util_set_output_level(const int log_level)
{
#if !MCL_LOG_DISABLED
    if (((MCL_LOG_LEVEL <= log_level) && (log_level <= MCL_LOG_LEVEL_FATAL)) || (MCL_LOG_LEVEL_NONE == log_level))
    {
        _mcl_log_level = log_level;
        return MCL_OK;
    }
#else
    if (MCL_LOG_LEVEL_NONE == log_level)
    {
        return MCL_OK;
    }
#endif

    return MCL_INVALID_LOG_LEVEL;
}

int mcl_log_util_get_output_level(void)
{
#if !MCL_LOG_DISABLED
    return _mcl_log_level;
#else
    return MCL_LOG_LEVEL_NONE;
#endif
}

mcl_error_t mcl_log_util_set_callback(mcl_log_util_callback_t callback, void *user_context)
{
#if !MCL_LOG_DISABLED
    mcl_error_t code = MCL_OK;

    MCL_DEBUG_ENTRY("mcl_log_util_callback_t callback = <%p>,  void *user_context = <%p>", callback, user_context);

    MCL_ASSERT_NOT_NULL(callback, code);

    mcl_log_util_function = callback;
    mcl_log_util_user_context = user_context;

MCL_FUNCTION_LEAVE_LABEL:
    MCL_DEBUG_LEAVE("retVal = <%d>", code);
    return code;
#else
    return MCL_FAIL;
#endif
}

#if !MCL_LOG_DISABLED
static void _mcl_log_util_default_callback(void *user_context, int log_level, const char *file, int line, const char *tag, const char * const format, ...)
{
    va_list args;
    va_start(args, format);

    // user_context will not be used.
    (void) user_context;

#if defined(WIN32) || defined(WIN64)
    printf_s("MCL | %s | %s | %d | %s | ", _mcl_level_strings[log_level - MCL_LOG_LEVEL_VERBOSE], file, line, tag);
    vprintf_s(format, args);
    putchar('\r');
#else
    printf("MCL | %s | %s | %d | %s | ", _mcl_level_strings[log_level - MCL_LOG_LEVEL_VERBOSE], file, line, tag);
    vprintf(format, args);
#endif

    putchar('\n');
    va_end(args);
}
#endif
