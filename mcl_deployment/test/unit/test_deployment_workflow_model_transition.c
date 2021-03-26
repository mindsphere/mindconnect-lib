/*!
 * @file     test_deployment_workflow_model_transition.c
 * @brief    This file implements all test cases of deployment_workflow_model_transition module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_deployment/mcl_deployment_workflow_model_transition.h"
#include "deployment_workflow_model_transition.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_json_util.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Non-null address for handle.
 * WHEN  : deployment_workflow_model_transition_initialize() is called.
 * THEN  : MCL_OK is returned, memory is allocated and default values for parameters are set.
 */
void test_initialize_001(void)
{
    mcl_deployment_workflow_model_transition_t *transition = MCL_NULL;

    // Test.
    mcl_error_t code = deployment_workflow_model_transition_initialize(&transition);

    // Check results.
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_NOT_NULL(transition);
    TEST_ASSERT_NULL(transition->from);
    TEST_ASSERT_NULL(transition->to);
    TEST_ASSERT_NULL(transition->type);
    TEST_ASSERT_NULL(transition->details);

    // Clean up.
    deployment_workflow_model_transition_destroy(&transition);
}

/**
 * GIVEN : Initialized transition.
 * WHEN  : mcl_deployment_workflow_model_transition_get_parameter() is called.
 * THEN  : MCL_OK is returned and parameter has the correct value.
 */
void test_get_parameter_001(void)
{
    mcl_error_t code;
    
    mcl_deployment_workflow_model_transition_t transition;
    transition.from = "DOWNLOADING";
    transition.to = "DOWNLOADED";
    transition.type = "DEVICE-TRIGGER";
    transition.details = MCL_NULL;

    char *from_actual = MCL_NULL;
    char *to_actual = MCL_NULL;
    char *type_actual = MCL_NULL;
    mcl_json_t *details_actual = MCL_NULL;

    // Test.
    code = mcl_deployment_workflow_model_transition_get_parameter(&transition, MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER_FROM, &from_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(transition.from, from_actual);

    code = mcl_deployment_workflow_model_transition_get_parameter(&transition, MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER_TO, &to_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(transition.to, to_actual);

    code = mcl_deployment_workflow_model_transition_get_parameter(&transition, MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER_TYPE, &type_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(transition.type, type_actual);

    code = mcl_deployment_workflow_model_transition_get_parameter(&transition, MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER_DETAILS, &details_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL(transition.details, details_actual);
}

/**
 * GIVEN : Null or invalid input arguments.
 * WHEN  : mcl_deployment_workflow_model_transition_get_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL or MCL_INVALID_PARAMETER is returned.
 */
void test_get_parameter_002(void)
{
    mcl_error_t code;
    mcl_deployment_workflow_model_transition_t transition;
    char *from = MCL_NULL;

    code = mcl_deployment_workflow_model_transition_get_parameter(MCL_NULL, MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER_FROM, &from);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);

    code = mcl_deployment_workflow_model_transition_get_parameter(&transition, MCL_DEPLOYMENT_WORKFLOW_MODEL_TRANSITION_PARAMETER_FROM, MCL_NULL);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);

    code = mcl_deployment_workflow_model_transition_get_parameter(&transition, 100, &from);
    TEST_ASSERT_EQUAL(MCL_INVALID_PARAMETER, code);
}
