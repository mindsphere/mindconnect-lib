/*!
 * @file     test_deployment_workflow.c
 * @brief    This file implements all test cases of deployment_workflow module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_deployment/mcl_deployment_workflow.h"
#include "deployment_workflow.h"
#include "mcl_deployment_workflow_state.h"
#include "deployment_workflow_state.h"
#include "mcl_deployment_workflow_model.h"
#include "deployment_workflow_model.h"
#include "mcl_core/mcl_memory.h"
#include "mcl_core/mcl_list.h"
#include "mcl_core/mcl_json_util.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Non-null address for handle.
 * WHEN  : deployment_workflow_initialize() is called.
 * THEN  : MCL_OK is returned, memory is allocated and default values for parameters are set.
 */
void test_initialize_001(void)
{
    mcl_deployment_workflow_t *workflow = MCL_NULL;

    // Test.
    mcl_error_t code = deployment_workflow_initialize(&workflow);

    // Check results.
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_NOT_NULL(workflow);
    TEST_ASSERT_NULL(workflow->id);
    TEST_ASSERT_NULL(workflow->device_id);
    TEST_ASSERT_NULL(workflow->created_at);
    TEST_ASSERT_NULL(workflow->current_state);
    TEST_ASSERT_NULL(workflow->history);
    TEST_ASSERT_NULL(workflow->model);
    TEST_ASSERT_NULL(workflow->data);

    // Clean up.
    mcl_deployment_workflow_destroy(&workflow);
}

/**
 * GIVEN : Initialized workflow.
 * WHEN  : mcl_deployment_workflow_get_parameter() is called.
 * THEN  : MCL_OK is returned and parameter has the correct value.
 */
void test_get_parameter_001(void)
{
    mcl_error_t code;
    mcl_deployment_workflow_t workflow;
    mcl_deployment_workflow_model_t model;
    mcl_deployment_workflow_state_t current_state;
    mcl_list_t *history = MCL_NULL;
    mcl_json_t *data = MCL_NULL;

    mcl_list_initialize(&history);
    mcl_json_util_initialize(MCL_JSON_OBJECT, &data);

    workflow.id = "1a5f74ef-83a0-4229-a7ed-21a175159dc9";
    workflow.device_id = "86286969-df31-414e-bb0c-5be930d415fb";
    workflow.created_at = "2020-11-17T06:49:18.926Z";
    workflow.current_state = &current_state;
    workflow.history = history;
    workflow.model = &model;
    workflow.data = data;

    char *id_actual = MCL_NULL;
    char *device_id_actual = MCL_NULL;
    char *created_at_actual = MCL_NULL;
    char *current_state_actual = MCL_NULL;
    mcl_list_t *history_actual = MCL_NULL;
    mcl_deployment_workflow_model_t *model_actual = MCL_NULL;
    mcl_json_t *data_actual = MCL_NULL;

    // Test.
    code = mcl_deployment_workflow_get_parameter(&workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_ID, &id_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(workflow.id, id_actual);
    
    code = mcl_deployment_workflow_get_parameter(&workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_DEVICE_ID, &device_id_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(workflow.device_id, device_id_actual);
    
    code = mcl_deployment_workflow_get_parameter(&workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CREATED_AT, &created_at_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(workflow.created_at, created_at_actual);
    
    code = mcl_deployment_workflow_get_parameter(&workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_CURRENT_STATE, &current_state_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_PTR(workflow.current_state, current_state_actual);
    
    code = mcl_deployment_workflow_get_parameter(&workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_HISTORY, &history_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_PTR(workflow.history, history_actual);
    
    code = mcl_deployment_workflow_get_parameter(&workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_MODEL, &model_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_PTR(workflow.model, model_actual);
    
    code = mcl_deployment_workflow_get_parameter(&workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_DATA, &data_actual);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_PTR(workflow.data, data_actual);

    mcl_list_destroy(&history);
    mcl_json_util_destroy(&data);
}

/**
 * GIVEN : Null or invalid input arguments.
 * WHEN  : mcl_deployment_workflow_get_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL or MCL_INVALID_PARAMETER is returned.
 */
void test_get_parameter_002(void)
{
    mcl_error_t code;
    mcl_deployment_workflow_t workflow;
    char *id = MCL_NULL;

    code = mcl_deployment_workflow_get_parameter(MCL_NULL, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_ID, &id);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);

    code = mcl_deployment_workflow_get_parameter(&workflow, MCL_DEPLOYMENT_WORKFLOW_PARAMETER_ID, MCL_NULL);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);

    code = mcl_deployment_workflow_get_parameter(&workflow, 100, &id);
    TEST_ASSERT_EQUAL(MCL_INVALID_PARAMETER, code);
}
