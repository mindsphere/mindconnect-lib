/*!
 * @file     timeseries_upload.c
 * @brief    Example custom agent code which uploads timeseries.
 *
 * It is assumed that data source configuration is already uploaded which complies with the timeseries you want to exchange.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "mcl_connectivity/mcl_connectivity.h"
#include "callbacks.h"

#if defined(WIN32) || defined(WIN64)
#include "Windows.h"
#else
#include <unistd.h>
#endif

const char *mindsphere_certificate = "-----BEGIN CERTIFICATE-----\n"\
"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\n"\
"-----END CERTIFICATE-----";

#define VALUE_LIST_PER_EXCHANGE 4
#define VALUE_BUFFER_LENGTH 8

// Number of data points.
#define DATA_POINT_COUNT 7

// Quality code for "Good".
#define QUALITY_CODE_GOOD "00000000"

// Definition of a function pointer type to create a function list by using it. This function list will be used to call and add new functions easily.
typedef mcl_bool_t (*read_data_function_template)(char **buffer);

// Functions to read sensor values from Motor A.
mcl_bool_t read_voltage_motor_a(char **buffer);
mcl_bool_t read_current_motor_a(char **buffer);

// Functions to read sensor values from Motor B.
mcl_bool_t read_voltage_motor_b(char **buffer);
mcl_bool_t read_current_motor_b(char **buffer);

// Functions to read sensor values from Environmental Sensor.
mcl_bool_t read_temperature(char **buffer);
mcl_bool_t read_humidity(char **buffer);
mcl_bool_t read_pressure(char **buffer);

// List consisting of the functions declared above to call them in a loop.
read_data_function_template read_data_function_list[DATA_POINT_COUNT] = { read_voltage_motor_a, read_current_motor_a, read_voltage_motor_b, read_current_motor_b, read_temperature, read_humidity, read_pressure };

// Data point id list.
const char *data_point_id_list[DATA_POINT_COUNT] =
{
    "59c27254-6987-407e-b292-acbffcda20dc",
    "da8a3fac-cd38-4b27-9412-8530f47f7cbe",
    "0736c966-8968-44c6-b5a5-5306ee8bc79e",
    "bfde1367-eec0-4fe4-a2c8-73046fd2bee1",
    "aaf8b3de-3f53-47ae-b021-ea38c2ad1577",
    "c5205d57-e74a-4cb3-b7fd-12a0cf187768",
    "b48e88ef-822a-4e48-8db4-628b803eb560"
};

// Timeseries configuration id comes from data source configuration upload.
const char *configuration_id = "229c923a-f91e-489e-9a3c-f1b10ac2d835";

// Static functions for initialization and parameter setting process of timeseries_value and timeseries_value_list handles.
static mcl_error_t add_value_to_value_list(const char *data_point_id, const char *value, const char *quality_code, mcl_timeseries_value_list_t *timeseries_value_list);
static mcl_error_t add_value_list_to_timeseries(mcl_timeseries_t *timeseries);

// This function tries to exchange, renews access token or rotates key if necessary.
static mcl_error_t safe_exchange(mcl_connectivity_t *connectivity, mcl_core_t *core, void *item);

// Sleep function.
static void sleep_in_seconds(mcl_uint32_t seconds);

int main(void)
{
    printf("***** CUSTOM AGENT using MCL *****\n");

    mcl_error_t code;
    mcl_core_t *core = NULL;
    mcl_connectivity_t *connectivity = NULL;

    // Core configuration and related parameters.
    mcl_core_configuration_t *core_configuration = NULL;
    mcl_uint16_t mindsphere_port = 443;
    mcl_uint16_t proxy_port = 3128;
    E_MCL_PROXY mcl_proxy = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_SHARED_SECRET;

    // Connectivity configuration and related parameter.
    mcl_connectivity_configuration_t *connectivity_configuration = NULL;
    mcl_size_t maximum_payload_size = 65536;

    // Set log level. Note that default log level is the level set during build.
    mcl_log_util_set_output_level(MCL_LOG_LEVEL_INFO);

    // Initialize core configuration.
    code = mcl_core_configuration_initialize(&core_configuration);
    printf("Core configuration initialization : %s.\n", MCL_CORE_CODE_TO_STRING(code));

    // Set core configuration parameters and core initialization.
    if (MCL_OK == code)
    {
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, "https://southgate.eu1.mindsphere.io");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mindsphere_certificate);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, "custom agent v1.0");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, "192.168.0.1");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &mcl_proxy);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, "mclibiot");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_LOAD_CALLBACK, custom_load_function_shared_secret);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_SAVE_CALLBACK, custom_save_function_shared_secret);

        // Initialize mcl core with the core configuration.
        code = mcl_core_initialize(core_configuration, &core);
        printf("MCL core initialization : %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    // REMINDER: It is also assumed that data source configuration is uploaded which complies with the timeseries you want to exchange.

    // If mcl core is successfully initialized, try to onboard.
    if (MCL_OK == code)
    {
        code = mcl_core_onboard(core);
        printf("Onboarding : %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    // If onboarding is successful or the agent is already onboarded, get access token.
    if ((MCL_OK == code) || (MCL_ALREADY_ONBOARDED == code))
    {
        code = mcl_core_get_access_token(core);
        printf("Getting access token : %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    // Initialize connectivity configuration.
    if (MCL_OK == code)
    {
        code = mcl_connectivity_configuration_initialize(&connectivity_configuration);
    }

    // Set core for connectivity configuration.
    if (MCL_OK == code)
    {
        code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_CORE, core);
    }

    // Set max HTTP payload size for connectivity configuration.
    if (MCL_OK == code)
    {
        code = mcl_connectivity_configuration_set_parameter(connectivity_configuration, MCL_CONNECTIVITY_CONFIGURATION_PARAMETER_MAX_HTTP_PAYLOAD_SIZE, &maximum_payload_size);
    }

    // Initialize mcl connectivity.
    if (MCL_OK == code)
    {
        code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    }

    // Exchange timeseries for each loop.
    while (MCL_OK == code)
    {
        mcl_timeseries_t *timeseries = NULL;

        // Create timeseries.
        code = mcl_timeseries_initialize(MCL_TIMESERIES_VERSION_1_0, &timeseries);
        printf("Timeseries initialization : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));

        if (MCL_OK == code)
        {
            // Set configuration id.
            code = mcl_timeseries_set_parameter(timeseries, MCL_TIMESERIES_PARAMETER_CONFIGURATION_ID, configuration_id);
            printf("Setting configuration id : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
        }

        // Timeseries will be prepared with four timeseries value list before exchange.
        if (MCL_OK == code)
        {
            for (mcl_size_t i = 0; i < VALUE_LIST_PER_EXCHANGE; i++)
            {
                // Set parameters for timeseries value list.
                code = add_value_list_to_timeseries(timeseries);

                // Sleep to get timeseries_values in 1 second period.
                sleep_in_seconds(1);
            }
        }
        if (MCL_OK == code)
        {
            // Exchange timeseries.
            code = safe_exchange(connectivity, core, timeseries);
            printf("Timeseries upload: %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
        }

        mcl_timeseries_destroy(&timeseries);
    }

    // Clean up.
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&core_configuration);

    if (MCL_OK != code)
    {
        return -1;
    }

    return 0;
}

static mcl_error_t add_value_to_value_list(const char *data_point_id, const char *quality_code, const char *value, mcl_timeseries_value_list_t *timeseries_value_list)
{
    mcl_timeseries_value_t *timeseries_value = NULL;

    // Initialize timeseries value.
    mcl_error_t code = mcl_timeseries_value_initialize(&timeseries_value);
    printf("Timeseries value creation : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));

    if (MCL_OK == code)
    {
        // Set parameter data point id.
        code = mcl_timeseries_value_set_parameter(timeseries_value, MCL_TIMESERIES_VALUE_PARAMETER_DATA_POINT_ID, data_point_id);
        printf("|-->Adding data point id to timeseries value : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
    }

    if (MCL_OK == code)
    {
        // Set parameter quality code.
        code = mcl_timeseries_value_set_parameter(timeseries_value, MCL_TIMESERIES_VALUE_PARAMETER_QUALITY_CODE, quality_code);
        printf("|-->Adding quality code to timeseries value : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
    }

    if (MCL_OK == code)
    {
        // Set parameter value.
        code = mcl_timeseries_value_set_parameter(timeseries_value, MCL_TIMESERIES_VALUE_PARAMETER_VALUE, value);
        printf("|-->Adding value to timeseries value : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
    }

    if (MCL_OK == code)
    {
        // Add timeseries value to timeseries value list.
        code = mcl_timeseries_value_list_add_value(timeseries_value_list, timeseries_value);
        printf("|-->Adding timeseries value to timeseries value list : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
    }

    if (MCL_OK != code)
    {
        mcl_timeseries_value_destroy(&timeseries_value);
    }

    return code;
}

static mcl_error_t add_value_list_to_timeseries(mcl_timeseries_t *timeseries)
{
    char timestamp[MCL_TIMESTAMP_LENGTH];
    mcl_timeseries_value_list_t *timeseries_value_list = NULL;

    // Get local time to create timestamp.
    time_t timer;
    time(&timer);

    mcl_error_t code = mcl_time_util_convert_to_iso_8601_format(&timer, timestamp);
    printf("Converting time_t to iso 8601 format : %s.\n", MCL_CORE_CODE_TO_STRING(code));

    if (MCL_OK == code)
    {
        // Initialize timeseries value list.
        code = mcl_timeseries_value_list_initialize(&timeseries_value_list);
        printf("Timeseries value list creation : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
    }

    if (MCL_OK == code)
    {
        // Set timestamp for timeseries value list.
        code = mcl_timeseries_value_list_set_parameter(timeseries_value_list, MCL_TIMESERIES_VALUE_LIST_PARAMETER_TIMESTAMP_ISO8601, timestamp);
        printf("|-->Setting timestamp of timeseries value list : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
    }

    // Data point parameters will be added to timeseries value list.
    if (MCL_OK == code)
    {
        for (mcl_size_t data_point = 0; data_point < DATA_POINT_COUNT; data_point++)
        {
            char *value = NULL;

            // Read values from related sensors.
            read_data_function_list[data_point](&value);

            // Set parameters for timeseries value.
            code = add_value_to_value_list(data_point_id_list[data_point], QUALITY_CODE_GOOD, value, timeseries_value_list);

            free(value);
        }
    }
    if (MCL_OK == code)
    {
        code = mcl_timeseries_add_value_list(timeseries, timeseries_value_list);
        printf("|-->Adding timeseries value list to timeseries : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
    }

    if (MCL_OK != code)
    {
        mcl_timeseries_value_list_destroy(&timeseries_value_list);
    }

    return code;
}

static mcl_error_t safe_exchange(mcl_connectivity_t *connectivity, mcl_core_t *core, void *item)
{
    // Exchange item.
    mcl_error_t code = mcl_connectivity_exchange(connectivity, item);
    printf("Uploading item : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));

    if (MCL_UNAUTHORIZED == code || MCL_NO_ACCESS_TOKEN_EXISTS == code)
    {
        // Get access token.
        code = mcl_core_get_access_token(core);
        printf("Getting new access token : %s.\n", MCL_CORE_CODE_TO_STRING(code));

        if (MCL_BAD_REQUEST == code)
        {
            // Rotate key.
            code = mcl_core_rotate_key(core);
            printf("Calling rotate key : %s.\n", MCL_CORE_CODE_TO_STRING(code));

            // If key rotation is successful, then try to get access token.
            if (MCL_OK == code)
            {
                // Get access token.
                code = mcl_core_get_access_token(core);
                printf("Getting new access token : %s.\n", MCL_CORE_CODE_TO_STRING(code));
            }
        }

        if (MCL_OK == code)
        {
            // Exchange item.
            code = mcl_connectivity_exchange(connectivity, item);
            printf("Retrying exchange : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
        }
    }

    return code;
}

mcl_bool_t read_temperature(char **buffer)
{
    (*buffer) = (char*)malloc(VALUE_BUFFER_LENGTH * sizeof(*buffer));
    float temperature = (float) (25.0 + ((rand() % 32) / 32.0));
    snprintf(*buffer, VALUE_BUFFER_LENGTH, "%2.4f", temperature);

    return MCL_TRUE;
}

mcl_bool_t read_humidity(char **buffer)
{
    (*buffer) = (char*)malloc(VALUE_BUFFER_LENGTH * sizeof(*buffer));
    float humidity = (float) (70.0 + ((rand() % 256) / 32.0));
    snprintf(*buffer, VALUE_BUFFER_LENGTH, "%2.4f", humidity);

    return MCL_TRUE;
}

mcl_bool_t read_pressure(char **buffer)
{
    (*buffer) = (char*)malloc(VALUE_BUFFER_LENGTH * sizeof(*buffer));
    float pressure = (float) (3000.0 + ((rand() % 256) / 64.0));
    snprintf(*buffer, VALUE_BUFFER_LENGTH, "%4.2f", pressure);

    return MCL_TRUE;
}

mcl_bool_t read_voltage_motor_a(char **buffer)
{
    (*buffer) = (char*)malloc(VALUE_BUFFER_LENGTH * sizeof(*buffer));
    float voltage_motor_a = (float) (220.0 + ((rand() % 32) / 64.0));
    snprintf(*buffer, VALUE_BUFFER_LENGTH, "%3.3f", voltage_motor_a);

    return MCL_TRUE;
}

mcl_bool_t read_current_motor_a(char **buffer)
{
    (*buffer) = (char*)malloc(VALUE_BUFFER_LENGTH * sizeof(*buffer));
    float current_motor_a = (float) (5.0 + ((float)(rand() % 32) / 64.0));
    snprintf(*buffer, VALUE_BUFFER_LENGTH, "%1.5f", current_motor_a);

    return MCL_TRUE;
}

mcl_bool_t read_voltage_motor_b(char **buffer)
{
    (*buffer) = (char*)malloc(VALUE_BUFFER_LENGTH * sizeof(*buffer));
    float voltage_motor_b = (float) (380.0 + ((float)(rand() % 32) / 64.0));
    snprintf(*buffer, VALUE_BUFFER_LENGTH, "%3.3f", voltage_motor_b);

    return MCL_TRUE;
}

mcl_bool_t read_current_motor_b(char **buffer)
{
    (*buffer) = (char*)malloc(VALUE_BUFFER_LENGTH * sizeof(*buffer));
    float current_motor_b = (float) (60.0 + ((rand() % 32) / 64.0));
    snprintf(*buffer, VALUE_BUFFER_LENGTH, "%2.4f", current_motor_b);

    return MCL_TRUE;
}

static void sleep_in_seconds(mcl_uint32_t seconds)
{
#if defined(WIN32) || defined(WIN64)
    Sleep(seconds * 1000);
#else
    sleep(seconds);
#endif
}
