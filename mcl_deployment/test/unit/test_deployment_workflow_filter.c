/*!
 * @file     test_deployment_workflow_filter.c
 * @brief    This file implements all test cases of deployment_workflow_filter module.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#include "unity.h"
#include "mcl_deployment/mcl_deployment_workflow_filter.h"
#include "deployment_workflow_filter.h"
#include "mcl_core/mcl_memory.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Null address for handle.
 * WHEN  : mcl_deployment_workflow_filter_initialize() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_initialize_001(void)
{
    // Test.
    mcl_error_t code = mcl_deployment_workflow_filter_initialize(MCL_NULL);

    // Check results.
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);
}

/**
 * GIVEN : Non-null address for handle.
 * WHEN  : mcl_deployment_workflow_filter_initialize() is called.
 * THEN  : MCL_OK is returned, memory is allocated and default values for parameters are set.
 */
void test_initialize_002(void)
{
    mcl_deployment_workflow_filter_t *filter = MCL_NULL;

    // Test.
    mcl_error_t code = mcl_deployment_workflow_filter_initialize(&filter);

    // Check results.
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_NOT_NULL(filter);
    TEST_ASSERT_EQUAL(MCL_FALSE, filter->model);
    TEST_ASSERT_EQUAL(MCL_FALSE, filter->history);
    TEST_ASSERT_NULL(filter->current_state);
    TEST_ASSERT_NULL(filter->group);
    TEST_ASSERT_NULL(filter->device_id);
    TEST_ASSERT_NULL(filter->model_key);

    // Clean up.
    mcl_deployment_workflow_filter_destroy(&filter);
}

/**
 * GIVEN : Initialized filter.
 * WHEN  : mcl_deployment_workflow_filter_set_parameter() is called.
 * THEN  : MCL_OK is returned and parameter is set.
 */
void test_set_parameter_001(void)
{
    mcl_error_t code;
    mcl_deployment_workflow_filter_t *filter = MCL_NULL;
    
    mcl_bool_t model = MCL_TRUE;
    mcl_bool_t history = MCL_TRUE;
    char *current_state = "DOWNLOAD";
    char *group = "DeviceTriggered";
    char *device_id = "1234567890";
    char *model_key = "mdsp-core-firmwareInstallation";

    code = mcl_deployment_workflow_filter_initialize(&filter);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_MODEL, &model);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL(model, filter->model);
    
    code = mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_HISTORY, &history);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL(history, filter->history);

    code = mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_CURRENT_STATE, current_state);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(current_state, filter->current_state);
    
    code = mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_GROUP, group);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(group, filter->group);
    
    code = mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_DEVICE_ID, device_id);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(device_id, filter->device_id);
    
    code = mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_MODEL_KEY, model_key);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL_STRING(model_key, filter->model_key);

    mcl_deployment_workflow_filter_destroy(&filter);
}

/**
 * GIVEN : Null or invalid input arguments.
 * WHEN  : mcl_deployment_workflow_filter_set_parameter() is called.
 * THEN  : MCL_TRIGGERED_WITH_NULL or MCL_INVALID_PARAMETER is returned.
 */
void test_set_parameter_002(void)
{
    mcl_error_t code;
    mcl_deployment_workflow_filter_t *filter = MCL_NULL;
    char *current_state = "DOWNLOAD";

    code = mcl_deployment_workflow_filter_initialize(&filter);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    code = mcl_deployment_workflow_filter_set_parameter(MCL_NULL, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_CURRENT_STATE, current_state);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);

    code = mcl_deployment_workflow_filter_set_parameter(filter, MCL_DEPLOYMENT_WORKFLOW_FILTER_PARAMETER_CURRENT_STATE, MCL_NULL);
    TEST_ASSERT_EQUAL(MCL_TRIGGERED_WITH_NULL, code);

    code = mcl_deployment_workflow_filter_set_parameter(filter, 100, current_state);
    TEST_ASSERT_EQUAL(MCL_INVALID_PARAMETER, code);

    mcl_deployment_workflow_filter_destroy(&filter);
}
