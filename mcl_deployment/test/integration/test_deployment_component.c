/*!
 * @file     test_deployment_component.c
 * @brief    Integration tests for mcl_deployment component.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_deployment/mcl_deployment.h"

 //J-
char *mock_server_certificate =
"-----BEGIN CERTIFICATE-----\n"\
"MIIDqDCCApCgAwIBAgIEo9uk7jANBgkqhkiG9w0BAQsFADBiMRswGQYDVQQDDBJ3\n"\
"d3cubW9ja3NlcnZlci5jb20xEzARBgNVBAoMCk1vY2tTZXJ2ZXIxDzANBgNVBAcM\n"\
"BkxvbmRvbjEQMA4GA1UECAwHRW5nbGFuZDELMAkGA1UEBhMCVUswIBcNMTUwMjEz\n"\
"MTYzMDA2WhgPMjExNjAxMjAxNjMwMDZaMGIxGzAZBgNVBAMMEnd3dy5tb2Nrc2Vy\n"\
"dmVyLmNvbTETMBEGA1UECgwKTW9ja1NlcnZlcjEPMA0GA1UEBwwGTG9uZG9uMRAw\n"\
"DgYDVQQIDAdFbmdsYW5kMQswCQYDVQQGEwJVSzCCASIwDQYJKoZIhvcNAQEBBQAD\n"\
"ggEPADCCAQoCggEBAIslQgdbmc1UDLjtCsFVeFEPHZWWQsIJkcLCT15QJbLnMPEm\n"\
"pyiSo07DQ1xQk7WCBOWWBuhJt+MwEAK5QFR/MLf11xcUF26PkO0GO01lEPtS6M8n\n"\
"pXpNCYKFF+gZ0q1hmxZ08ZQD+6LAPnLx5XUKk/XvjEpKRz01Sza4nUhgc4QDnN+B\n"\
"CiamQ4fzhbwR6eTPm9Z266RYhK+oPBBN1AxE/+CwO0nV85n9EYimn5kfZqyideCQ\n"\
"3ZbjPzXgfXctC9cWxKK4eEdfjMRnxxwAVVPCJU59sMv6JjhDV3YKWX4UMevsc6bf\n"\
"Q/KOgVAoFJuFXbAiS+oE5/JOW0yU9fHngD7QWUMCAwEAAaNkMGIwHQYDVR0OBBYE\n"\
"FKmWp1h2Zhsn4FP97H977H4izXGkMA8GA1UdEwEB/wQFMAMBAf8wCwYDVR0PBAQD\n"\
"AgG2MCMGA1UdJQQcMBoGCCsGAQUFBwMBBggrBgEFBQcDAgYEVR0lADANBgkqhkiG\n"\
"9w0BAQsFAAOCAQEAFW+ARa1RmSjbX82aj5+4hAVb3hiVcEVfDD/yfSwEYP93yNgV\n"\
"u74z/a3eqrzQKWveKH+CTGaRij1WNAknswrBijSEppNKEebg/J/vpXNwaDNK49oi\n"\
"8zbNAWPOB/mRK8ogY0qWXUHzt69r6Cm6Jid1TLKlpuJ60REZT8kfQlbXaNjy2g0N\n"\
"W254k/MBugmBQcKEJ/nfiiZe/1zLkn2JLQAgFf5LIvhGNBS/t7Fz0QiKwyfhyjP3\n"\
"t0YCdqheMQM8/rFjufCgJDu7IfieKguRoyRRbfSb6/gApANG3ARlLu6SoXK1TjfX\n"\
"gJvTmKj67HcgyjQgsSTx7r1O/HgxF4+rhpOpxg==\n"\
"-----END CERTIFICATE-----";
//J+

// Initialize configuration.
static mcl_core_configuration_t* _initialize_configuration(const char* user_agent);

void setUp(void)
{
}

void tearDown(void)
{
}

/**
* GIVEN : Deployment workflow is created for the agent.
* WHEN  : mcl_deployment_list_workflows() is called.
* THEN  : MCL_OK is returned and a list of deployment workflows are retrieved.
*/
void test_list_workflows_01(void)
{
    mcl_error_t code;
    mcl_core_t *core = MCL_NULL;
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=LIST_INSTANCES;HTTP_STATUS=200;%";
    mcl_core_configuration_t *core_configuration = _initialize_configuration(user_agent);

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(core_configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize deployment component configuration.
    mcl_deployment_configuration_t *configuration = MCL_NULL;
    code = mcl_deployment_configuration_initialize(&configuration);

    // Set core parameter of deployment configuration.
    code = mcl_deployment_configuration_set_parameter(configuration, MCL_DEPLOYMENT_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_deployment_configuration_set_parameter() failed.");

    // Initialize deployment component.
    mcl_deployment_t *deployment = MCL_NULL;
    code = mcl_deployment_initialize(configuration, &deployment);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for deployment component.");
    
    mcl_list_t *workflows = MCL_NULL;
    
    code = mcl_deployment_list_workflows(deployment, MCL_NULL, &workflows);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Listing workflow instances failed.");
    
    TEST_ASSERT_EQUAL_MESSAGE(1, workflows->count, "Number of workflows mismatch.");
    
    mcl_list_node_t *node = MCL_NULL;
    mcl_list_next(workflows, &node);
    mcl_deployment_workflow_t *workflow = (mcl_deployment_workflow_t*) node->data;

    char *id = MCL_NULL;
    char *device_id = MCL_NULL;
    char *created_at = MCL_NULL;
    
    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_ID, &id);
    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_DEVICE_ID, &device_id);
    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CREATED_AT, &created_at);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("1a5f74ef-83a0-4229-a7ed-21a175159dc9", id,"Workflow id mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("86286969-df31-414e-bb0c-5be930d415fb", device_id,"Device id mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2020-11-17T06:49:18.926Z", created_at,"Time created at mismatch.");
    
    mcl_deployment_workflow_state_t *workflow_state = MCL_NULL;
    char *state;
    double *progress;
    char *entered;
    char *updated;
    char *message;
    mcl_json_t *details;

    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CURRENT_STATE, &workflow_state);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, &state);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_PROGRESS, &progress);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_ENTERED, &entered);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_UPDATED, &updated);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_MESSAGE, &message);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_DETAILS, &details);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("DOWNLOAD", state,"Current state mismatch.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0, *progress,"State progress mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2020-11-17T06:49:18.926Z", entered,"Entered time mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2020-11-17T06:49:18.926Z", updated,"Updated time mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("New firmware release to download", message,"Message mismatch.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&core_configuration);
    mcl_deployment_destroy(&deployment);
    mcl_deployment_configuration_destroy(&configuration);
    mcl_list_destroy_with_content(&workflows, (mcl_list_item_destroy_callback) mcl_deployment_workflow_destroy);
}

/**
* GIVEN : Deployment workflow is created for the agent.
* WHEN  : mcl_deployment_list_workflows() is called with filter.
* THEN  : MCL_OK is returned and a list of deployment workflows are retrieved.
*/
void test_list_workflows_02(void)
{
    mcl_error_t code;
    mcl_core_t* core = MCL_NULL;
    const char* user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=LIST_INSTANCES;HTTP_STATUS=200;%";
    mcl_core_configuration_t* core_configuration = _initialize_configuration(user_agent);

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(core_configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize deployment component configuration.
    mcl_deployment_configuration_t* configuration = MCL_NULL;
    code = mcl_deployment_configuration_initialize(&configuration);

    // Set core parameter of deployment configuration.
    code = mcl_deployment_configuration_set_parameter(configuration, MCL_DEPLOYMENT_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_deployment_configuration_set_parameter() failed.");

    // Initialize deployment component.
    mcl_deployment_t *deployment = MCL_NULL;
    code = mcl_deployment_initialize(configuration, &deployment);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for deployment component.");

    mcl_list_t *workflows = MCL_NULL;

    mcl_deployment_workflow_filter_t *filter = MCL_NULL;
    code = mcl_deployment_workflow_filter_initialize(&filter);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization of workflow filter failed.");

    mcl_bool_t true = MCL_TRUE;
    char *filter_current_state = "DOWNLOAD";
    char *filter_device_id = "86286969-df31-414e-bb0c-5be930d415fb";
    char *filter_group = "iqfleet";
    char *filter_model_key = "core-simoticsconnect-firmware";

    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_MODEL, &true);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_HISTORY, &true);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_CURRENT_STATE, filter_current_state);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_DEVICE_ID, filter_device_id);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_GROUP, filter_group);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_MODEL_KEY, filter_model_key);

    code = mcl_deployment_list_workflows(deployment, filter, &workflows);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Listing workflow instances failed.");

    TEST_ASSERT_EQUAL_MESSAGE(1, workflows->count, "Number of workflows mismatch.");

    mcl_list_node_t* node = MCL_NULL;
    mcl_list_next(workflows, &node);
    mcl_deployment_workflow_t* workflow = (mcl_deployment_workflow_t*)node->data;

    char* id = MCL_NULL;
    char* device_id = MCL_NULL;
    char* created_at = MCL_NULL;

    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_ID, &id);
    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_DEVICE_ID, &device_id);
    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CREATED_AT, &created_at);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("1a5f74ef-83a0-4229-a7ed-21a175159dc9", id, "Workflow id mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("86286969-df31-414e-bb0c-5be930d415fb", device_id, "Device id mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2020-11-17T06:49:18.926Z", created_at, "Time created at mismatch.");

    mcl_deployment_workflow_state_t* workflow_state = MCL_NULL;
    char* state;
    double* progress;
    char* entered;
    char* updated;
    char* message;
    mcl_json_t* details;

    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CURRENT_STATE, &workflow_state);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, &state);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_PROGRESS, &progress);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_ENTERED, &entered);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_UPDATED, &updated);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_MESSAGE, &message);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_DETAILS, &details);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("DOWNLOAD", state, "Current state mismatch.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0, *progress, "State progress mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2020-11-17T06:49:18.926Z", entered, "Entered time mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2020-11-17T06:49:18.926Z", updated, "Updated time mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("New firmware release to download", message, "Message mismatch.");

    // Clean up.
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&core_configuration);
    mcl_deployment_destroy(&deployment);
    mcl_deployment_configuration_destroy(&configuration);
    mcl_list_destroy_with_content(&workflows, (mcl_list_item_destroy_callback)mcl_deployment_workflow_destroy);
    mcl_deployment_workflow_filter_destroy(&filter);
}

/**
* GIVEN : ID of a deployment workflow.
* WHEN  : mcl_deployment_get_workflow() is called.
* THEN  : MCL_OK is returned and deployment workflow description is retrieved.
*/
void test_get_workflow_01(void)
{
    mcl_error_t code;
    mcl_core_t *core = MCL_NULL;
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=GET_INSTANCE;HTTP_STATUS=200;%";
    mcl_core_configuration_t *core_configuration = _initialize_configuration(user_agent);

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(core_configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize deployment component configuration.
    mcl_deployment_configuration_t *configuration = MCL_NULL;
    code = mcl_deployment_configuration_initialize(&configuration);

    // Set core parameter of deployment configuration.
    code = mcl_deployment_configuration_set_parameter(configuration, MCL_DEPLOYMENT_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_deployment_configuration_set_parameter() failed.");

    // Initialize deployment component.
    mcl_deployment_t *deployment = MCL_NULL;
    code = mcl_deployment_initialize(configuration, &deployment);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for deployment component.");

    // Get deployment description.
    mcl_deployment_workflow_t *workflow = MCL_NULL;
    code = mcl_deployment_get_workflow(deployment, "1a5f74ef-83a0-4229-a7ed-21a175159dc9", MCL_NULL, &workflow);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Failed to get workflow description.");

    char *id = MCL_NULL;
    char *device_id = MCL_NULL;
    char *created_at = MCL_NULL;
    mcl_json_t *data = MCL_NULL;
    
    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_ID, &id);
    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_DEVICE_ID, &device_id);
    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CREATED_AT, &created_at);
    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_DATA, &data);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("1a5f74ef-83a0-4229-a7ed-21a175159dc9", id, "Workflow id mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("86286969-df31-414e-bb0c-5be930d415fb", device_id, "Device id mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2020-11-17T06:49:18.926Z", created_at, "Time created at mismatch.");
    TEST_ASSERT_NOT_NULL_MESSAGE(data, "Data of workflow is null.");

    mcl_deployment_workflow_state_t *workflow_state = MCL_NULL;
    char *state;
    double *progress;
    char *entered;
    char *updated;
    char *message;
    mcl_json_t *details;

    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CURRENT_STATE, &workflow_state);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, &state);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_PROGRESS, &progress);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_ENTERED, &entered);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_UPDATED, &updated);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_MESSAGE, &message);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_DETAILS, &details);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("DOWNLOAD", state, "Current state mismatch.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0, *progress, "State progress mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2020-11-17T06:49:18.926Z", entered, "Entered time mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2020-11-17T06:49:18.926Z", updated, "Updated time mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("New firmware release to download", message, "Message mismatch.");

    // Clean up.
    mcl_deployment_workflow_destroy(&workflow);
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&core_configuration);
    mcl_deployment_destroy(&deployment);
    mcl_deployment_configuration_destroy(&configuration);
}

/**
* GIVEN : ID of a deployment workflow.
* WHEN  : mcl_deployment_get_workflow() is called with model and history flags set to true.
* THEN  : MCL_OK is returned and deployment workflow description is retrieved together with model and history.
*/
void test_get_workflow_02(void)
{
    mcl_error_t code;
    mcl_core_t *core = MCL_NULL;
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=GET_INSTANCE;HTTP_STATUS=200;%";
    mcl_core_configuration_t *core_configuration = _initialize_configuration(user_agent);

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(core_configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize deployment component configuration.
    mcl_deployment_configuration_t *configuration = MCL_NULL;
    code = mcl_deployment_configuration_initialize(&configuration);

    // Set core parameter of deployment configuration.
    code = mcl_deployment_configuration_set_parameter(configuration, MCL_DEPLOYMENT_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_deployment_configuration_set_parameter() failed.");

    // Initialize deployment component.
    mcl_deployment_t *deployment = MCL_NULL;
    code = mcl_deployment_initialize(configuration, &deployment);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for deployment component.");

    // Prepare filter parameters to filter the response.
    mcl_deployment_workflow_filter_t* filter = MCL_NULL;
    code = mcl_deployment_workflow_filter_initialize(&filter);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization of workflow filter failed.");

    mcl_bool_t true = MCL_TRUE;
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_MODEL, &true);
    mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_HISTORY, &true);

    // Get deployment description.
    mcl_deployment_workflow_t *workflow = MCL_NULL;
    code = mcl_deployment_get_workflow(deployment, "1a5f74ef-83a0-4229-a7ed-21a175159dc9", filter, &workflow);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Failed to get workflow description.");

    // Test history of workflow.
    mcl_list_t *history = MCL_NULL;
    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_HISTORY, &history);
    TEST_ASSERT_EQUAL_MESSAGE(1, history->count, "Number of entries in history is not what is expected.");

    mcl_list_node_t *node = MCL_NULL;
    mcl_list_next(history, &node);
    
    mcl_deployment_workflow_state_t *workflow_state = (mcl_deployment_workflow_state_t*) node->data;
    char *state;
    double *progress;
    char *entered;
    char *updated;
    char *message;
    mcl_json_t *details;

    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, &state);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_PROGRESS, &progress);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_ENTERED, &entered);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_UPDATED, &updated);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_MESSAGE, &message);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_DETAILS, &details);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("DOWNLOAD", state, "Current state mismatch.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0, *progress, "State progress mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2020-11-17T06:49:18.926Z", entered, "Entered time mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2020-11-17T06:49:18.926Z", updated, "Updated time mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("New firmware release to download", message, "Message mismatch.");

    // Clean up.
    mcl_deployment_workflow_destroy(&workflow);
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&core_configuration);
    mcl_deployment_destroy(&deployment);
    mcl_deployment_configuration_destroy(&configuration);
    mcl_deployment_workflow_filter_destroy(&filter);
}

/**
* GIVEN : ID of a deployment workflow and new state.
* WHEN  : mcl_deployment_update_workflow() is called.
* THEN  : MCL_OK is returned and updated deployment workflow description is retrieved.
*/
void test_update_workflow_01(void)
{
    mcl_error_t code;
    mcl_core_t *core = MCL_NULL;
    const char *user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=UPDATE_INSTANCE;HTTP_STATUS=200;%";
    mcl_core_configuration_t *core_configuration = _initialize_configuration(user_agent);

    // Initialize an MCL core handle with the given configuration.
    code = mcl_core_initialize(core_configuration, &core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Core initialization failed.");

    // Onboard.
    code = mcl_core_onboard(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");

    // Get access token.
    code = mcl_core_get_access_token(core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Get access token failed.");

    // Initialize deployment component configuration.
    mcl_deployment_configuration_t *configuration = MCL_NULL;
    code = mcl_deployment_configuration_initialize(&configuration);

    // Set core parameter of deployment configuration.
    code = mcl_deployment_configuration_set_parameter(configuration, MCL_DEPLOYMENT_CONFIGURATION_PARAMETER_CORE, core);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_deployment_configuration_set_parameter() failed.");

    // Initialize deployment component.
    mcl_deployment_t *deployment = MCL_NULL;
    code = mcl_deployment_initialize(configuration, &deployment);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for deployment component.");

    // Initialize new state for workflow.
    mcl_deployment_workflow_state_t *new_state = MCL_NULL;
    code = mcl_deployment_workflow_state_initialize(&new_state);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Set the parameters of new workflow state.
    char *new_state_state = "DOWNLOADED"; 
    char *new_state_message = "Completed download."; 
    double new_state_progress = 1.0;
    mcl_json_t *new_state_details = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &new_state_details);
    mcl_deployment_workflow_state_set_parameter(new_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, new_state_state);
    mcl_deployment_workflow_state_set_parameter(new_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_PROGRESS, &new_state_progress);
    mcl_deployment_workflow_state_set_parameter(new_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_MESSAGE, new_state_message);
    mcl_deployment_workflow_state_set_parameter(new_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_DETAILS, new_state_details);

    // Update deployment description.
    mcl_deployment_workflow_t *workflow = MCL_NULL;
    code = mcl_deployment_update_workflow(deployment, "1a5f74ef-83a0-4229-a7ed-21a175159dc9", new_state, MCL_NULL, &workflow);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Failed to get workflow description.");

    char *id = MCL_NULL;
    char *device_id = MCL_NULL;
    char *created_at = MCL_NULL;

    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_ID, &id);
    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_DEVICE_ID, &device_id);
    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CREATED_AT, &created_at);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("1a5f74ef-83a0-4229-a7ed-21a175159dc9", id, "Workflow id mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("86286969-df31-414e-bb0c-5be930d415fb", device_id, "Device id mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2020-11-17T06:49:18.926Z", created_at, "Time created at mismatch.");

    mcl_deployment_workflow_state_t *workflow_state = MCL_NULL;
    char *state;
    double *progress;
    char *entered;
    char *updated;
    char *message;
    mcl_json_t *details;

    mcl_deployment_workflow_get_parameter(workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CURRENT_STATE, &workflow_state);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, &state);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_PROGRESS, &progress);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_ENTERED, &entered);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_UPDATED, &updated);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_MESSAGE, &message);
    mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_DETAILS, &details);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("DOWNLOADED", state, "Current state mismatch.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1.0, *progress, "State progress mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2020-11-17T06:49:18.926Z", entered, "Entered time mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("2020-11-17T06:49:18.926Z", updated, "Updated time mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Completed download.", message, "Message mismatch.");

    // Clean up.
    mcl_json_util_destroy(&new_state_details);
    mcl_deployment_workflow_state_destroy(&new_state);
    mcl_deployment_workflow_destroy(&workflow);
    mcl_core_destroy(&core);
    mcl_core_configuration_destroy(&core_configuration);
    mcl_deployment_destroy(&deployment);
    mcl_deployment_configuration_destroy(&configuration);
}

static mcl_core_configuration_t *_initialize_configuration(const char *user_agent)
{
    // Variable declaration.
    mcl_error_t code;
    mcl_uint16_t mindsphere_port = 1080;
    mcl_uint16_t proxy_port = 8888;
    const char *proxy_hostname = "http://127.0.0.1";
    const char *mindsphere_hostname = "https://localhost";
    const char *tenant = "br-smk1";
    const char *iat = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
    E_MCL_PROXY proxy_type = MCL_PROXY_HTTP;
    E_MCL_SECURITY_PROFILE security_profile = MCL_SECURITY_RSA_3072;

    // Initialize core configuration.
    mcl_core_configuration_t *core_configuration = MCL_NULL;
    mcl_core_configuration_initialize(&core_configuration);

    // Set mindsphere hostname parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_HOST, mindsphere_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere hostname parameter.");

    // Set mindsphere port parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_MDSP_PORT, &mindsphere_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere port parameter.");

    // Set mindsphere certificate parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_CERTIFICATE, mock_server_certificate);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for mindsphere certificate parameter.");

    // Set security profile parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_SECURITY_PROFILE, &security_profile);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for security profile parameter.");

    // Set proxy hostname parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_HOST, proxy_hostname);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy hostname parameter.");

    // Set proxy port parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_PORT, &proxy_port);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy port parameter.");

    // Set proxy type parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_PROXY_TYPE, &proxy_type);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for proxy type parameter.");

    // Set user agent parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_USER_AGENT, user_agent);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for user agent parameter.");

    // Set initial access token parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_IAT, iat);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for initial access token parameter.");

    // Set tenant parameter of core configuration.
    code = mcl_core_configuration_set_parameter(core_configuration, MCL_CORE_CONFIGURATION_PARAMETER_TENANT, tenant);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Core configuration set parameter failed for tenant parameter.");

    return core_configuration;
}
