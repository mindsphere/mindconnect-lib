/*!
 * @file     mcl_assert.h
 * @brief    Assert module header file.
 *
 * This module contains assert functions.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_ASSERT_H_
#define MCL_ASSERT_H_

#include "mcl_core/mcl_log_util.h"

#ifdef  __cplusplus
extern "C"
{
#endif

#define MCL_ERROR_RETURN_POINTER(return_value, ...) \
    MCL_ERROR(__VA_ARGS__); \
    MCL_DEBUG_LEAVE("retVal = <%p>", (return_value)); \
    return (return_value);

#define MCL_ERROR_RETURN(return_value, ...) \
    MCL_ERROR(__VA_ARGS__); \
    MCL_DEBUG_LEAVE("retVal = <%d>", (return_value)); \
    return (return_value);

#define MCL_ASSERT_OK(code) \
    if(MCL_OK != code) \
    { \
        MCL_ERROR("Expected: 0, actual: %d for " #code".", code); \
        goto MCL_FUNCTION_LEAVE_LABEL; \
    }

#define MCL_ASSERT_NOT_NULL(argument, return_variable) \
    if(MCL_NULL == argument) \
    { \
        MCL_ERROR_STRING("Null function argument (" #argument ")."); \
        return_variable = MCL_TRIGGERED_WITH_NULL; \
        goto MCL_FUNCTION_LEAVE_LABEL; \
    }

#define MCL_ASSERT(condition) \
    if(!(condition)) \
    { \
        MCL_ERROR_STRING("Assertion failed for condition = <" #condition ">."); \
        MCL_DEBUG_LEAVE("retVal = <%p>", MCL_NULL); \
        return MCL_NULL; \
    }

#define MCL_ASSERT_MESSAGE(condition, ...) \
    if(!(condition)) \
    { \
        MCL_ERROR_STRING("Assertion failed for condition = <" #condition ">."); \
        MCL_ERROR_RETURN_POINTER(MCL_NULL, __VA_ARGS__); \
    }

#define MCL_ASSERT_STATEMENT_MESSAGE(condition, statement, ...) \
    if(!(condition)) \
    { \
        MCL_ERROR_STRING("Assertion failed for condition = <" #condition ">."); \
        (statement); \
        MCL_ERROR_RETURN_POINTER(MCL_NULL, __VA_ARGS__); \
    }

#define MCL_ASSERT_CODE(condition, return_code) \
    if(!(condition)) \
    { \
        MCL_ERROR_STRING("Assertion failed for condition = <" #condition ">."); \
        MCL_DEBUG_LEAVE("retVal = <%d>", (return_code)); \
        return (return_code); \
    }

#define MCL_ASSERT_CODE_MESSAGE(condition, return_code, ...) \
    if(!(condition)) \
    { \
        MCL_ERROR_STRING("Assertion failed for condition = <" #condition ">."); \
        MCL_ERROR_RETURN(return_code, __VA_ARGS__); \
    }

#define MCL_ASSERT_STATEMENT_CODE(condition, statement, return_code) \
    if(!(condition)) \
    { \
        MCL_ERROR_STRING("Assertion failed for condition = <" #condition ">."); \
        (statement); \
        MCL_DEBUG_LEAVE("retVal = <%d>", (return_code)); \
        return (return_code); \
    }

#define MCL_ASSERT_STATEMENT_CODE_MESSAGE(condition, statement, return_code, ...) \
    if(!(condition)) \
    { \
        MCL_ERROR_STRING("Assertion failed for condition = <" #condition ">."); \
        (statement); \
        MCL_ERROR_RETURN(return_code, __VA_ARGS__); \
    }

#ifdef  __cplusplus
}
#endif

#endif //MCL_ASSERT_H_
