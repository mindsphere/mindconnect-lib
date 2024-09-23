/*!
 * @file     dsc_upload.c
 * @brief    Example custom agent code which uploads data source configuration.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mcl_connectivity/mcl_connectivity.h"
#include "mcl_core/mcl_random.h"
#include "callbacks.h"

const char *mindsphere_certificate = "-----BEGIN CERTIFICATE-----\n"
"MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n"
"MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n"
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n"
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n"
"2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n"
"1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n"
"q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n"
"tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n"
"vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n"
"BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n"
"5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n"
"1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n"
"NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n"
"Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n"
"8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n"
"pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n"
"MrY=\n"
"-----END CERTIFICATE-----\n"
"-----BEGIN CERTIFICATE-----\n"\
"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\n"\
"-----END CERTIFICATE-----";


// Replace with your own initial access token.
const char *initial_access_token = "<Initial Access Token>";

// Name of the configuration file which is used to save data point ids.
const char *configuration_file_name = "configuration.txt";

// DATA_SOURCE_COUNT is used in main function: keep the name, change the value and update the tables accordingly if necessary.
#define DATA_SOURCE_COUNT 3

// These definitions are not used in main function: add/change/remove as long as the tables are kept consistent.
#define DATA_POINT_COUNT_MOTOR 2
#define DATA_POINT_COUNT_ENVIRONMENTAL_SENSOR 3
#define CUSTOM_FIELD_COUNT_MOTOR 2
#define CUSTOM_FIELD_COUNT_VOLTAGE 2

// Only the tables with size DATA_SOURCE_COUNT are directly accessed in main.
// Data points for motor source: Name, Type, Unit, Description.
const char *data_point_voltage[4] = {"Voltage", "DOUBLE", "Volt", "Voltage Description"};
const char *data_point_current[4] = {"Current", "DOUBLE", "Ampere", "Current Description"};
const char **data_points_motor[DATA_POINT_COUNT_MOTOR] = {data_point_voltage, data_point_current};

// Data points for environmental sensor source: Name, Type, Unit, Description.
const char *data_point_temperature[4] = {"Temperature", "DOUBLE", "Celsius", "Ambient Temperature"};
const char *data_point_humidity[4] = {"Humidity", "DOUBLE", "%", "Relative Humidity"};
const char *data_point_pressure[4] = {"Pressure", "DOUBLE", "hPa", "Atmospheric Pressure"};
const char **data_points_environmental_sensor[DATA_POINT_COUNT_ENVIRONMENTAL_SENSOR] = {data_point_temperature, data_point_humidity, data_point_pressure};

// Data point table.
const char ***data_point_table[DATA_SOURCE_COUNT] = {data_points_motor, data_points_motor, data_points_environmental_sensor};
const mcl_size_t data_point_count_table[DATA_SOURCE_COUNT] = {DATA_POINT_COUNT_MOTOR, DATA_POINT_COUNT_MOTOR, DATA_POINT_COUNT_ENVIRONMENTAL_SENSOR};

// Data sources: Name, Description.
const char *data_source_motor_a[2] = {"Motor A", "Motor A Description"};
const char *data_source_motor_b[2] = {"Motor B", "Motor B Description"};
const char *data_source_environmental_sensor[2] = {"Environmental Sensors", "Environmental Sensors Description"};

// Data source table.
const char **data_source_table[DATA_SOURCE_COUNT] = {data_source_motor_a, data_source_motor_b, data_source_environmental_sensor};

// Data source custom fields for motor.
const char *custom_field_motor[CUSTOM_FIELD_COUNT_MOTOR] = {"CustomFieldX_Motor", "CustomFieldY_Motor"};
const char *custom_value_motor_a[CUSTOM_FIELD_COUNT_MOTOR] = {"CustomValueX_MotorA", "CustomValueY_MotorA"};
const char *custom_value_motor_b[CUSTOM_FIELD_COUNT_MOTOR] = {"CustomValueX_MotorB", "CustomValueY_MotorB"};

// There is no custom data field for environmental sensor for this example.
const char **data_source_custom_fields[DATA_SOURCE_COUNT] = {custom_field_motor, custom_field_motor, NULL};
const char **data_source_custom_values[DATA_SOURCE_COUNT] = {custom_value_motor_a, custom_value_motor_b, NULL};
const mcl_size_t data_source_custom_field_count[DATA_SOURCE_COUNT] = {CUSTOM_FIELD_COUNT_MOTOR, CUSTOM_FIELD_COUNT_MOTOR, 0};

// Data point custom fields for voltage.
const char *custom_field_voltage[CUSTOM_FIELD_COUNT_VOLTAGE] = {"CustomFieldX_Voltage", "CustomFieldY_Voltage"};
const char *custom_value_voltage[CUSTOM_FIELD_COUNT_VOLTAGE] = {"CustomValueX_Voltage", "CustomValueY_Voltage"};

// There is no data point custom field for current for this example.
const char **data_point_custom_fields_motor[DATA_POINT_COUNT_MOTOR] = {custom_field_voltage, NULL};
const char **data_point_custom_values_motor[DATA_POINT_COUNT_MOTOR] = {custom_value_voltage, NULL};
mcl_size_t data_point_custom_field_count_motor[DATA_POINT_COUNT_MOTOR] = {CUSTOM_FIELD_COUNT_VOLTAGE, 0};

// There is no data point custom field for any data point in environmental sensor source for this example.
const char ***data_point_custom_fields[DATA_SOURCE_COUNT] = {data_point_custom_fields_motor, data_point_custom_fields_motor, NULL};
const char ***data_point_custom_values[DATA_SOURCE_COUNT] = {data_point_custom_values_motor, data_point_custom_values_motor, NULL};
mcl_size_t *data_point_custom_field_counts[DATA_SOURCE_COUNT] = {data_point_custom_field_count_motor, data_point_custom_field_count_motor, NULL};

void save_data_source_configuration(char *configuration_string)
{
    FILE *fd = fopen(configuration_file_name, "w");
    fputs(configuration_string, fd);
    fclose(fd);
}

int main(void)
{
    printf("***** CUSTOM AGENT using MCL *****\n");

    mcl_error_t code;
    mcl_core_t *core = NULL;
    mcl_connectivity_t *connectivity = NULL;

    // Core configuration and related parameters.
    mcl_core_configuration_t *core_configuration = NULL;
    mcl_uint16_t mindsphere_port = 443;
    mcl_uint16_t proxy_port = 9400;
    E_MCL_PROXY mcl_proxy = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_SHARED_SECRET;

    // Connectivity configuration.
    mcl_connectivity_configuration_t *connectivity_configuration = NULL;

    // Data source configuration.
    mcl_data_source_configuration_t *data_source_configuration = NULL;

    // These will be used to save data source configuration.
    mcl_json_t *configuration_json = NULL;
    mcl_json_t *data_sources_json = NULL;
    char *configuration_string = NULL;

    // Data source configuration id.
    char *configuration_id = NULL;

    // Set log level. Note that default log level is the level set during build.
    mcl_log_util_set_output_level(MCL_LOG_LEVEL_INFO);

    // Initialize core configuration.
    code = mcl_core_configuration_initialize(&core_configuration);
    printf("Core configuration initialization : %s.\n", MCL_CORE_CODE_TO_STRING(code));

    if (MCL_OK == code)
    {
        // Set core configuration parameters.
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, "https://southgate.eu1.mindsphere.io");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mindsphere_certificate);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, "custom agent v1.0");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, "194.138.0.5");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &mcl_proxy);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, initial_access_token);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, "mclibiot");
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_LOAD_CALLBACK, custom_load_function_shared_secret);
        mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CREDENTIALS_SAVE_CALLBACK, custom_save_function_shared_secret);

        // Initialize mcl core with the core configuration.
        code = mcl_core_initialize(core_configuration, &core);
        printf("MCL core initialization : %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

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

    // Initialize mcl connectivity.
    if (MCL_OK == code)
    {
        code = mcl_connectivity_initialize(connectivity_configuration, &connectivity);
    }

    // Create data source configuration.
    if (MCL_OK == code)
    {      
        code = mcl_data_source_configuration_initialize(MCL_DATA_SOURCE_CONFIGURATION_1_0, &data_source_configuration);
        printf("Data source configuration creation : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
    }

    if (MCL_OK == code)
    {
        code = mcl_random_generate_guid(&configuration_id);
        printf("Data source configuration id creation : %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    if (MCL_OK == code)
    {
        code = mcl_data_source_configuration_set_parameter(data_source_configuration, MCL_DATA_SOURCE_CONFIGURATION_PARAMETER_ID, configuration_id);
        printf("Data source configuration id setting : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_initialize(MCL_JSON_OBJECT, &configuration_json);
        printf("Configuration json initialization : %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_start_object(configuration_json, configuration_id, &data_sources_json);
        printf("Data sources json initialization : %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    for (mcl_size_t i = 0; i < DATA_SOURCE_COUNT && MCL_OK == code; i++)
    {
        mcl_data_source_t *data_source = NULL;
        mcl_json_t *data_source_json = NULL;

        code = mcl_data_source_initialize(&data_source);
        printf("\nData source creation for \"%s\" : %s.\n", data_source_table[i][0], MCL_CONNECTIVITY_CODE_TO_STRING(code));

        if (MCL_OK == code)
        {
            code = mcl_json_util_start_object(data_sources_json, data_source_table[i][0], &data_source_json);
            printf("|-->Data source json initialization : %s.\n", MCL_CORE_CODE_TO_STRING(code));
        }

        if (MCL_OK == code)
        {
            code = mcl_data_source_set_parameter(data_source, MCL_DATA_SOURCE_PARAMETER_NAME, data_source_table[i][0]);
            printf("|-->Data source name setting : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
        }
        
        if (MCL_OK == code)
        {
            code = mcl_data_source_set_parameter(data_source, MCL_DATA_SOURCE_PARAMETER_DESCRIPTION, data_source_table[i][1]);
            printf("|-->Data source description setting to \"%s\" : %s.\n", data_source_table[i][1], MCL_CONNECTIVITY_CODE_TO_STRING(code));
        }

        if (MCL_OK == code && 0 != data_source_custom_field_count[i])
        {
            mcl_json_t *data_source_custom_data = NULL;

            code = mcl_json_util_initialize(MCL_JSON_OBJECT, &data_source_custom_data);
            printf("|-->Custom data json for data source creation : %s.\n", MCL_CORE_CODE_TO_STRING(code));

            for (mcl_size_t j = 0; j < data_source_custom_field_count[i] && MCL_OK == code; j++)
            {
                code = mcl_json_util_add_string(data_source_custom_data, data_source_custom_fields[i][j], data_source_custom_values[i][j]);
                printf("    |-->Custom field adding \"%s\" with value \"%s\" : %s.\n", data_source_custom_fields[i][j], data_source_custom_values[i][j], MCL_CORE_CODE_TO_STRING(code));
            }

            if (MCL_OK == code)
            {
                code = mcl_data_source_set_parameter(data_source, MCL_DATA_SOURCE_PARAMETER_CUSTOM_DATA, data_source_custom_data);
                printf("|-->Custom data field setting : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
            }

            mcl_json_util_destroy(&data_source_custom_data);
        }

        for (mcl_size_t j = 0; j < data_point_count_table[i] && MCL_OK == code; j++)
        {
            mcl_data_point_t *data_point = NULL;
            char *data_point_id = NULL;

            code = mcl_data_point_initialize(&data_point);
            printf("|\n|-->Data point creation for \"%s\" : %s.\n", data_point_table[i][j][0], MCL_CONNECTIVITY_CODE_TO_STRING(code));

            if (MCL_OK == code)
            {
                code = mcl_random_generate_guid(&data_point_id);
                printf("    |-->Data point id creation : %s.\n", MCL_CORE_CODE_TO_STRING(code));
            }

            if (MCL_OK == code)
            {
                code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_ID, data_point_id);
                printf("    |-->Data point id setting : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
            }

            if (MCL_OK == code)
            {
                code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_NAME, data_point_table[i][j][0]);
                printf("    |-->Data point name setting : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
            }

            if (MCL_OK == code)
            {
                code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_TYPE, data_point_table[i][j][1]);
                printf("    |-->Data point type setting to \"%s\" : %s.\n", data_point_table[i][j][1], MCL_CONNECTIVITY_CODE_TO_STRING(code));
            }

            if (MCL_OK == code)
            {
                code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_UNIT, data_point_table[i][j][2]);
                printf("    |-->Data point unit setting to \"%s\" : %s.\n", data_point_table[i][j][2], MCL_CONNECTIVITY_CODE_TO_STRING(code));
            }

            if (MCL_OK == code)
            {
                code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_DESCRIPTION, data_point_table[i][j][3]);
                printf("    |-->Data point description setting to \"%s\" : %s.\n", data_point_table[i][j][3], MCL_CONNECTIVITY_CODE_TO_STRING(code));
            }

            if (MCL_OK == code && NULL != data_point_custom_field_counts[i] && 0 != data_point_custom_field_counts[i][j])
            {
                mcl_json_t *data_point_custom_data = NULL;

                code = mcl_json_util_initialize(MCL_JSON_OBJECT, &data_point_custom_data);
                printf("    |-->Custom data json creation for data point : %s.\n", MCL_CORE_CODE_TO_STRING(code));

                for (mcl_size_t k = 0; k < data_point_custom_field_counts[i][j] && MCL_OK == code; k++)
                {
                    code = mcl_json_util_add_string(data_point_custom_data, data_point_custom_fields[i][j][k], data_point_custom_values[i][j][k]);
                    printf("        |-->Custom field adding \"%s\" with value \"%s\" : %s.\n", data_point_custom_fields[i][j][k], data_point_custom_values[i][j][k], MCL_CONNECTIVITY_CODE_TO_STRING(code));
                }

                if (MCL_OK == code)
                {
                    code = mcl_data_point_set_parameter(data_point, MCL_DATA_POINT_PARAMETER_CUSTOM_DATA, data_point_custom_data);
                    printf("    |-->Custom data field setting : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
                }

                mcl_json_util_destroy(&data_point_custom_data);
            }

            if (MCL_OK == code)
            {
                code = mcl_data_source_add_data_point(data_source, data_point);
                printf("    |-->Data point to data source adding : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
            }

            if (MCL_OK == code)
            {
                code = mcl_json_util_add_string(data_source_json, data_point_table[i][j][0], data_point_id);
                printf("    |-->Data point id to data source json adding : %s.\n", MCL_CORE_CODE_TO_STRING(code));
            }

            if (MCL_OK != code)
            {
                mcl_data_point_destroy(&data_point);
            }

            free(data_point_id);
        }

        if (MCL_OK == code)
        {
            code = mcl_data_source_configuration_add_data_source(data_source_configuration, data_source);
            printf("|-->Data source to data source configuration adding : %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
        }

        if (MCL_OK != code)
        {
            mcl_data_source_destroy(&data_source);
        }
    }

    if (MCL_OK == code)
    {
        // Upload the data source configuration.
        code = mcl_connectivity_exchange(connectivity, data_source_configuration);
        printf("Data source configuration upload: %s.\n", MCL_CONNECTIVITY_CODE_TO_STRING(code));
    }

    if (MCL_OK == code)
    {
        code = mcl_json_util_to_string(configuration_json, &configuration_string);
        printf("Configuration json to string conversion : %s.\n", MCL_CORE_CODE_TO_STRING(code));
    }

    if (MCL_OK == code)
    {
        printf("Saving configuration.\n");
        save_data_source_configuration(configuration_string);
    }

    // Clean up.
    free(configuration_id);
    free(configuration_string);
    mcl_json_util_destroy(&configuration_json);
    mcl_connectivity_destroy(&connectivity);
    mcl_connectivity_configuration_destroy(&connectivity_configuration);
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&core_configuration);
    mcl_data_source_configuration_destroy(&data_source_configuration);

    if (MCL_OK != code)
    {
        return -1;
    }

    return 0;
}

