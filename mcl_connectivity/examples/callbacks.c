/*!
 * @file     callbacks.c
 * @brief    Sample callback functions for MCL.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "callbacks.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *credentials_file_name = "credentialsFile.txt";

mcl_error_t custom_load_function(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri)
{
    // It is strongly recommended to use callback functions with encryption for loading/saving credentials.
    FILE *fd = fopen(credentials_file_name, "r");

    if (NULL == fd)
    {
        *client_id = NULL;
        *client_secret = NULL;
        *registration_access_token = NULL;
        *registration_uri = NULL;
        printf("[INFO] : Absence of the file means we are not onboarded yet, initial access token will be used to onboard.\n");
        return MCL_CREDENTIALS_NOT_LOADED;
    }

    char string_length[5] = {0};
    int size_to_allocate;

    fgets(string_length, 5, fd);
    size_to_allocate = atoi(string_length) + 1;
    *client_id = calloc(size_to_allocate, 1);
    fgets(*client_id, size_to_allocate, fd);

    fgets(string_length, 5, fd);
    size_to_allocate = atoi(string_length) + 1;
    *client_secret = calloc(size_to_allocate, 1);
    fgets(*client_secret, size_to_allocate, fd);

    fgets(string_length, 5, fd);
    size_to_allocate = atoi(string_length) + 1;
    *registration_access_token = calloc(size_to_allocate, 1);
    fgets(*registration_access_token, size_to_allocate, fd);

    fgets(string_length, 5, fd);
    size_to_allocate = atoi(string_length) + 1;
    *registration_uri = calloc(size_to_allocate, 1);
    fgets(*registration_uri, size_to_allocate, fd);

    fclose(fd);
    return MCL_OK;
}

mcl_error_t custom_save_function(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri)
{
    FILE *fd = fopen(credentials_file_name, "w");

    if (NULL == fd)
    {
        printf("[ERROR] : File cannot be opened or truncated to save the credentials.\n");
        return MCL_CREDENTIALS_NOT_SAVED;
    }

    char string_length[5] = {0};

    snprintf(string_length, 5, "%04u", (unsigned int) strlen(client_id));
    fputs(string_length, fd);
    fputs(client_id, fd);

    snprintf(string_length, 5, "%04u", (unsigned int) strlen(client_secret));
    fputs(string_length, fd);
    fputs(client_secret, fd);

    snprintf(string_length, 5, "%04u", (unsigned int) strlen(registration_access_token));
    fputs(string_length, fd);
    fputs(registration_access_token, fd);

    snprintf(string_length, 5, "%04u", (unsigned int) strlen(registration_uri));
    fputs(string_length, fd);
    fputs(registration_uri, fd);

    fclose(fd);
    return MCL_OK;
}
