/*!
 * @file     callbacks.h
 * @brief    Sample callback functions for MCL.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef CALLBACKS_H_
#define CALLBACKS_H_

#include "mcl_core/mcl_core.h"

#ifdef __cplusplus
extern "C"
{
#endif

mcl_error_t custom_load_function(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri);
mcl_error_t custom_save_function(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri);

#ifdef  __cplusplus
}
#endif

#endif //CALLBACKS_H_
