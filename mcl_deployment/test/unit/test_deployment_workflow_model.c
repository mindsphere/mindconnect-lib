/*!
 * @file     test_deployment_workflow_model.c
 * @brief    This file implements all test cases of deployment_workflow_model module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_deployment/mcl_deployment_workflow_model.h"
#include "deployment_workflow_model.h"
#include "mcl_core/mcl_memory.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Non-null address for handle.
 * WHEN  : deployment_workflow_model_initialize() is called.
 * THEN  : MCL_OK is returned, memory is allocated and default values for parameters are set.
 */
void test_initialize_001(void)
{
    mcl_deployment_workflow_model_t *model = MCL_NULL;

    // Test.
    mcl_error_t code = deployment_workflow_model_initialize(&model);

    // Check results.
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_NOT_NULL(model);
    TEST_ASSERT_NULL(model->key);
    TEST_ASSERT_NULL(model->states);
    TEST_ASSERT_NULL(model->transitions);
    TEST_ASSERT_NULL(model->state_groups);

    // Clean up.
    deployment_workflow_model_destroy(&model);
}

/**
 * GIVEN : Initialized model.
 * WHEN  : mcl_deployment_workflow_model_get_parameter() is called.
 * THEN  : MCL_OK is returned and parameter has the correct value.
 */
void test_get_parameter_001(void)
{
    mcl_error_t code;
    mcl_deployment_workflow_model_t model;
    model.key = "mdsp-core-firmwareInstallation";
    model.states = MCL_NULL;
    model.state_groups = MCL_NULL;
    model.transitions = MCL_NULL;

    char *key_actual = MCL_NULL;
    mcl_list_t *states_actual = MCL_NULL;
    mcl_list_t *state_groups_actual = MCL_NULL;
    mcl_list_t *transitions_actual = MCL_NULL;

    // Test.
    code = mcl_deployment_workflow_model_get_parameter(&model, MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER_KEY, &key_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(model.key, key_actual);

    code = mcl_deployment_workflow_model_get_parameter(&model, MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER_STATES, &states_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL(model.states, states_actual);
    
    code = mcl_deployment_workflow_model_get_parameter(&model, MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER_STATE_GROUPS, &state_groups_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL(model.state_groups, state_groups_actual);    
    
    code = mcl_deployment_workflow_model_get_parameter(&model, MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER_TRANSITIONS, &transitions_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL(model.transitions, transitions_actual);
}

/**
 * GIVEN : Null or invalid input arguments.
 * WHEN  : mcl_deployment_workflow_model_get_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL or MCL_INVALID_PARAMETER is returned.
 */
void test_get_parameter_002(void)
{
    mcl_error_t code;
    mcl_deployment_workflow_model_t model;
    char *key = MCL_NULL;

    code = mcl_deployment_workflow_model_get_parameter(MCL_NULL, MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER_KEY, &key);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);

    code = mcl_deployment_workflow_model_get_parameter(&model, MCL_DEPLOYMENT_WORKFLOW_MODEL_PARAMETER_KEY, MCL_NULL);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);

    code = mcl_deployment_workflow_model_get_parameter(&model, 100, &key);
    TEST_ASSERT_EQUAL(MCL_INVALID_PARAMETER, code);
}
