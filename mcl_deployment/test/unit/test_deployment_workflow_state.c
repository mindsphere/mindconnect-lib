/*!
 * @file     test_deployment_workflow_state.c
 * @brief    This file implements all test cases of deployment_workflow_state module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_deployment/mcl_deployment_workflow_state.h"
#include "deployment_workflow_state.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_string_util.h"
#include "mcl_core/mcl_json_util.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Null address for handle.
 * WHEN  : mcl_deployment_workflow_state_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_001(void)
{
    // Test.
    mcl_error_t code = mcl_deployment_workflow_state_initialize(MCL_NULL);

    // Check results.
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);
}

/**
 * GIVEN : Non-null address for handle.
 * WHEN  : mcl_deployment_workflow_state_initialize() is called.
 * THEN  : MCL_OK is returned, memory is allocated and default values for parameters are set.
 */
void test_initialize_002(void)
{
    mcl_deployment_workflow_state_t *state = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_deployment_workflow_state_initialize(&state);

    // Check results.
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_NOT_NULL(state);
    TEST_ASSERT_NULL(state->state);
    TEST_ASSERT_EQUAL(0.0, state->progress);
    TEST_ASSERT_NULL(state->entered);
    TEST_ASSERT_NULL(state->updated);
    TEST_ASSERT_NULL(state->message);
    TEST_ASSERT_NULL(state->details);

    // Clean up.
    mcl_deployment_workflow_state_destroy(&state);
}

/**
 * GIVEN : Null or invalid input arguments.
 * WHEN  : mcl_deployment_workflow_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL or MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_001(void)
{
    mcl_deployment_workflow_state_t *workflow_state = MCL_NULL;
    mcl_error_t code = mcl_deployment_workflow_state_initialize(&workflow_state);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    char *state = "DOWNLOAD";

    code = mcl_deployment_workflow_state_set_parameter(MCL_NULL, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, state);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);

    code = mcl_deployment_workflow_state_set_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, MCL_NULL);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);
    
    code = mcl_deployment_workflow_state_set_parameter(workflow_state, 100, state);
    TEST_ASSERT_EQUAL(MCL_INVALID_PARAMETER, code);

    mcl_deployment_workflow_state_destroy(&workflow_state);
}

/**
 * GIVEN : Null or invalid input arguments.
 * WHEN  : mcl_deployment_workflow_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL or MCL_INVALID_PARAMETER is returned.
 */
void test_get_parameter_001(void)
{
    mcl_deployment_workflow_state_t *workflow_state = MCL_NULL;
    mcl_error_t code = mcl_deployment_workflow_state_initialize(&workflow_state);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    char *state = MCL_NULL;

    code = mcl_deployment_workflow_state_get_parameter(MCL_NULL, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, &state);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);

    code = mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, MCL_NULL);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);

    code = mcl_deployment_workflow_state_get_parameter(workflow_state, 100, &state);
    TEST_ASSERT_EQUAL(MCL_INVALID_PARAMETER, code);

    mcl_deployment_workflow_state_destroy(&workflow_state);
}

/**
 * GIVEN : Initialized workflow state.
 * WHEN  : mcl_deployment_workflow_set_parameter() is called.
 * THEN  : MCL_OK is returned and parameter is set.
 */
void test_set_get_parameter_001(void)
{
    mcl_deployment_workflow_state_t *workflow_state = MCL_NULL;
    mcl_error_t code = mcl_deployment_workflow_state_initialize(&workflow_state);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    char *state = "DOWNLOADED";
    double progress = 0.5;
    char *entered = "2020-11-17T06:49:18.926Z";
    char *updated = "2020-11-17T06:49:18.926Z";
    char *message = "Some Message";
    mcl_json_t *details = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &details);
    mcl_json_util_add_string(details, "customField", "customValue");
    
    char *state_actual = MCL_NULL;
    double *progress_actual = MCL_NULL;
    char *entered_actual = MCL_NULL;
    char *updated_actual = MCL_NULL;
    char *message_actual = MCL_NULL;
    mcl_json_t *details_actual = MCL_NULL;

    code = mcl_deployment_workflow_state_set_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, state);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    code = mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, &state_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(state, state_actual);

    code = mcl_deployment_workflow_state_set_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_PROGRESS, &progress);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    code = mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_PROGRESS, &progress_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_DOUBLE(progress, *progress_actual);

    code = mcl_string_util_reset(entered, &workflow_state->entered);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    code = mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_ENTERED, &entered_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(entered, entered_actual);

    code = mcl_string_util_reset(updated, &workflow_state->updated);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    code = mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_UPDATED, &updated_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(updated, updated_actual);

    code = mcl_deployment_workflow_state_set_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_MESSAGE, message);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    code = mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_MESSAGE, &message_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(message, message_actual);

    code = mcl_deployment_workflow_state_set_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_DETAILS, details);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    code = mcl_deployment_workflow_state_get_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_DETAILS, &details_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    
    mcl_json_t *custom_field = MCL_NULL;
    char *custom_value = MCL_NULL;
    mcl_json_util_get_object_item(details_actual, "customField", &custom_field);
    mcl_json_util_get_string(custom_field, &custom_value);
    TEST_ASSERT_EQUAL_STRING("customValue", custom_value);

    // Clean up.
    MCL_FREE(custom_value);
    mcl_json_util_destroy(&details);
    mcl_deployment_workflow_state_destroy(&workflow_state);
}

/**
 * GIVEN : Initialized workflow state with parameters set.
 * WHEN  : deployment_workflow_validate() is called.
 * THEN  : MCL_OK is returned.
 */
void test_validate_001(void)
{
    mcl_deployment_workflow_state_t *workflow_state = MCL_NULL;
    mcl_error_t code = mcl_deployment_workflow_state_initialize(&workflow_state);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    char *state = "DOWNLOADED";
    char *message = "Some Message";

    code = deployment_workflow_state_validate(workflow_state);
    TEST_ASSERT_EQUAL(MCL_INVALID_PARAMETER, code);

    code = mcl_deployment_workflow_state_set_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_STATE, state);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = deployment_workflow_state_validate(workflow_state);
    TEST_ASSERT_EQUAL(MCL_INVALID_PARAMETER, code);

    code = mcl_deployment_workflow_state_set_parameter(workflow_state, MCL_DEPLOYMENT_WORKFLOW_STATE_PARAMETER_MESSAGE, message);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = deployment_workflow_state_validate(workflow_state);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    // Clean up.
    mcl_deployment_workflow_state_destroy(&workflow_state);
}
