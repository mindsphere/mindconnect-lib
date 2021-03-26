/*!
 * @file     deployment_json.h
 * @brief    Header for json operations of MCL deployment component.
 *
 * This module provides the functions for json operations of MCL deployment component.
 *
 * @copyright Copyright (C) 2019 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 */

#ifndef DEPLOYMENT_JSON_H_
#define DEPLOYMENT_JSON_H_

#include "mcl_deployment/mcl_deployment_common.h"
#include "mcl_core/mcl_list.h"
#include "deployment_workflow.h"

/**
 * This function parses a given json string into a list of deployment workflow objects.
 *
 * @param [in] json_string Json string.
 * @param [in] string_length Length of json string.
 * @param [out] workflows List of deployment workflows.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_JSON_NON_EXISTING_CHILD in case there is a missing mandatory field.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t deployment_json_parse_workflows(const char *json_string, mcl_size_t string_length, mcl_list_t **workflows);

/**
 * This function parses a given json string into a deployment workflow object.
 *
 * @param [in] json_string Json string.
 * @param [in] string_length Length of json string.
 * @param [out] workflow Deployment workflow.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_JSON_NON_EXISTING_CHILD in case there is a missing mandatory field.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t deployment_json_parse_workflow(const char* json_string, mcl_size_t string_length, deployment_workflow_t **workflow);

/**
 * This function converts a deployment workflow state to a json string.
 *
 * @param [in] workflow_state Workflow state.
 * @param [out] json_string Json string.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_JSON_NON_EXISTING_CHILD in case there is a missing mandatory field.</li>
 * </ul>
 */
MCL_LOCAL mcl_error_t deployment_json_from_workflow_state(deployment_workflow_state_t *workflow_state, char **json_string);

#endif //DEPLOYMENT_JSON_H_
