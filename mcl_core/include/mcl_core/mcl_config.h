/*!
 * @file     mcl_config.h
 * @brief    Configuration header of MindConnect Library.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef MCL_CONFIG_H_
#define MCL_CONFIG_H_

#define MCL_LOG_LEVEL_VERBOSE  1
#define MCL_LOG_LEVEL_DEBUG    2
#define MCL_LOG_LEVEL_INFO     3
#define MCL_LOG_LEVEL_WARN     4
#define MCL_LOG_LEVEL_ERROR    5
#define MCL_LOG_LEVEL_FATAL    6
#define MCL_LOG_LEVEL_NONE     0xFF

/**
 * Enable if file system exists.
 * It can be overridden by a global preprocessor definition as it is when building with CMake.
 */
#ifndef HAVE_FILE_SYSTEM_
#ifdef __MBED__
#define HAVE_FILE_SYSTEM_ 0
#else
#define HAVE_FILE_SYSTEM_ 1
#endif
#endif

 /**
  * Select log level for MindConnect Library.
  * MCL_LOG_LEVEL_VERBOSE
  * MCL_LOG_LEVEL_DEBUG
  * MCL_LOG_LEVEL_INFO
  * MCL_LOG_LEVEL_WARN
  * MCL_LOG_LEVEL_ERROR
  * MCL_LOG_LEVEL_FATAL
  * MCL_LOG_LEVEL_NONE
  * It can be overridden by a global preprocessor definition as it is when building with CMake.
  */
#ifndef MCL_LOG_LEVEL
#define MCL_LOG_LEVEL MCL_LOG_LEVEL_INFO
#endif

#endif //MCL_CONFIG_H_
