/*!
 * @file     test_deployment_workflow_model_state_group.c
 * @brief    This file implements all test cases of deployment_workflow_model_state_group module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_deployment/mcl_deployment_workflow_model_state_group.h"
#include "deployment_workflow_model_state_group.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_list.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Non-null address for handle.
 * WHEN  : deployment_workflow_model_state_group_initialize() is called.
 * THEN  : MCL_OK is returned, memory is allocated and default values for parameters are set.
 */
void test_initialize_001(void)
{
    mcl_deployment_workflow_model_state_group_t *state_group = MCL_NULL;

    // Test.
    mcl_error_t code = deployment_workflow_model_state_group_initialize(&state_group);

    // Check results.
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_NOT_NULL(state_group);
    TEST_ASSERT_NULL(state_group->name);
    TEST_ASSERT_NULL(state_group->states);

    // Clean up.
    deployment_workflow_model_state_group_destroy(&state_group);
}

/**
 * GIVEN : Initialized state group.
 * WHEN  : mcl_deployment_workflow_model_state_group_get_parameter() is called.
 * THEN  : MCL_OK is returned and parameter has the correct value.
 */
void test_get_parameter_001(void)
{
    mcl_error_t code;
    mcl_deployment_workflow_model_state_group_t state_group;
    state_group.name = "DeviceTriggered";
    state_group.states = MCL_NULL;

    char *name_actual = MCL_NULL;
    mcl_list_t *states_actual = MCL_NULL;

    // Test.
    code = mcl_deployment_workflow_model_state_group_get_parameter(&state_group, MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_PARAMETER_NAME, &name_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(state_group.name, name_actual);

    code = mcl_deployment_workflow_model_state_group_get_parameter(&state_group, MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_PARAMETER_STATES, &states_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL(state_group.states, states_actual);
}

/**
 * GIVEN : Null or invalid input arguments.
 * WHEN  : mcl_deployment_workflow_model_state_group_get_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL or MCL_INVALID_PARAMETER is returned.
 */
void test_get_parameter_002(void)
{
    mcl_error_t code;
    mcl_deployment_workflow_model_state_group_t state_group;
    char *name = MCL_NULL;

    code = mcl_deployment_workflow_model_state_group_get_parameter(MCL_NULL, MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_PARAMETER_NAME, &name);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);

    code = mcl_deployment_workflow_model_state_group_get_parameter(&state_group, MCL_DEPLOYMENT_WORKFLOW_MODEL_STATE_GROUP_PARAMETER_NAME, MCL_NULL);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);

    code = mcl_deployment_workflow_model_state_group_get_parameter(&state_group, 100, &name);
    TEST_ASSERT_EQUAL(MCL_INVALID_PARAMETER, code);
}
