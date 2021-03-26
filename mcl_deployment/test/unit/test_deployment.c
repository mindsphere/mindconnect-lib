/*!
 * @file     test_deployment.c
 * @brief    This file implements unit test cases for deployment module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_deployment/mcl_deployment_common.h"
#include "deployment_common.h"
#include "mcl_deployment/mcl_deployment.h"
#include "deployment.h"
#include "mcl_deployment/mcl_deployment_configuration.h"
#include "deployment_configuration.h"
#include "mcl_deployment/mcl_deployment_workflow_filter.h"
#include "deployment_workflow_filter.h"
#include "mcl_deployment/mcl_deployment_workflow_model.h"
#include "deployment_workflow_model.h"
#include "mcl_deployment/mcl_deployment_workflow_model_state.h"
#include "deployment_workflow_model_state.h"
#include "mcl_deployment/mcl_deployment_workflow_model_state_group.h"
#include "deployment_workflow_model_state_group.h"
#include "mcl_deployment/mcl_deployment_workflow_model_transition.h"
#include "deployment_workflow_model_transition.h"
#include "deployment_processor.h"
#include "deployment_json.h"
#include "deployment_workflow.h"
#include "deployment_workflow_state.h"
#include "deployment_workflow_model.h"

#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"
#include "mcl_core/mcl_list.h"

#include "mock_mcl_core.h"
#include "mock_mcl_http_client.h"
#include "mock_mcl_http_response.h"
#include "mock_mcl_http_request.h"

char *workflow_list_payload = 
"{"\
    "\"content\":"\
    "["\
        "{"\
            "\"id\":\"e87f48b3-1ce3-4f99-b7e3-50e1ec48ca65\","\
            "\"deviceId\":\"be69cb32-0377-4690-8cca-a315211c609f\","\
            "\"createdAt\":\"2021-01-15T06:50:20.074Z\","\
            "\"currentState\":"\
            "{"\
                "\"entered\":\"2021-01-15T06:50:20.104Z\","\
                "\"updated\": null,"\
                "\"progress\": 0.0,"\
                "\"message\":\"State machine is triggered instantaneously.\","\
                "\"details\":null,"\
                "\"state\":\"DOWNLOAD\""\
            "},"\
            "\"history\":"\
            "["\
                "{"\
                    "\"entered\":\"2021-01-15T06:50:20.104Z\","\
                    "\"updated\": null,"\
                    "\"progress\": 0.0,"\
                    "\"message\":\"State machine is triggered instantaneously.\","\
                    "\"details\":null,"\
                    "\"state\":\"DOWNLOAD\""\
                "}"\
            "],"\
            "\"model\":"\
            "{"\
                "\"key\":\"mdsp-core-firmwareInstallation\","\
                "\"states\":"\
                "["\
                    "{"\
                        "\"name\":\"DOWNLOAD\","\
                        "\"description\":\"string\","\
                        "\"initial\":true,"\
                        "\"final\":true,"\
                        "\"cancel\":true"\
                    "}"\
                "],"\
                "\"transitions\":"\
                "["\
                    "{"\
                        "\"from\":\"DOWNLOAD\","\
                        "\"to\":\"DOWNLOADING\","\
                        "\"type\":\"DEVICE-TRIGGER\","\
                        "\"details\":"\
                        "{"\
                            "\"userDefined\":{}"\
                        "}"\
                    "}"\
                "],"\
                "\"stateGroups\":"\
                "["\
                    "{"\
                        "\"name\":\"device triggered\","\
                        "\"states\":"\
                        "["\
                            "\"DOWNLOAD\""\
                        "]"\
                    "}"\
                "]"\
            "},"\
            "\"data\":{}"\
        "}"\
    "]"\
"}";

char *workflow_payload = 
"{"\
    "\"id\":\"e87f48b3-1ce3-4f99-b7e3-50e1ec48ca65\","\
    "\"deviceId\":\"be69cb32-0377-4690-8cca-a315211c609f\","\
    "\"createdAt\":\"2021-01-15T06:50:20.074Z\","\
    "\"currentState\":"\
    "{"\
        "\"entered\":\"2021-01-15T06:50:20.104Z\","\
        "\"updated\": null,"\
        "\"progress\": 0.0,"\
        "\"message\":\"State machine is triggered instantaneously.\","\
        "\"details\":null,"\
        "\"state\":\"DOWNLOAD\""\
    "},"\
    "\"history\":"\
    "["\
        "{"\
            "\"entered\":\"2021-01-15T06:50:20.104Z\","\
            "\"updated\": null,"\
            "\"progress\": 0.0,"\
            "\"message\":\"State machine is triggered instantaneously.\","\
            "\"details\":null,"\
            "\"state\":\"DOWNLOAD\""\
        "}"\
    "],"\
    "\"model\":"\
    "{"\
        "\"key\":\"mdsp-core-firmwareInstallation\","\
        "\"states\":"\
        "["\
            "{"\
                "\"name\":\"DOWNLOAD\","\
                "\"description\":\"string\","\
                "\"initial\":true,"\
                "\"final\":true,"\
                "\"cancel\":true"\
             "}"\
        "],"\
        "\"transitions\":"\
        "["\
            "{"\
                "\"from\":\"DOWNLOAD\","\
                "\"to\":\"DOWNLOADING\","\
                "\"type\":\"DEVICE-TRIGGER\","\
                "\"details\":"\
                "{"\
                    "\"userDefined\":{}"\
                "}"\
            "}"\
        "],"\
        "\"stateGroups\":"\
        "["\
            "{"\
                "\"name\":\"device triggered\","\
                "\"states\":"\
                "["\
                    "\"DOWNLOAD\""\
                "]"\
            "}"\
        "]"\
    "},"\
    "\"data\":{}"\
"}";

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Configuration parameter is null.
 * WHEN  : mcl_deployment_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_001(void)
{
    mcl_error_t code;
    mcl_deployment_t *deployment = MCL_NULL;

    // Test.
    code = mcl_deployment_initialize(MCL_NULL, &deployment);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_deployment_initialize() should have returned MCL_TRIGGERED_WITH_NULL.");
}

/**
 * GIVEN : Deployment parameter is null.
 * WHEN  : mcl_deployment_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_002(void)
{
    mcl_error_t code;
    mcl_deployment_configuration_t configuration;

    // Test.
    code = mcl_deployment_initialize(&configuration, MCL_NULL);

    // Check result.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_deployment_initialize() should have returned MCL_TRIGGERED_WITH_NULL.");
}

/**
 * GIVEN : Core parameter of deployment configuration is not set.
 * WHEN  : mcl_deployment_initialize() is called.
 * THEN  : MCL_INVALID_PARAMETER is returned.
 */
void test_initialize_003(void)
{
    mcl_error_t code;
    mcl_deployment_configuration_t *configuration = MCL_NULL;
    mcl_deployment_t *deployment = MCL_NULL;

    // Initialize configuration.
    code = mcl_deployment_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization of configuration failed.");

    // Test.
    code = mcl_deployment_initialize(configuration, &deployment);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_deployment_initialize() should have returned MCL_INVALID_PARAMETER.");

    // Clean up.
    mcl_deployment_configuration_destroy(&configuration);
}

/**
 * GIVEN : Initialized deployment configuration.
 * WHEN  : mcl_deployment_initialize() is called.
 * THEN  : MCL_OK is returned.
 */
void test_initialize_004(void)
{
    mcl_error_t code;
    mcl_core_t *core = (mcl_core_t*) 0xABCD;
    mcl_deployment_configuration_t *configuration = MCL_NULL;
    mcl_deployment_t *deployment = MCL_NULL;
    char *hostname = "hostname";
    mcl_http_client_t http_client;

    // Mock core functions.
    mcl_core_get_host_name_ExpectAnyArgsAndReturn(hostname);
    mcl_core_get_http_client_IgnoreAndReturn(&http_client);

    // Initialize configuration.
    code = mcl_deployment_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization of deployment configuration failed.");

    // Set parameters of configuration.
    code = mcl_deployment_configuration_set_parameter(configuration, MCL_DEPLOYMENT_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_deployment_configuration_set_parameter() failed.");

    // Test.
    code = mcl_deployment_initialize(configuration, &deployment);

    // Check results.
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization of deployment component failed.");
    TEST_ASSERT_EQUAL_MESSAGE(configuration, deployment->configuration, "Configuration is not set.");
    TEST_ASSERT_NULL_MESSAGE(deployment->processor.access_token, "Access token must be initially null.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&http_client, deployment->processor.http_client, "Http client is not set.");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(configuration->workflow_instances_url, deployment->processor.workflow_instances_url, "URL is not set.");

    // Clean up.
    mcl_deployment_destroy(&deployment);
    mcl_deployment_configuration_destroy(&configuration);
}

/**
 * GIVEN : Initialized deployment.
 * WHEN  : mcl_deployment_list_workflows() is called.
 * THEN  : MCL_OK is returned.
 */
void test_list_workflows_001(void)
{
    mcl_error_t code;
    mcl_core_t *core = (mcl_core_t*) 0xABCD;
    mcl_deployment_configuration_t *configuration = MCL_NULL;
    mcl_deployment_t *deployment = MCL_NULL;
    char *hostname = "hostname";
    mcl_http_client_t http_client;
    mcl_list_t *workflows = MCL_NULL;

    // Mock core functions.
    mcl_core_get_host_name_ExpectAnyArgsAndReturn(hostname);
    mcl_core_get_http_client_IgnoreAndReturn(&http_client);

    // Initialize configuration.
    code = mcl_deployment_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Set parameters of configuration.
    code = mcl_deployment_configuration_set_parameter(configuration, MCL_DEPLOYMENT_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Initialize deployment component.
    code = mcl_deployment_initialize(configuration, &deployment);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Mock access token.
    char *token = MCL_MALLOC(8);
    mcl_core_get_last_access_token_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_core_get_last_access_token_ReturnThruPtr_token(&token);
    
    // Mock http request.
    mcl_http_request_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);
    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);
    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);
    mcl_http_request_destroy_Ignore();

    // Mock http response.
    mcl_http_response_t *http_response = MCL_NULL;
    MCL_NEW(http_response);
    http_response->header = MCL_NULL;
    http_response->payload = workflow_list_payload;
    http_response->payload_size = mcl_string_util_strlen(workflow_list_payload);
    http_response->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    mcl_http_response_destroy_Ignore();

    // Mock http client.
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response); 

    // Create a filter for the response.
    mcl_deployment_workflow_filter_t *filter = MCL_NULL;
    mcl_bool_t history = MCL_TRUE;
    mcl_bool_t model = MCL_TRUE;
    char *state = "DOWNLOAD";
    char *device_id = "be69cb32-0377-4690-8cca-a315211c609f";
    char *group = "firmware";
    char *model_key = "mdsp-core-firmwareInstallation";

    mcl_deployment_workflow_filter_initialize(&filter);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_HISTORY, &history);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_MODEL, &model);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_CURRENT_STATE, state);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_DEVICE_ID, device_id);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_GROUP, group);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_MODEL_KEY, model_key);
    
    // Test.
    code = mcl_deployment_list_workflows(deployment, filter, &workflows);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_NOT_NULL(workflows);

    // Clean up.
    MCL_FREE(http_response);
    mcl_list_destroy_with_content(&workflows, (mcl_list_item_destroy_callback) mcl_deployment_workflow_destroy);
    mcl_deployment_destroy(&deployment);
    mcl_deployment_configuration_destroy(&configuration);
    mcl_deployment_workflow_filter_destroy(&filter);
}

/**
 * GIVEN : Initialized deployment.
 * WHEN  : mcl_deployment_get_workflow() is called.
 * THEN  : MCL_OK is returned.
 */
void test_get_workflow_001(void)
{
    mcl_error_t code;
    mcl_core_t *core = (mcl_core_t *) 0xABCD;
    mcl_deployment_configuration_t *configuration = MCL_NULL;
    mcl_deployment_t *deployment = MCL_NULL;
    char *hostname = "hostname";
    mcl_http_client_t http_client;
    mcl_deployment_workflow_t *workflow = MCL_NULL;

    // Mock core functions.
    mcl_core_get_host_name_ExpectAnyArgsAndReturn(hostname);
    mcl_core_get_http_client_IgnoreAndReturn(&http_client);

    // Initialize configuration.
    code = mcl_deployment_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Set parameters of configuration.
    code = mcl_deployment_configuration_set_parameter(configuration, MCL_DEPLOYMENT_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Initialize deployment component.
    code = mcl_deployment_initialize(configuration, &deployment);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Mock access token.
    char *token = MCL_MALLOC(8);
    mcl_core_get_last_access_token_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_core_get_last_access_token_ReturnThruPtr_token(&token);

    // Mock http request.
    mcl_http_request_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);
    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);
    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);
    mcl_http_request_destroy_Ignore();

    // Mock http response.
    mcl_http_response_t *http_response = MCL_NULL;
    MCL_NEW(http_response);
    http_response->header = MCL_NULL;
    http_response->payload = workflow_payload;
    http_response->payload_size = mcl_string_util_strlen(workflow_payload);
    http_response->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    mcl_http_response_destroy_Ignore();

    // Mock http client.
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // Create a filter for the response.
    mcl_deployment_workflow_filter_t *filter = MCL_NULL;
    mcl_bool_t history = MCL_TRUE;
    mcl_bool_t model = MCL_TRUE;

    mcl_deployment_workflow_filter_initialize(&filter);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_HISTORY, &history);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_MODEL, &model);

    // Test.
    code = mcl_deployment_get_workflow(deployment,"e87f48b3-1ce3-4f99-b7e3-50e1ec48ca65", filter, &workflow);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_NOT_NULL(workflow);

    // Clean up.
    MCL_FREE(http_response);
    mcl_deployment_workflow_destroy(&workflow);
    mcl_deployment_destroy(&deployment);
    mcl_deployment_configuration_destroy(&configuration);
    mcl_deployment_workflow_filter_destroy(&filter);
}

/**
 * GIVEN : Initialized deployment.
 * WHEN  : mcl_deployment_update_workflow() is called.
 * THEN  : MCL_OK is returned.
 */
void test_update_workflow_001(void)
{
    mcl_error_t code;
    mcl_core_t *core = (mcl_core_t *) 0xABCD;
    mcl_deployment_configuration_t *configuration = MCL_NULL;
    mcl_deployment_t *deployment = MCL_NULL;
    char *hostname = "hostname";
    mcl_http_client_t http_client;
    mcl_deployment_workflow_t *workflow = MCL_NULL;

    // Mock core functions.
    mcl_core_get_host_name_ExpectAnyArgsAndReturn(hostname);
    mcl_core_get_http_client_IgnoreAndReturn(&http_client);

    // Initialize configuration.
    code = mcl_deployment_configuration_initialize(&configuration);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Set parameters of configuration.
    code = mcl_deployment_configuration_set_parameter(configuration, MCL_DEPLOYMENT_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Initialize deployment component.
    code = mcl_deployment_initialize(configuration, &deployment);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Mock access token.
    char* token = MCL_MALLOC(8);
    mcl_core_get_last_access_token_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_core_get_last_access_token_ReturnThruPtr_token(&token);

    // Mock http request.
    mcl_http_request_initialize_IgnoreAndReturn(MCL_OK);
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);
    mcl_http_request_set_parameter_IgnoreAndReturn(MCL_OK);
    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);
    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);
    mcl_http_request_add_header_IgnoreAndReturn(MCL_OK);
    mcl_http_request_destroy_Ignore();

    // Mock http response.
    mcl_http_response_t *http_response = MCL_NULL;
    MCL_NEW(http_response);
    http_response->header = MCL_NULL;
    http_response->payload = workflow_payload;
    http_response->payload_size = mcl_string_util_strlen(workflow_payload);
    http_response->status_code = MCL_HTTP_STATUS_CODE_SUCCESS;
    mcl_http_response_destroy_Ignore();

    // Mock http client.
    mcl_http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    mcl_http_client_send_ReturnThruPtr_http_response(&http_response);

    // Create a filter for the response.
    mcl_deployment_workflow_filter_t *filter = MCL_NULL;
    mcl_bool_t history = MCL_TRUE;
    mcl_bool_t model = MCL_TRUE;
    
    mcl_deployment_workflow_filter_initialize(&filter);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_HISTORY, &history);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_MODEL, &model);

    // Test.
    mcl_deployment_workflow_state_t *new_state = MCL_NULL;
    char *state = "DOWNLOADING";
    double progress = 0.5;
    char *message = "Download in progress.";

    code = mcl_deployment_workflow_state_initialize(&new_state);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_deployment_workflow_state_set_parameter(new_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, state);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_deployment_workflow_state_set_parameter(new_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_PROGRESS, &progress);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_deployment_workflow_state_set_parameter(new_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_MESSAGE, message);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_deployment_update_workflow(deployment, "e87f48b3-1ce3-4f99-b7e3-50e1ec48ca65", new_state, MCL_NULL, &workflow);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_NOT_NULL(workflow);

    // Clean up.
    MCL_FREE(http_response);
    mcl_deployment_workflow_state_destroy(&new_state);
    mcl_deployment_workflow_destroy(&workflow);
    mcl_deployment_destroy(&deployment);
    mcl_deployment_configuration_destroy(&configuration);
    mcl_deployment_workflow_filter_destroy(&filter);
}
