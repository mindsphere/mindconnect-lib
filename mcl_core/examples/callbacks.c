/*!
 * @file     callbacks.c
 * @brief    Sample callback functions for MCL.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "callbacks.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "mcl_core/mcl_file_util.h"
#include "mcl_core/mcl_string_util.h"
#include "mcl_core/mcl_memory.h"

static const char *credentials_file_name = "registrationInformationFile.txt";

#define MAX_DIGIT_FOR_PARAMETER_LENGTH 4

static mcl_error_t read_parameter_from_file(char **parameter, void *file_descriptor);
static mcl_error_t save_parameter_to_file(const char *parameter, void *file_descriptor);

mcl_error_t custom_load_function_shared_secret(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri)
{
    // It is strongly recommended to use callback functions with encryption for loading/saving credentials.
    void *file_descriptor = NULL;
    mcl_error_t code = mcl_file_util_fopen(credentials_file_name, "r", &file_descriptor);

    if (MCL_OK == code)
    {
        code = read_parameter_from_file(client_id, file_descriptor);
    }

    if (MCL_OK == code)
    {
        code = read_parameter_from_file(client_secret, file_descriptor);
    }

    if (MCL_OK == code)
    {
        code = read_parameter_from_file(registration_access_token, file_descriptor);
    }

    if (MCL_OK == code)
    {
        code = read_parameter_from_file(registration_uri, file_descriptor);
    }

    if (MCL_OK != code)
    {
        MCL_FREE(*client_id);
        MCL_FREE(*client_secret);
        MCL_FREE(*registration_access_token);
        MCL_FREE(*registration_uri);

        code = MCL_CREDENTIALS_NOT_LOADED;
    }

    (void) mcl_file_util_fclose(file_descriptor);

    return code;
}

mcl_error_t custom_save_function_shared_secret(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri)
{
    void *file_descriptor = NULL;
    mcl_error_t code = mcl_file_util_fopen(credentials_file_name, "w", &file_descriptor);

    if (MCL_OK == code)
    {
        save_parameter_to_file(client_id, file_descriptor);
    }

    if (MCL_OK == code)
    {
        save_parameter_to_file(client_secret, file_descriptor);
    }

    if (MCL_OK == code)
    {
        save_parameter_to_file(registration_access_token, file_descriptor);
    }

    if (MCL_OK == code)
    {
        save_parameter_to_file(registration_uri, file_descriptor);
    }

    if (MCL_OK != code)
    {
        // In real application, user may try to save more aggressively instead of giving up.
        code = MCL_CREDENTIALS_NOT_SAVED;
    }

    (void) mcl_file_util_fclose(file_descriptor);

    return code;
}

mcl_error_t custom_load_function_rsa(char **client_id, char **public_key, char **private_key, char **registration_access_token, char **registration_uri)
{
    // It is strongly recommended to use callback functions with encryption for loading/saving credentials.
    void *file_descriptor = NULL;
    mcl_error_t code = mcl_file_util_fopen(credentials_file_name, "r", &file_descriptor);

    if (MCL_OK == code)
    {
        code = read_parameter_from_file(client_id, file_descriptor);
    }

    if (MCL_OK == code)
    {
        code = read_parameter_from_file(public_key, file_descriptor);
    }

    if (MCL_OK == code)
    {
        code = read_parameter_from_file(private_key, file_descriptor);
    }

    if (MCL_OK == code)
    {
        code = read_parameter_from_file(registration_access_token, file_descriptor);
    }

    if (MCL_OK == code)
    {
        code = read_parameter_from_file(registration_uri, file_descriptor);
    }

    if (MCL_OK != code)
    {
        MCL_FREE(*client_id);
        MCL_FREE(*public_key);
        MCL_FREE(*private_key);
        MCL_FREE(*registration_access_token);
        MCL_FREE(*registration_uri);

        code = MCL_CREDENTIALS_NOT_LOADED;
    }

    (void) mcl_file_util_fclose(file_descriptor);

    return code;
}

mcl_error_t custom_save_function_rsa(const char *client_id, const char *public_key, const char *private_key, const char *registration_access_token, const char *registration_uri)
{
    void *file_descriptor = NULL;
    mcl_error_t code = mcl_file_util_fopen(credentials_file_name, "w", &file_descriptor);

    if (MCL_OK == code)
    {
        save_parameter_to_file(client_id, file_descriptor);
    }

    if (MCL_OK == code)
    {
        save_parameter_to_file(public_key, file_descriptor);
    }

    if (MCL_OK == code)
    {
        save_parameter_to_file(private_key, file_descriptor);
    }

    if (MCL_OK == code)
    {
        save_parameter_to_file(registration_access_token, file_descriptor);
    }

    if (MCL_OK == code)
    {
        save_parameter_to_file(registration_uri, file_descriptor);
    }

    if (MCL_OK != code)
    {
        // In real application, user may try to save more aggressively instead of giving up.
        code = MCL_CREDENTIALS_NOT_SAVED;
    }

    (void) mcl_file_util_fclose(file_descriptor);

    return code;
}

static mcl_error_t read_parameter_from_file(char **parameter, void *file_descriptor)
{
    mcl_error_t code;
    char length_string[MAX_DIGIT_FOR_PARAMETER_LENGTH + 1] = {0};
    int length;
    
    *parameter = NULL;
    code = mcl_file_util_fgets(length_string, MAX_DIGIT_FOR_PARAMETER_LENGTH + 1, file_descriptor);

    if (MCL_OK == code)
    {
        length = atoi(length_string);
        *parameter = MCL_CALLOC(length + 1, 1);

        if (NULL != *parameter)
        {
            mcl_size_t actual_count = 0;
            mcl_file_util_fread(*parameter, 1, length, file_descriptor, &actual_count);

            if (actual_count != (unsigned) length)
            {
                code = MCL_FAIL;
                MCL_FREE(*parameter);
            }
        }
        else
        {
            code = MCL_OUT_OF_MEMORY;
        }
    }

    return code;
}

static mcl_error_t save_parameter_to_file(const char *parameter, void *file_descriptor)
{
    mcl_error_t code;
    char length_string[MAX_DIGIT_FOR_PARAMETER_LENGTH + 1] = {0};
    mcl_size_t parameter_length = mcl_string_util_strlen(parameter);

    code = mcl_string_util_snprintf(length_string, MAX_DIGIT_FOR_PARAMETER_LENGTH + 1, "%04u", (unsigned int) parameter_length);

    if (MCL_OK == code)
    {
        code = mcl_file_util_fwrite(length_string, 1, MAX_DIGIT_FOR_PARAMETER_LENGTH, file_descriptor);
    }

    if (MCL_OK == code)
    {
        code = mcl_file_util_fwrite(parameter, 1, parameter_length, file_descriptor);
    }

    return code;
}
