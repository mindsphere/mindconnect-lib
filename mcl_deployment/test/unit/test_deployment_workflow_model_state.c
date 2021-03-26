/*!
 * @file     test_deployment_workflow_model_state.c
 * @brief    This file implements all test cases of deployment_workflow_model_state module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_deployment/mcl_deployment_workflow_model_state.h"
#include "deployment_workflow_model_state.h"
#include "mcl_core/mcl_memory.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Non-null address for handle.
 * WHEN  : deployment_workflow_model_state_initialize() is called.
 * THEN  : MCL_OK is returned, memory is allocated and default values for parameters are set.
 */
void test_initialize_001(void)
{
    mcl_deployment_workflow_model_state_t *state = MCL_NULL;

    // Test.
    mcl_error_t code = deployment_workflow_model_state_initialize(&state);

    // Check results.
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_NOT_NULL(state);
    TEST_ASSERT_NULL(state->name);
    TEST_ASSERT_NULL(state->description);
    TEST_ASSERT_EQUAL(MCL_FALSE, state->initial);
    TEST_ASSERT_EQUAL(MCL_FALSE, state->final);
    TEST_ASSERT_EQUAL(MCL_FALSE, state->cancel);

    // Clean up.
    deployment_workflow_model_state_destroy(&state);
}

/**
 * GIVEN : Initialized state.
 * WHEN  : mcl_deployment_workflow_model_state_get_parameter() is called.
 * THEN  : MCL_OK is returned and parameter has the correct value.
 */
void test_get_parameter_001(void)
{
    mcl_error_t code;
    mcl_deployment_workflow_model_state_t state;
    
    state.name = "DOWNLOAD";
    state.description = "Initial state";
    state.initial = MCL_TRUE;
    state.final = MCL_FALSE;
    state.cancel = MCL_FALSE;

    char *name_actual = MCL_NULL;
    char *description_actual = MCL_NULL;
    mcl_bool_t *initial_actual = MCL_NULL;
    mcl_bool_t *final_actual = MCL_NULL;
    mcl_bool_t *cancel_actual = MCL_NULL;
    
    // Test.
    code = mcl_deployment_workflow_model_state_get_parameter(&state, MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_NAME, &name_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(state.name, name_actual);
    
    code = mcl_deployment_workflow_model_state_get_parameter(&state, MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_DESCRIPTION, &description_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(state.description, description_actual);
    
    code = mcl_deployment_workflow_model_state_get_parameter(&state, MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_INITIAL, &initial_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL(state.initial, *initial_actual);
 
    code = mcl_deployment_workflow_model_state_get_parameter(&state, MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_FINAL, &final_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL(state.final, *final_actual);

    code = mcl_deployment_workflow_model_state_get_parameter(&state, MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_CANCEL, &cancel_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL(state.cancel, *cancel_actual);
}

/**
 * GIVEN : Null or invalid input arguments.
 * WHEN  : mcl_deployment_workflow_model_state_get_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL or MCL_INVALID_PARAMETER is returned.
 */
void test_get_parameter_002(void)
{
    mcl_error_t code;
    mcl_deployment_workflow_model_state_t state;
    char *name = MCL_NULL;

    code = mcl_deployment_workflow_model_state_get_parameter(MCL_NULL, MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_NAME, &name);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);

    code = mcl_deployment_workflow_model_state_get_parameter(&state, MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_PARAMETER_NAME, MCL_NULL);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);

    code = mcl_deployment_workflow_model_state_get_parameter(&state, 100, &name);
    TEST_ASSERT_EQUAL(MCL_INVALID_PARAMETER, code);
}
